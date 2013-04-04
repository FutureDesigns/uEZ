/*-------------------------------------------------------------------------*
 * File:  AudioAmp_8551T.c
 *-------------------------------------------------------------------------*
 * Description:
 *      
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

#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZRTOS.h>
#include <uEZGPIO.h>
#include <HAL/HAL.h>
#include <HAL/GPIO.h>
#include "AudioAmp_TDA8551T.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define HIGH_LEVEL      64 //total number of levels
#define LOW_LEVEL       0  //lowest level
#define DEFAULT_LEVEL   (HIGH_LEVEL/2)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_AudioAmp *iDevice;

    T_uezSemaphore iSem;
    TUInt32 iNumOpen;
    TUInt8 iLevel;
    TBool iIsMuted;
    TBool iIsOn;
    T_uezGPIOPortPin iVolumeGPIO;
    T_uezGPIOPortPin iModeGPIO;
    T_uezGPIOPortPin iVolEnableGPIO;
//    HAL_GPIOPort **iVolumeGPIO;
//    HAL_GPIOPort **iModeGPIO;
//#if UEZ_OPTIONAL_VOL_CTRL_ENABLE
//HAL_GPIOPort **iVolEnableGPIO;
//#endif
} T_AudioAmp_8551T_Workspace;

/*---------------------------------------------------------------------------*
 * Routine: SetLevel
 *---------------------------------------------------------------------------*
 * Description: 
 *      Set the gain of the audio amp to the desired level
 * Inputs:
 *      void *aWorkspace
 *      TUInt8 aLevel             --new level to set the amp to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError AudioAmp_8551T_SetLevel(void *aWorkSpace, TUInt8 aLevel)
{
    T_AudioAmp_8551T_Workspace *p = (T_AudioAmp_8551T_Workspace *)aWorkSpace;
    TUInt8 i, levels = p->iLevel;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    UEZGPIOSetMux(p->iVolumeGPIO, 0); // set to GPIO

    if (levels != aLevel) {
        if (levels < aLevel) {
            for (i = levels; i < aLevel; i++) {
                UEZGPIOSet(p->iVolumeGPIO); //set High to go up
                UEZGPIOOutput(p->iVolumeGPIO); //set to output
                if (p->iVolEnableGPIO != GPIO_NONE)
                    UEZGPIOSet(p->iVolEnableGPIO);
                UEZTaskDelay(1);
                if (p->iVolEnableGPIO != GPIO_NONE)
                    UEZGPIOClear(p->iVolEnableGPIO);
                UEZGPIOInput(p->iVolumeGPIO);
                UEZGPIOClear(p->iVolumeGPIO);
                UEZGPIOSetPull(p->iVolumeGPIO, GPIO_PULL_NONE);
                UEZTaskDelay(1);
            }
        } else {
            for (i = levels; i > aLevel; i--) {
                UEZGPIOClear(p->iVolumeGPIO); //set low to go down
                UEZGPIOOutput(p->iVolumeGPIO); //set to output
                if (p->iVolEnableGPIO != GPIO_NONE)
                    UEZGPIOSet(p->iVolEnableGPIO);
                UEZTaskDelay(1);
                if (p->iVolEnableGPIO != GPIO_NONE)
                    UEZGPIOClear(p->iVolEnableGPIO);
                UEZGPIOInput(p->iVolumeGPIO);
                UEZGPIOSetPull(p->iVolumeGPIO, GPIO_PULL_NONE);
                UEZTaskDelay(1);
            }
        }
        p->iLevel = aLevel;
    }

    if (p->iVolEnableGPIO != GPIO_NONE)
        UEZGPIOClear(p->iVolEnableGPIO);
    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  GetLevel
 *---------------------------------------------------------------------------*
 * Description:
 *     Routine returns the current level that the audio amp is set to
 * Inputs:
 *      void *aWorkSpace
 * Outputs:
 *      TUInt8                   -- The Level
 *---------------------------------------------------------------------------*/
TUInt8 AudioAmp_8551T_GetLevel(void *aWorkSpace)
{
    T_AudioAmp_8551T_Workspace *p = (T_AudioAmp_8551T_Workspace *)aWorkSpace;
    TUInt8 levels = p->iLevel;

    return levels;
}

/*---------------------------------------------------------------------------*
 * Routine:  Mute
 *---------------------------------------------------------------------------*
 * Description:
 *     Sets the amp to standby so no sound is played
 * Inputs:
 *      void *aWorkSpace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError AudioAmp_8551T_Mute(void *aWorkSpace)
{
    T_AudioAmp_8551T_Workspace *p = (T_AudioAmp_8551T_Workspace *)aWorkSpace;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (!p->iIsMuted) {
        p->iIsMuted = ETrue;
        UEZGPIOSetMux(p->iModeGPIO, 0); // set to GPIO
        UEZGPIOSet(p->iModeGPIO);
    }

    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UnMute
 *---------------------------------------------------------------------------*
 * Description:
 *     Takes the amp out of standby mode
 * Inputs:
 *      void *aWorkSpace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError AudioAmp_8551T_UnMute(void *aWorkSpace)
{
    T_AudioAmp_8551T_Workspace *p = (T_AudioAmp_8551T_Workspace *)aWorkSpace;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if (p->iIsMuted) {
        p->iIsMuted = EFalse;
        UEZGPIOSetMux(p->iModeGPIO, 0); // set to GPIO
        UEZGPIOClear(p->iModeGPIO);
    }

    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Open
 *---------------------------------------------------------------------------*
 * Description:
 *     Initalizes pins for the audio amp 
 *     turns it on
 *     sets the level to the default
 * Inputs:
 *      void *aWorkSpace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError AudioAmp_8551T_Open(void *aWorkSpace)
{
    T_AudioAmp_8551T_Workspace *p = (T_AudioAmp_8551T_Workspace *)aWorkSpace;
    TUInt8 i, levels = p->iLevel;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    p->iNumOpen++;

    if (!(p->iIsOn)) {
        p->iIsOn = ETrue;
        UEZGPIOOutput(p->iModeGPIO);
        UEZGPIOSetMux(p->iModeGPIO, 0); // set to GPIO
        UEZGPIOClear(p->iModeGPIO); // Turn on the amp
        UEZGPIOOutput(p->iVolumeGPIO);
        UEZGPIOSetMux(p->iVolumeGPIO, 0); // set to GPIO

        for (i = HIGH_LEVEL; i > 0; i--) {
            UEZGPIOClear(p->iVolumeGPIO); // set low to go down
            UEZGPIOOutput(p->iVolumeGPIO); // set to output
            if (p->iVolEnableGPIO != GPIO_NONE)
                UEZGPIOSet(p->iVolEnableGPIO);
            UEZTaskDelay(1);
            if (p->iVolEnableGPIO != GPIO_NONE)
                UEZGPIOClear(p->iVolEnableGPIO);
            UEZGPIOInput(p->iVolumeGPIO); // set to input
            UEZGPIOSetPull(p->iVolumeGPIO, GPIO_PULL_NONE);
            UEZTaskDelay(1);
        }

        for (i = LOW_LEVEL; i < levels; i++) {
            UEZGPIOSet(p->iVolumeGPIO); // set low to go down
            UEZGPIOOutput(p->iVolumeGPIO); // set to output
            if (p->iVolEnableGPIO != GPIO_NONE)
                UEZGPIOSet(p->iVolEnableGPIO);
            UEZTaskDelay(1);
            if (p->iVolEnableGPIO != GPIO_NONE)
                UEZGPIOClear(p->iVolEnableGPIO);
            UEZGPIOInput(p->iVolumeGPIO); // set to input
            UEZGPIOSetPull(p->iVolumeGPIO, GPIO_PULL_NONE);
            UEZTaskDelay(1);
        }
    }
    if (p->iVolEnableGPIO != GPIO_NONE)
        UEZGPIOClear(p->iVolEnableGPIO);

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Close
 *---------------------------------------------------------------------------*
 * Description:
 *     Puts the device in standby when iNumOpen is 0
 * Inputs:
 *      void *aWorkSpace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError AudioAmp_8551T_Close(void *aWorkSpace)
{
    T_AudioAmp_8551T_Workspace *p = (T_AudioAmp_8551T_Workspace *)aWorkSpace;

    if (p->iIsOn) {
        p->iNumOpen--;
    }

    if (p->iNumOpen == 0) {
        UEZGPIOOutput(p->iModeGPIO);
        UEZGPIOSetMux(p->iModeGPIO, 0); // set to GPIO
        UEZGPIOSet(p->iModeGPIO); // set to standby
        p->iIsOn = EFalse;
        p->iIsMuted = EFalse;
        p->iLevel = DEFAULT_LEVEL;
    }
    if (p->iVolEnableGPIO != GPIO_NONE)
        UEZGPIOClear(p->iVolEnableGPIO);

    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *     Sets the default settings for the amp
 * Inputs:
 *      void *aWorkSpace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError AudioAmp_8551T_InitializeWorkspace(void *aWorkSpace)
{
    T_uezError error;
    T_AudioAmp_8551T_Workspace *p = (T_AudioAmp_8551T_Workspace *)aWorkSpace;

    p->iNumOpen = 0;
    p->iLevel = DEFAULT_LEVEL;
    p->iIsMuted = EFalse;
    p->iIsOn = EFalse;

    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

void AudioAmp_8551T_Create(
        const char *aName,
        T_uezGPIOPortPin aVolumeGPIO,
        T_uezGPIOPortPin aModeGPIO,
        T_uezGPIOPortPin aVolEnableGPIO)
{
    T_AudioAmp_8551T_Workspace *p;
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&AudioAmp_8551T_Interface,
            0,
            (T_uezDeviceWorkspace **)&p);

    UEZGPIOLock(aVolumeGPIO);
    p->iVolumeGPIO = aVolumeGPIO;
    UEZGPIOLock(aModeGPIO);
    p->iModeGPIO = aModeGPIO;
    if (aVolEnableGPIO != GPIO_NONE)
        UEZGPIOLock(aVolEnableGPIO);
    p->iVolEnableGPIO = aVolEnableGPIO;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_AudioAmp AudioAmp_8551T_Interface = { {
// Common device interface
    "AudioAmp:NXP:8551T",
    0x0100,
    AudioAmp_8551T_InitializeWorkspace,
    sizeof(T_AudioAmp_8551T_Workspace), },

// Functions
    AudioAmp_8551T_Open,
    AudioAmp_8551T_Close,
    AudioAmp_8551T_UnMute,
    AudioAmp_8551T_Mute,
    AudioAmp_8551T_GetLevel,
    AudioAmp_8551T_SetLevel };

/*-------------------------------------------------------------------------*
 * End of File:  AudioAmp_8551T.c
 *-------------------------------------------------------------------------*/
