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


#include <uEZ.h>
#include <Types/LCD.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _SIMPLEUI_TYPES_H_
/*-------------------------------------------------------------------------*
 * File:  SimpleUI_Types.c
 *-------------------------------------------------------------------------*/
