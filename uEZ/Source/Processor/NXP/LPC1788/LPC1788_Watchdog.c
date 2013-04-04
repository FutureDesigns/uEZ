/*-------------------------------------------------------------------------*
 * File:  Watchdog.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1788 Watchdog Interface.
 * Implementation:
 *      PCLK based implementation only, but can override by
 *      setting WATCHDOG_CLOCK_FREQUENCY
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
#include <uEZ.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1788/LPC1788_Watchdog.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef WATCHDOG_CLOCK_FREQUENCY
#define WATCHDOG_CLOCK_FREQUENCY  500000 // Hz
#endif
#define MAXIMUM_WATCHDOG_CLOCK_CYCLES    (1UL<<24)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_Watchdog *iWatchdog;
    TBool iIsActive;
    TUInt32 iMinimumTime;
    TUInt32 iMaximumTime;
} T_LPC1788_Watchdog_Workspace;

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
#if(COMPILER_TYPE==Keil4)
#define nop()      __nop()
#else
#define nop()      NOP()//asm("nop")
#endif
/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
static TUInt32 IMillisecondsToWatchdogCycles(TUInt32 aMillseconds)
{
    // Calculate the number of cycles
    TUInt32 cycles = (aMillseconds * WATCHDOG_CLOCK_FREQUENCY / 1000);
    // Divide by 4
    cycles >>= 2;
    return cycles;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_Watchdog_SetMaximumTime
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the maximum amount of time that can go by before feeding the
 *      watchdog.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aMaxTime            -- Time til watchdog expires in
 *                                     milliseconds.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError LPC1788_Watchdog_SetMaximumTime(
        void *aWorkspace,
        TUInt32 aMaxTime)
{
    T_LPC1788_Watchdog_Workspace *p =
            (T_LPC1788_Watchdog_Workspace *)aWorkspace;

    // Is this value in range?
    // Calculate value to store
    TUInt32 cycles = IMillisecondsToWatchdogCycles(aMaxTime);
    if (cycles >= MAXIMUM_WATCHDOG_CLOCK_CYCLES)
        return UEZ_ERROR_OUT_OF_RANGE;

    // Cannot set a maximum lower than the current minimum
    // either.
    // Is this range smaller than the maximum?
    if (cycles <= p->iMinimumTime)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Good value, store the ms time
    p->iMaximumTime = aMaxTime;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_Watchdog_SetMinimumTime
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the minimum amount of time that must go by before feeding the
 *      watchdog.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *      TUInt32 aMinTime            -- Time needed before feeding watchdog
 *                                     milliseconds.
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError LPC1788_Watchdog_SetMinimumTime(
        void *aWorkspace,
        TUInt32 aMinTime)
{
    T_LPC1788_Watchdog_Workspace *p =
            (T_LPC1788_Watchdog_Workspace *)aWorkspace;

    // Is this value in range? (note: 0 is valid)
    // Calculate value to store
    TUInt32 cycles = IMillisecondsToWatchdogCycles(aMinTime);
    if (cycles >= MAXIMUM_WATCHDOG_CLOCK_CYCLES)
        return UEZ_ERROR_OUT_OF_RANGE;

    // Is this range smaller than the maximum?
    if (cycles >= p->iMaximumTime)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Good value, store the ms time
    p->iMinimumTime = aMinTime;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_Watchdog_IsResetFromWatchdog
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the minimum amount of time that must go by before feeding the
 *      watchdog.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if reset was watchdog reset, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
static TBool LPC1788_Watchdog_IsResetFromWatchdog(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);
    // Determine if the processor says the last reset is from
    // the watchdog
    // Look at the Reset Source Identification Register
    // (SC block, register RSID, bit WDTR
    if (LPC_SC->RSID & (1<<2)) {
        return ETrue;
    } else {
        return EFalse;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_Watchdog_ClearResetFlag
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear the state of the reset flag that says the last reboot
 *      was from a watchdog reset.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 *---------------------------------------------------------------------------*/
static void LPC1788_Watchdog_ClearResetFlag(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);
    // Write a 1 to the RSID's WDTR bit
    LPC_SC->RSID |= (1<<2);
    // Clear the WDTOF bit
    LPC_WDT->MOD &= ~(1 << 2);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_Watchdog_IsActive
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if the watchdog is actively running.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if watchdog is running, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
static TBool LPC1788_Watchdog_IsActive(void *aWorkspace)
{
    T_LPC1788_Watchdog_Workspace *p =
            (T_LPC1788_Watchdog_Workspace *)aWorkspace;

    // Are we running the watchdog yet?
    return p->iIsActive;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_Watchdog_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the watchdog running.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if watchdog is running, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
static T_uezError LPC1788_Watchdog_Start(void *aWorkspace)
{
    T_LPC1788_Watchdog_Workspace *p =
            (T_LPC1788_Watchdog_Workspace *)aWorkspace;

    // Program and start the watchdog (unless already active)
    if (p->iIsActive)
        return UEZ_ERROR_ILLEGAL_OPERATION;
    p->iIsActive = ETrue;

    // Ensure WDTOF is off
    LPC_WDT->MOD &= (~((1 << 2) | (1 << 3))); // clear WDTOF and WDINT in case set
    LPC_WDT->TC = IMillisecondsToWatchdogCycles(p->iMaximumTime);
    // Determine cycles by subtracting the minimum time from the maximum
    // This gives the cycle count that is decremented down to when
    // the window becomes open.  yeah, its backwards thinking, but the
    // counter is a decrementer.
    LPC_WDT->WINDOW = IMillisecondsToWatchdogCycles(p->iMaximumTime
            - p->iMinimumTime);
    LPC_WDT->MOD = (1 << 0) | // WDEN: Enable to run
            (1 << 1); // WDRESET: Reset mode on!

    //    LPC_WDT->WARNINT = 0; // No warning interrupt in this driver

    // Up to this point, the watchdog is NOT running
    // We have to feed it to make the little dog look for more food.    
    // Start by feeding it
    LPC_WDT->FEED = 0xAA;
    LPC_WDT->FEED = 0x55;

    // We're running ALIVE now.  Reset to occur in 5, 4, 3, ...

    //    LPC_WDT->MOD |= (1<<4); // WDPROTECT: Protect the counter, does not work as advertised

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_Watchdog_Feed
 *---------------------------------------------------------------------------*
 * Description:
 *      Feed the watchdog.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      TBool                       -- ETrue if watchdog is running, else
 *                                      EFalse.
 *---------------------------------------------------------------------------*/
static T_uezError LPC1788_Watchdog_Feed(void *aWorkspace)
{
    T_LPC1788_Watchdog_Workspace *p =
            (T_LPC1788_Watchdog_Workspace *)aWorkspace;

    // Can only feed if we are active
    if (!p->iIsActive)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Feed the watchdog by stuffing 0xAA and 0x55
    LPC_WDT->FEED = 0xAA;
    LPC_WDT->FEED = 0x55;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_Watchdog_Trip
 *---------------------------------------------------------------------------*
 * Description:
 *      Purposely cause the watchdog to be tripped.
 * Inputs:
 *      void *aWorkspace            -- Workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError LPC1788_Watchdog_Trip(void *aWorkspace)
{
    // NOTE: Looks like we can trip at ANY time, do not even have
    // to be active.

    if (aWorkspace) {
        // Trip up the watchdog instantly
        // Do this by half way feeding the watchdog (per the specs, if you
        // touch any of the registers after feeding 0xAA, you'll get a reset).
        LPC_WDT->FEED = 0xAA;
        // Touch something bad
        // In this case, we'll go ahead and try to set the flag that is 1
        // when a reset occurs.
        LPC_WDT->MOD |= (1 << 2);

        // But, just in case the above fails to get a reset (plus, it takes
        // a couple of cycles before the reset occurs), sit here until death
        while (1) {
            nop();
        }
    }
    return UEZ_ERROR_NOT_AVAILABLE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_Watchdog_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 Watchdog workspace.
 * Inputs:
 *      void *aW                    -- Particular Watchdog workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Watchdog_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_Watchdog_Workspace *p =
            (T_LPC1788_Watchdog_Workspace *)aWorkspace;
    p->iIsActive = EFalse;
    p->iMaximumTime = 0; // not set yet
    p->iMinimumTime = 0; // not set yet

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_Watchdog Watchdog_LPC1788_Interface = {
        {
                "Watchdog:NXP:LPC1788",
                0x0106,
                LPC1788_Watchdog_InitializeWorkspace,
                sizeof(T_LPC1788_Watchdog_Workspace), },

        LPC1788_Watchdog_SetMaximumTime,
        LPC1788_Watchdog_SetMinimumTime,
        LPC1788_Watchdog_IsResetFromWatchdog,
        LPC1788_Watchdog_ClearResetFlag,
        LPC1788_Watchdog_IsActive,
        LPC1788_Watchdog_Start,
        LPC1788_Watchdog_Feed,
        LPC1788_Watchdog_Trip };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1788_Watchdog_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("Watchdog",
            (T_halInterface *)&Watchdog_LPC1788_Interface, 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  Watchdog.c
 *-------------------------------------------------------------------------*/
