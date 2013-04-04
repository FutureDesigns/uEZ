/*-------------------------------------------------------------------------*
 * File:  I2SBUS.h
 *-------------------------------------------------------------------------*
 * Description:
 *     I2CBUS HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_I2SBUS_H_
#define _HAL_I2SBUS_H_

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
#include <uEZTypes.h>
#include <HAL/HAL.h>
//#include <Types/I2S.h>

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
#endif // _HAL_I2SBUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  I2SBUS.h
 *-------------------------------------------------------------------------*/
