/*-------------------------------------------------------------------------*
 * File:  LPC1788_LCDController.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC1788_LCDCONTROLLER_H_
#define LPC1788_LCDCONTROLLER_H_

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
#include <HAL/LCDController.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezGPIOPortPin iPWR;
    T_uezGPIOPortPin iDCLK;
    T_uezGPIOPortPin iENAB_M;
    T_uezGPIOPortPin iFP;
    T_uezGPIOPortPin iLE;
    T_uezGPIOPortPin iLP;
    T_uezGPIOPortPin iVD[24];
    T_uezGPIOPortPin iCLKIN;
    
    T_uezGPIOPortPin iPowerPin;
    TBool iPowerPinIsActiveHigh;
    TUInt32 iPowerOnDelay;
} T_LPC1788_LCDController_Pins;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_LCDController LCDController_LPC1788_Interface;
void LPC1788_LCDController_Require(const T_LPC1788_LCDController_Pins *aPins);
extern T_uezError LCDController_LPC1788_ConfigurePowerPin(
            void *aWorkspace,
            HAL_GPIOPort **aPowerPort,
            TUInt32 aPowerPinIndex,
            TBool aPowerPinIsActiveHigh,
            TUInt32 aPowerOnDelay);

#endif // LPC1788_LCDCONTROLLER_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1788_LCDController.h
 *-------------------------------------------------------------------------*/
