/*-------------------------------------------------------------------------*
 * File:  I2SBUS.h
 *-------------------------------------------------------------------------*
 * Description:
 *     I2CBUS HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_I2SBUS_H_
#define _HAL_I2SBUS_H_

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
 *  @file   /Include/HAL/I2SBUS.h
 *  @brief  uEZ I2SBUS HAL Interface
 *
 *  The uEZ I2SBUS HAL Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Include/uEZTypes.h>
#include <Include/HAL/HAL.h>
//#include <Types/I2S.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*HAL_I2S_Callback_Transmit_Low)(
                void *aCallbackWorkspace,
                TInt32* samples,
                TInt8 numSamples);
//              TInt16* samples,
//              TUInt16* numSamples);

typedef struct {
// Header
    T_halInterface iInterface;

    //Functions
    T_uezError (*Configure)(
                void *aWorkspace,
                HAL_I2S_Callback_Transmit_Low );
    T_uezError (*Start) ( void *aWorkspace);
    T_uezError (*Stop) ( void *aWorkspace);
}HAL_I2S;

#ifdef __cplusplus
}
#endif

#endif // _HAL_I2SBUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  I2SBUS.h
 *-------------------------------------------------------------------------*/
