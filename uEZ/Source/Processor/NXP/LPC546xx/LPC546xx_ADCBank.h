/*-------------------------------------------------------------------------*
 * File:  LPC546xx_ADCBank.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC546xx_ADCBANK_H_
#define LPC546xx_ADCBANK_H_

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
#include <HAL/ADCBank.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef TUInt8 T_LPC546xx_ADC_UsedChannels;
#define LPC546xx_ADC0_USE_CHANNEL0       (1<<0)
#define LPC546xx_ADC0_USE_CHANNEL1       (1<<1)
#define LPC546xx_ADC0_USE_CHANNEL2       (1<<2)
#define LPC546xx_ADC0_USE_CHANNEL3       (1<<3)
#define LPC546xx_ADC0_USE_CHANNEL4       (1<<4)
#define LPC546xx_ADC0_USE_CHANNEL5       (1<<5)
#define LPC546xx_ADC0_USE_CHANNEL6       (1<<6)
#define LPC546xx_ADC0_USE_CHANNEL7       (1<<7)
#define LPC546xx_ADC0_USE_CHANNEL8       (1<<8)
#define LPC546xx_ADC0_USE_CHANNEL9       (1<<9)
#define LPC546xx_ADC0_USE_CHANNEL10      (1<<10)
#define LPC546xx_ADC0_USE_CHANNEL11      (1<<11)

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_ADCBank ADC_LPC546xx_ADC0_Interface;

void LPC546xx_ADC0_Require(void);
void LPC546xx_ADC1_Require(void);
void LPC546xx_ADC0_0_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_1_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_2_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_3_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_4_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_5_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_6_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_7_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_8_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_9_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_10_Require(T_uezGPIOPortPin aPin);
void LPC546xx_ADC0_11_Require(T_uezGPIOPortPin aPin);

#endif // LPC546xx_ADCBANK_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC546xx_ADCBank.h
 *-------------------------------------------------------------------------*/
