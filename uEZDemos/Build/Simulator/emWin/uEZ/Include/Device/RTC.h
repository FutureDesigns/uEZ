/*-------------------------------------------------------------------------*
 * File:  Device/RTC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Real Time Clock (RTC) Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_RTC_H_
#define _DEVICE_RTC_H_

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
 *  @file   /Include/Device/RTC.h
 *  @brief  uEZ RTC Device Interface
 *
 *  The uEZ RTC Device Interface
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
#include <Types/TimeDate.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Get)(void *aWorkspace, T_uezTimeDate *aTimeDate);
    T_uezError (*Set)(void *aWorkspace, const T_uezTimeDate *aTimeDate);
    T_uezError (*Validate)(void *aWorkspace, const T_uezTimeDate *aTimeDate);
    T_uezError (*SetClockOutHz)(void *aWorkspace, TUInt32 aHertz);
} DEVICE_RTC;

#endif // _DEVICE_RTC_H_
/*-------------------------------------------------------------------------*
 * End of File:  Device/RTC.h
 *-------------------------------------------------------------------------*/

