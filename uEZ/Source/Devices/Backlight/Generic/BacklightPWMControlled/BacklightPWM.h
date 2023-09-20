/*-------------------------------------------------------------------------*
 * File:  BacklightPWMControlled.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef BACKLIGHTPWMCONTROLLED_H_
#define BACKLIGHTPWMCONTROLLED_H_

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


/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Device/Backlight.h>
#include <HAL/PWM.h>
#include <HAL/GPIO.h>
#include <uEZGPIO.h>

#ifdef __cplusplus
extern "C" {
#endif


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        // Name of HAL PWM driver to connect to
        const char *iHALPWMName;

        // Index to the PWM register/channel to connect to for master counting
        TUInt8 iMasterRegister;

        // Index to the PWM register/channel to connect to for backlight
        TUInt8 iBacklightRegister;

        // Period in cycles for complete cycle
        TUInt32 iPeriod;

        // Low power period
        TUInt32 iLowPeriod;

        // High power period
        TUInt32 iHighPeriod;

        // Pin to control power for this pin, or GPIO_NONE for no power pin
        T_uezGPIOPortPin iPowerPin;

        // ETrue if iPowerPin is active High, else EFalse
        TBool iIsPowerPinActiveHigh;
} T_backlightGenericPWMSettings;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_Backlight Backlight_Generic_PWMControlled_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError Generic_BacklightPWMControlled_Configure(
        void *aW,
        HAL_PWM **aPWM,
        TUInt8 aPWMMasterRegister,
        TUInt8 aPWMBacklightRegister,
        TUInt32 aPWMPeriod,
        TUInt32 aPWMLowPeriod,
        TUInt32 aPWMHighPeriod);
T_uezError Generic_BacklightPWMControlled_ConfigurePowerPin(
        void *aW,
        HAL_GPIOPort **aPort,
        TUInt32 iPinIndex,
        TBool iIsActiveHigh);
T_uezError Backlight_Generic_PWMControlled_Create(
        const char *aName,
        const T_backlightGenericPWMSettings *aSettings);
		
#ifdef __cplusplus
}
#endif

#endif // BACKLIGHTPWMCONTROLLED_H_
/*-------------------------------------------------------------------------*
 * End of File:  BacklightPWMControlled.h
 *-------------------------------------------------------------------------*/
