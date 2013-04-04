/*-------------------------------------------------------------------------*
 * File:  Audio.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Routines for handling audio.
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


#ifndef _APP_AUDIO_H_
#define _APP_AUDIO_H_

#include <uEZ.h>

void AudioStart(void);
void PlayAudio(TUInt32 aHz, TUInt32 aMS);
void ButtonClick(void);
void BeepError(void);
void Beep(void);

#endif // _APP_AUDIO_H_

/*-------------------------------------------------------------------------*
 * End of File:  Audio.c
 *-------------------------------------------------------------------------*/
