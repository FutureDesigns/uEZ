/*-------------------------------------------------------------------------*
 * File:  Generic_RS485.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef GENERIC_RS485_H_
#define GENERIC_RS485_H_

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Types/GPIO.h>
#include <Device/Stream.h>
#include <HAL/Serial.h>
#include <HAL/GPIO.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Name of HAL Serial device driver to use
    const char *iSerialName;

    // Size of the send and receive buffers in bytes
    TUInt32 iQueueSendSize;
    TUInt32 iQueueReceiveSize;

    // Drive enable gpio pin
    T_uezGPIOPortPin iDriveEnable;

    // Drive enable polarity (ETrue = HIGH true, EFalse = LOW true)
    TBool iDriveEnablePolarity;

    // How long to wait after drive is released before continuing? (in ms)
    TUInt32 iDriveEnableReleaseTime;

    // Receive enable gpio port pin
    T_uezGPIOPortPin iReceiveEnable;

    // Receive enable polarity (ETrue = HIGH true, EFalse = LOW true)
    TBool iReceiveEnablePolarity;

    // Receive enable release time (in milliseconds)
    TUInt32 aReceiveEnableReleaseTime;
} T_RS485_GenericHalfDuplex_Settings;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_STREAM RS485_GenericHalfDuplex_Stream_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError RS485_GenericHalfDuplex_Configure(
            T_uezDeviceWorkspace *aWorkspace,
            HAL_Serial **aSerial,
            TUInt32 aQueueSendSize,
            TUInt32 aQueueReceiveSize,
            HAL_GPIOPort **aDriveEnablePort,
            TUInt32 aDriveEnablePin,
            TBool aDriveEnablePolarity,
            TUInt32 aDriveEnableReleaseTime,
            HAL_GPIOPort **aReceiveEnablePort,
            TUInt32 aReceiveEnablePin,
            TBool aReceiveEnablePolarity,
            TUInt32 aReceiveEnableReleaseTime);
T_uezError RS485_GenericHalfDuplex_Create(
    const char *aName,
    const T_RS485_GenericHalfDuplex_Settings *aSettings);
#endif // GENERIC_RS485_H_
/*-------------------------------------------------------------------------*
 * End of File:  Generic_RS485.h
 *-------------------------------------------------------------------------*/
