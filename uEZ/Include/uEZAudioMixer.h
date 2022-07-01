/*-------------------------------------------------------------------------*
 * File:  uEZAudioMixer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZAUDIOMIXER_H_
#define UEZAUDIOMIXER_H_

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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZErrors.h>

#ifdef __cplusplus
extern "C" {
#endif

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


#ifdef __cplusplus
}
#endif

#endif // UEZAUDIOMIXER_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZAudioMixer.h
 *-------------------------------------------------------------------------*/
