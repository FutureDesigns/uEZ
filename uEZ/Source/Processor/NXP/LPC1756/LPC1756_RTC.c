/*-------------------------------------------------------------------------*
 * File:  LPC1756_RTC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1756 RTC Interface.
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
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
 /*
 *  @{
 *  @brief     uEZ LPC1756 RTC Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    HAL implementation of the LPC1756 RTC Interface.
 *    Implementation:
 *      Direct access to RTC registers is sufficient here.
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */ 
#include <Config.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_RTC.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RTC_VALID_MARKER        0xAA55DCCD

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_RTC *iHAL;
} T_LPC1756_RTC_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_RTC_InitializeWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Setup the LPC1756 RTC workspace.
 *
 *  @param [in]    *aWorkspace              Particular RTC workspace
 *
 *  @return        T_uezError       Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *	TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_RTC_InitializeWorkspace(void *aWorkspace)
{
//    T_LPC1756_RTC_Workspace *p = (T_LPC1756_RTC_Workspace *)aWorkspace;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_RTC_Get
 *---------------------------------------------------------------------------*/
/**
 *  Get the current date and time.
 *
 *  @param [in]    *aWorkspace           	RTC workspace
 *
 *  @param [in]    *aTimeDate    	Time and date of the current time
 *
 *  @return        T_uezError    	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *	TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_RTC_Get(void *aWorkspace, T_uezTimeDate *aTimeDate)
{
    TUInt32 d, t;

    // Read the time and date, but check to see if it is just
    // changing.  If it just changes, then read again.  We don't
    // want to be caught reading between 11:59:59 and 0:00:00.
    do {
        t = RTC->CTIME0;
        d = RTC->CTIME1;
    } while (t != RTC->CTIME0 || d != RTC->CTIME1);

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
 * Routine:  LPC1756_RTC_Set
 *---------------------------------------------------------------------------*/
/**
 *  Set the current date and time.
 *
 *  @param [in]    *aWorkspace            RTC workspace
 *
 *  @param [in]    *aTimeDate     Time and date of the current time
 *
 *  @return        T_uezError     Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *	TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_RTC_Set(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
//    T_LPC1756_RTC_Workspace *p = (T_LPC1756_RTC_Workspace *)aWorkspace;

    // Stop RTC (disable and reset)
    RTC->CCR = 2;

    // Update RTC registers
    RTC->SEC = aTimeDate->iTime.iSecond;
    RTC->MIN = aTimeDate->iTime.iMinute;
    RTC->HOUR = aTimeDate->iTime.iHour;
    RTC->DOM = aTimeDate->iDate.iDay;
    RTC->MONTH = aTimeDate->iDate.iMonth;
    RTC->YEAR = aTimeDate->iDate.iYear;

    // Start RTC with external XTAL
    RTC->CCR = 1;

    // Store our marker for validity (last general purpose register)
    RTC->GPREG4 = RTC_VALID_MARKER;

    return UEZ_ERROR_NONE;
}

 /*---------------------------------------------------------------------------*
 * Routine:  LPC1756_RTC_Validate
 *---------------------------------------------------------------------------*/
/**
 *  Set the current date and time.
 *
 *  @param [in]    *aWorkspace            RTC workspace
 *
 *  @param [in]    *aTimeDate     Time and date to use if invalid
 *
 *  @return        T_uezError     Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *	TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_RTC_Validate(
        void *aWorkspace,
        const T_uezTimeDate *aTimeDate)
{
    T_uezTimeDate time;
    T_uezError error = UEZ_ERROR_NONE;

    // What's the current time?
    LPC1756_RTC_Get(aWorkspace, &time);

    // Is any part of it invalid?
    if ((time.iTime.iHour >= 24) || (time.iTime.iMinute >= 60)
            || (time.iTime.iSecond >= 60) || (time.iDate.iMonth == 0)
            || (time.iDate.iMonth > 12) || (time.iDate.iDay == 0)
            || (time.iDate.iDay > 31) || (time.iDate.iYear > 9999) ||
            (RTC->GPREG4 != RTC_VALID_MARKER))
        error = UEZ_ERROR_INVALID;

    if (error == UEZ_ERROR_INVALID) {
        // Reset the RTC
        LPC1756_RTC_Set(aWorkspace, aTimeDate);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_RTC LPC1756_RTC_Interface = {
    {
    "LPC1756:RTC",
    0x0100,
    LPC1756_RTC_InitializeWorkspace,
    sizeof(T_LPC1756_RTC_Workspace),
    },

    LPC1756_RTC_Get,
    LPC1756_RTC_Set,
    LPC1756_RTC_Validate,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1756_RTC_Require(TBool aIsExternalClock)
{
    T_halWorkspace *p_rtc;

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister(
            "RTC",
            (T_halInterface *)&LPC1756_RTC_Interface,
            0,
            &p_rtc);
    LPC1756_RTC_Configure(p_rtc, aIsExternalClock);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_RTC.c
 *-------------------------------------------------------------------------*/
