/*-------------------------------------------------------------------------*
 * File:  I2S.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ I2S Bus Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_I2S_BUS_H_
#define _DEVICE_I2S_BUS_H_

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

#endif // _DEVICE_I2C_BUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  I2CBus.h
 *-------------------------------------------------------------------------*/
