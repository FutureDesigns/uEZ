/*-------------------------------------------------------------------------*
 * File:  RX63N_Serial.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef RX63N_SCI_H_
#define RX63N_SCI_H_

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

#include <HAL/Serial.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const HAL_Serial Serial_RX63N_SCI2_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void RX63N_SCI0_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI1_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI2_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI3_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI4_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI5_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI6_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI7_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI8_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI9_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI10_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI11_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
void RX63N_SCI12_Require(
	T_uezGPIOPortPin aPinTXD,
	T_uezGPIOPortPin aPinRXD);
	
#ifdef __cplusplus
}
#endif

#endif // RX63N_SCI_H_
/*-------------------------------------------------------------------------*
 * End of File:  RX63N_Serial.h
 *-------------------------------------------------------------------------*/
