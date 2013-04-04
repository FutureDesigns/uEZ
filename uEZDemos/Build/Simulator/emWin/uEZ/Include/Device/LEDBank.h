/*-------------------------------------------------------------------------*
 * File:  LEDBank.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ LEDBank Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_LEDBank_H_
#define _DEVICE_LEDBank_H_

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
 *  @file   /Include/Device/LEDBank.h
 *  @brief  uEZ LED Bank Device Interface
 *
 *  The uEZ LED Bank Device Interface
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

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*On)(
            void *aWorkspace,
            TUInt32 aLEDs);
    T_uezError (*Off)(
            void *aWorkspace,
            TUInt32 aLEDs);
    T_uezError (*Blink)(
            void *aWorkspace,
            TUInt32 aBlinkReg,
            TUInt32 aLEDs);
    T_uezError (*SetBlinkRegister)(
            void *aWorkspace,
            TUInt32 aBlinkReg,
            TUInt32 aPeriod,    /**< in ms */
            TUInt8 aDutyCycle);
} DEVICE_LEDBank;

#endif // _DEVICE_LEDBank_H_
/*-------------------------------------------------------------------------*
 * End of File:  LEDBank.h
 *-------------------------------------------------------------------------*/

