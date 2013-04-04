/*-------------------------------------------------------------------------*
 * File:  Watchdog.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Watchdog HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_Watchdog_H_
#define _HAL_Watchdog_H_

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
 *  @file   /Include/HAL/Watchdog.h
 *  @brief  uEZ Watchdog HAL Interface
 *
 *  The uEZ Watchdog HAL Interface
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
#include <HAL/HAL.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*SetMaximumTime)(
        void *aWorkspace, 
        TUInt32 aMaxTime /* in ms */);
    T_uezError (*SetMinimumTime)(
        void *aWorkspace, 
        TUInt32 aMinTime /* in ms */);
    TBool (*IsResetFromWatchdog)(void *aWorkspace);
    void (*ClearResetFlag)(void *aWorkspace);
    TBool (*IsActive)(void *aWorkspace);
    T_uezError (*Start)(void *aWorkspace);
    T_uezError (*Feed)(void *aWorkspace);
    T_uezError (*Trip)(void *aWorkspace);
} HAL_Watchdog;

#endif // _HAL_Watchdog_H_
/*-------------------------------------------------------------------------*
 * End of File:  Watchdog.h
 *-------------------------------------------------------------------------*/
