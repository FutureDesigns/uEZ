/*-------------------------------------------------------------------------*
 * File:  uEZAudioMixer.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZAudioMixer.h>
#include <UEZPlatform.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct{
        T_uezAudioMixerOutput iOutput;
        TUInt8 iLevel;
        TBool iMuted;
        T_AudioMixer_Callback iCallback;
}T_AudioMixer;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
T_AudioMixer G_AudioMixer[MAX_AUDIO_MIXER_OUTPUTS];

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static T_uezError AudioMixer_MasterSetLevel(TBool aMute, TUInt8 aLevel)
{
    T_uezError error = UEZ_ERROR_NONE;
    TInt32 i;

    for (i = 1; i < MAX_AUDIO_MIXER_OUTPUTS; i++) {
        if(G_AudioMixer[i].iCallback){
            error = G_AudioMixer[i].iCallback((T_uezAudioMixerOutput)i,
                    aMute?ETrue:G_AudioMixer[i].iMuted,
                    (G_AudioMixer[i].iLevel * aLevel)/256);
        }
    }
    return error;
}

static T_uezError AudioMixer_MasterSetMute(TBool aMute)
{
    T_uezError error = UEZ_ERROR_NONE;
    TInt32 i;

    for(i = 1; i < MAX_AUDIO_MIXER_OUTPUTS; i++){
        if(G_AudioMixer[i].iCallback){
            error = G_AudioMixer[i].iCallback((T_uezAudioMixerOutput)i,
                    aMute?ETrue:G_AudioMixer[i].iMuted,
                    (G_AudioMixer[i].iLevel * G_AudioMixer[UEZ_AUDIO_MIXER_OUTPUT_MASTER].iLevel)/256);
        }
    }
    return error;
}

T_uezError UEZAudioMixerSetLevel(T_uezAudioMixerOutput aOutput, TUInt8 aLevel)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 masterLevel;
    TBool masterIsMuted;

    G_AudioMixer[aOutput].iLevel = aLevel;
    masterLevel = G_AudioMixer[UEZ_AUDIO_MIXER_OUTPUT_MASTER].iLevel;
    masterIsMuted = G_AudioMixer[UEZ_AUDIO_MIXER_OUTPUT_MASTER].iMuted;

    if (aOutput == UEZ_AUDIO_MIXER_OUTPUT_MASTER) {
        // Handle special case of changing master level across the mixer system
        AudioMixer_MasterSetLevel(masterIsMuted, masterLevel);
    } else if(G_AudioMixer[aOutput].iCallback){
        error = G_AudioMixer[aOutput].iCallback(aOutput,
                masterIsMuted?ETrue:G_AudioMixer[aOutput].iMuted,
                (G_AudioMixer[aOutput].iLevel * masterLevel)/256);
    }
    return error;
}

T_uezError UEZAudioMixerGetLevel(T_uezAudioMixerOutput aOutput, TUInt8 *aLevel)
{
    T_uezError error = UEZ_ERROR_NONE;

    *aLevel = G_AudioMixer[aOutput].iLevel;
    return error;
}

T_uezError UEZAudioMixerMute(T_uezAudioMixerOutput aOutput)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 masterLevel;
    TBool masterIsMuted;

    masterLevel = G_AudioMixer[UEZ_AUDIO_MIXER_OUTPUT_MASTER].iLevel;
    masterIsMuted = G_AudioMixer[UEZ_AUDIO_MIXER_OUTPUT_MASTER].iMuted;

    G_AudioMixer[aOutput].iMuted = ETrue;
    if(aOutput == UEZ_AUDIO_MIXER_OUTPUT_MASTER){
        AudioMixer_MasterSetMute(ETrue);
    } else {
        if(G_AudioMixer[aOutput].iCallback){
            error = G_AudioMixer[aOutput].iCallback(aOutput,
                    masterIsMuted?ETrue:G_AudioMixer[aOutput].iMuted,
                    (G_AudioMixer[aOutput].iLevel * masterLevel)/256);
        }
    }
    return error;
}

T_uezError UEZAudioMixerUnmute(T_uezAudioMixerOutput aOutput)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 masterLevel;
    TBool masterIsMuted;

    masterLevel = G_AudioMixer[UEZ_AUDIO_MIXER_OUTPUT_MASTER].iLevel;
    masterIsMuted = G_AudioMixer[UEZ_AUDIO_MIXER_OUTPUT_MASTER].iMuted;
    
    G_AudioMixer[aOutput].iMuted = EFalse;
    if(aOutput == UEZ_AUDIO_MIXER_OUTPUT_MASTER){
        AudioMixer_MasterSetMute(EFalse);
    } else {
        if(G_AudioMixer[aOutput].iCallback){
            error = G_AudioMixer[aOutput].iCallback(aOutput,
                    masterIsMuted?ETrue:G_AudioMixer[aOutput].iMuted,
                    (G_AudioMixer[aOutput].iLevel * masterLevel)/256);
        }
    }
    return error;
}

T_uezError UEZAudioMixerGetMute(T_uezAudioMixerOutput aOutput, TBool *aMute)
{
    T_uezError error = UEZ_ERROR_NONE;

    *aMute = G_AudioMixer[aOutput].iMuted;
    return error;
}

T_uezError UEZAudioMixerRegister(T_uezAudioMixerOutput aOutput, T_AudioMixer_Callback aCallback)
{
    T_uezError error = UEZ_ERROR_NONE;
    static TBool iHaveRun = EFalse;
    TUInt8 i;
    
    if(!iHaveRun){
       for( i = 0; i < MAX_AUDIO_MIXER_OUTPUTS; i++){
           G_AudioMixer[i].iOutput = (T_uezAudioMixerOutput)i;
           if(i == 0){
               G_AudioMixer[i].iLevel = 255;
           }else {
               G_AudioMixer[i].iLevel = 0;
           }
           G_AudioMixer[i].iMuted = EFalse;
       }
       iHaveRun = ETrue;
    }

    if(!aOutput == UEZ_AUDIO_MIXER_OUTPUT_MASTER && aOutput < MAX_AUDIO_MIXER_OUTPUTS){
        G_AudioMixer[aOutput].iCallback = *aCallback;
    }
    return error;
}

/*-------------------------------------------------------------------------*
 * End of File:  uEZAudioMixer.c
 *-------------------------------------------------------------------------*/
