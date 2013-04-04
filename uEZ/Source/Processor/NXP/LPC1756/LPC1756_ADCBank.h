/*-------------------------------------------------------------------------*
 * File:  LPC1756_ADCBank.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#ifndef LPC1756_ADCBANK_H_
#define LPC1756_ADCBANK_H_

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
#include <HAL/ADCBank.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef TUInt8 T_LPC1756_ADC_UsedChannels;
#define LPC1756_ADC0_USE_CHANNEL0       (1<<0)
#define LPC1756_ADC0_USE_CHANNEL1       (1<<1)
#define LPC1756_ADC0_USE_CHANNEL2       (1<<2)
#define LPC1756_ADC0_USE_CHANNEL3       (1<<3)
#define LPC1756_ADC0_USE_CHANNEL4       (1<<4)
#define LPC1756_ADC0_USE_CHANNEL5       (1<<5)
#define LPC1756_ADC0_USE_CHANNEL6       (1<<6)
#define LPC1756_ADC0_USE_CHANNEL7       (1<<7)

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_ADCBank ADC_LPC1756_ADC0_Interface;

void LPC1756_ADC0_Require(void);
void LPC1756_ADC0_2_Require(T_uezGPIOPortPin aPin);
void LPC1756_ADC0_3_Require(T_uezGPIOPortPin aPin);
void LPC1756_ADC0_4_Require(T_uezGPIOPortPin aPin);
void LPC1756_ADC0_5_Require(T_uezGPIOPortPin aPin);
void LPC1756_ADC0_6_Require(T_uezGPIOPortPin aPin);
void LPC1756_ADC0_7_Require(T_uezGPIOPortPin aPin);

#endif // LPC1756_ADCBANK_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_ADCBank.h
 *-------------------------------------------------------------------------*/
