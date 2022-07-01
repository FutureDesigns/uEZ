/*-------------------------------------------------------------------------*
 * File:  Timer_Generic.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZRTOS.h>
#include <HAL/HAL.h>
#include <HAL/Timer.h>
#include <uEZDeviceTable.h>
#include "Timer_Generic.h"

/*---------------------------------------------------------------------------*
 * Constants/Macros:
 *---------------------------------------------------------------------------*/
#define IGrab()         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()      UEZSemaphoreRelease(p->iSem)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Timer *iDevice;
    HAL_Timer **iTimer;
    T_uezSemaphore iSem;
    T_DeviceTimer_Callback iCallbackFunc;
    void *iCallbackWorkspace;
} T_Timer_Generic_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  Timer_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a Timer  and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Timer_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Timer_Generic_Workspace *p = (T_Timer_Generic_Workspace *)aW;

    p->iCallbackFunc = 0;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Timer_Generic_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a Timer and link to the HAL equivalent
 * Inputs:
 *      const char *aHALDriver -- Name of HAL driver to link to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Timer_Generic_Create(const char *aDeviceName, const char *aHALDriver)
{
    T_uezError error;
    T_Timer_Generic_Workspace *p;

    // Create the timer device
    error = UEZDeviceTableRegister(aDeviceName,
            (T_uezDeviceInterface *)&Timer_Generic_Interface, 0,
            (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Find the HAL and link to this device
    error = HALInterfaceFind(aHALDriver, (T_halWorkspace **)&p->iTimer);
    if (error)
        return error;

    return error;
}

T_uezError Timer_Generic_Reset(void *aWorkspace)
{
    T_Timer_Generic_Workspace *p = (T_Timer_Generic_Workspace *)aWorkspace;

    IGrab();
    (*p->iTimer)->Reset(p->iTimer);
    IRelease();

    return UEZ_ERROR_NONE;
}
T_uezError Timer_Generic_Enable(void *aWorkspace)
{
    T_Timer_Generic_Workspace *p = (T_Timer_Generic_Workspace *)aWorkspace;

    IGrab();
    (*p->iTimer)->Enable(p->iTimer);
    IRelease();

    return UEZ_ERROR_NONE;
}
T_uezError Timer_Generic_Disable(void *aWorkspace)
{
    T_Timer_Generic_Workspace *p = (T_Timer_Generic_Workspace *)aWorkspace;

    IGrab();
    (*p->iTimer)->Disable(p->iTimer);
    IRelease();

    return UEZ_ERROR_NONE;
}
T_uezError Timer_Generic_SetTimerMode(void *aWorkspace, T_Timer_Mode aMode)
{
    T_Timer_Generic_Workspace *p = (T_Timer_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iTimer)->SetTimerMode(p->iTimer, aMode);
    IRelease();

    return error;
}
T_uezError Timer_Generic_SetCaptureSource(
        void *aWorkspace,
        TUInt32 aSourceIndex)
{
    T_Timer_Generic_Workspace *p = (T_Timer_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iTimer)->SetCaptureSource(p->iTimer, aSourceIndex);
    IRelease();

    return error;
}
T_uezError Timer_Generic_SetMatchRegister(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        T_Timer_MatchPointFunction aFunctions)
{
    T_Timer_Generic_Workspace *p = (T_Timer_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iTimer)->SetMatchRegisterFunctions(p->iTimer, aMatchRegister,
            aFunctions, aMatchPoint);
    IRelease();

    return error;
}
T_uezError Timer_Generic_SetExternalControl(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        T_Timer_ExternalControl aExtControl)
{
    T_Timer_Generic_Workspace *p = (T_Timer_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iTimer)->SetExternalControl(p->iTimer, aMatchRegister,
            aExtControl);
    IRelease();

    return error;
}

static void ITimerGenericCallback(void *aCallbackWorkspace)
{
    T_Timer_Generic_Workspace *p =
            (T_Timer_Generic_Workspace *)aCallbackWorkspace;

    // Call the callback function with the callbackworkspce
    p->iCallbackFunc(p->iCallbackWorkspace);
}

T_uezError Timer_Generic_SetMatchCallback(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        T_DeviceTimer_Callback aCallbackFunc,
        void *aCallbackWorkspace)
{
    T_Timer_Generic_Workspace *p = (T_Timer_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    p->iCallbackFunc = aCallbackFunc;
    p->iCallbackWorkspace = aCallbackWorkspace;
    error = (*p->iTimer)->SetMatchCallback(p->iTimer, aMatchRegister,
            ITimerGenericCallback, p);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Timer Timer_Generic_Interface = {

{
// Common device interface
        "Timer:Generic",
        0x0100,
        Timer_Generic_InitializeWorkspace,
        sizeof(T_Timer_Generic_Workspace),

},

// Functions
        Timer_Generic_Reset,
        Timer_Generic_Enable,
        Timer_Generic_Disable,
        Timer_Generic_SetTimerMode,
        Timer_Generic_SetCaptureSource,
        Timer_Generic_SetMatchRegister,
        Timer_Generic_SetExternalControl,
        Timer_Generic_SetMatchCallback,

};
/*-------------------------------------------------------------------------*
 * End of File:  Timer_Generic.c
 *-------------------------------------------------------------------------*/
