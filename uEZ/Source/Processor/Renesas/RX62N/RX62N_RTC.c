/*-------------------------------------------------------------------------*
 * File:  RX62N_RTC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      RX62N RTC
 *
 * Implementation:
 *      Each TMR can be enabled by it's config macro, UEZ_ENABLE_RTCx,
 *      where x is the MTU channel
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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
#include <uEZ.h>
#include "RX62N_RTC.h"
#include <Source/Processor/Renesas/RX62N/RXToolset/iodefine.h>

/*---------------------------------------------------------------------------*
 * Defines:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_RTC *iHAL;
} T_RX62N_RTC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static TUInt16 IBCDToDecimal(TUInt16 aBCD)
{
    TUInt16 decimal = 0;
    int i;

    for (i=0; i<4; i++) {
        decimal *= 10;
        decimal += ((aBCD>>12) & 15);
        aBCD <<= 4;
    }

    return decimal;
}

static TUInt32 IDecimalToBCD(TUInt16 aDecimal)
{
    TUInt16 bcd;
    TUInt16 digit0, digit1, digit2, digit3;

    digit0 = (aDecimal / 1000) % 10;
    digit1 = (aDecimal / 100) % 10;
    digit2 = (aDecimal / 10) % 10;
    digit3 = (aDecimal / 1) % 10;
    bcd = (digit0 << 12) | (digit1 << 8) | (digit2 << 4) | (digit3 << 0);

    return bcd;
}


/*---------------------------------------------------------------------------*
 * Routine:  RX62N_RTC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N RTC workspace.
 * Inputs:
 *      void *aW                    -- Particular RTC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_RTC_InitializeWorkspace(void *aWorkspace)
{
    //T_RX62N_RTC_Workspace *p = (T_RX62N_RTC_Workspace *)aWorkspace;
    PARAM_NOT_USED(aWorkspace);
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_RTC_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_RTC_Get(void *aWorkspace, T_uezTimeDate *aTimeDate)
{
    // Read the time and date, but check to see if it is just
    // changing.  If it just changes, then read again.  We don't
    // want to be caught reading between 11:59:59 and 0:00:00.
    do {
        // Clear the carry flag
        ICU.IR[IR_RTC_CUP].BIT.IR = 0;

        aTimeDate->iTime.iSecond = IBCDToDecimal(RTC.RSECCNT.BYTE);
        aTimeDate->iTime.iMinute = IBCDToDecimal(RTC.RMINCNT.BYTE);
        aTimeDate->iTime.iHour = IBCDToDecimal(RTC.RHRCNT.BYTE);
        aTimeDate->iDate.iDay = IBCDToDecimal(RTC.RDAYCNT.BYTE);
        aTimeDate->iDate.iMonth = IBCDToDecimal(RTC.RMONCNT.BYTE);
        aTimeDate->iDate.iYear = IBCDToDecimal(RTC.RYRCNT.WORD);
    } while (ICU.IR[IR_RTC_CUP].BIT.IR);

    // Now extract the consolidated registers into the structure
    // in a more useful form.

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_RTC_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current date and time.
 * Inputs:
 *      void *aW                     -- RTC workspace
 *      T_uezTimeDate *aTimeDate     -- Time and date of the current time
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_RTC_Set(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
    //T_RX62N_RTC_Workspace *p = (T_RX62N_RTC_Workspace *)aWorkspace;
    PARAM_NOT_USED(aWorkspace);

    // Stop RTC (disable and reset)
    RTC.RCR2.BIT.START = 0;
    RTC.RCR2.BIT.RESET = 1;

    // Update RTC registers
    RTC.RSECCNT.BYTE = IDecimalToBCD(aTimeDate->iTime.iSecond);
    RTC.RMINCNT.BYTE = IDecimalToBCD(aTimeDate->iTime.iMinute);
    RTC.RHRCNT.BYTE = IDecimalToBCD(aTimeDate->iTime.iHour);
    RTC.RDAYCNT.BYTE = IDecimalToBCD(aTimeDate->iDate.iDay);
    RTC.RMONCNT.BYTE = IDecimalToBCD(aTimeDate->iDate.iMonth);
    RTC.RYRCNT.WORD = IDecimalToBCD(aTimeDate->iDate.iYear);

    // Start RTC with external XTAL
    RTC.RCR2.BIT.START = 1;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_RTC_Configure
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
T_uezError RX62N_RTC_Configure(void *aWorkspace)
{
    //T_RX62N_RTC_Workspace *p = (T_RX62N_RTC_Workspace *)aWorkspace;
    PARAM_NOT_USED(aWorkspace);

    // Go ahead and make clock run (regardless of the time)
    RTC.RCR2.BIT.START = 1;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_RTC_Validate
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
T_uezError RX62N_RTC_Validate(
        void *aWorkspace,
        const T_uezTimeDate *aTimeDate)
{
    T_uezTimeDate time;
    T_uezError error = UEZ_ERROR_NONE;

    // What's the current time?
    RX62N_RTC_Get(aWorkspace, &time);

    // Is any part of it invalid?
    if ((time.iTime.iHour >= 24) || (time.iTime.iMinute >= 60)
            || (time.iTime.iSecond >= 60) || (time.iDate.iMonth == 0)
            || (time.iDate.iMonth > 12) || (time.iDate.iDay == 0)
            || (time.iDate.iDay > 31) || (time.iDate.iYear > 9999))
        error = UEZ_ERROR_INVALID;

    if (error == UEZ_ERROR_INVALID) {
        // Reset the RTC
        RX62N_RTC_Set(aWorkspace, aTimeDate);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * HAL Interfaces
 *---------------------------------------------------------------------------*/
const HAL_RTC RTC_RX62N_RTC_Interface = {
    {
    "RTC:RX62N:RTC",
    0x0100,
    RX62N_RTC_InitializeWorkspace,
    sizeof(T_RX62N_RTC_Workspace),
    },

    RX62N_RTC_Get,
    RX62N_RTC_Set,
    RX62N_RTC_Validate,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void RX62N_RTC_Require(void)
{
    void *workspace;

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("RTC", (T_halInterface *)&RTC_RX62N_RTC_Interface, 0,
        (T_halWorkspace **)&workspace);

    RX62N_RTC_Configure(workspace);
}

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_RTC.c
 *-------------------------------------------------------------------------*/

