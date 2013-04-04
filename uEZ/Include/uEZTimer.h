/*-------------------------------------------------------------------------*
 * File:  uEZTimer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZTIMER_H_
#define UEZTIMER_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
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

#endif // UEZTIMER_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZTimer.h
 *-------------------------------------------------------------------------*/
