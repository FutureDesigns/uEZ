/*-------------------------------------------------------------------------*
* File:  time.h
*--------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
*-------------------------------------------------------------------------*/
#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

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
 *  @file   /Include/sys/time.h
 *  @brief  uEZ system time
 *
 *  TODO: Write Explanation here
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
 
struct timeval {
    int32_t tv_sec;
    int32_t tv_usec;
};

struct timezone {
    int32_t	tz_minuteswest;		/**< minutes west of Greenwich */
    int32_t	tz_dsttime;			/**< type of dst correction */
};

int32_t gettimeofday(struct timeval *tv, struct timezone *tz);

/**
 * Structure returned by gettimeofday(2) system call,
 * and used in other calls.
 */

#define	DST_NONE	0		/**< not on dst */
#define	DST_USA		1		/**< USA style dst */
#define	DST_AUST	2		/**< Australian style dst */
#define	DST_WET		3		/**< Western European dst */
#define	DST_MET		4		/**< Middle European dst */
#define	DST_EET		5		/**< Eastern European dst */
#define	DST_CAN		6		/**< Canada */
#define	DST_GB		7		/**< Great Britain and Eire */
#define	DST_RUM		8		/**< Rumania */
#define	DST_TUR		9		/**< Turkey */
#define	DST_AUSTALT	10		/**< Australian style with shift in 1986 */

#endif // _SYS_TIME_H_
/*-------------------------------------------------------------------------*
 * End of File:  time.h
 *-------------------------------------------------------------------------*/
