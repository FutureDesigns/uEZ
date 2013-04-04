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
#ifndef _SYS_TIME_H_
#define _SYS_TIME_H_

/**
 *  @file   /Include/sys_compat/time.h
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
    long int tv_sec;
    int tv_usec;
};

struct timezone {
    int	tz_minuteswest;		/**< minutes west of Greenwich */
    int	tz_dsttime;			/**< type of dst correction */
};

int gettimeofday(struct timeval *tv, struct timezone *tz);

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
