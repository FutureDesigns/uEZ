/*-------------------------------------------------------------------------*
 * File:  Drawing.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Utility routines to use for drawing graphics and showing pages
 *      of the LCD display.
 *-------------------------------------------------------------------------*/
#ifndef _SIMPLEUI_DRAWING_H_
#define _SIMPLEUI_DRAWING_H_

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

#include "SimpleUI_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

void SUIHidePage0(void);
void SUIShowPage0(void);
void SUIShowPage0Fancy(void);
#define SUIShowPage0FancyUp() SUIShowPage0Fancy()
void SUIShowPage0FancyDown(void);
void SUIDrawPixel(
    TUInt32 x,
    TUInt32 y,
    T_pixelColor color);
void SUIFillRect(
    T_pixelColor *pixels,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    T_pixelColor color);
void SUIDrawIcon(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset);
void SUIDrawImage(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset);
int32_t SUILoadPicture(
    char *aPicture,
    TUInt8 aPage,
    TBool *aAbortFlag,
    TUInt8 *aLoadAddr);
void SUICopyRGBRaster(
    TUInt8 aPage,
    TUInt16 x,
    TUInt16 y,
    TUInt16 aNumPixels,
    TUInt8 *aRGB);
TUInt16 SUIGetPointRaw(TUInt8 aPage, TUInt16 x, T_pixelColor y);

#ifdef __cplusplus
}
#endif

#endif // _SIMPLEUI_DRAWING_H_
/*-------------------------------------------------------------------------*
 * File:  Drawing.c
 *-------------------------------------------------------------------------*/
