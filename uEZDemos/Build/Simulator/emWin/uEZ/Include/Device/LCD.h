/*-------------------------------------------------------------------------*
 * File:  LCD.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ LCD Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_LCD_H_
#define _DEVICE_LCD_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
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

#endif // _DEVICE_LCD_H_
/*-------------------------------------------------------------------------*
 * End of File:  LCD.h
 *-------------------------------------------------------------------------*/
