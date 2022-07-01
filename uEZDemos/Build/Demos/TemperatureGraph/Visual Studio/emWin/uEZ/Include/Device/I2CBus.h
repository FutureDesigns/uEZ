/*-------------------------------------------------------------------------*
 * File:  I2CBus.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ I2C Bus Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_I2C_BUS_H_
#define _DEVICE_I2C_BUS_H_

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

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*T_I2CSlaveCallbackFunc)(
    void *aCallbackWorkspace,
    T_I2CSlaveEvent aStatus);

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
} DEVICE_I2C_BUS;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_I2C_BUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  I2CBus.h
 *-------------------------------------------------------------------------*/

