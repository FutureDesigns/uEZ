/*--------------------------------------------------------------------------*/
/*  RTC controls                                                            */

#include "rtc.h"
#include <UEZTimeDate.h>


BOOL rtc_gettime (RTC *rtc)
{
    T_uezTimeDate td;
    T_uezError error;

    error = UEZTimeDateGet(&td);

    if (error != UEZ_ERROR_NONE) {
        // If there is an error, return Jan. 1, 2000, 12:00:00
        rtc->sec = 0;
        rtc->min = 0;
        rtc->hour = 12;
        rtc->mday = 1;
        rtc->month = 1;
        rtc->year = 2000;

        return FALSE;
    }

    rtc->sec = td.iTime.iSecond;
    rtc->min = td.iTime.iMinute;
    rtc->hour = td.iTime.iHour;
    rtc->wday = 0;  // not specified
    rtc->mday = td.iDate.iDay;
    rtc->month = td.iDate.iMonth;
    rtc->year = td.iDate.iYear;

    return TRUE;
}




BOOL rtc_settime (const RTC *rtc)
{
    T_uezTimeDate td;
    T_uezError error;

    td.iTime.iSecond = rtc->sec;
    td.iTime.iMinute = rtc->min;
    td.iTime.iHour = rtc->hour;
    td.iDate.iDay = rtc->mday;
    td.iDate.iMonth = rtc->month;
    td.iDate.iYear = rtc->year;

    error = UEZTimeDateSet(&td);
    if (error != UEZ_ERROR_NONE)
        return FALSE;

    return TRUE;
}


/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */
/* This is not required in read-only configuration.        */


DWORD get_fattime ()
{
	RTC rtc = {0, 0, 0, 0, 0, 0};

	rtc_gettime(&rtc);

	return	        ((DWORD)(rtc.year-1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.mday << 16)
			| ((DWORD)rtc.hour << 11)
			| ((DWORD)rtc.min << 5)
			| ((DWORD)rtc.sec >> 1);
}
