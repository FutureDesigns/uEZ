/*-------------------------------------------------------------------------*
 * File:  ExternalInterrupt.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ ExternalInterrupt Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_ExternalInterrupt_H_
#define _DEVICE_ExternalInterrupt_H_

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
 *  @file   /Include/Device/ExternalInterrupt.h
 *  @brief  uEZ External Interrupt Device Interface
 *
 *  The uEZ External Interrupt Device Interface
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
#include <HAL/Interrupt.h>
#include <Types/EINT.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Set)(
        void *aWorkspace,
        TUInt32 aChannel,
        T_EINTTrigger aTrigger,
        EINT_Callback aCallbackFunc,
        void *aCallbackWorkspace,
        T_EINTPriority aPriority,
        const char *aName);
    T_uezError (*Reset)(void *aWorkspace, TUInt32 aChannel);
    T_uezError (*Clear)(void *aWorkspace, TUInt32 aChannel);
    T_uezError (*Disable)(void *aWorkspace, TUInt32 aChannel);
    T_uezError (*Enable)(void *aWorkspace, TUInt32 aChannel);
} DEVICE_ExternalInterrupt;

#endif // _DEVICE_ExternalInterrupt_H_
/*-------------------------------------------------------------------------*
 * End of File:  ExternalInterrupt.h
 *-------------------------------------------------------------------------*/

