/*-------------------------------------------------------------------------*
 * File:  UEZDACWAVFile.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
 
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>
#include "Source/Devices/DAC/Generic/DAC_Generic.h"
#include <HAL/Timer.h>
#include <HAL/DAC.h>
#include <uEZProcessor.h>
#include <uEZAudioAmp.h>
#include <uEZFile.h>
#include <uEZRTOS.h>
#include "uEZDACWAVFile.h"
#include <uEZPlatform.h>
#include <uEZAudioMixer.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifdef UEZ_DAC_WAV_FILE_SAMPLE_RATE
    #define SAMPLE_RATE                     UEZ_DAC_WAV_FILE_SAMPLE_RATE
#else
    #define SAMPLE_RATE                     11025
#endif


#ifndef DAC_AUDIO_BUFFER_SIZE
    #define DAC_AUDIO_BUFFER_SIZE           SAMPLE_RATE*24  // 8k buffers
#endif

#ifndef DAC_AUDIO_TASK_STACK_SIZE
    #define DAC_AUDIO_TASK_STACK_SIZE       UEZ_TASK_STACK_BYTES(2*1024)
#endif

#define MONO                                (1)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct{
    T_uezSemaphore iSem;
    T_uezSemaphore iSemFeed;
    DEVICE_DAC **iDac;
    HAL_Timer **iTimer;
    TBool iUseBuffer1;
    TUInt8 *iBuffer1;
    TUInt32 iBytesBuffer1;
    TBool iNeedFillBuffer1;
    TUInt8 *iBuffer2;
    TUInt32 iBytesBuffer2;
    TBool iNeedFillBuffer2;
    TUInt8 *iMemBuffer;
    TUInt32 iMemBufferSize;
    TUInt32 iCount;
    wavFileHeader iHeader;
    TUInt32 iNumBytesReadHeader;
    T_uezFile iFile;
    TBool iPlaying;
    TBool iFileOpen;
    TBool iPlayFromFile;
    TBool iPlayFromMem;
    TBool iPause;
    TUInt16 iVolume;
    TUInt32 iSamplePos;
    T_uezSemaphore iDACAudioTaskSem;
    T_uezSemaphore iStopSem;
    TBool iConfigured;
}T_UEZDACWAVFile_Workspace;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_UEZDACWAVFile_Workspace G_DACFileWorkspace;
T_uezTask G_DACAudioTask=0;
static TUInt32 MuteTask(T_uezTask aMyTask, void *aParams);

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static T_HALTimer_Callback playDACAudio(void * workspace);
static void DACAudioTask(void);


//Public functions
/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVGetStatus
 *---------------------------------------------------------------------------*/
/**
 *  Return if audio is playing or not
 *
 *  @return TBool       True if playing False if not
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool UEZDACWAVGetStatus(void)
{
    return G_DACFileWorkspace.iPlaying;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWaveSetVolume
 *---------------------------------------------------------------------------*/
/**
 *  Set Volume
 *
 *  @param [in]     aVolume         Volume to set (Range of 0-256)
 */
/*---------------------------------------------------------------------------*/
void UEZDACWaveSetVolume(TUInt16 aVolume)
{
    if(G_DACFileWorkspace.iConfigured){
        G_DACFileWorkspace.iVolume = aVolume;
    }
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVGetSamplePos
 *---------------------------------------------------------------------------*/
/**
 *  Set Volume
 *
 *  @return pos
 */
/*---------------------------------------------------------------------------*/
TUInt32 UEZDACWAVGetSamplePos(void)
{
    TUInt32 pos = 0;

    if(G_DACFileWorkspace.iConfigured){
        RTOS_ENTER_CRITICAL();
        pos = G_DACFileWorkspace.iSamplePos;
        RTOS_EXIT_CRITICAL();
    }

    return pos;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVIsDone
 *---------------------------------------------------------------------------*/
/**
 *  Set Volume
 *
 *  @return       bool
 */
/*---------------------------------------------------------------------------*/
TBool UEZDACWAVIsDone(void)
{
    // True if we are not playing anything
    return G_DACFileWorkspace.iPlaying;//((G_DACFileWorkspace.iPlayFromMem == EFalse)
        //&& (G_DACFileWorkspace.iPlayFromFile == EFalse));
}

static TUInt32 IPopulateHeader(T_uezFile aFile, wavFileHeader *aHeader)
{
    TUInt32 startOfData = 48;
    char *s;
    TUInt8 buffer[512];
    TUInt32 bytesRead;

    memset((void*)aHeader, 0, sizeof(wavFileHeader));

    UEZFileRead(aFile, buffer, sizeof(buffer), &bytesRead);
    if(bytesRead){
        s = strstr((char*)buffer+44, "INFO");
        if(s){
            //populate with metadata
            memcpy((void*)aHeader, buffer, 44);
            s = strstr(s, "INAM");
            if(s){
                s +=8;
                memcpy((void*)aHeader->iSongName, (void*)s, 20);
            }
            s += strlen((char*)aHeader->iSongName) + 1;
            s = strstr(s, "IART");
            if(s){
                s +=8;
                memcpy((void*)aHeader->iArtist, (void*)s, 20);
            }
        } else {
            memcpy((void*)aHeader, buffer, 44);
        }
    } else {
        return 0;
    }
    s = (char*)&buffer;
    startOfData = 0;
    while(startOfData < sizeof(buffer)){
        if(s[0] == '\0' && s[1] == 'd' && s[2] == 'a' && s[3] == 't' && s[4] == 'a'){
            break;
        }
        startOfData++;
        s++;
    }
    startOfData += 7;
    while(startOfData%4 != 0){
        //make sure offset is aligned to 32-bit boundaries.
        startOfData++;
    }
    return startOfData;
}
/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVPlay
 *---------------------------------------------------------------------------*/
/**
 *  Play the specifed WAV file
 *
 *  @param [in]     *aFileName      Name of file to be played eg "1:file.wav"
 *                                       for SD or "0:file.wav" for USB Drive
 *  @return         T_uezError      UEZ_ERROR_NONE if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVPlay(char* aFileName, wavFileHeader *aHeader)
{
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 pos; //Starting point of the data

    if(G_DACFileWorkspace.iConfigured){
        UEZSemaphoreGrab(G_DACFileWorkspace.iSem, UEZ_TIMEOUT_INFINITE);

        if(UEZFileOpen(aFileName, FILE_FLAG_READ_ONLY, &G_DACFileWorkspace.iFile) == UEZ_ERROR_NONE){
            G_DACFileWorkspace.iFileOpen = ETrue;
            pos = IPopulateHeader(G_DACFileWorkspace.iFile, &G_DACFileWorkspace.iHeader);
            memcpy((void*)aHeader, (void*)&G_DACFileWorkspace.iHeader, sizeof(G_DACFileWorkspace.iHeader));
            if(G_DACFileWorkspace.iHeader.iNumChannels != MONO || pos == 0){//|| G_DACFileWorkspace.iHeader.sampleRate != SAMPLE_RATE){
                UEZFileClose(G_DACFileWorkspace.iFile);
                G_DACFileWorkspace.iFile = 0;
                UEZSemaphoreRelease(G_DACFileWorkspace.iSem);
                return UEZ_ERROR_INVALID;
            }
            error = (*G_DACFileWorkspace.iTimer)->SetMatchRegister(G_DACFileWorkspace.iTimer,
                                                                    0,
                                                                    ((PROCESSOR_OSCILLATOR_FREQUENCY/1)/G_DACFileWorkspace.iHeader.iSampleRate),
                                                                    ETrue,
                                                                    ETrue,
                                                                    EFalse);
            UEZFileSeekPosition(G_DACFileWorkspace.iFile, pos);
            UEZFileRead(G_DACFileWorkspace.iFile,
                              G_DACFileWorkspace.iBuffer1,
                              DAC_AUDIO_BUFFER_SIZE,
                              &G_DACFileWorkspace.iBytesBuffer1);
            UEZFileRead(G_DACFileWorkspace.iFile,
                              G_DACFileWorkspace.iBuffer2,
                              DAC_AUDIO_BUFFER_SIZE,
                              &G_DACFileWorkspace.iBytesBuffer2);

            G_DACFileWorkspace.iUseBuffer1 = ETrue;
            G_DACFileWorkspace.iNeedFillBuffer1 = EFalse;
            G_DACFileWorkspace.iNeedFillBuffer2 = EFalse;
            G_DACFileWorkspace.iCount = 0;
            G_DACFileWorkspace.iSamplePos = 0;
            G_DACFileWorkspace.iPlaying = ETrue;
            G_DACFileWorkspace.iPlayFromFile = ETrue;
            G_DACFileWorkspace.iPlayFromMem = EFalse;
            G_DACFileWorkspace.iPause = EFalse;

            (*G_DACFileWorkspace.iTimer)->Reset(G_DACFileWorkspace.iTimer);
            (*G_DACFileWorkspace.iTimer)->Enable(G_DACFileWorkspace.iTimer);
            UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
            error = UEZSemaphoreRelease(G_DACFileWorkspace.iDACAudioTaskSem);
        } else {
            error = UEZ_ERROR_INCOMPLETE_CONFIGURATION;
        }

        UEZSemaphoreRelease(G_DACFileWorkspace.iSem);
    } else {
        error = UEZ_ERROR_DEVICE_NOT_FOUND;
    }

    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVPlayBuffer
 *---------------------------------------------------------------------------*/
/**
 *  Play the specifed WAV buffer
 *
 *  @param [in]    *aBuffer     Pointer to buffer to play
 *
 *  @param [in]    aSize        Buffer size
 *
 *  @return        T_uezError   UEZ_ERROR_NONE if success

 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVPlayBuffer(TUInt8 *aBuffer, TUInt32 aSize)
{
    T_uezError error = UEZ_ERROR_NONE;

    if(G_DACFileWorkspace.iConfigured){
        UEZSemaphoreGrab(G_DACFileWorkspace.iSem, UEZ_TIMEOUT_INFINITE);

        if(!G_DACFileWorkspace.iPlaying){
            G_DACFileWorkspace.iPlayFromMem = ETrue;
            G_DACFileWorkspace.iPlayFromFile = EFalse;
            G_DACFileWorkspace.iPause = EFalse;
            G_DACFileWorkspace.iCount = 0;

            G_DACFileWorkspace.iMemBuffer = aBuffer;
            G_DACFileWorkspace.iMemBufferSize = aSize;

            UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
            G_DACFileWorkspace.iPlaying = ETrue;
            (*G_DACFileWorkspace.iTimer)->Enable(G_DACFileWorkspace.iTimer);
            UEZSemaphoreRelease(G_DACFileWorkspace.iSem);
        } else {
            error = UEZ_ERROR_ILLEGAL_OPERATION;
        }
    } else {
        error = UEZ_ERROR_DEVICE_NOT_FOUND;
    }
    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVPlayPause
 *---------------------------------------------------------------------------*/
/**
 *  Pause the DAC playback
 *
 *  @param [in]     *aBool          If true, pause, if False then unpause
 *
 *  @return         T_uezError      UEZ_ERROR_NONE if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVPlayPause(TBool aBool)
{
    T_uezError error = UEZ_ERROR_NONE;

    if(G_DACFileWorkspace.iConfigured){
        G_DACFileWorkspace.iPause = aBool;
    } else {
        error = UEZ_ERROR_DEVICE_NOT_FOUND;
    }
    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVStop
 *---------------------------------------------------------------------------*/
/**
 *  Stop audio from playing, remove the task, and close the file
 *
 *  @return         T_uezError      UEZ_ERROR_NONE if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVStop(void)
{
    T_uezError error = UEZ_ERROR_NONE;

    if(G_DACFileWorkspace.iConfigured){
        UEZSemaphoreGrab(G_DACFileWorkspace.iSem, UEZ_TIMEOUT_INFINITE);
        if(G_DACFileWorkspace.iPlaying){
            (*G_DACFileWorkspace.iTimer)->Disable(G_DACFileWorkspace.iTimer);
            if (G_DACFileWorkspace.iPlayFromMem) {
                // Point to the end so the task closes out naturally
                G_DACFileWorkspace.iCount = G_DACFileWorkspace.iBytesBuffer1;
            }
            G_DACFileWorkspace.iPlaying = EFalse;
            if(G_DACFileWorkspace.iPlayFromFile){
                if(G_DACFileWorkspace.iFileOpen == ETrue){
                    G_DACFileWorkspace.iFileOpen = EFalse;
                    UEZFileClose(G_DACFileWorkspace.iFile);
                    G_DACFileWorkspace.iFile = 0;
                }
            }
        }

        if(G_DACFileWorkspace.iFile){
            UEZFileClose(G_DACFileWorkspace.iFile);
            G_DACFileWorkspace.iFile = 0;
        }
        G_DACFileWorkspace.iPlayFromMem = EFalse;
        G_DACFileWorkspace.iPlayFromFile = EFalse;
        UEZSemaphoreRelease(G_DACFileWorkspace.iSem);
        UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);

        (*G_DACFileWorkspace.iTimer)->Disable(G_DACFileWorkspace.iTimer);

        UEZSemaphoreRelease(G_DACFileWorkspace.iSem);
    } else {
        error = UEZ_ERROR_DEVICE_NOT_FOUND;
    }

    return error;
}

/*---------------------------------------------------------------------------*
* Routine: UEZDACWAVConfig
 *---------------------------------------------------------------------------*/
/**
 *  Setups the DAC, Timer, and allocated memory for the buffers
 *  This driver uses the HAL Layer for the timer and DAC, should these device
 *  be used else where this driver may not function properly. Use of the HAL
 *  Layer is required for speed.
 *
 * @return T_uezError, UEZ_ERROR_NONE if successful
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVConfig(const char* aTimer)
{
    T_uezError error = UEZ_ERROR_NONE;
    static TBool runOnce = EFalse;
    TUInt32 v;

    if(!runOnce){
        runOnce = ETrue;
        memset((void*)&G_DACFileWorkspace, 0 , sizeof(G_DACFileWorkspace));

        //Check to see if the time and DAC are registered
        if ((!UEZDeviceTableIsRegistered("DAC0")) ||
              (!UEZDeviceTableIsRegistered(aTimer)) ){
            //the devices have not been required, add the correct require routines to the startup
            return UEZ_ERROR_DEVICE_NOT_FOUND;
        }

        //Setup the workspace
        G_DACFileWorkspace.iVolume = 256; // 100%
        UEZSemaphoreCreateBinary(&G_DACFileWorkspace.iSem);
        UEZSemaphoreCreateBinary(&G_DACFileWorkspace.iSemFeed);
        UEZSemaphoreCreateBinary(&G_DACFileWorkspace.iDACAudioTaskSem);
        UEZSemaphoreCreateBinary(&G_DACFileWorkspace.iStopSem);

        //Grab the semaphore to hold the task when it gets created
        UEZSemaphoreGrab(G_DACFileWorkspace.iDACAudioTaskSem, UEZ_TIMEOUT_INFINITE);
        UEZSemaphoreGrab(G_DACFileWorkspace.iStopSem, UEZ_TIMEOUT_INFINITE);

        //Configure the DAC
        if(HALInterfaceFind("DAC0", (T_halWorkspace **)&G_DACFileWorkspace.iDac) == UEZ_ERROR_NONE){
            v = 0x8000;
            v = (v * G_DACFileWorkspace.iVolume) / 256;
            v >>= 6;
            error = (*G_DACFileWorkspace.iDac)->Write(G_DACFileWorkspace.iDac, v);
        } else {
            error = UEZ_ERROR_DEVICE_NOT_FOUND;
        }

        if(error == UEZ_ERROR_NONE){
            //DAC is configured with not problems, configure the timer
            if(HALInterfaceFind(aTimer, (T_halWorkspace **)&G_DACFileWorkspace.iTimer) == UEZ_ERROR_NONE){
                error = (*G_DACFileWorkspace.iTimer)->SetMatchRegister(G_DACFileWorkspace.iTimer,
                                                                        0,
                                                                        (PROCESSOR_OSCILLATOR_FREQUENCY/1)/SAMPLE_RATE,
                                                                        ETrue,
                                                                        ETrue,
                                                                        EFalse);

                if(error == UEZ_ERROR_NONE){
                    error = (*G_DACFileWorkspace.iTimer)->SetMatchCallback(G_DACFileWorkspace.iTimer,
                                                                           0,
                                                                           (T_HALTimer_Callback)&playDACAudio,
                                                                           &G_DACFileWorkspace);
                }

            } else {
                error = UEZ_ERROR_DEVICE_NOT_FOUND;
            }
        }

        //Timer configured
        if(error == UEZ_ERROR_NONE){
            //Setup the rest of the workspace, allocate memory for the buffers, and create the feed task
            G_DACFileWorkspace.iPlaying = EFalse;

            G_DACFileWorkspace.iPlayFromMem = EFalse;
            G_DACFileWorkspace.iPlayFromFile = EFalse;
            G_DACFileWorkspace.iFile = 0;

                //Allocate the memory for the buffers
            G_DACFileWorkspace.iBuffer1 = UEZMemAlloc(DAC_AUDIO_BUFFER_SIZE);
            G_DACFileWorkspace.iBuffer2 = UEZMemAlloc(DAC_AUDIO_BUFFER_SIZE);


            error = UEZTaskCreate((T_uezTaskFunction)DACAudioTask, "DAC Audio",
                                  DAC_AUDIO_TASK_STACK_SIZE, 0, UEZ_PRIORITY_HIGH,
                                  &G_DACAudioTask);

            UEZTaskCreate(MuteTask, "MuteTask", 128, (void *)0, UEZ_PRIORITY_LOW, 0);
        }
        if(error == UEZ_ERROR_NONE){
            G_DACFileWorkspace.iConfigured = ETrue;
        }
    }
    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVCleanUp
 *---------------------------------------------------------------------------*/
/**
 *  Free all allocated memory, this driver can not be restarted after this call
 *  is made.
 *
 */
/*---------------------------------------------------------------------------*/
void UEZDACWAVCleanUp(void)
{
    if(G_DACFileWorkspace.iConfigured){
        //Deallocate buffers
        UEZMemFree(G_DACFileWorkspace.iBuffer1);
        UEZMemFree(G_DACFileWorkspace.iBuffer2);

        //Delete the feed task
        UEZTaskDelete(G_DACAudioTask);

        //close the time and dac
        //not required when using the HAL layer

        //delete the semaphores
        UEZSemaphoreDelete(G_DACFileWorkspace.iSem);
        UEZSemaphoreDelete(G_DACFileWorkspace.iSemFeed);
        UEZSemaphoreDelete(G_DACFileWorkspace.iDACAudioTaskSem);
        UEZSemaphoreDelete(G_DACFileWorkspace.iStopSem);

        //clear the workspace
        memset((void*)&G_DACFileWorkspace, 0 , sizeof(G_DACFileWorkspace));
    }
}

//private functions
/*---------------------------------------------------------------------------*
* Routine:  playDACAudio
 *---------------------------------------------------------------------------*/
/**
 *  Timer callback routine to put data into the DAC
 *
 *  @param [in]    *workspace               callback functions workspace
 *
 *  @return        T_HALTimer_Callback      Callback
 */
/*---------------------------------------------------------------------------*/
static T_HALTimer_Callback playDACAudio(void * workspace)
{
    TUInt32 v;
    TUInt16 r;

    if(!G_DACFileWorkspace.iPause){
        if (!G_DACFileWorkspace.iPlayFromMem) {
            if (G_DACFileWorkspace.iUseBuffer1) { // if use buffer1
                if (!G_DACFileWorkspace.iNeedFillBuffer1) {
#if DAC_WAV_IS_8BIT
                    v = G_DACFileWorkspace.iBuffer1[G_DACFileWorkspace.iCount++]
                                                     << 2;
                    v = (v * G_DACFileWorkspace.iVolume) / 256;
#else
                    v = (*((TUInt16 *)(G_DACFileWorkspace.iBuffer1 + G_DACFileWorkspace.iCount)) + 0x8000) & 0xFFFF;
                    //v = (v * G_DACFileWorkspace.iVolume) / 256;
                    v >>= 6;
                    G_DACFileWorkspace.iCount+=2;
#endif
                    (*G_DACFileWorkspace.iDac)->Write(G_DACFileWorkspace.iDac, v);
                    G_DACFileWorkspace.iSamplePos++;

                    if (G_DACFileWorkspace.iCount
                            >= G_DACFileWorkspace.iBytesBuffer1) {
                        G_DACFileWorkspace.iNeedFillBuffer1 = ETrue;
                        G_DACFileWorkspace.iUseBuffer1 = EFalse;
                        G_DACFileWorkspace.iCount = 0;
                        G_DACFileWorkspace.iBytesBuffer1 = 0;
                        //DEBUG_SV_Printf("A1");
                        _isr_UEZSemaphoreRelease(G_DACFileWorkspace.iSemFeed);
                    }
                }
            } else {
                if (!G_DACFileWorkspace.iNeedFillBuffer2) {
#if DAC_WAV_IS_8BIT
                    v = G_DACFileWorkspace.iBuffer2[G_DACFileWorkspace.iCount++]
                                                     << 2;
                    v = (v * G_DACFileWorkspace.iVolume) / 256;
                    (*G_DACFileWorkspace.iDac)->Write(G_DACFileWorkspace.iDac, v);
                    G_DACFileWorkspace.iSamplePos++;
#else
                    v = (*((TUInt16 *)(G_DACFileWorkspace.iBuffer2 + G_DACFileWorkspace.iCount)) + 0x8000) & 0xFFFF;
                    //v = (v * G_DACFileWorkspace.iVolume) / 256;
                    v >>= 6;
                    G_DACFileWorkspace.iCount+=2;
#endif
                    (*G_DACFileWorkspace.iDac)->Write(G_DACFileWorkspace.iDac, v);
                    G_DACFileWorkspace.iSamplePos++;

                    if (G_DACFileWorkspace.iCount
                            >= G_DACFileWorkspace.iBytesBuffer2) {
                        G_DACFileWorkspace.iNeedFillBuffer2 = ETrue;
                        G_DACFileWorkspace.iUseBuffer1 = ETrue;
                        G_DACFileWorkspace.iCount = 0;
                        G_DACFileWorkspace.iBytesBuffer2 = 0;
                        //DEBUG_SV_Printf("A2");
                        _isr_UEZSemaphoreRelease(G_DACFileWorkspace.iSemFeed);
                    }
                }
            }
        } else {
            if (G_DACFileWorkspace.iCount >= G_DACFileWorkspace.iMemBufferSize) {
                (*G_DACFileWorkspace.iDac)->Write(G_DACFileWorkspace.iDac, 0);
                G_DACFileWorkspace.iPlaying = EFalse;
                (*G_DACFileWorkspace.iTimer)->Disable(G_DACFileWorkspace.iTimer);
                if(G_DACFileWorkspace.iStopSem){
                    _isr_UEZSemaphoreRelease(G_DACFileWorkspace.iStopSem);
                }

            } else {
                r = G_DACFileWorkspace.iMemBuffer[G_DACFileWorkspace.iCount++] << 2;
                r = (r * G_DACFileWorkspace.iVolume) / 256;
                (*G_DACFileWorkspace.iDac)->Write(G_DACFileWorkspace.iDac, r);
            }
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*
* Routine: DACAudioTask
 *---------------------------------------------------------------------------*/
/**
 *  Tasked to feed the buffers and start the timer
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void DACAudioTask(void)
{
    TBool forever = ETrue;

    while(forever){
        //wait here until time to play
        UEZSemaphoreGrab(G_DACFileWorkspace.iDACAudioTaskSem, UEZ_TIMEOUT_INFINITE);
        (*G_DACFileWorkspace.iTimer)->Enable(G_DACFileWorkspace.iTimer);
        while(1){
            if(G_DACFileWorkspace.iPlayFromFile){
                //Playing from a file, check to see if new data needs to be put in the buffers
                if(G_DACFileWorkspace.iNeedFillBuffer1){
                    UEZSemaphoreGrab(G_DACFileWorkspace.iSemFeed, UEZ_TIMEOUT_INFINITE);
                    UEZFileRead(G_DACFileWorkspace.iFile,
                            G_DACFileWorkspace.iBuffer1,
                        DAC_AUDIO_BUFFER_SIZE, &G_DACFileWorkspace.iBytesBuffer1);
                    G_DACFileWorkspace.iNeedFillBuffer1 = EFalse;
                    UEZSemaphoreRelease(G_DACFileWorkspace.iSemFeed);
                    if (G_DACFileWorkspace.iBytesBuffer1 == 0){
                        //UEZDACWAVStop(voi);
                        break;
                    }
                }
                if(G_DACFileWorkspace.iNeedFillBuffer2){
                    UEZSemaphoreGrab(G_DACFileWorkspace.iSemFeed, UEZ_TIMEOUT_INFINITE);
                    UEZFileRead(G_DACFileWorkspace.iFile,
                            G_DACFileWorkspace.iBuffer2,
                        DAC_AUDIO_BUFFER_SIZE, &G_DACFileWorkspace.iBytesBuffer2);
                    G_DACFileWorkspace.iNeedFillBuffer2 = EFalse;
                    UEZSemaphoreRelease(G_DACFileWorkspace.iSemFeed);
                    if (G_DACFileWorkspace.iBytesBuffer2 == 0){
                        //UEZDACWAVStop();
                        break;
                    }
                }
            } else if(G_DACFileWorkspace.iPlayFromMem){
                //playing from memory
                if (G_DACFileWorkspace.iCount
                    >= G_DACFileWorkspace.iMemBufferSize) {
                    // I'm done, I'm leaving ... cya!
                    UEZDACWAVStop();
                    break;
                }

            } else {
                break;
            }
            UEZTaskDelay(1);
        }
        G_DACFileWorkspace.iPlaying = EFalse;
    }
}

//used with Play buffer
static TUInt32 MuteTask(T_uezTask aMyTask, void *aParams)
{
    UEZSemaphoreGrab(G_DACFileWorkspace.iStopSem, UEZ_TIMEOUT_INFINITE);
    UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
    return 1;
}

/*-------------------------------------------------------------------------*
 * End of File:  UEZDACWAVFile.c
 *-------------------------------------------------------------------------*/
