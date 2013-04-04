/*-------------------------------------------------------------------------*
 * File:  Timer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Timer HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_Timer_H_
#define _HAL_Timer_H_

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

#endif // _HAL_Timer_H_
/*-------------------------------------------------------------------------*
 * End of File:  Timer.h
 *-------------------------------------------------------------------------*/
