/*-------------------------------------------------------------------------*
 * File:  RX62N_DKTSKIT_TS.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Four Wire Touchscreen interface
 *      for the DKTSKIT with SOMDIMM-RX62N.  This should be replaced
 *      later with more generic drivers.
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
#include <Device/ADCBank.h>
#include <HAL/GPIO.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    DEVICE_ADCBank **iADC;
    TUInt32 iADCChannel;
    HAL_GPIOPort **iGPIO;
    TUInt32 iGPIOBit;

    T_gpioMux iGPIOMuxAsADC;
    T_gpioMux iGPIOMuxAsGPIO;
} TS_RX62N_DKTSKIT_ADCLine;

typedef struct {
    HAL_GPIOPort **iGPIO;
    TUInt32 iGPIOBit;
} TS_RX62N_DKTSKIT_GPIOLine;

typedef struct {
    TS_RX62N_DKTSKIT_ADCLine iXPlus;
    TS_RX62N_DKTSKIT_GPIOLine iXMinus;
    TS_RX62N_DKTSKIT_ADCLine iYPlus;
    TS_RX62N_DKTSKIT_GPIOLine iYMinus;
} TS_RX62N_DKTSKIT_Configuration;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_RX62N_DKTSKIT_Interface;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
//T_uezError TS_RX62N_DKTSKIT_Configure(
//                T_uezDeviceWorkspace *aW, 
//                TS_RX62N_DKTSKIT_Configuration *aConfig);

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_DKTSKIT_TS.h
 *-------------------------------------------------------------------------*/
