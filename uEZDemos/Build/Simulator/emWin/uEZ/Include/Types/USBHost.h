/*-------------------------------------------------------------------------*
 * File:  Types/USBHost.h
 *-------------------------------------------------------------------------*
 * Description:
 *     USBHost types
 *-------------------------------------------------------------------------*/
#ifndef _USBHost_TYPES_H_
#define _USBHost_TYPES_H_

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

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define  USB_DESCRIPTOR_TYPE_DEVICE                     1
#define  USB_DESCRIPTOR_TYPE_CONFIGURATION              2
#define  USB_DESCRIPTOR_TYPE_INTERFACE                  4
#define  USB_DESCRIPTOR_TYPE_ENDPOINT                   5

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

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    USB_HOST_STATE_CLEAR=0,
    USB_HOST_STATE_CONNECTED=1,
    USB_HOST_STATE_DISCONNECTED=2,
} T_usbHostDeviceState;

#endif // _USBHost_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/USBHost.h
 *-------------------------------------------------------------------------*/
