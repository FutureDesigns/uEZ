/*-------------------------------------------------------------------------*
 * File:  uEZDACWAVFile.c
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZDACWAVFile
 *  @{
 *  @brief     uEZ DAC wav file player
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 * 		The driver is timer based and plays 11025Hz, Mono, 8-bit 
 *      Windows based wav files.
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
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
#include <Types/WAVFile.h>
#include <UEZRTOS.h>
#include "UEZDACWAVFile.h"
#include <uEZPlatform.h>

/*---------------------------------------------------------------------------*
* Constants:
*---------------------------------------------------------------------------*/
#ifndef UEZ_DAC_WAV_FILE_TASK_BASED
#define UEZ_DAC_WAV_FILE_TASK_BASED   0
#endif

/*---------------------------------------------------------------------------*
* Types:
*---------------------------------------------------------------------------*/
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
}T_UEZDACWAVFile_Workspace;

/*---------------------------------------------------------------------------*
* Globals:
*---------------------------------------------------------------------------*/
T_UEZDACWAVFile_Workspace *G_DACWAV_Workspace = 0;
T_uezTask G_DACAudioTask=0;

/*---------------------------------------------------------------------------*
* Macros:
*---------------------------------------------------------------------------*/
#ifndef DAC_AUDIO_BUFFER_SIZE
    #define DAC_AUDIO_BUFFER_SIZE                     1024*8  // 8k buffers
#endif

#ifdef UEZ_DAC_WAV_FILE_SAMPLE_RATE
    #define SAMPLE_RATE                     UEZ_DAC_WAV_FILE_SAMPLE_RATE
#else
    #define SAMPLE_RATE                     22050 // 11025   //11025Hz is the fastest supported sample rate
#endif

#ifndef DAC_AUDIO_TASK_STACK_SIZE
    #define DAC_AUDIO_TASK_STACK_SIZE       UEZ_TASK_STACK_BYTES(2*1024)
#endif

#ifndef DAC_WAV_IS_8BIT
#define DAC_WAV_IS_8BIT                     0
#endif

#define MONO                            1       //MONO is the only supported format for wav files

/*---------------------------------------------------------------------------*
* Routine:  playDACAudio
 *---------------------------------------------------------------------------*/
/**
 *  Timer callback routine to put data into the DAC
 *
 *  @param [in]    *workspace 				callback functions workspace
 *
 *  @return        T_HALTimer_Callback		Callback 
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_HALTimer_Callback playDACAudio(void * workspace)
{
    TUInt32 v;

    if(!G_DACWAV_Workspace->iPause){
        if (!G_DACWAV_Workspace->iPlayFromMem) {
            if (G_DACWAV_Workspace->iUseBuffer1) { // if use buffer1
                if (!G_DACWAV_Workspace->iNeedFillBuffer1) {
#if DAC_WAV_IS_8BIT
                    v = G_DACWAV_Workspace->iBuffer1[G_DACWAV_Workspace->iCount++]
                                                     << 2;
                    v = (v * G_DACWAV_Workspace->iVolume) / 256;
#else
                    v = (*((TUInt16 *)(G_DACWAV_Workspace->iBuffer1 + G_DACWAV_Workspace->iCount)) + 0x8000) & 0xFFFF;
                    v = (v * G_DACWAV_Workspace->iVolume) / 256;
                    v >>= 6;
                    G_DACWAV_Workspace->iCount+=2;
#endif
                    (*G_DACWAV_Workspace->iDac)->Write(G_DACWAV_Workspace->iDac, v);
                    G_DACWAV_Workspace->iSamplePos++;

                    if (G_DACWAV_Workspace->iCount
                            >= G_DACWAV_Workspace->iBytesBuffer1) {
                        G_DACWAV_Workspace->iNeedFillBuffer1 = ETrue;
                        G_DACWAV_Workspace->iUseBuffer1 = EFalse;
                        G_DACWAV_Workspace->iCount = 0;
                        G_DACWAV_Workspace->iBytesBuffer1 = 0;
                        _isr_UEZSemaphoreRelease(G_DACWAV_Workspace->iSemFeed);
                    }
                }
            } else {
                if (!G_DACWAV_Workspace->iNeedFillBuffer2) {
#if DAC_WAV_IS_8BIT
                    v = G_DACWAV_Workspace->iBuffer2[G_DACWAV_Workspace->iCount++]
                                                     << 2;
                    v = (v * G_DACWAV_Workspace->iVolume) / 256;
                    (*G_DACWAV_Workspace->iDac)->Write(G_DACWAV_Workspace->iDac, v);
                    G_DACWAV_Workspace->iSamplePos++;
#else
                    v = (*((TUInt16 *)(G_DACWAV_Workspace->iBuffer2 + G_DACWAV_Workspace->iCount)) + 0x8000) & 0xFFFF;
                    v = (v * G_DACWAV_Workspace->iVolume) / 256;
                    v >>= 6;
                    G_DACWAV_Workspace->iCount+=2;
#endif
                    (*G_DACWAV_Workspace->iDac)->Write(G_DACWAV_Workspace->iDac, v);
                    G_DACWAV_Workspace->iSamplePos++;

                    if (G_DACWAV_Workspace->iCount
                            >= G_DACWAV_Workspace->iBytesBuffer2) {
                        G_DACWAV_Workspace->iNeedFillBuffer2 = ETrue;
                        G_DACWAV_Workspace->iUseBuffer1 = ETrue;
                        G_DACWAV_Workspace->iCount = 0;
                        G_DACWAV_Workspace->iBytesBuffer2 = 0;
                        _isr_UEZSemaphoreRelease(G_DACWAV_Workspace->iSemFeed);
                    }
                }
            }
        } else {
            if (G_DACWAV_Workspace->iCount >= G_DACWAV_Workspace->iBytesBuffer1) {
                (*G_DACWAV_Workspace->iDac)->Write(G_DACWAV_Workspace->iDac, 0);
                G_DACWAV_Workspace->iPlaying = EFalse;
                (*G_DACWAV_Workspace->iTimer)->Disable(G_DACWAV_Workspace->iTimer);
            } else {
                v = G_DACWAV_Workspace->iBuffer1[G_DACWAV_Workspace->iCount++] << 2;
                v = (v * G_DACWAV_Workspace->iVolume) / 256;
                (*G_DACWAV_Workspace->iDac)->Write(G_DACWAV_Workspace->iDac, v);
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
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void DACAudioTask()
{
    (*G_DACWAV_Workspace->iTimer)->Enable(G_DACWAV_Workspace->iTimer);

    while (1) {
        if (!G_DACWAV_Workspace->iPlayFromMem) {
            UEZSemaphoreGrab(G_DACWAV_Workspace->iSemFeed,
                UEZ_TIMEOUT_INFINITE);
            if (G_DACWAV_Workspace->iNeedFillBuffer1) {
                UEZFileRead(G_DACWAV_Workspace->iFile,
                    G_DACWAV_Workspace->iBuffer1,
                    DAC_AUDIO_BUFFER_SIZE, &G_DACWAV_Workspace->iBytesBuffer1);
                G_DACWAV_Workspace->iNeedFillBuffer1 = EFalse;
                if (G_DACWAV_Workspace->iBytesBuffer1 == 0)
                    UEZDACWAVStop();
            }
            if (G_DACWAV_Workspace->iNeedFillBuffer2) {
                UEZFileRead(G_DACWAV_Workspace->iFile,
                    G_DACWAV_Workspace->iBuffer2,
                    DAC_AUDIO_BUFFER_SIZE, &G_DACWAV_Workspace->iBytesBuffer2);
                G_DACWAV_Workspace->iNeedFillBuffer2 = EFalse;
                if (G_DACWAV_Workspace->iBytesBuffer2 == 0)
                    UEZDACWAVStop();
            }
        } else {
            if (G_DACWAV_Workspace->iCount
                >= G_DACWAV_Workspace->iBytesBuffer1) {
                // I'm done, I'm leaving ... cya!
                UEZDACWAVStop();
                UEZSemaphoreGrab(G_DACWAV_Workspace->iSem,
                    UEZ_TIMEOUT_INFINITE);
                G_DACAudioTask = 0;
                UEZSemaphoreRelease(G_DACWAV_Workspace->iSem);
                G_DACWAV_Workspace->iPlaying = EFalse;
                break;
            }
        }
    }
}

/*---------------------------------------------------------------------------*
* Routine: UEZDACWAVConfig 
 *---------------------------------------------------------------------------*/
/**
 *  Setups the DAC, Timer, and allocated memory for the buffers
 *
 *  @param [in] 	*aTimer			Timer to use eg "Timer0"
 *                  
 *	@return			T_uezError		UEZ_ERROR_NONE if success
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVConfig(const char* aTimer)
{
    T_uezError error;
    TUInt32 v;
    
    G_DACWAV_Workspace = UEZMemAlloc(sizeof(T_UEZDACWAVFile_Workspace));
    G_DACWAV_Workspace->iVolume = 256; // 100%
    
    //create the semiphore
    UEZSemaphoreCreateBinary(&G_DACWAV_Workspace->iSem);
    UEZSemaphoreCreateBinary(&G_DACWAV_Workspace->iSemFeed);
    
    HALInterfaceFind(aTimer, (T_halWorkspace **)&G_DACWAV_Workspace->iTimer);
    HALInterfaceFind("DAC0", (T_halWorkspace **)&G_DACWAV_Workspace->iDac);
    
    //Configure the DAC
    error = (*G_DACWAV_Workspace->iDac)->SetBias((void*)G_DACWAV_Workspace->iDac, EFalse);
    error = (*G_DACWAV_Workspace->iDac)->SetVRef((void*)G_DACWAV_Workspace->iDac, 3300);
    
    // Configure the DAC midpoint
    v = 0x8000;
    v = (v * G_DACWAV_Workspace->iVolume) / 256;
    v >>= 6;
    (*G_DACWAV_Workspace->iDac)->Write(G_DACWAV_Workspace->iDac, v);
    
    if(error)
        return error;
        
    //Setup the timer
    error = (*G_DACWAV_Workspace->iTimer)->SetMatchRegister(G_DACWAV_Workspace->iTimer,
                                                            0,
                                                            (PROCESSOR_OSCILLATOR_FREQUENCY/1)/SAMPLE_RATE,
                                                            ETrue,
                                                            ETrue,
                                                            EFalse);
    error = (*G_DACWAV_Workspace->iTimer)->SetMatchCallback(G_DACWAV_Workspace->iTimer,
                                                           0,
                                                           (T_HALTimer_Callback)&playDACAudio,
                                                           G_DACWAV_Workspace);
    if(error)
        return error;                                                   
       
    G_DACWAV_Workspace->iPlaying = EFalse;
    
    G_DACWAV_Workspace->iPlayFromMem = EFalse;
    G_DACWAV_Workspace->iPlayFromFile = EFalse;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVStop
 *---------------------------------------------------------------------------*/
/**
 *  Stop audio from playing, remove the task, and close the file
 *
 *  @return			T_uezError		UEZ_ERROR_NONE if success       
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVStop()
{
    if(G_DACWAV_Workspace){
        UEZSemaphoreGrab(G_DACWAV_Workspace->iSem, UEZ_TIMEOUT_INFINITE);
        if(G_DACWAV_Workspace->iPlaying){
            (*G_DACWAV_Workspace->iTimer)->Disable(G_DACWAV_Workspace->iTimer);
            if (G_DACWAV_Workspace->iPlayFromMem) {
                // Point to the end so the task closes out naturally
                G_DACWAV_Workspace->iCount = G_DACWAV_Workspace->iBytesBuffer1;
            }
            G_DACWAV_Workspace->iPlaying = EFalse;
            if(G_DACWAV_Workspace->iPlayFromFile){
                if(G_DACWAV_Workspace->iFileOpen == ETrue){
                    G_DACWAV_Workspace->iFileOpen = EFalse;
                    UEZFileClose(G_DACWAV_Workspace->iFile);
                }
            }
        }
        G_DACWAV_Workspace->iPlayFromMem = EFalse;
        G_DACWAV_Workspace->iPlayFromFile = EFalse;
        UEZSemaphoreRelease(G_DACWAV_Workspace->iSem);
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVPlayPause
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
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVPlayPause(TBool aBool)
{
    G_DACWAV_Workspace->iPause = aBool;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVPlay
 *---------------------------------------------------------------------------*/
/**
 *  Play the specifed WAV file
 *
 *	@param [in] 	*aFileName		Name of file to be played eg "1:file.wav"
 *                                       for SD or "0:file.wav" for USB Drive
 *	@return			T_uezError		UEZ_ERROR_NONE if success     
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVPlay(char* aFileName)
{
    T_uezError error;
    
    if(G_DACWAV_Workspace == NULL)
        return UEZ_ERROR_NOT_ACTIVE; // Must call config first

    G_DACWAV_Workspace->iPlayFromFile = ETrue;
    G_DACWAV_Workspace->iPlayFromMem = EFalse;
    G_DACWAV_Workspace->iPause = EFalse;
        
    UEZSemaphoreGrab(G_DACWAV_Workspace->iSem, UEZ_TIMEOUT_INFINITE);
    if (G_DACWAV_Workspace->iPlaying == ETrue) {
        UEZSemaphoreRelease(G_DACWAV_Workspace->iSem);
        return UEZ_ERROR_ILLEGAL_OPERATION;
    }
    
    //Allocate the memory for the buffers
    G_DACWAV_Workspace->iBuffer1 = UEZMemAlloc(DAC_AUDIO_BUFFER_SIZE);
    G_DACWAV_Workspace->iBuffer2 = UEZMemAlloc(DAC_AUDIO_BUFFER_SIZE);

    if(UEZFileOpen(aFileName, FILE_FLAG_READ_ONLY, &G_DACWAV_Workspace->iFile) == UEZ_ERROR_NONE){
        G_DACWAV_Workspace->iFileOpen = ETrue;
        UEZFileRead(G_DACWAV_Workspace->iFile, &G_DACWAV_Workspace->iHeader, 44, &G_DACWAV_Workspace->iNumBytesReadHeader);
        if(G_DACWAV_Workspace->iHeader.numChannels != MONO || G_DACWAV_Workspace->iHeader.sampleRate != SAMPLE_RATE){
            UEZFileClose(G_DACWAV_Workspace->iFile);
            UEZSemaphoreRelease(G_DACWAV_Workspace->iSem);
            return UEZ_ERROR_INVALID;
        }
        UEZFileRead(G_DACWAV_Workspace->iFile,
                          G_DACWAV_Workspace->iBuffer1,
                          DAC_AUDIO_BUFFER_SIZE,
                          &G_DACWAV_Workspace->iBytesBuffer1);
        UEZFileRead(G_DACWAV_Workspace->iFile,
                          G_DACWAV_Workspace->iBuffer2,
                          DAC_AUDIO_BUFFER_SIZE,
                          &G_DACWAV_Workspace->iBytesBuffer2);
    }
    else {
        UEZSemaphoreRelease(G_DACWAV_Workspace->iSem);
        return UEZ_ERROR_NOT_FOUND;
    }
    //Inilize the workspace
    G_DACWAV_Workspace->iUseBuffer1 = ETrue;
    G_DACWAV_Workspace->iNeedFillBuffer1 = EFalse;
    G_DACWAV_Workspace->iNeedFillBuffer2 = EFalse;
    G_DACWAV_Workspace->iCount = 0;
    G_DACWAV_Workspace->iSamplePos = 0;
    
    G_DACWAV_Workspace->iPlaying = ETrue;
    
    error = UEZTaskCreate((T_uezTaskFunction)DACAudioTask, "DAC Audio",
                          DAC_AUDIO_TASK_STACK_SIZE, 0, UEZ_PRIORITY_HIGH,
                          &G_DACAudioTask);
    UEZSemaphoreRelease(G_DACWAV_Workspace->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVPlayBuffer
 *---------------------------------------------------------------------------*/
/**
 *  Play the specifed WAV buffer
 *
 *  @param [in]    *aBuffer 	Pointer to buffer to play
 *
 *	@param [in]	   aSize		Buffer size
 *
 *	@return		   T_uezError	UEZ_ERROR_NONE if success       
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWAVPlayBuffer(TUInt8 *aBuffer, TUInt32 aSize)
{
    T_uezError error;
    
    if(G_DACWAV_Workspace == NULL)
        return UEZ_ERROR_NOT_ACTIVE; // Must call config first
    
//aSize /= 5;
    G_DACWAV_Workspace->iPlayFromMem = ETrue;
    G_DACWAV_Workspace->iPlayFromFile = EFalse;
    G_DACWAV_Workspace->iPause = EFalse;
    
    UEZSemaphoreGrab(G_DACWAV_Workspace->iSem, UEZ_TIMEOUT_INFINITE);
    if (G_DACWAV_Workspace->iPlaying == ETrue) {
        UEZSemaphoreRelease(G_DACWAV_Workspace->iSem);
        return UEZ_ERROR_ILLEGAL_OPERATION;
    }
    
    G_DACWAV_Workspace->iUseBuffer1 = ETrue;
    G_DACWAV_Workspace->iNeedFillBuffer1 = EFalse;
    G_DACWAV_Workspace->iNeedFillBuffer2 = EFalse;
    G_DACWAV_Workspace->iCount = 0;
    
    G_DACWAV_Workspace->iBuffer1 = aBuffer;
    G_DACWAV_Workspace->iBytesBuffer1 = aSize;
    
    G_DACWAV_Workspace->iPlaying = ETrue;
    G_DACAudioTask = 0;


#if UEZ_DAC_WAV_FILE_TASK_BASED
    error = UEZTaskCreate((T_uezTaskFunction)DACAudioTask, "DAC Audio",
                          DAC_AUDIO_TASK_STACK_SIZE, 0, UEZ_PRIORITY_NORMAL,
                          &G_DACAudioTask);
    if (error) {
        UEZFailureMsg("UEZDAVWAVPlayBuffer failed to create task!");
    }
#else
   (*G_DACWAV_Workspace->iTimer)->Enable(G_DACWAV_Workspace->iTimer);
    error = UEZ_ERROR_NONE;
#endif
    UEZSemaphoreRelease(G_DACWAV_Workspace->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVCleanUp
 *---------------------------------------------------------------------------*/
/**
 *  Free all allocaed memory
 * 
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDACWAVCleanUp()
{
    if(G_DACWAV_Workspace){
       (*G_DACWAV_Workspace->iTimer)->Disable(G_DACWAV_Workspace->iTimer);
        UEZSemaphoreGrab(G_DACWAV_Workspace->iSem, UEZ_TIMEOUT_INFINITE);
        if(G_DACWAV_Workspace->iPlayFromFile){
            UEZMemFree(G_DACWAV_Workspace->iBuffer1);
            UEZMemFree(G_DACWAV_Workspace->iBuffer2);
        }

        UEZSemaphoreDelete(G_DACWAV_Workspace->iSem);
        G_DACWAV_Workspace->iSem = 0;
        UEZSemaphoreDelete(G_DACWAV_Workspace->iSemFeed);
        G_DACWAV_Workspace->iBuffer1 = 0;
        G_DACWAV_Workspace->iBuffer2 = 0;
        G_DACWAV_Workspace->iPlayFromMem = EFalse;
        G_DACWAV_Workspace->iPlayFromFile = EFalse;
        UEZMemFree(G_DACWAV_Workspace);
        G_DACWAV_Workspace = 0;
    }
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWAVGetStatus
 *---------------------------------------------------------------------------*/
/**
 *  Return if audio is playing or not
 *
 *  @return TBool 		True if playing False if not
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool UEZDACWAVGetStatus()
{
    return G_DACWAV_Workspace->iPlaying;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZDACWaveSetVolume
 *---------------------------------------------------------------------------*/
/**
 *  Set Volume
 *
 *  @param [in] 	aVolume			Volume to set (Range of 0-256)
 */
/*---------------------------------------------------------------------------*/
void UEZDACWaveSetVolume(TUInt16 aVolume)
{
    G_DACWAV_Workspace->iVolume = aVolume;
}

TUInt32 UEZDACWAVGetSamplePos(void)
{
    TUInt32 pos;

    RTOS_ENTER_CRITICAL();
    pos = G_DACWAV_Workspace->iSamplePos;
    RTOS_EXIT_CRITICAL();

    return pos;
}

TBool UEZDACWAVIsDone(void)
{
    // True if we are not playing anything
    return ((G_DACWAV_Workspace->iPlayFromMem == EFalse)
        && (G_DACWAV_Workspace->iPlayFromFile == EFalse));
}


/** @} */

