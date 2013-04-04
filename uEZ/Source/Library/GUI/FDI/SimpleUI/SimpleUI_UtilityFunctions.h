/*-------------------------------------------------------------------------*
 * File:  SimpleUI_UtilityFunctions.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Utility routines to use for drawing graphics and showing pages
 *      of the LCD display.
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
#ifndef _SIMPLEUI_UTILITY_FUNCTIONS_H_
#define _SIMPLEUI_UTILITY_FUNCTIONS_H_

#include <uEZ.h>
#include "SimpleUI.h"
#include <Source/Library/Graphics/SWIM/lpc_swim.h>

void SUICopyFast32(void *aDest, void *aSrc, TUInt32 aNumBytes);
T_uezError SUIScreenShot(SWIM_WINDOW_T *win, const char *aFilename);
void SUISetSettings(const T_SUISettings *aSettings);
void SUIInitialize(TBool aDoubleSize, TBool aFlipX, TBool aFlipY);
void SUISetDrawWindow(const SWIM_WINDOW_T *aWindow);
SWIM_WINDOW_T *SUIGetDrawWindow();

#endif // _SIMPLEUI_UTILITY_FUNCTIONS_H_
/*-------------------------------------------------------------------------*
 * File:  SimpleUI_UtilityFunctions.c
 *-------------------------------------------------------------------------*/
