/*-------------------------------------------------------------------------*
 * File:  DEVICE/USBHost.h
 *-------------------------------------------------------------------------*
 * Description:
 *     USBHost DEVICE interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_USBHost_H_
#define _DEVICE_USBHost_H_

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
 *  @file   /Include/Device/USBHost.h
 *  @brief  uEZ USB Host Device Interface
 *
 *  The uEZ USB Host Device Interface
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
#include <uEZDevice.h>
#include <Types/USBHost.h>

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
#define  USB_DESCRIPTOR_TYPE_REPORT_DESCRIPTOR          0x2200

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezError (*ConnectedCheckMatch)(
        void *aDeviceDriverWorkspace,
        TUInt8 aDeviceAddress);
    T_uezError (*Disconnected)(
        void *aDeviceDriverWorkspace,
        TUInt8 aDeviceAddress);
} T_USBHostDeviceDriverAPI;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iInterface;

    // Functions
    T_uezError (*USBHostDeviceDriverRegister)(
        void *aWorkspace,
        T_uezDeviceWorkspace *iDeviceDriverWorkspace,
        const T_USBHostDeviceDriverAPI *aAPI);
    T_uezError (*USBHostDeviceDriverUnregister)(
        void *aWorkspace,
        T_uezDeviceWorkspace *iDeviceDriverWorkspace);
    T_uezError (*Init)(void *aWorkspace);
    void (*ResetPort)(void *aWorkspace);
    void * (*AllocBuffer)(void *aWorkspace, TUInt32 aSize);
    T_usbHostDeviceState (*GetState)(void *aWorkspace);

    T_uezError (*SetControlPacketSize)(
            void *aWorkspace, 
            TUInt8 aDeviceAddress,
            TUInt16 aPacketSize);
    T_uezError (*SetAddress)(
            void *aWorkspace, 
            TUInt8 aAddress);
    T_uezError (*ConfigureEndpoint)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpointAndInOut,   /**< bit 0x80 is set if IN */
            TUInt16 aMaxPacketSize);
    T_uezError (*SetConfiguration)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt32 aConfiguration);

    T_uezError (*Control)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aBMRequestType,
            TUInt8 aRequest,
            TUInt16 aValue,
            TUInt16 aIndex,
            TUInt16 aLength,
            void *aBuffer,
            TUInt32 aTimeout);
    T_uezError (*GetDescriptor)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aType,
            TUInt8 aIndex,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*GetString)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt32 aIndex,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*BulkOut)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*BulkIn)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*InterruptOut)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            const TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
    T_uezError (*InterruptIn)(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout);
} DEVICE_USBHost ;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_USBHost_H_
/*-------------------------------------------------------------------------*
 * End of File:  DEVICE/USBHost.h
 *-------------------------------------------------------------------------*/
