/*-------------------------------------------------------------------------*
 * File:  LPC1788_SSP.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC1788_SSP_H_
#define LPC1788_SSP_H_

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
#include <HAL/SPI.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_SPI SSP_LPC1788_Port0_Interface;
extern const HAL_SPI SSP_LPC1788_Port1_Interface;
extern const HAL_SPI SSP_LPC1788_Port2_Interface;

void LPC1788_SSP0_Require(
        T_uezGPIOPortPin aPinSCK0,
        T_uezGPIOPortPin aPinMISO0,
        T_uezGPIOPortPin aPinMOSI0,
        T_uezGPIOPortPin aPinSSEL0);
void LPC1788_SSP1_Require(
        T_uezGPIOPortPin aPinSCK1,
        T_uezGPIOPortPin aPinMISO1,
        T_uezGPIOPortPin aPinMOSI1,
        T_uezGPIOPortPin aPinSSEL1);
void LPC1788_SSP2_Require(
        T_uezGPIOPortPin aPinSCK2,
        T_uezGPIOPortPin aPinMISO2,
        T_uezGPIOPortPin aPinMOSI2,
        T_uezGPIOPortPin aPinSSEL2);

#endif // LPC1788_SSP_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1788_SSP.h
 *-------------------------------------------------------------------------*/
