/*-------------------------------------------------------------------------*
 * File:  Touchscreen.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Touchscreen device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_TOUCHSCREEN_H_
#define _DEVICE_TOUCHSCREEN_H_

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
 *  @file   /Include/Device/Touchscreen.h
 *  @brief  uEZ Touchscreen Device Interface
 *
 *  The uEZ Touchscreen Device Interface
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
#include <uEZTS.h>
#include <uEZDevice.h>
#include <Device/SPIBus.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iResolutionX;
    TUInt32 iResolutionY;
} T_deviceConfigTouchscreen;

typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Open)(void *aWorkspace);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*Poll)(void *aWorkspace, T_uezTSReading *aReading);

    T_uezError (*CalibrationStart)(void *aWorkspace);
    T_uezError (*CalibrationAddReading)(
                    void *aWorkspace, 
                    const T_uezTSReading *aReadingTaken,
                    const T_uezTSReading *aReadingExpected);
    T_uezError (*CalibrationEnd)(void *aWorkspace);
    T_uezError (*SetTouchDetectSensitivity)(
                    void *aWorkspace,
                    TUInt16 aLowLevel,
                    TUInt16 aHighLevel);

    T_uezError (*WaitForTouch)(void *aWorkspace, TUInt32 aTimeout);
} DEVICE_TOUCHSCREEN ;

#endif // _DEVICE_TOUCHSCREEN_H_
/*-------------------------------------------------------------------------*
 * End of File:  Touchscreen.h
 *-------------------------------------------------------------------------*/
