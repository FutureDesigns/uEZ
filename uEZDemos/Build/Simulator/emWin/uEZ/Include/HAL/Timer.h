/*-------------------------------------------------------------------------*
 * File:  Timer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Timer HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_Timer_H_
#define _HAL_Timer_H_

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
 *  @file   /Include/HAL/Timer.h
 *  @brief  uEZ Timer HAL Interface
 *
 *  The uEZ Timer HAL Interface
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
#include <Types/Timer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*T_HALTimer_Callback)(void *aCallbackWorkspace);

/*-------------------------------------------------------------------------*
 * HAL Interface:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
/**
 * Sets the clock rate using match register 0
 */
    T_uezError (*SetTimerMode)(
        void *aWorkspace, 
        T_Timer_Mode aMode);
    T_uezError (*SetCaptureSource)(
        void *aWorkspace,
        TUInt32 aSourceIndex);
    T_uezError (*SetMatchRegister)(
        void *aWorkspace, 
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop);
    T_uezError (*SetExternalControl)(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        T_Timer_ExternalControl aExtControl);
    void (*Reset)(void *aWorkspace);
    void (*Enable)(void *aWorkspace);
    void (*Disable)(void *aWorkspace);

/**
 * uEZ v1.12
 */
    T_uezError (*SetMatchCallback)(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        T_HALTimer_Callback aCallbackFunc,
        void *aCallbackWorkspace);

/**
 * uEZ v1.13
 */
    T_uezError (*SetMatchRegisterFunctions)(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        T_Timer_MatchPointFunction aFunctions,
        TUInt32 aMatchPoint);
} HAL_Timer;

#ifdef __cplusplus
}
#endif

#endif // _HAL_Timer_H_
/*-------------------------------------------------------------------------*
 * End of File:  Timer.h
 *-------------------------------------------------------------------------*/
