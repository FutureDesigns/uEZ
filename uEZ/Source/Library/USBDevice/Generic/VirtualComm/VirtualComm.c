/*-------------------------------------------------------------------------*
 * File:  VirtualComm.c
 *-------------------------------------------------------------------------*
 | Description:
 |      USB implementation of the device side of a virtual comm port
 |      following CDC rules.
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
#include <uEZDeviceTable.h>
#include "Device/USBDevice.h"
#include "VirtualComm.h"
#include <uEZPacked.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define VCOMM_MAX_PACKET_SIZE 64

// Special CDC class descriptors:
#define VCOMM_CDC_INTERFACE                         0x24
#define VCOMM_CDC_ENDPOINT                          0x25

// CDC class commands:
#define	SET_LINE_CODING                             0x20
#define	GET_LINE_CODING                             0x21
#define	SET_CONTROL_LINE_STATE                      0x22

#define CONTROL_LINE_STATE_RTS                      (1<<1)
#define CONTROL_LINE_STATE_DTR                      (1<<0)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iBaudRate;
    TUInt8 iCharFormat;
    TUInt8 iParity;
    TUInt8 iNumDataBits;
} PACKED TLineCoding;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
TBool VirtualCommHandleClassRequest(
        void *aWorkspace,
        T_USBSetupPacket *aSetup, 
        TUInt16 *aLength, 
        TUInt8 **aData);

void VirtualCommBulkIn(
        void *aWorkspace,
        TUInt8 aEndpoint, 
        T_USBEndpointStatus aStatus);

void VirtualCommBulkOut(
        void *aWorkspace,
        TUInt8 aEndpoint, 
        T_USBEndpointStatus aStatus);

void VirtualCommInterruptIn(
        void *aWorkspace,
        TUInt8 aEndpoint, 
        T_USBEndpointStatus aStatus);

extern TBool MainCharReceivedMonitor(char aChar);
extern void MainCharEmptyOutput(void);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static TLineCoding LineCoding = {115200, 0, 0, 8};
static TUInt8 G_vcommBuffer[64];
static TUInt8 G_VirtualCommRequest[8];

static T_uezQueue G_vcommFifoOut;   // out from this device
static T_uezQueue G_vcommFifoIn;    // in to this device

static T_vcommCallbacks G_callbacks;
static DEVICE_USB_DEVICE **G_vcDevice;
static T_uezDevice G_usbDev;
static T_uezTask G_vcTask;
//static TBool G_isBusy;

#if 0
const T_USBDevice G_USBDevice = {
    // Routine to handle status changes
    0,

    // Recipient type of handlers
    // Standard handler
    USBStandardDeviceRequest, 
    G_standardRequestPacketData,

    // Class handler
    VirtualCommHandleClassRequest, 
    G_VirtualCommRequest,

    // Vendor handler
    0, 
    0,

    // Reserved handler (all others)
    0, 
    0,

    // Endpoint 0
    USBHandleControlTransfer,
    USBHandleControlTransfer,

    // Endpoint 1
    VirtualCommInterruptIn,  /* In 0x81 */
    0,

    // Endpoint 2
    VirtualCommBulkIn, /* In 0x82 */
    0,

    // Endpoint 3
    0,
    0,

    // Endpoint 4
    0,
    0,

    // Endpoint 5
    0,
    VirtualCommBulkOut, /* Out 0x05 */

    // Endpoint 6
    0,
    0,

    // Endpoint 7
    0,
    0,

    // Endpoint 8
    0,
    0,

    // Endpoint 9
    0,
    0,

    // Endpoint 10
    0,
    0,

    // Endpoint 11
    0,
    0,

    // Endpoint 12
    0,
    0,

    // Endpoint 13
    0,
    0,

    // Endpoint 14
    0,
    0,

    // Endpoint 15
    0,
    0
};
#endif

#if 0
const TUInt8 G_USBDescriptorTable[] = {
    // device descriptor
    0x12,
    USB_DESCRIPTOR_DEVICE,
    USB_UINT16(0x0200),			// bcdUSB
    0x02,						// bDeviceClass
    0x00,						// bDeviceSubClass
    0x00,						// bDeviceProtocol
    USB_ENDPOINT0_PACKET_SIZE,			// bMaxPacketSize
    USB_UINT16(0xFFFF),			// idVendor
    USB_UINT16(0x0005),			// idProduct
    USB_UINT16(0x0100),			// bcdDevice
    0x01,						// iManufacturer
    0x02,						// iProduct
    0x03,						// iSerialNumber
    0x01,						// bNumConfigurations

    // configuration descriptor
    0x09,
    USB_DESCRIPTOR_CONFIGURATION,
    USB_UINT16(67),				// wTotalLength
    0x02,						// bNumInterfaces
    0x01,						// bConfigurationValue
    0x00,						// iConfiguration
    0xC0,						// bmAttributes
    0x32,						// bMaxPower
    // control class interface
    0x09,
    USB_DESCRIPTOR_INTERFACE,
    0x00,						// bInterfaceNumber
    0x00,						// bAlternateSetting
    0x01,						// bNumEndPoints
    0x02,						// bInterfaceClass
    0x02,						// bInterfaceSubClass
    0x01,						// bInterfaceProtocol, linux requires value of 1 for the cdc_acm module
    0x00,						// iInterface
    // header functional descriptor
    0x05,
    VCOMM_CDC_INTERFACE,
    0x00,
    USB_UINT16(0x0110),
    // call management functional descriptor
    0x05,
    VCOMM_CDC_INTERFACE,
    0x01,
    0x01,						// bmCapabilities = device handles call management
    0x01,						// bDataInterface
    // ACM functional descriptor
    0x04,
    VCOMM_CDC_INTERFACE,
    0x02,
    0x02,						// bmCapabilities
    // union functional descriptor
    0x05,
    VCOMM_CDC_INTERFACE,
    0x06,
    0x00,						// bMasterInterface
    0x01,						// bSlaveInterface0
    // notification EP
    0x07,
    USB_DESCRIPTOR_ENDPOINT,
    USB_ENDPOINT_ADDRESS(1, USB_ENDPOINT_ADDRESS_DIR_IN),					// bEndpointAddress
    USB_ENDPOINT_TYPE_INTERRUPT,						// bmAttributes = intr
    USB_UINT16(8),					// wMaxPacketSize
    0x0A,						// bInterval
    // data class interface descriptor
    0x09,
    USB_DESCRIPTOR_INTERFACE,
    0x01,						// bInterfaceNumber
    0x00,						// bAlternateSetting
    0x02,						// bNumEndPoints
    0x0A,						// bInterfaceClass = data
    0x00,						// bInterfaceSubClass
    0x00,						// bInterfaceProtocol
    0x00,						// iInterface
    // data EP OUT
    0x07,
    USB_DESCRIPTOR_ENDPOINT,
    USB_ENDPOINT_ADDRESS(5, USB_ENDPOINT_ADDRESS_DIR_OUT),
    USB_ENDPOINT_TYPE_BULK,						// bmAttributes = bulk
    USB_UINT16(VCOMM_MAX_PACKET_SIZE),	// wMaxPacketSize
    0x00,						// bInterval
    // data EP in
    0x07,
    USB_DESCRIPTOR_ENDPOINT,
    USB_ENDPOINT_ADDRESS(2, USB_ENDPOINT_ADDRESS_DIR_IN),
    USB_ENDPOINT_TYPE_BULK,						// bmAttributes = bulk
    USB_UINT16(VCOMM_MAX_PACKET_SIZE),	// wMaxPacketSize
    0x00,						// bInterval

    // string descriptors
    0x04,
    USB_DESCRIPTOR_STRING,
    USB_UINT16(0x0409),

    0x2A,
    USB_DESCRIPTOR_STRING,
    'F', 0, 'u', 0, 't', 0, 'u', 0, 'r', 0, 'e', 0, ' ', 0, 'D', 0, 'e', 0, 's', 0, 'i', 0, 'g', 0, 'n', 0, 's', 0, ',', 0, ' ', 0, 'I', 0, 'n', 0, 'c', 0, '.', 0,

    0x20,
    USB_DESCRIPTOR_STRING,
    'L', 0, 'C', 0, 'D', 0, 'D', 0, 'e', 0, 'm', 0, 'o', 0, '-', 0, 'L', 0, 'P', 0,
	'C', 0, '2', 0, '1', 0, '5', 0, '8', 0,

    0x0C,
    USB_DESCRIPTOR_STRING,
    'R', 0, 'e', 0, 'v', 0, ' ', 0, '1', 0,

    // terminating zero
    0
};
#endif

const TUInt8 G_USBDescriptorTable[] = {
    // device descriptor
    0x12,
    USB_DESCRIPTOR_DEVICE,
    USB_UINT16(0x0200),			// bcdUSB
    0x02,						// bDeviceClass
    0x00,						// bDeviceSubClass
    0x00,						// bDeviceProtocol
    USB_ENDPOINT0_PACKET_SIZE,			// bMaxPacketSize
    USB_UINT16(0xFFFF),			// idVendor
    USB_UINT16(0x0005),			// idProduct
    USB_UINT16(0x0100),			// bcdDevice
    0x01,						// iManufacturer
    0x02,						// iProduct
    0x03,						// iSerialNumber
    0x01,						// bNumConfigurations

    // configuration descriptor
    0x09,
    USB_DESCRIPTOR_CONFIGURATION,
    USB_UINT16(67),				// wTotalLength
    0x02,						// bNumInterfaces
    0x01,						// bConfigurationValue
    0x00,						// iConfiguration
    0xC0,						// bmAttributes
    0x32,						// bMaxPower
    // control class interface
    0x09,
    USB_DESCRIPTOR_INTERFACE,
    0x00,						// bInterfaceNumber
    0x00,						// bAlternateSetting
    0x01,						// bNumEndPoints
    0x02,						// bInterfaceClass
    0x02,						// bInterfaceSubClass
    0x01,						// bInterfaceProtocol, linux requires value of 1 for the cdc_acm module
    0x00,						// iInterface
    // header functional descriptor
    0x05,
    VCOMM_CDC_INTERFACE,
    0x00,
    USB_UINT16(0x0110),
    // call management functional descriptor
    0x05,
    VCOMM_CDC_INTERFACE,
    0x01,
    0x01,						// bmCapabilities = device handles call management
    0x01,						// bDataInterface
    // ACM functional descriptor
    0x04,
    VCOMM_CDC_INTERFACE,
    0x02,
    0x02,						// bmCapabilities
    // union functional descriptor
    0x05,
    VCOMM_CDC_INTERFACE,
    0x06,
    0x00,						// bMasterInterface
    0x01,						// bSlaveInterface0
    // notification EP
    0x07,
    USB_DESCRIPTOR_ENDPOINT,
    USB_ENDPOINT_ADDRESS(1, USB_ENDPOINT_ADDRESS_DIR_IN),					// bEndpointAddress
    USB_ENDPOINT_TYPE_INTERRUPT,						// bmAttributes = intr
    USB_UINT16(8),					// wMaxPacketSize
    0x0A,						// bInterval
    // data class interface descriptor
    0x09,
    USB_DESCRIPTOR_INTERFACE,
    0x01,						// bInterfaceNumber
    0x00,						// bAlternateSetting
    0x02,						// bNumEndPoints
    0x0A,						// bInterfaceClass = data
    0x00,						// bInterfaceSubClass
    0x00,						// bInterfaceProtocol
    0x00,						// iInterface
    // data EP OUT
    0x07,
    USB_DESCRIPTOR_ENDPOINT,
    USB_ENDPOINT_ADDRESS(5, USB_ENDPOINT_ADDRESS_DIR_OUT),
    USB_ENDPOINT_TYPE_BULK,						// bmAttributes = bulk
    USB_UINT16(VCOMM_MAX_PACKET_SIZE),	// wMaxPacketSize
    0x00,						// bInterval
    // data EP in
    0x07,
    USB_DESCRIPTOR_ENDPOINT,
    USB_ENDPOINT_ADDRESS(2, USB_ENDPOINT_ADDRESS_DIR_IN),
    USB_ENDPOINT_TYPE_BULK,						// bmAttributes = bulk
    USB_UINT16(VCOMM_MAX_PACKET_SIZE),	// wMaxPacketSize
    0x0A,						// bInterval

    // string descriptors
    0x04,
    USB_DESCRIPTOR_STRING,
    USB_UINT16(0x0409),

    0x2A,
    USB_DESCRIPTOR_STRING,
    'F', 0, 'u', 0, 't', 0, 'u', 0, 'r', 0, 'e', 0, ' ', 0, 'D', 0, 'e', 0, 's', 0, 'i', 0, 'g', 0, 'n', 0, 's', 0, ',', 0, ' ', 0, 'I', 0, 'n', 0, 'c', 0, '.', 0,

    0x16,
    USB_DESCRIPTOR_STRING,
    'U', 0, 'S', 0, 'B', 0, '-', 0, 'D', 0, 'o', 0, 'n', 0, 'g', 0, 'l', 0, 'e', 0,

    0x0C,
    USB_DESCRIPTOR_STRING,
    'R', 0, 'e', 0, 'v', 0, ' ', 0, '2', 0,

    // terminating zero
    0
};

/*-------------------------------------------------------------------------*
 * Function:  VirtualCommInterruptIn
 *-------------------------------------------------------------------------*
 * Description:
 *      A USB interrupt has polled.  Respond with a notification about
 *      the state of the serial lines.
 *      NOTE:  Currently this does not seem to work!
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void VirtualCommInterruptIn(
        void *aWorkspace,
        TUInt8 aEndpoint, 
        T_USBEndpointStatus aStatus)
{
    // Do nothing
    PARAM_NOT_USED(aStatus);
    PARAM_NOT_USED(aWorkspace);

    G_vcommBuffer[0] = 0xA1;
    G_vcommBuffer[1] = 0x20;  // SERIAL_STATE
    G_vcommBuffer[2] = 0x00;  // wValue
    G_vcommBuffer[3] = 0x00;
    G_vcommBuffer[4] = 0x01;  // Interface
    G_vcommBuffer[5] = 0x00;
    G_vcommBuffer[6] = 0x02;  // wLength
    G_vcommBuffer[7] = 0x00;
    G_vcommBuffer[8] = 0x00;  // UART State
    G_vcommBuffer[9] = 0x00; 
    ((*G_vcDevice)->Write)(G_vcDevice, aEndpoint, G_vcommBuffer, 10);
}

/*-------------------------------------------------------------------------*
 * Function:  VirtualCommBulkOut
 *-------------------------------------------------------------------------*
 * Description:
 *      Bulk virtual comm data has come out of the PC.  This data
 *      is put into the fifo.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void VirtualCommBulkOut(
        void *aWorkspace,
        TUInt8 aEndpoint, 
        T_USBEndpointStatus aStatus)
{
    TInt16 i, length;
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aStatus);

    // Read data from endpoint
    length = (*G_vcDevice)->Read(
                G_vcDevice,
                aEndpoint, 
                G_vcommBuffer, 
                sizeof(G_vcommBuffer));

    // Store all data until we are full.
    // TBD:  If the PC keeps sending us data and our
    //          buffer is full, we currently have no handshaking
    //          to handle overflow.
    for (i=0; i<length; i++)  {
        // Put in the queue one character at time quickly, until full.
        if (UEZQueueSend(G_vcommFifoIn, &G_vcommBuffer[i], 0) != UEZ_ERROR_NONE)
            break;
    }
}


/*-------------------------------------------------------------------------*
 * Function:  VirtualCommBulkIn
 *-------------------------------------------------------------------------*
 * Description:
 *      The PC is requesting data to be sent back to it.  Pull data
 *      output of the fifo and send it back up to the maximum size
 *      packet.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void VirtualCommBulkIn(
        void *aWorkspace,
        TUInt8 aEndpoint, 
        T_USBEndpointStatus aStatus)
{
    TInt16 i, length;
    TUInt8 c;
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aStatus);

    // Pull out data up to the maximum size of a packet
    for (i=0; i<VCOMM_MAX_PACKET_SIZE; i++)  {
        // Only process if we have data
        if (UEZQueueReceive(G_vcommFifoOut, &G_vcommBuffer[i], 0) != UEZ_ERROR_NONE)
            break;
    }
    length = i;

    // If we have data, send it back.
    // If we do not have data, don't respond.  Responding has
    // been shown to cause continual updates and USB bandwidth issues.
    // Effectively a NAK is created.
    if (length > 0) {
//        G_isBusy = ETrue;
        (*G_vcDevice)->Write(G_vcDevice, aEndpoint, G_vcommBuffer, length);
    } else {
//        G_isBusy = EFalse;
    }

    // Check to see if there is more in the queue and if not
    // report we are empty (if we have a callback)
    if (G_callbacks.iVCEmptyOutput) {
        if (UEZQueuePeek(G_vcommFifoOut, &c, 0) == UEZ_ERROR_TIMEOUT) {
            G_callbacks.iVCEmptyOutput();
        }
    }
}

/*-------------------------------------------------------------------------*
 * Function:  VirtualCommHandleClassRequest
 *-------------------------------------------------------------------------*
 * Description:
 *      Handle CDC class requests that are required for the virtual
 *      COMM port.  Notify the system each time the baud rate is changed.
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
TBool VirtualCommHandleClassRequest(
                void *aWorkspace,
                T_USBSetupPacket *aSetup, 
                TUInt16 *aLength, 
                TUInt8 **aData)
{
    PARAM_NOT_USED(aWorkspace);

//printf("{VC.Class $%02X}", aSetup->iRequest);
    switch (aSetup->iRequest) {
        // set line coding
        case SET_LINE_CODING:
            LineCoding = *((TLineCoding *)*aData);
            *aData = (TUInt8 *)&LineCoding;
            *aLength = 7;
            UEZTaskDelay(1); // Why is this delay needed?
            if (G_callbacks.iVCLineSpeedChange)
                G_callbacks.iVCLineSpeedChange(LineCoding.iBaudRate);
            break;

        // get line coding
        case GET_LINE_CODING:
             *aData = (TUInt8 *)&LineCoding;
            *aLength = 7;
            break;

        // set control line state
        case SET_CONTROL_LINE_STATE:
            if (G_callbacks.iVCControlLineState)
                G_callbacks.iVCControlLineState(
                    (aSetup->iValue & CONTROL_LINE_STATE_RTS)?ETrue:EFalse,
                    (aSetup->iValue & CONTROL_LINE_STATE_DTR)?ETrue:EFalse);
            break;

        default:
            return EFalse;
    }
    return ETrue;
}

TUInt32 VirtualCommMonitor(T_uezTask aMyTask, void *aParameters)
{
    PARAM_NOT_USED(aParameters);
    PARAM_NOT_USED(aMyTask);

    // Just constantly process endpoint data
    for (;;)  {
        ((*G_vcDevice)->ProcessEndpoints)(G_vcDevice, UEZ_TIMEOUT_INFINITE);
    }
}

/*-------------------------------------------------------------------------*
 * Function:  VirtualCommInitialize
 *-------------------------------------------------------------------------*
 * Description:
 *      Initialize the Virtual Comm variables.
 *-------------------------------------------------------------------------*/
T_uezError VirtualCommInitialize(T_vcommCallbacks *aCallbacks)
{
    T_uezError error;

    // Copy over the callback information
    G_callbacks = *aCallbacks;

    // Note we are NOT busy processing an interrupt
//    G_isBusy = EFalse;

    // See if the USBDevice exists
    error = UEZDeviceTableFind(
                "USBDevice", 
                &G_usbDev);
    if (error != UEZ_ERROR_NONE)
        return error;

    // Find the workspace for all the routines (we'll just make it 
    // global for this cheap configuration).
    error = UEZDeviceTableGetWorkspace(
                G_usbDev, 
                (T_uezDeviceWorkspace **)&G_vcDevice);
    if (error != UEZ_ERROR_NONE)
        return error;

    error = UEZQueueCreate(VIRTUAL_COMM_QUEUE_IN_SIZE, 1, &G_vcommFifoIn);
    if (error != UEZ_ERROR_NONE)
        return error;

    error = UEZQueueCreate(VIRTUAL_COMM_QUEUE_OUT_SIZE, 1, &G_vcommFifoOut);
    if (error != UEZ_ERROR_NONE) {
        UEZQueueDelete(G_vcommFifoIn);
        return error;
    }

    // Turn on Nak interrupts on bulk input
    // so it polls the BulkIn for data when there is none
//    ((*G_vcDevice)->InterruptNakEnable)(G_vcDevice, USB_DEVICE_SET_MODE_INAK_BI);

    // Configure the device driver
    // Tell it our descriptor table
    ((*G_vcDevice)->Configure)(G_vcDevice, G_USBDescriptorTable);
    ((*G_vcDevice)->RegisterRequestTypeCallback)(
            G_vcDevice,
            USB_REQUEST_TYPE_CLASS,
            G_VirtualCommRequest,
            0,
            VirtualCommHandleClassRequest);
    ((*G_vcDevice)->RegisterEndpointCallback)(
            G_vcDevice, 
            ENDPOINT_IN(1), 
            VirtualCommInterruptIn);
    ((*G_vcDevice)->RegisterEndpointCallback)(
            G_vcDevice,
            ENDPOINT_IN(2),
            VirtualCommBulkIn);
    ((*G_vcDevice)->RegisterEndpointCallback)(
            G_vcDevice,
            ENDPOINT_OUT(5),
            VirtualCommBulkOut);

    // We are ready, let's initialize it and connect
    ((*G_vcDevice)->Initialize)(G_vcDevice);
    ((*G_vcDevice)->Connect)(G_vcDevice);

    // Now create a task that constantly process the VComm buffers
    error = UEZTaskCreate(
                VirtualCommMonitor,
                "VComm",
                256,
                0,
                UEZ_PRIORITY_HIGH,
                &G_vcTask);

    return error;
}

/*-------------------------------------------------------------------------*
 * Function:  VirtualCommPut
 *-------------------------------------------------------------------------*
 * Description:
 *      Send a character out the virtual comm driver.
 *      If no room is available in buffer, returns EFalse.  Otherwise, it
 *      returns ETrue
 * Inputs:
 *      TUInt8 c                 -- Character sent
 *      TUInt32 aTimeout         -- Time to wait for character to go out
 * Outputs:
 *      TBool                    -- ETrue if sent, else EFalse
 *-------------------------------------------------------------------------*/
TBool VirtualCommPut(TUInt8 c, TUInt32 aTimeout)
{
    // Try to stuff in the data, but don't block
    T_uezError error = UEZQueueSend(G_vcommFifoOut, &c, aTimeout);

    if (error == UEZ_ERROR_NONE) {
//        if (!G_isBusy) {
//            VirtualCommBulkIn(0, ENDPOINT_IN(2), 0);
//        }
        return ETrue;
    }

    return EFalse;
}

/*-------------------------------------------------------------------------*
 * Function:  VirtualCommGet
 *-------------------------------------------------------------------------*
 * Description:
 *      Try to get a character from the virtual comm driver.
 *      If no characters are available, returns -1.  Does not block.
 * Inputs:
 *      TUInt32 aTimeout         -- Time to wait for a character to appear
 * Outputs:
 *      TInt32                   -- Character from buffer, or -1
 *-------------------------------------------------------------------------*/
TInt32 VirtualCommGet(TUInt32 aTimeout)
{
    TUInt8 c;

    // Try to get data, but don't block
    T_uezError error;
    error = UEZQueueReceive(G_vcommFifoIn, &c, aTimeout);

    // If error, report none
    if (error == UEZ_ERROR_NONE) {
        return (TInt32)c;
    } else {
        return -1;
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  VirtualComm.c
 *-------------------------------------------------------------------------*/
