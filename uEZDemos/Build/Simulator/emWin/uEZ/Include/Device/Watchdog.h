/*-------------------------------------------------------------------------*
 * File:  DEVICE/Watchdog.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Watchdog Device interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_Watchdog_H_
#define _DEVICE_Watchdog_H_

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
 *  @file   /Include/Device/Watchdog.h
 *  @brief  uEZ Watchdog Device Interface
 *
 *  The uEZ Watchdog Device Interface
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
    T_uezDeviceInterface iInterface;

   /**
    * uEZ v1.07 Functions
    */
    T_uezError (*SetMaximumTime)(
        void *aWorkspace,
        TUInt32 aMaxTime); /**< in ms */
    T_uezError (*SetMinimumTime)(
        void *aWorkspace,
        TUInt32 aMinTime); /**< in ms */
    TBool (*IsResetFromWatchdog)(void *aWorkspace);
    void (*ClearResetFlag)(void *aWorkspace);
    TBool (*IsActive)(void *aWorkspace);
    T_uezError (*Start)(void *aWorkspace);
    T_uezError (*Feed)(void *aWorkspace);
    T_uezError (*Trip)(void *aWorkspace);
    T_uezError (*FeedAutomatically)(void *aWorkspace, TUInt32 aFeedInterval);
} DEVICE_Watchdog;

#endif /* _DEVICE_Watchdog_H_ */
