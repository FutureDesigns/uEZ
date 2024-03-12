/*-------------------------------------------------------------------------*
 * File:  uEZWAVFile.c
 *-------------------------------------------------------------------------*
 * Description:
 *      
 * Implementation:
 *      
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZWAVFile
 *  @{
 *  @brief     uEZ WAVE File Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ WAVE File interface.
 *
 *  @par Example code:
 *  Example task to play a wave file
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *  TUInt32 playWAV(T_uezTask aMyTask, void *aParams)
 *  {
 *      UEZWAVConfig(128); // 50% volume
 *      if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *
 *          //void UEZDACWaveSetVolume(128); // 50% volume
 *          UEZWAVConfig(192); // 75% volume
 *			
 *          //	Stop playing the WAV File
 *          void UEZStopWAV();
 *			
 *          //	Start playing the selected WAV File
 *          void UEZStartWAV();
 *			
 *          // clean up a WAV file
 *          void UEZWAVCleanUp();
 *
 *      }
 *      return 0;
 *  }
 *  @endcode
 */
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
#include "Config.h"
#include "uEZDevice.h"
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZMemory.h>
#include <Device/AudioAmp.h>
#include <Device/AudioCodec.h>
#include <uEZAudioAmp.h>
#include <Device/I2S.h>
#include <HAL/I2SBus.h>
#include <uEZRTOS.h>
#include <uEZFile.h>
#include <uEZWAVFile.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>


T_uezFile file;
TUInt32 numBytesRead, numBytesRead2 = 1;
TInt32 *wav, *wav2;
TUInt32 index;
TInt8 volume;
TBool flag = ETrue, read1 = EFalse, read2 = ETrue;
T_uezDevice amp;
T_uezTask G_playerTask;

DEVICE_AudioCodec **ac;
T_uezDevice p_ac;
HAL_I2S **i2s;
T_uezDevice p_i2s, serial;
TUInt8 byte;
TUInt8 throwAway[50];

TBool playStatus;
TBool fileOpen = EFalse;
static TBool G_AudioAmpOpen = EFalse;

T_uezSemaphore wavSem;
#define DATA_TO_READ 1024

/*---------------------------------------------------------------------------*
 * Routine:  UEZWAWVolumeUp
 *---------------------------------------------------------------------------*/
/**
 *  WAWE Volume Up
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *      UEZWAWVolumeUp(); raise volume
 *      //	Start playing the selected WAV File
 *      UEZStartWAV();
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZWAWVolumeUp(void)
{
    volume++;
    if(volume > 64)
    {
        volume = 64;
        return;
    }
    
    if(G_AudioAmpOpen)
        UEZAudioAmpSetLevel(amp, volume);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZWAWVolumeDown
 *---------------------------------------------------------------------------*/
/**
 *  UEZ WAWE Volume Down
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *      UEZWAWVolumeDown(); lower volume
 *   	//	Start playing the selected WAV File
 *   	UEZStartWAV();
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZWAWVolumeDown(void)
{
    volume--;
    if(volume < 0)
    {
        volume = 0;
        return;
    }
    
    if(G_AudioAmpOpen)
        UEZAudioAmpSetLevel(amp, volume);
}

/*---------------------------------------------------------------------------*
 * Routine:  readNextBlock
 *---------------------------------------------------------------------------*/
/**
 *  Read in the next block
 *
 *  @param [in]    *data 		Pointer to data block
 *
 *  @param [in]    *BytesRead 	Pointer to number of bytes read
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  UEZFileOpen("TEST.WAV", FILE_FLAG_READ_ONLY, &file);
 *  UEZFileRead(file, &throwAway, 44, &numBytesRead);
 *  readNextBlock(wav, &numBytesRead);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void readNextBlock(TInt32 *data, TUInt32* BytesRead )
{
   UEZFileRead(file, data, DATA_TO_READ , BytesRead); 
}

/*---------------------------------------------------------------------------*
 * Routine:  i2sCallBack
 *---------------------------------------------------------------------------*/
/**
 *  Load sample data
 *
 *  @param [in]    *aCallbackWorkspace		Pointer to workspace
 *
 *  @param [in]    *samples					Pointer to samples
 *
 *  @param [in]    numSamples				Number of Samples (1-255)
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  TInt32 samples[];
 *  T_uezDeviceWorkspace workspace;
 *  void i2sCallBack(&workspace, samples, 50);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void i2sCallBack(void *aCallbackWorkspace,
                TInt32* samples,
                TUInt8 numSamples)
{
    TUInt16 i = 0;
    TInt32 *p;

    if (numSamples == 4) {
        if (flag) {
            p = wav+index;
        } else {
            p = wav2+index;
        }
        samples[i+0] = p[0];
        samples[i+1] = p[1];
        samples[i+2] = p[2];
        samples[i+3] = p[3];
        index += 4;
    } else {
        for( i = 0; i < numSamples; i++)
        {
            if(flag)
            {
                samples[i] = wav[index++];
            }
            else
            {
                samples[i] = wav2[index++];
            }
        }
    }
    if (index >= (DATA_TO_READ /4))
    {
        index = 0;
        if(flag) read1 = ETrue;
        if(!flag) read2 = ETrue;
        flag = !flag;
        _isr_UEZSemaphoreRelease(wavSem);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZWAVPlayFile
 *---------------------------------------------------------------------------*/
/**
 *  Play a file
 *
 *  @param [in]    *fileName		Pointer to filename
 *
 *  @return        T_uezError		Returns UEZ_ERROR_NONE if ok, else 
 *                                  returns error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *      UEZWAVConfig(192); // 75% volume
 *      //  Stop playing the WAV File
 *   	UEZStopWAV();
 *      //  Start playing the selected WAV File
 *   	UEZStartWAV();
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZWAVPlayFile(char* fileName)
{
    T_uezError error;
    
    error=UEZFileOpen(fileName, FILE_FLAG_READ_ONLY, &file);
    if(error != UEZ_ERROR_NONE)
        return error;
    fileOpen = ETrue;
    error = UEZFileRead(file, &throwAway, 44, &numBytesRead);
    readNextBlock(wav, &numBytesRead);
    readNextBlock(wav2, &numBytesRead2);
    read1 = 0;
    read2 = 0;
    index = 0;
    flag = ETrue;

    UEZTaskCreate((T_uezTaskFunction)UEZStartWAV, "Player", 4096, 0,
                UEZ_PRIORITY_NORMAL, &G_playerTask);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZStartWAV
 *---------------------------------------------------------------------------*/
/**
 *  Start WAVE
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *      UEZWAVConfig(192); // 75% volume
 *      // Stop playing the WAV File
 *      UEZStopWAV();
 *      // Start playing the selected WAV File
 *      UEZStartWAV();
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZStartWAV(void)
{
  (*i2s)->Start((void*)i2s);
  playStatus = ETrue;

    while (numBytesRead != 0 && numBytesRead2 != 0)
    {
        UEZSemaphoreGrab(wavSem, UEZ_TIMEOUT_INFINITE);
        if (read1)
        {
            readNextBlock(wav, &numBytesRead);
            read1 = EFalse;
        }
        if (read2)
        {
            readNextBlock(wav2, &numBytesRead2);
            read2 = EFalse;
        }
    }
    UEZStopWAV();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZStopWAV
 *---------------------------------------------------------------------------*/
/**
 *  Stop WAVE
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *      UEZWAVConfig(192); // 75% volume
 *      // Stop playing the WAV File
 *      UEZStopWAV();
 *      // Start playing the selected WAV File
 *      UEZStartWAV();
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZStopWAV(void)
{
    (*i2s)->Stop((void*)i2s);
    playStatus = EFalse;
    UEZFileClose(file);
    fileOpen = EFalse;
    UEZTaskDelete(G_playerTask);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZWAVCleanUp
 *---------------------------------------------------------------------------*/
/**
 *  UEZ WAVE CleanUp
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *      UEZWAVConfig(192); // 75% volume
 *      // Stop playing the WAV File
 *      UEZStopWAV();
 *      UEZWAVCleanUp();
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZWAVCleanUp(void)
{
    T_uezError error;

    error = (*ac)->UseConfiguration((void*)ac, 0);
    if(error != UEZ_ERROR_NONE){
        //the configuration did not take try again.
        (*ac)->UseConfiguration((void*)ac, 0);
    }
    if(G_AudioAmpOpen){
        UEZAudioAmpSetLevel(amp, 48);
        G_AudioAmpOpen = EFalse;
    }
    UEZMemFree(wav);
    UEZMemFree(wav2);
    UEZAudioAmpClose(amp);
    numBytesRead = 1;
    numBytesRead2 = 1;
    flag = ETrue;
    UEZSemaphoreDelete(wavSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZWAVChangeFile
 *---------------------------------------------------------------------------*/
/**
 *  Change the filename
 *
 *  @param [in]    *newFile			Pointer to filename
 *
 *  @return        T_uezError		Returns UEZ_ERROR_NONE if ok, else 
 *                                  returns error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *      UEZWAVConfig(192); // 75% volume
 *      // Stop playing the WAV File
 *      UEZStopWAV();
 *      // change wav file
 *      UEZWAVChangeFile("TEST2.WAV");
 *      // Start playing the selected WAV File
 *      UEZStartWAV();
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZWAVChangeFile(char* newFile)
{
    T_uezError error;
    
    UEZTaskSuspend(G_playerTask);
    (*i2s)->Stop((void*)i2s);//UEZStopWAV();
    playStatus = EFalse;
    UEZFileClose(file);
    fileOpen = EFalse;
    error = UEZFileOpen(newFile, FILE_FLAG_READ_ONLY, &file);
    if(error != UEZ_ERROR_NONE)
        return error;
     fileOpen = ETrue;
    error = UEZFileRead(file, &throwAway, 44, &numBytesRead);
    readNextBlock(wav, &numBytesRead);
    readNextBlock(wav2, &numBytesRead2);
    read1 = 0;
    read2 = 0;
    index = 0;
    flag = ETrue;
    playStatus = ETrue;
    UEZTaskResume(G_playerTask);
    (*i2s)->Start((void*)i2s);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZWAVConfig
 *---------------------------------------------------------------------------*/
/**
 *  Configurer UEZWAV
 *
 *  @param [in]    onBoardSpeakerVolume		Volume for onboard speaker (0-255)
 *
 *  @par Example Code:
 *  @code
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *      UEZWAVConfig(192); // 75% volume
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZWAVConfig(TUInt8 onBoardSpeakerVolume)
{
    T_uezError error;

    if(UEZAudioAmpOpen("AMP0", &amp) == UEZ_ERROR_NONE){
        UEZAudioAmpSetLevel(amp, onBoardSpeakerVolume);
        G_AudioAmpOpen = ETrue;
    }
    
    volume = onBoardSpeakerVolume;
    error = UEZDeviceTableFind("AudioCodec0", &p_ac);
    error = UEZDeviceTableGetWorkspace(p_ac, (T_uezDeviceWorkspace **)&ac);

    (*ac)->UseConfiguration((void*)ac, 1);
    if(error != UEZ_ERROR_NONE){
        (*ac)->UseConfiguration((void*)ac, 1);
    }

    wav = UEZMemAlloc(DATA_TO_READ);
    wav2 = UEZMemAlloc(DATA_TO_READ);

    error = UEZDeviceTableFind("I2S", &p_i2s);
    error = UEZDeviceTableGetWorkspace(p_i2s, (T_uezDeviceWorkspace **)&i2s);

    (*i2s)->Configure((void*)i2s, (HAL_I2S_Callback_Transmit_Low) i2sCallBack);
    
    UEZSemaphoreCreateBinary(&wavSem);
    playStatus = EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZWAVGetStatus
 *---------------------------------------------------------------------------*/
/**
 *  Get Play Status
 *
 *  @return        TBool	Play Status
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <UEZWAVFile.h>
 *
 *  TBool status;
 * 	if (UEZWAVPlayFile("TEST.WAV")  == UEZ_ERROR_NONE) {
 *      UEZWAVConfig(192); // 75% volume
 *      //	Start playing the selected WAV File
 *      UEZStartWAV();
 *      status = UEZWAVGetStatus();
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool UEZWAVGetStatus(void)
{
    return playStatus;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZWAVFile.c
 *-------------------------------------------------------------------------*/
