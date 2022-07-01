/*-------------------------------------------------------------------------*
 * File:  I2S.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ I2S Bus Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_I2S_BUS_H_
#define _DEVICE_I2S_BUS_H_

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
 *  @file   /Include/Device/I2S.h
 *  @brief  uEZ I2S Bus Device Interface
 *
 *  The uEZ I2S Bus Device Interface
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
#include <HAL/I2SBus.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
	// Header
    T_halInterface iInterface;

    //Functions
    T_uezError (*Configure)(
                void *aWorkspace,
                HAL_I2S_Callback_Transmit_Low iTransmitLow );
    T_uezError (*Start) ( void *aWorkspace);
    T_uezError (*Stop) ( void *aWorkspace);
} DEVICE_I2S_BUS;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_I2C_BUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  I2CBus.h
 *-------------------------------------------------------------------------*/
