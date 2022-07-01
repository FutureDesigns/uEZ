/*-------------------------------------------------------------------------*
 * File:  uEZAccelerometer.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Easy access routines to the accelerometer.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
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
/**
 *  @addtogroup uEZAccelerometer
 *  @{
 *  @brief     uEZ Accelerometer Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ Accelerometer interface.
 *
 *  @par Example code:
 *  Example task to read the accelerometer
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAccelerometer.h>
 *
 *  TUInt32 AccelerometerRead(T_uezTask aMyTask, void *aParams)
 *  {
 *       T_uezDevice Accel;
 *       double x, y, z;
 *       AccelerometerReading ar;
 *       if (UEZAccelerometerOpen("Accel0", &Accel) == UEZ_ERROR_NONE) {
 *           // the device opened properly
 *
 *           x = y = z = 0;
 *           for (i = 0; i < 8; i++) {
 *               if (UEZAccelerometerReadXYZ(Accel, &ar, 10)
 *                       != UEZ_ERROR_NONE)
 *                   UEZFailureMsg("Accelerometer failed to read!");
 *               x += ar.iX;
 *               y += ar.iY;
 *               z += ar.iZ;
 *           }
 *           // Convert from signed 15.16 fixed format to doubles over 8 samples
 *           // The value is not in g's.  For example, the x, y, z is
 *           // (0.0, 0.0, -1.0) when at rest on a table.
 *           x /= 65536.0 * 8;
 *           y /= 65536.0 * 8;
 *           z /= 65536.0 * 8;
 *
 *           if (UEZAccelerometerClose(Accel) != UEZ_ERROR_NONE) {
 *               // error closing accelerometer
 *           }
 *       } else {
 *           // an error occurred opening the accelerometer
 *       }
 *       return 0;
 *   }
 * @endcode
 */
#include <uEZ.h>
#include <Device/Accelerometer.h>
#include "uEZDevice.h"
#include <uEZDeviceTable.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine: UEZAccelerometerOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open access to the accelerometer
 *
 *  @param [in]    *aName 		Name of accelerometer
 *
 *  @param [in]    *aDevice 	Device handle to be returned
 *
 *  @return        T_uezError 	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAccelerometer.h>
 *
 *  T_uezDevice Accel;
 *  if (UEZAccelerometerOpen("Accel0", &Accel) == UEZ_ERROR_NONE) {
 *      // the device opened properly
 *
 *  } else {
 *      // an error occurred opening the accelerometer
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAccelerometerOpen(const char * const aName, T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_Accelerometer **p;

    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Nothing special done here
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: UEZAccelerometerClose
 *---------------------------------------------------------------------------*/
/**
 *  Close access to the previously opened accelerometer
 *
 *  @param [in]    *aDevice 	Device handle to be returned
 *
 *  @return        T_uezError 	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAccelerometer.h>
 *
 *   T_uezDevice Accel;
 *   if (UEZAccelerometerOpen("Accel0", &Accel) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       if (UEZAccelerometerClose(Accel) != UEZ_ERROR_NONE) {
 *           // error closing accelerometer
 *       }
 *   } else {
 *       // an error occurred opening the accelerometer
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAccelerometerClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_Accelerometer **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Nothing special done here
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: UEZAccelerometerGetInfo
 *---------------------------------------------------------------------------*/
/**
 *  Get information about the accelerometer.
 *
 *  @param [in]    *aDevice 	Device handle to be returned
 *
 *  @param [in]    *aInfo 		Pointer to info structure
 *
 *  @return        T_uezError 	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAccelerometer.h>
 *
 *   T_uezDevice Accel;
 *   AccelerometerInfo *aInfo
 *   TUInt32 chipID;
 *   TUInt32 revision;
 *   if (UEZAccelerometerOpen("Accel0", &Accel) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       UEZAccelerometerGetInfo(Accel,&aInfo);
 *       chipID = aInfo.iChipID;
 *       revision = aInfo.iRevision;
 *
 *   } else {
 *       // an error occurred opening the accelerometer
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAccelerometerGetInfo(
        T_uezDevice aDevice,
        AccelerometerInfo *aInfo)
{
    T_uezError error;
    DEVICE_Accelerometer **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetInfo(p, aInfo);
}

/*---------------------------------------------------------------------------*
 * Routine: UEZAccelerometerReadXYZ
 *---------------------------------------------------------------------------*/
/**
 *  Read the XYZ values of the accelerometer once. The values are in
 *      fixed signed 15.16 integers.
 *
 *  @param [in]    *aDevice 	Device handle to be returned
 *
 *  @param [in]    *aReading 	Returned reading
 *
 *  @param [in]    aTimeout 	Maximum time spent looking for reading before
 *                          	timeout.
 *  @return        T_uezError	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAccelerometer.h>
 *
 *   T_uezDevice Accel;
 *   double x, y, z;
 *   AccelerometerReading ar;
 *   if (UEZAccelerometerOpen("Accel0", &Accel) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       x = y = z = 0;
 *       for (i = 0; i < 8; i++) {
 *           if (UEZAccelerometerReadXYZ(Accel, &ar, 10)
 *                   != UEZ_ERROR_NONE)
 *               UEZFailureMsg("Accelerometer failed to read!");
 *           x += ar.iX;
 *           y += ar.iY;
 *           z += ar.iZ;
 *       }
 *       // Convert from signed 15.16 fixed format to doubles over 8 samples
 *       // The value is not in g's.  For example, the x, y, z is
 *       // (0.0, 0.0, -1.0) when at rest on a table.
 *       x /= 65536.0 * 8;
 *       y /= 65536.0 * 8;
 *       z /= 65536.0 * 8;
 *
 *       if (UEZAccelerometerClose(Accel) != UEZ_ERROR_NONE) {
 *           // error closing accelerometer
 *       }
 *   } else {
 *       // an error occurred opening the accelerometer
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAccelerometerReadXYZ(
        T_uezDevice aDevice,
        AccelerometerReading *aReading,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_Accelerometer **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->ReadXYZ(p, aReading, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine: UEZAccelerometerReadXYZFloat
 *---------------------------------------------------------------------------*/
/**
 *  Read the XYZ values of the accelerometer once as float.
 *
 *  @param [in]    *aDevice 	Device handle to be returned
 *
 *  @param [in]    *aReading 	Returned reading
 *
 *  @param [in]    aTimeout 	Maximum time spent looking for reading before
 *                          	timeout.
 *  @return        T_uezError	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAccelerometer.h>
 *
 *   T_uezDevice Accel;
 *   float x, y, z;
 *   AccelerometerReadingFloat ar;
 *   if (UEZAccelerometerOpen("Accel0", &Accel) == UEZ_ERROR_NONE) {
 *       // the device opened properly *
 *       x = y = z = 0;
 *       if (UEZAccelerometerReadXYZ(Accel, &ar, 10)
 *                 != UEZ_ERROR_NONE) {
 *           UEZFailureMsg("Accelerometer failed to read!"); 
 *       }
 *       if (UEZAccelerometerClose(Accel) != UEZ_ERROR_NONE) {
 *           // error closing accelerometer
 *       }
 *   } else {
 *       // an error occurred opening the accelerometer
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAccelerometerReadXYZFloat(
        T_uezDevice aDevice,
        AccelerometerReadingFloat *aReading,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_Accelerometer **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->ReadXYZFloat(p, aReading, aTimeout);
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZAccelerometer.c
 *-------------------------------------------------------------------------*/
