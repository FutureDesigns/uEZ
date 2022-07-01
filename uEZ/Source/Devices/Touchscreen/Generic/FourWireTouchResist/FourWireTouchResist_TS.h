/*-------------------------------------------------------------------------*
 * File:  FourWireTouchResist_TS.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Four Wire Touchscreen interface.
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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
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

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

/*-------------------------------------------------------------------------*
 * End of File:  FourWireTouchResist_TS.h
 *-------------------------------------------------------------------------*/
