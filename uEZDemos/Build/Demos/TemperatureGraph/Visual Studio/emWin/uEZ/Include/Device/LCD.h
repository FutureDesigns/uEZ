/*-------------------------------------------------------------------------*
 * File:  LCD.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ LCD Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_LCD_H_
#define _DEVICE_LCD_H_

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
/**
 *  @file   /Include/Device/LCD.h
 *  @brief  uEZ LCD Device Interface
 *
 *  The uEZ LCD Device Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZLCD.h>
#include <uEZDevice.h>
#include <HAL/LCDController.h>
#include <HAL/PWM.h>
#include <Device/Backlight.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Open)(void *aWorkspace);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*Configure)(
                    void *aWorkspace, 
                    HAL_LCDController **aLCDController, 
                    TUInt32 aBaseAddress,
                    DEVICE_Backlight **aBacklight);
    T_uezError (*GetInfo)(void *aWorkspace, T_uezLCDConfiguration *aConfiguration);
    T_uezError (*GetFrame)(void *aWorkspace, TUInt32 aFrame, void **aFrameBuffer);
    T_uezError (*ShowFrame)(void *aWorkspace, TUInt32 aFrame);
    T_uezError (*On)(void *aWorkspace);
    T_uezError (*Off)(void *aWorkspace);
    T_uezError (*SetBacklightLevel)(void *aWorkspace, TUInt32 aLevel);
    T_uezError (*GetBacklightLevel)(
                    void *aWorkspace, 
                    TUInt32 *aLevel, 
                    TUInt32 *aNumLevels);
    T_uezError (*SetPaletteColor)(
                    void *aWorkspace, 
                    TUInt32 aColorIndex, 
                    TUInt16 aRed, 
                    TUInt16 aGreen, 
                    TUInt16 aBlue);
} DEVICE_LCD;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_LCD_H_
/*-------------------------------------------------------------------------*
 * End of File:  LCD.h
 *-------------------------------------------------------------------------*/
