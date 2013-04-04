/*-------------------------------------------------------------------------*
 * File:  DirectDrive_TS.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Direct Drive Touchscreen interface.
 *
 *  Under Construction - Pending an RX ADC driver
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2011 Future Designs, Inc.
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
    const char *iADCName;
    TUInt8 iADCChannel;
} TS_DirectDrive_ADCLine;

typedef struct {
    T_uezGPIOPortPin iGPIO;
} TS_DirectDrive_GPIOLine;

typedef struct {
	TS_DirectDrive_GPIOLine iXDrive;
	TS_DirectDrive_GPIOLine iYDrive;
	TS_DirectDrive_ADCLine iXL;	// LCD Left
    TS_DirectDrive_ADCLine iXR;	// LCD Right
    TS_DirectDrive_ADCLine iYU;	// LCD Up
	TS_DirectDrive_ADCLine iYD;	// LCD Down
} TS_DirectDrive_Configuration;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_DirectDrive_Interface;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void Touchscreen_DirectDrive_Create(
        const char *aName,
        const TS_DirectDrive_Configuration *aConfig);

/*-------------------------------------------------------------------------*
 * End of File:  DirectDrive_TS.c
 *-------------------------------------------------------------------------*/
