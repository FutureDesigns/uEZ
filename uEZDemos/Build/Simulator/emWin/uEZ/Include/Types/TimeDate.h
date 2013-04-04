/*-------------------------------------------------------------------------*
 * File:  Types/TimeDate.h
 *-------------------------------------------------------------------------*
 * Description:
 *     TimeDate HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _TimeDate_TYPES_H_
#define _TimeDate_TYPES_H_

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
 *  @file   /Include/Types/TimeDate.h
 *  @brief  uEZ Time Date Types
 *
 *  The uEZ Time Date Types
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

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        TUInt8 iHour; 		/** 0 - 23 */
        TUInt8 iMinute; 	/** 0 - 59 */
        TUInt8 iSecond; 	/** 0 - 59 */
        TUInt8 iReserved; 	/** Always 0 */
} T_uezTime;

typedef struct {
        TUInt8 iMonth; 		/** 1 - 12 */
        TUInt8 iDay; 		/** 1 - 31 */
        TUInt16 iYear; 		/** 0 - 9999 */
} T_uezDate;

typedef struct {
        T_uezTime iTime;
        T_uezDate iDate;
} T_uezTimeDate;

typedef struct {
        TUInt16 iDays; 		/** Maximum of 65535 days or 179 years */
        TUInt8 iHours; 		/** 0 - 23 */
        TUInt8 iMinutes; 	/** 0 - 59 */
        TUInt8 iSeconds; 	/** 0 - 59 */
} T_uezTimeDuration;

#endif // _TimeDate_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/TimeDate.h
 *-------------------------------------------------------------------------*/
