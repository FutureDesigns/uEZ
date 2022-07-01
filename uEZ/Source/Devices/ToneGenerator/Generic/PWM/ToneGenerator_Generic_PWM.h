/*-------------------------------------------------------------------------*
 * File:  ToneGenerator_Generic_PWM.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef TONEGENERATOR_GENERIC_PWM_H_
#define TONEGENERATOR_GENERIC_PWM_H_

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
#include <Device/ToneGenerator.h>
#include <Device/PWM.h>
#include <HAL/GPIO.h>
#include "ToneGenerator_Generic_PWM.h"

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
        const char *iHALPWMName;
        TUInt8 iMatchRegister;
        T_uezGPIOPortPin iPin;
        T_gpioMux iGPIOAsGPIOMux;
        T_gpioMux iGPIOAsPWMMux;
} T_ToneGenerator_Generic_PWM_Settings;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_ToneGenerator ToneGenerator_Generic_PWM_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void ToneGenerator_Generic_PWM_Configure(
        void *aWorkspace,
        DEVICE_PWM **aPWM,
        TUInt8 aMatchRegister);
void ToneGenerator_Generic_PWM_ConfigurePortPin(
        void *aWorkspace,
        HAL_GPIOPort **aGPIOPort,
        TUInt8 aGPIOPin,
        T_gpioMux aGPIOAsGPIOMux,
        T_gpioMux aGPIOAsPWMMux);
T_uezError ToneGenerator_Generic_PWM_Create(
        const char *aName,
        const T_ToneGenerator_Generic_PWM_Settings *aSettings);
		
#ifdef __cplusplus
}
#endif

#endif // TONEGENERATOR_GENERIC_PWM_H_
/*-------------------------------------------------------------------------*
 * End of File:  ToneGenerator_Generic_PWM.h
 *-------------------------------------------------------------------------*/
