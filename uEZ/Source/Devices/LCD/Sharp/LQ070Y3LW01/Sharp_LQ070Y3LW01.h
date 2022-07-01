/*-------------------------------------------------------------------------*
 * File:  Sharp_LQ070Y3LW01.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef SHARP_LQ070Y3LW01_H_
#define SHARP_LQ070Y3LW01_H_

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
#include <Device/LCD.h>
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

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const T_uezDeviceInterface *LCD_SHARP_LQ070Y3LW01_InterfaceArray[];
extern const DEVICE_LCD LCD_SHARP_LQ070Y3LW01_Interface_8Bit;
extern const DEVICE_LCD LCD_SHARP_LQ070Y3LW01_Interface_I15Bit;
extern const DEVICE_LCD LCD_SHARP_LQ070Y3LW01_Interface_16Bit;

void LCD_LQ070Y3LW01_Create(char* aName,  
                              T_uezGPIOPortPin aLVDSShutPin,
                              T_uezGPIOPortPin aLCDPowerEnablePin,
                              T_uezGPIOPortPin aLCDPWMPin,
                              T_uezGPIOPortPin aBackLightEnablePin);
							  
#ifdef __cplusplus
}
#endif

#endif // SHARP_LQ070Y3LW01_H_
/*-------------------------------------------------------------------------*
 * End of File:  Sharp_LQ070Y3LW01.h
 *-------------------------------------------------------------------------*/
