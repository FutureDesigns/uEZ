/*-------------------------------------------------------------------------*
 * File:  RX62N_PWM_TMR.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef RX62N_PWM_TMR_H_
#define RX62N_PWM_TMR_H_

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

#include <HAL/PWM.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const HAL_PWM PWM_RX62N_TMR0_Interface;
extern const HAL_PWM PWM_RX62N_TMR1_Interface;
extern const HAL_PWM PWM_RX62N_TMR2_Interface;
extern const HAL_PWM PWM_RX62N_TMR3_Interface;

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum 
{
	TMR_DIV_1 		= 1,
	TMR_DIV_2 		= 2,
	TMR_DIV_8 		= 8,
	TMR_DIV_32 		= 32,
	TMR_DIV_64 		= 64,
	TMR_DIV_1024 	= 1024,
	TMR_DIV_8192 	= 8192
}TMR_PCLK_DIV;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void RX62N_PWM_TMR0_Require(void);
void RX62N_PWM_TMR1_Require(void);
void RX62N_PWM_TMR2_Require(void);
void RX62N_PWM_TMR3_Require(void);


#ifdef __cplusplus
}
#endif

#endif // RX62N_8BIT_TMR_H_
/*-------------------------------------------------------------------------*
 * End of File:  RX62N_8BIT_TMR.h
 *-------------------------------------------------------------------------*/
