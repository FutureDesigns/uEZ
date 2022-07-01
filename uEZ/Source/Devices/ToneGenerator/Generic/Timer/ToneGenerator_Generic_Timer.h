/*-------------------------------------------------------------------------*
 * File:  ToneGenerator_Generic_Timer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef TONEGENERATOR_GENERIC_Timer_H_
#define TONEGENERATOR_GENERIC_Timer_H_

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
#include <Device/Timer.h>
#include <HAL/GPIO.h>
#include "ToneGenerator_Generic_Timer.h"

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
        const char *iTimerName;
        TUInt8 iMatchRegister;
        T_uezGPIOPortPin iGPIOPortPin;
        T_gpioMux iGPIOAsGPIOMux;
        T_gpioMux iGPIOAsTimerMux;
} T_ToneGenerator_Generic_Timer_Settings;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_ToneGenerator ToneGenerator_Generic_Timer_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void ToneGenerator_Generic_Timer_Configure(
        void *aWorkspace,
        DEVICE_Timer **aTimer,
        TUInt8 aMatchRegister);
T_uezError ToneGenerator_Generic_Timer_Create(
        const char *aName,
        const T_ToneGenerator_Generic_Timer_Settings *aSettings);

#ifdef __cplusplus
}
#endif
		
#endif // TONEGENERATOR_GENERIC_Timer_H_
/*-------------------------------------------------------------------------*
 * End of File:  ToneGenerator_Generic_Timer.h
 *-------------------------------------------------------------------------*/
