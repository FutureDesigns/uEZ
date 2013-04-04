/*-------------------------------------------------------------------------*
 * File:  Temperature.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Temperature Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_Temperature_H_
#define _DEVICE_Temperature_H_

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
 *  @file   /Include/Device/Temperature.h
 *  @brief  uEZ Temperature Device Interface
 *
 *  The uEZ Temperature Device Interface
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
   /**
	* Read the temperature in 15.16 signed fixed point format 
	* degrees Celsius.
	*/ 
    T_uezError (*Read)(
            void *aWorkspace,
            TInt32 *aTemperature);
} DEVICE_Temperature;

#endif // _DEVICE_Temperature_H_
/*-------------------------------------------------------------------------*
 * End of File:  Temperature.h
 *-------------------------------------------------------------------------*/

