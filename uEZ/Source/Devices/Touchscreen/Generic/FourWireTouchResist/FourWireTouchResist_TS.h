/*-------------------------------------------------------------------------*
 * File:  FourWireTouchResist_TS.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Four Wire Touchscreen interface.
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
#include <HAL/GPIO.h>
#include <Device/ADCBank.h>
#include <Device/Touchscreen.h>
#include <Types/GPIO.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const char *iADC;
    TUInt8 iADCChannel; // index / number of channel
    T_uezGPIOPortPin iGPIO;

    T_gpioMux iGPIOMuxAsGPIO;
    T_gpioMux iGPIOMuxAsADC;
} TS_FourWireTouchResist_ADCLine;

typedef struct {
    T_uezGPIOPortPin iGPIO;
} TS_FourWireTouchResist_GPIOLine;

typedef struct {
    TS_FourWireTouchResist_ADCLine iXPlus;
    TS_FourWireTouchResist_GPIOLine iXMinus;
    TS_FourWireTouchResist_ADCLine iYPlus;
    TS_FourWireTouchResist_GPIOLine iYMinus;
    TBool iIRQBased;
    TBool iNeedInputBufferDisabled;
} TS_FourWireTouchResist_Configuration;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_FourWireTouchResist_Interface;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
T_uezError TS_FourWireTouchResist_Configure(
                T_uezDeviceWorkspace *aW, 
                const TS_FourWireTouchResist_Configuration *aConfig);
void Touchscreen_FourWireTouchResist_Create(
        const char *aName,
        const TS_FourWireTouchResist_Configuration *aConfig);

/*-------------------------------------------------------------------------*
 * End of File:  FourWireTouchResist_TS.h
 *-------------------------------------------------------------------------*/
