/*-------------------------------------------------------------------------*
 * File:  SimpleUI_UtilityFunctions.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Utility routines to use for drawing graphics and showing pages
 *      of the LCD display.
 *-------------------------------------------------------------------------*/
#ifndef _SIMPLEUI_UTILITY_FUNCTIONS_H_
#define _SIMPLEUI_UTILITY_FUNCTIONS_H_

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

#include <uEZ.h>
#include "SimpleUI.h"
#include <Source/Library/Graphics/SWIM/lpc_swim.h>

#ifdef __cplusplus
extern "C" {
#endif

void SUICopyFast32(void *aDest, void *aSrc, TUInt32 aNumBytes);
T_uezError SUIScreenShot(SWIM_WINDOW_T *win, const char *aFilename);
void SUISetSettings(const T_SUISettings *aSettings);
void SUIInitialize(TBool aDoubleSize, TBool aFlipX, TBool aFlipY);
void SUISetDrawWindow(const SWIM_WINDOW_T *aWindow);
SWIM_WINDOW_T *SUIGetDrawWindow();

#ifdef __cplusplus
}
#endif

#endif // _SIMPLEUI_UTILITY_FUNCTIONS_H_
/*-------------------------------------------------------------------------*
 * File:  SimpleUI_UtilityFunctions.c
 *-------------------------------------------------------------------------*/
