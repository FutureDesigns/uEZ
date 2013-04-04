/*-------------------------------------------------------------------------*
 * File:  I2CBUS.h
 *-------------------------------------------------------------------------*
 * Description:
 *     I2CBUS HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_I2CBUS_H_
#define _HAL_I2CBUS_H_

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
 *  @file   /Include/HAL/I2CBUS.h
 *  @brief  uEZ I2C BUS HAL Interface
 *
 *  The uEZ I2C BUS HAL Interface
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
#include <Types/I2C.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
/**
 * Master callbacks:
 */
typedef void (*I2CRequestCompleteCallback)(
    void *aCallbackWorkspace,
    I2C_Request *iRequest);

/**
 * Slave callbacks:
 */
typedef TBool (*I2CSlaveIsLastReceiveByte)(void *aCallbackWorkspace);
typedef void (*I2CSlaveReceiveByte)(void *aCallbackWorkspace, TUInt8 aByte);
typedef TBool (*I2CSlaveIsLastTransmitByte)(void *aCallbackWorkspace);
typedef TUInt8 (*I2CSlaveGetTransmitByte)(void *aCallbackWorkspace);
typedef void (*I2CTransferComplete)(void *aCallbackWorkspace);


typedef struct {
    I2CSlaveIsLastReceiveByte iIsLastReceiveByteCallback;
    I2CSlaveReceiveByte iReceiveByteCallback;
    I2CSlaveIsLastTransmitByte iIsLastTransmitByteCallback;
    I2CSlaveGetTransmitByte iGetTransmitByteCallback;
    I2CTransferComplete iTransferCompleteCallback;
} T_I2CSlaveCallbacks;

typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
/**
 * uEZ v1.00
 */
    void (*RequestRead)(
        void *aWorkspace,
        I2C_Request *iRequest,
        void *aCallbackWorkspace,
        I2CRequestCompleteCallback aCallbackFunc);
    void (*RequestWrite)(
        void *aWorkspace,
        I2C_Request *iRequest,
        void *aCallbackWorkspace,
        I2CRequestCompleteCallback aCallbackFunc);

/**
 * uEZ v1.10
 */
    T_uezError (*ConfigureSlave)(
        void *aWorkspace,
        TUInt8 aAddressLower7Bits,
        void *aCallbackWorkspace,
        const T_I2CSlaveCallbacks *aCallbacks);
    T_uezError (*Enable)(void *aWorkspace);
    T_uezError (*Disable)(void *aWorkspace);
} HAL_I2CBus;

#endif // _HAL_I2CBUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  I2CBUS.h
 *-------------------------------------------------------------------------*/
