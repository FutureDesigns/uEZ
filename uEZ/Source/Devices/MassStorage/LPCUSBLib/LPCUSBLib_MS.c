/*-------------------------------------------------------------------------*
 * File:  USBMS_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the USB MassStorage Device.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZErrors.h>
#include <uEZProcessor.h>
#include <uEZRTOS.h>
#include "LPCUSBLib_MS.h"
#include <Device/USBHost.h>
#include <LPCUSBLib/Drivers/USB/USB.h> // LPCUSBLib
#include <Source/Library/LPCOpen/LPCUSBLib/LPCUSBLib_uEZ.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define IGrab()         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()      UEZSemaphoreRelease(p->iSem)

static const USB_ClassInfo_MS_Host_t FlashDisk_MS_Interface = {
    .Config = {
        .DataINPipeNumber       = 1,
        .DataINPipeDoubleBank   = false,

        .DataOUTPipeNumber      = 2,
        .DataOUTPipeDoubleBank  = false,
        .PortNumber = 0,
    },
};

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_MassStorage *iDevice;
    volatile TBool iInitPerformed;
    T_uezSemaphore iSem;
    T_uezSemaphore iDone;
    TUInt32 iUnitAddress;
    volatile TBool iIsConnected;
    TBool iSWWriteProtect;

    TUInt32 iBlockSize;
    //TUInt8 *blockBuf;

    SCSI_Capacity_t iDiskCapacity;
    USB_ClassInfo_MS_Host_t *hDisk; // Disk configuration and state

    T_MassStorage_LPCUSBLib_Callbacks iCallbacks;
    void *iCallbackWorkspace;
} T_LPCUSBLib_MS_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static USB_ClassInfo_MS_Host_t MS_Disks[2]; //!< USB configuration for drives
static T_LPCUSBLib_MS_Workspace *MS_Workspaces[2];
/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void IEVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum);
static int32_t IUpdate(int32_t aUnitAddress);

/*---------------------------------------------------------------------------*
 * Internal Routine:  HostErrorToUEZ
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert a LPCUSBLib Host error code into a uEZ error code
 * Inputs:
 *      uint8_t errorCode            -- Error code to convert
 * Outputs:
 *      T_uezError                   -- uEZ equivalent error code
 *---------------------------------------------------------------------------*/
static T_uezError HostErrorToUEZ(uint8_t errorCode)
{
   switch (errorCode) {
      case HOST_SENDCONTROL_Successful:
         return UEZ_ERROR_NONE;
      case HOST_SENDCONTROL_DeviceDisconnected:
         return UEZ_ERROR_CLOSED;
      case HOST_SENDCONTROL_PipeError:
         return UEZ_ERROR_FAIL;
      case HOST_SENDCONTROL_SetupStalled:
         return UEZ_ERROR_STALL;
      case HOST_SENDCONTROL_SoftwareTimeOut:
         return UEZ_ERROR_TIMEOUT;
   }
   return UEZ_ERROR_UNKNOWN;
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  ILPCUSBLibHostTask
 *---------------------------------------------------------------------------*
 * Description:
 *      Task that monitors the USB drive and configures the connected/
 *      unconnected state while running the LPCLibUsb task thread.
 * Inputs:
 *      T_uezTask aTask              -- This task
 *      void *aParameters            -- Parameters (workspace)
 * Outputs:
 *      TUInt32                      -- return code (never returns)
 *---------------------------------------------------------------------------*/
static int32_t IUpdate(int32_t aUnitAddress)
{
    int32_t activity = 0;
    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)MS_Workspaces[aUnitAddress];
    TUInt32 core = p->iUnitAddress;

    if ((USB_HostState[core] == HOST_STATE_Unattached) && (p->iIsConnected == EFalse)) {
        // Slow delay here
        activity = 0;
    } else {
        if ((USB_HostState[core] == HOST_STATE_Configured) && (p->iIsConnected == EFalse)) {
            //printf("MS%d Connected\r\n", core);
            p->iIsConnected = ETrue;
            if (p->iCallbacks.iDriveConnected)
                p->iCallbacks.iDriveConnected(p->iCallbackWorkspace);

            activity = 1;
        } else if ((USB_HostState[core] == HOST_STATE_Unattached) && (p->iIsConnected == ETrue)) {
            //printf("MS%d Disconnected\r\n", core);
            p->iIsConnected = EFalse;
            // Reset the structure
            memcpy(p->hDisk, &FlashDisk_MS_Interface, sizeof(*p->hDisk));
            p->hDisk->Config.PortNumber = p->iUnitAddress;
            if (p->iCallbacks.iDriveDisconnected)
                p->iCallbacks.iDriveDisconnected(p->iCallbackWorkspace);

            activity = 1;
        }
    }

    return activity;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a MassStorage device on the USB Host.
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error;
    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;

    p->iInitPerformed = EFalse;
    p->iSWWriteProtect = EFalse;

    // Create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;
    error = UEZSemaphoreCreateBinary(&p->iDone);
    if (error)
        return error;

    p->iIsConnected = EFalse;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Prepare the LPCUSBLib driver
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      TUInt32                      -- 0=MS0, 1=MS1 being configured
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError MassStorage_LPCUSBLib_Configure(
        void *aWorkspace,
        TUInt32 aUnitAddress,
        T_MassStorage_LPCUSBLib_Callbacks *aCallbacks,
        void *aCallbackWorkspace)
{
    extern void Chip_USB0_Init(void);
    extern void Chip_USB1_Init(void);
    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    MS_Workspaces[aUnitAddress] = p;
    p->iUnitAddress = aUnitAddress;
    p->hDisk = &MS_Disks[p->iUnitAddress];
    memcpy(p->hDisk, &FlashDisk_MS_Interface, sizeof(*p->hDisk));
    p->hDisk->Config.PortNumber = aUnitAddress;
    p->iCallbacks = *aCallbacks;
    p->iCallbackWorkspace = aCallbackWorkspace;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      File system needs to start a new session.  Initialize it here.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      TUInt32 aAddress             -- File system unit number (not used)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_Init(void *aWorkspace, TUInt32 aAddress)
{
    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;
    USB_ClassInfo_MS_Host_t *hDisk = p->hDisk;

    // Cannot initialize an unconnected device
    if (!p->iIsConnected)
        return UEZ_ERROR_NOT_AVAILABLE;

    // Get the disk capacity
    //printf("MS%d Waiting for ready...", p->iUnitAddress);
    for (;; ) {
        uint8_t ErrorCode = MS_Host_TestUnitReady(hDisk, 0);

        if (!(ErrorCode)) {
            break;
        }

        /* Check if an error other than a logical command error (device busy) received */
        if (ErrorCode != MS_ERROR_LOGICAL_CMD_FAILED) {
            //printf("Failed\r\n");
            USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
            return UEZ_ERROR_NOT_AVAILABLE;
        }
    }
    //printf("Done.\r\n");

    if (MS_Host_ReadDeviceCapacity(hDisk, 0, &p->iDiskCapacity)) {
        //printf("Error retrieving device capacity.\r\n");
        USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
        return UEZ_ERROR_NOT_AVAILABLE;
    }

    //printf(("%lu blocks of %lu bytes.\r\n"), p->iDiskCapacity.Blocks, p->iDiskCapacity.BlockSize);

    p->iInitPerformed = ETrue;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_Status
 *---------------------------------------------------------------------------*
 * Description:
 *      Return general status.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      T_msStatus *aStatus          -- Returned status
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_Status(void *aWorkspace, T_msStatus *aStatus)
{
    T_msStatus status;
    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;

    IGrab();

    status = 0;

    if (!p->iIsConnected)
        status |= MASS_STORAGE_STATUS_NO_MEDIUM;

    // Note if we need to init
    if (!p->iInitPerformed)
        status |= MASS_STORAGE_STATUS_NEED_INIT;

    // Cannot detect HW write protected currently.

    // But can detect software write protect
    if (p->iSWWriteProtect)
        status |= MASS_STORAGE_STATUS_WRITE_PROTECTED;

    // Return status
    *aStatus = status;

    IRelease();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read a group of sequential blocks
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      TUInt32 aStart               -- Location of first block to read
 *      TUInt32 aNumBlocks           -- Number of blocks to read
 *      void *aBuffer                -- Where to place blocks
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_Read(
            void *aWorkspace,
            const TUInt32 aStart,
            const TUInt32 aNumBlocks,
            void *aBuffer)
{
    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;
    USB_ClassInfo_MS_Host_t *hDisk = p->hDisk;
    uint8_t errorCode;

    IGrab();

    errorCode = MS_Host_ReadDeviceBlocks(hDisk, 0, aStart, aNumBlocks, p->iDiskCapacity.BlockSize, aBuffer);

    if (errorCode) {
        //printf("Error reading device block.\r\n");
        USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
        IRelease();
        return HostErrorToUEZ(errorCode);
    }

    IRelease();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write a group of sequential blocks
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      TUInt32 aStart               -- Location of first block to write
 *      TUInt32 aNumBlocks           -- Number of blocks to write
 *      void *aBuffer                -- Buffer holding data to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_Write(
            void *aWorkspace,
            const TUInt32 aStart,
            const TUInt32 aNumBlocks,
            const void *aBuffer)
{
    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;
    USB_ClassInfo_MS_Host_t *hDisk = p->hDisk;
    uint8_t errorCode;

    IGrab();

    errorCode = MS_Host_WriteDeviceBlocks(hDisk, 0, aStart, aNumBlocks, p->iDiskCapacity.BlockSize, aBuffer);

    if (errorCode) {
        //printf("Error writing device block.\r\n");
        IRelease();
        return HostErrorToUEZ(errorCode);
    }

    IRelease();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_Sync
 *---------------------------------------------------------------------------*
 * Description:
 *      Flush any data waiting to write
 *      NOTE: This implementation does not buffer
 * Inputs:
 *      void *aWorkspace             -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_Sync(void *aWorkspace)
{
//    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;
//    USB_ClassInfo_MS_Host_t *hDisk = p->hDisk;

    // Just always report done, we are not caching anywhere
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_GetSizeInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the drive capacity information
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      T_msSizeInfo *aInfo          -- Place to store information
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_GetSizeInfo(
            void *aWorkspace,
            T_msSizeInfo *aInfo)
{
    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;

    IGrab();

    aInfo->iNumSectors = p->iDiskCapacity.Blocks;
    aInfo->iBlockSize = p->iDiskCapacity.BlockSize;
    aInfo->iSectorSize = 512; // always

    IRelease();

    // Determine block size
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_SetPower
 *---------------------------------------------------------------------------*
 * Description:
 *      Set power on/off (not supported)
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      TBool aON                    -- ETrue = on, else false
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_SetPower(void *aWorkspace, TBool aOn)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_SetLock
 *---------------------------------------------------------------------------*
 * Description:
 *      Lock or unlock unit (not supported).  Lock to keep from removing.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      TBool aLock                  -- ETrue = locked, else false
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_SetLock(void *aWorkspace, TBool aLock)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_SetSoftwareWriteProtect
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the driver's write protect.  Does nothing other than keep from
 *      writing to the unit.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      TBool aSWWriteProtect        -- ETrue = protected, else false
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_SetSoftwareWriteProtect(
        void *aWorkspace,
        TBool aSWWriteProtect)
{
    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;
    IGrab();
    p->iSWWriteProtect = aSWWriteProtect;
    IRelease();
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_Eject
 *---------------------------------------------------------------------------*
 * Description:
 *      Eject the unit (not supported)
 * Inputs:
 *      void *aWorkspace             -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_Eject(void *aWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_MiscControl
 *---------------------------------------------------------------------------*
 * Description:
 *      Unit specific controls (NOT SUPPORTED)
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      TUInt32 aControlCode         -- Control code/cmd to execute
 *      void *aBuffer                -- Data to use with command
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_LPCUSBLib_MiscControl(
            void *aWorkspace,
            TUInt32 aControlCode,
            void *aBuffer)
{
//    T_LPCUSBLib_MS_Workspace *p = (T_LPCUSBLib_MS_Workspace *)aWorkspace;
    return UEZ_ERROR_ILLEGAL_OPERATION;
}

static T_LPCUSBLib_Host_Callbacks G_HostCallbacks = {
        0, // HostError
        IEVENT_USB_Host_DeviceEnumerationComplete, // DeviceEnumerationComplete
        0, // DeviceEnumerationFailed
        0, // DeviceUnattached
        0, // DeviceAttached
        IUpdate, // Update
};
/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_LPCUSBLib_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create the mass storage device at the given hardware unit (0 or 1)
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      const char *aName            -- Name of this device (usually "MS0" or "MS1")
 *      TUInt32 aUnit                -- Which hardware to use (0=USB0, 1=USB1)
 *---------------------------------------------------------------------------*/
void MassStorage_LPCUSBLib_Create(
        const char *aName,
        TUInt32 aUnitAddress,
        T_MassStorage_LPCUSBLib_Callbacks *aCallbacks,
        void *aCallbackWorkspace)
{
    DEVICE_MassStorage **ms;

    UEZ_LPCUSBLib_Host_Require(aUnitAddress, &G_HostCallbacks, 0);

    // Register USB Mass Storage device
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&MassStorage_LPCUSBLib_Interface,
            0,
            (T_uezDeviceWorkspace **)&ms);
    MassStorage_LPCUSBLib_Configure((void *)ms, aUnitAddress, aCallbacks, aCallbackWorkspace);
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
static void IEVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
    uint16_t ConfigDescriptorSize;
    uint8_t  ConfigDescriptorData[512];
    USB_ClassInfo_MS_Host_t *hDisk = &MS_Disks[corenum];

    if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize, ConfigDescriptorData,
                                           sizeof(ConfigDescriptorData)) != HOST_GETCONFIG_Successful) {
        //printf("Error Retrieving Configuration Descriptor.\r\n");
        return;
    }

    hDisk->Config.PortNumber = corenum;
    if (MS_Host_ConfigurePipes(hDisk,
                               ConfigDescriptorSize, ConfigDescriptorData) != MS_ENUMERROR_NoError) {
        //printf("Attached Device Not a Valid Mass Storage Device.\r\n");
        return;
    }

    if (USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 1) != HOST_SENDCONTROL_Successful) {
        //printf("Error Setting Device Configuration.\r\n");
        return;
    }

    uint8_t MaxLUNIndex;
    if (MS_Host_GetMaxLUN(hDisk, &MaxLUNIndex)) {
        //printf("Error retrieving max LUN index.\r\n");
        USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
        return;
    }

    //printf(("Total LUNs: %d - Using first LUN in device.\r\n"), (MaxLUNIndex + 1));

    if (MS_Host_ResetMSInterface(hDisk)) {
        //printf("Error resetting Mass Storage interface.\r\n");
        USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
        return;
    }

    SCSI_Request_Sense_Response_t SenseData;
    if (MS_Host_RequestSense(hDisk, 0, &SenseData) != 0) {
        printf("Error retrieving device sense.\r\n");
        USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
        return;
    }

//     if (MS_Host_PreventAllowMediumRemoval(&FlashDisk_MS_Interface, 0, true)) {
//         printf("Error setting Prevent Device Removal bit.\r\n");
//         USB_Host_SetDeviceConfiguration(FlashDisk_MS_Interface.Config.PortNumber, 0);
//         return;
//     }

    SCSI_Inquiry_Response_t InquiryData;
    if (MS_Host_GetInquiryData(hDisk, 0, &InquiryData)) {
        //printf("Error retrieving device Inquiry data.\r\n");
        USB_Host_SetDeviceConfiguration(hDisk->Config.PortNumber, 0);
        return;
    }

    /* printf("Vendor \"%.8s\", Product \"%.16s\"\r\n", InquiryData.VendorID, InquiryData.ProductID); */

    //printf("Mass Storage Device Enumerated.\r\n");
}


/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_MassStorage MassStorage_LPCUSBLib_Interface = {
    {
        // Common device interface
        "MS:USB",
        0x0100,
        MassStorage_LPCUSBLib_InitializeWorkspace,
        sizeof(T_LPCUSBLib_MS_Workspace),
    },

    // Functions
    MassStorage_LPCUSBLib_Init,
    MassStorage_LPCUSBLib_Status,
    MassStorage_LPCUSBLib_Read,
    MassStorage_LPCUSBLib_Write,
    MassStorage_LPCUSBLib_Sync,
    MassStorage_LPCUSBLib_GetSizeInfo,
    MassStorage_LPCUSBLib_SetPower,
    MassStorage_LPCUSBLib_SetLock,
    MassStorage_LPCUSBLib_SetSoftwareWriteProtect,
    MassStorage_LPCUSBLib_Eject,
    MassStorage_LPCUSBLib_MiscControl,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  USBMS_driver.c
 *-------------------------------------------------------------------------*/
