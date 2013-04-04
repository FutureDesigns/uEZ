/*-------------------------------------------------------------------------*
 * File:  Types/I2C.h
 *-------------------------------------------------------------------------*
 * Description:
 *     I2C HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _I2C_TYPES_H_
#define _I2C_TYPES_H_

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


#endif // _I2C_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/I2C.h
 *-------------------------------------------------------------------------*/
