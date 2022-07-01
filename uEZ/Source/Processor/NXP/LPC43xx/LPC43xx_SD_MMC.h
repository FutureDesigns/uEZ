/*-------------------------------------------------------------------------*
 * File:  LPC43xx_MCI.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#ifndef LPC43xx_MCI_H_
#define LPC43xx_MCI_H_

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
#include <uEZTypes.h>
#include <uEZGPIO.h>
#include <HAL/SD_MMC.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezGPIOPortPin iDAT0;
    T_uezGPIOPortPin iDAT1;
    T_uezGPIOPortPin iDAT2;
    T_uezGPIOPortPin iDAT3;
    T_uezGPIOPortPin iCLK;
    T_uezGPIOPortPin iCMD;
    T_uezGPIOPortPin iCardDetect;
    T_uezGPIOPortPin iWriteProtect;
    T_uezGPIOPortPin iPOW;
} T_LPC43xx_SD_MMC_Pins;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC43xx_SD_MMC_Require(const T_LPC43xx_SD_MMC_Pins *aPins);
extern const HAL_SD_MMC LPC43xx_SD_MMC_Interface;

#endif // LPC43xx_MCI_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_MCI.h
 *-------------------------------------------------------------------------*/
