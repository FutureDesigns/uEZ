/*-------------------------------------------------------------------------*
 * File:  LPC43xx_PWM.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC43xx_PWM_H_
#define LPC43xx_PWM_H_

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
extern const HAL_PWM PWM_LPC43xx_Port0_Interface;
extern const HAL_PWM PWM_LPC43xx_Port1_Interface;
void LPC43xx_PWM0_Require(void);
void LPC43xx_PWM1_Require(void);
void LPC43xx_PWM2_Require(void);
void LPC43xx_PWM0_A_Require(T_uezGPIOPortPin aPortPin);
void LPC43xx_PWM0_B_Require(T_uezGPIOPortPin aPortPin);
void LPC43xx_PWM1_A_Require(T_uezGPIOPortPin aPortPin);
void LPC43xx_PWM1_B_Require(T_uezGPIOPortPin aPortPin);
void LPC43xx_PWM2_A_Require(T_uezGPIOPortPin aPortPin);
void LPC43xx_PWM2_B_Require(T_uezGPIOPortPin aPortPin);


#endif // LPC43xx_PWM_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_PWM.h
 *-------------------------------------------------------------------------*/
