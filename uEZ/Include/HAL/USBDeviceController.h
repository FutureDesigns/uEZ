/*-------------------------------------------------------------------------*
 * File:  USBDeviceController.h
 *-------------------------------------------------------------------------*
 * Description:
 *     USB Device Controller HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_USBDeviceController_H_
#define _HAL_USBDeviceController_H_

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
 *  @file   /Include/HAL/USBDeviceController.h
 *  @brief  uEZ USB Device Controller HAL Interface
 *
 *  The uEZ USB Device Controller HAL Interface
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
#include <HAL/HAL.h>
#include <Include/Types/USBDevice.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const TUInt8 *iUSBDescriptorTable;
    T_USBDevice *iUSBDevice;
} T_USBDeviceControllerSettings;

typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*Configure)(
        void *aWorkspace, 
        T_USBDeviceControllerSettings *iSettings);
    void (*Initialize)(void *aWorkspace);
    void (*Connect)(void *aWorkspace);
    void (*Disconnect)(void *aWorkspace);
    void (*SetAddress)(void *aWorkspace, TUInt8 aAddress);
    void (*SetConfiguration)(
                void *aWorkspace,
                TUInt8 aConfigurationIndex);
    TUInt8 (*GetConfiguration)(void *aWorkspace);
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
    void (*ProcessEndpoints)(void *aWorkspace);
    const TUInt8 * (*GetDescriptor)(
            void *aWorkspace, 
            TUInt8 aType, 
            TUInt16 aIndex, 
            TUInt16 *aLength);
    void (*InterruptNakEnable)(void *aWorkspace, TUInt8 aNakBits);
} HAL_USBDeviceController ;

#endif // _HAL_USBDeviceController_H_
/*-------------------------------------------------------------------------*
 * End of File:  USBDeviceController.h
 *-------------------------------------------------------------------------*/
