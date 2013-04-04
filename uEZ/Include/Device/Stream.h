/*-------------------------------------------------------------------------*
 * File:  Stream.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Stream Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_STREAM_H_
#define _DEVICE_STREAM_H_

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

#endif // _DEVICE_STREAM_H_
/*-------------------------------------------------------------------------*
 * End of File:  Stream.h
 *-------------------------------------------------------------------------*/
