/*-------------------------------------------------------------------------*
 * File:  Generic_RS485.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef GENERIC_RS485_TIMER_H_
#define GENERIC_RS485_TIMER_H_

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


/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Types/GPIO.h>
#include <HAL/Serial.h>
#include <HAL/GPIO.h>
#include <HAL/Timer.h>
#include <Device/Stream.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        // HAL Serial driver name
        const char *iSerialName;

        // Size of send and receive queues in number of bytes
        TUInt32 iQueueSendSize;
        TUInt32 iQueueReceiveSize;

        // Port pin to control drive enable, or GPIO_NONE for none
        T_uezGPIOPortPin iDriveEnable;

        // Drive enable is high true when ETrue
        TBool iDriveEnablePolarity;

        // Pin to control receive enable, or GPIO_NONE for none
        T_uezGPIOPortPin iReceiveEnable;

        // Receive enable is high true when ETrue
        TBool iReceiveEnablePolarity;

        const char *iTimerName; // HAL Timer driver name
        TUInt32 iReleaseTime; // Release time in CPU cycles
        TUInt32 iDriveTime;     // Drive time in milliseconds
} T_RS485_Generic_Timer_Settings;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_STREAM RS485_Generic_Timer_Stream_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError RS485_Generic_Timer_Configure(
    T_uezDeviceWorkspace *aWorkspace,
    HAL_Serial **aSerial,
    TUInt32 aQueueSendSize,
    TUInt32 aQueueReceiveSize,
    HAL_GPIOPort **aDriveEnablePort,
    TUInt32 aDriveEnablePin,
    TBool aDriveEnablePolarity,
    HAL_GPIOPort **aReceiveEnablePort,
    TUInt32 aReceiveEnablePin,
    TBool aReceiveEnablePolarity,

    HAL_Timer **aTimer,
    TUInt32 aReleaseTimeCPUCycles,
    TUInt32 aDriveTimeMS);
T_uezError RS485_Generic_Timer_Create(
    const char *aName,
    const T_RS485_Generic_Timer_Settings *aSettings);

#ifdef __cplusplus
}
#endif

#endif // GENERIC_RS485_TIMER_H_
/*-------------------------------------------------------------------------*
 * End of File:  Generic_RS485.h
 *-------------------------------------------------------------------------*/
