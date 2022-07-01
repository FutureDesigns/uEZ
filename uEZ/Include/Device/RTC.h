/*-------------------------------------------------------------------------*
 * File:  Device/RTC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Real Time Clock (RTC) Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_RTC_H_
#define _DEVICE_RTC_H_

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
    T_uezError (*Get)(void *aWorkspace, T_uezTimeDate *aTimeDate);
    T_uezError (*Set)(void *aWorkspace, const T_uezTimeDate *aTimeDate);
    T_uezError (*Validate)(void *aWorkspace, const T_uezTimeDate *aTimeDate);
    T_uezError (*SetClockOutHz)(void *aWorkspace, TUInt32 aHertz);
} DEVICE_RTC;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_RTC_H_
/*-------------------------------------------------------------------------*
 * End of File:  Device/RTC.h
 *-------------------------------------------------------------------------*/

