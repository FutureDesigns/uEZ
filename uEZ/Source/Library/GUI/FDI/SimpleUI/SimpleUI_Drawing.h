/*-------------------------------------------------------------------------*
 * File:  Drawing.h
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
#ifndef _SIMPLEUI_DRAWING_H_
#define _SIMPLEUI_DRAWING_H_

#include "SimpleUI_Types.h"

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
    int x1,
    int y1,
    int x2,
    int y2,
    T_pixelColor color);
void SUIDrawIcon(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset);
void SUIDrawImage(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset);
int SUILoadPicture(
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

#endif // _SIMPLEUI_DRAWING_H_
/*-------------------------------------------------------------------------*
 * File:  Drawing.c
 *-------------------------------------------------------------------------*/
