/*-------------------------------------------------------------------------*
 * File:  uEZTimer.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

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
 *  @addtogroup uEZTimer
 *  @{
 *  @brief     uEZ Timer Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ Timer interface.
 *
 *  @par Example code:
 *  Example task to use the timer
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  void TimerCallback(void *aWorkspace)
 *  {
 *
 *  }
 *
 *  T_uezDevice Timer;
 *  T_uezTimerCallback callback;
 *  T_uezError error;
 *
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *      callback.iTimer = timer;
 *      callback.iMatchRegister = 1;
 *      callback.iTriggerSem = 0;
 *      callback.iCallback = TimerCallback;
 *      if ( UEZTimerSetupOneShot(
 *                      timer,
 *                      1,
 *                      (PROCESSOR_OSCILLATOR_FREQUENCY)/1000,
 *                      &callback) == UEZ_ERROR_NONE){
 *          error = UEZTimerSetTimerMode(timer, TIMER_MODE_CLOCK);
 *          error = UEZTimerReset(timer);
 *          UEZTimerEnable(timer);
 *  } else {
 *      // an error occurred opening Timer
 *  }
 *  @endcode
 */

 
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <UEZTimer.h>
#include <Device/Timer.h>
#include <uEZDeviceTable.h>

/*-------------------------------------------------------------------------*
 * Routines:
 *-------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open the Timer interface.
 *
 *  @param [in]    aTimerName       Name of timer to open
 *
 *  @param [in]    *aTimer          Pointer to device handle to be returned
 *
 *  @return        T_uezError     	If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  T_uezDevice Timer;
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *     	// the device opened properly
 *  } else {
 *      // an error occurred opening Timer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerOpen(const char *aTimerName, T_uezDevice *aTimer)
{
    T_uezError error;
    DEVICE_Timer **p;

    error = UEZDeviceTableFind(aTimerName, aTimer);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aTimer, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}


/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerClose
 *---------------------------------------------------------------------------*/
/**
 *  Close the Timer interface.
 *
 *  @param [in]    aTimer			Pointer to Timer Device
 *
 *  @return        T_uezError     	If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  T_uezDevice Timer;
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *  
 *      if (UEZTimerClose(Timer) != UEZ_ERROR_NONE) {
 *    	    //an error occurred
 *      }
 *
 *  } else {
 *      // an error occurred opening Timer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerClose(T_uezDevice aTimer)
{
    // Nothing currently
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IUEZTimerCallback
 *---------------------------------------------------------------------------*/
/**
 *  E
 *
 *  @param [in]    *aCallbackWorkspace			Timer workspace
 *
 *  @return        void
 *
 *  @par Example Code:
 *  @code

 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IUEZTimerCallback(void *aCallbackWorkspace)
{
    T_uezTimerCallback *p = (T_uezTimerCallback *)aCallbackWorkspace;
    if (p->iTriggerSem)
        _isr_UEZSemaphoreRelease(p->iTriggerSem);
    if (p->iCallback)
        p->iCallback(aCallbackWorkspace);
}


/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerSetupOneShot
 *---------------------------------------------------------------------------*/
/**
 *  E
 *
 *  @param [in]    aTimer			    Pointer to Timer Device
 * *
 *  @param [in]    aMatchRegister       Match Register number
 *
 *  @param [in]    aPeriodInCPUCycles   Period in CPU cycles
 *
 *  @param [in]    *aCallback			Pointer to callback
 *
 *  @return        T_uezError     	    If the device is successfully closed,
 *                                      returns UEZ_ERROR_NONE.  If the device
 *                                      handle is bad, returns
 *                                      UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  void TimerCallback(void *aWorkspace)
 *  {
 *
 *  }
 *
 *  T_uezDevice Timer;
 *  T_uezTimerCallback callback;
 *  T_uezError error;
 *
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *      callback.iTimer = timer;
 *      callback.iMatchRegister = 1;
 *      callback.iTriggerSem = 0;
 *      callback.iCallback = TimerCallback;
 *      if ( UEZTimerSetupOneShot(
 *                      timer,
 *                      1,
 *                      (PROCESSOR_OSCILLATOR_FREQUENCY)/1000,
 *                      &callback) == UEZ_ERROR_NONE){
 *          error = UEZTimerSetTimerMode(timer, TIMER_MODE_CLOCK);
 *          error = UEZTimerReset(timer);
 *          UEZTimerEnable(timer);
 *  } else {
 *      // an error occurred opening Timer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerSetupOneShot(
        T_uezDevice aTimer,
        TUInt32 aMatchRegister,
        TUInt32 aPeriodInCPUCycles,
        T_uezTimerCallback *aCallback)
{
    T_uezError error;
    DEVICE_Timer **p;

    error = UEZDeviceTableGetWorkspace(aTimer, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    aCallback->iTimer = aTimer;
    aCallback->iMatchRegister = aMatchRegister;
    error = (*p)->SetMatchCallback(p, aMatchRegister, IUEZTimerCallback,
            (void *)aCallback);
    if (error)
        return error;

    error = (*p)->SetMatchRegister(p, aMatchRegister, aPeriodInCPUCycles,
            ((aCallback) ? TIMER_MATCH_POINT_FUNC_INTERRUPT : 0)
                    | TIMER_MATCH_POINT_FUNC_STOP);
    if (error)
        return error;

    return error;
}


/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerSetupRegularInterval
 *---------------------------------------------------------------------------*/
/**
 *  E
 *
 *  @param [in]    aTimer               Pointer to Timer Device
 * *
 *  @param [in]    aMatchRegister       Match Register number
 *
 *  @param [in]    aPeriodInCPUCycles   Period in CPU cycles
 *
 *  @param [in]    *aCallback			Pointer to callback
 *
 *  @return        T_uezError           If the device is successfully closed,
 *                                      returns UEZ_ERROR_NONE.  If the device
 *                                      handle is bad, returns
 *                                      UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  void TimerCallback(void *aWorkspace)
 *  {
 *
 *  }
 *
 *  T_uezDevice Timer;
 *  T_uezTimerCallback callback;
 *  T_uezError error;
 *
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *      callback.iTimer = timer;
 *      callback.iMatchRegister = 1;
 *      callback.iTriggerSem = 0;
 *      callback.iCallback = TimerCallback;
 *      if ( UEZTimerSetupRegularInterval(
 *                      timer,
 *                      1,
 *                      (PROCESSOR_OSCILLATOR_FREQUENCY)/1000,
 *                      &callback) == UEZ_ERROR_NONE){
 *          error = UEZTimerSetTimerMode(timer, TIMER_MODE_CLOCK);
 *          error = UEZTimerReset(timer);
 *          UEZTimerEnable(timer);
 *  } else {
 *      // an error occurred opening Timer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerSetupRegularInterval(
        T_uezDevice aTimer,
        TUInt32 aMatchRegister,
        TUInt32 aPeriodInCPUCycles,
        T_uezTimerCallback *aCallback)
{
    T_uezError error;
    DEVICE_Timer **p;

    error = UEZDeviceTableGetWorkspace(aTimer, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    if (aCallback) {
        aCallback->iTimer = aTimer;
        aCallback->iMatchRegister = aMatchRegister;
        error = (*p)->SetMatchCallback(p, aMatchRegister, IUEZTimerCallback,
                (void *)aCallback);
        if (error)
            return error;
    }

    error = (*p)->SetMatchRegister(p, aMatchRegister, aPeriodInCPUCycles,
            ((aCallback) ? TIMER_MATCH_POINT_FUNC_INTERRUPT : 0)
                    | TIMER_MATCH_POINT_FUNC_RESET);
    if (error)
        return error;

    return error;
}


/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerEnable
 *---------------------------------------------------------------------------*/
/**
 *  Enable the specified timer.
 *
 *  @param [in]    aTimer			Pointer to Timer Device
 *
 *  @return        T_uezError     	If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  T_uezDevice Timer;
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *  
 *      UEZTimerDisable(Timer);
 *      UEZTimerEnable(Timer);
 *
 *  } else {
 *      // an error occurred opening Timer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerEnable(T_uezDevice aTimer)
{
    DEVICE_Timer **p;
    T_uezError error;

    error = UEZDeviceTableGetWorkspace(aTimer, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Enable(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerDisable
 *---------------------------------------------------------------------------*/
/**
 *  Disable the specified timer.
 *
 *  @param [in]    aTimer			Pointer to Timer Device
 *
 *  @return        T_uezError     	If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  T_uezDevice Timer;
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *  
 *      UEZTimerDisable(Timer);
 *      UEZTimerEnable(Timer);
 *
 *  } else {
 *      // an error occurred opening Timer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerDisable(T_uezDevice aTimer)
{
    DEVICE_Timer **p;
    T_uezError error;

    error = UEZDeviceTableGetWorkspace(aTimer, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Disable(p);
}


/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerReset
 *---------------------------------------------------------------------------*/
/**
 *  Reset the specified Timer.
 *
 *  @param [in]    aTimer			Pointer to Timer Device
 *
 *  @return        T_uezError     	If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  T_uezDevice Timer;
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *  
 *      UEZTimerReset(Timer);
 *
 *  } else {
 *      // an error occurred opening Timer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerReset(T_uezDevice aTimer)
{
    DEVICE_Timer **p;
    T_uezError error;

    error = UEZDeviceTableGetWorkspace(aTimer, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Reset(p);
}


/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerSetCaptureSource
 *---------------------------------------------------------------------------*/
/**
 *  Set the timer capture source
 *
 *  @param [in]    aTimer			Pointer to Timer Device
 
 *  @param [in]    aCaptureSource   The Capture Source
 *
 *  @return        T_uezError     	If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *   *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  void TimerCallback(void *aWorkspace)
 *  {
 *
 *  }
 *  
 *  T_uezDevice timer;
 *  T_uezTimerCallback callback;
 *  T_uezError error;
 *
 *  if ( UEZTimerOpen("Timer1", &timer) == UEZ_ERROR_NONE){
 *      callback.iTimer = timer;
 *      callback.iMatchRegister = 1;
 *      callback.iTriggerSem = 0;
 *      callback.iCallback = TimerCallback;
 *
 *      if ( UEZTimerSetupRegularInterval(
 *                      timer,
 *                      1,
 *                      (PROCESSOR_OSCILLATOR_FREQUENCY)/1000,
 *                      &callback) == UEZ_ERROR_NONE){
 *          error = UEZTimerSetExternalControl(timer, 1,
 *              TIMER_EXT_CONTROL_TOGGLE);
 *          error = UEZTimerSetCaptureSource(timer, 0);
 *          error = UEZTimerSetTimerMode(timer, TIMER_MODE_RISING_CAP);
 *          error = UEZTimerReset(timer);
 *          UEZTimerEnable(timer);
 *      }
 *  } else {
 *      //An error occurred.
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerSetCaptureSource(T_uezDevice aTimer, TUInt32 aCaptureSource)
{
    DEVICE_Timer **p;
    T_uezError error;

    error = UEZDeviceTableGetWorkspace(aTimer, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetCaptureSource(p, aCaptureSource);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerSetTimerMode
 *---------------------------------------------------------------------------*/
/**
 *  Set the operating mode of the timer.
 *
 *  @param [in]    aTimer			Pointer to Timer Device
 *
 *  @param [in]    aMode      		The Timer mode
 *
 *  @return        T_uezError     	If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  T_uezDevice Timer;
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *  
 *      if (UEZTimerSetTimerMode(Timer,TIMER_MODE_CLOCK) != UEZ_ERROR_NONE) {
 *          // an error occurred
 *      }
 *
 *  } else {
 *     // an error occurred opening Timer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerSetTimerMode(T_uezDevice aTimer, T_Timer_Mode aMode)
{
    DEVICE_Timer **p;
    T_uezError error;

    error = UEZDeviceTableGetWorkspace(aTimer, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetTimerMode(p, aMode);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTimerSetExternalControl
 *---------------------------------------------------------------------------*/
/**
 *  Set the External Timer Input controls
 *
 *  @param [in]    aTimer			Pointer to Timer Device
 * *
 *  @param [in]    aMatchRegister   Match Register number
 *
 *  @param [in]    aExtControl      T
 *
 *  @return        T_uezError     	If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimer.h>
 *
 *  T_uezDevice Timer;
 *  if (UEZTimerOpen("Timer0", &Timer) == UEZ_ERROR_NONE) {
 *  
 *      if (UEZTimerSetExternalControl(Timer, 0, TIMER_EXT_CONTROL_SET)
 *              != UEZ_ERROR_NONE) {
 *          // an error occurred
 *      }
 *
 *  } else {
 *      // an error occurred opening Timer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimerSetExternalControl(
        T_uezDevice aTimer,
        TUInt8 aMatchRegister,
        T_Timer_ExternalControl aExtControl)
{
    DEVICE_Timer **p;
    T_uezError error;

    error = UEZDeviceTableGetWorkspace(aTimer, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetExternalControl(p, aMatchRegister, aExtControl);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZTimer.c
 *-------------------------------------------------------------------------*/
