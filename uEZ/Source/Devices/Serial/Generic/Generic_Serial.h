/*-------------------------------------------------------------------------*
 * File:  Serial_Generic.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef SERIAL_GENERIC_H_
#define SERIAL_GENERIC_H_

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


/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <HAL/Serial.h>
#include <Device/Stream.h>
#include <HAL/GPIO.h>
#include <Types/GPIO.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_STREAM Serial_Generic_Stream_Interface;
extern const DEVICE_STREAM Serial_GenericHalfDuplex_Stream_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError Serial_Generic_Configure(
            T_uezDeviceWorkspace *aWorkspace,
            HAL_Serial **aSerial,
            TUInt32 aQueueSendSize,
            TUInt32 aQueueReceiveSize);
T_uezError Serial_GenericHalfDuplex_Configure(
        T_uezDeviceWorkspace *aWorkspace,
        HAL_Serial **aSerial,
        TUInt32 aQueueSendSize,
        TUInt32 aQueueReceiveSize,
        HAL_GPIOPort **aDriveEnablePort,
        TUInt32 aDriveEnablePin,
        TBool aDriveEnablePolarity,
        TUInt32 aDriveEnableReleaseTime);
T_uezError Serial_Generic_FullDuplex_Stream_Create(
        const char *aName,
        const char *aSerialHALName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
T_uezError Serial_Generic_HalfDuplex_Stream_Create(
        const char *aName,
        const char *aSerialHALName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize,
        T_uezGPIOPortPin aDriveEnablePortPin,
        TBool aDriveEnablePolarity,
        TUInt32 aDriveEnableReleaseTime);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_GENERIC_H_
/*-------------------------------------------------------------------------*
 * End of File:  Serial_Generic.h
 *-------------------------------------------------------------------------*/
