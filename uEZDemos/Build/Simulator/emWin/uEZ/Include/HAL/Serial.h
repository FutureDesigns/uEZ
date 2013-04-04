/*-------------------------------------------------------------------------*
 * File:  Serial.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Serial HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_SERIAL_H_
#define _HAL_SERIAL_H_

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
 *  @file   /Include/HAL/Serial.h
 *  @brief  uEZ Serial HAL Interface
 *
 *  The uEZ Serial HAL Interface
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
#include <HAL/HAL.h>
#include <Types/Serial.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*HAL_Serial_Callback_Transmit_Empty)(
                void *aCallbackWorkspace,
                TUInt32 aNumBytesAvailable);
typedef void (*HAL_Serial_Callback_Received_Byte)(
                void *aCallbackWorkspace, 
                TUInt8 aByte);

typedef TUInt8 T_serialStatusByte;
#define SERIAL_STATUS_TRANSMIT_EMPTY        (1<<0)

typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*Configure)(
                void *aWorkspace,
                void *aCallbackWorkspace,
                HAL_Serial_Callback_Received_Byte aReceivedByteCallback,
                HAL_Serial_Callback_Transmit_Empty aTransmitEmptyCallback);
    T_uezError (*SetSerialSettings)(
                    void *aWorkspace, 
                    T_Serial_Settings *aSettings);
    T_uezError (*GetSerialSettings)(
                    void *aWorkspace, 
                    T_Serial_Settings *aSettings);
    T_uezError (*Activate)(void *aWorkspace);
    T_uezError (*Deactivate)(void *aWorkspace);
    T_uezError (*OutputByte)(
                void *aWorkspace, 
                TUInt8 aByte);
    T_uezError (*SetHandshakingControl)(
                    void *aWorkspace,
                    T_serialHandshakingControl aControl);
    T_uezError (*GetHandshakingStatus)(
                    void *aWorkspace,
                    T_serialHandshakingStatus *aControl);
    T_serialStatusByte (*GetStatus)(void *aWorkspace);
} HAL_Serial ;

#endif // _HAL_SERIAL_H_
/*-------------------------------------------------------------------------*
 * End of File:  Serial.h
 *-------------------------------------------------------------------------*/
