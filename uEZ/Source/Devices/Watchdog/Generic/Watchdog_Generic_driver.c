/*-------------------------------------------------------------------------*
 * File:  Watchdog_Generic_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a Watchdog sensor
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "Watchdog_Generic.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Watchdog *iDevice;
    const HAL_Watchdog **iWatchdog;
    T_uezSemaphore iSem;
    T_uezTask iFeedTask;
    TUInt32 iFeedInterval;
} T_Watchdog_Generic_Workspace;

#define IGrab()         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()      UEZSemaphoreRelease(p->iSem)

#ifndef WATCHDOG_GENERIC_AUTO_FEED_TASK_STACK_SIZE
    #define WATCHDOG_GENERIC_AUTO_FEED_TASK_STACK_SIZE \
                UEZ_TASK_STACK_BYTES(256)
#endif

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCF8563.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Watchdog_Generic_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Watchdog_Generic_Workspace *p = (T_Watchdog_Generic_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_SetMaximumTime
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the maximum amount of time that can go by before feeding the
 *      watchdog.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aMaxTime            -- Time til watchdog expires in
 *                                     milliseconds.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Watchdog_Generic_SetMaximumTime(void *aWorkspace, TUInt32 aMaxTime)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iWatchdog)->SetMaximumTime(p->iWatchdog, aMaxTime);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_SetMinimumTime
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the minimum amount of time that must go by before feeding the
 *      watchdog.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aMinTime            -- Time needed before feeding watchdog
 *                                     milliseconds.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Watchdog_Generic_SetMinimumTime(void *aWorkspace, TUInt32 aMaxTime)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iWatchdog)->SetMinimumTime(p->iWatchdog, aMaxTime);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_IsResetFromWatchdog
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the minimum amount of time that must go by before feeding the
 *      watchdog.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if reset was watchdog reset, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
TBool Watchdog_Generic_IsResetFromWatchdog(void *aWorkspace)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;
    TBool result;

    IGrab();
    result = (*p->iWatchdog)->IsResetFromWatchdog(p->iWatchdog);
    IRelease();

    return result;
}

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_ClearResetFlag
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear the state of the reset flag that says the last reboot
 *      was from a watchdog reset.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *---------------------------------------------------------------------------*/
void Watchdog_Generic_ClearResetFlag(void *aWorkspace)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;

    IGrab();
    (*p->iWatchdog)->ClearResetFlag(p->iWatchdog);
    IRelease();
}

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_IsActive
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if the watchdog is actively running.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if watchdog is running, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
TBool Watchdog_Generic_IsActive(void *aWorkspace)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;
    TBool result;

    IGrab();
    result = (*p->iWatchdog)->IsActive(p->iWatchdog);
    IRelease();

    return result;
}

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the watchdog running.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if watchdog is running, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
T_uezError Watchdog_Generic_Start(void *aWorkspace)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iWatchdog)->Start(p->iWatchdog);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_Feed
 *---------------------------------------------------------------------------*
 * Description:
 *      Feed the watchdog.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if watchdog is running, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
T_uezError Watchdog_Generic_Feed(void *aWorkspace)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iWatchdog)->Feed(p->iWatchdog);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_Trip
 *---------------------------------------------------------------------------*
 * Description:
 *      Purposely cause the watchdog to be tripped.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if watchdog is running, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
T_uezError Watchdog_Generic_Trip(void *aWorkspace)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();
    error = (*p->iWatchdog)->Trip(p->iWatchdog);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IWatchdogFeedAutomaticallyTask
 *---------------------------------------------------------------------------*
 * Description:
 *      Purposely cause the watchdog to be tripped.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if watchdog is running, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
static TUInt32 IWatchdogFeedAutomaticallyTask(
            T_uezTask aMyTask,
            void *aParameters)
{
    T_Watchdog_Generic_Workspace *p =
                (T_Watchdog_Generic_Workspace *)aParameters;
    PARAM_NOT_USED(aMyTask);

    // Just feed and sleep all day -- just like a big baby
    while (1) {
        // Sleep for a bit
        UEZTaskDelay(p->iFeedInterval);

        // Now feed using the standard feed routine
        Watchdog_Generic_Feed((void *)p);
    }
#if (COMPILER_TYPE!=IAR)
    return 0;
#endif
}
/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_FeedAutomatically
 *---------------------------------------------------------------------------*
 * Description:
 *      Purposely cause the watchdog to be tripped.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aFeedInterval       -- Number milliseconds between feedings
 * Outputs:
 *      T_uezError -- UEZ_ERROR_ALREADY_EXISTS if feeding automatically already,
 *                      else returns UEZ_ERROR_NONE or error code.
 *---------------------------------------------------------------------------*/
T_uezError Watchdog_Generic_FeedAutomatically(
        void *aWorkspace,
        TUInt32 aFeedInterval)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab();
    // Is there already a task running?
    if (!p->iFeedTask) {
        // Record the interval
        p->iFeedInterval = aFeedInterval;

        // Need to setup a task for feeding
        error = UEZTaskCreate(
                    IWatchdogFeedAutomaticallyTask,
                    "Watchdog",
                    WATCHDOG_GENERIC_AUTO_FEED_TASK_STACK_SIZE,
                    aWorkspace,
                    UEZ_PRIORITY_VERY_HIGH,
                    &p->iFeedTask);
    } else {
        // Feeding task already is running
        error = UEZ_ERROR_ALREADY_EXISTS;
    }
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Watchdog_Generic_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the watchdog driver's linkage
 * Inputs:
 *      void *aW                    -- Workspace
 *      HAL_Watchdog **aWatchdog    -- Watchdog interface to link to
 *---------------------------------------------------------------------------*/
void Watchdog_Generic_Configure(
        void *aWorkspace,
        const HAL_Watchdog **aWatchdog)
{
    T_Watchdog_Generic_Workspace *p =
            (T_Watchdog_Generic_Workspace *)aWorkspace;

    // Set the watchdog HAL driver
    p->iWatchdog = aWatchdog;
    p->iFeedTask = 0;
    p->iFeedInterval = 100; // 100 ms is default
}

void Watchdog_Generic_Create(const char *aName, const char *aHALName)
{
    T_uezDeviceWorkspace *p_watchdog;
    T_halWorkspace *p_watchdogHAL;

    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&Watchdog_Generic_Interface,
            0,
            &p_watchdog);
    HALInterfaceFind(aHALName, (T_halWorkspace **)&p_watchdogHAL);
    Watchdog_Generic_Configure(
            p_watchdog,
            ((const HAL_Watchdog **)p_watchdogHAL));
}
/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Watchdog Watchdog_Generic_Interface = { {
// Common device interface
        "Watchdog:Generic",
        0x0107,
        Watchdog_Generic_InitializeWorkspace,
        sizeof(T_Watchdog_Generic_Workspace), },

// v1.07 Functions
        Watchdog_Generic_SetMaximumTime,
        Watchdog_Generic_SetMinimumTime,
        Watchdog_Generic_IsResetFromWatchdog,
        Watchdog_Generic_ClearResetFlag,
        Watchdog_Generic_IsActive,
        Watchdog_Generic_Start,
        Watchdog_Generic_Feed,
        Watchdog_Generic_Trip,
        Watchdog_Generic_FeedAutomatically };

/*-------------------------------------------------------------------------*
 * End of File:  Watchdog_Generic_driver.c
 *-------------------------------------------------------------------------*/
