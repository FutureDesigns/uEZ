/*-------------------------------------------------------------------------*
 * File:  Audio.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Routines for handling audio.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include "Audio.h"
#include <uEZToneGenerator.h>
#include <uEZPlatform.h>
#include <uEZAudioMixer.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#ifndef UEZ_DISABLE_AUDIO
#define UEZ_DISABLE_AUDIO   0
#endif
#ifndef UEZ_SPEAKER_TEST
#define UEZ_SPEAKER_TEST    0
#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#if !UEZ_DISABLE_AUDIO
static T_uezDevice G_ToneGenerator;
extern volatile TBool G_mmTestMode;
#endif
TBool G_audioIsStarted = EFalse;

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
#if UEZ_DISABLE_AUDIO
    return;
#else
    if(!G_audioIsStarted) {
        UEZToneGeneratorOpen("Speaker", &G_ToneGenerator);
        G_audioIsStarted = ETrue;
    }
    
    if (UEZ_SPEAKER_TEST == 0 || G_mmTestMode == ETrue)
    {
        UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
    }
    
    UEZToneGeneratorPlayTone(G_ToneGenerator,TONE_GENERATOR_HZ(aHz), aMS);
    
    if (UEZ_SPEAKER_TEST == 0 || G_mmTestMode == ETrue)
    {
        UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
    }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  PlayAudioContinuous
 *---------------------------------------------------------------------------*
 * Description:
 *      Play a tone indefinitely.
 * Inptus:
 *      TUInt32 aHz                 -- Tone in Hz
 *---------------------------------------------------------------------------*/
void PlayAudioContinuous(TUInt32 aHz)
{
#if UEZ_DISABLE_AUDIO
    return;
#else
    if(!G_audioIsStarted) {
        UEZToneGeneratorOpen("Speaker", &G_ToneGenerator);
        G_audioIsStarted = ETrue;
    }
    UEZToneGeneratorPlayToneContinuous(G_ToneGenerator,TONE_GENERATOR_HZ(aHz));
#endif
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
