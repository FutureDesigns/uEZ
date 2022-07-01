/*-------------------------------------------------------------------------*
 * File:  AudioAmp_8551T.c
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZRTOS.h>
#include <HAL/HAL.h>
#include "AudioCodec_WM8731.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define WM8731_ADDR         (0x34>>1)
#define WM8731_I2C_Speed    100
#define WM8731__TIMEOUT     100
#define WM8731_LINVOL       (0x00<<1)
#define WM8731_RINVOL       (0x01<<1)
#define WM8731_LOUT1V       (0x02<<1)
#define WM8731_ROUT1V       (0x03<<1)
#define WM8731_APANA        (0x04<<1)
#define WM8731_APDIGI       (0x05<<1)
#define WM8731_PWR          (0x06<<1)
#define WM8731_IFACE        (0x07<<1)
#define WM8731_SRATE        (0x08<<1)
#define WM8731_ACTIVE       (0x09<<1)
#define WM8731_RESET	    (0x0F<<1)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    // whatever settings you need for this part
    TUInt16 LINVOL;
    TUInt16 RINVOL;
    TUInt16 LOUT1V;
    TUInt16 ROUT1V;
    TUInt16 APANA;
    TUInt16 APDIGI;
    TUInt16 PWR;
    TUInt16 IFACE;
    TUInt16 SRATE;
} T_wm8731ConfigSettings;

typedef struct {
    const DEVICE_AudioCodec *iDevice;

    T_uezSemaphore iSem;
    const char* iI2CBus;
    T_wm8731ConfigSettings iSettings;
} T_AudioCodec_WM8731_Workspace;

typedef enum {
    WM8731_CONFIGURATION_SIMPLE_SOUND=0,
    WM8731_CONFIGURATION_I2S_AUDIO=1
} T_wm8731ConfigurationIndex;

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
T_uezError AudioCodec_WM8731_InitializeWorkspace(void *aWorkSpace)
{
    T_uezError error;
    T_AudioCodec_WM8731_Workspace *p = (T_AudioCodec_WM8731_Workspace *)aWorkSpace;

    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

// In platform file, call this, 1 per configuration:
T_uezError AudioCodec_WM8731_SetConfiguration(void *aWorkSpace, const char* i2cBus)
{
    T_AudioCodec_WM8731_Workspace *p = (T_AudioCodec_WM8731_Workspace *)aWorkSpace;

    p->iI2CBus = i2cBus;
    return UEZ_ERROR_NONE;
}

T_uezError AudioCodec_WM8731_UseConfiguration(void *aWorkspace, TUInt32 aConfiguration)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_uezDevice i2c;
    TUInt8 data[11];
    T_AudioCodec_WM8731_Workspace *p = (T_AudioCodec_WM8731_Workspace *)aWorkspace;

    UEZI2COpen(p->iI2CBus, &i2c);

    switch( aConfiguration)
    {
    case WM8731_CONFIGURATION_SIMPLE_SOUND:
        //Setup simple here
        data[0] =WM8731_RESET;
        data[1] = 0x00;
        error = UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if (error)
            break;

        data[0] =WM8731_ACTIVE;
        data[1] = 0x01;
        error = UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if (error)
            break;

        data[0] = WM8731_PWR ;
        p->iSettings.PWR = data[1] = (0 << 7) | //POWEROFF
                (0 << 6) | //CLKOUTPD
                (0 << 5) | //OSCPD
                (1 << 4) | //OUTPD
                (1 << 3) | //DACPD
                (0 << 2) | //ADCPD
                (1 << 1) | //MICPD
                (0 << 0) ; //LINEINPD

        error = UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if (error)
            break;

        data[0] = WM8731_LINVOL;
        p->iSettings.LINVOL = data[1] = (0 << 7) |  //LINMUTE
                (0x14); //VOL

        p->iSettings.RINVOL = data[2] = (0 << 7) |  //RINMUTE
                (0x14); //VOL

        p->iSettings.LOUT1V = data[3] = (0 << 7) |  //RZCEN
                (0x6F); //VOL

        p->iSettings.ROUT1V = data[4] = (0 << 7) |  //RZCEN
                (0x6F); //VOL

        p->iSettings.APANA = data[5] =  (0 << 7) | //SIDEATT
                (0 << 6) | //SIDEATT
                (0 << 5) | //SIDETONE
                (0 << 4) | //DACSEL
                (1 << 3) | //BYPASS
                (0 << 2) | //INSEL
                (1 << 1) | //MUTEMIC
                (0 << 0) ; //MICBOOST

        p->iSettings.APDIGI = data[6] = (0 << 7) |
                (0 << 6) |
                (0 << 5) |
                (0 << 4) | //HPOR
                (0 << 3) | //DACMU
                (0 << 2) | //DEEMP[1]
                (0 << 1) | //DEEMP[0]
                (0 << 0) ; //ADCHPD

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 7, WM8731__TIMEOUT);
        if(error){
            break;
        }

        data[0] = WM8731_IFACE;
        p->iSettings.IFACE = data[1] = (0 << 7) | //BCLKINV
                (0 << 6) | //MS
                (0 << 5) | //LRSWAP
                (0 << 4) | //LPR
                (0 << 3) | //IWL[1]
                (0 << 2) | //IWL[0]
                (1 << 1) | //FORMAT[1]
                (0 << 0) ; //FORMAT[0]

        p->iSettings.SRATE = data[2] = (0 << 7) | //CLKODIV
                (0 << 6) | //CLKDIV2
                (0 << 5) | //SR[3]
                (0 << 4) | //SR[2]
                (0 << 3) | //SR[1]
                (0 << 2) | //SR[0]
                (0 << 1) | //BORS
                (1 << 0) ; //NORMAL 0/ USB 1

        data[10] = 0x01;

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 3, WM8731__TIMEOUT);
        if(error){
            break;
        }

        data[0] = WM8731_PWR;
        p->iSettings.PWR = data[8] = (1 << 7) | //POWERON
                (0 << 6) | //CLKOUTPD
                (0 << 5) | //OSCPD
                (0 << 4) | //OUTPD
                (1 << 3) | //DACPD
                (0 << 2) | //ADCPD
                (1 << 1) | //MICPD
                (0 << 0) ; //LINEINPD

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        break;
    case WM8731_CONFIGURATION_I2S_AUDIO:
        //set up I2S here
        data[0] =WM8731_RESET;
        data[1] = 0x00;

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] =WM8731_ACTIVE;
        data[1] = 0x00;

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] = WM8731_PWR ;
        p->iSettings.PWR = data[1] = (0 << 7) | //POWEROFF
                (0 << 6) | //CLKOUTPD
                (0 << 5) | //OSCPD
                (1 << 4) | //OUTPD
                (0 << 3) | //DACPD
                (1 << 2) | //ADCPD
                (1 << 1) | //MICPD
                (0 << 0) ; //LINEINPD

        //UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] = WM8731_LINVOL;
        p->iSettings.LINVOL = data[1] = (0 << 7) |  //LINMUTE
                (0x00); //VOL

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] = WM8731_RINVOL;
        p->iSettings.RINVOL = data[1] = (0 << 7) |  //RINMUTE
                (0x00); //VOL

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] = WM8731_LOUT1V;
        p->iSettings.LOUT1V = data[1] = (0 << 7) |  //RZCEN
                (0x6F); //VOL

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] =WM8731_ROUT1V;
        p->iSettings.ROUT1V = data[1] = (0 << 7) |  //RZCEN
                (0x6F); //VOL

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] =WM8731_APANA;
        p->iSettings.APANA = data[1] = 0x12;
        //            data[1] = (0 << 7) | //SIDEATT
        //                      (0 << 6) | //SIDEATT = -6 dB
        //                      (0 << 5) | //SIDETONE = Disabled Side Tone
        //                      (1 << 4) | //DACSEL = Select DAC for I2S input
        //                      (0 << 3) | //BYPASS = Disable Bypass
        //                      (0 << 2) | //INSEL = Line Input, not microphone
        //                      (1 << 1) | //MUTEMIC = Muted
        //                      (0 << 0) ; //MICBOOST

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] =WM8731_APDIGI;
        p->iSettings.APDIGI = data[1] = (0 << 7) |
                (0 << 6) |
                (0 << 5) |
                (0 << 4) | //HPOR
                (0 << 3) | //DACMU
                (0 << 2) | //DEEMP[1]
                (0 << 1) | //DEEMP[0]
                (0 << 0) ; //ADCHPD

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] = WM8731_IFACE;
        p->iSettings.IFACE = data[1] = (0 << 7) | //BCLKINV
                (0 << 6) | //MS
                (0 << 5) | //LRSWAP
                (0 << 4) | //LPR
                (0 << 3) | //IWL[1]
                (0 << 2) | //IWL[0]
                (1 << 1) | //FORMAT[1]
                (0 << 0) ; //FORMAT[0]

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        p->iSettings.SRATE = data[2] = (0 << 7) | //CLKODIV
                (0 << 6) | //CLKDIV2
                (1 << 5) | //SR[3]
                (0 << 4) | //SR[2]
                (1 << 3) | //SR[1]
                (0 << 2) | //SR[0]
                (1 << 1) | //BORS
                (1 << 0) ; //NORMAL 0/ USB 1

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] = WM8731_PWR;
        p->iSettings.PWR = data[1] = (0 << 7) | //POWERON
                (0 << 6) | //CLKOUTPD
                (0 << 5) | //OSCPD
                (0 << 4) | //OUTPD
                (0 << 3) | //DACPD
                (1 << 2) | //ADCPD
                (1 << 1) | //MICPD
                (1 << 0) ; //LINEINPD

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        data[0] =WM8731_ACTIVE ;
        data[1] = 0x01;

        UEZI2CWrite(i2c, WM8731_ADDR, WM8731_I2C_Speed, data, 2, WM8731__TIMEOUT);
        if(error)
            break;

        break;
    default:
        error = UEZ_ERROR_NOT_SUPPORTED;
        //return some error
        break;
    }
    UEZI2CClose(i2c);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  AudioCodec_WM8731_ADXL345_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create an audio codec driver for the Wolfson WM8731.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aI2CBusName -- Name of I2C bus device driver used by this
 *          device.
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError AudioCodec_WM8731_ADXL345_Create(
        const char *aName,
        const char *aI2CBusName)
{
    T_uezDeviceWorkspace *p;

    // Setup the accelerator device
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&AudioCodec_WM8731_Interface,
            0, &p);
    return AudioCodec_WM8731_SetConfiguration(p, aI2CBusName);
}
/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_AudioCodec AudioCodec_WM8731_Interface = {
        {
                // Common device interface
                "AudioCodec:Wolfson:WM8731",
                0x0100,
                AudioCodec_WM8731_InitializeWorkspace,
                sizeof(T_AudioCodec_WM8731_Workspace),
        },
        AudioCodec_WM8731_UseConfiguration,
        //AudioCodec_WM8731_SetLevel,
        //AudioCodec_WM8731_SetRate,
        //AudioCodec_WM8731_SetBitsPerSample,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  AudioCodec_WM8731.c
 *-------------------------------------------------------------------------*/
