/*-------------------------------------------------------------------------*
 * File:  USBDevice.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ I2C Bus Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_USB_DEVICE_H_
#define _DEVICE_USB_DEVICE_H_

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

#endif // _DEVICE_USB_DEVICE_H_
/*-------------------------------------------------------------------------*
 * End of File:  USBDevice.h
 *-------------------------------------------------------------------------*/

