/*-------------------------------------------------------------------------*
 * File:  SimpleUI_Types.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Utility routines to use for drawing graphics and showing pages
 *      of the LCD display.
 *-------------------------------------------------------------------------*/
#ifndef _SIMPLEUI_TYPES_H_
#define _SIMPLEUI_TYPES_H_
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
#include <Types/LCD.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>

// 8-bit pixels
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_8_BIT)
    typedef TUInt8 T_pixelColor;
    #define RGB(r, g, b) \
        ((((r>>5)&7)<<5)| \
        (((g>>5)&7)<<2)| \
        (((b>>6)&3)<<0))
#elif (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_16_BIT)
    typedef TUInt16 T_pixelColor;
    #define RGB(r, g, b)      \
        ( (((r>>3)&0x1F)<<11)| \
          (((g>>2)&0x3F)<<5)| \
          (((b>>3)&0x1F)<<0) )
#elif (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_I15_BIT)
    typedef TUInt16 T_pixelColor;
    #define RGB(r, g, b)      \
        ( (((r>>3)&0x1F)<<10)| \
          (((g>>3)&0x1F)<<5)| \
          (((b>>3)&0x1F)<<0) )
#else
    #error "Unknown screen pixel type!"
#endif

typedef struct {
    TBool iDoubleSizeIcons; // default is EFalse, ETrue for 2x
    TBool iFlipX;
    TBool iFlipY;
    SWIM_WINDOW_T iWindow;
} T_SUISettings;

#endif // _SIMPLEUI_TYPES_H_
/*-------------------------------------------------------------------------*
 * File:  SimpleUI_Types.c
 *-------------------------------------------------------------------------*/
