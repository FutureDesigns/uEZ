/*-------------------------------------------------------------------------*
 * File:  ToneGenerator_Generic_Timer_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a ToneGenerator sensor 
 *-------------------------------------------------------------------------*/

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

#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZGPIO.h>
#include <uEZTimer.h>
#include "ToneGenerator_Generic_Timer.h"
#include <uEZProcessor.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_ToneGenerator *iDevice;
    T_uezSemaphore iSem;
    T_uezDevice iTimer;
    TUInt8 iMatchRegister;
    T_uezGPIOPortPin iGPIOPort;
    T_gpioMux iGPIOAsGPIOMux;
    T_gpioMux iGPIOAsTimerMux;
} T_ToneGenerator_Generic_Timer_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_Timer_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCF8563.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_Timer_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_ToneGenerator_Generic_Timer_Workspace *p =
        (T_ToneGenerator_Generic_Timer_Workspace *)aW;

    // Don't bother with GPIO port pin
    p->iGPIOPort = GPIO_NONE;
    p->iGPIOAsGPIOMux = 0;
    p->iGPIOAsTimerMux = 0;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_Timer_SetTone
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current Generic_Timer ToneGenerator reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_Timer_SetTone(
        void *aWorkspace, 
        TUInt32 aTonePeriod)
{
    T_uezError error;
    T_ToneGenerator_Generic_Timer_Workspace *p =
        (T_ToneGenerator_Generic_Timer_Workspace *)aWorkspace;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    while (1) {
        if (aTonePeriod == TONE_GENERATOR_OFF) {
            // Stop generating output (stop toggling the output pin)
            error = UEZTimerDisable(p->iTimer);
            if (error)
                break;
            error = UEZTimerSetExternalControl(p->iTimer, p->iMatchRegister,
                TIMER_EXT_CONTROL_DO_NOTHING);
            if (error)
                break;

            // Put the tone's pin back into GPIO mode
            if (p->iGPIOPort != GPIO_NONE)
                UEZGPIOSetMux(p->iGPIOPort, p->iGPIOAsGPIOMux);
        } else {
            // Set the time for the Timer interval
            error = UEZTimerSetupRegularInterval(p->iTimer, p->iMatchRegister, aTonePeriod
                / 2, 0);
            if (error)
                break;

            // Make sure it is based on the clock
            error = UEZTimerSetTimerMode(p->iTimer, TIMER_MODE_CLOCK);
            if (error)
                break;

            // Have it toggle the external clock
            error = UEZTimerSetExternalControl(p->iTimer, p->iMatchRegister,
                TIMER_EXT_CONTROL_TOGGLE);
            if (error)
                break;

            // Run it now
            error = UEZTimerReset(p->iTimer);
            error = UEZTimerEnable(p->iTimer);
            if (error)
                break;

            // Put the tone's pin back into Timer mode
            if (p->iGPIOPort != GPIO_NONE)
                UEZGPIOSetMux(p->iGPIOPort, p->iGPIOAsTimerMux);

        }
        break;
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Creation routines:
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_Timer_Create(
        const char *aName,
        const T_ToneGenerator_Generic_Timer_Settings *aSettings)
{
    T_ToneGenerator_Generic_Timer_Workspace *p;
    //T_halWorkspace *p_Timer;
    T_uezError error;

    error = UEZDeviceTableRegister(
                "Speaker",
                (T_uezDeviceInterface *)&ToneGenerator_Generic_Timer_Interface,
                0,
                (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;
    error = UEZTimerOpen(aSettings->iTimerName, &p->iTimer);
    if (!error) {
        p->iMatchRegister = aSettings->iMatchRegister;

        // Is this pin to be controlled for full off?
        if (aSettings->iGPIOPortPin != GPIO_NONE) {
            // Configure the pin so it can be turned off fully!
            p->iGPIOPort = aSettings->iGPIOPortPin;
            p->iGPIOAsGPIOMux = aSettings->iGPIOAsGPIOMux;
            p->iGPIOAsTimerMux = aSettings->iGPIOAsTimerMux;
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_Timer_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the tone generator (if necessary)
 * Inputs:
 *      void *aW                    -- Workspace
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_Timer_Open(void *aWorkspace)
{
    // Do nothing
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_Timer_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the tone generator (if necessary)
 * Inputs:
 *      void *aW                    -- Workspace
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_Timer_Close(void *aWorkspace)
{
    // Do nothing
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_ToneGenerator ToneGenerator_Generic_Timer_Interface = {
	{
	    // Common device interface
	    "ToneGenerator:NXP:Generic_Timer",
	    0x0100,
	    ToneGenerator_Generic_Timer_InitializeWorkspace,
	    sizeof(T_ToneGenerator_Generic_Timer_Workspace),
	},
	
    // Functions
    ToneGenerator_Generic_Timer_Open,
    ToneGenerator_Generic_Timer_Close,
    ToneGenerator_Generic_Timer_SetTone,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  ToneGenerator_Generic_Timer_driver.c
 *-------------------------------------------------------------------------*/
