/*-------------------------------------------------------------------------*
 * File:  Types/I2C.h
 *-------------------------------------------------------------------------*
 * Description:
 *     I2C HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _I2C_TYPES_H_
#define _I2C_TYPES_H_

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

/**
 *  @file   /Include/Types/I2C.h
 *  @brief  uEZ I2C Types
 *
 *  The uEZ I2C Types
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
#include <HAL/GPIO.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    I2C_OK=0,

    /** currently active */ 
    I2C_BUSY=1,

    /** some error occurred (bus arbitration?) */
    I2C_ERROR=2,

    /** Received NAK */
    I2C_NAK=3,

    /** Hardware is not ready to perform function */
    I2C_NOT_READY=4,
} T_uEZI2CStatus;


typedef struct {
    TUInt8 iAddr;           /** 7-bit address of I2C device */
    TUInt16 iSpeed;         /** in kHz */
    const TUInt8 *iWriteData;     /** 0 or NULL value means no write action */
    TUInt8 iWriteLength;
    TUInt32 iWriteTimeout;
    TUInt8 *iReadData;      /** 0 or NULL value means no read action */
    TUInt8 iReadLength;
    TUInt32 iReadTimeout;
    volatile T_uEZI2CStatus iStatus;
} I2C_Request;

typedef enum {
    I2C_SLAVE_EVENT_RECEIVED, I2C_SLAVE_EVENT_SENT,
} T_I2CSlaveEvent;

typedef struct {
    TUInt8 iAddressLower7Bits;
    TUInt8 *iSendBuffer;
    TUInt32 iMaxSendLength;
    TUInt8 *iReceiveBuffer;
    TUInt32 iMaxReceiveLength;
} T_I2CSlaveSetup;

#ifdef __cplusplus
}
#endif

#endif // _I2C_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/I2C.h
 *-------------------------------------------------------------------------*/
