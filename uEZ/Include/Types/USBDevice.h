/*-------------------------------------------------------------------------*
 * File:  Types/USBDevice.h
 *-------------------------------------------------------------------------*
 * Description:
 *     USB Device types for configuration and control
 *-------------------------------------------------------------------------*/
#ifndef _USBDevice_TYPES_H_
#define _USBDevice_TYPES_H_

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
/**
 *  @file   /Include/Types/USBDevice.h
 *  @brief  uEZ USB Device Types
 *
 *  The uEZ USB Device Types
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZPacked.h>

#if (COMPILER_TYPE==IAR)
#pragma pack(push, 1)
#define PACKED
#endif

/**
 * Constants:
 */
#define MAX_NUM_ENDPOINTS                   16
#define USB_ENDPOINT0_PACKET_SIZE           64

/**
 * Macros:
 */
/** USB Unsigned 16-bit integers are low first, then high */
#define USB_UINT16(x)                       ((x)&0xFF),((x)>>8)
#define ENDPOINT_IN(endp)                   (0x80 | endp)
#define ENDPOINT_OUT(endp)                  (0x00 | endp)
#define ENDPOINT_IS_IN(endp)                (endp & 0x80)
#define ENDPOINT_INDEX(endp)                (endp & 0x7F)

#if (COMPILER_TYPE==CodeRed)
#define PACKED __packed
#endif
#if (COMPILER_TYPE==RowleyARM)
#define PACKED __attribute__ ((__packed__))
#endif
#if (COMPILER_TYPE==Keil4)
#define PACKED __attribute__ ((__packed__))
#endif
#if (COMPILER_TYPE==GCC)
#define PACKED __attribute__ ((__packed__))
#endif

/** Communication Structures: */
/** Structure for splitting up 8 bit value into 3 bit fields
 * that holds recipient type */
typedef struct {
    TUInt8 iRecipientType:5;
        #define USB_RECIPIENT_TYPE_DEVICE               0
        #define USB_RECIPIENT_TYPE_INTERFACE            1
        #define USB_RECIPIENT_TYPE_ENDPOINT             2
        #define USB_RECIPIENT_TYPE_OTHER                3 /** or higher */ 

    TUInt8 iType:2;
        #define USB_REQUEST_TYPE_STANDARD               0
        #define USB_REQUEST_TYPE_CLASS                  1
        #define USB_REQUEST_TYPE_VENDOR                 2
        #define USB_REQUEST_TYPE_RESERVED               3

    TUInt8 iDirection:1;
        #define USB_DIRECTION_TO_DEVICE                 0
        #define USB_DIRECTION_TO_HOST                   1

} PACKED T_USBRequestType;

/** Setup packet is first packet for requests.  Sets up the
 * next group of transactions. */
typedef struct {
    /** Bits containing the type of setup packet */
    T_USBRequestType iRequestType;

    /** Request number */
    TUInt8 iRequest;

    /** 16 bit general use value, if needed */
    TUInt16 iValue;

    /** 16 bit value for index, if needed */
    TUInt16 iIndex;

    /** 16 bit value specifying data size to come or be received. */
    TUInt16 iLength;
} PACKED T_USBSetupPacket;

/**
 * Configuration Structures:
 */
typedef TUInt8 T_USBStatus;
#define USB_STATUS_CONNECT                              (1<<0)
#define USB_STATUS_CONNECT_CHANGED                      (1<<1)
#define USB_STATUS_SUSPEND                              (1<<2)
#define USB_STATUS_SUSPEND_CHANGED                      (1<<3)
#define USB_STATUS_BUS_RESET                            (1<<4)

typedef TUInt8 T_USBEndpointStatus;
#define USB_ENDPOINT_SELECT_FULL_EMPTY                  (1<<0)
#define USB_ENDPOINT_SELECT_STALLED                     (1<<1)
#define USB_ENDPOINT_SELECT_SETUP_BIT                   (1<<2)
#define USB_ENDPOINT_SELECT_PACKET_OVERWRITTEN          (1<<3)
#define USB_ENDPOINT_SELECT_EP_NAKED                    (1<<4)
#define USB_ENDPOINT_SELECT_B1_FULL                     (1<<5)
#define USB_ENDPOINT_SELECT_B2_FULL                     (1<<6)

typedef TUInt8 T_USBDeviceMode;
#define USB_DEVICE_SET_MODE_AP_CLK                      (1<<0)
#define USB_DEVICE_SET_MODE_INAK_CI                     (1<<1)
#define USB_DEVICE_SET_MODE_INAK_CO                     (1<<2)
#define USB_DEVICE_SET_MODE_INAK_II                     (1<<3)
#define USB_DEVICE_SET_MODE_INAK_IO                     (1<<4)
#define USB_DEVICE_SET_MODE_INAK_BI                     (1<<5)
#define USB_DEVICE_SET_MODE_INAK_BO                     (1<<6)

/** Endpoints have a handler for receiving and for sending. */
typedef void (*T_USBEndpointHandlerFunc)(
                    void *aCallbackWorkspace,
                    TUInt8 aEndpoint,
                    T_USBEndpointStatus aStatus);

/** Callback routine for status changes */
typedef void (*T_USBStatusChangeFunc)(
                void *aCallbackWorkspace,
                T_USBStatus aStatus);

/** Notification routine that endpoint data is ready */
typedef void (*T_USBStatusEndpointReady)(void *aCallbackWorkspace);

/** An endpoint is defined as a pair of functions.  One for */
typedef struct {
    /** Function for handling packets from device to host
     * (In to Host) */
    T_USBEndpointHandlerFunc iIn;

    /** Function for handling packets from host to device
     * (Out of Host) */
    T_USBEndpointHandlerFunc iOut;
} T_USBEndpointHandler;

typedef TBool (*T_USBRequestHandlerFunc)(
                void *aCallbackWorkspace,
                T_USBSetupPacket *aSetup,
                TUInt16 *aLength,
                TUInt8 **aData);

typedef struct {
    T_USBRequestHandlerFunc iHandler;
    TUInt8 *iBuffer;
    void *iWorkspace;
} T_USBDeviceRequestTypeHandlerStruct;

typedef struct {
    /** Callback workspace */
    void *iCallbackWorkspace;

    /** Routine to handle status changes */
    T_USBStatusChangeFunc iStatusChangeFunc;

    /** Routine to handle endpoint data ready */
    T_USBStatusEndpointReady iStatusEndpointReadyFunc;

    T_USBDeviceRequestTypeHandlerStruct iRequestTypeHandlers[4];

    /** Function pointers for all the endpoints.
     *  If not used, leave blank with nulls. */
    T_USBEndpointHandler iEndpointFuncs[MAX_NUM_ENDPOINTS];
} T_USBDevice;

/**
 * USB Standard Device Requests:
 */
#define	USB_DEVICE_REQUEST_GET_STATUS                   0x00
#define USB_DEVICE_REQUEST_CLEAR_FEATURE                0x01
#define USB_DEVICE_REQUEST_SET_FEATURE                  0x03
#define USB_DEVICE_REQUEST_SET_ADDRESS                  0x05
#define USB_DEVICE_REQUEST_GET_DESCRIPTOR               0x06
#define USB_DEVICE_REQUEST_SET_DESCRIPTOR               0x07
#define USB_DEVICE_REQUEST_GET_CONFIGURATION            0x08
#define USB_DEVICE_REQUEST_SET_CONFIGURATION            0x09
#define USB_DEVICE_REQUEST_GET_INTERFACE                0x0A
#define USB_DEVICE_REQUEST_SET_INTERFACE                0x0B
#define USB_DEVICE_REQUEST_SYNCH_FRAME                  0x0C

/**
 * USB Standard Interface Requests:
 */
#define USB_INTERFACE_REQUEST_GET_STATUS                0x00
#define USB_INTERFACE_REQUEST_CLEAR_FEATURE             0x01
#define USB_INTERFACE_REQUEST_SET_FEATURE               0x03
#define USB_INTERFACE_REQUEST_GET_INTERFACE             0x0A
#define USB_INTERFACE_REQUEST_SET_INTERFACE             0x11

/**
 * USB Standard Endpoint Requests:
 */
#define USB_ENDPOINT_REQUEST_GET_STATUS                 0x00
#define USB_ENDPOINT_REQUEST_CLEAR_FEATURE              0x01
#define USB_ENDPOINT_REQUEST_SET_FEATURE                0x03
#define         USB_ENDPOINT_REQUEST_FEATURE_HALT               0x00
#define USB_ENDPOINT_REQUEST_SYNC_FRAME                 0x12

/**
 * USB Descriptor Structures:
 */
/** USB Descriptor Header */
typedef struct {
    TUInt8 iLength;
    TUInt8 iDescriptorType;
        /** Types of descriptors */
        #define USB_DESCRIPTOR_DEVICE                   1
        #define USB_DESCRIPTOR_CONFIGURATION            2
        #define USB_DESCRIPTOR_STRING                   3
        #define USB_DESCRIPTOR_INTERFACE                4
        #define USB_DESCRIPTOR_ENDPOINT                 5
        #define USB_DESCRIPTOR_DEVICE_QUALIFIER         6
        #define USB_DESCRIPTOR_OTHER_SPEED              7
        #define USB_DESCRIPTOR_INTERFACE_POWER          8
} PACKED T_USBDescriptorHeader;

typedef struct {
    TUInt8 reserved1:5;
    TUInt8 iRemoteWakeup:1;
    TUInt8 iSelfPowered:1;
    TUInt8 reserved2:1;
} PACKED T_USBConfigurationAttributes;

typedef struct {
    T_USBDescriptorHeader iHeader;
    TUInt16 iTotalLength;
    TUInt8 iNumInterfaces;
    TUInt8 iConfigurationValue;
    TUInt8 iConfiguration;
    T_USBConfigurationAttributes iAttributes;
    TUInt8 iMaxPower;  /** value * 2 milli-amps */
} PACKED T_USBDescriptorConfiguration;

typedef struct {
    T_USBDescriptorHeader iHeader;
    TUInt8 iNumber;
    TUInt8 iAlternateSetting;
    TUInt8 iNumEndpoints;
    TUInt8 iClass;
    TUInt8 iSubClass;
    TUInt8 iProtocol;
    TUInt8 iString;
} PACKED T_USBDescriptorInterface;

typedef TUInt8 T_USBEndpointAddress;
    #define USB_ENDPOINT_ADDRESS_ADDRESS                    0x0F
    #define USB_ENDPOINT_ADDRESS_DIRECTION                  (1<<7)
        #define USB_ENDPOINT_ADDRESS_DIR_OUT                    0
        #define USB_ENDPOINT_ADDRESS_DIR_IN                     1
    #define USB_ENDPOINT_ADDRESS(addr, dir)                 (addr|(dir<<7))

typedef struct {
    TUInt8 iType:2;
        #define USB_ENDPOINT_TYPE_CONTROL               0
        #define USB_ENDPOINT_TYPE_ISOCHRONOUS           1
        #define USB_ENDPOINT_TYPE_BULK                  2
        #define USB_ENDPOINT_TYPE_INTERRUPT             3
    TUInt8 iSynchronizationType:2;  /** Isochronous mode only */
        #define USB_ENDPOINT_SYNC_TYPE_NONE             0
        #define USB_ENDPOINT_SYNC_TYPE_ASYNC            1
        #define USB_ENDPOINT_SYNC_TYPE_ADAPTIVE         2
        #define USB_ENDPOINT_SYNC_TYPE_SYNC             3
    TUInt8 iUsageType:2;  /** Isochronous mode only */
        #define USB_ENDPOINT_USAGE_TYPE_DATA            0
        #define USB_ENDPOINT_USAGE_TYPE_FEEDBACK        1
        #define USB_ENDPOINT_USAGE_TYPE_EXPL_FEEDBACK   2
        #define USB_ENDPOINT_USAGE_TYPE_RESERVED        3
    TUInt8 reserved:2;
} PACKED T_USBEndpointAttributes;

typedef struct {
    T_USBDescriptorHeader iHeader;
    T_USBEndpointAddress iAddress;
    T_USBEndpointAttributes iAttributes;
    TUInt16 iMaximumPacketSize;
    TUInt8 iPollInterval;   /** number of frames between polls (Usually ms) */
} PACKED T_USBDescriptorEndpoint;

#if (COMPILER_TYPE==IAR)
#pragma pack(pop)
#endif

#endif // _USBDevice_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/USBDevice.h
 *-------------------------------------------------------------------------*/
