/*-------------------------------------------------------------------------*
* File:  uEZTS.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to low level Touchscreen drivers.
*-------------------------------------------------------------------------*/
/**
 *    @file     uEZTS.h
 *  @brief     uEZ Touchscreen Interface
 *
 *    The uEZ interface which maps to low level Touchscreen drivers.
 */
#ifndef _UEZ_TS_H_
#define _UEZ_TS_H_

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
#include "uEZTypes.h"
#include "uEZErrors.h"
#include <uEZRTOS.h>

#ifdef __cplusplus
extern "C" {
#endif

#define UEZTS_NO_PRESSURE    0

/**
 *    @typedef T_uezTSFlags
 */
typedef TUInt32 T_uezTSFlags;
#define TSFLAG_PEN_DOWN     (1<<0)

/**
 *    @struct T_uezTSReading
 */
typedef struct {
    T_uezTSFlags iFlags;
    TInt32 iX;
    TInt32 iY;
    TUInt32 iPressure;
} T_uezTSReading;

/**
 *    @struct T_uezTSEvent
 */
typedef struct {
    T_uezDevice iSource;
    T_uezTSReading iReading;
} T_uezTSEvent;


/**
 *    Open a touchscreen device and register queue (if given)
 *
 *    @param [in]     *aName                    Name of touchscreen device
 *    @param [out]    *aTSDevice                Opened device (if opened)
 *    @param [out]    *aEventQueue        Pointer to queue to receive
 *                                touchscreen events.
 *
 *    @return        T_uezError
 */
T_uezError UEZTSOpen(
            const char * const aName,
            T_uezDevice *aTSDevice,
            T_uezQueue *aEventQueue);
                        
/**
 *    End access to the Flash bank.
 *
 *    @param [in]        aTSDevice                    Touchscreen device to close
 *    @param [in]        aEventQueue            Queue being used with touchscreen
 *
 *    @return        T_uezError
 */
T_uezError UEZTSClose(
            T_uezDevice aTSDevice,
            T_uezQueue aEventQueue);

/**
 *    Forces the touch screen to poll for an immediate reading.
 *
 *    @param [in]            aTSDevice                Touchscreen device to read
 *    @param [out]        *aReading            Pointer to reading structure to use
 *
 *    @return        T_uezError
 */
T_uezError UEZTSGetReading(
            T_uezDevice aTSDevice,
            T_uezTSReading *aReading);

/**
 *    Put the touchscreen in calibration mode.  Readings will be taken
 *  and added to the calibration system.
 *
 *    @param [in]            aTSDevice                Touchscreen device
 *
 *    @return        T_uezError
 */
T_uezError UEZTSCalibrationStart(T_uezDevice aTSDevice);

/**
 *    Finishes the calibration and calculates the final setting.
 *
 *    @param [in]        aTSDevice                            Touchscreen device
 *    @param [out]    *aReadingTaken            Reading taken using UEZTSGetReading.
 *  @param [in]        *aReadingExpected        Reading that was expected (ideal).
 *
 *    @return        T_uezError
 */
T_uezError UEZTSCalibrationAddReading(
            T_uezDevice aTSDevice,
            const T_uezTSReading *aReadingTaken,
            const T_uezTSReading *aReadingExpected);
                        
/**
 *    Finishes the calibration and calculates the final setting.
 *
 *    @param [in]            aTSDevice                Touchscreen device
 *
 *    @return        T_uezError
 */
T_uezError UEZTSCalibrationEnd(T_uezDevice aTSDevice);

/**
 *    Finishes the calibration and calculates the final setting.
 *
 *    @param [in]        aTSDevice                    Touchscreen device
 *    @param [out]    aEventQueue            Queue to receive touchscreen events
 *
 *    @return        T_uezError
 */
T_uezError UEZTSAddQueue(T_uezDevice aTSDevice, T_uezQueue aEventQueue);

/**
 *    Remove queue receiving touch screen events for associate device.
 *
 *    @param [in]        aTSDevice                    Touchscreen device
 *    @param [out]    aEventQueue            Queue receiving touchscreen events
 *
 *    @return        T_uezError
 */
T_uezError UEZTSRemoveQueue(T_uezDevice aTSDevice, T_uezQueue aEventQueue);

/**
 *    Change the low (press) and high (release) points of the touch
 *  Detection code.  These points are hardware specific, but the values
 *  passed are 0-0xFFFF as a scale between 0 V and Ref Voltage.  Notice
 *  that putting a gap between high and low provides a range where
 *  the press/release is reported as the previous press/release until
 *  the threshold is met.
 *
 *    @param [in]        aTSDevice                Touchscreen device
 *    @param [in]        aLowLevel            Low (press) reference level (0-0xFFFF)
 *    @param [in]        aHighLevel        High (release) reference level (0-0xFFFF)
 *
 *    @return        T_uezError
 */
T_uezError UEZTSSetTouchDetectSensitivity(
                T_uezDevice aTSDevice,
                TUInt16 aLowLevel,
                TUInt16 aHighLevel);

#ifdef __cplusplus
}
#endif

#endif // _UEZ_TS_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZTS.h
 *-------------------------------------------------------------------------*/
