/*-------------------------------------------------------------------------*
 * File:  AudioPlayer.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Generic audio player information
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
#ifndef _AUDIOPLAYER_H_
#define _AUDIOPLAYER_H_

#define MAX_AUDIO_FILES        	20
#define MAX_FILE_DEF_NAME_LEN   80
#define MAX_FILE_DEF_SIZE_LEN   40

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
    #define AUDIO_FILE_ICON_HEIGHT    64
    #define AUDIO_FILE_ICON_WIDTH     64
    #define EXIT_BUTTON_HEIGHT        64
    #define EXIT_BUTTON_WIDTH         128
#else
    #define AUDIO_FILE_ICON_HEIGHT    32
    #define AUDIO_FILE_ICON_WIDTH     32
    #define EXIT_BUTTON_HEIGHT        32
    #define EXIT_BUTTON_WIDTH         64
#endif

typedef struct {
    char iName[MAX_FILE_DEF_NAME_LEN+1];
	TUInt32 iSize;
} T_audioFileDefinition;

typedef struct {
    TUInt32 iCount;
    T_audioFileDefinition iList[MAX_AUDIO_FILES];
} T_audioList;

#define AUDIO_DIRECTORY                    "1:/AUDIO/"

#endif // _AUDIOPLAYER_H_
