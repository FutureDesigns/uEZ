/*-------------------------------------------------------------------------*
 * File:  uTickCounter.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ base 1 ms tick counter
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_TICK_COUNTER_H_
#define _UEZ_TICK_COUNTER_H_

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
 *  @file   uEZTickCounter.h
 *  @brief  uEZ Tick Counter
 *
 *  uEZ base 1 ms tick counter
 */
#include "uEZTypes.h"

#define UEZ_TIMEOUT_NONE         0
#define UEZ_TIMEOUT_INFINITE     0xFFFFFFFF

TUInt32 UEZTickCounterGet(void);
TUInt32 UEZTickCounterGetDelta(TUInt32 aStart);

#endif // _UEZ_TICK_COUNTER_H_

/*-------------------------------------------------------------------------*
 * End of File:  uTickCounter.h
 *-------------------------------------------------------------------------*/
