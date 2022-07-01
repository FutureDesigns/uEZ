/*-------------------------------------------------------------------------*
 * File:  Accelerometer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Accelerometer Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_Accelerometer_H_
#define _DEVICE_Accelerometer_H_

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
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
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

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*GetInfo)(void *aWorkspace, AccelerometerInfo *aInfo);
    T_uezError (*ReadXYZ)(void *aWorkspace, 
            AccelerometerReading *aReading, TUInt32 aTimeout);
	
	// uEZ 2.10
    T_uezError (*ReadXYZSingle)(void *aWorkspace, 
            AccelerometerReading *aReading, TUInt32 aTimeout);
    T_uezError (*ReadXYZFloat)(void *aWorkspace, 
            AccelerometerReadingFloat *aReading, TUInt32 aTimeout);
    T_uezError (*ReadXYZFloatSingle)(void *aWorkspace, 
            AccelerometerReadingFloat *aReading, TUInt32 aTimeout);
} DEVICE_Accelerometer;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_Accelerometer_H_
/*-------------------------------------------------------------------------*
 * End of File:  Accelerometer.h
 *-------------------------------------------------------------------------*/

