/*-------------------------------------------------------------------------*
 * File:  Timer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef _TYPES_TIMER_H_
#define _TYPES_TIMER_H_

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
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
/**
 *  @file   /Include/Types/Timer.h
 *  @brief  uEZ Timer Types
 *
 *  The uEZ Timer Types
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    TIMER_EXT_CONTROL_DO_NOTHING=0,
    TIMER_EXT_CONTROL_CLEAR=1,
    TIMER_EXT_CONTROL_SET=2,
    TIMER_EXT_CONTROL_TOGGLE=3,
} T_Timer_ExternalControl;

typedef enum {
    TIMER_MODE_CLOCK=0,
    TIMER_MODE_RISING_CAP=1,
    TIMER_MODE_FALLING_CAP=2,
    TIMER_MODE_RISING_OR_FALLING_CAP=3
} T_Timer_Mode;

typedef TUInt16 T_Timer_MatchPointFunction;
#define TIMER_MATCH_POINT_FUNC_RESET            0x0001
#define TIMER_MATCH_POINT_FUNC_INTERRUPT        0x0002
#define TIMER_MATCH_POINT_FUNC_STOP             0x0004

#ifdef __cplusplus
}
#endif

#endif // _TYPES_TIMER_H_
/*-------------------------------------------------------------------------*
 * End of File:  Timer.h
 *-------------------------------------------------------------------------*/
