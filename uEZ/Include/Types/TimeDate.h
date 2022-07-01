/*-------------------------------------------------------------------------*
 * File:  Types/TimeDate.h
 *-------------------------------------------------------------------------*
 * Description:
 *     TimeDate HAL interface
 *-------------------------------------------------------------------------*/
#ifndef _TimeDate_TYPES_H_
#define _TimeDate_TYPES_H_

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

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _TimeDate_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/TimeDate.h
 *-------------------------------------------------------------------------*/
