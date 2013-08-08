/*-------------------------------------------------------------------------*
 * File:  uEZAudioMixer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZAUDIOMIXER_H_
#define UEZAUDIOMIXER_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZErrors.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define MAX_AUDIO_MIXER_OUTPUTS     (10)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    UEZ_AUDIO_MIXER_OUTPUT_MASTER,
    UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER,
    UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER,
    UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_HEADPHONES,
    UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_HEADPHONES,
} T_uezAudioMixerOutput;

typedef T_uezError (*T_AudioMixer_Callback)(T_uezAudioMixerOutput aChangedOutput, TBool aMute, TUInt8 aLevel);

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError UEZAudioMixerSetLevel(T_uezAudioMixerOutput aOutput, TUInt8 aLevel);
T_uezError UEZAudioMixerGetLevel(T_uezAudioMixerOutput aOutput, TUInt8 *aLevel);
T_uezError UEZAudioMixerMute(T_uezAudioMixerOutput aOutput);
T_uezError UEZAudioMixerUnmute(T_uezAudioMixerOutput aOutput);
T_uezError UEZAudioMixerGetMute(T_uezAudioMixerOutput aOutput, TBool *aMute);
T_uezError UEZAudioMixerRegister(T_uezAudioMixerOutput aOutput, T_AudioMixer_Callback aCallback);

#endif // UEZAUDIOMIXER_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZAudioMixer.h
 *-------------------------------------------------------------------------*/
