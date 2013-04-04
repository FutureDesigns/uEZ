/*-------------------------------------------------------------------------*
 * File:  LPC1756_Serial.h
 *-------------------------------------------------------------------------*/
#ifndef LPC1756_SERIAL_H_
#define LPC1756_SERIAL_H_

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
/**
 *  @file   LPC1756_Serial.h
 *  @brief  uEZ 1756 Serial
 *
 *  uEZ 1756 Serial
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <HAL/Serial.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_Serial G_LPC1756_Serial_UART0;
extern const HAL_Serial G_LPC1756_Serial_UART1;
extern const HAL_Serial G_LPC1756_Serial_UART2;
extern const HAL_Serial G_LPC1756_Serial_UART3;

void LPC1756_UART0_Require(
        T_uezGPIOPortPin aPinTXD0,
        T_uezGPIOPortPin aPinRXD0);
void LPC1756_UART1_Require(
        T_uezGPIOPortPin aPinTXD1,
        T_uezGPIOPortPin aPinRXD1,
        T_uezGPIOPortPin aPinCTS,
        T_uezGPIOPortPin aPinDCD,
        T_uezGPIOPortPin aPinDSR,
        T_uezGPIOPortPin aPinDTR,
        T_uezGPIOPortPin aPinRI,
        T_uezGPIOPortPin aPinRTS);
void LPC1756_UART2_Require(
        T_uezGPIOPortPin aPinTXD2,
        T_uezGPIOPortPin aPinRXD2);
void LPC1756_UART3_Require(
        T_uezGPIOPortPin aPinTXD3,
        T_uezGPIOPortPin aPinRXD3);

#endif // LPC1756_SERIAL_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_Serial.h
 *-------------------------------------------------------------------------*/
