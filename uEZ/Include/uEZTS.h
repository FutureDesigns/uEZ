/**
 *	@file 	uEZTS.h
 *  @brief 	uEZ Touchscreen Interface
 *
 *	The uEZ interface which maps to low level Touchscreen drivers.
 */
#ifndef _UEZ_TS_H_
#define _UEZ_TS_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
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

#include "uEZTypes.h"
#include "uEZErrors.h"
#include <uEZRTOS.h>

#define UEZTS_NO_PRESSURE    0

/**
 *	@typedef T_uezTSFlags
 */
typedef TUInt32 T_uezTSFlags;
#define TSFLAG_PEN_DOWN     (1<<0)

/**
 *	@struct T_uezTSReading
 */
typedef struct {
    T_uezTSFlags iFlags;
    TInt32 iX;
    TInt32 iY;
    TUInt32 iPressure;
} T_uezTSReading;

/**
 *	@struct T_uezTSEvent
 */
typedef struct {
    T_uezDevice iSource;
    T_uezTSReading iReading;
} T_uezTSEvent;


/**
 *	Open a touchscreen device and register queue (if given)
 *
 *	@param [in] 	*aName					Name of touchscreen device
 *	@param [out]	*aTSDevice				Opened device (if opened)
 *	@param [out]	*aEventQueue		Pointer to queue to receive
 *                                touchscreen events.
 *
 *	@return		T_uezError
 */
T_uezError UEZTSOpen(
            const char * const aName,
            T_uezDevice *aTSDevice,
            T_uezQueue *aEventQueue);
						
/**
 *	End access to the Flash bank.
 *
 *	@param [in]		aTSDevice					Touchscreen device to close
 *	@param [in]		aEventQueue			Queue being used with touchscreen
 *
 *	@return		T_uezError
 */
T_uezError UEZTSClose(
            T_uezDevice aTSDevice,
            T_uezQueue aEventQueue);

/**
 *	Forces the touch screen to poll for an immediate reading.
 *
 *	@param [in]			aTSDevice				Touchscreen device to read
 *	@param [out]		*aReading			Pointer to reading structure to use
 *
 *	@return		T_uezError
 */
T_uezError UEZTSGetReading(
            T_uezDevice aTSDevice,
            T_uezTSReading *aReading);

/**
 *	Put the touchscreen in calibration mode.  Readings will be taken
 *  and added to the calibration system.
 *
 *	@param [in]			aTSDevice				Touchscreen device
 *
 *	@return		T_uezError
 */
T_uezError UEZTSCalibrationStart(T_uezDevice aTSDevice);

/**
 *	Finishes the calibration and calculates the final setting.
 *
 *	@param [in]		aTSDevice							Touchscreen device
 *	@param [out]	*aReadingTaken			Reading taken using UEZTSGetReading.
 *  @param [in]		*aReadingExpected		Reading that was expected (ideal).
 *
 *	@return		T_uezError
 */
T_uezError UEZTSCalibrationAddReading(
            T_uezDevice aTSDevice,
            const T_uezTSReading *aReadingTaken,
            const T_uezTSReading *aReadingExpected);
						
/**
 *	Finishes the calibration and calculates the final setting.
 *
 *	@param [in]			aTSDevice				Touchscreen device
 *
 *	@return		T_uezError
 */
T_uezError UEZTSCalibrationEnd(T_uezDevice aTSDevice);

/**
 *	Finishes the calibration and calculates the final setting.
 *
 *	@param [in]		aTSDevice					Touchscreen device
 *	@param [out]	aEventQueue			Queue to receive touchscreen events
 *
 *	@return		T_uezError
 */
T_uezError UEZTSAddQueue(T_uezDevice aTSDevice, T_uezQueue aEventQueue);

/**
 *	Remove queue receiving touch screen events for associate device.
 *
 *	@param [in]		aTSDevice					Touchscreen device
 *	@param [out]	aEventQueue			Queue receiving touchscreen events
 *
 *	@return		T_uezError
 */
T_uezError UEZTSRemoveQueue(T_uezDevice aTSDevice, T_uezQueue aEventQueue);

/**
 *	Change the low (press) and high (release) points of the touch
 *  Detection code.  These points are hardware specific, but the values
 *  passed are 0-0xFFFF as a scale between 0 V and Ref Voltage.  Notice
 *  that putting a gap between high and low provides a range where
 *  the press/release is reported as the previous press/release until
 *  the threshold is met.
 *
 *	@param [in]		aTSDevice				Touchscreen device
 *	@param [in]		aLowLevel			Low (press) reference level (0-0xFFFF)
 *	@param [in]		aHighLevel		High (release) reference level (0-0xFFFF)
 *
 *	@return		T_uezError
 */
T_uezError UEZTSSetTouchDetectSensitivity(
                T_uezDevice aTSDevice,
                TUInt16 aLowLevel,
                TUInt16 aHighLevel);

#endif // _UEZ_TS_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZTS.h
 *-------------------------------------------------------------------------*/
