/*-------------------------------------------------------------------------*
 * File:  Audio.c
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



#include <uEZ.h>
#include "Audio.h"
#include <uEZToneGenerator.h>
#include <uEZPlatform.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static volatile TUInt32 G_dummyCounter = 0;
static T_uezDevice G_ToneGenerator;

/*---------------------------------------------------------------------------*
 * Routine:  CalibrateAudioTiming
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a rough LPC2478 specific calibration method to determine
 *      how fast to control the lines to the audio.  Yes, this is crude.
 *---------------------------------------------------------------------------*/
void AudioStart(void)
{
    UEZToneGeneratorOpen("Speaker", &G_ToneGenerator);
}

/*---------------------------------------------------------------------------*
 * Routine:  PlayAudio
 *---------------------------------------------------------------------------*
 * Description:
 *      Play a tone for the given length, staying here until done.
 * Inptus:
 *      TUInt32 aHz                 -- Tone in Hz
 *      TUInt32 aMS                 -- Duration of tone
 *---------------------------------------------------------------------------*/
void PlayAudio(TUInt32 aHz, TUInt32 aMS)
{
    UEZToneGeneratorPlayTone(G_ToneGenerator,TONE_GENERATOR_HZ(aHz), aMS);
}

/*---------------------------------------------------------------------------*
 * Routine:  ButtonClick
 *---------------------------------------------------------------------------*
 * Description:
 *      Play a standard button click sound
 *---------------------------------------------------------------------------*/
void ButtonClick(void)
{
    PlayAudio(250, 20);
}

/*---------------------------------------------------------------------------*
 * Routine:  BeepError
 *---------------------------------------------------------------------------*
 * Description:
 *      Play a beep for errors.
 *---------------------------------------------------------------------------*/
void BeepError(void)
{
    PlayAudio(100, 500);
}

/*---------------------------------------------------------------------------*
 * Routine:  Beep
 *---------------------------------------------------------------------------*
 * Description:
 *      Play a standard beep.
 *---------------------------------------------------------------------------*/
void Beep(void)
{
    // Beep at 500 Hz for 250 ms
    PlayAudio(500, 250);
}

/*-------------------------------------------------------------------------*
 * End of File:  Audio.c
 *-------------------------------------------------------------------------*/
