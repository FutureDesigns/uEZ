/*-------------------------------------------------------------------------*
 * File:  Types/USBHost.h
 *-------------------------------------------------------------------------*
 * Description:
 *     USBHost types
 *-------------------------------------------------------------------------*/
#ifndef _USBHost_TYPES_H_
#define _USBHost_TYPES_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
/**
 *  @file   /Include/Types/USBHost.h
 *  @brief  uEZ USB Host Types
 *
 *  The uEZ USB Host Types
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

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define  USB_DESCRIPTOR_TYPE_DEVICE                     1
#define  USB_DESCRIPTOR_TYPE_CONFIGURATION              2
#define  USB_DESCRIPTOR_TYPE_INTERFACE                  4
#define  USB_DESCRIPTOR_TYPE_ENDPOINT                   5
#define  USB_DESCRIPTOR_TYPE_HID                        33
#define  USB_DESCRIPTOR_TYPE_REPORT                     34

/**  ----------- Control RequestType Fields  ----------- */
#define  USB_DEVICE_TO_HOST         0x80
#define  USB_HOST_TO_DEVICE         0x00
#define  USB_REQ_TYPE_CLASS         0x20
#define  USB_REQ_TYPE_STANDARD      0x00
#define  USB_RECIPIENT_DEVICE       0x00
#define  USB_RECIPIENT_INTERFACE    0x01

/** -------------- USB Standard Requests  -------------- */
#define  SET_ADDRESS                 5
#define  GET_DESCRIPTOR              6
#define  SET_CONFIGURATION           9
#define  SET_INTERFACE              11

#define USB_ENDPOINT_TYPE_INTERRUPT     3

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    USB_HOST_STATE_CLEAR=0,
    USB_HOST_STATE_CONNECTED=1,
    USB_HOST_STATE_DISCONNECTED=2,
} T_usbHostDeviceState;

typedef enum {
    USB_HOST_SPEED_LOW=0,
    USB_HOST_SPEED_FULL=1,
    USB_HOST_SPEED_HIGH=2 // not always supported
} T_usbHostSpeed;

PACK_STRUCT_BEGIN
typedef struct {
    //! Length of this HID descriptor (should be 9 bytes)
    PACK_STRUCT_FIELD(TUInt8 iLength);

    //! Descriptor type (should be USB_DESCRIPTOR_TYPE_HID or 33)
    PACK_STRUCT_FIELD(TUInt8 iType);

    //! BCD code HID version (e.g., 1.10 = 0x0110)
    PACK_STRUCT_FIELD(TUInt16 iBCDHIDVersion);

    //! Country code (usually 0x00)
    PACK_STRUCT_FIELD(TUInt8 iCountryCode);

    //! Number of descriptors (usually 1)
    PACK_STRUCT_FIELD(TUInt8 iNumDescriptors);

    //! Type of descriptors (usually USB_DESCRIPTOR_TYPE_REPORT or 34)
    PACK_STRUCT_FIELD(TUInt8 iDescriptorType);

    //! Descriptor length
    PACK_STRUCT_FIELD(TUInt16 iDescriptorLength);
} PACK_STRUCT_STRUCT T_usbHIDDescriptor;
PACK_STRUCT_END

PACK_STRUCT_BEGIN
typedef struct {
    //! Length of this Endpoint descriptor (should be 7 bytes)
    PACK_STRUCT_FIELD(TUInt8 iLength);

    //! Descriptor type (should be USB_DESCRIPTOR_TYPE_ENDPOINT or 5)
    PACK_STRUCT_FIELD(TUInt8 iType);

    //! Endpoint address in lower 7 bits, top bit is 1=IN (0x80), 0=OUT (0x00)
    PACK_STRUCT_FIELD(TUInt8 iEndpointAddress);

    //! Endpoint type (lowest 3 bits)
    PACK_STRUCT_FIELD(TUInt8 iEndpointType);

    //! Maximum packet size per transfer (plus HS bit)
    PACK_STRUCT_FIELD(TUInt16 iMaxPacketSize);

    //! Poll interval (), for interrupt endpoints
    PACK_STRUCT_FIELD(TUInt8 iInterval);
} PACK_STRUCT_STRUCT T_usbEndpointDescriptor;
PACK_STRUCT_END

#ifdef __cplusplus
}
#endif

#endif // _USBHost_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/USBHost.h
 *-------------------------------------------------------------------------*/
