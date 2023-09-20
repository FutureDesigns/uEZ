/*-------------------------------------------------------------------------*
 * File:  RTC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC17xx_40xx RTC Interface.
 * Implementation:
 *      Direct access to RTC registers is sufficient here.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_RTC.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RTC_VALID_MARKER        0xAA55DCCD

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_RTC *iHAL;
    TBool iExternalClock;
} T_LPC17xx_40xx_RTC_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_RTC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC17xx_40xx RTC workspace.
 * Inputs:
 *      void *aW                    -- Particular RTC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_RTC_InitializeWorkspace(void *aWorkspace)
{
    T_LPC17xx_40xx_RTC_Workspace *p = (T_LPC17xx_40xx_RTC_Workspace *)aWorkspace;

    // Be default, use external clock
    p->iExternalClock = ETrue;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_RTC_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_RTC_Get(void *aWorkspace, T_uezTimeDate *aTimeDate)
{
    TUInt32 d, t;

    // Read the time and date, but check to see if it is just
    // changing.  If it just changes, then read again.  We don't
    // want to be caught reading between 11:59:59 and 0:00:00.
    do {
        t = LPC_RTC->CTIME0;
        d = LPC_RTC->CTIME1;
    }
    while ((t != LPC_RTC->CTIME0) || (d != LPC_RTC->CTIME1));

    // Now extract the consolidated registers into the structure
    // in a more useful form.
    aTimeDate->iTime.iSecond = (t >> 0) & 63;
    aTimeDate->iTime.iMinute = (t >> 8) & 63;
    aTimeDate->iTime.iHour = (t >> 16) & 31;
    aTimeDate->iDate.iDay = d & 31;
    aTimeDate->iDate.iMonth = (d >> 8) & 15;
    aTimeDate->iDate.iYear = (d >> 16) & 4095; // 0 - 4095

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_RTC_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_RTC_Set(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
    T_LPC17xx_40xx_RTC_Workspace *p = (T_LPC17xx_40xx_RTC_Workspace *)aWorkspace;

    // Stop RTC (disable and reset)
    LPC_RTC->CCR = (p->iExternalClock?0x10:0)|2;

    // Update RTC registers
    LPC_RTC->SEC = aTimeDate->iTime.iSecond;
    LPC_RTC->MIN = aTimeDate->iTime.iMinute;
    LPC_RTC->HOUR = aTimeDate->iTime.iHour;
    LPC_RTC->DOM = aTimeDate->iDate.iDay;
    LPC_RTC->MONTH = aTimeDate->iDate.iMonth;
    LPC_RTC->YEAR = aTimeDate->iDate.iYear;

    // Make sure the oscillator error flag is cleared
    LPC_RTC->RTC_AUX |= (1<<4); // OSCF cleared

    // Start RTC with external XTAL
    LPC_RTC->CCR = (p->iExternalClock?0x10:0)|1;

    // Store our marker for validity (last general purpose register)
    LPC_RTC->GPREG4 = RTC_VALID_MARKER;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_RTC_Configure
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
T_uezError LPC17xx_40xx_RTC_Configure(void *aWorkspace, TBool aIsExternalClock)
{
    T_LPC17xx_40xx_RTC_Workspace *p = (T_LPC17xx_40xx_RTC_Workspace *)aWorkspace;

    LPC17xx_40xxPowerOn(1 << 9);
    p->iExternalClock = aIsExternalClock;

    // Go ahead and make clock run (regardless of the time)
    LPC_RTC->CCR = (p->iExternalClock?0x10:0)|1;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_RTC_Validate
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
T_uezError LPC17xx_40xx_RTC_Validate(
        void *aWorkspace,
        const T_uezTimeDate *aTimeDate)
{
    T_uezTimeDate time;
    T_uezError error = UEZ_ERROR_NONE;

    // What's the current time?
    LPC17xx_40xx_RTC_Get(aWorkspace, &time);

    // Is any part of it invalid?
    if ((time.iTime.iHour >= 24) || (time.iTime.iMinute >= 60)
            || (time.iTime.iSecond >= 60) || (time.iDate.iMonth == 0)
            || (time.iDate.iMonth > 12) || (time.iDate.iDay == 0)
            || (time.iDate.iDay > 31) || (time.iDate.iYear > 9999) ||
            (LPC_RTC->GPREG4 != RTC_VALID_MARKER))
        error = UEZ_ERROR_INVALID;

    if (error == UEZ_ERROR_INVALID) {
        // Reset the RTC
        LPC17xx_40xx_RTC_Set(aWorkspace, aTimeDate);
    }

    return error;
}
/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_RTC LPC17xx_40xx_RTC_Interface = {
        {
        "LPC17xx_40xx:RTC",
        0x0100,
        LPC17xx_40xx_RTC_InitializeWorkspace,
        sizeof(T_LPC17xx_40xx_RTC_Workspace),
        },

        LPC17xx_40xx_RTC_Get,
        LPC17xx_40xx_RTC_Set,
        LPC17xx_40xx_RTC_Validate, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_RTC_Require(TBool aIsExternalClock)
{
    T_halWorkspace *p_rtc;

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister(
            "RTC",
            (T_halInterface *)&LPC17xx_40xx_RTC_Interface,
            0,
            &p_rtc);
    LPC17xx_40xx_RTC_Configure(p_rtc, aIsExternalClock);
}

/*-------------------------------------------------------------------------*
 * End of File:  RTC.c
 *-------------------------------------------------------------------------*/
