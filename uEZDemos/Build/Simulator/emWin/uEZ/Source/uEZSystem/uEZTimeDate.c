/*-------------------------------------------------------------------------*
 * File:  uEZTimeDate.c
 *-------------------------------------------------------------------------*
 * Description:
 *     TimeDate Interface
 *-------------------------------------------------------------------------*/
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
 *    @addtogroup uEZTimeDate
 *  @{
 *  @brief     uEZ Time Date Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ Time Date interface.
 *
 *  @par Example code:
 *  Example task to setup Time and date, then later retrieve time from RTC.
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  TUInt32 RTCTest(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezTimeDate TimeDate;
 *      if (UEZTimeDateInit() == UEZ_ERROR_NONE) {
 *
 *          // success on setting up RTC
 *          // Setup intial time into RTC
 *          // set date to January 2, 2012
 *          UEZDateParse(&TimeDate.iDate, "01/02/2012");
 *
 *          // set time to 4:30pm
 *          UEZTimeParse(&TimeDate.iTime, "16:30:00");
 *          if (UEZTimeDateSet(&TimeDate) == UEZ_ERROR_NONE) {
 *              // success on setting RTC to 4:30pm Jan 2, 2012
 *          }
 *
 *          // After some time has passed, read the RTC.
 *          if (UEZTimeDateGet(&TimeDate) == UEZ_ERROR_NONE) {
 *              // success on retriving time from RTC
 *              // current time value stored in TimeDate
 *          }
 *
 *      } else {
 *          // an error occurred initialzing the RTC
 *      }
 *      return 0;
 *  }
 *  @endcode
 */
#include <ctype.h>
#include <string.h>
#include "Config.h"
#include "HAL/HAL.h"
#include "Device/RTC.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>
#include <uEZTimeDate.h>

/*-------------------------------------------------------------------------*
 * Internal data:
 *-------------------------------------------------------------------------*/
static TUInt8 G_daysInMonth[] = { 0, //added for offset
        31, //Jan
        28, //Feb
        31, //Mar
        30, //Apr
        31, //May
        30, //Jun
        31, //Jul
        31, //Aug
        30, //Sep
        31, //Oct
        30, //Nov
        31  //Dec
        };

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_uezDevice G_sysTimeDate;
static DEVICE_RTC **G_sysTimeDateDevice = 0;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static TUInt8 ITD_GrabNumber(char * *aStringPtr);

/*---------------------------------------------------------------------------*
 * Routine:  UEZTimeDateInit
 *---------------------------------------------------------------------------*/
/**
 *  Setup the time/date device using the default RTC device.
 *
 *  @return        T_uezError       If the device is opened, returns
 *                                  UEZ_ERROR_NONE.  If the device cannot be
 *                                  found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  if (UEZTimeDateInit() == UEZ_ERROR_NONE) {
 *      // success on setting up RTC
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimeDateInit(void)
{
    T_uezError error = UEZ_ERROR_NONE;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTimeDateGet
 *---------------------------------------------------------------------------*/
/**
 *  Get the time date structure from the system.
 *
 *  @param [in]    *aTimeDate         Time and date structure returned
 *
 *  @return        T_uezError       If time/date structure retrieved,
 *                                  UEZ_ERROR_NONE, else error code.
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDate TimeDate;
 *  if (UEZTimeDateGet(&TimeDate) == UEZ_ERROR_NONE) {
 *      // success on retriving time from RTC
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimeDateGet(T_uezTimeDate *aTimeDate)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    aTimeDate->iTime.iHour = 12;
    aTimeDate->iTime.iMinute = 12;
    aTimeDate->iTime.iSecond = 12;
    
    aTimeDate->iDate.iMonth = 12;
    aTimeDate->iDate.iDay = 12;
    aTimeDate->iDate.iYear = 2012;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTimeDateSet
 *---------------------------------------------------------------------------*/
/**
 *  Sets the time and date on the system clock.
 *
 *  @param [in]    *aTimeDate         Time and date structure to use
 *
 *  @return        T_uezError       If time/date structure changed,
 *                                  UEZ_ERROR_NONE, else error code.
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDate TimeDate;
 *  UEZDateParse(&TimeDate.iDate, "01/02/2012"); // set date to January 2, 2012
 *  UEZTimeParse(&TimeDate.iTime, "16:30:00");   // set time to 4:30pm
 *  if (UEZTimeDateSet(&TimeDate) == UEZ_ERROR_NONE) {
 *      // success on setting RTC to 4:30pm Jan 2, 2012
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimeDateSet(T_uezTimeDate *aTimeDate)
{
    T_uezError error = UEZ_ERROR_NONE;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTimeDateIsValid
 *---------------------------------------------------------------------------*/
/**
 *  Determines if the current clock is valid or not.  Returns other
 *      errors if there is a problem with the RTC.  If not valid, the
 *      time is reset with the passed in reset time and date.
 *
 *  @param [in]    *aResetTimeDate         Time and date structure to use
 *                                      if the RTC needs to be reset.
 *  @return        T_uezError             If time/date invalid, returns 
 *                                        UEZ_ERROR_INVALID, else returns 
 *                                        UEZ_ERROR_NONE or a system error code.
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDate TimeDate;
 *  UEZDateParse(&TimeDate.iDate, "01/02/2012"); // set date to January 2, 2012
 *  UEZTimeParse(&TimeDate.iTime, "16:30:00");   // set time to 4:30pm
 *  if (UEZTimeDateIsValid(&TimeDate) == UEZ_ERROR_NONE) {
 *      // time matches system clock and is valid (if RTC says 4:30pm Jan 2, 2012)
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTimeDateIsValid(const T_uezTimeDate *aResetTimeDate)
{
    T_uezError error = UEZ_ERROR_NONE;

    return error;
}

/*-------------------------------------------------------------------------*
 * Function:    ITD_GrabNumber
 *---------------------------------------------------------------------------*/
/**
 *  Parse a number from a string and move to the next location.
 *
 *  @param [in]    **aStringPtr     pointer to a pointer of the string
 *
 *  @return        TUInt8             number parsed
 *                      (also) char (*aStringPtr) updated to next character
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTime       Time;
 *  char aString[] = "03:05:06";
 *  char * stringptr = &aString;
 *  Time.iHour = (TUInt8)ITD_GrabNumber(stringptr);
 *  Time.iMinute = (TUInt8)ITD_GrabNumber(stringptr);
 *  Time.iSecond = (TUInt8)ITD_GrabNumber(stringptr);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static TUInt8 ITD_GrabNumber(char * *aStringPtr)
{
    T_uezError error = UEZ_ERROR_NONE;

    return error;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeInit
 *---------------------------------------------------------------------------*/
/**
 *  Reset a time value
 *
 *  @param [in]    *aTime         time structure to reset
 *                 
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTime       Time;
 *  UEZTimeInit(&Time);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZTimeInit(T_uezTime *aTime)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeCalcDuration
 *---------------------------------------------------------------------------*/
/**
 *  Calculates the difference of time between the first and second
 * times and put them into the duration variable.  The calculation assumes
 * the same day and is down to the second.
 *
 *  @param [in]    *aT1         first time
 *
 *  @param [in]    *aT2         second time
 *
 *  @param [in]    *aDuration     difference in time to the second
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTime       Time1;
 *  T_uezTime       Time2;
 *  T_uezTimeDuration Duration;
 *  UEZTimeParse(&Time1, "14:30:00"); // 2:30pm
 *  UEZTimeParse(&Time2, "16:30:00"); // 4:30pm
 *  UEZTimeCalcDuration(&Time1, &Time2, &Duration);
 *  // Duration.iHours = 2
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZTimeCalcDuration(T_uezTime *aT1, T_uezTime *aT2, T_uezTimeDuration *aDuration)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeCompare
 *---------------------------------------------------------------------------*/
/**
 *  Compare two times and determine if the first is greater than the
 * second or visa-versa or they are equal.
 *
 *  @param [in]    *aT1     first to compare
 *
 *  @param [in]    *aT2     second to compare
 *
 *  @return        char        positive if aT1 > aT2
 *                            negative if aT1 < aT2
 *                            0        if aT1 = aT2
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTime       Time1;
 *  T_uezTime       Time2;
 *  UEZTimeParse(&Time1, "14:30:00"); // 2:30pm
 *  UEZTimeParse(&Time2, "16:30:00"); // 4:30pm
 *  char compare = UEZTimeCompare(&Time1, &Time2);
 *  //  compare = -2 hours
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
char UEZTimeCompare(T_uezTime *aT1, T_uezTime *aT2)
{
    return 0;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeParse
 *---------------------------------------------------------------------------*/
/**
 *  Parses a string by reading in three numbers with any non-digit
 * delimiters.  Currently, this version formats the time in the following
 * order:  [Hours] [Minutes] [Seconds]
 * If a field is missing, it is zero.
 *
 *  @param [out]    *aTime         place to store the current date
 *
 *  @param [in]     *aString      time inpput string version
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTime       Time;
 *  UEZTimeParse(&Time, "16:30:00"); // time is now 4:30pm
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZTimeParse(T_uezTime *aTime, char *aString)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeAddDuration
 *---------------------------------------------------------------------------*/
/**
 *   Add the hours, minutes, and seconds of a duration to a time
 * structure.  The resulting value is the number of days that are rolled
 * over.
 *
 *  @param [in]    *aTime         Time structure to have duration added to
 *
 *  @param [in]    *aDuration     Amount of time to add
 *
 *  @return        TUInt16         Number of midnights passed.
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTime       Time;
 *  T_uezTimeDuration Duration;
 *  Duration.iDays = 3;
 *  Duration.iHours = 2;
 *  UEZTimeParse(&Time, "16:30:00"); // 4:30pm
 *  TUInt16 nights = UEZTimeAddDuration(&Time, &Duration);
 *  // time is 18:30, nights = 3
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt16 UEZTimeAddDuration(T_uezTime *aTime, T_uezTimeDuration *aDuration)
{
    return 1;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeSubtractDuration
 *---------------------------------------------------------------------------*/
/**
 *  Subtract the hours, minutes, and seconds of a duration to a time
 *      structure.  The resulting value is the number of days that are rolled
 *      back (0 or 1).
 *
 *  @param [in]    *aTime        Time structure to have duration subtracted from
 *
 *  @param [in]    *aDuration     Amount of time to subtract
 *
 *  @return        TUInt16         Number of midnights rolled back.
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTime       Time;
 *  T_uezTimeDuration Duration;
 *  Duration.iDays = 3;
 *  Duration.iHours = 2;
 *  UEZTimeParse(&Time, "16:30:00"); // 4:30pm
 *  TUInt16 nights = UEZTimeSubtractDuration(&Time, &Duration);
 *  // time is 14:30, nights = 3
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt16 UEZTimeSubtractDuration(T_uezTime *aTime, T_uezTimeDuration *aDuration)
{
    return 1;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateInit
 *---------------------------------------------------------------------------*/
/**
 *  Setup a date structure with zeros.
 *
 *  @param [in]    *aDate         place to store zeros
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateInit(&Date);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDateInit(T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateCompare
 *---------------------------------------------------------------------------*/
/**
 *  Compare two dates and determine if the first is greater than the
 *  second or visa-versa or they are equal.
 *
 *  @param [in]    *aDate1         first to compare
 *
 *  @param [in]    *aDate2         second to compare
 *
 *  @return        TInt8     positive if aDate1 > aDate2
 *                          negative if aDate1 < aDate2
 *                          0        if aDate1 = aDate2
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date1;
 *  T_uezDate       Date2;
 *  UEZDateParse(&Date1, "12/15/2012"); // set date to Dec 15, 2012
 *  UEZDateParse(&Date2, "12/25/2012"); // set date to Dec 25, 2012
 *  TInt8 datecompare = UEZDateCompare(&Date1,&Date2);
 *  // datecompare = -10
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TInt8 UEZDateCompare(T_uezDate *aDate1, T_uezDate *aDate2)
{
    return 0;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateIsLeapYear
 *---------------------------------------------------------------------------*/
/**
 *  Determine if the year of the given date is a leap year.
 *
 *  @param [in]    *aDate         date of which has the year
 *
 *  @return        TBool         ETrue if leapyear, else EFalse
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "12/31/2012"); // set date to Dec 31, 2012
 *  TBool isLeapYear = UEZDateIsLeapYear(&Date);
 *  // isLeapYear = ETrue;
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool UEZDateIsLeapYear(T_uezDate *aDate)
{
    return EFalse;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateGetDayOfYear
 *---------------------------------------------------------------------------*/
/**
 *  Determine the 0 based day of the year.  January 1st is 0 and
 *  December 31st is either day 365 or 366 (if a leap year).
 *
 *  @param [in]    *aDate     date to get the day of the year of
 *
 *  @return        TUInt16     Calculated day of the year
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "12/31/2012"); // set date to Dec 31, 2012
 *  TUInt16 days = UEZDateGetDayOfYear(&Date);
 *  // days == 366
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt16 UEZDateGetDayOfYear(T_uezDate *aDate)
{
    return 1;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateGetNumDaysInMonth
 *---------------------------------------------------------------------------*/
/**
 *  Determine how many days in the given month of the date object
 * taking into consideration if the given year is a leap year.
 *
 *  @param [in]    *aDate     date with month item
 *
 *  @return        TUInt8     Number of days in that month
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "02/01/2012"); // set date to February 1, 2012
 *  TUInt8 days = UEZDateGetNumDaysInMonth(&Date);
 *  // days == 29
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt8 UEZDateGetNumDaysInMonth(T_uezDate *aDate)
{
    return 0;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateNextDay
 *---------------------------------------------------------------------------*/
/** 
 *  Increments a date structure by one day.
 *    
 *  @param [in]    *aDate         date to step forward one day
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "01/01/2012"); // set date to January 1, 2012
 *  UEZDateNextDay(&Date);
 *  // now Date is Jan 2nd, 2012
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDateNextDay(T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateNextWeek
 *---------------------------------------------------------------------------*/
/**
 *  Determines what the date is one week in advance of the current day.
 *
 *  @param [in]    *aDate         date to step forward one week
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "01/01/2012"); // set date to January 1, 2012
 *  UEZDateNextWeek(&Date);
 *  // now Date is Jan 8th, 2012
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDateNextWeek(T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateNextMonth
 *---------------------------------------------------------------------------*/
/**
 *  Determines what the date is one month from the current date.
 *
 *  @param [in]    *aDate     date to step forward one month
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "01/01/2012"); // set date to January 1, 2012
 *  UEZDateNextMonth(&Date);
 *  // now Date is Feb 1st, 2012
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDateNextMonth(T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateNextYear
 *---------------------------------------------------------------------------*/
/**
 *  Determines what the date is one year from the current date and
 * updates the century field.
 *
 *  @param [in]    *aDate     date to step forward one year
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "01/01/2012"); // set date to January 1, 2012
 *  UEZDateNextYear(&Date);
 *  // now Date is Jan 1st, 2013
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDateNextYear(T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDatePreviousDay
 *---------------------------------------------------------------------------*/
/**
 *  Go back one day and adjust the month.
 *
 *  @param [in]    *aDate     date containing a day
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "01/01/2012"); // set date to January 1, 2012
 *  UEZDatePreviousDay(&Date);
 *  // now Date is Dec 31st, 2011
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDatePreviousDay(T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDatePreviousWeek
 *---------------------------------------------------------------------------*/
/**
 *  Go back one week and adjust the month.
 *
 *  @param [in]    *aDate     date containing a day
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "01/02/2012"); // set date to January 2, 2012
 *  UEZDatePreviousWeek(&Date);
 *  // now Date is Dec 26, 2011
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDatePreviousWeek(T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDatePreviousMonth
 *---------------------------------------------------------------------------*/
/**
 *  Go back one month and adjust the year.
 *
 *  @param [in]    *aDate         date containing a month
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  Date.iMonth = 7;
 *  UEZDatePreviousMonth(&Date);
 *  // now Date.iMonth is 6
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDatePreviousMonth(T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDatePreviousYear
 *---------------------------------------------------------------------------*/
/**
 *  Go back one year and adjust century.

 *  @param [in]    *aDate         date containing a year
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  Date.iYear = 2000;
 *  UEZDatePreviousYear(&Date);
 *  // now Date.iYear is 1999
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDatePreviousYear(T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateGetNumDaysInYear
 *---------------------------------------------------------------------------*/
/**
 *  Determine the number of days in a year factoring in leap years.
 *
 *  @param [in]    *aDate     date containing year
 *
 *  @return        TUInt16     number of days in year
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate Date;
 *  Date.iYear = 2012;
 *  TUInt16 days =  UEZDateGetNumDaysInYear(&Date);
 *  // days == 366
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt16 UEZDateGetNumDaysInYear(T_uezDate *aDate)
{
    return 365;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateCalcDuration
 *---------------------------------------------------------------------------*/
/**
 *  Calculates the difference of time in days between the first and second
 *      times and put them into the duration variable.  The calculation assumes
 *      the same day and is down to the second.
 *
 *  @param [in]    *aD1         first date
 *
 *  @param [in]    *aD2         second date
 *
 *  @return        TUInt32         Number of days difference
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date1;
 *  T_uezDate       Date2;
 *  TUInt32 secDifference =  UEZDateCalcDuration(&Date1, &Date2);
 *  // difference in time in seconds of date1 and date2 stored in secDifference
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt32 UEZDateCalcDuration(T_uezDate *aD1, T_uezDate *aD2)
{
    return 0;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateParse
 *---------------------------------------------------------------------------*/
/**
 *  Parses a string by reading in three numbers with any non-digit
 *      delimiters.  Currently, this version formats the date in the following
 *      order:  [Month] [Day] [Year]
 *      If a field is missing, it is zero.
 *
 *  @param [out]   *aDate         place to store the current date
 *
 *  @param [in]    *aString     time input string
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateParse(&Date, "01/02/2012"); // parse January 2, 2012
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDateParse(T_uezDate *aDate, char *aString)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateGetDayOfWeek
 *---------------------------------------------------------------------------*/
/**
 *  Determines what day of the week the current date is.
 *      The result goes from Monday = 0 to Sunday = 6
 *
 *  @param [in]    *aDate     Date to determine day of week of
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *    Date.iMonth = 5; 
 *  Date.iDay = 20;
 *  Date.iYear = 2012; 
 *  TUInt8 dayOfWeek = UEZDateGetDayOfWeek(&Date);
 *  // dayOfWeek == 6, Sunday
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt8 UEZDateGetDayOfWeek(T_uezDate *aDate)
{
    return 0;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZDateDayOfYearToDate
 *---------------------------------------------------------------------------*/
/**
 *  Determine what the date is based on the day of the year.
 *
 *  @param [in]    aDayOfYear     Day of year to convert to date (0 based)
 *
 *  @param [in]    *aDate         Receiving date from day of year
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezDate       Date;
 *  UEZDateDayOfYearToDate(300, &Date); // turn 300th day of year into a date
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZDateDayOfYearToDate(TUInt16 aDayOfYear, T_uezDate *aDate)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeDateCompare
 *---------------------------------------------------------------------------*/
/**
 *  Compare two dates and times and figure out the first is later
 *      than the second (even if down to the second).
 *
 *  @param [in]    *aTD1     first to compare
 *
 *  @param [in]    *aTD2     second to compare
 *
 *  @return        TInt8     positive if aTD1 > aTD2
 *                             negative if aTD1 < aTD2
 *                          zero     if aTD1 = aTD2
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDate TimeDate1;
 *  T_uezTimeDate TimeDate2;
 *  TInt8 result =  UEZTimeDateCompare(&TimeDate1, &TimeDate2);
 *  if (result == 0) {
 *      // TimeDate1 == TimeDate2
 *  } else if result > 0) {
 *      // TimeDate1 greater than TimeDate2
 *  } else {
 *      // TimeDate1 less than TimeDate2
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TInt8 UEZTimeDateCompare(T_uezTimeDate *aTD1, T_uezTimeDate *aTD2)
{
    return 0;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeDateCalcDuration
 *---------------------------------------------------------------------------*/
/**
 *  Calculates the difference of time between the first and second
 *      times and put them into the duration variable.  The calculation is down
 *      to the second.
 *
 *  @param [in]    *aTD1         first date and time
 *
 *  @param [in]    *aTD2         second date and time
 *
 *  @param [in]    *aDuration     difference in time to the second
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDuration Duration;
 *  T_uezTimeDate TimeDate1;
 *  T_uezTimeDate TimeDate2;
 *  UEZTimeDateCalcDuration(&TimeDate1, &TimeDate2, &Duration);
 *  // now time difference stored in Duration
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZTimeDateCalcDuration(
        T_uezTimeDate *aTD1,
        T_uezTimeDate *aTD2,
        T_uezTimeDuration *aDuration)
{
    return;
}

/*---------------------------------------------------------------------------*
 * Function:    UEZTimeDateDurationInit
 *---------------------------------------------------------------------------*/
/**
 *  Clear a duration structure to the standard defaults.
 *
 *  @param [in]    *aDuration     difference in time to the second
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDuration Duration;
 *  UEZTimeDateDurationInit(&Duration);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZTimeDateDurationInit(T_uezTimeDuration *aDuration)
{
    return;
}

/*---------------------------------------------------------------------------*
 * Function:    UEZTimeDateSumDuration
 *---------------------------------------------------------------------------*/
/**
 *  Adds two durations together and puts the result in the first.
 *
 *  @param [in]    *aSum     Sum of the two
 *
 *  @param [in]    *aAdded         duration added to first
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDuration Sum;
 *  T_uezTimeDuration Added;
 *    Sum.iDays = 2;
 *  Added.iDays = 3;
 *  UEZTimeDateSumDuration(&Sum, &Added); // Sum.iDays now equals 5
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZTimeDateSumDuration(T_uezTimeDuration *aSum, T_uezTimeDuration *aAdded)
{
    return;
}

/*---------------------------------------------------------------------------*
 * Function:    UEZTimeDateParse
 *---------------------------------------------------------------------------*/
/**
 *  Parse out the time and date basically doing a call to UEZTimeParse
 *      and UEZDateParse.
 *
 *  @param [in]    *aTD             Time and date structure to fill
 *
 *  @param [in]    *aDateString     Date string to parse
 *
 *  @param [in]    *aTimeString     Time string to parse
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDate TimeDate;
 *  // 4:30pm, January 2nd, 2012
 *  UEZTimeDateParse(&TimeDate;, "01/02/2012", "16:30:00"); 
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZTimeDateParse(
        T_uezTimeDate *aTD,
        char *aDateString,
        char *aTimeString)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeDateDurationToDouble
 *---------------------------------------------------------------------------*/
/**
 *  Convert a duration value into a double which represents 1 day as 1.0
 *
 *  @param [in]    *aDuration     difference in time to the second
 *
 *  @return        double         duration as double float
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDuration Duration;
 *  Duration.iDays = 50; 
 *  result is 50.0
 *  double durationfloat = UEZTimeDateDurationToDouble(&Duration);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
double UEZTimeDateDurationToDouble(T_uezTimeDuration *aDuration)
{
    return 1;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeDateAddDuration
 *---------------------------------------------------------------------------*/
/**
 *  Adds a time duration to a time structure.
 *
 *  @param [in]    *aTD         Time to add to
 *
 *  @param [in]    *aAdded         amount of time to add
 *
 *  @return        void
 *  @par Example Code:
 *  @code 
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDate TimeDate;
 *  T_uezTimeDuration Added;
 *  Added.iDays = 50; 
 *  UEZTimeDateAddDuration(&TimeDate, &Added); // add 50 days
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZTimeDateAddDuration(T_uezTimeDate *aTD, T_uezTimeDuration *aAdded)
{
    return;
}

/*-------------------------------------------------------------------------*
 * Function:    UEZTimeDateSubtractDuration
 *---------------------------------------------------------------------------*/
/**
 *  Subtracts a time duration from a time structure.
 *
 *  @param [in]    *aTD             Time to add to
 *
 *  @param [in]    *aDuration         amount of time to add
 *
 *  @return        void
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTimeDate.h>
 *
 *  T_uezTimeDate TimeDate;
 *  T_uezTimeDuration Duration;
 *  Duration.iDays = 50; 
 *  UEZTimeDateSubtractDuration(&TimeDate, &Duration); // subtract 50 days
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZTimeDateSubtractDuration(
        T_uezTimeDate *aTD,
        T_uezTimeDuration *aDuration)
{
    return;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZTimeDate.c
 *-------------------------------------------------------------------------*/
