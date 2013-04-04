/*-------------------------------------------------------------------------*
 * File:  Slideshow.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Generic slideshow information
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
#ifndef _SLIDESHOW_H_
#define _SLIDESHOW_H_

#include <uEZ.h>

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



#endif // _SLIDESHOW_H_
/*-------------------------------------------------------------------------*
 * End of File:  Slideshow.c
 *-------------------------------------------------------------------------*/
