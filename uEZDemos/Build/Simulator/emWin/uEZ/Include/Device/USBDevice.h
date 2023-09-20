/*-------------------------------------------------------------------------*
 * File:  USBDevice.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ I2C Bus Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_USB_DEVICE_H_
#define _DEVICE_USB_DEVICE_H_

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
 *  @file   /Include/Device/USBDevice.h
 *  @brief  uEZ USB Device Device Interface
 *
 *  The uEZ USB Device Device Interface
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
#include <HAL/USBDeviceController.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Configure)(
        void *aWorkspace, 
        const TUInt8 *aDescriptorTable);
    void (*Initialize)(void *aWorkspace);
    void (*Connect)(void *aWorkspace);
    void (*Disconnect)(void *aWorkspace);
    void (*SetAddress)(void *aWorkspace, TUInt8 aAddress);
    void (*EndpointConfigure)(
            void *aWorkspace, 
            TUInt8 aEndpoint, 
            TUInt16 aMaxPacketSize);
    TInt16 (*Read)(
            void *aWorkspace, 
            TUInt8 aEndpoint, 
            TUInt8 *aData, 
            TUInt16 iMaxLen);
    TUInt16 (*Write)(
            void *aWorkspace, 
            TUInt8 aEndpoint, 
            TUInt8 *aData, 
            TUInt16 aLength);
    void (*SetStallState)(
            void *aWorkspace, 
            TUInt8 aEndpoint, 
            TBool aStallState);
    TBool (*IsStalled)(
            void *aWorkspace, 
            TUInt8 aEndpoint);
    void (*EndpointInterruptsEnable)(
            void *aWorkspace, 
            TUInt8 aEndpoint);
    void (*EndpointInterruptsDisable)(
            void *aWorkspace, 
            TUInt8 aEndpoint);
    void (*ForceAddressAndConfigurationNum)(
            void *aWorkspace, 
            TUInt8 aAddress, 
            TUInt8 aConfigNum);
    T_uezError (*RegisterEndpointCallback)(
            void *aWorkspace, 
            TUInt8 aEndpoint, 
            T_USBEndpointHandlerFunc aEndpointFunc);
    T_uezError (*UnregisterEndpointCallback)(
            void *aWorkspace, 
            TUInt8 aEndpoint);
    T_uezError (*RegisterRequestTypeCallback)(
            void *aWorkspace, 
            TUInt8 aType, 
            void *aBuffer,
            void *aCallbackWorkspace,
            T_USBRequestHandlerFunc aRequestHandler);
    T_uezError (*UnregisterRequestTypeCallback)(
            void *aWorkspace, 
            TUInt8 aType);
    T_uezError (*ProcessEndpoints)(void *aWorkspace, TUInt32 aTimeout);
    void (*InterruptNakEnable)(void *aWorkspace, TUInt8 aNakBits);
} DEVICE_USB_DEVICE;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_USB_DEVICE_H_
/*-------------------------------------------------------------------------*
 * End of File:  USBDevice.h
 *-------------------------------------------------------------------------*/

