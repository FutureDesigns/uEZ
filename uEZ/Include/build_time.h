/*
 * buildTime.h
 *
 *  Created on: Aug 12, 2021
 *      Author: Bill Fleming
 */

#ifndef BUILDTIME_H_
#define BUILDTIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/*********************************************************************************************************************************/
/*  Build time specific macros                                                                                                   */
/*********************************************************************************************************************************/

// Example of __DATE__ string: "Jul 27 2012"
//                              01234567890
#define BUILD_YEAR_CH0 (__DATE__[ 7])
#define BUILD_YEAR_CH1 (__DATE__[ 8])
#define BUILD_YEAR_CH2 (__DATE__[ 9])
#define BUILD_YEAR_CH3 (__DATE__[10])

#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')

#define BUILD_MONTH_CH0 \
    ((BUILD_MONTH_IS_OCT || BUILD_MONTH_IS_NOV || BUILD_MONTH_IS_DEC) ? '1' : '0')

#define BUILD_MONTH_CH1 \
    ( \
        (BUILD_MONTH_IS_JAN) ? '1' : \
        (BUILD_MONTH_IS_FEB) ? '2' : \
        (BUILD_MONTH_IS_MAR) ? '3' : \
        (BUILD_MONTH_IS_APR) ? '4' : \
        (BUILD_MONTH_IS_MAY) ? '5' : \
        (BUILD_MONTH_IS_JUN) ? '6' : \
        (BUILD_MONTH_IS_JUL) ? '7' : \
        (BUILD_MONTH_IS_AUG) ? '8' : \
        (BUILD_MONTH_IS_SEP) ? '9' : \
        (BUILD_MONTH_IS_OCT) ? '0' : \
        (BUILD_MONTH_IS_NOV) ? '1' : \
        (BUILD_MONTH_IS_DEC) ? '2' : \
        /* error default */    '?' \
    )

#define BUILD_DAY_CH0 ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define BUILD_DAY_CH1 (__DATE__[ 5])

// Example of __TIME__ string: "21:06:19"
//                              01234567
#define BUILD_HOUR_CH0 (__TIME__[0])
#define BUILD_HOUR_CH1 (__TIME__[1])

#define BUILD_MIN_CH0 (__TIME__[3])
#define BUILD_MIN_CH1 (__TIME__[4])

#define BUILD_SEC_CH0 (__TIME__[6])
#define BUILD_SEC_CH1 (__TIME__[7])

// Macros to create integer numbers to set clock with
#define BUILD_HOUR ((BUILD_HOUR_CH0-48)*10+(BUILD_HOUR_CH1-48))
#define BUILD_MIN ((BUILD_MIN_CH0-48)*10+(BUILD_MIN_CH1-48))
#define BUILD_SEC ((BUILD_SEC_CH0-48)*10+(BUILD_SEC_CH1-48))

#define BUILD_MONTH ((BUILD_MONTH_CH0-48)*10+(BUILD_MONTH_CH1-48))
#define BUILD_DAY ((BUILD_DAY_CH0-48)*10+(BUILD_DAY_CH1-48))
#define BUILD_YEAR ((BUILD_YEAR_CH0-48)*1000+(BUILD_YEAR_CH1-48)*100 \
		+(BUILD_YEAR_CH2-48)*10+(BUILD_YEAR_CH3-48))
#define BUILD_YEAR_SHORT (BUILD_YEAR-2000)
#define BUILD_YEAR_FROM_1900 (BUILD_YEAR-1900)

/*********************************************************************************************************************************/
/*  Macros for checking the month string                                                                                         */
/*********************************************************************************************************************************/
#define CHECK_STRING_MONTH_IS_JAN(MON_STR) (MON_STR[0] == 'J' && MON_STR[1] == 'a' && MON_STR[2] == 'n')
#define CHECK_STRING_MONTH_IS_FEB(MON_STR) (MON_STR[0] == 'F')
#define CHECK_STRING_MONTH_IS_MAR(MON_STR) (MON_STR[0] == 'M' && MON_STR[1] == 'a' && MON_STR[2] == 'r')
#define CHECK_STRING_MONTH_IS_APR(MON_STR) (MON_STR[0] == 'A' && MON_STR[1] == 'p')
#define CHECK_STRING_MONTH_IS_MAY(MON_STR) (MON_STR[0] == 'M' && MON_STR[1] == 'a' && MON_STR[2] == 'y')
#define CHECK_STRING_MONTH_IS_JUN(MON_STR) (MON_STR[0] == 'J' && MON_STR[1] == 'u' && MON_STR[2] == 'n')
#define CHECK_STRING_MONTH_IS_JUL(MON_STR) (MON_STR[0] == 'J' && MON_STR[1] == 'u' && MON_STR[2] == 'l')
#define CHECK_STRING_MONTH_IS_AUG(MON_STR) (MON_STR[0] == 'A' && MON_STR[1] == 'u')
#define CHECK_STRING_MONTH_IS_SEP(MON_STR) (MON_STR[0] == 'S')
#define CHECK_STRING_MONTH_IS_OCT(MON_STR) (MON_STR[0] == 'O')
#define CHECK_STRING_MONTH_IS_NOV(MON_STR) (MON_STR[0] == 'N')
#define CHECK_STRING_MONTH_IS_DEC(MON_STR) (MON_STR[0] == 'D')


#define CHECK_STRING_MONTH_CH0(MON_STR) \
    ((CHECK_STRING_MONTH_IS_OCT(MON_STR) || CHECK_STRING_MONTH_IS_NOV(MON_STR) || CHECK_STRING_MONTH_IS_DEC(MON_STR)) ? '1' : '0')

#define CHECK_STRING_MONTH_CH1(MON_STR) \
    ( \
        (CHECK_STRING_MONTH_IS_JAN(MON_STR)) ? '1' : \
        (CHECK_STRING_MONTH_IS_FEB(MON_STR)) ? '2' : \
        (CHECK_STRING_MONTH_IS_MAR(MON_STR)) ? '3' : \
        (CHECK_STRING_MONTH_IS_APR(MON_STR)) ? '4' : \
        (CHECK_STRING_MONTH_IS_MAY(MON_STR)) ? '5' : \
        (CHECK_STRING_MONTH_IS_JUN(MON_STR)) ? '6' : \
        (CHECK_STRING_MONTH_IS_JUL(MON_STR)) ? '7' : \
        (CHECK_STRING_MONTH_IS_AUG(MON_STR)) ? '8' : \
        (CHECK_STRING_MONTH_IS_SEP(MON_STR)) ? '9' : \
        (CHECK_STRING_MONTH_IS_OCT(MON_STR)) ? '0' : \
        (CHECK_STRING_MONTH_IS_NOV(MON_STR)) ? '1' : \
        (CHECK_STRING_MONTH_IS_DEC(MON_STR)) ? '2' : \
        /* error default */    '?' \
    )

#define CHECK_STRING_MONTH(MON_STR) ((CHECK_STRING_MONTH_CH0(MON_STR)-48)*10+(CHECK_STRING_MONTH_CH1(MON_STR)-48))

/*********************************************************************************************************************************/
/*  Other time/date macros                                                                                                       */
/*********************************************************************************************************************************/

// Use Julian Day calculation to find the day of the week
#define DAY_OF_WEEK(DAY,MONTH,YEAR) \
((( DAY                                                      \
  + ((153 * (MONTH + 12 * ((14 - MONTH) / 12) - 3) + 2) / 5) \
  + (365 * (YEAR + 4800 - ((14 - MONTH) / 12)))              \
  + ((YEAR + 4800 - ((14 - MONTH) / 12)) / 4)                \
  - ((YEAR + 4800 - ((14 - MONTH) / 12)) / 100)              \
  + ((YEAR + 4800 - ((14 - MONTH) / 12)) / 400)              \
  - 32045                                                    \
) % 7)+1) // Day of week needs to be adjusted for the MCU peripheral. This macro may not work in some cases.

#ifdef __cplusplus
}
#endif

#endif /* BUILDTIME_H_ */
