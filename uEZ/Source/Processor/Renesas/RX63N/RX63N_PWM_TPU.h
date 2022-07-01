/*-------------------------------------------------------------------------*
 * File:  RX63N_PWM_TPU.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef RX63N_PWM_TPU_H_
#define RX63N_PWM_TPU_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
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
extern const HAL_PWM PWM_RX63N_TPU0_Interface;
extern const HAL_PWM PWM_RX63N_TPU1_Interface;
//extern const HAL_PWM PWM_RX63N_TPU2_Interface;
//extern const HAL_PWM PWM_RX63N_TPU3_Interface;
//extern const HAL_PWM PWM_RX63N_TPU4_Interface;
//extern const HAL_PWM PWM_RX63N_TPU5_Interface;
//extern const HAL_PWM PWM_RX63N_TPU6_Interface;
//extern const HAL_PWM PWM_RX63N_TPU7_Interface;
//extern const HAL_PWM PWM_RX63N_TPU8_Interface;
//extern const HAL_PWM PWM_RX63N_TPU9_Interface;
//extern const HAL_PWM PWM_RX63N_TPU10_Interface;
//extern const HAL_PWM PWM_RX63N_TPU11_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void RX63N_PWM_TPU0_Require(T_uezGPIOPortPin aOutputPin);
void RX63N_PWM_TPU1_Require(T_uezGPIOPortPin aOutputPin);
//void RX63N_PWM_TPU2_Require(void);
//void RX63N_PWM_TPU3_Require(void);
//void RX63N_PWM_TPU4_Require(void);
//void RX63N_PWM_TPU5_Require(void);
//void RX63N_PWM_TPU6_Require(void);
//void RX63N_PWM_TPU7_Require(void);
//void RX63N_PWM_TPU8_Require(void);
//void RX63N_PWM_TPU9_Require(void);
//void RX63N_PWM_TPU10_Require(void);
//void RX63N_PWM_TPU11_Require(void);

#ifdef __cplusplus
}
#endif

#endif // RX63N_TPU_H_
/*-------------------------------------------------------------------------*
 * End of File:  RX63N_TPU.h
 *-------------------------------------------------------------------------*/
