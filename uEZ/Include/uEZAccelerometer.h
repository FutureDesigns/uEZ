/*-------------------------------------------------------------------------*
* File:  uEZAccelerometer.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface to the accelerometer
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
#ifndef UEZACCELEROMETER_H_
#define UEZACCELEROMETER_H_

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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/Accelerometer.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // UEZACCELEROMETER_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZAccelerometer.h
 *-------------------------------------------------------------------------*/
