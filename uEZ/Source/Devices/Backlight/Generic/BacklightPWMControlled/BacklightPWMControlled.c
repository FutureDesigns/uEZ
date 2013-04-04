/*-------------------------------------------------------------------------*
 * File:  BacklightPWMControlled.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of any Backlight controlled with a single
 *      PWM source (including on/off).
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
#include <uEZRTOS.h>
#include <HAL/HAL.h>
#include <HAL/GPIO.h>
#include "BacklightPWM.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_Backlight *iDevice;

    T_uezSemaphore iSem;
    HAL_PWM **iPWM;
    TUInt8 iPWMMasterRegister;
    TUInt8 iPWMBacklightRegister;
    TUInt32 iPWMPeriod;
    TUInt32 iPWMLowPeriod;  // 0% and off
    TUInt32 iPWMHighPeriod; // 100% and on
    TUInt32 iPWMLastOnSetting;
    TBool iIsOn;
    HAL_GPIOPort **iPowerPort;
    TUInt32 iPowerPin;
    TBool iPowerIsActiveHigh;
} T_BacklightPWMControlled_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  ISetPWMLevel
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a SPI transfer (both read and write) out the SPI port using
 *      the given request.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 *      TUInt32 aLevel              -- Level of backlight PWM
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError BacklightPWMControlled_SetPWMLevel(
                        T_BacklightPWMControlled_Workspace *p, 
                        TUInt32 aLevel)
{
    if (aLevel > p->iPWMPeriod)
        aLevel = p->iPWMPeriod;

    // Now set to the backlight and return the error code
    return (*p->iPWM)->SetMatchRegister(
                p->iPWM,
                p->iPWMBacklightRegister,
                aLevel,
                EFalse,
                EFalse,
                EFalse);
}


/*---------------------------------------------------------------------------*
 * Routine:  Backlight_EA_InitializeWorkspace_Backlight
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a Backlight Bank called Backlight and semaphore to ensure single accesses
 *      at a time.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError BacklightPWMControlled_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_BacklightPWMControlled_Workspace *p = 
        (T_BacklightPWMControlled_Workspace *)aW;

    // No PWM declared yet
    p->iPWM = 0;
    p->iPWMLastOnSetting = 0;
    p->iIsOn = EFalse;
    p->iPowerPort = 0;
    p->iPowerPin = 0;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    return error;
}

T_uezError Generic_BacklightPWMControlled_On(void *aW)
{
    T_uezError error;
    T_BacklightPWMControlled_Workspace *p =
            (T_BacklightPWMControlled_Workspace *)aW;
    if (!p->iPWM)
        return UEZ_ERROR_DEVICE_NOT_FOUND;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    error = BacklightPWMControlled_SetPWMLevel(p, p->iPWMLastOnSetting);
    if (error == UEZ_ERROR_NONE) {
        p->iIsOn = ETrue;
        if (p->iPowerPort) {
            // Turn on the power pin
            if (p->iPowerIsActiveHigh)
                (*p->iPowerPort)->Set(p->iPowerPort, p->iPowerPin);
            else
                (*p->iPowerPort)->Clear(p->iPowerPort, p->iPowerPin);
        }
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Generic_BacklightPWMControlled_Off(void *aW)
{
    T_uezError error;
    T_BacklightPWMControlled_Workspace *p =
            (T_BacklightPWMControlled_Workspace *)aW;
    if (!p->iPWM)
        return UEZ_ERROR_DEVICE_NOT_FOUND;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    error = BacklightPWMControlled_SetPWMLevel(p, p->iPWMLowPeriod);
    if (error == UEZ_ERROR_NONE) {
        p->iIsOn = EFalse;
        if (p->iPowerPort) {
            // Turn off the power pin
            if (p->iPowerIsActiveHigh)
                (*p->iPowerPort)->Clear(p->iPowerPort, p->iPowerPin);
            else
                (*p->iPowerPort)->Set(p->iPowerPort, p->iPowerPin);
        }
    }
    UEZSemaphoreRelease(p->iSem);
    
    return UEZ_ERROR_NONE;
}

T_uezError Generic_BacklightPWMControlled_SetRatio(void *aW, TUInt16 aRatio)
{
    long delta;
    T_uezError error;
    T_BacklightPWMControlled_Workspace *p = 
        (T_BacklightPWMControlled_Workspace *)aW;
    if (!p->iPWM)
        return UEZ_ERROR_DEVICE_NOT_FOUND;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    aRatio >>= 8;

    // Calculate a percentage between high and low
    delta = p->iPWMHighPeriod - p->iPWMLowPeriod;     // May be negative number
    delta *= ((TInt32)aRatio);

    delta /= 255 ;
    delta += p->iPWMLowPeriod;

    error = BacklightPWMControlled_SetPWMLevel(p, delta);    
    if (error == UEZ_ERROR_NONE) {
        if (delta == p->iPWMLowPeriod) {
            p->iIsOn = EFalse;
            p->iPWMLastOnSetting = delta;
        } else {
            // Note we are on and remember the setting
            p->iIsOn = ETrue;
            p->iPWMLastOnSetting = delta;
        }
    }
    UEZSemaphoreRelease(p->iSem);
    
    return UEZ_ERROR_NONE;
}

T_uezError Generic_BacklightPWMControlled_Configure(
        void *aW, 
        HAL_PWM **aPWM,
        TUInt8 aPWMMasterRegister,
        TUInt8 aPWMBacklightRegister,
        TUInt32 aPWMPeriod,
        TUInt32 aPWMLowPeriod,
        TUInt32 aPWMHighPeriod)
{
    T_BacklightPWMControlled_Workspace *p = 
        (T_BacklightPWMControlled_Workspace *)aW;
    T_uezError error;

    p->iPWM = aPWM;
    p->iPWMMasterRegister = aPWMMasterRegister;
    p->iPWMBacklightRegister = aPWMBacklightRegister;
    p->iPWMPeriod = aPWMPeriod;
    p->iPWMLowPeriod = aPWMLowPeriod;  // 0% and off
    p->iPWMHighPeriod = aPWMHighPeriod; // 100% and on

    // Go ahead and setup the PWM's settings and return
    // any errors
    error = (*aPWM)->SetMaster(aPWM, aPWMPeriod, aPWMMasterRegister);
    if (error)
        return error;

    // Setup the backlight, but set to off
    error = (*aPWM)->EnableSingleEdgeOutput(aPWM, aPWMBacklightRegister);
    if (error)
        return error;

    // Now set to the backlight to off
    error = (*aPWM)->SetMatchRegister(
                aPWM,
                aPWMBacklightRegister,
                p->iPWMLowPeriod,
                EFalse,
                EFalse,
                EFalse);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

T_uezError Generic_BacklightPWMControlled_ConfigurePowerPin(
        void *aW,
        HAL_GPIOPort **aPort,
        TUInt32 iPinIndex,
        TBool iIsActiveHigh)
{
    T_BacklightPWMControlled_Workspace *p =
        (T_BacklightPWMControlled_Workspace *)aW;

    p->iPowerPort = aPort;
    p->iPowerPin = (1<<iPinIndex);
    p->iPowerIsActiveHigh = iIsActiveHigh;
    (*p->iPowerPort)->SetMux(p->iPowerPort, iPinIndex, 0);
    // Turn off the power pin
    if (p->iPowerIsActiveHigh)
        (*p->iPowerPort)->Clear(p->iPowerPort, p->iPowerPin);
    else
        (*p->iPowerPort)->Set(p->iPowerPort, p->iPowerPin);
    (*p->iPowerPort)->SetOutputMode(p->iPowerPort, p->iPowerPin);
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
T_uezError Backlight_Generic_PWMControlled_Create(
        const char *aName,
        const T_backlightGenericPWMSettings *aSettings)
{
    T_uezDeviceWorkspace *p_backlight;
    T_halWorkspace *p_pwm0;
    T_uezError error;

    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&Backlight_Generic_PWMControlled_Interface,
            0,
            &p_backlight);
    HALInterfaceFind(aSettings->iHALPWMName, (T_halWorkspace **)&p_pwm0);

    error = Generic_BacklightPWMControlled_Configure(
            p_backlight,
            (HAL_PWM **)p_pwm0,
            aSettings->iMasterRegister,
            aSettings->iBacklightRegister,
            aSettings->iPeriod,
            aSettings->iLowPeriod,
            aSettings->iHighPeriod);

    // Configure the power pin (if we have any)
    if ((aSettings->iPowerPin != GPIO_NONE) && (!error)) {
        error = Generic_BacklightPWMControlled_ConfigurePowerPin(p_backlight,
                UEZGPIOGetPortOrFail(UEZ_GPIO_PORT_FROM_PORT_PIN(
                        aSettings->iPowerPin)), UEZ_GPIO_PIN_FROM_PORT_PIN(
                        aSettings->iPowerPin), aSettings->iIsPowerPinActiveHigh);
    }
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Backlight Backlight_Generic_PWMControlled_Interface = {
	{
	    // Common device interface
	    "Backlight:Generic:PWM",
	    0x0100,
	    BacklightPWMControlled_InitializeWorkspace,
	    sizeof(T_BacklightPWMControlled_Workspace),
	},
	
    // Functions
    Generic_BacklightPWMControlled_On,
    Generic_BacklightPWMControlled_Off,
    Generic_BacklightPWMControlled_SetRatio
} ;

/*-------------------------------------------------------------------------*
 * End of File:  BacklightPWMControlled.c
 *-------------------------------------------------------------------------*/
