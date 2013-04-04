/*-------------------------------------------------------------------------*
 * File:  uEZAccelerometer.h
 *-------------------------------------------------------------------------*/
/**
 *  @file   uEZAccelerometer.h
 *  @brief  uEZ accelerometer Interface
 *
 *  The uEZ interface to the accelerometer
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#ifndef UEZACCELEROMETER_H_
#define UEZACCELEROMETER_H_

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/Accelerometer.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/**
 *  Start access to the accelerometer
 *
 *  @param [in]     *aName          Pointer to name of accelerometer (usually "Accel0")
 *  @param [out]    *aDevice        Pointer to device handle to be returned
 *
 *  @return T_uezError
 */
T_uezError UEZAccelerometerOpen(const char * const aName, T_uezDevice *aDevice);

/*!
 *  End access to the accelerometer.
 *
 *  @param [in] aDevice     Device handle of accelerometer
 *
 *  @return T_uezError
 */
T_uezError UEZAccelerometerClose(T_uezDevice aDevice);

/*!
 *  Get information about the accelerometer.
 *
 *  @param [in] aDevice     Device handle of accelerometer
 *  @param [out] *aInfo     Structure to hold accelerometer information
 *
 *  @return T_uezError
 */
T_uezError UEZAccelerometerGetInfo(
        T_uezDevice aDevice,
        AccelerometerInfo *aInfo);

/*!
 *  Get the current XYZ status of the accelerometer
 *
 *  @param [in] aDevice     Device handle of accelerometer
 *  @param [out] *aReading  Readings structure to fill with return values
 *  @param [in] aTimeout    Maximum number of milliseconds to wait for data
 *
 *  @return T_uezError
 */

T_uezError UEZAccelerometerReadXYZ(
        T_uezDevice aDevice,
        AccelerometerReading *aReading,
        TUInt32 aTimeout);

#endif // UEZACCELEROMETER_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZAccelerometer.h
 *-------------------------------------------------------------------------*/
