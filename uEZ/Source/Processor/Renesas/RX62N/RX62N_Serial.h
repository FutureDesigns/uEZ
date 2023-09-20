/*-------------------------------------------------------------------------*
 * File:  RX62N_Serial.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef RX62N_SCI_H_
#define RX62N_SCI_H_

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
extern const HAL_Serial Serial_RX62N_SCI0_Interface;
extern const HAL_Serial Serial_RX62N_SCI1_Interface;
extern const HAL_Serial Serial_RX62N_SCI2_Interface;
extern const HAL_Serial Serial_RX62N_SCI3_Interface;
extern const HAL_Serial Serial_RX62N_SCI5_Interface;
extern const HAL_Serial Serial_RX62N_SCI6_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void RX62N_SCI0_Require(void);
void RX62N_SCI1_A_Require(void);
void RX62N_SCI1_B_Require(void);
void RX62N_SCI2_A_Require(void);
void RX62N_SCI2_B_Require(void);
void RX62N_SCI3_A_Require(void);
void RX62N_SCI3_B_Require(void);
void RX62N_SCI5_Require(void);
void RX62N_SCI6_A_Require(void);
void RX62N_SCI6_B_Require(void);

#ifdef __cplusplus
}
#endif

#endif // RX62N_SCI_H_
/*-------------------------------------------------------------------------*
 * End of File:  RX62N_Serial.h
 *-------------------------------------------------------------------------*/
