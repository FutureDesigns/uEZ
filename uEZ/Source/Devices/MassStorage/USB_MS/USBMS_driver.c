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
#include <uEZRTOS.h>
#include "USB_MS.h"
#include <Device/USBHost.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define CBW_SIGNATURE                       0x43425355
#define CSW_SIGNATURE                       0x53425355
#define CBW_SIZE                            31
#define CSW_SIZE                            13
#define CSW_CMD_PASSED                      0x00
#define SCSI_CMD_REQUEST_SENSE              0x03
#define SCSI_CMD_TEST_UNIT_READY            0x00
#define SCSI_CMD_READ_10                    0x28
#define SCSI_CMD_READ_CAPACITY              0x25
#define SCSI_CMD_WRITE_10                   0x2A

// Mass Storage Specific Definitions:
#define MS_GET_MAX_LUN_REQ                  0xFE
#define MASS_STORAGE_CLASS                  0x08
#define MASS_STORAGE_SUBCLASS_SCSI          0x06
#define MASS_STORAGE_PROTOCOL_BO            0x50

#define IGrab()         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()      UEZSemaphoreRelease(p->iSem)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_MassStorage *iDevice;
    T_uezSemaphore iSem;
    T_uezSemaphore iDone;
    DEVICE_USBHost **iUSBHost;
    volatile TBool iInitPerformed;
    TUInt8 *descBuf;
    TUInt32 iBlockSize;
    TUInt8 *blockBuf;
    TBool iSWWriteProtect;
    TUInt8 iEpBulkIn;
    TUInt8 iEpBulkOut;
    volatile TBool iIsConnected;
} T_MSUSB_Workspace;

typedef enum ms_data_dir {
    MS_DATA_DIR_IN     = 0x80,
    MS_DATA_DIR_OUT    = 0x00,
    MS_DATA_DIR_NONE   = 0x01
} MS_DATA_DIR;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_USB_ConnectedCheckMatch(
            void *aWorkspace,
            TUInt8 aDeviceAddress);
static T_uezError MassStorage_USB_Disconnected(
            void *aWorkspace,
            TUInt8 aDeviceAddress);
static T_uezError MassStorage_USB_Init(void *aWorkspace, TUInt32 aAddress);
static T_uezError MassStorage_USB_TryConnect(void *aWorkspace, TUInt32 aAddress);
static T_uezError MassStorage_USB_Read(
            void *aWorkspace,
            const TUInt32 aStart,
            const TUInt32 aNumBlocks,
            void *aBuffer);
extern void  WriteLE32U (volatile  TUInt8 *pmem, TUInt32   val);
extern void  WriteBE32U (volatile  TUInt8  *pmem,
                            TUInt32   val);
extern TUInt32  ReadBE32U (volatile  TUInt8  *pmem);
extern TUInt16  ReadLE16U (volatile  TUInt8  *pmem);
void  WriteBE16U (volatile  TUInt8  *pmem,
                            TUInt16   val);
TUInt16 ReadBE16U (volatile  TUInt8  *pmem);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static const T_USBHostDeviceDriverAPI MassStorage_USB_USBHost_API = {
    MassStorage_USB_ConnectedCheckMatch,
    MassStorage_USB_Disconnected,
};

static T_uezError MassStorage_USB_Disconnected(
            void *aWorkspace,
            TUInt8 aDeviceAddress)
{
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;
    PARAM_NOT_USED(aDeviceAddress);

    IGrab();
    // Note that we need to init before we can run
    p->iInitPerformed = EFalse;
    p->iIsConnected = EFalse;
    IRelease();

    return UEZ_ERROR_NONE;
}

static T_uezError MassStorage_USB_ConnectedCheckMatch(
            void *aWorkspace,
            TUInt8 aDeviceAddress)
{
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;
    T_uezError error;
    PARAM_NOT_USED(aDeviceAddress);

    // Try to do the initialization
    error = MassStorage_USB_TryConnect(aWorkspace, aDeviceAddress);
    if (error == UEZ_ERROR_NONE) {
        // Kick it off by reading the first block on the device
        MassStorage_USB_Read(
                    aWorkspace,
                    0,
                    1,
                    p->blockBuf);
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_USB_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a MassStorage device on the USB Host.
 * Inputs:
 *      void *aWorkspace                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_USB_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error;
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;

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

static void IFillCommand(
        T_MSUSB_Workspace *p,
        TUInt32   block_number,
        TUInt32   block_size,
        TUInt16   num_blocks,
        MS_DATA_DIR  direction,
        TUInt8   scsi_cmd,
        TUInt8   scsi_cmd_len)
{
    TUInt32  data_len;
    static  TUInt32  tag_cnt = 0;
            TUInt32  cnt;
    TUInt8 *TDBuffer = p->descBuf;


    for (cnt = 0; cnt < CBW_SIZE; cnt++)
         TDBuffer[cnt] = 0;
    switch(scsi_cmd) {

        case SCSI_CMD_TEST_UNIT_READY:
             data_len = 0;
             break;
        case SCSI_CMD_READ_CAPACITY:
             data_len = 8;
             break;
        case SCSI_CMD_REQUEST_SENSE:
             data_len = 18;
             break;
        default:
             data_len = block_size * num_blocks;
             break;
    }
    WriteLE32U(TDBuffer, CBW_SIGNATURE);
    WriteLE32U(&TDBuffer[4], tag_cnt);
    WriteLE32U(&TDBuffer[8], data_len);
    TDBuffer[12]     = (direction == MS_DATA_DIR_NONE) ? 0 : direction;
    TDBuffer[14]     = scsi_cmd_len;                                   /* Length of the CBW                 */
    TDBuffer[15]     = scsi_cmd;
    if (scsi_cmd     == SCSI_CMD_REQUEST_SENSE) {
        TDBuffer[19] = (TUInt8)data_len;
    } else {
        WriteBE32U(&TDBuffer[17], block_number);
    }
    WriteBE16U(&TDBuffer[22], num_blocks);
}

static T_uezError ITestUnitReady(T_MSUSB_Workspace *p)
{
    T_uezError error;
    DEVICE_USBHost **p_usbHost = p->iUSBHost;

    IFillCommand(
        p,
        0,
        0,
        0,
        MS_DATA_DIR_NONE,
        SCSI_CMD_TEST_UNIT_READY,
        6);
    error = (*p_usbHost)->BulkOut(
                p_usbHost,
                1,
                p->iEpBulkOut,
                p->descBuf,
                CBW_SIZE,
                2000);
    if (error == UEZ_ERROR_NONE) {
        error = (*p_usbHost)->BulkIn(
                    p_usbHost,
                    1,
                    p->iEpBulkIn,
                    p->descBuf,
                    CBW_SIZE,
                    2000);
        if (error == UEZ_ERROR_NONE) {
            if (p->descBuf[12] != 0)
                error = UEZ_ERROR_NAK;
        }
    }
    return error;
}

static T_uezError IGetSenseInfo(T_MSUSB_Workspace *p)
{
    T_uezError error;
    DEVICE_USBHost **p_usbHost = p->iUSBHost;

    IFillCommand(
            p,
            0,
            0,
            0,
            MS_DATA_DIR_IN,
            SCSI_CMD_REQUEST_SENSE,
            6);
    error = (*p_usbHost)->BulkOut(
                p_usbHost,
                1,
                p->iEpBulkOut,
                p->descBuf,
                CBW_SIZE,
                2000);
    if (error == UEZ_ERROR_NONE) {
        error = (*p_usbHost)->BulkIn(
                    p_usbHost,
                    1,
                    p->iEpBulkIn,
                    p->descBuf,
                    18,
                    2000);
        if (error == UEZ_ERROR_NONE) {
            error = (*p_usbHost)->BulkIn(
                        p_usbHost,
                        1,
                        p->iEpBulkIn,
                        p->descBuf,
                        CSW_SIZE,
                        2000);
            if (error == UEZ_ERROR_NONE) {
                if (p->descBuf[12] != 0)
                    error = UEZ_ERROR_NAK;
            }
        }
    }
    return error;
}

static T_uezError IReadCapacity(T_MSUSB_Workspace *p)
{
    T_uezError error;
    DEVICE_USBHost **p_usbHost  = p->iUSBHost;

    IFillCommand(
        p,
        0,
        0,
        0,
        MS_DATA_DIR_IN,
        SCSI_CMD_READ_CAPACITY,
        10);
    error = (*p_usbHost)->BulkOut(
                p_usbHost,
                1,
                p->iEpBulkOut,
                p->descBuf,
                CBW_SIZE,
                10000);
    if (error == UEZ_ERROR_NONE) {
        error = (*p_usbHost)->BulkIn(
                    p_usbHost,
                    1,
                    p->iEpBulkIn,
                    p->descBuf,
                    8,
                    10000);
        if (error == UEZ_ERROR_NONE) {
            p->iBlockSize = ReadBE32U(&p->descBuf[4]);
            error = (*p_usbHost)->BulkIn(
                        p_usbHost,
                        1,
                        p->iEpBulkIn,
                        p->descBuf,
                        CSW_SIZE,
                        10000);
            if (error == UEZ_ERROR_NONE) {
                if (p->descBuf[12] != 0)
                    error = UEZ_ERROR_NAK;
            }
        }
    }
    return error;
}

static T_uezError MassStorage_USB_TryConnect(void *aWorkspace, TUInt32 aAddress)
{
    T_uezError error;
    TUInt32 retry;
    TUInt32 descLen;
    TUInt8 *desc_ptr;
    TUInt8 ms_int_found;

    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;
    DEVICE_USBHost **p_usbHost;

    IGrab();

    p_usbHost = p->iUSBHost;

    // Allocate some buffers
    p->descBuf = (*p_usbHost)->AllocBuffer(p_usbHost, 128);
    p->blockBuf = (*p_usbHost)->AllocBuffer(p_usbHost, 4096);

    // Get configuration information
    error = (*p_usbHost)->GetDescriptor(
                p_usbHost,
                aAddress,
                USB_DESCRIPTOR_TYPE_CONFIGURATION,
                0,
                p->descBuf,
                9,
                5000);
    if (error) {
        IRelease();
        return error;
    }

    // Get type configuration descriptor
    descLen = ReadLE16U(&p->descBuf[2]);
    error = (*p_usbHost)->GetDescriptor(
                p_usbHost,
                aAddress,
                USB_DESCRIPTOR_TYPE_CONFIGURATION,
                0,
                p->descBuf,
                descLen,
                5000);
    if (error) {
        IRelease();
        return error;
    }

    desc_ptr = p->descBuf;
    ms_int_found = 0;

    // Make sure this is a type configuration
    if (desc_ptr[1] != USB_DESCRIPTOR_TYPE_CONFIGURATION) {
        IRelease();
        return UEZ_ERROR_UNKNOWN;
    }

    // Skip down to the data
    desc_ptr += desc_ptr[0];
    descLen = *((TUInt16 *) &p->descBuf[2]);

    // walk through the data's buffers until it gets to the end (complete length)
    while (desc_ptr < (p->descBuf + descLen)) {
        // What type of block is this?
        switch (desc_ptr[1]) {
            case USB_DESCRIPTOR_TYPE_INTERFACE:
                // Is this the right type of interface for mass storage?
                if (desc_ptr[5] == MASS_STORAGE_CLASS &&
                        desc_ptr[6] == MASS_STORAGE_SUBCLASS_SCSI &&
                        desc_ptr[7] == MASS_STORAGE_PROTOCOL_BO)
                    ms_int_found = 1;
                break;
            case USB_DESCRIPTOR_TYPE_ENDPOINT:
                // Found an endpoint descriptor
                // Is it a bulk endpoint? (We're only configuring bulk right now, ignore rest)
                if ((desc_ptr[3] & 0x03) == 0x02) {
                    if (desc_ptr[2] & 0x80) {
                        p->iEpBulkIn = desc_ptr[2] & 0x7F;
                    } else {
                        p->iEpBulkOut = desc_ptr[2] & 0x7F;
                    }

                    // Configure the endpiont (either in or out)
                    error = (*p_usbHost)->ConfigureEndpoint(
                                p_usbHost,
                                aAddress,
                                desc_ptr[2],  // end point and if in or out
                                ReadLE16U(&desc_ptr[4])); // max packet size
                    if (error) {
                        IRelease();
                        return error;
                    }
                }
                break;
            default:
                // All others, we ignore
                break;
        }

        // Move to next descriptor
        desc_ptr += desc_ptr[0];
    }
    if (ms_int_found) {
//        printf("Mass Storage device connected\n");
    } else {
        IRelease();
//            printf("Not a Mass Storage device\n");
        return UEZ_ERROR_NOT_FOUND;
    }

    // Set to the first configuration (to activate)
    error = (*p_usbHost)->SetConfiguration(p_usbHost, aAddress, 1);
    if (error) {
        IRelease();
        return error;
    }
    UEZTaskDelay(100);

#if 0
    TUInt32 str;
    for (str=0; str<10; str++) {
        memset(p->descBuf, 0xFF, 128);
        error = (*p_usbHost)->GetString(p_usbHost, aAddress, str, descBuf, 128, 2000);
        printf("String #%d error %d\n", str, error);
    }
#endif

    // Get MAX logical unit
    error = (*p_usbHost)->Control(
            p_usbHost,
            aAddress,
            USB_DEVICE_TO_HOST | USB_REQ_TYPE_CLASS | USB_RECIPIENT_INTERFACE,
            MS_GET_MAX_LUN_REQ,
            0,
            0,
            1,
            p->descBuf,
            10000);
    if (error == UEZ_ERROR_STALL) {
        // command not supported ... ignore
    } else if (error) {
        IRelease();
        return error;
    }

    // Wait until ready (up to 10 seconds, or 200 * 100 ms)
    retry = 200;
    while (retry) {
        error = ITestUnitReady(p);
        if (error == UEZ_ERROR_NONE)
            break;
        UEZTaskDelay(100);
        IGetSenseInfo(p);
        retry--;
    }
    error = IReadCapacity(p);
    if (error) {
        IRelease();
        return error;
    }

//    p->iInitPerformed = ETrue;
    p->iIsConnected = ETrue;

    IRelease();
    return UEZ_ERROR_NONE;
}

T_uezError MassStorage_USB_Configure(
        void *aWorkspace,
        const char *aUSBHostName)
{
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;
    T_uezError error;
    T_uezDevice device;

    // Link the USB Host to this mass storage device
    error = UEZDeviceTableFind(aUSBHostName, &device);
    if (error != UEZ_ERROR_NONE)
        return error;
    error = UEZDeviceTableGetWorkspace(device, (T_uezDeviceWorkspace **)&p->iUSBHost);
    if (error != UEZ_ERROR_NONE)
        return error;

    (*p->iUSBHost)->USBHostDeviceDriverRegister(
        p->iUSBHost,
        aWorkspace,
        &MassStorage_USB_USBHost_API);

    return error;
}

static T_uezError MassStorage_USB_Init(void *aWorkspace, TUInt32 aAddress)
{
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;

    // Cannot initialize an unconnected device
    if (!p->iIsConnected)
        return UEZ_ERROR_NOT_AVAILABLE;

    p->iInitPerformed = ETrue;

    return UEZ_ERROR_NONE;
}

static T_uezError MassStorage_USB_Status(void *aWorkspace, T_msStatus *aStatus)
{
    T_msStatus status;
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;

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

static T_uezError IBulkRecv(
        T_MSUSB_Workspace *p,
        TUInt32 block_number,
        TUInt16 num_blocks)
{
    T_uezError error;
    DEVICE_USBHost **p_usbHost = p->iUSBHost;

    IFillCommand(
            p,
            block_number,
            p->iBlockSize,
            num_blocks,
            MS_DATA_DIR_IN,
            SCSI_CMD_READ_10,
            10);
    error = (*p_usbHost)->BulkOut(
                p_usbHost,
                1,
                p->iEpBulkOut,
                p->descBuf,
                CBW_SIZE,
                10000);
    if (error == UEZ_ERROR_NONE) {
        error = (*p_usbHost)->BulkIn(
                    p_usbHost,
                    1,
                    p->iEpBulkIn,
                    p->blockBuf,
                    p->iBlockSize * num_blocks,
                    10000);
        if (error == UEZ_ERROR_NONE) {
            error = (*p_usbHost)->BulkIn(
                    p_usbHost,
                    1,
                    p->iEpBulkIn,
                    p->descBuf,
                    CSW_SIZE,
                    10000);
            if (error == UEZ_ERROR_NONE) {
                if (p->descBuf[12] != 0)
                    error = UEZ_ERROR_NAK;
            }
        }
    }
    return error;
}

static T_uezError IBulkSend(
        T_MSUSB_Workspace *p,
        TUInt32 block_number,
        TUInt16 num_blocks)
{
    T_uezError error;
    DEVICE_USBHost **p_usbHost = p->iUSBHost;

    IFillCommand(
            p,
            block_number,
            p->iBlockSize,
            num_blocks,
            MS_DATA_DIR_OUT,
            SCSI_CMD_WRITE_10,
            10);
    error = (*p_usbHost)->BulkOut(
            p_usbHost,
            1,
            p->iEpBulkOut,
            p->descBuf,
            CBW_SIZE,
            10000);
    if (error == UEZ_ERROR_NONE) {
        error = (*p_usbHost)->BulkOut(
                    p_usbHost,
                    1,
                    p->iEpBulkOut,
                    p->blockBuf,
                    p->iBlockSize * num_blocks,
                    10000);
        if (error == UEZ_ERROR_NONE) {
            error = (*p_usbHost)->BulkIn(
                        p_usbHost,
                        1,
                        p->iEpBulkIn,
                        p->descBuf,
                        CSW_SIZE,
                        10000);
            if (error == UEZ_ERROR_NONE) {
                if (p->descBuf[12] != 0)
                    error = UEZ_ERROR_NAK;
            }
        }
    }
    return error;

}


static T_uezError MassStorage_USB_Read(
            void *aWorkspace,
            const TUInt32 aStart,
            const TUInt32 aNumBlocks,
            void *aBuffer)
{
    T_uezError error;
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;
    TUInt32 block = aStart;
    TUInt32 num = aNumBlocks;

    IGrab();

    // Although there are faster ways to do this, we'll
    // read one block at a time for now and let someone optimize it later
    while (num--) {
        error = IBulkRecv(p, block, 1);
        if (error != UEZ_ERROR_NONE) {
            IRelease();
            return error;
        }
        memcpy(aBuffer, p->blockBuf, p->iBlockSize);
        aBuffer = (void *)(((char *)aBuffer)+p->iBlockSize);
        block++;
    }

    IRelease();

    return UEZ_ERROR_NONE;
}

static T_uezError MassStorage_USB_Write(
            void *aWorkspace,
            const TUInt32 aStart,
            const TUInt32 aNumBlocks,
            const void *aBuffer)
{
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 num = aNumBlocks;
    TUInt32 block = aStart;

    IGrab();

    // Write each block in order
    while (num--) {
        memcpy(p->blockBuf, aBuffer, p->iBlockSize);
        error = IBulkSend(p, block, 1);
        if (error)
            break;
        block++;
        aBuffer = (void *)(((char *)aBuffer)+p->iBlockSize);
    }

    IRelease();

    return error;
}

static T_uezError MassStorage_USB_Sync(void *aWorkspace)
{
//    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;

    // Just always report done, we are not caching anywhere
    return UEZ_ERROR_NONE;
}

static T_uezError MassStorage_USB_GetSizeInfo(
            void *aWorkspace,
            T_msSizeInfo *aInfo)
{
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;
    TUInt32 mbrBlock;
    TUInt32 numSec;
    #define BPB_FATSz16			22
    #define BPB_FATSz32			36
    #define BPB_BytsPerSec		11
    T_uezError error;

    IGrab();

    // Get the number of sectors
    // Get the boot sector
    error = IBulkRecv(p, 0, 1);
    if (error) {
        IRelease();
        return error;
    }
    // Determine where the master boot record is
    mbrBlock = (TUInt32)(p->blockBuf[454]);
    // get MBR sector
    error = IBulkRecv(p, mbrBlock, 1);
    if (error) {
        IRelease();
        return error;
    }

    // Read Number of sectors per FAT (FAT16 first, FAT32 otherwise)
    numSec = ReadBE32U(&p->blockBuf[BPB_FATSz16]);
    if (!numSec)
        numSec = ReadBE32U(&p->blockBuf[BPB_FATSz32]);
    aInfo->iNumSectors = numSec;

    // Get the sector size (usually 512)
    aInfo->iSectorSize = ReadBE16U(&p->blockBuf[BPB_BytsPerSec]);

    // Get the size of the blocks
    error = IReadCapacity(p);
    if (error) {
        IRelease();
        return error;
    }
    aInfo->iBlockSize = p->iBlockSize;

    // Determine block size
    return UEZ_ERROR_NONE;
}

static T_uezError MassStorage_USB_SetPower(void *aWorkspace, TBool aOn)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

static T_uezError MassStorage_USB_SetLock(void *aWorkspace, TBool aLock)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

static T_uezError MassStorage_USB_SetSoftwareWriteProtect(
        void *aWorkspace,
        TBool aSWWriteProtect)
{
    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;
    IGrab();
    p->iSWWriteProtect = aSWWriteProtect;
    IRelease();
    return UEZ_ERROR_NONE;
}

static T_uezError MassStorage_USB_Eject(void *aWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

static T_uezError MassStorage_USB_MiscControl(
            void *aWorkspace,
            TUInt32 aControlCode,
            void *aBuffer)
{
//    T_MSUSB_Workspace *p = (T_MSUSB_Workspace *)aWorkspace;
    return UEZ_ERROR_ILLEGAL_OPERATION;
}


void MassStorage_USB_Create(const char *aName, const char *aUSBHost)
{
    DEVICE_MassStorage **ms;

    // Register USB Mass Storage device
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&MassStorage_USB_Interface,
            0,
            (T_uezDeviceWorkspace **)&ms);
    MassStorage_USB_Configure((void *)ms, aUSBHost);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_MassStorage MassStorage_USB_Interface = {
	{
	    // Common device interface
	    "MS:USB",
	    0x0100,
	    MassStorage_USB_InitializeWorkspace,
	    sizeof(T_MSUSB_Workspace),
	},
	
    // Functions
    MassStorage_USB_Init,
    MassStorage_USB_Status,
    MassStorage_USB_Read,
    MassStorage_USB_Write,
    MassStorage_USB_Sync,
    MassStorage_USB_GetSizeInfo,
    MassStorage_USB_SetPower,
    MassStorage_USB_SetLock,
    MassStorage_USB_SetSoftwareWriteProtect,
    MassStorage_USB_Eject,
    MassStorage_USB_MiscControl,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  USBMS_driver.c
 *-------------------------------------------------------------------------*/
