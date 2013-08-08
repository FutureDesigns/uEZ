/*-------------------------------------------------------------------------*
 * File:  Audio.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Routines for handling audio.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm)+ Bootloader - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ+ Bootloader distribution.
 *
 * uEZ(tm)+ Bootloader is commercial software licensed under the
 * Software End-User License Agreement (EULA) delivered with this source
 * code package.  The Software and Documentation contain material that is
 * protected by United States Copyright Law and trade secret law, and by
 * international treaty provisions. All rights not granted to Licensee
 * herein are expressly reserved by FDI. Licensee may not remove any
 * proprietary notice of FDI from any copy of the Software or
 * Documentation.
 *
 * For more details, please review your EULA agreement included with this
 * software package.
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
