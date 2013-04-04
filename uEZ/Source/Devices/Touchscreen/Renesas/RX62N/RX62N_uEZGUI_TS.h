/*-------------------------------------------------------------------------*
 * File:  RX62N_uEZGUI_TS.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Direct Drive Touchscreen interface.
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
#include <Device/ADCBank.h>
#include <HAL/GPIO.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    DEVICE_ADCBank **iADC;
    TUInt32 iADCChannel;
} RX62N_uEZGUI_TS_ADCLine;

typedef struct {
    HAL_GPIOPort **iGPIO;
    TUInt32 iGPIOBit;
} RX62N_uEZGUI_TS_GPIOLine;

typedef struct {
	RX62N_uEZGUI_TS_GPIOLine iXDrive;
	RX62N_uEZGUI_TS_GPIOLine iYDrive;
	RX62N_uEZGUI_TS_ADCLine iInputXL;	// LCD Left
    RX62N_uEZGUI_TS_ADCLine iInputXR;	// LCD Right
    RX62N_uEZGUI_TS_ADCLine iInputYU;	// LCD Up
	RX62N_uEZGUI_TS_ADCLine iInputYD;	// LCD Down
} RX62N_uEZGUI_TS_Configuration;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN Touchscreen_DirectDrive_Interface;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
T_uezError RX62N_uEZGUI_TS_Configure(
        T_uezDeviceWorkspace *aW, 
        RX62N_uEZGUI_TS_Configuration *aConfig);

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_uEZGUI_TS.h
 *-------------------------------------------------------------------------*/
