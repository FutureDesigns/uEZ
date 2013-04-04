/*-------------------------------------------------------------------------*
 * File:  uEZDemoCommon.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Common library features of all FDI uEZ Demos
 *-------------------------------------------------------------------------*/
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
#include <uEZ.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/Graphics/SWIM/lpc_rom8x8.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include <uEZFile.h>
#include "UDCImages.h"
#include "Slideshow.h"
#include "AppMenu.h"
#include "Calibration.h"

#define DISPLAY_WIDTH       UEZ_LCD_DISPLAY_WIDTH
#define DISPLAY_HEIGHT      UEZ_LCD_DISPLAY_HEIGHT

void AccelDemoMode(const T_choice *aChoice);
// Accelerometer Demo Options:
#ifndef ACCEL_DEMO_SWAP_XY
    #define ACCEL_DEMO_SWAP_XY          0
#endif
#ifndef ACCEL_DEMO_FLIP_X
    #define ACCEL_DEMO_FLIP_X           0
#endif
#ifndef ACCEL_DEMO_FLIP_Y
    #define ACCEL_DEMO_FLIP_Y           0
#endif
#ifndef ACCEL_DEMO_G_TO_PIXELS
    #define ACCEL_DEMO_G_TO_PIXELS      128
#endif
#ifndef ACCEL_DEMO_ALLOW_ROTATE
    #define ACCEL_DEMO_ALLOW_ROTATE     1
#endif

void DrawMode(const T_choice *aChoice);

void MultiSlideshowMode(const T_choice *aChoice);

void SingleSlideshowMode(T_slideshowDefinition *aDef);

void ConsoleMode(const T_choice *aChoice);
#ifndef CONSOLE_ROOT_DIRECTORY
    #define CONSOLE_ROOT_DIRECTORY      "0:/"
#endif

void TimeDateMode(const T_choice *aChoice);

void AppSubmenu(const T_choice *aChoice);
void AppMenu(const T_appMenu *aMenu);

void YourAppMode(const T_choice *aChoice);

void BrightnessControlMode(const T_choice *aChoice);

void TempMode(const T_choice *aChoice);

void AudioPlayerMode(const T_choice *aChoice);

/*-------------------------------------------------------------------------*
 * File:  uEZDemoCommon.h
 *-------------------------------------------------------------------------*/
