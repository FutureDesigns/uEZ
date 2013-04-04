/*-------------------------------------------------------------------------*
 * File:  GenericBulk.c
 *-------------------------------------------------------------------------*
 | Description:
 |      USB implementation of the device side of a virtual comm port
 |      following CDC rules.
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
#include <Source/Library/USBDevice/Generic/Bulk/GenericBulk.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define GENERIC_BULK_MAX_PACKET_SIZE 64

// Special CDC class descriptors:
#define GenBulk_CDC_INTERFACE                         0x24
#define GenBulk_CDC_ENDPOINT                          0x25

// CDC class commands:
#define	SET_LINE_CODING                             0x20
#define	GET_LINE_CODING                             0x21
#define	SET_CONTROL_LINE_STATE                      0x22

#define CONTROL_LINE_STATE_RTS                      (1<<1)
#define CONTROL_LINE_STATE_DTR                      (1<<0)

#define GENERIC_BULK_MAX_PACKET_SIZE                64

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
TBool GenericBulkHandleClassRequest(
        void *aWorkspace,
        T_USBSetupPacket *aSetup,
        TUInt16 *aLength,
        TUInt8 **aData);

void GenericBulkBulkIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);

void GenericBulkBulkOut(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);

void GenericBulkInterruptIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);

extern TBool MainCharReceivedMonitor(char aChar);
extern void MainCharEmptyOutput(void);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static TUInt8 G_GenBulkBuffer[64];
static TUInt8 G_GenericBulkRequest[8];

static T_uezQueue G_GenBulkFifoOut;   // out from this device
static T_uezQueue G_GenBulkFifoIn;    // in to this device

static T_GenBulkCallbacks G_callbacks;
static DEVICE_USB_DEVICE **G_ghDevice;
static T_uezDevice G_usbDev;
static T_uezTask G_ghTask;


const TUInt8 G_GenericBulk_USBDescriptorTable[] = {
    // device descriptor
    0x12,
    USB_DESCRIPTOR_DEVICE,
    USB_UINT16(0x0100),			// bcdUSB
    0x00,						// bDeviceClass
    0x00,						// bDeviceSubClass
    0x00,						// bDeviceProtocol
    USB_ENDPOINT0_PACKET_SIZE,			// bMaxPacketSize
    USB_UINT16(0x2416),			// idVendor
    USB_UINT16(0x0001),			// idProduct
    USB_UINT16(0x0100),			// bcdDevice
    0x01,						// iManufacturer
    0x02,						// iProduct
    0x03,						// iSerialNumber
    0x01,						// bNumConfigurations

    // configuration descriptor
    0x09,
    USB_DESCRIPTOR_CONFIGURATION,
    USB_UINT16(33),                                     // wTotalLength (9+9+7+7=33)
    0x01,						// bNumInterfaces
    0x01,						// bConfigurationValue
    0x00,						// iConfiguration
    0xA0,						// bmAttributes
    0x32,						// bMaxPower

    // control class interface
    0x09,
    USB_DESCRIPTOR_INTERFACE,
    0x00,						// bInterfaceNumber
    0x00,						// bAlternateSetting
    0x02,						// bNumEndPoints
    0xFF,						// bInterfaceClass (Vender)
    0x00,						// bInterfaceSubClass (TBD: ??? Boot Interface code)
    0x00,						// bInterfaceProtocol, 1=Keyboard
    0x00,						// iInterface

    // data EP in
    0x07,                                               // bLength
    USB_DESCRIPTOR_ENDPOINT,                            // bDescriptorType
    USB_ENDPOINT_ADDRESS(1, USB_ENDPOINT_ADDRESS_DIR_IN),   // bEndpointAddress
    USB_ENDPOINT_TYPE_BULK,                        // bmAttributes = interrupt
    USB_UINT16(GENERIC_BULK_MAX_PACKET_SIZE),                   // wMaxPacketSize
    0x0A,                                               // bInterval

    // data EP OUT
    0x07,
    USB_DESCRIPTOR_ENDPOINT,
    USB_ENDPOINT_ADDRESS(2, USB_ENDPOINT_ADDRESS_DIR_OUT),
    USB_ENDPOINT_TYPE_BULK,		        // bmAttributes = interrupt
    USB_UINT16(GENERIC_BULK_MAX_PACKET_SIZE),           // wMaxPacketSize
    0x0A,						// bInterval (10 ms)

    // string descriptors
    0x04,
    USB_DESCRIPTOR_STRING,
    USB_UINT16(0x0409),

    0x2A,    // string 0x01
    USB_DESCRIPTOR_STRING,
    'F', 0,
    'u', 0,
    't', 0,
    'u', 0,
    'r', 0,
    'e', 0,
    ' ', 0,
    'D', 0,
    'e', 0,
    's', 0,
    'i', 0,
    'g', 0,
    'n', 0,
    's', 0,
    ',', 0,
    ' ', 0,
    'I', 0,
    'n', 0,
    'c', 0,
    '.', 0,

    0x2E,   // string 0x02
    USB_DESCRIPTOR_STRING,
    'F', 0,
    'D', 0,
    'I', 0,
    ' ', 0,
    'S', 0,
    'e', 0,
    'r', 0,
    'i', 0,
    'a', 0,
    'l', 0,
    ' ', 0,
    'B', 0,
    'u', 0,
    'l', 0,
    'k', 0,
    ' ', 0,
    'D', 0,
    'r', 0,
    'i', 0,
    'v', 0,
    'e', 0,
    'r', 0,

    0x0C,   // string 0x03
    USB_DESCRIPTOR_STRING,
    'v', 0,
    '1', 0,
    '.', 0,
    '0', 0,
    '0', 0,

    // terminating zero
    0
};


/*-------------------------------------------------------------------------*
 * Function:  GenericBulkInterruptIn
 *-------------------------------------------------------------------------*
 * Description:
 *      A USB interrupt has polled.  Respond with a notification about
 *      the state of the serial lines.
 *      NOTE:  Currently this does not seem to work!
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void GenericBulkInterruptIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    // Do nothing
    PARAM_NOT_USED(aStatus);
    PARAM_NOT_USED(aWorkspace);

    G_GenBulkBuffer[0] = 0xA1;
    G_GenBulkBuffer[1] = 0x20;  // SERIAL_STATE
    G_GenBulkBuffer[2] = 0x00;  // wValue
    G_GenBulkBuffer[3] = 0x00;
    G_GenBulkBuffer[4] = 0x01;  // Interface
    G_GenBulkBuffer[5] = 0x00;
    G_GenBulkBuffer[6] = 0x02;  // wLength
    G_GenBulkBuffer[7] = 0x00;
    G_GenBulkBuffer[8] = 0x00;  // UART State
    G_GenBulkBuffer[9] = 0x00;
    ((*G_ghDevice)->Write)(G_ghDevice, aEndpoint, G_GenBulkBuffer, 10);
}

/*-------------------------------------------------------------------------*
 * Function:  GenericBulkBulkOut
 *-------------------------------------------------------------------------*
 * Description:
 *      Bulk virtual comm data has come out of the PC.  This data
 *      is put into the fifo.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void GenericBulkBulkOut(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    TInt16 i, length;
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aStatus);

    // Read data from endpoint
    length = (*G_ghDevice)->Read(
                G_ghDevice,
                aEndpoint,
                G_GenBulkBuffer,
                sizeof(G_GenBulkBuffer));

    if (length) {
        // Store all data until we are full.
        // TBD:  If the PC keeps sending us data and our
        //          buffer is full, we currently have no handshaking
        //          to handle overflow.
        for (i=1; i<=G_GenBulkBuffer[0]; i++)  {
            // Put in the queue one character at time quickly, until full.
            if (UEZQueueSend(G_GenBulkFifoIn, &G_GenBulkBuffer[i], 0) != UEZ_ERROR_NONE)
                break;
        }
    }

    // Setup a response
    GenericBulkBulkIn(aWorkspace, USB_ENDPOINT_ADDRESS(1, USB_ENDPOINT_ADDRESS_DIR_IN), aStatus);
}


/*-------------------------------------------------------------------------*
 * Function:  GenericBulkBulkIn
 *-------------------------------------------------------------------------*
 * Description:
 *      The PC is requesting data to be sent back to it.  Pull data
 *      output of the fifo and send it back up to the maximum size
 *      packet.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void GenericBulkBulkIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    TInt16 i, length;
    TUInt8 c;
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aStatus);

    // Pull out data up to the maximum size of a packet
    memset(G_GenBulkBuffer, 0xFF, sizeof(G_GenBulkBuffer));
    for (i=0; i<(GENERIC_BULK_MAX_PACKET_SIZE-1); i++)  {
        // Only process if we have data
        if (UEZQueueReceive(G_GenBulkFifoOut, &G_GenBulkBuffer[i+1], 0) != UEZ_ERROR_NONE)
            break;
    }
    length = i;
    G_GenBulkBuffer[0] = length;

    // Send what we have back (first byte is the length of the rest of the bytes)
    (*G_ghDevice)->Write(G_ghDevice, aEndpoint, G_GenBulkBuffer, sizeof(G_GenBulkBuffer));

    // Check to see if there is more in the queue and if not
    // report we are empty (if we have a callback)
    if (G_callbacks.iGenBulkEmptyOutput) {
        if (UEZQueuePeek(G_GenBulkFifoOut, &c, 0) == UEZ_ERROR_TIMEOUT) {
            G_callbacks.iGenBulkEmptyOutput(G_callbacks.iCallbackWorkspace);
        }
    }
}

void GenericBulkBulkInComplete(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
}

/*-------------------------------------------------------------------------*
 * Function:  GenericBulkHandleClassRequest
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
TBool GenericBulkHandleClassRequest(
                void *aWorkspace,
                T_USBSetupPacket *aSetup,
                TUInt16 *aLength,
                TUInt8 **aData)
{
    PARAM_NOT_USED(aWorkspace);

    switch (aSetup->iRequest) {
        default:
            return EFalse;
    }
#if (COMPILER_TYPE!=IAR)
    return ETrue;
#endif
}

TUInt32 GenericBulkMonitor(T_uezTask aMyTask, void *aParameters)
{
    PARAM_NOT_USED(aParameters);
    PARAM_NOT_USED(aMyTask);

    // Just constantly process endpoint data
    for (;;)  {
        ((*G_ghDevice)->ProcessEndpoints)(G_ghDevice, UEZ_TIMEOUT_INFINITE);
    }
}

/*-------------------------------------------------------------------------*
 * Function:  GenericBulkInitialize
 *-------------------------------------------------------------------------*
 * Description:
 *      Initialize the Virtual Comm variables.
 *-------------------------------------------------------------------------*/
T_uezError GenericBulkInitialize(T_GenBulkCallbacks *aCallbacks)
{
    T_uezError error;

    // Copy over the callback information
    G_callbacks = *aCallbacks;

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
                (T_uezDeviceWorkspace **)&G_ghDevice);
    if (error != UEZ_ERROR_NONE)
        return error;

    error = UEZQueueCreate(GENERIC_BULK_QUEUE_IN_SIZE, 1, &G_GenBulkFifoIn);
    if (error != UEZ_ERROR_NONE)
        return error;

    error = UEZQueueCreate(GENERIC_BULK_QUEUE_OUT_SIZE, 1, &G_GenBulkFifoOut);
    if (error != UEZ_ERROR_NONE) {
        UEZQueueDelete(G_GenBulkFifoIn);
        return error;
    }

    // Turn on Nak interrupts on bulk input
    // so it polls the BulkIn for data when there is none
//    ((*G_ghDevice)->InterruptNakEnable)(G_ghDevice, USB_DEVICE_SET_MODE_INAK_BI);

    // Configure the device driver
    // Tell it our descriptor table
    ((*G_ghDevice)->Configure)(G_ghDevice, G_GenericBulk_USBDescriptorTable);
    ((*G_ghDevice)->RegisterRequestTypeCallback)(
            G_ghDevice,
            USB_REQUEST_TYPE_CLASS,
            G_GenericBulkRequest,
            0,
            GenericBulkHandleClassRequest);
#if 0
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_IN(1),
            GenericBulkInterruptIn);
#endif
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_IN(1),
            GenericBulkBulkInComplete);
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_OUT(2),
            GenericBulkBulkOut);

    // We are ready, let's initialize it and connect
    ((*G_ghDevice)->Initialize)(G_ghDevice);
    ((*G_ghDevice)->Connect)(G_ghDevice);

    // Now create a task that constantly process the GenBulk buffers
    error = UEZTaskCreate(
                GenericBulkMonitor,
                "GenBulk",
                256,
                0,
                UEZ_PRIORITY_HIGH,
                &G_ghTask);

    return error;
}

/*-------------------------------------------------------------------------*
 * Function:  GenericBulkPut
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
TBool GenericBulkPut(TUInt8 c, TUInt32 aTimeout)
{
    // Try to stuff in the data, but don't block
    T_uezError error = UEZQueueSend(G_GenBulkFifoOut, &c, aTimeout);

    if (error == UEZ_ERROR_NONE) {
        return ETrue;
    }

    return EFalse;
}

/*-------------------------------------------------------------------------*
 * Function:  GenericBulkGet
 *-------------------------------------------------------------------------*
 * Description:
 *      Try to get a character from the virtual comm driver.
 *      If no characters are available, returns -1.  Does not block.
 * Inputs:
 *      TUInt32 aTimeout         -- Time to wait for a character to appear
 * Outputs:
 *      TInt32                   -- Character from buffer, or -1
 *-------------------------------------------------------------------------*/
TInt32 GenericBulkGet(TUInt32 aTimeout)
{
    TUInt8 c;

    // Try to get data, but don't block
    T_uezError error;
    error = UEZQueueReceive(G_GenBulkFifoIn, &c, aTimeout);

    // If error, report none
    if (error == UEZ_ERROR_NONE) {
        return (TInt32)c;
    } else {
        return -1;
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  GenericBulk.c
 *-------------------------------------------------------------------------*/
