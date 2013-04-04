/*-------------------------------------------------------------------------*
 * File:  PWM.h
 *-------------------------------------------------------------------------*
 * Description:
 *     PWM HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_PWM_H_
#define _HAL_PWM_H_

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
 *  @file   /Include/HAL/PWM.h
 *  @brief  uEZ PWM HAL Interface
 *
 *  The uEZ PWM HAL Interface
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
    T_uezError (*SetMaster)(
        void *aWorkspace, 
        TUInt32 aPeriod,
        TUInt8 aMatchRegister);
    T_uezError (*SetMatchRegister)(
        void *aWorkspace, 
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop);
    T_uezError (*EnableSingleEdgeOutput)(
        void *aWorkspace,
        TUInt8 aMatchRegister);
    T_uezError (*DisableOutput)(
        void *aWorkspace,
        TUInt8 aMatchRegister);
	T_uezError (*SetMatchCallback)(
        void *aWorkspace,
        TUInt8 aMatchRegister,
		void (*aCallback)(void *),
		void *aCallbackWorkspace);
} HAL_PWM;

#endif // _HAL_PWM_H_
/*-------------------------------------------------------------------------*
 * End of File:  PWM.h
 *-------------------------------------------------------------------------*/
