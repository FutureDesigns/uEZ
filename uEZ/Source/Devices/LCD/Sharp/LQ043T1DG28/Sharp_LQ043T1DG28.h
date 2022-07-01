/*-------------------------------------------------------------------------*
 * File:  Sharp_LQ043T1DG28.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#ifndef SHARP_LQ043T1DG28_H_
#define SHARP_LQ043T1DG28_H_

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
extern const T_uezDeviceInterface *LCD_SHARP_LQ043T1DG28_InterfaceArray[];
extern const DEVICE_LCD LCD_SHARP_LQ043T1DG28_Interface_8Bit;
extern const DEVICE_LCD LCD_SHARP_LQ043T1DG28_Interface_I15Bit;
extern const DEVICE_LCD LCD_SHARP_LQ043T1DG28_Interface_16Bit;
void LCD_LQ043T1DG28_Create(const char *aName,
                            char* aSPIBus,
                            T_uezGPIOPortPin aResetPin,
                            T_uezGPIOPortPin aShutPin,
                            T_uezGPIOPortPin aSPICSPin);

#ifdef __cplusplus
}
#endif

#endif // SHARP_LQ043T1DG28_H_
/*-------------------------------------------------------------------------*
 * End of File:  Sharp_LQ043T1DG28.h
 *-------------------------------------------------------------------------*/
