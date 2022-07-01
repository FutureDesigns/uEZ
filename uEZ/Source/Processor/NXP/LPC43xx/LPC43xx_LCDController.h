/*-------------------------------------------------------------------------*
 * File:  LPC43xx_LCDController.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC43xx_LCDCONTROLLER_H_
#define LPC43xx_LCDCONTROLLER_H_

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
} T_LPC43xx_LCDController_Pins;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_LCDController LCDController_LPC43xx_Interface;
void LPC43xx_LCDController_Require(const T_LPC43xx_LCDController_Pins *aPins);
extern T_uezError LCDController_LPC43xx_ConfigurePowerPin(
            void *aWorkspace,
            HAL_GPIOPort **aPowerPort,
            TUInt32 aPowerPinIndex,
            TBool aPowerPinIsActiveHigh,
            TUInt32 aPowerOnDelay);

#endif // LPC43xx_LCDCONTROLLER_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_LCDController.h
 *-------------------------------------------------------------------------*/
