/*-------------------------------------------------------------------------*
 * File:  LPC43xx_Serial.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC43xx_SERIAL_H_
#define LPC43xx_SERIAL_H_

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
extern const HAL_Serial G_LPC43xx_Serial_UART0;
extern const HAL_Serial G_LPC43xx_Serial_UART1;
extern const HAL_Serial G_LPC43xx_Serial_UART2;
extern const HAL_Serial G_LPC43xx_Serial_UART3;

void LPC43xx_UART0_Require(
        T_uezGPIOPortPin aPinTXD0,
        T_uezGPIOPortPin aPinRXD0,
        T_uezGPIOPortPin aPinDIR0,
        T_uezGPIOPortPin aPinUCLK0);
void LPC43xx_UART1_Require(
        T_uezGPIOPortPin aPinTXD1,
        T_uezGPIOPortPin aPinRXD1,
        T_uezGPIOPortPin aPinCTS,
        T_uezGPIOPortPin aPinDCD,
        T_uezGPIOPortPin aPinDSR,
        T_uezGPIOPortPin aPinDTR,
        T_uezGPIOPortPin aPinRI,
        T_uezGPIOPortPin aPinRTS);
void LPC43xx_UART2_Require(
        T_uezGPIOPortPin aPinTXD2,
        T_uezGPIOPortPin aPinRXD2,
        T_uezGPIOPortPin aPinDIR2,
        T_uezGPIOPortPin aPinUCLK2);
void LPC43xx_UART3_Require(
        T_uezGPIOPortPin aPinTXD3,
        T_uezGPIOPortPin aPinRXD3,
        T_uezGPIOPortPin aPinBAUD3,
        T_uezGPIOPortPin aPinDIR3,
        T_uezGPIOPortPin aPinUCLK3);

#endif // LPC43xx_SERIAL_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_Serial.h
 *-------------------------------------------------------------------------*/
