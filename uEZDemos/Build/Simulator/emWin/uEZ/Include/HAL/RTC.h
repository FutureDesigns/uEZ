/*-------------------------------------------------------------------------*
 * File:  RTC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     RTC HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _HAL_RTC_H_
#define _HAL_RTC_H_

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
 *  @file   /Include/HAL/RTC.h
 *  @brief  uEZ RTC HAL Interface
 *
 *  The uEZ RTC HAL Interface
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
#include <Types/TimeDate.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*Get)(void *aWorkspace, T_uezTimeDate *aTimeDate);
    T_uezError (*Set)(void *aWorkspace, const T_uezTimeDate *aTimeDate);
/**
 * uEZ v1.06
 */
    T_uezError (*Validate)(void *aWorkspace, const T_uezTimeDate *aTimeDate);
} HAL_RTC ;

#endif // _HAL_RTC_H_
/*-------------------------------------------------------------------------*
 * End of File:  HAL/RTC.h
 *-------------------------------------------------------------------------*/
