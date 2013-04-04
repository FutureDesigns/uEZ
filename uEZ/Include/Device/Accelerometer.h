/*-------------------------------------------------------------------------*
 * File:  Accelerometer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Accelerometer Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_Accelerometer_H_
#define _DEVICE_Accelerometer_H_

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
 *  @file   /Include/Device/Accelerometer.h
 *  @brief  uEZ Accelerometer Device Interface
 *
 *  The uEZ Accelerometer Device Interface
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
#include <Types/Accelerometer.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*GetInfo)(
            void *aWorkspace,
            AccelerometerInfo *aInfo);
    T_uezError (*ReadXYZ)(
            void *aWorkspace, 
            AccelerometerReading *aReading,
            TUInt32 aTimeout);
} DEVICE_Accelerometer;

#endif // _DEVICE_Accelerometer_H_
/*-------------------------------------------------------------------------*
 * End of File:  Accelerometer.h
 *-------------------------------------------------------------------------*/

