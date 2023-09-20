/*-------------------------------------------------------------------------*
 * File:  AudioAmp_WM8731.c
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
#include "AudioAmp_WM8731.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define HIGH_LEVEL          (0x7F)
#define LOW_LEVEL           (0x30)
#define MODE_CONTROL        (0)
#define DIAGNOSTIC_CONTROL  (1)
#define FAULT_DETECTION     (2)
#define VOLUME_1            (3)
#define VOLUME_2            (4)
#define WM8731_ADDR         (0x34>>1)
#define WM8731_I2C_Speed    100

#define WM8731_LOUT1V       (0x02<<1)
#define WM8731_ROUT1V       (0x03<<1)

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
} T_AudioAmp_WM8731_Workspace;

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
T_uezError AudioAmp_WM8731_SetLevel(void *aWorkSpace, TUInt8 aLevel)
{
    T_AudioAmp_WM8731_Workspace *p = (T_AudioAmp_WM8731_Workspace *)aWorkSpace;
    T_uezDevice i2c;
    TUInt8 data[4];
    TUInt8 setLevel;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p->iLevel = aLevel;
    setLevel = ((p->iLevel * (p->iMaxLevel - p->iMinLevel))/0xFF) + p->iMinLevel;
    if(!p->iIsMuted){
        if(UEZI2COpen(p->iI2CBus, &i2c) == UEZ_ERROR_NONE){
            data[0] = WM8731_LOUT1V;
            data[1] = (0 << 7) |  //RZCEN
                    setLevel; //VOL
            UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, 100);
            data[0] = WM8731_ROUT1V;
            data[1] = (0 << 7) |  //RZCEN
                    setLevel; //VOL
            UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 3, 100);
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
TUInt8 AudioAmp_WM8731_GetLevel(void *aWorkSpace)
{
    T_AudioAmp_WM8731_Workspace *p = (T_AudioAmp_WM8731_Workspace *)aWorkSpace;
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
T_uezError AudioAmp_WM8731_Mute(void *aWorkSpace)
{
    T_AudioAmp_WM8731_Workspace *p = (T_AudioAmp_WM8731_Workspace *)aWorkSpace;
    T_uezDevice i2c;
    TUInt8 data[4];

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if(UEZI2COpen(p->iI2CBus, &i2c) == UEZ_ERROR_NONE){
        data[0] = WM8731_LOUT1V;
        data[1] = (0 << 7) |  //RZCEN
                  (0x00); //VOL
        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, 100);
        data[0] = WM8731_ROUT1V;
        data[1] = (0 << 7) |  //RZCEN
                  (0x00); //VOL
        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 3, 100);
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
T_uezError AudioAmp_WM8731_UnMute(void *aWorkSpace)
{
    T_AudioAmp_WM8731_Workspace *p = (T_AudioAmp_WM8731_Workspace *)aWorkSpace;
    T_uezDevice i2c;
    TUInt8 data[4];
    TUInt8 setLevel;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    if(UEZI2COpen(p->iI2CBus, &i2c) == UEZ_ERROR_NONE){
        setLevel = ((p->iLevel * (p->iMaxLevel - p->iMinLevel))/0xFF) + p->iMinLevel;

        data[0] = WM8731_LOUT1V;
        data[1] = (0 << 7) |  //RZCEN
                  setLevel; //VOL
        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, 100);
        data[0] = WM8731_ROUT1V;
        data[1] = (0 << 7) |  //RZCEN
                  setLevel; //VOL
        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 3, 100);
        UEZI2CClose(i2c);
    }
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Open
 *---------------------------------------------------------------------------*
 * Description:
 *     Initializes pins for the audio amp
 *     turns it on
 *     sets the level to the default
 * Inputs:
 *      void *aWorkSpace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError AudioAmp_WM8731_Open(void *aWorkSpace)
{
    T_AudioAmp_WM8731_Workspace *p = (T_AudioAmp_WM8731_Workspace *)aWorkSpace;
    T_uezDevice i2c;
    TUInt8 data[4];
    TUInt8 setLevel;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if(p->iNumOpen == 0){
        if(UEZI2COpen(p->iI2CBus, &i2c)== UEZ_ERROR_NONE){
            setLevel = ((p->iLevel * (p->iMaxLevel - p->iMinLevel))/0xFF) + p->iMinLevel;
            data[0] = WM8731_LOUT1V;
            data[1] = (0 << 7) |  //RZCEN
                      setLevel; //VOL
            UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, 100);
            data[0] = WM8731_ROUT1V;
            data[1] = (0 << 7) |  //RZCEN
                      setLevel; //VOL
            UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 3, 100);
            UEZI2CClose(i2c);
        }
    }
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
T_uezError AudioAmp_WM8731_Close(void *aWorkSpace)
{
    T_AudioAmp_WM8731_Workspace *p = (T_AudioAmp_WM8731_Workspace *)aWorkSpace;

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
T_uezError AudioAmp_WM8731_InitializeWorkspace(void *aWorkSpace)
{
    T_uezError error;
    T_AudioAmp_WM8731_Workspace *p = (T_AudioAmp_WM8731_Workspace *)aWorkSpace;

    p->iNumOpen = 0;
    p->iLevel = LOW_LEVEL*2;
    p->iIsMuted = EFalse;
    p->iIsOn = EFalse;

    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

void AudioAmp_WM8731_Create(
        const char *aName,
        const char *aI2CBus,
        TUInt8 aMaxValue)
{
    T_AudioAmp_WM8731_Workspace *p;
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&AudioAmp_WM8731_Interface,
            0,
            (T_uezDeviceWorkspace **)&p);

    strcpy(p->iI2CBus, aI2CBus);

    if(aMaxValue < HIGH_LEVEL){
        p->iMaxLevel = aMaxValue;
    } else {
        p->iMaxLevel = HIGH_LEVEL;
    }
    p->iMinLevel = LOW_LEVEL;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_AudioAmp AudioAmp_WM8731_Interface = { {
// Common device interface
    "AudioAmp:Wolfson:WM8731",
    0x0100,
    AudioAmp_WM8731_InitializeWorkspace,
    sizeof(T_AudioAmp_WM8731_Workspace), },

// Functions
    AudioAmp_WM8731_Open,
    AudioAmp_WM8731_Close,
    AudioAmp_WM8731_UnMute,
    AudioAmp_WM8731_Mute,
    AudioAmp_WM8731_GetLevel,
    AudioAmp_WM8731_SetLevel };

/*-------------------------------------------------------------------------*
 * End of File:  AudioAmp_WM8731.c
 *-------------------------------------------------------------------------*/
