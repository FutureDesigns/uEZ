/*-------------------------------------------------------------------------*
 * File:  LPC1756_Watchdog.c
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
 *  @brief     uEZ LPC1756 Watchdog Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    HAL implementation of the LPC1756 Watchdog Interface.
 * Implementation:
 *      PCLK based implementation only, but can override by
 *      setting WATCHDOG_CLOCK_FREQUENCY
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
#include <uEZ.h>
#include <uEZProcessor.h>
#include <Config_Build.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_Watchdog.h>
#include <uEZPlatformAPI.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef WATCHDOG_CLOCK_FREQUENCY
    #define WATCHDOG_CLOCK_FREQUENCY  PROCESSOR_OSCILLATOR_FREQUENCY // counts per second
#endif
#define MAXIMUM_WATCHDOG_CLOCK_CYCLES      (1UL<<31)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_Watchdog *iWatchdog;
    TBool iIsActive;
    TUInt32 iMinimumTime;
    TUInt32 iMaximumTime;
} T_LPC1756_Watchdog_Workspace;

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
    TUInt32 cycles = (aMillseconds * (WATCHDOG_CLOCK_FREQUENCY / 1000));
    // Divide by 4
    cycles >>= 2;
    return cycles;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_Watchdog_SetMaximumTime
 *---------------------------------------------------------------------------*/
/**
 *  Set the maximum amount of time that can go by before feeding the
 *      watchdog.
 *
 *  @param [in]    *aWorkspace   	Workspace
 *
 *  @param [in]    aMaxTime       	Time til watchdog expires in
 *                                     milliseconds.
 *  @return        T_uezError      	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_Watchdog_SetMaximumTime(
        void *aWorkspace,
        TUInt32 aMaxTime)
{
    T_LPC1756_Watchdog_Workspace *p =
        (T_LPC1756_Watchdog_Workspace *)aWorkspace;

    // Is this value in range?
    // Calculate value to store
    TUInt32 cycles = IMillisecondsToWatchdogCycles(aMaxTime);
    if (cycles >= MAXIMUM_WATCHDOG_CLOCK_CYCLES)
        return UEZ_ERROR_OUT_OF_RANGE;

    // Good value, store the ms time
    p->iMaximumTime = aMaxTime;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_Watchdog_SetMinimumTime
 *---------------------------------------------------------------------------*/
/**
 *  The LPC1756 only has a maximum time
 *
 *  @param [in]    *aWorkspace   	Workspace
 *
 *  @param [in]    aMinTime      	Time needed before feeding watchdog
 *                                     milliseconds.
 *  @return        T_uezError     	UEZ_ERROR_ILLEGAL_OPERATION
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_Watchdog_SetMinimumTime(
        void *aWorkspace,
        TUInt32 aMinTime)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_Watchdog_IsResetFromWatchdog
 *---------------------------------------------------------------------------*/
/**
 *  Set the minimum amount of time that must go by before feeding the
 *      watchdog.
 *
 *  @param [in]    *aWorkspace 	Workspace
 *
 *  @return        TBool 		ETrue if reset was watchdog reset, else EFalse.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static TBool LPC1756_Watchdog_IsResetFromWatchdog(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);
    // Determine if the processor says the last reset is from
    // the watchdog
    // Look at the Reset Source Identification Register
    // (SC block, register RSID, bit WDTR
    if (SC->RSID & (1<<2)) {
        return ETrue;
    } else {
        return EFalse;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_Watchdog_ClearResetFlag
 *---------------------------------------------------------------------------*/
/**
 *  Clear the state of the reset flag that says the last reboot
 *      was from a watchdog reset.
 *
 *  @param [in]   *aWorkspace  	Workspace
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void LPC1756_Watchdog_ClearResetFlag(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);

    SC->RSID &= ~(1<<2);    // Clear the RSID's WDTR bit
    WDT->WDMOD &= ~(1<<1);  // Clear the WDRESET bit
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_Watchdog_IsActive
 *---------------------------------------------------------------------------*/
/**
 *  Determine if the watchdog is actively running.
 *
 *  @param [in]    *aWorkspace  	Workspace
 *
 *  @return        TBool       		ETrue if watchdog is running, else EFalse.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static TBool LPC1756_Watchdog_IsActive(void *aWorkspace)
{
    T_LPC1756_Watchdog_Workspace *p =
        (T_LPC1756_Watchdog_Workspace *)aWorkspace;

    // Are we running the watchdog yet?
    return p->iIsActive;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_Watchdog_Start
 *---------------------------------------------------------------------------*/
/**
 *  Start the watchdog running.
 *
 *  @param [in]    *aWorkspace  Workspace
 *
 *  @return        TBool    	ETrue if watchdog is running, else EFalse.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_Watchdog_Start(void *aWorkspace)
{
    T_LPC1756_Watchdog_Workspace *p =
        (T_LPC1756_Watchdog_Workspace *)aWorkspace;

    // Program and start the watchdog (unless already active)
    if (p->iIsActive)
        return UEZ_ERROR_ILLEGAL_OPERATION;
    p->iIsActive = ETrue;

    // Divide by 1, not by 4
    SC->PCLKSEL0 &= ~3;
    SC->PCLKSEL0 |= 1;

    // Ensure WDTOF is off
    WDT->WDMOD &= (~((1<<2)|(1<<3))); // clear WDTOF and WDINT in case set
    WDT->WDCLKSEL = 1; // choose peripheral clock
    WDT->WDTC = IMillisecondsToWatchdogCycles(p->iMaximumTime);
    WDT->WDMOD |=
          (1<<0) |  // WDEN: Enable to run
          (1<<1);   // WDRESET: Reset mode on!

    // Up to this point, the watchdog is NOT running
    // We have to feed it to make the little dog look for more food.
    // Start by feeding it
    WDT->WDFEED = 0xAA;
    WDT->WDFEED = 0x55;

    // We're running ALIVE now.  Reset to occur in 5, 4, 3, ...

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_Watchdog_Feed
 *---------------------------------------------------------------------------*/
/**
 *  Feed the watchdog.
 *
 *  @param [in]    *aWorkspace    	Workspace
 *
 *  @return        TBool        	ETrue if watchdog is running, else EFalse.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_Watchdog_Feed(void *aWorkspace)
{
    T_LPC1756_Watchdog_Workspace *p =
        (T_LPC1756_Watchdog_Workspace *)aWorkspace;

    // Can only feed if we are active
    if (!p->iIsActive)
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Feed the watchdog by stuffing 0xAA and 0x55
    WDT->WDFEED = 0xAA;
    WDT->WDFEED = 0x55;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_Watchdog_Trip
 *---------------------------------------------------------------------------*/
/**
 *  Purposely cause the watchdog to be tripped.
 *
 *  @param [in]    *aWorkspace   	Workspace
 *
 *  @return        TBool       		ETrue if watchdog is running, else  EFalse
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_Watchdog_Trip(void *aWorkspace)
{
    // NOTE: Looks like we can trip at ANY time, do not even have
    // to be active.

    if (aWorkspace) {
        // Trip up the watchdog instantly
        // Do this by half way feeding the watchdog (per the specs, if you
        // touch any of the registers after feeding 0xAA, you'll get a reset).
        WDT->WDFEED = 0xAA;
        // Touch something bad
        // In this case, we'll go ahead and try to set the flag that is 1
        // when a reset occurs.
        WDT->WDMOD |= (1 << 2);

        // But, just in case the above fails to get a reset (plus, it takes
        // a couple of cycles before the reset occurs), sit here until death
        while (1) {
            nop();
        }
    }
    return UEZ_ERROR_NOT_AVAILABLE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_Watchdog_InitializeWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Setup the LPC1756 Watchdog workspace.
 *
 *  @param [in]    *aWorkspace  	Particular Watchdog workspace
 *
 *  @return        T_uezError   	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_Watchdog_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1756_Watchdog_Workspace *p = (T_LPC1756_Watchdog_Workspace *)aWorkspace;
    p->iIsActive = EFalse;
    p->iMaximumTime = 0; // not set yet

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_Watchdog Watchdog_LPC1756_Interface = {
	{
            "Watchdog:NXP:LPC1756",
	    0x0106,
	    LPC1756_Watchdog_InitializeWorkspace,
	    sizeof(T_LPC1756_Watchdog_Workspace),
    },

    LPC1756_Watchdog_SetMaximumTime,
    LPC1756_Watchdog_SetMinimumTime,
    LPC1756_Watchdog_IsResetFromWatchdog,
    LPC1756_Watchdog_ClearResetFlag,
    LPC1756_Watchdog_IsActive,
    LPC1756_Watchdog_Start,
    LPC1756_Watchdog_Feed,
    LPC1756_Watchdog_Trip
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1756_Watchdog_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("Watchdog",
            (T_halInterface *)&Watchdog_LPC1756_Interface, 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  Watchdog.c
 *-------------------------------------------------------------------------*/
