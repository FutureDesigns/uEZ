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
#include <ctype.h>
#include <uEZ.h>
#include "VirtualComm.h"
#include <uEZPacked.h>
#include <uEZGPIO.h>
#include <HAL/Interrupt.h>
#include <Source/Library/LPCOpen/LPCUSBLib/Drivers/USB/Class/Device/CDCClassDevice.h>
#include <Source/Library/LPCOpen/LPCUSBLib/Drivers/USB/USB.h>
#include <Source/Library/LPCOpen/LPCUSBLib/LPCUSBLib_uEZ.h>


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define VCOMM_MAX_PACKET_SIZE 64 // 16

// Special CDC class descriptors:
#define VCOMM_CDC_INTERFACE                         0x24
#define VCOMM_CDC_ENDPOINT                          0x25

// CDC class commands:
#define SET_LINE_CODING                             0x20
#define GET_LINE_CODING                             0x21
#define SET_CONTROL_LINE_STATE                      0x22

#define CONTROL_LINE_STATE_RTS                      (1<<1)
#define CONTROL_LINE_STATE_DTR                      (1<<0)

/** Endpoint address for the CDC control interface event notification endpoint. */
#define CDC_NOTIFICATION_EPADDR        (ENDPOINT_DIR_IN | 2)

/** Endpoint address for the CDC data interface TX (data IN) endpoint. */
#define CDC_TX_EPADDR                  (ENDPOINT_DIR_IN | 3)

/** Endpoint address for the CDC data interface RX (data OUT) endpoint. */
#define CDC_RX_EPADDR                  (ENDPOINT_DIR_OUT | 4)

/** Size of the CDC data interface TX and RX data endpoint banks, in bytes. */
#define CDC_TXRX_EPSIZE                64 // 16

/** Size of the CDC control interface notification endpoint bank, in bytes. */
#define CDC_NOTIFICATION_EPSIZE        64 // 8


/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
PACK_STRUCT_BEGIN
typedef ATTR_IAR_PACKED struct {
        PACK_STRUCT_FIELD(TUInt32 iBaudRate);
        PACK_STRUCT_FIELD(TUInt8 iCharFormat);
        PACK_STRUCT_FIELD(TUInt8 iParity);
        PACK_STRUCT_FIELD(TUInt8 iNumDataBits);
} PACK_STRUCT_STRUCT TLineCoding;
PACK_STRUCT_END

/* Type Defines: */
/** Type define for the device configuration descriptor structure. This must be defined in the
 *  application code, as the configuration descriptor contains several sub-descriptors which
 *  vary between devices, and which describe the device's usage to the host.
 */
typedef struct
{
    USB_Descriptor_Configuration_Header_t    Config;

    // CDC Control Interface
    USB_Descriptor_Interface_t               CDC_CCI_Interface;
    USB_CDC_Descriptor_FunctionalHeader_t    CDC_Functional_Header;
    USB_CDC_Descriptor_FunctionalACM_t       CDC_Functional_ACM;
    USB_CDC_Descriptor_FunctionalUnion_t     CDC_Functional_Union;
    USB_Descriptor_Endpoint_t                CDC_NotificationEndpoint;

    // CDC Data Interface
    USB_Descriptor_Interface_t               CDC_DCI_Interface;
    USB_Descriptor_Endpoint_t                CDC_DataOutEndpoint;
    USB_Descriptor_Endpoint_t                CDC_DataInEndpoint;
} USB_Descriptor_Configuration_t;

/* Type Defines: */
    /** Type define for the device configuration descriptor structure. This must be defined in the
     *  application code, as the configuration descriptor contains several sub-descriptors which
     *  vary between devices, and which describe the device's usage to the host.
     */
enum StringDescriptors_t
{
    STRING_ID_Language     = 0, /**< Supported Languages string descriptor ID (must be zero) */
    STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
    STRING_ID_Product      = 2, /**< Product string ID */
};

/** Enum for the device interface descriptor IDs within the device. Each interface descriptor
 *  should have a unique ID index associated with it, which can be used to refer to the
 *  interface from other descriptors.
 */
enum InterfaceDescriptors_t
{
    INTERFACE_ID_CDC_CCI = 0, /**< CDC CCI interface descriptor ID */
    INTERFACE_ID_CDC_DCI = 1, /**< CDC DCI interface descriptor ID */
};

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
//static TLineCoding LineCoding = {115200, 0, 0, 8};

static T_uezQueue G_vcommFifoOut;   // out from this device
static T_uezQueue G_vcommFifoIn;    // in to this device

static T_vcommCallbacks G_callbacks;
//static int32_t G_USBCDC_UnitAddress;


static USB_ClassInfo_CDC_Device_t UEZ_CDC_Device = {
    .Config = {
        .ControlInterfaceNumber = 0,

        .DataINEndpointNumber = 3,
        .DataINEndpointSize = CDC_TXRX_EPSIZE,
        .DataINEndpointDoubleBank = false,

        .DataOUTEndpointNumber = 4,
        .DataOUTEndpointSize = CDC_TXRX_EPSIZE,
        .DataOUTEndpointDoubleBank = false,

        .NotificationEndpointNumber = 2,
        .NotificationEndpointSize = CDC_NOTIFICATION_EPSIZE,
        .NotificationEndpointDoubleBank = false,

        .PortNumber = 0
    }
};

void EVENT_CDC_Device_ControLineStateChanged(USB_ClassInfo_CDC_Device_t *const CDCInterfaceInfo)
{
    /* You can get changes to the virtual CDC lines in this callback; a common
       use-case is to use the Data Terminal Ready (DTR) flag to enable and
       disable CDC communications in your application when set to avoid the
       application blocking while waiting for a host to become ready and read
       in the pending data from the USB endpoints.
    */
    bool dtr = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_DTR) != 0;
    bool rts = (CDCInterfaceInfo->State.ControlLineStates.HostToDevice & CDC_CONTROL_LINE_OUT_RTS) != 0;
    // NOTE: DTR can be used to know if there is a terminal open

    //    printf("dtr=%d, rts=%d\n", dtr, rts);
    if (G_callbacks.iVCControlLineState)
        G_callbacks.iVCControlLineState(rts, dtr);
}

void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo)
{
//    printf("baud=%d\n", CDCInterfaceInfo->State.LineEncoding.BaudRateBPS);
    if (G_callbacks.iVCLineSpeedChange)
        G_callbacks.iVCLineSpeedChange(CDCInterfaceInfo->State.LineEncoding.BaudRateBPS);
}

#define FIXED_NUM_CONFIGURATIONS         1

/** Device descriptor structure. This descriptor, located in SRAM memory, describes the overall
 *  device characteristics, including the supported USB version, control endpoint size and the
 *  number of device configurations. The descriptor is read out by the USB host when the enumeration
 *  process begins.
 */
const USB_Descriptor_Device_t DeviceDescriptor =
{
    .Header                 = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

    .USBSpecification       = VERSION_BCD(1.1),
    .Class                  = CDC_CSCP_CDCClass,
    .SubClass               = CDC_CSCP_NoSpecificSubclass,
    .Protocol               = CDC_CSCP_NoSpecificProtocol,

    .Endpoint0Size          = FIXED_CONTROL_ENDPOINT_SIZE,

    .VendorID               = 0x03EB,
    .ProductID              = 0x204A,
    .ReleaseNumber          = VERSION_BCD(1.0),

    .ManufacturerStrIndex   = STRING_ID_Manufacturer,
    .ProductStrIndex        = STRING_ID_Product,
    .SerialNumStrIndex      = NO_DESCRIPTOR,

    .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in SRAM memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
const USB_Descriptor_Configuration_t ConfigurationDescriptor =
{
    .Config =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

            .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
            .TotalInterfaces        = 2,

            .ConfigurationNumber    = 1,
            .ConfigurationStrIndex  = NO_DESCRIPTOR,

            .ConfigAttributes       = USB_CONFIG_ATTR_BUSPOWERED,

            .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
        },

    .CDC_CCI_Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

            .InterfaceNumber        = INTERFACE_ID_CDC_CCI,
            .AlternateSetting       = 0,

            .TotalEndpoints         = 1,

            .Class                  = CDC_CSCP_CDCClass,
            .SubClass               = CDC_CSCP_ACMSubclass,
            .Protocol               = CDC_CSCP_ATCommandProtocol,

            .InterfaceStrIndex      = NO_DESCRIPTOR
        },

    .CDC_Functional_Header =
        {
            .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalHeader_t), .Type = DTYPE_CSInterface},
            .Subtype                = 0x00,

            .CDCSpecification       = VERSION_BCD(1.1),
        },

    .CDC_Functional_ACM =
        {
            .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalACM_t), .Type = DTYPE_CSInterface},
            .Subtype                = 0x02,

            .Capabilities           = 0x02,
        },

    .CDC_Functional_Union =
        {
            .Header                 = {.Size = sizeof(USB_CDC_Descriptor_FunctionalUnion_t), .Type = DTYPE_CSInterface},
            .Subtype                = 0x06,

            .MasterInterfaceNumber  = INTERFACE_ID_CDC_CCI,
            .SlaveInterfaceNumber   = INTERFACE_ID_CDC_DCI,
        },

    .CDC_NotificationEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = CDC_NOTIFICATION_EPADDR,
            .Attributes             = (EP_TYPE_INTERRUPT | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_NOTIFICATION_EPSIZE,
            .PollingIntervalMS      = 0xFF
        },

    .CDC_DCI_Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

            .InterfaceNumber        = INTERFACE_ID_CDC_DCI,
            .AlternateSetting       = 0,

            .TotalEndpoints         = 2,

            .Class                  = CDC_CSCP_CDCDataClass,
            .SubClass               = CDC_CSCP_NoDataSubclass,
            .Protocol               = CDC_CSCP_NoDataProtocol,

            .InterfaceStrIndex      = NO_DESCRIPTOR
        },

    .CDC_DataOutEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = CDC_RX_EPADDR,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_TXRX_EPSIZE,
            .PollingIntervalMS      = 0x05
        },

    .CDC_DataInEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = CDC_TX_EPADDR,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = CDC_TXRX_EPSIZE,
            .PollingIntervalMS      = 0x05
        }
};

/** Convenience macro to easily create \ref USB_Descriptor_String_t instances from a wide character string.
 *
 *  \note This macro is for little-endian systems only.
 *
 *  \param[in] String  String to initialize a USB String Descriptor structure with.
 */
#define USB_STRING_DESCRIPTOR(PString)     \
    { \
        .Header = {\
           .Size = sizeof(USB_Descriptor_Header_t) + (sizeof(PString) - 2), \
           .Type = DTYPE_String \
        }, \
        .String = PString \
    }

/** Language descriptor structure. This descriptor, located in SRAM memory, is returned when the host requests
 *  the string descriptor with index 0 (the first index). It is actually an array of 16-bit integers, which indicate
 *  via the language ID table available at USB.org what languages the device supports for its string descriptors.
 */
//const USB_Descriptor_String_t LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
uint16_t languages[] = { LANGUAGE_ID_ENG };
const ATTR_IAR_PACKED struct { USB_Descriptor_Header_t Header; uint16_t UnicodeString[1]; } ATTR_PACKED
LanguageString = {
    .Header = {
        .Size = sizeof(USB_Descriptor_Header_t) + sizeof(languages),
        .Type = DTYPE_String
    },
    .UnicodeString = { LANGUAGE_ID_ENG }
};

/** Manufacturer descriptor string. This is a string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const ATTR_IAR_PACKED struct { USB_Descriptor_Header_t Header; TUInt8 String[sizeof("Future Designs, Inc.")-1]; } ATTR_PACKED
ManufacturerString = {
    .Header = {
        .Size = sizeof(USB_Descriptor_Header_t) + sizeof("Future Designs, Inc.")-2,
        .Type = DTYPE_String
    },
    .String = "Future Designs, Inc."
};
//USB_STRING_DESCRIPTOR(L"Dean Camera");

/** Product descriptor string. This is a string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
const ATTR_IAR_PACKED struct { USB_Descriptor_Header_t Header; TUInt8 String[sizeof("CDC")-1]; } ATTR_PACKED
ProductString = USB_STRING_DESCRIPTOR("CDC");

/** This function is called by the library when in device mode, and must be overridden (see LUFA library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
uint16_t VirtualCommGetDescriptor(uint8_t corenum,
                                    const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);

    const void* Address = NULL;
    uint16_t    Size    = NO_DESCRIPTOR;

    if (corenum == 0) {
        // USB0 descriptor here
        switch (DescriptorType)
        {
            case DTYPE_Device:
                Address = &DeviceDescriptor;
                Size    = sizeof(USB_Descriptor_Device_t);
                break;
            case DTYPE_Configuration:
                Address = &ConfigurationDescriptor;
                Size    = sizeof(USB_Descriptor_Configuration_t);
                break;
            case DTYPE_String:
                if (DescriptorNumber == STRING_ID_Language)
                {
                    Address = &LanguageString;
                    Size    = LanguageString.Header.Size;
                }
                else if (DescriptorNumber == STRING_ID_Manufacturer)
                {
                    Address = &ManufacturerString;
                    Size    = ManufacturerString.Header.Size;
                }
                else if (DescriptorNumber == STRING_ID_Product)
                {
                    Address = &ProductString;
                    Size    = ProductString.Header.Size;
                }

                break;
        }

        *DescriptorAddress = Address;
    } else {
        // USB1 descriptor here
    }
    return Size;
}

void VirtualCommConnected(void)
{
    printf("VC Connected!\n");
}
void VirtualCommDisconnected(void)
{
    printf("VC Disconnected\n");
}
void VirtualCommConfigChanged(void)
{
    printf("VC Config Changed\n");
   if (!(CDC_Device_ConfigureEndpoints(&UEZ_CDC_Device))) {
       UEZFailureMsg("USB Configure Endpoints Error!");
   }
}

void VirtualCommControlRequest(void)
{
    CDC_Device_ProcessControlRequest(&UEZ_CDC_Device);
}

int32_t VirtualCommUpdate(int32_t aUnitAddress)
{
    uint16_t num;
    uint16_t i;
    int32_t activity = 0;
    TUInt8 buffer[VCOMM_MAX_PACKET_SIZE];

    CDC_Device_USBTask(&UEZ_CDC_Device);

    // Process any bytes waiting to come in
    num = CDC_Device_BytesReceived(&UEZ_CDC_Device);
    for (i=0; i<num; i++) {
        int16_t b = CDC_Device_ReceiveByte(&UEZ_CDC_Device);
        // Put in the queue one character at time quickly (dropping if we overflow)
        UEZQueueSend(G_vcommFifoIn, &b, 0);
        activity = 1;
    }

    // Process any waiting bytes
    for (i=0; i<VCOMM_MAX_PACKET_SIZE; i++)  {
        // Only process if we have data
        if (UEZQueueReceive(G_vcommFifoOut, &buffer[i], 0) != UEZ_ERROR_NONE)
            break;
    }
    if (i != 0) {
        CDC_Device_SendData(&UEZ_CDC_Device, (char const *)buffer, i);
        CDC_Device_Flush(&UEZ_CDC_Device);
        activity = 1;

        if (G_callbacks.iVCEmptyOutput) {
            TUInt8 c;
            if (UEZQueuePeek(G_vcommFifoOut, &c, 0) == UEZ_ERROR_TIMEOUT) {
                G_callbacks.iVCEmptyOutput();
                // No more to come
                activity = 0;
            } else {
                // More to come
                activity = 1;
            }
        }
    }

    return activity;
}

static T_LPCUSBLib_Device_Callbacks G_USBDeviceCallbacks = {
        VirtualCommConnected, // Connected
        VirtualCommDisconnected, // Disconnected
        VirtualCommConfigChanged, // ConfigurationChanged
        VirtualCommControlRequest, // ControlRequest
        0, // Suspend
        0, // WakeUp
        0, // Reset
        VirtualCommUpdate, // Update
        VirtualCommGetDescriptor, // GetDescriptor
};

/*-------------------------------------------------------------------------*
 * Function:  VirtualCommInitialize
 *-------------------------------------------------------------------------*
 * Description:
 *      Initialize the Virtual Comm variables.
 *-------------------------------------------------------------------------*/
T_uezError VirtualCommInitialize(T_vcommCallbacks *aCallbacks, int32_t aUnitAddress, TUInt8 aForceFullSpeed)
{
    T_uezError error;

    //G_USBCDC_UnitAddress = aUnitAddress;

    // Copy over the callback information
    G_callbacks = *aCallbacks;

    error = UEZQueueCreate(VIRTUAL_COMM_QUEUE_IN_SIZE, 1, &G_vcommFifoIn);
    if (error != UEZ_ERROR_NONE)
        return error;

    error = UEZQueueCreate(VIRTUAL_COMM_QUEUE_OUT_SIZE, 1, &G_vcommFifoOut);
    if (error != UEZ_ERROR_NONE) {
        UEZQueueDelete(G_vcommFifoIn);
        return error;
    }

    // Setup full speed USB device
    UEZ_LPCUSBLib_Device_Require(aUnitAddress, &G_USBDeviceCallbacks, aForceFullSpeed);

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
//        CDC_Device_SendByte(&UEZ_CDC_Device, c);
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
