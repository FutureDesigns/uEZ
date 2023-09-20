/*-------------------------------------------------------------------------*
 * File:  ToneGenerator_Generic_PWM_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a ToneGenerator sensor 
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
#include <uEZGPIO.h>
#include "ToneGenerator_Generic_PWM.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_ToneGenerator *iDevice;
    T_uezSemaphore iSem;
    DEVICE_PWM **iPWM;
    TUInt8 iMatchRegister;
    HAL_GPIOPort **iGPIOPort;
    TUInt32 iGPIOPortPinIndex;
    T_gpioMux iGPIOAsGPIOMux;
    T_gpioMux iGPIOAsPWMMux;
} T_ToneGenerator_Generic_PWM_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_PWM_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCF8563.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_PWM_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_ToneGenerator_Generic_PWM_Workspace *p = 
        (T_ToneGenerator_Generic_PWM_Workspace *)aW;

    // Don't bother with GPIO port pin
    p->iGPIOPort = 0;
    p->iGPIOPortPinIndex = 0;
    p->iGPIOAsGPIOMux = 0;
    p->iGPIOAsPWMMux = 0;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_PWM_SetTone
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current Generic_PWM ToneGenerator reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_PWM_SetTone(
        void *aWorkspace, 
        TUInt32 aTonePeriod)
{
    T_uezError error;
    T_ToneGenerator_Generic_PWM_Workspace *p = 
        (T_ToneGenerator_Generic_PWM_Workspace *)aWorkspace;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (aTonePeriod == TONE_GENERATOR_OFF) {
        // Stop generating output
        error = (*p->iPWM)->DisableOutput(p->iPWM, p->iMatchRegister);

        if (p->iGPIOPort) {
            // Put the tone's pin back into GPIO mode
            (*p->iGPIOPort)->SetMux(p->iGPIOPort, p->iGPIOPortPinIndex,
                p->iGPIOAsGPIOMux);
            (*p->iGPIOPort)->SetOutputMode(p->iGPIOPort, 1<<p->iGPIOPortPinIndex);
        }
    } else {
        /* TODO save the current tone setting here or just read the registers
         * to verify if it is running. Then on subsequent calls don't set the 
         * tone again so that you don't here a clicking noise.
         */

        // Set the time for the PWM interval
        error = (*p->iPWM)->SetMaster(
            p->iPWM, 
            aTonePeriod, 
            0);

        if (!error) {
            if (p->iGPIOPort) {
                // Put the tone's pin back into PWM mode
                (*p->iGPIOPort)->SetMux(p->iGPIOPort, p->iGPIOPortPinIndex,
                    p->iGPIOAsPWMMux);
            }

            // Set the frequency based on the processor speed
            error = (*p->iPWM)->SetMatchRegister(
                p->iPWM, 
                p->iMatchRegister,
                aTonePeriod>>1, // divide by two for full period
                EFalse,
                EFalse,
                EFalse);
            if (!error)
                error = (*p->iPWM)->EnableSingleEdgeOutput(p->iPWM, p->iMatchRegister);
        }
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_PWM_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the Generic_PWM's PWM setting.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_PWM **aPWM       -- PWM interface
 *---------------------------------------------------------------------------*/
void ToneGenerator_Generic_PWM_Configure(
        void *aWorkspace, 
        DEVICE_PWM **aPWM,
        TUInt8 aMatchRegister)
{
    T_ToneGenerator_Generic_PWM_Workspace *p = 
        (T_ToneGenerator_Generic_PWM_Workspace *)aWorkspace;

    // Set the PWM bus
    p->iPWM = aPWM;
    p->iMatchRegister = aMatchRegister;
}

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_PWM_ConfigurePortPin
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the Generic_PWM's port pin setting.  This is an extra
 *      feature of generic PWMs that forces the PWM pin to a low setting
 *      when the PWM is not playing.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_PWM **aPWM           -- PWM interface
 *      T_gpioMux aGPIOAsGPIOMux    -- Setting for pin as a GPIO port pin
 *      T_gpioMux aGPIOAsPWMMux     -- Setting for pin as a PWM port pin
 *---------------------------------------------------------------------------*/
void ToneGenerator_Generic_PWM_ConfigurePortPin(
        void *aWorkspace,
        HAL_GPIOPort **aGPIOPort,
        TUInt8 aGPIOPin,
        T_gpioMux aGPIOAsGPIOMux,
        T_gpioMux aGPIOAsPWMMux)
{
    T_ToneGenerator_Generic_PWM_Workspace *p =
        (T_ToneGenerator_Generic_PWM_Workspace *)aWorkspace;

    p->iGPIOPort = aGPIOPort;
    p->iGPIOPortPinIndex = aGPIOPin;
    p->iGPIOAsGPIOMux = aGPIOAsGPIOMux;
    p->iGPIOAsPWMMux = aGPIOAsPWMMux;
    (*p->iGPIOPort)->SetOutputMode(p->iGPIOPort, 1<<p->iGPIOPortPinIndex);
}

/*---------------------------------------------------------------------------*
 * Creation routines:
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_PWM_Create(
        const char *aName,
        const T_ToneGenerator_Generic_PWM_Settings *aSettings)
{
    T_uezDeviceWorkspace *p;
    T_halWorkspace *p_pwm;
    T_uezError error;

    error = UEZDeviceTableRegister(
                "Speaker",
                (T_uezDeviceInterface *)&ToneGenerator_Generic_PWM_Interface,
                0,
                &p);
    if (error)
        return error;
    HALInterfaceFind(aSettings->iHALPWMName, (T_halWorkspace **)&p_pwm);
    ToneGenerator_Generic_PWM_Configure(
            p,
            ((DEVICE_PWM **)p_pwm),
            aSettings->iMatchRegister);

    // Is this pin to be controlled for full off?
    if (aSettings->iPin != GPIO_NONE) {
        // Configure the pin so it can be turned off fully!
        ToneGenerator_Generic_PWM_ConfigurePortPin(p,
                UEZGPIOGetPortOrFail(
                        UEZ_GPIO_PORT_FROM_PORT_PIN(aSettings->iPin)),
                UEZ_GPIO_PIN_FROM_PORT_PIN(aSettings->iPin),
                aSettings->iGPIOAsGPIOMux, aSettings->iGPIOAsPWMMux);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_PWM_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the tone generator (if necessary)
 * Inputs:
 *      void *aW                    -- Workspace
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_PWM_Open(void *aWorkspace)
{
    // Do nothing
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ToneGenerator_Generic_PWM_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the tone generator (if necessary)
 * Inputs:
 *      void *aW                    -- Workspace
 *---------------------------------------------------------------------------*/
T_uezError ToneGenerator_Generic_PWM_Close(void *aWorkspace)
{
    // Do nothing
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_ToneGenerator ToneGenerator_Generic_PWM_Interface = {
	{
	    // Common device interface
	    "ToneGenerator:NXP:Generic_PWM",
	    0x0100,
	    ToneGenerator_Generic_PWM_InitializeWorkspace,
	    sizeof(T_ToneGenerator_Generic_PWM_Workspace),
	},
	
    // Functions
    ToneGenerator_Generic_PWM_Open,
    ToneGenerator_Generic_PWM_Close,
    ToneGenerator_Generic_PWM_SetTone,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  ToneGenerator_Generic_PWM_driver.c
 *-------------------------------------------------------------------------*/
