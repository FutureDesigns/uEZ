/*-------------------------------------------------------------------------*
 * File:  GenericHID.c
 *-------------------------------------------------------------------------*
 | Description:
 |      USB implementation of the device side of a virtual comm port
 |      following CDC rules.
 |
 |      NOTE: This code has not been tested yet!  I've got caps lock
 |      working but it needs more work!
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include "Device/USBDevice.h"
#include <Source/Library/USBDevice/Generic/HID/GenericHID.h>
#include <uEZDeviceTable.h>
#include <stdio.h>

#define HIGH_BYTE(x)        (((x)>>8) & 0xFF)

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define GenHID_MAX_PACKET_SIZE 64

// Special CDC class descriptors:
#define GenHID_CDC_INTERFACE                         0x24
#define GenHID_CDC_ENDPOINT                          0x25

// CDC class commands:
#define	SET_LINE_CODING                             0x20
#define	GET_LINE_CODING                             0x21
#define	SET_CONTROL_LINE_STATE                      0x22

#define CONTROL_LINE_STATE_RTS                      (1<<1)
#define CONTROL_LINE_STATE_DTR                      (1<<0)

    #define USB_DESCRIPTOR_HID                      0x21
    #define USB_DESCRIPTOR_REPORT                   0x22

#define GHID_MAX_PACKET_SIZE                64

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt8 iLEDs;
} __packed T_hidKeyboardOutput;

typedef struct {
    TUInt8 iModifiers;
    TUInt8 iPressedKey;
} __packed T_hidKeyboardInput;


/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
TBool GenericHIDHandleClassRequest(
        void *aWorkspace,
        T_USBSetupPacket *aSetup,
        TUInt16 *aLength,
        TUInt8 **aData);

void GenericHIDBulkIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);

void GenericHIDBulkOut(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);

void GenericHIDInterruptIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);

void GenericHIDInterruptOut(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);

extern TBool MainCharReceivedMonitor(char aChar);
extern void MainCharEmptyOutput(void);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static TUInt8 G_ghidBuffer[64];
static TUInt8 G_GenericHIDRequest[8];

static T_uezQueue G_ghidFifoOut;   // out from this device
static T_uezQueue G_ghidFifoIn;    // in to this device

static T_GenHIDCallbacks G_callbacks;
static DEVICE_USB_DEVICE **G_ghDevice;
static T_uezDevice G_usbDev;
static T_uezTask G_ghTask;

// HID stuff
static TUInt8 G_hidKeyboardIdle=0;
static T_hidKeyboardOutput G_hidKeyboardOutput;
static T_hidKeyboardInput G_hidKeyboardInput;
TBool G_capsOn;

//---------------------------------------------------------------------------
// Global pages
//---------------------------------------------------------------------------
// Current usage page
#define HID_GLOBAL_USAGE_PAGE            0x04
// Minimum value that a variable or array item will report
#define HID_GLOBAL_LOGICAL_MINIMUM       0x14
// Maximum value that a variable or array item will report
#define HID_GLOBAL_LOGICAL_MAXIMUM       0x24
// Minimum value for the physical extent of a variable item
#define HID_GLOBAL_PHYSICAL_MINIMUM      0x34
// Maximum value for the physical extent of a variable item
#define HID_GLOBAL_PHYSICAL_MAXIMUM      0x44
// Value of the unit exponent in base 10
#define HID_GLOBAL_UNIT_EXPONENT         0x54
// Unit values
#define HID_GLOBAL_UNIT                  0x64
// Size of the report fiels in bits
#define HID_GLOBAL_REPORT_SIZE           0x74
// Specifies the report ID
#define HID_GLOBAL_REPORT_ID             0x84
// Number of data fields for an item
#define HID_GLOBAL_REPORT_COUNT          0x94
// Places a copy of the global item state table on the stack
#define HID_GLOBAL_PUSH                  0xA4
// Replaces the item state table with the top structure from the stack
#define HID_GLOBAL_POP                   0xB4

//---------------------------------------------------------------------------
// Local items
//---------------------------------------------------------------------------
// Suggested usage for an item or collection
#define HID_LOCAL_USAGE                  0x08
// Defines the starting usage associated with an array or bitmap
#define HID_LOCAL_USAGE_MINIMUM          0x18
// Defines the ending usage associated with an array or bitmap
#define HID_LOCAL_USAGE_MAXIMUM          0x28
// Determines the body part used for a control
#define HID_LOCAL_DESIGNATOR_INDEX       0x38
// Defines the index of the starting designator associated with an array or bitmap
#define HID_LOCAL_DESIGNATOR_MINIMUM     0x48
// Defines the index of the ending designator associated with an array or bitmap
#define HID_LOCAL_DESIGNATOR_MAXIMUM     0x58
// String index for a string descriptor
#define HID_LOCAL_STRING_INDEX           0x78
// Specifies the first string index when assigning a group of sequential
//      strings to controls in an array or bitmap
#define HID_LOCAL_STRING_MINIMUM         0x88
// Specifies the last string index when assigning a group of sequential
//      strings to controls in an array or bitmap
#define HID_LOCAL_STRING_MAXIMUM         0x98
// Defines the beginning or end of a set of local items
#define HID_LOCAL_DELIMITER              0xA8

//---------------------------------------------------------------------------
// Usage pages
//---------------------------------------------------------------------------
#define HID_USAGE_PAGE_UNDEFINED            0x00
#define HID_USAGE_PAGE_GENERIC_DESKTOP      0x01
#define HID_USAGE_PAGE_SIMULATION           0x02
#define HID_USAGE_PAGE_VR                   0x03
#define HID_USAGE_PAGE_SPORT                0x04
#define HID_USAGE_PAGE_GAME                 0x05
#define HID_USAGE_PAGE_GENERIC_DEVICE       0x06
#define HID_USAGE_PAGE_KEYBOARD_KEYPAD      0x07
#define HID_USAGE_PAGE_LEDS                 0x08
#define HID_USAGE_PAGE_BUTTON               0x09
#define HID_USAGE_PAGE_ORDINAL              0x0A
#define HID_USAGE_PAGE_TELEPHONY            0x0B
#define HID_USAGE_PAGE_CONSUMER             0x0C
#define HID_USAGE_PAGE_DIGITIZER            0x0D

//---------------------------------------------------------------------------
// USB Physical Interface Device definitions
// (force feedback and related devices)
//---------------------------------------------------------------------------
#define HID_USAGE_PAGE_PID                  0x0F
#define HID_USAGE_PAGE_UNICODE              0x10
#define HID_USAGE_PAGE_ALPHANUM_DISPLAY     0x14
#define HID_USAGE_PAGE_MEDICAL_INSTRUMENTS  0x40
#define HID_USAGE_PAGE_MONITOR_PAGES        0x80
#define HID_USAGE_PAGE_POWER_PAGES          0x84
#define HID_USAGE_PAGE_BARCODE_SCANNER      0x8C
#define HID_USAGE_PAGE_SCALE_PAGE           0x8D
#define HID_USAGE_PAGE_MSR                  0x8E
#define HID_USAGE_PAGE_CAMERA_CONTROL       0x90
#define HID_USAGE_PAGE_ARCADE               0x91

//---------------------------------------------------------------------------
// Generic Desktop Usages
//---------------------------------------------------------------------------
#define HID_USAGE_POINTER                   0x01
#define HID_USAGE_MOUSE                     0x02
#define HID_USAGE_JOYSTICK                  0x04
#define HID_USAGE_GAMEPAD                   0x05
#define HID_USAGE_KEYBOARD                  0x06
#define HID_USAGE_KEYPAD                    0x07
#define HID_USAGE_MULTIAXIS                 0x08
#define HID_USAGE_X                         0x30
#define HID_USAGE_Y                         0x31

//---------------------------------------------------------------------------
// Report descriptor
//---------------------------------------------------------------------------
#define HID_MAIN_INPUT                   0x80
#define HID_MAIN_OUTPUT                  0x90
#define HID_MAIN_FEATURE                 0xB0
#define HID_MAIN_COLLECTION              0xA0
#define HID_MAIN_ENDCOLLECTION           0xC0

//---------------------------------------------------------------------------
// Input, Output, and Features Items
//---------------------------------------------------------------------------
#define HID_DATA                     (0 << 0)
#define HID_CONSTANT                 (1 << 0)
#define HID_ARRAY                    (0 << 1)
#define HID_VARIABLE                 (1 << 1)
#define HID_ABSOLUTE                 (0 << 2)
#define HID_RELATIVE                 (1 << 2)
#define HID_NOWRAP                   (0 << 3)
#define HID_WRAP                     (1 << 3)
#define HID_LINEAR                   (0 << 4)
#define HID_NONLINEAR                (1 << 4)
#define HID_PREFERREDSTATE           (0 << 5)
#define HID_NOPREFERRED              (1 << 5)
#define HID_NONULLPOSITION           (0 << 6)
#define HID_NULLSTATE                (1 << 6)
#define HID_NONVOLATILE              (0 << 7)
#define HID_VOLATILE                 (1 << 7)
#define HID_BITFIELD                 (0 << 8)
#define HID_BUFFEREDBYTES            (1 << 8)

//---------------------------------------------------------------------------
// Collection, End Collection Items
//---------------------------------------------------------------------------
#define HID_COLLECTION_PHYSICAL          0x00
#define HID_COLLECTION_APPLICATION       0x01
#define HID_COLLECTION_LOGICAL           0x02
#define HID_COLLECTION_REPORT            0x03
#define HID_COLLECTION_NAMEDARRAY        0x04
#define HID_COLLECTION_HID_USAGESWITCH   0x05
#define HID_COLLECTION_HID_USAGEMODIFIER 0x06

#define HID_NULL(field)                 ((field)|0x00)
#define HID_BYTE(field, data)           ((field)|0x01), (data)
#define HID_WORD(field, data)           ((field)|0x02), (((data)&0xFF00)>>8), ((data)&0xFF)

//---------------------------------------------------------------------------
// HID Keyboard Class specific Request Codes
//---------------------------------------------------------------------------
#define HID_GET_REPORT              0x01    // Mandatory for all devices
#define HID_GET_IDLE                0x02
#define HID_GET_PROTOCOL            0x03    // Mandatory for boot devices
#define HID_SET_REPORT              0x09
#define HID_SET_IDLE                0x0A
#define HID_SET_PROTOCOL            0x0B    // Mandatory for boot devices

//---------------------------------------------------------------------------
// HID report types
//---------------------------------------------------------------------------
#define HID_INPUT_REPORT                1
#define HID_OUTPUT_REPORT               2
#define HID_FEATURE_REPORT              3

TUInt8 G_USBHIDKeyboardReport[] = {
    HID_BYTE(HID_GLOBAL_USAGE_PAGE, HID_USAGE_PAGE_GENERIC_DESKTOP),
    HID_BYTE(HID_LOCAL_USAGE, HID_USAGE_KEYBOARD),
    HID_BYTE(HID_MAIN_COLLECTION, HID_COLLECTION_APPLICATION),
        // IN: Keyboard Modifier byte
        HID_BYTE(HID_GLOBAL_REPORT_SIZE, 1),
        HID_BYTE(HID_GLOBAL_REPORT_COUNT, 8),
        HID_BYTE(HID_GLOBAL_USAGE_PAGE, HID_USAGE_PAGE_KEYBOARD_KEYPAD),
        HID_BYTE(HID_LOCAL_USAGE_MINIMUM, 224),
        HID_BYTE(HID_LOCAL_USAGE_MAXIMUM, 231),
        HID_BYTE(HID_GLOBAL_LOGICAL_MINIMUM, 0),
        HID_BYTE(HID_GLOBAL_LOGICAL_MAXIMUM, 1),
        HID_BYTE(HID_MAIN_INPUT, HID_DATA|HID_VARIABLE|HID_ABSOLUTE),

        // IN: 1 key being pressed
        HID_BYTE(HID_GLOBAL_REPORT_COUNT, 1),
        HID_BYTE(HID_GLOBAL_REPORT_SIZE, 8),
        HID_BYTE(HID_GLOBAL_LOGICAL_MINIMUM, 0),
        HID_BYTE(HID_GLOBAL_LOGICAL_MAXIMUM, 101),
        HID_BYTE(HID_GLOBAL_USAGE_PAGE, HID_USAGE_PAGE_KEYBOARD_KEYPAD),
        HID_BYTE(HID_LOCAL_USAGE_MINIMUM, 0),
        HID_BYTE(HID_LOCAL_USAGE_MAXIMUM, 101),
        HID_BYTE(HID_MAIN_INPUT, HID_DATA|HID_ARRAY),

        // OUT: LEDs (6 bits are standard leds, 2 bits are padding)
        HID_BYTE(HID_GLOBAL_REPORT_COUNT, 6),
        HID_BYTE(HID_GLOBAL_REPORT_SIZE, 1),
        HID_BYTE(HID_GLOBAL_USAGE_PAGE, HID_USAGE_PAGE_LEDS),
        HID_BYTE(HID_LOCAL_USAGE_MINIMUM, 1),
        HID_BYTE(HID_LOCAL_USAGE_MAXIMUM, 6),
        HID_BYTE(HID_MAIN_OUTPUT, HID_DATA|HID_VARIABLE|HID_ABSOLUTE),

        HID_BYTE(HID_GLOBAL_REPORT_COUNT, 1),
        HID_BYTE(HID_GLOBAL_REPORT_SIZE, 2),
        HID_BYTE(HID_MAIN_OUTPUT, HID_CONSTANT),
    HID_NULL(HID_MAIN_ENDCOLLECTION),
};

const TUInt8 G_GenericHID_USBDescriptorTable[] = {
    // device descriptor
    0x12,
    USB_DESCRIPTOR_DEVICE,
    USB_UINT16(0x0100),			// bcdUSB
    0x00,						// bDeviceClass
    0x00,						// bDeviceSubClass
    0x00,						// bDeviceProtocol
    USB_ENDPOINT0_PACKET_SIZE,			// bMaxPacketSize
    USB_UINT16(0xFFFF),			// idVendor
    USB_UINT16(0x0001),			// idProduct
    USB_UINT16(0x0100),			// bcdDevice
    0x01,						// iManufacturer
    0x02,						// iProduct
    0x03,						// iSerialNumber
    0x01,						// bNumConfigurations

    // configuration descriptor
    0x09,
    USB_DESCRIPTOR_CONFIGURATION,
    USB_UINT16(41),                                     // wTotalLength (9+9+9+7+7=41)
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
    0x03,						// bInterfaceClass (HID code)
    0x00,						// bInterfaceSubClass (TBD: ??? Boot Interface code)
    0x00,						// bInterfaceProtocol, 1=Keyboard
    0x00,						// iInterface

    // HID Descriptor
    0x09,                                               // bLength
    USB_DESCRIPTOR_HID,                                 // bDescriptorType
    USB_UINT16(0x101),                                  // bcdHID (HID Class Specification release number)
    0x00,                                               // bCountryCode
    0x01,                                               // Number of HID class descriptors to follow
    USB_DESCRIPTOR_REPORT,                              // bDescriptorType (report descriptor type)
    USB_UINT16(sizeof(G_USBHIDKeyboardReport)),         // Report descriptor length

    // data EP in
    0x07,                                               // bLength
    USB_DESCRIPTOR_ENDPOINT,                            // bDescriptorType
    USB_ENDPOINT_ADDRESS(1, USB_ENDPOINT_ADDRESS_DIR_IN),   // bEndpointAddress
    USB_ENDPOINT_TYPE_INTERRUPT,                        // bmAttributes = interrupt
    USB_UINT16(GHID_MAX_PACKET_SIZE),                   // wMaxPacketSize
    0x0A,                                               // bInterval

    // data EP OUT
    0x07,
    USB_DESCRIPTOR_ENDPOINT,
    USB_ENDPOINT_ADDRESS(2, USB_ENDPOINT_ADDRESS_DIR_OUT),
    USB_ENDPOINT_TYPE_INTERRUPT,						// bmAttributes = interrupt
    USB_UINT16(GHID_MAX_PACKET_SIZE),                   // wMaxPacketSize
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

    0x16,   // string 0x02
    USB_DESCRIPTOR_STRING,
    'H', 0,
    'I', 0,
    'D', 0,
    '-', 0,
    'D', 0,
    'e', 0,
    'v', 0,
    'i', 0,
    'c', 0,
    'e', 0,

    0x0C,   // string 0x03
    USB_DESCRIPTOR_STRING,
    'v', 0,
    '0', 0,
    '.', 0,
    '0', 0,
    '0', 0,

    // terminating zero
    0
};


/*-------------------------------------------------------------------------*
 * Function:  GenericHIDInterruptIn
 *-------------------------------------------------------------------------*
 * Description:
 *      A USB interrupt has polled.  Respond with a notification about
 *      the state of the serial lines.
 *      NOTE:  Currently this does not seem to work!
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void GenericHIDInterruptIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    // Do nothing
    PARAM_NOT_USED(aStatus);
    PARAM_NOT_USED(aWorkspace);

    ((*G_ghDevice)->Write)(
        G_ghDevice,
        aEndpoint,
        (TUInt8 *)&G_hidKeyboardInput,
        sizeof(G_hidKeyboardInput));
}

/*-------------------------------------------------------------------------*
 * Function:  GenericHIDInterruptIn
 *-------------------------------------------------------------------------*
 * Description:
 *      A USB interrupt has polled.  Respond with a notification about
 *      the state of the serial lines.
 *      NOTE:  Currently this does not seem to work!
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void GenericHIDInterruptOut(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    // Do nothing
    PARAM_NOT_USED(aStatus);
    PARAM_NOT_USED(aWorkspace);

    ((*G_ghDevice)->Read)(
                G_ghDevice,
                aEndpoint,
                (TUInt8 *)&G_hidKeyboardOutput,
                sizeof(T_hidKeyboardOutput));
    if (G_hidKeyboardOutput.iLEDs & 0x02) {  // caps lock
        G_capsOn = ETrue;
    } else {
        G_capsOn = EFalse;
    }
}

/*-------------------------------------------------------------------------*
 * Function:  GenericHIDBulkOut
 *-------------------------------------------------------------------------*
 * Description:
 *      Bulk virtual comm data has come out of the PC.  This data
 *      is put into the fifo.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void GenericHIDBulkOut(
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
                G_ghidBuffer,
                sizeof(G_ghidBuffer));

    // Store all data until we are full.
    // TBD:  If the PC keeps sending us data and our
    //          buffer is full, we currently have no handshaking
    //          to handle overflow.
    for (i=0; i<length; i++)  {
        // Put in the queue one character at time quickly, until full.
        if (UEZQueueSend(G_ghidFifoIn, &G_ghidBuffer[i], 0) != UEZ_ERROR_NONE)
            break;
    }
}


/*-------------------------------------------------------------------------*
 * Function:  GenericHIDBulkIn
 *-------------------------------------------------------------------------*
 * Description:
 *      The PC is requesting data to be sent back to it.  Pull data
 *      output of the fifo and send it back up to the maximum size
 *      packet.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void GenericHIDBulkIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    TInt16 i, length;
    TUInt8 c;
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aStatus);

    // Pull out data up to the maximum size of a packet
    for (i=0; i<GenHID_MAX_PACKET_SIZE; i++)  {
        // Only process if we have data
        if (UEZQueueReceive(G_ghidFifoOut, &G_ghidBuffer[i], 0) != UEZ_ERROR_NONE)
            break;
    }
    length = i;

    // If we have data, send it back.
    // If we do not have data, don't respond.  Responding has
    // been shown to cause continual updates and USB bandwidth issues.
    // Effectively a NAK is created.
    if (length > 0) {
        (*G_ghDevice)->Write(G_ghDevice, aEndpoint, G_ghidBuffer, length);
    } else {
    }

    // Check to see if there is more in the queue and if not
    // report we are empty (if we have a callback)
    if (G_callbacks.iGHIDEmptyOutput) {
        if (UEZQueuePeek(G_ghidFifoOut, &c, 0) == UEZ_ERROR_TIMEOUT) {
            G_callbacks.iGHIDEmptyOutput();
        }
    }
}

/*-------------------------------------------------------------------------*
 * Function:  GenericHIDHandleClassRequest
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
TBool GenericHIDHandleClassRequest(
                void *aWorkspace,
                T_USBSetupPacket *aSetup,
                TUInt16 *aLength,
                TUInt8 **aData)
{
    PARAM_NOT_USED(aWorkspace);

printf("{GHID.Class $%02X}", aSetup->iRequest);
    switch (aSetup->iRequest) {
        // set line coding
        case HID_GET_IDLE:
            *aData = &G_hidKeyboardIdle;
            *aLength = 1;
            break;

        case HID_SET_IDLE:
            G_hidKeyboardIdle = HIGH_BYTE(aSetup->iValue);
            break;

        case HID_GET_REPORT:
            if (HIGH_BYTE(aSetup->iValue) == HID_INPUT_REPORT)  {
                *aData = (TUInt8 *)&G_hidKeyboardInput;
                *aLength = sizeof(G_hidKeyboardInput);
            } else if (HIGH_BYTE(aSetup->iValue) == HID_OUTPUT_REPORT)  {
                *aData = (TUInt8 *)&G_hidKeyboardOutput;
                *aLength = sizeof(G_hidKeyboardOutput);
            }
            break;

        case HID_SET_REPORT:
            if (HIGH_BYTE(aSetup->iValue) == HID_INPUT_REPORT)  {
                // TBD: What really goes here?  Do we need it?
                *aData = (TUInt8 *)&G_hidKeyboardInput;
                *aLength = sizeof(G_hidKeyboardInput);
            } else if (HIGH_BYTE(aSetup->iValue) == HID_OUTPUT_REPORT)  {
                // TBD: What really goes here?  Do we need it?
                *aData = (TUInt8 *)&G_hidKeyboardOutput;
                *aLength = sizeof(G_hidKeyboardOutput);
            }
            break;

        case USB_DEVICE_REQUEST_GET_DESCRIPTOR:
            if (HIGH_BYTE(aSetup->iValue) == USB_DESCRIPTOR_REPORT)  {
                *aData = G_USBHIDKeyboardReport;
                *aLength = sizeof(G_USBHIDKeyboardReport);
                return ETrue;
            }
            break;

        default:
            return EFalse;
    }
    return ETrue;
}

TUInt32 GenericHIDMonitor(T_uezTask aMyTask, void *aParameters)
{
    PARAM_NOT_USED(aParameters);
    PARAM_NOT_USED(aMyTask);

    // Just constantly process endpoint data
    for (;;)  {
        ((*G_ghDevice)->ProcessEndpoints)(G_ghDevice, UEZ_TIMEOUT_INFINITE);
    }
}

/*-------------------------------------------------------------------------*
 * Function:  GenericHIDInitialize
 *-------------------------------------------------------------------------*
 * Description:
 *      Initialize the Virtual Comm variables.
 *-------------------------------------------------------------------------*/
T_uezError GenericHIDInitialize(T_GenHIDCallbacks *aCallbacks)
{
    T_uezError error;

    G_capsOn = EFalse;

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

    error = UEZQueueCreate(GENERIC_HID_QUEUE_IN_SIZE, 1, &G_ghidFifoIn);
    if (error != UEZ_ERROR_NONE)
        return error;

    error = UEZQueueCreate(GENERIC_HID_QUEUE_OUT_SIZE, 1, &G_ghidFifoOut);
    if (error != UEZ_ERROR_NONE) {
        UEZQueueDelete(G_ghidFifoIn);
        return error;
    }

    // Turn on Nak interrupts on bulk input
    // so it polls the BulkIn for data when there is none
//    ((*G_ghDevice)->InterruptNakEnable)(G_ghDevice, USB_DEVICE_SET_MODE_INAK_BI);

    // Configure the device driver
    // Tell it our descriptor table
    ((*G_ghDevice)->Configure)(G_ghDevice, G_GenericHID_USBDescriptorTable);
    ((*G_ghDevice)->RegisterRequestTypeCallback)(
            G_ghDevice,
            USB_REQUEST_TYPE_CLASS,
            G_GenericHIDRequest,
            0,
            GenericHIDHandleClassRequest);
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_IN(1),
            GenericHIDInterruptIn);
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_OUT(2),
            GenericHIDInterruptOut);

    // We are ready, let's initialize it and connect
    ((*G_ghDevice)->Initialize)(G_ghDevice);
    ((*G_ghDevice)->Connect)(G_ghDevice);

    // Now create a task that constantly process the GenHID buffers
    error = UEZTaskCreate(
                GenericHIDMonitor,
                "GenHID",
                200,
                0,
                UEZ_PRIORITY_HIGH,
                &G_ghTask);

    return error;
}

/*-------------------------------------------------------------------------*
 * Function:  GenericHIDPut
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
TBool GenericHIDPut(TUInt8 c, TUInt32 aTimeout)
{
    // Try to stuff in the data, but don't block
    T_uezError error = UEZQueueSend(G_ghidFifoOut, &c, aTimeout);

    if (error == UEZ_ERROR_NONE) {
        return ETrue;
    }

    return EFalse;
}

/*-------------------------------------------------------------------------*
 * Function:  GenericHIDGet
 *-------------------------------------------------------------------------*
 * Description:
 *      Try to get a character from the virtual comm driver.
 *      If no characters are available, returns -1.  Does not block.
 * Inputs:
 *      TUInt32 aTimeout         -- Time to wait for a character to appear
 * Outputs:
 *      TInt32                   -- Character from buffer, or -1
 *-------------------------------------------------------------------------*/
TInt32 GenericHIDGet(TUInt32 aTimeout)
{
    TUInt8 c;

    // Try to get data, but don't block
    T_uezError error;
    error = UEZQueueReceive(G_ghidFifoIn, &c, aTimeout);

    // If error, report none
    if (error == UEZ_ERROR_NONE) {
        return (TInt32)c;
    } else {
        return -1;
    }
}

/*-------------------------------------------------------------------------*
 * Function:  HIDKeyboardOutput
 *-------------------------------------------------------------------------*
 * Description:
 *      Sends the current keyboard configuration to the USB buffer
 *      for output.
 *-------------------------------------------------------------------------*/
void HIDKeyboardOutput(void)
{
    (*G_ghDevice)->Write(
        G_ghDevice,
        ENDPOINT_IN(1),
        (TUInt8 *)&G_hidKeyboardInput,
        sizeof(T_hidKeyboardInput));
}

/*-------------------------------------------------------------------------*
 * Function:  HIDKeyboardUpdate
 *-------------------------------------------------------------------------*
 * Description:
 *      Updates the keyboard pressed keys
 *-------------------------------------------------------------------------*/
void HIDKeyboardUpdate(void)
{
    TUInt32 i;

    // All buttons are on P0.xx
    #define NUM_BUTTONS 8
    typedef struct {
        TUInt8 portPin;
        TUInt8 keypress;
    } T_button;
#if 1
    const T_button G_buttons[NUM_BUTTONS] = {
        {   6,  0x04,   },  // #1 - P0.6,   MOSI        a
        {   4,  0x05,   },  // #2 - P0.4,   SCLK        b
        {   17, 0x06,   },  // #3 - P0.17,  PSEN        c
        {   8,  0x07,   },  // #4 - P0.8,   HOST_TXDbfca    d
        {   9,  0x08,   },  // #5 - P0.9,   HOST_RXD    e
        {   5,  0x09,   },  // #6 - P0.5,   MISO        f
        {   3,  0x0A,   },  // #7 - P0.3,   PDA         g
        {   2,  0x0B,   },  // #8 - P0.2,   PCL         h
    };
#else
    const T_button G_buttons[NUM_BUTTONS] = {
        {   6,  0x70,   },  // #1 - P0.6,   MOSI
        {   4,  0x71,   },  // #2 - P0.4,   SCLK
        {   17, 0x72,   },  // #3 - P0.17,  PSEN
        {   8,  0x73,   },  // #4 - P0.8,   HOST_TXD
        {   9,  0x74,   },  // #5 - P0.9,   HOST_RXD
        {   5,  0x75,   },  // #6 - P0.5,   MISO
        {   3,  0x76,   },  // #7 - P0.3,   PDA
        {   2,  0x77,   },  // #8 - P0.2,   PCL
    };
#endif

    // Latch P0 at once
    static TUInt32 port = (1<<6);
    TUInt8 wasKey;
    TUInt8 newKey;
	TUInt32 fell;

    // Toggle the 'a' pin
    port ^= (1<<6);

    // Let's find the pins that fell (changed pins and'd with not port)
//    TUInt32 fell = (port^lastPort)&(~port);
    fell = (1<<6);

    // Assume no keys are pressed
    wasKey = G_hidKeyboardInput.iPressedKey;
    newKey = 0;

    // Check which key(s) are pressed.  If more than one, only
    // use the last one.
    for (i=0; i<NUM_BUTTONS; i++)  {
        if ((fell & (1<<G_buttons[i].portPin))!=0)  {
            // This button is being pressed.  Record
            newKey = G_buttons[i].keypress;
        }
    }

    // Output the new settings (if changed)
    if (wasKey != newKey)  {
        G_hidKeyboardInput.iPressedKey = newKey;
        HIDKeyboardOutput();
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  GenericHID.c
 *-------------------------------------------------------------------------*/
