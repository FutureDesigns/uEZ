/*-------------------------------------------------------------------------*
 * File:  Stream.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Stream Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_STREAM_H_
#define _DEVICE_STREAM_H_

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
/**
 *  @file   /Include/Device/Stream.h
 *  @brief  uEZ Stream Device Interface
 *
 *  The uEZ Stream Device Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZDevice.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define STREAM_CONTROL_COMMAND_CODE             0
#define STREAM_CONTROL_SET_SERIAL_SETTINGS      1
#define STREAM_CONTROL_GET_SERIAL_SETTINGS      2
#define STREAM_CONTROL_CONTROL_HANDSHAKING      3
#define STREAM_CONTROL_HANDSHAKING_STATUS       4
#define STREAM_CONTROL_GET_READ_NUM_WAITING     5

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Open)(void *aWorkspace);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*Control)(void *aWorkspace, TUInt32 aControl, void *aControlData);
    T_uezError (*Read)(
            void *aWorkspace,
            TUInt8 *aData, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesRead,
            TUInt32 aTimeout);
    T_uezError (*Write)(
            void *aWorkspace,
            TUInt8 *aData, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesWritten,
            TUInt32 aTimeout);
    T_uezError (*Flush)(void *aWorkspace);
} DEVICE_STREAM;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_STREAM_H_
/*-------------------------------------------------------------------------*
 * End of File:  Stream.h
 *-------------------------------------------------------------------------*/
