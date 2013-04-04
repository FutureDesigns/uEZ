/*-------------------------------------------------------------------------*
 * File:  LPC1768_PWM.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#ifndef LPC1768_PWM_H_
#define LPC1768_PWM_H_

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <HAL/PWM.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_PWM PWM_LPC1768_Port0_Interface;
extern const HAL_PWM PWM_LPC1768_Port1_Interface;
void LPC1768_PWM0_Require(void);
void LPC1768_PWM0_1_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM0_2_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM0_3_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM0_4_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM0_5_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM0_6_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM0_CAP0_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM1_Require(void);
void LPC1768_PWM1_1_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM1_2_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM1_3_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM1_4_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM1_5_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM1_6_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM1_CAP0_Require(T_uezGPIOPortPin aPortPin);
void LPC1768_PWM1_CAP1_Require(T_uezGPIOPortPin aPortPin);

#endif // LPC1768_PWM_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1768_PWM.h
 *-------------------------------------------------------------------------*/
