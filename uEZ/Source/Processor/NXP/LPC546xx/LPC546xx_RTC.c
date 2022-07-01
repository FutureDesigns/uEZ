/*-------------------------------------------------------------------------*
 * File:  RTC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC546xx RTC Interface.
 * Implementation:
 *      Direct access to RTC registers is sufficient here.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_RTC.h>
#include <LPC54608.h>

//TODO: Remove
#include "iar/Include/CMSIS/LPC54608.h"
/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RTC_VALID_MARKER        0xAA55DCCD
#define SECONDS_IN_A_DAY        (86400U)
#define SECONDS_IN_A_HOUR       (3600U)
#define SECONDS_IN_A_MINUTE     (60U)
#define DAYS_IN_A_YEAR          (365U)
#define YEAR_RANGE_START        (1970U)
#define YEAR_RANGE_END          (2099U)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_RTC *iHAL;
    TBool iExternalClock;
} T_LPC546xx_RTC_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_RTC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx RTC workspace.
 * Inputs:
 *      void *aW                    -- Particular RTC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_RTC_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_RTC_Workspace *p = (T_LPC546xx_RTC_Workspace *)aWorkspace;

    // Be default, use external clock
    p->iExternalClock = ETrue;

    return UEZ_ERROR_NONE;
}

static void LPC546xx_RCT_CovertToTimeDate(T_uezTimeDate *aTimeDate, TUInt32 aCount)
{
    TUInt32 i;
    TUInt32 secondsRemaining;
    TUInt32 days;
    TUInt32 daysInYear;
    //Table of days in month, first value is not used.
    TUInt8 daysPerMonth[13] = {0U, 31U, 28U, 31U, 30U, 31U, 30U, 31U, 31U, 30U, 31U, 30U, 31U};

    secondsRemaining = aCount;

    days = secondsRemaining / SECONDS_IN_A_DAY + 1;

    secondsRemaining = secondsRemaining % SECONDS_IN_A_DAY;

    aTimeDate->iTime.iHour = secondsRemaining / SECONDS_IN_A_HOUR;
    secondsRemaining = secondsRemaining % SECONDS_IN_A_HOUR;
    aTimeDate->iTime.iMinute = secondsRemaining / 60;
    aTimeDate->iTime.iSecond = secondsRemaining % SECONDS_IN_A_MINUTE;

    daysInYear = DAYS_IN_A_YEAR;
    aTimeDate->iDate.iYear = YEAR_RANGE_START;

    while(days > daysInYear){
        days -= daysInYear;
        aTimeDate->iDate.iYear++;

        if(aTimeDate->iDate.iYear & 0x03){
            daysInYear = DAYS_IN_A_YEAR;
        } else {
            daysInYear = DAYS_IN_A_YEAR + 1;
        }
    }

    if(aTimeDate->iDate.iYear & 0x03){
        daysPerMonth[2] = 29;
    }

    for(i = 1; i <= 12; i++){
        if(days <= daysPerMonth[i]){
            aTimeDate->iDate.iMonth = i;
            break;
        } else {
            days -= daysPerMonth[i];
        }
    }
    aTimeDate->iDate.iDay = days;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_RTC_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_RTC_Get(void *aWorkspace, T_uezTimeDate *aTimeDate)
{
    TUInt32 d, t;

    // Read the time and date, but check to see if it is just
    // changing.  If it just changes, then read again.  We don't
    // want to be caught reading between 11:59:59 and 0:00:00.
    do {
        t = RTC->COUNT;
        d = RTC->COUNT;
    }
    while ((t != RTC->COUNT) || (d != RTC->COUNT));

    // Now extract the consolidated registers into the structure
    // in a more useful form.
    LPC546xx_RCT_CovertToTimeDate(aTimeDate, RTC->COUNT);

    return UEZ_ERROR_NONE;
}

static void LPC546xx_RCT_CovertToSeconds(const T_uezTimeDate *aTimeDate)
{
    TUInt16 monthDays[] = {0U, 0U, 31U, 59U, 90U, 120U, 151U, 181U, 212U, 243U, 273U, 304U, 334U};
    TUInt32 seconds;
    
    /* Compute number of days from 1970 till given year*/
    seconds = (aTimeDate->iDate.iYear - YEAR_RANGE_START) * DAYS_IN_A_YEAR;
    /* Add leap year days */
    seconds += ((aTimeDate->iDate.iYear / 4) - (YEAR_RANGE_START / 4));
    /* Add number of days till given month*/
    seconds += monthDays[aTimeDate->iDate.iMonth];
    /* Add days in given month. We subtract the current day as it is
     * represented in the hours, minutes and seconds field*/
    seconds += (aTimeDate->iDate.iDay - 1);
    /* For leap year if month less than or equal to Febraury, decrement day counter*/
    if ((!(aTimeDate->iDate.iYear & 3U)) && (aTimeDate->iDate.iMonth <= 2U))
    {
        seconds--;
    }

    seconds = (seconds * SECONDS_IN_A_DAY) + (aTimeDate->iTime.iHour * SECONDS_IN_A_HOUR) +
              (aTimeDate->iTime.iMinute * SECONDS_IN_A_MINUTE) + aTimeDate->iTime.iSecond;
    
    RTC->COUNT = seconds;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_RTC_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_RTC_Set(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
    PARAM_NOT_USED(aWorkspace);//T_LPC546xx_RTC_Workspace *p = (T_LPC546xx_RTC_Workspace *)aWorkspace;

    //Disable the clock while setting the time
	RTC->CTRL &= ~(1<<6);

    // Update RTC registers
	LPC546xx_RCT_CovertToSeconds(aTimeDate);

  	// Restore Clock Control Register to old setting
	RTC->CTRL |= (1<<6);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_RTC_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure for internal or external clock and turn on.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      TBool aIsExternalClock       -- Internal or external clock config.
 *      TUInt16 aPrescaleInteger     -- Prescalar integer (13-bit) when
 *                                      internal config
 *      TUInt16 aPrescaleFraction    -- Prescalar fraction (15-bit) when
 *                                      internal
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_RTC_Configure(void *aWorkspace, TBool aIsExternalClock)
{
    T_LPC546xx_RTC_Workspace *p = (T_LPC546xx_RTC_Workspace *)aWorkspace;
    p->iExternalClock = aIsExternalClock;

    LPC546xxPowerOn(kCLOCK_Rtc);

    RTC->CTRL = 0;
    
    RTC->CTRL = (1<<7) | (1<<6);//Enable RTC and External clock

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_RTC_Validate
 *---------------------------------------------------------------------------*
 * Description:
 *      Check if the RTC values  have gone corrupt.  If they have gone
 *      corrupt reset the time to given time and report it invalid.
 *      Otherwise, leave everything alone.
 * Inputs:
 *      void *aWorkspace             -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- time to use if invalid
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_RTC_Validate(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
    T_uezTimeDate time;
    T_uezError error = UEZ_ERROR_NONE;

    // What's the current time?
    LPC546xx_RTC_Get(aWorkspace, &time);

    // Is any part of it invalid?
    if ((time.iTime.iHour >= 24) || (time.iTime.iMinute >= 60)
            || (time.iTime.iSecond >= 60) || (time.iDate.iMonth == 0)
            || (time.iDate.iMonth > 12) || (time.iDate.iDay == 0)
            || (time.iDate.iDay > 31) || (time.iDate.iYear > 9999))
        error = UEZ_ERROR_INVALID;

    if (error == UEZ_ERROR_INVALID) {
        // Reset the RTC
        LPC546xx_RTC_Set(aWorkspace, aTimeDate);
    }

    return error;
}
/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_RTC LPC546xx_RTC_Interface = {
        {
        "LPC546xx:RTC",
        0x0100,
        LPC546xx_RTC_InitializeWorkspace,
        sizeof(T_LPC546xx_RTC_Workspace),
        },

        LPC546xx_RTC_Get,
        LPC546xx_RTC_Set,
        LPC546xx_RTC_Validate, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_RTC_Require(TBool aIsExternalClock)
{
    T_halWorkspace *p_rtc;

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister(
            "RTC",
            (T_halInterface *)&LPC546xx_RTC_Interface,
            0,
            &p_rtc);
    LPC546xx_RTC_Configure(p_rtc, aIsExternalClock);
}

/*-------------------------------------------------------------------------*
 * End of File:  RTC.c
 *-------------------------------------------------------------------------*/
