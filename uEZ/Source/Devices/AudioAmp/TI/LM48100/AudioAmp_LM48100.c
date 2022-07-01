/*-------------------------------------------------------------------------*
 * File:  AudioAmp_LM48110.c
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
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZRTOS.h>
#include <uEZGPIO.h>
#include <HAL/HAL.h>
#include <HAL/GPIO.h>
#include <uEZI2C.h>
#include "AudioAmp_LM48100.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define HIGH_LEVEL          (0x1F)
#define MODE_CONTROL        (0)
#define DIAGNOSTIC_CONTROL  (1)
#define FAULT_DETECTION     (2)
#define VOLUME_1            (3)
#define VOLUME_2            (4)
#define LM48100_ADDR        (0x7C)
#define LM48100_SPEED       (400)

#define VOLUME_1_MASK       (0x60)
#define VOLUME_2_MASK       (0x80)

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
    char iI2CBus[5];
    TUInt8 iMaxLevel;
    TUInt8 iMinLevel;
} T_AudioAmp_LM48110_Workspace;

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
T_uezError AudioAmp_LM48110_SetLevel(void *aWorkSpace, TUInt8 aLevel)
{
    T_AudioAmp_LM48110_Workspace *p = (T_AudioAmp_LM48110_Workspace *)aWorkSpace;
    T_uezDevice i2c;
    TUInt8 data[5] = { 0x1C, //Turn on both inputs
                       0x20, //Mask for Diagnostic
                       0x40, //Mask for Fault
    };
    TUInt8 setLevel;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    p->iLevel = aLevel;
    setLevel = ((p->iLevel * (p->iMaxLevel - p->iMinLevel))/0xFF) + p->iMinLevel;

    data[3] = (VOLUME_1_MASK | setLevel);
    data[4] = (VOLUME_2_MASK | setLevel);

    if(!p->iIsMuted){
        if(UEZI2COpen(p->iI2CBus, &i2c) == UEZ_ERROR_NONE){
            UEZI2CWrite(i2c, LM48100_ADDR, LM48100_SPEED, data, 5, 100);//UEZ_TIMEOUT_INFINITE);
            UEZI2CClose(i2c);
        }
    }

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
TUInt8 AudioAmp_LM48110_GetLevel(void *aWorkSpace)
{
    T_AudioAmp_LM48110_Workspace *p = (T_AudioAmp_LM48110_Workspace *)aWorkSpace;
    return p->iLevel;
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
T_uezError AudioAmp_LM48110_Mute(void *aWorkSpace)
{
    T_AudioAmp_LM48110_Workspace *p = (T_AudioAmp_LM48110_Workspace *)aWorkSpace;
    T_uezDevice i2c;
    TUInt8 data[5] = { 0x1C, //Turn on both inputs
                       0x20, //Mask for Diagnostic
                       0x40, //Mask for Fault
    };

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    data[3] = (VOLUME_1_MASK | 0);
    data[4] = (VOLUME_2_MASK | 0);
    p->iIsMuted = ETrue;

    if(UEZI2COpen(p->iI2CBus, &i2c) == UEZ_ERROR_NONE){
        UEZI2CWrite(i2c, LM48100_ADDR, LM48100_SPEED, data, 5, 100);//UEZ_TIMEOUT_INFINITE);
        UEZI2CClose(i2c);
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
T_uezError AudioAmp_LM48110_UnMute(void *aWorkSpace)
{
    T_AudioAmp_LM48110_Workspace *p = (T_AudioAmp_LM48110_Workspace *)aWorkSpace;
    T_uezDevice i2c;
    TUInt8 data[5] = { 0x1C, //Turn on both inputs
                       0x20, //Mask for Diagnostic
                       0x40, //Mask for Fault
    };
    TUInt8 setLevel;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    setLevel = ((p->iLevel * (p->iMaxLevel - p->iMinLevel))/0xFF) + p->iMinLevel;
    data[3] = (VOLUME_1_MASK | setLevel);
    data[4] = (VOLUME_2_MASK | setLevel);
    p->iIsMuted = EFalse;

    if(UEZI2COpen(p->iI2CBus, &i2c) == UEZ_ERROR_NONE){
        UEZI2CWrite(i2c, LM48100_ADDR, LM48100_SPEED, data, 5, 100);//UEZ_TIMEOUT_INFINITE);
        UEZI2CClose(i2c);
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
T_uezError AudioAmp_LM48110_Open(void *aWorkSpace)
{
    T_AudioAmp_LM48110_Workspace *p = (T_AudioAmp_LM48110_Workspace *)aWorkSpace;
    T_uezDevice i2c;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt8 data[5] = { 0x1C, //Turn on both inputs
                       0x20, //Mask for Diagnostic
                       0x40, //Mask for Fault
                       (VOLUME_1_MASK | 0x00), //Set the volume to 0
                       (VOLUME_2_MASK | 0x00)}; // Set the volume to 0

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if(p->iNumOpen == 0){
        p->iIsOn = ETrue;
        if(UEZI2COpen(p->iI2CBus, &i2c) == UEZ_ERROR_NONE){
            p->iNumOpen++;
            error = UEZI2CWrite(i2c, LM48100_ADDR, LM48100_SPEED, data, 5, 100);//UEZ_TIMEOUT_INFINITE);
            UEZI2CClose(i2c);
        }
        p->iLevel = 0;
    }
    UEZSemaphoreRelease(p->iSem);
    return error;
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
T_uezError AudioAmp_LM48110_Close(void *aWorkSpace)
{
    T_AudioAmp_LM48110_Workspace *p = (T_AudioAmp_LM48110_Workspace *)aWorkSpace;

    if (p->iIsOn) {
        p->iNumOpen--;
    }

    if (p->iNumOpen == 0) {
        p->iIsOn = EFalse;
    }

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
T_uezError AudioAmp_LM48110_InitializeWorkspace(void *aWorkSpace)
{
    T_uezError error;
    T_AudioAmp_LM48110_Workspace *p = (T_AudioAmp_LM48110_Workspace *)aWorkSpace;

    p->iNumOpen = 0;
    p->iLevel = 0;
    p->iIsMuted = EFalse;
    p->iIsOn = EFalse;

    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

void AudioAmp_LM48110_Create(
        const char *aName,
        const char *aI2CBus,
        TUInt8 aMaxValue)
{
    T_AudioAmp_LM48110_Workspace *p;
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&AudioAmp_LM48110_Interface,
            0,
            (T_uezDeviceWorkspace **)&p);

    strcpy(p->iI2CBus, aI2CBus);

    if(aMaxValue < HIGH_LEVEL){
        p->iMaxLevel = aMaxValue;
    } else {
        p->iMaxLevel = HIGH_LEVEL;
    }
    p->iMinLevel = 0;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_AudioAmp AudioAmp_LM48110_Interface = { {
// Common device interface
    "AudioAmp:NXP:LM48110",
    0x0100,
    AudioAmp_LM48110_InitializeWorkspace,
    sizeof(T_AudioAmp_LM48110_Workspace), },

// Functions
    AudioAmp_LM48110_Open,
    AudioAmp_LM48110_Close,
    AudioAmp_LM48110_UnMute,
    AudioAmp_LM48110_Mute,
    AudioAmp_LM48110_GetLevel,
    AudioAmp_LM48110_SetLevel };

/*-------------------------------------------------------------------------*
 * End of File:  AudioAmp_LM48110.c
 *-------------------------------------------------------------------------*/
