/*-------------------------------------------------------------------------*
 * File:  Stream_LPCUSBLib_SerialHost.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Stream interface for the Segger SWO Cortex M debugger terminal.
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
#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include "Stream_LPCUSBLib_SerialHost.h"
#include <Source/Library/LPCOpen/LPCUSBLib/Drivers/USB/USB.h>
#include <Source/Library/LPCOpen/LPCUSBLib/LPCUSBLib_uEZ.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
/* Max CDC interfaces supported depends on the total number of pipes the host stack supports */
#define MAX_SERIAL_INTERFACES       ((PIPE_TOTAL_PIPES - 1)/3)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_STREAM *iDevice;
    T_uezTask iMonitorTask;
    TUInt8 iUnitAddress;
    USB_ClassInfo_CDC_Host_t *iSerialInterface;
} T_Stream_LPCUSBLib_SerialHost_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static void IEVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum);
static int IUpdate(int aUnitAddress);

static USB_ClassInfo_CDC_Host_t Serial_Interface0[MAX_SERIAL_INTERFACES] = {
    {
        .Config = {
            .NotificationPipeNumber         = 1,
            .NotificationPipeDoubleBank = false,

            .DataINPipeNumber               = 2,
            .DataINPipeDoubleBank           = false,

            .DataOUTPipeNumber              = 3,
            .DataOUTPipeDoubleBank          = false,
            .PortNumber = 0,
        },
    },
    {
        .Config = {
            .NotificationPipeNumber         = 4,
            .NotificationPipeDoubleBank = false,

            .DataINPipeNumber               = 5,
            .DataINPipeDoubleBank           = false,

            .DataOUTPipeNumber              = 6,
            .DataOUTPipeDoubleBank          = false,
            .PortNumber = 0,
        },
    },
};

static USB_ClassInfo_CDC_Host_t Serial_Interface1[MAX_SERIAL_INTERFACES] = {
    {
        .Config = {
            .NotificationPipeNumber         = 1,
            .NotificationPipeDoubleBank = false,

            .DataINPipeNumber               = 2,
            .DataINPipeDoubleBank           = false,

            .DataOUTPipeNumber              = 3,
            .DataOUTPipeDoubleBank          = false,
            .PortNumber = 1,
        },
    },
    {
        .Config = {
            .NotificationPipeNumber         = 4,
            .NotificationPipeDoubleBank = false,

            .DataINPipeNumber               = 5,
            .DataINPipeDoubleBank           = false,

            .DataOUTPipeNumber              = 6,
            .DataOUTPipeDoubleBank          = false,
            .PortNumber = 1,
        },
    },
};

static T_LPCUSBLib_Host_Callbacks G_HostCallbacks = {
        0, // HostError
        IEVENT_USB_Host_DeviceEnumerationComplete, // DeviceEnumerationComplete
        0, // DeviceEnumerationFailed
        0, // DeviceUnattached
        0, // DeviceAttached
        IUpdate, // Update
};


/* Buffer for serial data */
static char buffer[MAX_SERIAL_INTERFACES][PIPE_MAX_SIZE] = { "SERIAL_ONE_MSG_0", "SERIAL_TWO_MSG_0" };
/* Total number of CDC  interfaces seen in the device configuration descriptor */
static uint8_t serialInfCount;

static USB_ClassInfo_CDC_Host_t *Serial_Interfaces[2] = {
        &Serial_Interface0[0],
        &Serial_Interface1[0]
};
static T_Stream_LPCUSBLib_SerialHost_Workspace *G_workspaces[2] = { 0, 0 };

void EVENT_CDC_Host_ControLineStateChanged(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo)
{
    /* Called when data is received on interrupt endpoint, the data is present in CDCInterfaceInfo->State.ControlLineStates.DeviceToHost*/
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
static void IEVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
    uint16_t ConfigDescriptorSize;
    uint8_t ConfigDescriptorData[512];
    uint8_t *pCfgDesc;
    uint8_t i;
    uint8_t err_code;
    USB_ClassInfo_CDC_Host_t *p_serial = Serial_Interfaces[corenum];

    /* Read Configuration descriptor */
    if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize, ConfigDescriptorData, sizeof(ConfigDescriptorData))
            != HOST_GETCONFIG_Successful) {
        printf("Error Retrieving Configuration Descriptor.\r\n");
        return;
    }

    serialInfCount = 0;
    pCfgDesc = ConfigDescriptorData;
    /* Parse configuration descriptor to find CDC interfaces, note that the pointer to the configuration data is changed each time the function is called */
    while ((err_code = CDC_Host_FindConfigInterface(&p_serial[serialInfCount], &ConfigDescriptorSize, (void **)&pCfgDesc))
            == CDC_ENUMERROR_NoError) {
        serialInfCount++;
        if (serialInfCount >= MAX_SERIAL_INTERFACES) {
            printf("Reached Max Serial Interface of %d\r\n", MAX_SERIAL_INTERFACES);
            break;
        }
    }

    if (err_code == CDC_ENUMERROR_PipeConfigurationFailed) {
        printf("Error in Configuring Pipe.\r\n");
        return;
    }
    /* No CDC interface was found */
    if (serialInfCount == 0) {
        printf("Attached Device Not a Valid CDC Device.\r\n");
        return;
    }

    if (USB_Host_SetDeviceConfiguration(p_serial[0].Config.PortNumber, 1) != HOST_SENDCONTROL_Successful) {
        printf("Error Setting Device Configuration.\r\n");
        return;
    }

    /* Set line coding for each interface */
    for (i = 0; i < serialInfCount; i++) {
        p_serial[i].State.LineEncoding.BaudRateBPS = 115200;
        p_serial[i].State.LineEncoding.CharFormat = 0;
        p_serial[i].State.LineEncoding.ParityType = 0;
        p_serial[i].State.LineEncoding.DataBits = 8;
        CDC_Host_SetLineEncoding(&p_serial[i]);
    }

    printf("Serial Device Enumerated.\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a generic serial workspace.  Configure will setup the next
 *      step.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_InitializeWorkspace(void *aW)
{
    return UEZ_ERROR_NONE;
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
static int IUpdate(int aUnitAddress)
{
    T_Stream_LPCUSBLib_SerialHost_Workspace *p = G_workspaces[aUnitAddress];
    TInt32 i, j;
    TUInt16 dataLen;
    int activity = 0;

    /* For each CDC interface check if any data is received, if so read data, modify data and send it back to device. */
    for (i = 0; i < serialInfCount; i++) {
        if((dataLen = CDC_Host_BytesReceived(p->iSerialInterface)) > 0) {
            for(j = 0; j < dataLen; j++) {
                buffer[i][j] = CDC_Host_ReceiveByte(p->iSerialInterface);
            }
            printf("Received data from interface %d: %s", i, buffer[i]);

// TESTING!
            /* Modify data and send it back to device */
            buffer[i][15]++;
            if(buffer[i][15] >= 0x3A) {
                buffer[i][15] = 0x30;
            }
            CDC_Host_SendData(p->iSerialInterface, (const uint8_t *)&buffer[i][0], 19);
        }
    }
    return activity;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the generic configuration with the given queue sizes and
 *      link to HAL Serial device.
 * Inputs:
 *      T_uezDeviceWorkspace *aWorkspace -- This hardware device
 *      TUInt32 aUnitAddress        -- Address of USB unit
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_Configure(
            T_uezDeviceWorkspace *aWorkspace,
            TUInt8 aUnitAddress)
{
    T_Stream_LPCUSBLib_SerialHost_Workspace *p =
            (T_Stream_LPCUSBLib_SerialHost_Workspace *)aWorkspace;
    if (aUnitAddress == 0)
        p->iSerialInterface = &Serial_Interface0[0];
    else
        p->iSerialInterface = &Serial_Interface1[0];

    extern void Chip_USB0_Init(void);
    extern void Chip_USB1_Init(void);
    p->iUnitAddress = aUnitAddress;
    if (aUnitAddress == 0) {
        Chip_USB0_Init();
    } else {
        Chip_USB1_Init();
    }
    G_workspaces[aUnitAddress] = p;

    USB_Init(aUnitAddress, USB_MODE_Host);

    // Create a main task (not running yet)
//    UEZTaskCreate(ISerialHostMonitor, "SerialHost", 2048, p,
//            UEZ_PRIORITY_NORMAL, &p->iMonitorTask);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the stream
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_Open(void *aWorkspace)
{
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the device.
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_Close(void *aWorkspace)
{
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Reading characters from the debug terminal is not possible. This
 *      is an output only stream. Therefore, we simulate a timeout to make
 *      this stream behave like a normal stream device.
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 *      TUInt8 *aData             -- Place to store data
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesRead    -- Number of bytes actually read.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_Read(
            void *aWorkspace,
            TUInt8 *aData,
            TUInt32 aNumBytes,
            TUInt32 *aNumBytesRead,
            TUInt32 aTimeout)
{
    *aNumBytesRead = 0;
    UEZTaskDelay(aTimeout);
    return UEZ_ERROR_TIMEOUT;
}


/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_WriteChar
 *---------------------------------------------------------------------------*
 * Description:
 *      Writes a single character to the SWO debug terminal.
 * Inputs:
 *      TInt8 c                   -- Character to write to the stream
 *      TUInt32 aTimeout          -- Timeout in milliseconds
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_WriteChar(TInt8 c, TUInt32 aTimeout) {
//    TUInt32 tickStart;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data out the segger SWO debug terminal.
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 *      TUInt8 *aData             -- Data to send
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesWritten -- Number of bytes actually written.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_TIMEOUT is returned
 *                                   if timeout occurs trying to write
 *                                   the full amount and not enough room was
 *                                   available.  UEZ_ERROR_NONE is reported if
 *                                   all data is written.
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_Write(
            void *aWorkspace,
            TUInt8 *aData,
            TUInt32 aNumBytes,
            TUInt32 *aNumBytesWritten,
            TUInt32 aTimeout)
{
    T_uezError error;
    TUInt32 i;
    for(i=0; i<aNumBytes; i++) {
        // Print out character per character
        error = Stream_LPCUSBLib_SerialHost_WriteChar(*aData++, aTimeout);
        if(error != UEZ_ERROR_NONE)
            return error;
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_Control
 *---------------------------------------------------------------------------*
 * Description:
 *      Not supported by this device
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 *      TUInt32 aControl          -- Command to execute
 *      void *aControlData        -- Data passed in with the command
 * Outputs:
 *      T_uezError                 -- Error code. UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_Control(
            void *aWorkspace,
            TUInt32 aControl,
            void *aControlData)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_Flush
 *---------------------------------------------------------------------------*
 * Description:
 *      Not supported by this device.
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 *      TUInt32 aTimeout          -- Timeout to wait for flush to finish
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_Flush(void *aWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FullDuplex_Stream_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a full duplex serial stream to a HAL serial device and create
 *      the read and write buffers.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aSerialHALName -- Name of low level Serial HAL driver
 *      TUInt32 aWriteBufferSize -- Size of the write buffer size in bytes
 *      TUInt32 aReadBufferSize -- Size of the read buffer size in bytes
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_Create(
                const char *aName,
                TUInt8 aUnitAddress)
{
    T_uezDeviceWorkspace *p_stream;

    UEZ_LPCUSBLib_Host_Require(aUnitAddress, &G_HostCallbacks, 0);

    // Create serial stream device and link to HAL_Serial driver
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Stream_LPCUSBLib_SerialHost_Interface, 0,
            &p_stream);
    return Stream_LPCUSBLib_SerialHost_Configure(p_stream, aUnitAddress);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_STREAM Stream_LPCUSBLib_SerialHost_Interface = {
    {
        // Generic device interface
        "Serial:LPCUSBLib:SerialHost",
        0x0100,
        Stream_LPCUSBLib_SerialHost_InitializeWorkspace,
        sizeof(T_Stream_LPCUSBLib_SerialHost_Workspace),
    },

    // Functions
    Stream_LPCUSBLib_SerialHost_Open,
    Stream_LPCUSBLib_SerialHost_Close,
    Stream_LPCUSBLib_SerialHost_Control,
    Stream_LPCUSBLib_SerialHost_Read,
    Stream_LPCUSBLib_SerialHost_Write,
    Stream_LPCUSBLib_SerialHost_Flush,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  uEZSerialGeneric.c
 *-------------------------------------------------------------------------*/
