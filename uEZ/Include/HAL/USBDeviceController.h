/*-------------------------------------------------------------------------*
 * File:  USBDeviceController.h
 *-------------------------------------------------------------------------*
 * Description:
 *     USB Device Controller HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_USBDeviceController_H_
#define _HAL_USBDeviceController_H_

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
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
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

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _HAL_USBDeviceController_H_
/*-------------------------------------------------------------------------*
 * End of File:  USBDeviceController.h
 *-------------------------------------------------------------------------*/
