/*-------------------------------------------------------------------------*
 * File:  LPC1756_UtilityFuncs.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#ifndef LPC1756_UTILITYFUNCS_H_
#define LPC1756_UTILITYFUNCS_H_

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
#include <uEZ.h>
#include <uEZTypes.h>
#include <uEZGPIO.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        T_uezGPIOPortPin iPortPin;
        TUInt16 iSetting;
} T_LPC1756_IOCON_ConfigList;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void CPUDisableInterrupts(void);
void CPUEnableInterrupts(void);

void LPC1756PowerOn(TUInt32 bits);
void LPC1756PowerOff(TUInt32 bits);

TUInt32 ReadLE32U(volatile TUInt8 *pmem);
void WriteLE32U(volatile TUInt8 *pmem, TUInt32 val);
TUInt32 ReadBE32U(volatile TUInt8 *pmem);
void WriteBE32U(volatile TUInt8 *pmem, TUInt32 val);
TUInt16 ReadLE16U(volatile TUInt8 *pmem);
void WriteLE16U(volatile TUInt8 *pmem, TUInt16 val);
TUInt16  ReadBE16U (volatile  TUInt8  *pmem);
void WriteBE16U(volatile TUInt8 *pmem, TUInt16 val);

void LPC1756_IOCON_ConfigPin(
        T_uezGPIOPortPin aPortPin,
        const T_LPC1756_IOCON_ConfigList *aList,
        TUInt8 aCount);
void LPC1756_IOCON_ConfigPinOrNone(
        T_uezGPIOPortPin aPortPin,
        const T_LPC1756_IOCON_ConfigList *aList,
        TUInt8 aCount);

void UEZFailureMsg(const char *aLine);

#endif // LPC1756_UTILITYFUNCS_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_UtilityFuncs.h
 *-------------------------------------------------------------------------*/
