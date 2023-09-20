/*-------------------------------------------------------------------------*
 * File:  Timer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_TIMER_H_
#define _DEVICE_TIMER_H_

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
 *  @file   /Include/Device/Timer.h
 *  @brief  uEZ Timer Device Interface
 *
 *  The uEZ Timer Device Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Types/Timer.h>
#include <uEZDevice.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*T_DeviceTimer_Callback)(void *aCallbackWorkspace);

typedef struct {
        // Header
        T_uezDeviceInterface iDevice;

        // Functions
        /**
		 * Added uEZ v1.13
		 */
        T_uezError (*Reset)(void *aWorkspace);
        T_uezError (*Enable)(void *aWorkspace);
        T_uezError (*Disable)(void *aWorkspace);

        T_uezError (*SetTimerMode)(void *aWorkspace, T_Timer_Mode aMode);
        T_uezError (*SetCaptureSource)(void *aWorkspace, TUInt32 aSourceIndex);
        T_uezError (*SetMatchRegister)(
                void *aWorkspace,
                TUInt8 aMatchRegister,
                TUInt32 aMatchPoint,
                T_Timer_MatchPointFunction aFunctions);
        T_uezError (*SetExternalControl)(
                void *aWorkspace,
                TUInt8 aMatchRegister,
                T_Timer_ExternalControl aExtControl);

        T_uezError (*SetMatchCallback)(
                void *aWorkspace,
                TUInt8 aMatchRegister,
                T_DeviceTimer_Callback aCallbackFunc,
                void *aCallbackWorkspace);
} DEVICE_Timer;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_TIMER_H_
/*-------------------------------------------------------------------------*
 * End of File:  Timer.h
 *-------------------------------------------------------------------------*/
