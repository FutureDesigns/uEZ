/*-------------------------------------------------------------------------*
 * File:  I2CBUS.h
 *-------------------------------------------------------------------------*
 * Description:
 *     I2CBUS HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_I2CBUS_H_
#define _HAL_I2CBUS_H_

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

#ifdef __cplusplus
extern "C" {
#endif

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
/**
 *  uEZ v2.05 *
 */
    T_uezError (*IsHung)(void *aWorkspace, TBool *aBool);
    T_uezError (*ResetBus)(void *aWorkspace);
} HAL_I2CBus;

#ifdef __cplusplus
}
#endif

#endif // _HAL_I2CBUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  I2CBUS.h
 *-------------------------------------------------------------------------*/
