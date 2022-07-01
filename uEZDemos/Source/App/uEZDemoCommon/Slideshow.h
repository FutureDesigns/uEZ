/*-------------------------------------------------------------------------*
 * File:  Slideshow.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Generic slideshow information
 *-------------------------------------------------------------------------*/
#ifndef _SLIDESHOW_H_
#define _SLIDESHOW_H_

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

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NUMBER_OF_SLIDESHOWS        20
#define MAX_SLIDESHOW_DEF_NAME_LEN      80
#define MAX_SLIDESHOW_DEF_DIR_LEN       40

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
    #define SLIDESHOW_ICON_HEIGHT     64
    #define SLIDESHOW_ICON_WIDTH      64
    #define EXIT_BUTTON_HEIGHT        64
    #define EXIT_BUTTON_WIDTH         128
#else
    #define SLIDESHOW_ICON_HEIGHT     32
    #define SLIDESHOW_ICON_WIDTH      32
    #define EXIT_BUTTON_HEIGHT        32
    #define EXIT_BUTTON_WIDTH         64
#endif

typedef struct {
    TUInt32 iDrive;
    char iName[MAX_SLIDESHOW_DEF_NAME_LEN+1];
    char iDirectory[MAX_SLIDESHOW_DEF_DIR_LEN+1];
} T_slideshowDefinition;

typedef struct {
    TUInt32 iCount;
    T_slideshowDefinition iList[MAX_NUMBER_OF_SLIDESHOWS];
} T_slideshowList;

#define SLIDES_DIRECTORY                    "SLIDES/%s/"
#if UEZ_DEFAULT_LCD_RES_VGA
    #define SLIDESHOW_PICTURE_NAMING        "%sVSLIDE%02d.TGA"
    #define SLIDESHOW_BACKWARDS_COMPATIBLE  0
    #define SLIDESHOW_AUDIO_NAMING        "%sVSLIDE%02d.WAV"
#endif
#if UEZ_DEFAULT_LCD_RES_QVGA
    #define SLIDESHOW_PICTURE_NAMING        "%sQSLIDE%02d.TGA"
    #define SLIDESHOW_BACKWARDS_COMPATIBLE  1
    #define SLIDESHOW_BACKWARD_NAMING       "%sPICTURE%d.TGA"
    #define SLIDESHOW_AUDIO_NAMING        "%sQSLIDE%02d.WAV"
    #define SLIDESHOW_AUDIO_NAMING        "%sQSLIDE%02d.WAV"
#endif
#if UEZ_DEFAULT_LCD_RES_480x272
    #define SLIDESHOW_PICTURE_NAMING        "%sWQSLID%02d.TGA"
    #define SLIDESHOW_BACKWARDS_COMPATIBLE  0
    #define SLIDESHOW_AUDIO_NAMING        "%sWQSLID%02d.WAV"
#endif
#if UEZ_DEFAULT_LCD_RES_WVGA
    #define SLIDESHOW_PICTURE_NAMING        "%sWVSLID%02d.TGA"
    #define SLIDESHOW_BACKWARDS_COMPATIBLE  0
    #define SLIDESHOW_AUDIO_NAMING        "%sWVSLID%02d.WAV"
#endif

#define SLIDESHOW_FLASH_DRIVE           0
#define SLIDESHOW_SDCARD_DRIVE          1
#define SLIDESHOW_DRIVE_NONE            2

#ifdef __cplusplus
}
#endif

#endif // _SLIDESHOW_H_
/*-------------------------------------------------------------------------*
 * End of File:  Slideshow.c
 *-------------------------------------------------------------------------*/
