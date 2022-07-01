/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_UtilityFuncs.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC17xx_40xx_UTILITYFUNCS_H_
#define LPC17xx_40xx_UTILITYFUNCS_H_

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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZGPIO.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        T_uezGPIOPortPin iPortPin;
        TUInt32 iSetting;
} T_LPC17xx_40xx_IOCON_ConfigList;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void CPUDisableInterrupts(void);
void CPUEnableInterrupts(void);

void LPC17xx_40xxPowerOn(TUInt32 bits);
void LPC17xx_40xxPowerOff(TUInt32 bits);

TUInt32 ReadLE32U(volatile TUInt8 *pmem);
void WriteLE32U(volatile TUInt8 *pmem, TUInt32 val);
TUInt32 ReadBE32U(volatile TUInt8 *pmem);
void WriteBE32U(volatile TUInt8 *pmem, TUInt32 val);
TUInt16 ReadLE16U(volatile TUInt8 *pmem);
void WriteLE16U(volatile TUInt8 *pmem, TUInt16 val);
TUInt16  ReadBE16U (volatile  TUInt8  *pmem);
void WriteBE16U(volatile TUInt8 *pmem, TUInt16 val);

void LPC17xx_40xx_IOCON_ConfigPin(
        T_uezGPIOPortPin aPortPin,
        const T_LPC17xx_40xx_IOCON_ConfigList *aList,
        TUInt8 aCount);
void LPC17xx_40xx_IOCON_ConfigPinOrNone(
        T_uezGPIOPortPin aPortPin,
        const T_LPC17xx_40xx_IOCON_ConfigList *aList,
        TUInt8 aCount);

#ifdef __cplusplus
}
#endif
		
#endif // LPC17xx_40xx_UTILITYFUNCS_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_UtilityFuncs.h
 *-------------------------------------------------------------------------*/
