/*-------------------------------------------------------------------------*
 * File:  LightSensor.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ LightSensor Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_LightSensor_H_
#define _DEVICE_LightSensor_H_

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
 *  @file   /Include/Device/LightSensor.h
 *  @brief  uEZ Light Sensor Device Interface
 *
 *  The uEZ Light Sensor Device Interface
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
    T_uezError (*Open)(void *aWorkspace, char* aDevice);
    T_uezError (*Close)(void *aWorkspace);
    TUInt32 (*GetLevel)(void *aWorkspace);
    T_uezError (*SetBacklightLevel) (void *aWorkSpace, TUInt32 aNewLevel, TUInt8 aSetting);
    TUInt32 (*GetBacklightLevel) (void *aWorkSpace, TUInt8 aSetting);
} DEVICE_LightSensor;

#endif // _DEVICE_LightSensor_H_
/*-------------------------------------------------------------------------*
 * End of File:  LightSensor.h
 *-------------------------------------------------------------------------*/

