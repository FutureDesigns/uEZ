/*-------------------------------------------------------------------------*
 * File:  AudioPlayer.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Generic audio player information
 *-------------------------------------------------------------------------*/
#ifndef _AUDIOPLAYER_H_
#define _AUDIOPLAYER_H_

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

#ifdef __cplusplus
extern "C" {
#endif


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

#ifdef __cplusplus
}
#endif

#endif // _AUDIOPLAYER_H_
/*-------------------------------------------------------------------------*
 * End of File:  AudioPlayer.h
 *-------------------------------------------------------------------------*/
