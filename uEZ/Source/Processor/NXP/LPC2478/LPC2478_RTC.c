/*-------------------------------------------------------------------------*
 * File:  RTC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC2478 RTC Interface.
 * Implementation:
 *      Direct access to RTC registers is sufficient here.
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
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC2478/LPC2478_RTC.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_RTC *iHAL;
    TBool iExternalClock;
} T_LPC2478_RTC_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_RTC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 RTC workspace.
 * Inputs:
 *      void *aW                    -- Particular RTC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_RTC_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_RTC_Workspace *p = (T_LPC2478_RTC_Workspace *)aWorkspace;

    // Be default, use external clock
    p->iExternalClock = ETrue;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_RTC_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_RTC_Get(void *aWorkspace, T_uezTimeDate *aTimeDate)
{
    TUInt32 d, t;

    // Read the time and date, but check to see if it is just
    // changing.  If it just changes, then read again.  We don't
    // want to be caught reading between 11:59:59 and 0:00:00.
    do {
        t = CTIME0;
        d = CTIME1;
    } while (t != CTIME0 || d != CTIME1);

    // Now extract the consolidated registers into the structure
    // in a more useful form.
    aTimeDate->iTime.iSecond = (t >> 0) & 63;
    aTimeDate->iTime.iMinute = (t >> 8) & 63;
    aTimeDate->iTime.iHour = (t >> 16) & 31;
    aTimeDate->iDate.iDay = d & 31;
    aTimeDate->iDate.iMonth = (d >> 8) & 15;
    aTimeDate->iDate.iYear = (d >> 16) & 4095;    // 0 - 4095

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_RTC_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_RTC_Set(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
    T_LPC2478_RTC_Workspace *p = (T_LPC2478_RTC_Workspace *)aWorkspace;

    // Stop RTC (disable and reset)
    CCR = (p->iExternalClock?0x10:0)|2;

    // Update RTC registers
    SEC = aTimeDate->iTime.iSecond;
    MIN = aTimeDate->iTime.iMinute;
    HOUR = aTimeDate->iTime.iHour;
    DOM = aTimeDate->iDate.iDay;
    MONTH = aTimeDate->iDate.iMonth;
    YEAR = aTimeDate->iDate.iYear;

    // Start RTC with external XTAL
    CCR = (p->iExternalClock?0x10:0)|1;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_RTC_Configure
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
T_uezError LPC2478_RTC_Configure(
                void *aWorkspace, 
                TBool aIsExternalClock,
                TUInt16 aPrescaleInteger,
                TUInt16 aPrescaleFraction)
{
    T_LPC2478_RTC_Workspace *p = (T_LPC2478_RTC_Workspace *)aWorkspace;

    PCONP |= PCONP_PCRTC_MASK;
    p->iExternalClock = aIsExternalClock;
    if (!aIsExternalClock) {
        PREINT = aPrescaleInteger;
        PREFRAC = aPrescaleFraction;
    }

    // Go ahead and make clock run (regardless of the time)
    CCR = (p->iExternalClock?0x10:0)|1;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC2478_RTC_Require(
    TBool aIsExternalClock,
    TUInt16 aPrescaleInteger,
    TUInt16 aPrescaleFraction)
{
    T_halWorkspace *p_rtc;

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister(
            "RTC",
            (T_halInterface *)&LPC2478_RTC_Interface,
            0,
            &p_rtc);
    LPC2478_RTC_Configure(p_rtc, aIsExternalClock, aPrescaleInteger,
        aPrescaleFraction);
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_RTC LPC2478_RTC_Interface = {
    "LPC2478:RTC",
    0x0100,
    LPC2478_RTC_InitializeWorkspace,
    sizeof(T_LPC2478_RTC_Workspace),

    LPC2478_RTC_Get,
    LPC2478_RTC_Set,
};

/*-------------------------------------------------------------------------*
 * End of File:  RTC.c
 *-------------------------------------------------------------------------*/
