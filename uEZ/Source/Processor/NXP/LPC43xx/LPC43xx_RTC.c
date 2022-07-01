/*-------------------------------------------------------------------------*
 * File:  RTC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC43xx RTC Interface.
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
#include <Source/Processor/NXP/LPC43xx/LPC43xx_RTC.h>
#include <lpc43xx.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RTC_VALID_MARKER        0xAA55DCCD
#define RTC_CCR_BITMASK         (0x00000013) // CCR register mask

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_RTC *iHAL;
    TBool iExternalClock;
} T_LPC43xx_RTC_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_RTC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx RTC workspace.
 * Inputs:
 *      void *aW                    -- Particular RTC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_RTC_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_RTC_Workspace *p = (T_LPC43xx_RTC_Workspace *)aWorkspace;

    // Be default, use external clock
    p->iExternalClock = ETrue;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_RTC_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_RTC_Get(void *aWorkspace, T_uezTimeDate *aTimeDate)
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
 * Routine:  LPC43xx_RTC_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_RTC_Set(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
    PARAM_NOT_USED(aWorkspace);//T_LPC43xx_RTC_Workspace *p = (T_LPC43xx_RTC_Workspace *)aWorkspace;
	uint32_t ccrValue = LPC_RTC->CCR; // read current CCR value

	// Temporarily disable Clock Control Register
	if (ccrValue & RTC_CCR_CLKEN_Msk) {
		LPC_RTC->CCR = (ccrValue & (~RTC_CCR_CLKEN_Msk)) & RTC_CCR_BITMASK;
	}

    // Update RTC registers
    LPC_RTC->SEC = aTimeDate->iTime.iSecond;
    LPC_RTC->MIN = aTimeDate->iTime.iMinute;
    LPC_RTC->HRS = aTimeDate->iTime.iHour;
    LPC_RTC->DOM = aTimeDate->iDate.iDay;
    LPC_RTC->MONTH = aTimeDate->iDate.iMonth;
    LPC_RTC->YEAR = aTimeDate->iDate.iYear;

  	// Restore Clock Control Register to old setting
	LPC_RTC->CCR = ccrValue;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_RTC_Configure
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
T_uezError LPC43xx_RTC_Configure(void *aWorkspace, TBool aIsExternalClock)
{
    T_LPC43xx_RTC_Workspace *p = (T_LPC43xx_RTC_Workspace *)aWorkspace;
    p->iExternalClock = aIsExternalClock;

    if((LPC_RTC->CCR & RTC_CCR_CLKEN_Msk) != 0){
        return UEZ_ERROR_NONE;
    }
    // Enable RTC Clock
	LPC_CREG->CREG0 &= ~((1 << 3) | (1 << 2));	/* Reset 32Khz oscillator */
	LPC_CREG->CREG0 |= (1 << 1) | (1 << 0);	/* Enable 32 kHz & 1 kHz on osc32k and release reset */

	// 2-Second delay after enabling RTC clock per datasheet
	LPC_ATIMER->DOWNCOUNTER = 2048;
	while (LPC_ATIMER->DOWNCOUNTER);

	// Disable RTC
    LPC_RTC->CCR = (LPC_RTC->CCR & ~RTC_CCR_CLKEN_Msk) & RTC_CCR_BITMASK;

	// Disable Calibration
    LPC_RTC->CCR |= RTC_CCR_CCALEN_Msk;

	// Reset RTC clock
	LPC_RTC->CCR |= RTC_CCR_CTCRST_Msk;
	while (!(LPC_RTC->CCR & RTC_CCR_CTCRST_Msk)) {}
	// Finish resetting RTC clock
	LPC_RTC->CCR = (LPC_RTC->CCR & ~RTC_CCR_CTCRST_Msk) & RTC_CCR_BITMASK;
	while (LPC_RTC->CCR & RTC_CCR_CTCRST_Msk) {}

	// Clear counter increment and alarm interrupt
	LPC_RTC->ILR = RTC_ILR_RTCCIF_Msk | RTC_ILR_RTCALF_Msk;
	while (LPC_RTC->ILR != 0) {}

	// Clear all register to be default
	LPC_RTC->CIIR = 0x00;
	LPC_RTC->AMR = 0xFF;
	LPC_RTC->CALIBRATION = 0x00;

    // Enable RTC
    LPC_RTC->CCR |= RTC_CCR_CLKEN_Msk;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_RTC_Validate
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
T_uezError LPC43xx_RTC_Validate(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
    T_uezTimeDate time;
    T_uezError error = UEZ_ERROR_NONE;

    // What's the current time?
    LPC43xx_RTC_Get(aWorkspace, &time);

    // Is any part of it invalid?
    if ((time.iTime.iHour >= 24) || (time.iTime.iMinute >= 60)
            || (time.iTime.iSecond >= 60) || (time.iDate.iMonth == 0)
            || (time.iDate.iMonth > 12) || (time.iDate.iDay == 0)
            || (time.iDate.iDay > 31) || (time.iDate.iYear > 9999))
        error = UEZ_ERROR_INVALID;

    if (error == UEZ_ERROR_INVALID) {
        // Reset the RTC
        LPC43xx_RTC_Set(aWorkspace, aTimeDate);
    }

    return error;
}
/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_RTC LPC43xx_RTC_Interface = {
        {
        "LPC43xx:RTC",
        0x0100,
        LPC43xx_RTC_InitializeWorkspace,
        sizeof(T_LPC43xx_RTC_Workspace),
        },

        LPC43xx_RTC_Get,
        LPC43xx_RTC_Set,
        LPC43xx_RTC_Validate, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC43xx_RTC_Require(TBool aIsExternalClock)
{
    T_halWorkspace *p_rtc;

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister(
            "RTC",
            (T_halInterface *)&LPC43xx_RTC_Interface,
            0,
            &p_rtc);
    LPC43xx_RTC_Configure(p_rtc, aIsExternalClock);
}

/*-------------------------------------------------------------------------*
 * End of File:  RTC.c
 *-------------------------------------------------------------------------*/