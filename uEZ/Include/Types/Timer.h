/*-------------------------------------------------------------------------*
 * File:  Timer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef _TYPES_TIMER_H_
#define _TYPES_TIMER_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

#endif // _TYPES_TIMER_H_
/*-------------------------------------------------------------------------*
 * End of File:  Timer.h
 *-------------------------------------------------------------------------*/
