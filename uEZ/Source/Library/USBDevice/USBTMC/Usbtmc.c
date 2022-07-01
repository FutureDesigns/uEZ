/*-------------------------------------------------------------------------*
 * File:  Usbtmc.c
 *-------------------------------------------------------------------------*
 | Description:
 |      USB implementation of the device side of a virtual comm port
 |      following CDC rules.
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <UEZDeviceTable.h>
#include <Device/USBDevice.h>
#include <Source/Library/USBDevice/USBTMC/Usbtmc.h>

//======================================================================================================================
// Private defines
//======================================================================================================================
#define USB_TMC_MAX_PACKET_SIZE                 (64)

// Assign the device endpoint numbers
#define USB_TMC_INT_IN_EP_NUM                   (1)
#define USB_TMC_BULK_OUT_EP_NUM                 (5)
#define USB_TMC_BULK_IN_EP_NUM                  (2)

// Form the complete endpoint address with direction bit 
#define USB_TMC_INT_EP_IN                       USB_ENDPOINT_ADDRESS(USB_TMC_INT_IN_EP_NUM, USB_ENDPOINT_ADDRESS_DIR_IN)
#define USB_TMC_BULK_EP_OUT                     USB_ENDPOINT_ADDRESS(USB_TMC_BULK_OUT_EP_NUM, USB_ENDPOINT_ADDRESS_DIR_OUT)
#define USB_TMC_BULK_EP_IN                      USB_ENDPOINT_ADDRESS(USB_TMC_BULK_IN_EP_NUM, USB_ENDPOINT_ADDRESS_DIR_IN)

// Interface descriptor parameters 
#define USB_DEVICE_CLASS_USBTMC                 (0xFE)
#define TMC_SUBCLASS                            (0x03)
#define TMC_PROTOCOL_NO_SUBCLASS                (0x00)
#define TMC_PROTOCOL_USB488                     (0x01)

// USBTMC class requests
#define TMC_INITIATE_ABORT_BULK_OUT             (1)
#define TMC_CHECK_ABORT_OUT_STATUS              (2)
#define TMC_INITIATE_ABORT_BULK_IN              (3)
#define TMC_CHECK_ABORT_BULK_IN_STATUS          (4)
#define TMC_INITIATE_CLEAR                      (5)
#define TMC_CHECK_CLEAR_STATUS                  (6)
#define TMC_GET_CAPABILITIES                    (7)
#define TMC_INDICATOR_PULSE                     (64)    // optional

// USB488 subclass requests
#define TMC_USB488_READ_STATUS_BYTE             (128)
#define TMC_USB488_REN_CONTROL                  (160)
#define TMC_USB488_GO_TO_LOCAL                  (161)
#define TMC_USB488_LOCAL_LOCKOUT                (162)

// Outbound TMC MsgIDs - host to device - BULK_OUT
#define USBTMC_DEV_DEP_MSG_OUT                  (1)
#define USBTMC_REQUEST_DEV_DEP_MSG_IN           (2)
#define USBTMC_VENDOR_SPECIFIC_OUT              (126)
#define USBTMC_REQUEST_VENDOR_SPECIFIC_IN       (127)
#define USBTMC_TRIGGER                          (128)

// Inbound TMC MsgIDs - device to host - BULK_IN
#define USBTMC_DEV_DEP_MSG_IN                   (2)
#define USBTMC_VENDOR_SPECIFIC_IN               (127)

// USBTMC status values
typedef enum
{
    USBTMC_STATUS_SUCCESS                       = 0x01,
    USBTMC_STATUS_PENDING                       = 0x02,
    USBTMC_STATUS_INTERRUPT_IN_BUSY             = 0x20,
    USBTMC_STATUS_FAILED                        = 0x80,
    USBTMC_STATUS_TRANSFER_NOT_IN_PROGRESS      = 0x81,
    USBTMC_STATUS_SPLIT_NOT_IN_PROGRESS         = 0x82,
    USBTMC_STATUS_SPLIT_IN_PROGRESS             = 0x83
} T_USBTMC_status;

// Queue interface to the application layer
#define GENERIC_BULK_QUEUE_IN_SIZE              (1024)
#define GENERIC_BULK_QUEUE_OUT_SIZE             (1024)

typedef struct  
{
    TUInt8 usbTmcStatus;
    TUInt8 reserved_0;
    TUInt8 bcdUSBTMC[2];
    TUInt8 usbTmcIfaceCapabilities;
    TUInt8 usbTmcDevCapabilities;
    TUInt8 reserved_1[6];
    TUInt8 bcdUSB488[2];
    union 
    {
        TUInt8 data;
        struct
        {
            TUInt8 D0           : 1;    // accept trigger command
            TUInt8 D1           : 1;    // accept REN_CONTROL, GO_TO_LOCAL, LOCAL_LOCKOUT
            TUInt8 D2           : 1;    // interface is usb488.2
            TUInt8 reserved     : 5;    // reserved, must be 0
        };
    } ifaceCapabilities;
    union
    {
        TUInt8 data;
        struct
        {
            TUInt8 D0           : 1;    // device is DT1 capable
            TUInt8 D1           : 1;    // device is RL1 capable
            TUInt8 D2           : 1;    // device is SR1 capable
            TUInt8 D3           : 1;    // device can interpret all mandatory SCPI commands
            TUInt8 reserved     : 4;    // reserved, must be 0
        };
    } devCapabilities;
    TUInt8 reserved_2[8];
} __packed GetCapabilities_t;

typedef struct
{
    T_USBDeviceDescriptor           device;
    T_USBDescriptorConfiguration    config;
    T_USBDescriptorInterface        interface;
    T_USBDescriptorEndpoint         endpoint[3];
    TUInt8                          string[128];
} __packed T_UsbDescriptorTable;

typedef struct
{
    TUInt32 sizeIn;
    TUInt8 bufferIn[USB_TMC_MAX_PACKET_SIZE];
    TUInt32 sizeOut;
    TUInt8 bufferOut[USB_TMC_MAX_PACKET_SIZE];
} __packed usbData_t;

typedef struct
{
    TBool active;
    UsbTmcReqDevDepMsgIn_t msg;
} __packed T_UsbTmcRequest;

typedef struct
{
    TUInt8 status;
    TUInt8 bTag;
    TUInt8 Constant;
} __packed T_CtrlRequestStatusResponse;

typedef struct
{
    struct bNotify1
    {
        TUInt8 bTag         : 7;
        TUInt8 one          : 1;            // must be 0x01
    };
    TUInt8 usb488StatusByte;    // bNotify2 field
} __packed T_IntStatusResponse;

typedef struct
{
    TUInt8 status;
    struct bmClear
    {
        TUInt8 bulkInFifoBytes  : 1; 
        TUInt8 reserved         : 7;
    };
} __packed T_CtrlCheckClearStatusResponse;

// Option to output symbols to debug state of usb
#if 0
    #define dprintf printf
#else
    #define dprintf(...)
#endif

//======================================================================================================================
// Private Prototypes
//======================================================================================================================
TBool UsbTmcHandleClassRequest( void *aWorkspace, T_USBSetupPacket *aSetup, TUInt16 *aLength, TUInt8 **aData);
static void UsbTmcInterruptIn(void *aWorkspace, TUInt8 aEndpoint, T_USBEndpointStatus aStatus);
static void UsbTmcBulkOut(void *aWorkspace, TUInt8 aEndpoint, T_USBEndpointStatus aStatus);
static void UsbTmcBulkIn(void *aWorkspace, TUInt8 aEndpoint, T_USBEndpointStatus aStatus);

//======================================================================================================================
// Private Globals Variables
//======================================================================================================================
static DEVICE_USB_DEVICE **G_ghDevice;
static T_uezDevice G_usbDev;
static T_uezTask G_ghTask;
static T_uezQueue TmcFifoOut;  
static T_uezQueue TmcFifoIn;
static T_uezSemaphore sLock = 0;

static TUInt8 G_USBTmcRequest[8];                           // temporary USB Setup packet storage
static usbData_t usbData;                                   // temporary USB data storage

static T_USBTMC_status USBTMC_status;                       // response status to control request
static T_CtrlRequestStatusResponse ctrlStatusResponse;      // response to status byte control request
static T_IntStatusResponse intStatusResponse;               // response to interrupt IN poll
static T_CtrlCheckClearStatusResponse checkClearStatResp;   // response to check clear status

// implement IEEE-488 registers
static T_UsbTmcStatusByte usb488StatusByte;                 // USB488 status byte register

static TBool epIntInEmpty = ETrue;                          // interrupt endpoint available?
static T_UsbTmcLocalRemoteStatus sTmcStatus;                // bit vector to aide in mainting remote/local control 
                                                            // - usb488 4.3.2
static T_UsbTmcSettings sSettings;                          // local copy of device settings from app

static T_UsbDescriptorTable usbDescriptor = {
  // device descriptor
  .device.iHeader.iLength           = 0x12,
  .device.iHeader.iDescriptorType   = USB_DESCRIPTOR_DEVICE,
  .device.bcdUSB                    = USB_UINT16(0x0110),
  .device.bDeviceClass              = 0x00,
  .device.bDeviceSubClass           = 0x00,
  .device.bDeviceProtocol           = 0x00,
  .device.bMaxPacketSize            = USB_ENDPOINT0_PACKET_SIZE,
  .device.idVendor                  = USB_UINT16(0xC251),
  .device.idProduct                 = USB_UINT16(0x1303),
  .device.bcdDevice                 = USB_UINT16(0x0100),
  .device.iManufacturer             = 0x00,
  .device.iProduct                  = 0x00,
  .device.iSerialNumber             = 0x00,
  .device.bNumConfigurations        = 0x01,
  // configuration descriptor
  .config.iHeader.iLength           = 0x09,
  .config.iHeader.iDescriptorType   = USB_DESCRIPTOR_CONFIGURATION,
  .config.iTotalLength              = USB_UINT16(9+9+7+7+7),
  .config.iNumInterfaces            = 0x01,
  .config.iConfigurationValue       = 0x01,
  .config.iConfiguration            = 0x00,
  .config.iAttributes               = 0xA0,
  .config.iMaxPower                 = 0x32, // (100mA)
  // interface descriptor
  .interface.iHeader.iLength        = 0x09,
  .interface.iHeader.iDescriptorType= USB_DESCRIPTOR_INTERFACE,
  .interface.iNumber                = 0x00,
  .interface.iAlternateSetting      = 0x00,
  .interface.iNumEndpoints          = 0x03,
  .interface.iClass                 = USB_DEVICE_CLASS_USBTMC,
  .interface.iSubClass              = TMC_SUBCLASS,
  .interface.iProtocol              = TMC_PROTOCOL_USB488,
  .interface.iString                = 0x00,
  // endpoint descriptor 
  .endpoint[0].iHeader.iLength      = 0x07,
  .endpoint[0].iHeader.iDescriptorType  = USB_DESCRIPTOR_ENDPOINT,
  .endpoint[0].iAddress                 = USB_TMC_INT_EP_IN,
  .endpoint[0].iAttributes              = USB_ENDPOINT_TYPE_INTERRUPT,
  .endpoint[0].iMaximumPacketSize       = USB_UINT16(USB_TMC_MAX_PACKET_SIZE),
  .endpoint[0].iPollInterval            = 0x0A,
  // endpoint descriptor 
  .endpoint[1].iHeader.iLength      = 0x07,
  .endpoint[1].iHeader.iDescriptorType  = USB_DESCRIPTOR_ENDPOINT,
  .endpoint[1].iAddress                 = USB_TMC_BULK_EP_OUT,
  .endpoint[1].iAttributes              = USB_ENDPOINT_TYPE_BULK,
  .endpoint[1].iMaximumPacketSize       = USB_UINT16(USB_TMC_MAX_PACKET_SIZE),
  .endpoint[1].iPollInterval            = 0x0A,
  // endpoint descriptor 
  .endpoint[2].iHeader.iLength      = 0x07,
  .endpoint[2].iHeader.iDescriptorType  = USB_DESCRIPTOR_ENDPOINT,
  .endpoint[2].iAddress                 = USB_TMC_BULK_EP_IN,
  .endpoint[2].iAttributes              = USB_ENDPOINT_TYPE_BULK,
  .endpoint[2].iMaximumPacketSize       = USB_UINT16(USB_TMC_MAX_PACKET_SIZE),
  .endpoint[2].iPollInterval            = 0x0A,
  // string descriptors
  .string                               = {0}
};

static GetCapabilities_t sGetCapabilities = 
{
    .usbTmcStatus                   = USBTMC_STATUS_SUCCESS,
    .reserved_0                     = 0,
    .bcdUSBTMC                      = {0x01, 0x00},
    .usbTmcIfaceCapabilities        = 0,
    .usbTmcDevCapabilities          = 0,
    .reserved_1                     = {0},
    .bcdUSB488                      = {0x01, 0x00},
    .ifaceCapabilities.D0           = 1,    // interface supports trigger          
    .ifaceCapabilities.D1           = 1,    // interface supports remote control
    .ifaceCapabilities.D2           = 1,    // usb488.2 device
    .devCapabilities.D0             = 1,    // DT1 capable
    .devCapabilities.D1             = 1,    // RL1 capable
    .devCapabilities.D2             = 1,    // SR1 capable
    .devCapabilities.D3             = 1,    // supports all mandatory SCPI cmds
    .reserved_2                     = {0}
};

static T_UsbTmcRequest sUsbTmcRequest = 
{
    .active                     = EFalse,
    .msg                        = {0}
};

//==========================================================================
// Private Functions
//==========================================================================

/*-------------------------------------------------------------------------*
 * Function:  modLock
 *-------------------------------------------------------------------------*
 * Description:
 *      Locks the binary semaphore. 
 * Inputs:
 *      TUInt32 period -- timeout
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static TBool modLock(TUInt32 period)
{
  T_uezError error;
  
  if (0 == sLock) return EFalse;
  error = UEZSemaphoreGrab(sLock, period);  
  return (UEZ_ERROR_NONE == error) ? ETrue : EFalse;
}

/*-------------------------------------------------------------------------*
 * Function:  modUnlock
 *-------------------------------------------------------------------------*
 * Description:
 *      Unlocks the binary semaphore. 
 * Inputs:
 *      TUInt32 period -- timeout
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static TBool modUnlock(void)
{
  TBool result = EFalse;
  if (sLock) {
    UEZSemaphoreRelease(sLock);
    result = ETrue;
  }
  return result;
}

/*-------------------------------------------------------------------------*
 * Function:  setTmcstatusBit
 *-------------------------------------------------------------------------*
 * Description:
 *      Set status bit to reflect SETUP packet data and state info 
 * Inputs:
 *      TUInt32 bitVector        -- bit to set    
 *      TUInt32 timeout          -- timeout for blocking
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static TBool setTmcstatusBit(TUInt32 bitVector, TUInt32 timeout)
{
    TBool status = EFalse;
    
    if (modLock(timeout) == ETrue)
    {   
        sTmcStatus.data |= (bitVector);
        modUnlock();
        status = ETrue;
    }
    return (status);
}

/*-------------------------------------------------------------------------*
 * Function:  clearTmcstatusBit
 *-------------------------------------------------------------------------*
 * Description:
 *      clear status bit to reflect SETUP packet data and state info 
 * Inputs:
 *      TUInt32 bitVector        -- bit to set    
 *      TUInt32 timeout          -- timeout for blocking
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static TBool clearTmcstatusBit(TUInt32 bitVector, TUInt32 timeout)
{
    TBool status = EFalse;
    
    if (modLock(timeout) == ETrue)
    {   
        sTmcStatus.data &= ~(bitVector);
        modUnlock();
        status = ETrue;
    }
    return (status);
}

/*-------------------------------------------------------------------------*
 * Function:  ctrlReadStatusByte
 *-------------------------------------------------------------------------*
 * Description:
 *      Class request to read usb488 status byte 
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 *-------------------------------------------------------------------------*/
static void ctrlReadStatusByte(T_USBSetupPacket *aSetup, TUInt16 *aLength, 
                           TUInt8 **aData)
{
    USBTMC_status = USBTMC_STATUS_INTERRUPT_IN_BUSY;
    TUInt8 bTag = aSetup->iValue;
    
    // NOTE: removed flag check for customer finding FIND002. 
    // queue interrupt IN response if interrupt endpoint available
    //if (epIntInEmpty)
    {
        // write to INT_IN endpoint FIFO
        intStatusResponse.one               = 0x01; // must be 0x01
        intStatusResponse.bTag              = bTag; 
        intStatusResponse.usb488StatusByte  = usb488StatusByte.data;
        ((*G_ghDevice)->Write)(G_ghDevice, USB_TMC_INT_EP_IN, (TUInt8*)&intStatusResponse, 2);
        epIntInEmpty = EFalse;
        USBTMC_status = USBTMC_STATUS_SUCCESS;
    }
    ctrlStatusResponse.status    = USBTMC_status;
    ctrlStatusResponse.bTag      = bTag; 
    ctrlStatusResponse.Constant  = 0x00;
    
    // setup control response
    *aData = (TUInt8*)&ctrlStatusResponse;
    *aLength = 3;
}

/*-------------------------------------------------------------------------*
 * Function:  ctrlRemoteEnable
 *-------------------------------------------------------------------------*
 * Description:
 *      Class request handler to change the state of the REN bit 
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static void ctrlRemoteEnable(T_USBSetupPacket *aSetup, TUInt16 *aLength, 
                           TUInt8 **aData)
{
    if (aSetup->iValue)
    {
        USBTMC_status = setTmcstatusBit(USBTMC_CMD_REN_CONTROL, UEZ_TIMEOUT_NONE) == ETrue ? 
                    USBTMC_STATUS_SUCCESS : USBTMC_STATUS_FAILED;
    }
    else
    {
        USBTMC_status = clearTmcstatusBit(USBTMC_CMD_REN_CONTROL, UEZ_TIMEOUT_NONE) == ETrue ? 
                    USBTMC_STATUS_SUCCESS : USBTMC_STATUS_FAILED;
    }
    dprintf("ctrlRemoteEnable: %02x ", aSetup->iValue);
    dprintf("ret: %02x\n", USBTMC_status);
    
    *aData = (TUInt8*)&USBTMC_status;
    *aLength = 1;
}

/*-------------------------------------------------------------------------*
 * Function:  ctrlGotoLocal
 *-------------------------------------------------------------------------*
 * Description:
 *      Class request handler to change the state of the "goto local" bit 
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static void ctrlGotoLocal(T_USBSetupPacket *aSetup, TUInt16 *aLength, 
                           TUInt8 **aData)
{
    USBTMC_status = setTmcstatusBit(USBTMC_CMD_GOTOLOCAL, UEZ_TIMEOUT_NONE) == ETrue ? 
                USBTMC_STATUS_SUCCESS : USBTMC_STATUS_FAILED;
    dprintf("ctrlGotoLocal: ret: %02x\n", USBTMC_status);
    
    *aData = (TUInt8*)&USBTMC_status;
    *aLength = 1;
}

/*-------------------------------------------------------------------------*
 * Function:  ctrlLocalLockout
 *-------------------------------------------------------------------------*
 * Description:
 *      Class request handler to change the state of the local lockout bit. 
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static void ctrlLocalLockout(T_USBSetupPacket *aSetup, TUInt16 *aLength, 
                           TUInt8 **aData)
{
    USBTMC_status = setTmcstatusBit(USBTMC_CMD_LLO, UEZ_TIMEOUT_NONE) == ETrue ? 
                USBTMC_STATUS_SUCCESS : USBTMC_STATUS_FAILED;
    dprintf("ctrlLocalLockout: ret: %02x\n", USBTMC_status);
    
    *aData = (TUInt8*)&USBTMC_status;
    *aLength = 1;
}

/*-------------------------------------------------------------------------*
 * Function:  ctrlIndicatorPulse
 *-------------------------------------------------------------------------*
 * Description:
 *      Class request handler to change the status of the indicator pulse bit. 
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static void ctrlIndicatorPulse(T_USBSetupPacket *aSetup, TUInt16 *aLength, 
                           TUInt8 **aData)
{
    USBTMC_status = setTmcstatusBit(USBTMC_CMD_INDICATOR, UEZ_TIMEOUT_NONE) == ETrue ? 
                USBTMC_STATUS_SUCCESS : USBTMC_STATUS_FAILED;
    dprintf("ctrlIndicatorPulse: ret: %02x\n", USBTMC_status);
    
    *aData = (TUInt8*)&USBTMC_status;
    *aLength = 1;
}

/*-------------------------------------------------------------------------*
 * Function:  ctrlInitiateClear
 *-------------------------------------------------------------------------*
 * Description:
 *      Class request handler to clear enpoint buffers. 
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static void ctrlInitiateClear(T_USBSetupPacket *aSetup, TUInt16 *aLength, 
                           TUInt8 **aData)
{
    memset(usbData.bufferIn, 0, sizeof(usbData.bufferIn));
    memset(usbData.bufferOut, 0, sizeof(usbData.bufferOut));
    usbData.sizeIn = 0;
    usbData.sizeOut = 0;

    USBTMC_status = USBTMC_STATUS_SUCCESS;
    *aData = (TUInt8*)&USBTMC_status;
    *aLength = 1;
}

/*-------------------------------------------------------------------------*
 * Function:  ctrlInitiateClear
 *-------------------------------------------------------------------------*
 * Description:
 *      Class request handler to check status of the clear operation.
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static void ctrlCheckClearStatus(T_USBSetupPacket *aSetup, TUInt16 *aLength, 
                           TUInt8 **aData)
{
    checkClearStatResp.status = USBTMC_STATUS_SUCCESS;
    checkClearStatResp.bulkInFifoBytes = 0;
    *aData = (TUInt8*)&checkClearStatResp;
    *aLength = 2;
}

/*-------------------------------------------------------------------------*
 * Function:  processRequest
 *-------------------------------------------------------------------------*
 * Description:
 *      Process a DEV_DEP_REQUEST from a BULK_OUT endpoint. Pull application
 *      data out of the queue, and write up to the maximum allowed to the
 *      BULK_IN endpoint.
 *-------------------------------------------------------------------------*/
static TBool processRequest(void)
{
    T_uezError error;
    TUInt32 bytesToSend;
    UsbTmcDevDepMsgIn_t *respMsg;
    TBool termFound = EFalse;
    
    error = UEZQueueGetCount(TmcFifoOut, &bytesToSend);
    if (error != UEZ_ERROR_NONE)
      return (EFalse);
    
    if (bytesToSend)
    {
        memset(usbData.bufferIn, 0, sizeof(usbData.bufferIn));
        respMsg = (UsbTmcDevDepMsgIn_t*)usbData.bufferIn;
        respMsg->header.MsgID       = USBTMC_DEV_DEP_MSG_IN;
        respMsg->header.bTag        = sUsbTmcRequest.msg.header.bTag;
        respMsg->header.bTagInverse = sUsbTmcRequest.msg.header.bTagInverse;
        
        TUInt32 i = 0;
        while (i < bytesToSend && i < USB_TMC_MAX_PACKET_SIZE && termFound == EFalse)
        {
            UEZQueueReceive(TmcFifoOut, &respMsg->cmd[i], UEZ_TIMEOUT_NONE);
            if (sSettings.bSupportsEOS == ETrue &&
                sSettings.ucEOSChar == respMsg->cmd[i])
            { // termination character for response - flag end of message
                respMsg->EOM = 1;
                termFound = ETrue;
            }
            i++;
        }
        respMsg->header.TransferSize = i;
        usbData.sizeIn = respMsg->header.TransferSize + 12; // payload + header
        
        // write to BULK_IN endpoint FIFO
        ((*G_ghDevice)->Write)(G_ghDevice, USB_TMC_BULK_EP_IN, usbData.bufferIn, usbData.sizeIn);
        dprintf("processRequest: writing BULK_IN resp %d bytes, termFound: %d\n", usbData.sizeIn, termFound);
    }
    return (termFound);
}

/*-------------------------------------------------------------------------*
 * Function:  usbTmcDevDepOut
 *-------------------------------------------------------------------------*
 * Description:
 *      Handler for BULK_OUT device dependent data. Pass the command data
 *      up to the application layer through the queue.
 * Inputs:
 *      TUInt8 *msg                 -- buffer to received data
 *      TUInt32 len                 -- length of buffer
 *-------------------------------------------------------------------------*/
static void usbTmcDevDepOut(UsbTmcDevDepMsgOut_t *msg, TUInt32 len)
{
    T_uezError error;
    TUInt32 i;
    
    if (msg->EOM)
    {
        for (i = 0; i < msg->header.TransferSize; i++)
        {
            error = UEZQueueSend(TmcFifoIn, &msg->cmd[i], UEZ_TIMEOUT_NONE);
            if (error)
              break;
        }
    } 
}

/*-------------------------------------------------------------------------*
 * Function:  usbTmcDevDepIn
 *-------------------------------------------------------------------------*
 * Description:
 *      Handle Device dependent data requests. 
 * Inputs:
 *      TUInt8 *msg                 -- buffer to received data
 *      TUInt32 len                 -- length of buffer
 *-------------------------------------------------------------------------*/
static void usbTmcDevDepIn(UsbTmcDevDepMsgIn_t *msg, TUInt32 len)
{   
    if (!sUsbTmcRequest.active)
    {
        // latch the request
        memcpy(&sUsbTmcRequest.msg, msg, len);       
        sUsbTmcRequest.active = ETrue;
    }
}

/*-------------------------------------------------------------------------*
 * Function:  usbTmcTrigger
 *-------------------------------------------------------------------------*
 * Description:
 *      Called in response to a BULK_OUT trigger msg 
 *-------------------------------------------------------------------------*/
static void usbTmcTrigger(void)
{
    USBTMC_status = setTmcstatusBit(USBTMC_CMD_TRIGGER, UEZ_TIMEOUT_NONE) == ETrue ? 
            USBTMC_STATUS_SUCCESS : USBTMC_STATUS_FAILED;
            dprintf("usbTmcTrigger: ret: %02x ", USBTMC_status);
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmcInterruptIn
 *-------------------------------------------------------------------------*
 * Description:
 *      Handler for the signifying the completion of an interrupt in endpiont
 *      poll request.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
static void UsbTmcInterruptIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    // data sent - allow new interrupt responses to be queued
    epIntInEmpty = ETrue;
    // clear the RQS bit from the status byte - will be set if this interrupt  
    // transfer was triggered by an SRQ condition
    usb488StatusByte.RQS = 0;
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmcBulkOut
 *-------------------------------------------------------------------------*
 * Description:
 *      Handler for receiving BULK_OUT data from the host.
 * Inputs:
 *      TUInt8 aEndpoint            -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
static void UsbTmcBulkOut(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    TUInt16 length;
    TUInt8 msgId;
    
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aStatus);
            
    TBool stallState = ((*G_ghDevice)->IsStalled)(G_ghDevice, USB_TMC_BULK_EP_OUT);
    dprintf("stall state: %d\n", stallState);    
    
    // Read data from endpoint
    length = (*G_ghDevice)->Read(G_ghDevice, 
                                 aEndpoint, 
                                 usbData.bufferOut, 
                                 sizeof(usbData.bufferOut));
    
    msgId = usbData.bufferOut[0];
    switch(msgId)
    {
        case USBTMC_DEV_DEP_MSG_OUT:
            usbTmcDevDepOut((UsbTmcDevDepMsgOut_t*)usbData.bufferOut, length);
            break;
        case USBTMC_REQUEST_DEV_DEP_MSG_IN:
            usbTmcDevDepIn((UsbTmcDevDepMsgIn_t*)usbData.bufferOut, length);
            break;
        case USBTMC_VENDOR_SPECIFIC_OUT:
            break;
        case USBTMC_REQUEST_VENDOR_SPECIFIC_IN:
            break;
        case USBTMC_TRIGGER:
            usbTmcTrigger();
            break;
    }
    // purge usb buffer
    memset(usbData.bufferOut, 0, sizeof(usbData.bufferOut));
    usbData.sizeOut = 0;   
}

/*-------------------------------------------------------------------------*
 * Function:  USBMSDriveBulkIn
 *-------------------------------------------------------------------------*
 * Description:
 *      Handler for signifying completion of a BULK_IN write to the host.
 * Inputs:
 *      TUInt8 aEndpoint            -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
static void UsbTmcBulkIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aStatus);
    
    TUInt32 bytesToSend;
    
    UEZQueueGetCount(TmcFifoOut, &bytesToSend);
    if (bytesToSend == 0)
        UsbTmc_clearSTBBit(USBTMC_STATUS_MAV, UEZ_TIMEOUT_NONE);
}

/*-------------------------------------------------------------------------*
 * Function:  appendStringDescriptor
 *-------------------------------------------------------------------------*
 * Description:
 *      Appends a new string descriptor to the existing string desriptor
 *      array.
 * Inputs:
 *      TUInt8 *str                 -- pointer to the NULL terminated string
 *                                      descriptor
 * Outputs:
 *      TUInt8 index                -- the index for this string descriptor
 *-------------------------------------------------------------------------*/
static TUInt8 appendStringDescriptor(TUInt8 *str)
{
    TUInt32 DescOffset = 0;
    TUInt32 i, len;
    TUInt8 index = 0;
    TUInt8 *strDesc;
    
    while (usbDescriptor.string[DescOffset] != 0 && DescOffset < sizeof(usbDescriptor.string))
    {
        DescOffset += usbDescriptor.string[DescOffset];
        index++;
    }
     
    len = strlen((const char*)str);
    // set the size of the string descriptor
    strDesc = &usbDescriptor.string[DescOffset];
    *strDesc++ = len * 2 + 2;
    *strDesc++ = USB_DESCRIPTOR_STRING;
 
    // convert character string to UTF-16
    for (i = 0; i < len; i++)
    {
        *strDesc++ = str[i];
        *strDesc++ = 0;
    }
    return (index);
}

/*-------------------------------------------------------------------------*
 * Function:  setDeviceConfiguration
 *-------------------------------------------------------------------------*
 * Description:
 *      Set configuration settings required for device operation
 * Inputs:
 *      T_UsbTmcSettings *settings          -- device settings
 *-------------------------------------------------------------------------*/
static void setDeviceConfiguration(T_UsbTmcSettings *settings)
{
    // copy settings locally for reference
    memcpy(&sSettings, settings, sizeof(sSettings));
    
    usbDescriptor.device.idVendor   = settings->usVendorId;
    usbDescriptor.device.idProduct  = settings->usProductId;
    
    // first set the language string
    usbDescriptor.string[0] = 4;
    usbDescriptor.string[1] = USB_DESCRIPTOR_STRING;
    usbDescriptor.string[2] = 0x09; // English - lower byte
    usbDescriptor.string[3] = 0x04; // English - upper byte
    
    if (settings->sManf)
    { // Manufacturer string exists
        usbDescriptor.device.iManufacturer = appendStringDescriptor(settings->sManf);
    }
    if (settings->sModel)
    { // Product string exists
        usbDescriptor.device.iProduct = appendStringDescriptor(settings->sModel);
    }
    if (settings->sSerial)
    { // Serial string exists
        usbDescriptor.device.iSerialNumber = appendStringDescriptor(settings->sSerial);
    }
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmcHandleClassRequest
 *-------------------------------------------------------------------------*
 * Description:
 *      Handle USBTMC class requests. 
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static TBool UsbTmcHandleClassRequest(
                void *aWorkspace,
                T_USBSetupPacket *aSetup,
                TUInt16 *aLength,
                TUInt8 **aData)
{
    PARAM_NOT_USED(aWorkspace);

    switch (aSetup->iRequest)
    { 
        case TMC_INITIATE_ABORT_BULK_OUT:
            dprintf("TMC_INITIATE_ABORT_BULK_OUT\n");
            break;
        case TMC_CHECK_ABORT_OUT_STATUS:
            dprintf("TMC_CHECK_ABORT_OUT_STATUS\n");
            break;
        case TMC_INITIATE_ABORT_BULK_IN:
            dprintf("TMC_INITIATE_ABORT_BULK_IN\n");
            break;
        case TMC_CHECK_ABORT_BULK_IN_STATUS:
            dprintf("TMC_CHECK_ABORT_BULK_IN_STATUS\n");
            break;
        case TMC_INITIATE_CLEAR:
            ctrlInitiateClear(aSetup, aLength, aData);
            dprintf("TMC_INITIATE_CLEAR\n");
            break;
        case TMC_CHECK_CLEAR_STATUS:
            ctrlCheckClearStatus(aSetup, aLength, aData);
            dprintf("TMC_CHECK_CLEAR_STATUS\n");
            break;
        case TMC_GET_CAPABILITIES:
            *aData = (TUInt8*)&sGetCapabilities;
            *aLength = sizeof(GetCapabilities_t);
            break;
        case TMC_INDICATOR_PULSE:
            ctrlIndicatorPulse(aSetup, aLength, aData);
            break;
        case TMC_USB488_READ_STATUS_BYTE:
          ctrlReadStatusByte(aSetup, aLength, aData);
          break;
        case TMC_USB488_REN_CONTROL:
          ctrlRemoteEnable(aSetup, aLength, aData);
          break;
        case TMC_USB488_GO_TO_LOCAL:
          ctrlGotoLocal(aSetup, aLength, aData);
          break;
        case TMC_USB488_LOCAL_LOCKOUT:
          ctrlLocalLockout(aSetup, aLength, aData);
          break;
        default:
            dprintf("Unknown class request\n");
            return EFalse;
    }
    return ETrue;
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmcMonitor
 *-------------------------------------------------------------------------*
 * Description:
 *      USBTMC monitoring task- process endpoints, and handle device requests 
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
static TUInt32 UsbTmcMonitor(T_uezTask aMyTask, void *aParameters)
{
    PARAM_NOT_USED(aParameters);
    PARAM_NOT_USED(aMyTask);

    // Just constantly process endpoint data
    for (;;)  
    {
        // process endpoints- periodically timeout the lock
        // to process an asynchronous RQS event
        ((*G_ghDevice)->ProcessEndpoints)(G_ghDevice, 1000);
        
        if (sUsbTmcRequest.active)
        {
            // deassert request when processing complete
            sUsbTmcRequest.active ^= processRequest();
        }
        if (usb488StatusByte.RQS && epIntInEmpty)
        {
            // write to INT_IN endpoint FIFO
            intStatusResponse.one               = 0x01; // must be 0x01
            intStatusResponse.bTag              = 0x01; 
            intStatusResponse.usb488StatusByte  = usb488StatusByte.data;
            ((*G_ghDevice)->Write)(G_ghDevice, USB_TMC_INT_EP_IN, (TUInt8*)&intStatusResponse, 2);
            epIntInEmpty = EFalse;
        }
    }
}
//==========================================================================
// Public Functions
//==========================================================================

/*-------------------------------------------------------------------------*
 * Function:  UsbTmc_open
 *-------------------------------------------------------------------------*
 * Description:
 *      Open the USBTMC interface and begin USB communication
 *
 * Inputs:
 *      const char *aDeviceName         -- the USB device name
 *      T_UsbTmcSettings *settings      -- setting for this device
 * Outputs:
 *      T_uezError error                -- UEZ_ERROR_NONE on success
 *-------------------------------------------------------------------------*/
T_uezError UsbTmc_open(const char *aDeviceName, T_UsbTmcSettings *settings)
{
    T_uezError error;

    error = UEZDeviceTableFind(aDeviceName, &G_usbDev);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(
                G_usbDev,
                (T_uezDeviceWorkspace **)&G_ghDevice);
    if (error)
        return error;

    error = UEZQueueCreate(GENERIC_BULK_QUEUE_IN_SIZE, 1, &TmcFifoIn);
    if (error != UEZ_ERROR_NONE)
        return error;

    error = UEZQueueCreate(GENERIC_BULK_QUEUE_OUT_SIZE, 1, &TmcFifoOut);
    if (error != UEZ_ERROR_NONE) 
    {
        UEZQueueDelete(TmcFifoIn);
        return error;
    }
    
    error = UEZSemaphoreCreateBinary(&sLock);
    
    // set device configuration
    setDeviceConfiguration(settings);

    // Configure the device driver descriptor table
    ((*G_ghDevice)->Configure)(G_ghDevice, (TUInt8 *)&usbDescriptor);

    ((*G_ghDevice)->RegisterRequestTypeCallback)(
            G_ghDevice,
            USB_REQUEST_TYPE_CLASS,
            G_USBTmcRequest,
            0,
            UsbTmcHandleClassRequest);
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_IN(USB_TMC_INT_IN_EP_NUM),
            UsbTmcInterruptIn);
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_OUT(USB_TMC_BULK_OUT_EP_NUM),
            UsbTmcBulkOut);
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_IN(USB_TMC_BULK_IN_EP_NUM),
            UsbTmcBulkIn);
      
    // We are ready, let's initialize it and connect
    ((*G_ghDevice)->Initialize)(G_ghDevice);
    ((*G_ghDevice)->Connect)(G_ghDevice);
    
    // Now create a task that constantly process the USBMSDrive buffers
    error = UEZTaskCreate(
                UsbTmcMonitor,
                "USBTMC",
                UEZ_TASK_STACK_BYTES(1024),
                0,
                UEZ_PRIORITY_HIGH,
                &G_ghTask);

    if (error == UEZ_ERROR_NONE)
        dprintf("UsbTmc_open: SUCCESS\n");
    else
        dprintf("UsbTmc_open: FAILURE\n");
        
    return error;
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmc_read
 *-------------------------------------------------------------------------*
 * Description:
 *      Application interface to receive new command SCPI command strings 
 * Inputs:
 *      TUInt8 *buf                 -- pointer to receive buffer
 *      TUInt32 size                -- size of buffer 
 *      TUInt32 timeout             -- timeout in ticks
 * Outputs:
 *      TUInt32                     -- Number of bytes received from queue
 *-------------------------------------------------------------------------*/
TUInt32 UsbTmc_read(TUInt8 *buf, TUInt32 size, TUInt32 timeout)
{
  TUInt32 i;
  
  for (i = 0; i < size; i++)
  {
    if (UEZ_ERROR_NONE != UEZQueueReceive(TmcFifoIn, &buf[i], timeout))
        break;
  }
  if (i)
  {
      dprintf("UsbTmc_read: SUCCESS reading %d bytes from queue\n", i);
  }
  return i;
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmc_write
 *-------------------------------------------------------------------------*
 * Description:
 *      Application interface to write SCPI response strings to host. 
 * Inputs:
 *      TUInt8 *buf                 -- pointer to send buffer
 *      TUInt32 size                -- length of buffer 
 *      TUInt32 timeout             -- timeout in ticks
 * Outputs:
 *      TUInt32                     -- Number of bytes sent from queue
 *-------------------------------------------------------------------------*/
TUInt32 UsbTmc_write(TUInt8 *buf, TUInt32 len, TUInt32 timeout)
{
  TUInt32 i;
  
  for (i = 0; i < len; i++)
  {
    if (UEZ_ERROR_NONE != UEZQueueSend(TmcFifoOut, &buf[i], timeout))
        break;
  }
  if (i)
  {
      // signify there is data available to the host
      UsbTmc_setSTBBit(USBTMC_STATUS_MAV, UEZ_TIMEOUT_NONE);
      dprintf("UsbTmc_write: SUCCESS writing %d/%d bytes to queue\n", i, len);
  }
  else
  {
    dprintf("UsbTmc_write: FAILED writing %d bytes to queue\n", len);
  }
  return (i);
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmc_getStatus
 *-------------------------------------------------------------------------*
 * Description:
 *      Handle USBTMC class requests. 
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
TBool UsbTmc_getStatus(TUInt32 *tmcstat, TUInt32 timeout)
{
    TBool status = EFalse;
    
    if (modLock(timeout) == ETrue)
    {   
        *tmcstat = sTmcStatus.data;
        modUnlock();
        status = ETrue;
    }
    return (status);
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmc_setREN
 *-------------------------------------------------------------------------*
 * Description:
 *      Set the remote enable bit in the driver remote/staus register
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
TBool UsbTmc_setREN(TUInt32 timeout)
{
    return(setTmcstatusBit(USBTMC_CMD_REN_CONTROL, timeout));
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmc_clearREN
 *-------------------------------------------------------------------------*
 * Description:
 *      Clear the remote enable bit in the driver remote/staus register
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
TBool UsbTmc_clearREN(TUInt32 timeout)
{
    return (clearTmcstatusBit(USBTMC_CMD_REN_CONTROL, timeout));
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmc_getSTB
 *-------------------------------------------------------------------------*
 * Description:
 *      Handle USBTMC class requests. 
 * Inputs:
 *      TUInt8 *iSTB                -- Pointer to application status byte
 *      TUInt32 timeout             -- timeout
 * Outputs:
 *      TBool                       -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
TBool UsbTmc_getSTB(TUInt8 *iSTB, TUInt32 timeout)
{
    TBool status = EFalse;
    
    if (modLock(timeout) == ETrue)
    {   
        *iSTB = usb488StatusByte.data;
        modUnlock();
        status = ETrue;
    }
    return (status);
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmc_setSTBBit
 *-------------------------------------------------------------------------*
 * Description:
 *      Set a bit in the 488 status byte 
 * Inputs:
 *      TUInt8 bitVector         -- bit(s) to clear
 *      TUInt32 timeout          -- timeout
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
TBool UsbTmc_setSTBBit(TUInt8 bitVector, TUInt32 timeout)
{
    TBool status = EFalse;
    
    if (modLock(timeout) == ETrue)
    {   
        usb488StatusByte.data |= bitVector;
        modUnlock();
        status = ETrue;
    }
    return (status);
}

/*-------------------------------------------------------------------------*
 * Function:  UsbTmc_clearSTBBit
 *-------------------------------------------------------------------------*
 * Description:
 *      Clear a bit in the 488 status byte 
 * Inputs:
 *      TUInt8 bitVector         -- bit(s) to clear
 *      TUInt32 timeout          -- timeout
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
TBool UsbTmc_clearSTBBit(TUInt8 bitVector, TUInt32 timeout)
{
    TBool status = EFalse;
    
    if (modLock(timeout) == ETrue)
    {   
        usb488StatusByte.data &= ~(bitVector);
        modUnlock();
        status = ETrue;
    }
    return (status);
}

/*-------------------------------------------------------------------------*
 * End of File:  usbtmc.c
 *-------------------------------------------------------------------------*/
