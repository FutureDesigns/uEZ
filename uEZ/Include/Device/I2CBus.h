/*-------------------------------------------------------------------------*
 * File:  I2CBus.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ I2C Bus Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_I2C_BUS_H_
#define _DEVICE_I2C_BUS_H_

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
 *  @file   /Include/Device/I2CBus.h
 *  @brief  uEZ I2C Bus Device Interface
 *
 *  The uEZ I2C Bus Device Interface
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
#include <HAL/I2CBus.h>
#include <Types/I2C.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*T_I2CSlaveCallbackFunc)(
    void *aCallbackWorkspace,
    T_I2CSlaveEvent aStatus);

typedef T_uezError (*T_I2CResetCallbackFunc)(void);

typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*ProcessRequest)(void *aWorkspace, I2C_Request *aRequest);


/**
 * v1.10 Functions for Slave
 */
    T_uezError (*SlaveStart)(
        void *aWorkspace,
        const T_I2CSlaveSetup *aSetup);
    T_uezError (*SlaveWaitForEvent)(
        void *aWorkspace,
        T_I2CSlaveEvent *aEvent,
        TUInt32 *aLength,
        TUInt32 aTimeout);
    T_uezError (*Enable)(void *aWorkspace);
    T_uezError (*Disable)(void *aWorkspace);
/**
 *  uEZ v2.05 *
 */
    T_uezError (*IsHung)(void *aWorkspace, TBool *aBool);
    T_uezError (*ResetBus)(void *aWorkspace);
} DEVICE_I2C_BUS;

#endif // _DEVICE_I2C_BUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  I2CBus.h
 *-------------------------------------------------------------------------*/

