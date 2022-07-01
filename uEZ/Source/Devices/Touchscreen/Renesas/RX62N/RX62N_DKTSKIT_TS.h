/*-------------------------------------------------------------------------*
 * File:  RX62N_DKTSKIT_TS.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Four Wire Touchscreen interface
 *      for the DKTSKIT with SOMDIMM-RX62N.  This should be replaced
 *      later with more generic drivers.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

#include <uEZ.h>
#include <Device/ADCBank.h>
#include <HAL/GPIO.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_DKTSKIT_TS.h
 *-------------------------------------------------------------------------*/
