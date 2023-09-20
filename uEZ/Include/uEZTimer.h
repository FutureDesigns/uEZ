/*-------------------------------------------------------------------------*
 * File:  uEZTimer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZTIMER_H_
#define UEZTIMER_H_

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
 *  @file   uEZTimer.h
 *  @brief  uEZ Timer
 *
 *  uEZ Timer
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Types/Timer.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct _T_uezTimerCallback {
        /** Device handle to timer this callback is associated with */
        T_uezDevice iTimer;

        /** Timer match register this callback is associated with */
        TUInt32 iMatchRegister;

        /** Semaphore to 'release' when interrupt occurs, or put in
         *  UEZ_NULL_HANDLE for none */
        T_uezSemaphore iTriggerSem;

        /** Set iCallback to a non-NULL value to call a function with
         *  this structure each time the interrupt fires for this match
         *  register. */
        void (*iCallback)(struct _T_uezTimerCallback *aCallbackData);

        /** Optional pointer to extra data */
        void *iData;
} T_uezTimerCallback;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError UEZTimerOpen(const char *aTimerName, T_uezDevice *aTimer);
T_uezError UEZTimerClose(T_uezDevice aTimer);
T_uezError UEZTimerSetupOneShot(
        T_uezDevice aTimer,
        TUInt32 aMatchRegister,
        TUInt32 aPeriodInCPUCycles,
        T_uezTimerCallback *aCallback);
T_uezError UEZTimerSetupRegularInterval(
        T_uezDevice aTimer,
        TUInt32 aMatchRegister,
        TUInt32 aPeriodInCPUCycles,
        T_uezTimerCallback *aCallback);
T_uezError UEZTimerEnable(T_uezDevice aTimer);
T_uezError UEZTimerDisable(T_uezDevice aTimer);
T_uezError UEZTimerReset(T_uezDevice aTimer);
T_uezError UEZTimerSetCaptureSource(T_uezDevice aTimer, TUInt32 aCaptureSource);
T_uezError UEZTimerSetTimerMode(T_uezDevice aTimer, T_Timer_Mode aMode);
T_uezError UEZTimerSetExternalControl(
        T_uezDevice aTimer,
        TUInt8 aMatchRegister,
        T_Timer_ExternalControl aExtControl);

#ifdef __cplusplus
}
#endif

#endif // UEZTIMER_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZTimer.h
 *-------------------------------------------------------------------------*/
