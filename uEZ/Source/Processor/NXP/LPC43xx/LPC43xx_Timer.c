/*-------------------------------------------------------------------------*
 * File:  Timer.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC43xx Timers Interface.
 * Implementation:
 *      Two Timers are created, Timer0 and Timer1.
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
#include <uEZErrors.h>
#include <uEZProcessor.h>
#include <HAL/GPIO.h>
#include <HAL/Timer.h>
#include <uEZBSP.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Timer.h>
#include <HAL/Interrupt.h>
#include <uEZPlatformAPI.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iIR; // 0x00
    TVUInt32 iTCR; // 0x04
    TVUInt32 iTC; // 0x08
    TVUInt32 iPR; // 0x0C
    TVUInt32 iPC; // 0x10
    TVUInt32 iMCR; // 0x14
    TVUInt32 iMR[4]; // 0x18, 0x1C, 0x20, 0x24
    TVUInt32 iCCR; // 0x28
    TVUInt32 iCR[4]; // 0x2C, 0x30, 0x34, 0x38
    TVUInt32 iEMR; // 0x3C
    TVUInt32 ireserved1[4]; // 0x40, 0x44, 0x48, 0x4C
    TVUInt32 ireserved2[4]; // 0x50, 0x54, 0x58, 0x5C
    TVUInt32 ireserved3[4]; // 0x60, 0x64, 0x68, 0x6C
    TVUInt32 iCTCR; // 0x70
} T_LPC43xx_Timer_Registers;

typedef struct {
    TUInt16 iVector;
    TISRFPtr iISR;
    const char *iName;
    TUInt8 iPCONBitIndex;
} T_LPC43xx_Timer_Info;

typedef struct {
    T_HALTimer_Callback iCallbackFunc;
    void *iCallbackWorkspace;
} T_LPC43xx_Timer_InterruptCallback;

typedef struct {
    const HAL_Timer *iHAL;
    T_LPC43xx_Timer_Registers *iReg;
    T_LPC43xx_Timer_InterruptCallback iCallbacks[4]; // one per match register
    const T_LPC43xx_Timer_Info *iInfo;
} T_LPC43xx_Timer_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static const T_LPC43xx_Timer_Info G_Timer0_Info;
static const T_LPC43xx_Timer_Info G_Timer1_Info;
static const T_LPC43xx_Timer_Info G_Timer2_Info;
static const T_LPC43xx_Timer_Info G_Timer3_Info;
T_LPC43xx_Timer_Workspace *G_Timer0_Workspace;
T_LPC43xx_Timer_Workspace *G_Timer1_Workspace;
T_LPC43xx_Timer_Workspace *G_Timer2_Workspace;
T_LPC43xx_Timer_Workspace *G_Timer3_Workspace;
static void ITimerProcessIRQ(T_LPC43xx_Timer_Workspace *p);

/*---------------------------------------------------------------------------*
 * Routine:  TimerX_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Interrupt service routines
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(Timer0_ISR)
{
    IRQ_START();
    ITimerProcessIRQ(G_Timer0_Workspace);
    IRQ_END()
    ;
}

IRQ_ROUTINE(Timer1_ISR)
{
    IRQ_START();
    ITimerProcessIRQ(G_Timer1_Workspace);
    IRQ_END()
    ;
}

IRQ_ROUTINE(Timer2_ISR)
{
    IRQ_START();
    ITimerProcessIRQ(G_Timer2_Workspace);
    IRQ_END()
    ;
}

IRQ_ROUTINE(Timer3_ISR)
{
    IRQ_START();
    ITimerProcessIRQ(G_Timer3_Workspace);
    IRQ_END()
    ;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer_SetTimerMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Timer is going to start -- chip select enabled for device
 * Inputs:
 *      void *aWorkspace        -- Workspace for Timer
 *      T_Timer_Mode aMode      -- Timer mode of operation
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE if success, else code.
 *---------------------------------------------------------------------------*/
static T_uezError LPC43xx_Timer_SetTimerMode(
        void *aWorkspace,
        T_Timer_Mode aMode)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    T_LPC43xx_Timer_Registers *r = p->iReg;

    // Setup the mode
    r->iCTCR = (r->iCTCR & ~3) | aMode;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer_SetCaptureSource
 *---------------------------------------------------------------------------*
 * Description:
 *      Timer is going to start -- chip select enabled for device
 * Inputs:
 *      void *aWorkspace        -- Workspace for Timer
 *      TUInt32 aSourceIndex    -- Timer capture source (depends on hardware
 *                                   underneath).
 * Outputs:
 *      T_uezError              -- Error code.  UEZ_ERROR_NONE if success.
 *                                  UEZ_ERROR_NOT_FOUND if source index
 *                                  does not exist.
 *---------------------------------------------------------------------------*/
static T_uezError LPC43xx_Timer_SetCaptureSource(
        void *aWorkspace,
        TUInt32 aSourceIndex)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    T_LPC43xx_Timer_Registers *r = p->iReg;

    if (aSourceIndex > 4)
        return UEZ_ERROR_NOT_FOUND;

    // Setup the source of the capture
    r->iCTCR = (r->iCTCR & ~(3 << 2)) | (aSourceIndex << 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer_SetMatchRegister
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup one of several timer's match registers and what do on
 *      a match.
 * Outputs:
 *      void *aWorkspace        -- Timer's workspace
 *      TUInt8 aMatchRegister   -- Index to match register (0-3)
 *      TUInt32 aMatchPoint     -- Number of CPU cycles until match
 *      TBool aDoInterrupt      -- ETrue if want an interrupt, else EFalse
 *                                  (NOTE: Interrupts currently not
 *                                  implemented)
 *      TBool aDoCounterReset   -- ETrue if counter for this Timer is
 *                                  to be reset on match.
 *      TBool aDoStop           -- ETrue if counter is to be stopped
 *                                  when match occurs.
 *---------------------------------------------------------------------------*/
static T_uezError LPC43xx_Timer_SetMatchRegister(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    T_LPC43xx_Timer_Registers *r = p->iReg;
    TUInt32 aFlags = 0;

    // Determine if legal match register
    if (aMatchRegister >= 4)
        return UEZ_ERROR_ILLEGAL_PARAMETER;

    // Determine what the combined flags are
    if (aDoInterrupt)
        aFlags |= (1 << 0);
    if (aDoCounterReset)
        aFlags |= (1 << 1);
    if (aDoStop)
        aFlags |= (1 << 2);

    // Set the actions on the match
    r->iMCR &= ~(7 << (aMatchRegister * 3));
    r->iMCR |= (aFlags << (aMatchRegister * 3));

    // Change the match register
    r->iMR[aMatchRegister] = aMatchPoint / (PROCESSOR_OSCILLATOR_FREQUENCY
        / PCLK_FREQUENCY);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer_SetExternalControl
 *---------------------------------------------------------------------------*
 * Description:
 *      Timer is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 * Inputs:
 *      void *aWorkspace        -- Timer's workspace
 *      TUInt8 aMatchRegister   -- Point in Fpclk cycles where Timer switches
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError LPC43xx_Timer_SetExternalControl(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        T_Timer_ExternalControl aExtControl)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    T_LPC43xx_Timer_Registers *r = p->iReg;

    // Determine if legal match register
    if (aMatchRegister >= 4)
        return UEZ_ERROR_ILLEGAL_PARAMETER;

    // Change the external control
    r->iEMR &= ~(3 << (4 + aMatchRegister * 2));
    r->iEMR |= (aExtControl << (4 + aMatchRegister * 2));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Reset the timer's counter.
 * Inputs:
 *      void *aWorkspace        -- Timer's workspace
 *---------------------------------------------------------------------------*/
static void LPC43xx_Timer_Reset(void *aWorkspace)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    T_LPC43xx_Timer_Registers *r = p->iReg;
    int i;

    // Reset the counter
    r->iTCR |= (1 << 1);

    // Small delay to let the timer reset
    for (i = 0; i < 5; i++) {
    }

    // Release the counter
    r->iTCR &= ~(1 << 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer_Enable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable (run) the timer's counter.
 * Inputs:
 *      void *aWorkspace        -- Timer's workspace
 *---------------------------------------------------------------------------*/
static void LPC43xx_Timer_Enable(void *aWorkspace)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    T_LPC43xx_Timer_Registers *r = p->iReg;

    // Enable the counter
    r->iTCR |= (1 << 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer_Disable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable the timer's counter.
 * Inputs:
 *      void *aWorkspace        -- Timer's workspace
 *---------------------------------------------------------------------------*/
static void LPC43xx_Timer_Disable(void *aWorkspace)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    T_LPC43xx_Timer_Registers *r = p->iReg;

    // Disable the counter
    r->iTCR &= ~(1 << 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  ITimerProcessIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      Routine called when IRQ occurs.  Determines which callbacks to
 *      call (if any) and clears the interrupts.
 * Inputs:
 *      T_LPC247x_Timer_Workspace *p -- Timer workspace
 *---------------------------------------------------------------------------*/
static void ITimerProcessIRQ(T_LPC43xx_Timer_Workspace *p)
{
    // Look at the registers and see which are causing an interrupt
    TUInt8 ir = p->iReg->iIR;
    if (ir & (1 << 0)) {
        // Match register 0
        if (p->iCallbacks[0].iCallbackFunc)
            p->iCallbacks[0].iCallbackFunc(p->iCallbacks[0].iCallbackWorkspace);
    }
    if (ir & (1 << 1)) {
        // Match register 1
        if (p->iCallbacks[1].iCallbackFunc)
            p->iCallbacks[1].iCallbackFunc(p->iCallbacks[1].iCallbackWorkspace);
    }
    if (ir & (1 << 2)) {
        // Match register 2
        if (p->iCallbacks[2].iCallbackFunc)
            p->iCallbacks[2].iCallbackFunc(p->iCallbacks[2].iCallbackWorkspace);
    }
    if (ir & (1 << 3)) {
        // Match register 3
        if (p->iCallbacks[3].iCallbackFunc)
            p->iCallbacks[3].iCallbackFunc(p->iCallbacks[3].iCallbackWorkspace);
    }
    // Clear interrupts called in this pass (if another one came in, we'll get it
    // on the next interrupt).
    p->iReg->iIR = ir;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC247x_Timer_SetMatchCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a callback routine for this timer on one of the match
 *      registers.  The callback is called when a match occurs and an
 *      interrupt occurs.  The callback is called from within the ISR.
 * Inputs:
 *      void *aWorkspace -- Timer workspace
 *      TUInt8 aMatchRegister -- Which match register to set callback
 *      T_Timer_Callback aCallbackFunc -- Function to call, or 0 to clear.
 *      void *aCallbackWorkspace -- Workspace to pass to callback
 * Outputs:
 *      T_uezError -- UEZ_ERROR_ILLEGAL_PARAMETER if match register is out
 *          of range.
 *---------------------------------------------------------------------------*/
static T_uezError LPC43xx_Timer_SetMatchCallback(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        T_HALTimer_Callback aCallbackFunc,
        void *aCallbackWorkspace)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    TBool any;
    TUInt8 i;

    // Determine if legal match register
    if (aMatchRegister >= 4)
        return UEZ_ERROR_ILLEGAL_PARAMETER;

    // Set the callback (which can be 0 as well as an address)
    p->iCallbacks[aMatchRegister].iCallbackFunc = aCallbackFunc;
    p->iCallbacks[aMatchRegister].iCallbackWorkspace = aCallbackWorkspace;

    // Check to see if there are any callbacks
    any = EFalse;
    for (i = 0; i < 4; i++) {
        if (p->iCallbacks[i].iCallbackFunc) {
            any = ETrue;
            break;
        }
    }

    // Register or deregister the interrupt
    if (any) {
        // Register if not already registered
        if (!InterruptIsRegistered(p->iInfo->iVector))
            InterruptRegister(p->iInfo->iVector, p->iInfo->iISR,
                    INTERRUPT_PRIORITY_HIGH, p->iInfo->iName);
        InterruptEnable(p->iInfo->iVector);
    } else {
        // No one registered anymore, turn off this callback (but leave registered)
        InterruptDisable(p->iInfo->iVector);
    }
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IClear
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear all the callback information for this workspace
 * Inputs:
 *      T_LPC247x_Timer_Workspace *p  -- Particular Timer workspace
 *---------------------------------------------------------------------------*/
static void IClear(T_LPC43xx_Timer_InterruptCallback aCallbacks[4])
{
    TUInt8 i;
    for (i = 0; i < 4; i++) {
        aCallbacks[i].iCallbackFunc = 0;
        aCallbacks[i].iCallbackWorkspace = 0;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx Timer0 workspace.
 * Inputs:
 *      void *aW                    -- Particular Timer workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Timer0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    p->iReg = (T_LPC43xx_Timer_Registers *)LPC_TIMER0_BASE;
    p->iInfo = &G_Timer0_Info;
    IClear(p->iCallbacks);

    G_Timer0_Workspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx Timer1 workspace.
 * Inputs:
 *      void *aW                    -- Particular Timer workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Timer1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    p->iReg = (T_LPC43xx_Timer_Registers *)LPC_TIMER1_BASE;
    p->iInfo = &G_Timer1_Info;
    IClear(p->iCallbacks);

    G_Timer1_Workspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx Timer1 workspace.
 * Inputs:
 *      void *aW                    -- Particular Timer workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Timer2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    p->iReg = (T_LPC43xx_Timer_Registers *)LPC_TIMER2_BASE;
    p->iInfo = &G_Timer2_Info;
    IClear(p->iCallbacks);

    G_Timer2_Workspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer3_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx Timer1 workspace.
 * Inputs:
 *      void *aW                    -- Particular Timer workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Timer3_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    p->iReg = (T_LPC43xx_Timer_Registers *)LPC_TIMER3_BASE;
    p->iInfo = &G_Timer3_Info;
    IClear(p->iCallbacks);

    G_Timer3_Workspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_Timer_SetMatchRegisterFunctions
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup one of several timer's match registers and what to do on
 *      a match.
 * Inputs:
 *      void *aWorkspace        -- Timer's workspace
 *      TUInt8 aMatchRegister   -- Index to match register (0-3)
 *      T_Timer_MatchPointFunction aFunctions -- List of functions to perform
 *          when the timer reaches this match point.
 *      TUInt32 aMatchPoint -- Point at which functions trigger
 * Outputs:
 *      T_uezError -- Returns UEZ_ERROR_ILLEGAL_PARAMETER if match register
 *          does not exist.  Returns UEZ_ERROR_ILLEGAL_OPERATION if a
 *          function in aFunctions is not supported.  Otherwise, returns
 *          UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LPC43xx_Timer_SetMatchRegisterFunctions(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        T_Timer_MatchPointFunction aFunctions,
        TUInt32 aMatchPoint)
{
    T_LPC43xx_Timer_Workspace *p = (T_LPC43xx_Timer_Workspace *)aWorkspace;
    T_LPC43xx_Timer_Registers *r = p->iReg;
    TUInt32 aFlags = 0;
    //const T_LPC43xx_Timer_Info *p_info = p->iInfo;

    // Determine if legal match register
    if (aMatchRegister >= 4)
        return UEZ_ERROR_ILLEGAL_PARAMETER;

    if (aFunctions & ~(TIMER_MATCH_POINT_FUNC_INTERRUPT
            | TIMER_MATCH_POINT_FUNC_RESET | TIMER_MATCH_POINT_FUNC_STOP))
        return UEZ_ERROR_ILLEGAL_OPERATION;

    // Determine what the combined flags are
    if (aFunctions & TIMER_MATCH_POINT_FUNC_INTERRUPT)
        aFlags |= (1 << 0);
    if (aFunctions & TIMER_MATCH_POINT_FUNC_RESET)
        aFlags |= (1 << 1);
    if (aFunctions & TIMER_MATCH_POINT_FUNC_STOP)
        aFlags |= (1 << 2);

    // Set the actions on the match
    r->iMCR &= ~(7 << (aMatchRegister * 3));
    r->iMCR |= (aFlags << (aMatchRegister * 3));

    // Change the match register
    r->iMR[aMatchRegister] = aMatchPoint / (PROCESSOR_OSCILLATOR_FREQUENCY
        / PCLK_FREQUENCY);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Internal Information tables:
 *---------------------------------------------------------------------------*/
static const T_LPC43xx_Timer_Info G_Timer0_Info = {
        TIMER0_IRQn,
        (TISRFPtr)Timer0_ISR,
        "Timer0",
        1, };

static const T_LPC43xx_Timer_Info G_Timer1_Info = {
        TIMER1_IRQn,
        (TISRFPtr)Timer1_ISR,
        "Timer1",
        2, };

static const T_LPC43xx_Timer_Info G_Timer2_Info = {
        TIMER2_IRQn,
        (TISRFPtr)Timer2_ISR,
        "Timer2",
        22, };

static const T_LPC43xx_Timer_Info G_Timer3_Info = {
        TIMER3_IRQn,
        (TISRFPtr)Timer3_ISR,
        "Timer3",
        23, };

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_Timer LPC43xx_Timer0_Interface = {
    {
    "NXP:LPC43xx:Timer0",
    0x0100,
    LPC43xx_Timer0_InitializeWorkspace,
    sizeof(T_LPC43xx_Timer_Workspace),
    },

    LPC43xx_Timer_SetTimerMode,
    LPC43xx_Timer_SetCaptureSource,
    LPC43xx_Timer_SetMatchRegister,
    LPC43xx_Timer_SetExternalControl,
    LPC43xx_Timer_Reset,
    LPC43xx_Timer_Enable,
    LPC43xx_Timer_Disable,
    LPC43xx_Timer_SetMatchCallback,
    LPC43xx_Timer_SetMatchRegisterFunctions,
};

const HAL_Timer LPC43xx_Timer1_Interface = {
    {
    "NXP:LPC43xx:Timer1",
    0x0100,
    LPC43xx_Timer1_InitializeWorkspace,
    sizeof(T_LPC43xx_Timer_Workspace),
    },

    LPC43xx_Timer_SetTimerMode,
    LPC43xx_Timer_SetCaptureSource,
    LPC43xx_Timer_SetMatchRegister,
    LPC43xx_Timer_SetExternalControl,
    LPC43xx_Timer_Reset,
    LPC43xx_Timer_Enable,
    LPC43xx_Timer_Disable,
    LPC43xx_Timer_SetMatchCallback,
    LPC43xx_Timer_SetMatchRegisterFunctions,
};

const HAL_Timer LPC43xx_Timer2_Interface = {
    {
    "NXP:LPC43xx:Timer2",
    0x0100,
    LPC43xx_Timer2_InitializeWorkspace,
    sizeof(T_LPC43xx_Timer_Workspace),
    },

    LPC43xx_Timer_SetTimerMode,
    LPC43xx_Timer_SetCaptureSource,
    LPC43xx_Timer_SetMatchRegister,
    LPC43xx_Timer_SetExternalControl,
    LPC43xx_Timer_Reset,
    LPC43xx_Timer_Enable,
    LPC43xx_Timer_Disable,
    LPC43xx_Timer_SetMatchCallback,
    LPC43xx_Timer_SetMatchRegisterFunctions,
};

const HAL_Timer LPC43xx_Timer3_Interface = {
    {
    "NXP:LPC43xx:Timer3",
    0x0100,
    LPC43xx_Timer3_InitializeWorkspace,
    sizeof(T_LPC43xx_Timer_Workspace),
    },

    LPC43xx_Timer_SetTimerMode,
    LPC43xx_Timer_SetCaptureSource,
    LPC43xx_Timer_SetMatchRegister,
    LPC43xx_Timer_SetExternalControl,
    LPC43xx_Timer_Reset,
    LPC43xx_Timer_Enable,
    LPC43xx_Timer_Disable,
    LPC43xx_Timer_SetMatchCallback,
    LPC43xx_Timer_SetMatchRegisterFunctions,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC43xx_Timer0_Require(const T_LPC43xx_Timer_Settings *aSettings)
{
    static const T_LPC43xx_SCU_ConfigList cap0[] = {
            {GPIO_P4_4     ,SCU_NORMAL_DRIVE_DEFAULT(7)},
    };
    static const T_LPC43xx_SCU_ConfigList cap1[] = {
            {GPIO_P1_5     ,SCU_NORMAL_DRIVE_DEFAULT(3)},
            {GPIO_P4_5     ,SCU_NORMAL_DRIVE_DEFAULT(7)},
    };
    static const T_LPC43xx_SCU_ConfigList cap2[] = {
            {GPIO_P0_15     ,SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P4_6     ,SCU_NORMAL_DRIVE_DEFAULT(7)},
    };
    static const T_LPC43xx_SCU_ConfigList cap3[] = {
            {GPIO_P0_4     ,SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P4_7     ,SCU_NORMAL_DRIVE_DEFAULT(7)},
    };

    static const T_LPC43xx_SCU_ConfigList mat0[] = {
            {GPIO_P0_3    ,SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P4_0    ,SCU_NORMAL_DRIVE_DEFAULT(7)},
    };
    static const T_LPC43xx_SCU_ConfigList mat1[] = {
            {GPIO_P0_2    ,SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P4_1    ,SCU_NORMAL_DRIVE_DEFAULT(7)},
    };
    static const T_LPC43xx_SCU_ConfigList mat2[] = {
            {GPIO_P1_7    ,SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P4_2    ,SCU_NORMAL_DRIVE_DEFAULT(7)},
    };
    static const T_LPC43xx_SCU_ConfigList mat3[] = {
            {GPIO_P0_13   ,SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P4_3    ,SCU_NORMAL_DRIVE_DEFAULT(7)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register Timer0
    HALInterfaceRegister("Timer0", (T_halInterface *)&LPC43xx_Timer0_Interface,
            0, 0);

    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[0], cap0, ARRAY_COUNT(cap0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[1], cap1, ARRAY_COUNT(cap1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[2], cap2, ARRAY_COUNT(cap2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[3], cap3, ARRAY_COUNT(cap3));

    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[0], mat0, ARRAY_COUNT(mat0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[1], mat1, ARRAY_COUNT(mat1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[2], mat2, ARRAY_COUNT(mat2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[3], mat3, ARRAY_COUNT(mat3));
}

void LPC43xx_Timer1_Require(const T_LPC43xx_Timer_Settings *aSettings)
{
    static const T_LPC43xx_SCU_ConfigList cap0[] = {
            {GPIO_P1_6    , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P2_9    , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList cap1[] = {
            {GPIO_P2_10   , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList cap2[] = {
            {GPIO_P2_11   , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList cap3[] = {
            {GPIO_P2_12   , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };

    static const T_LPC43xx_SCU_ConfigList mat0[] = {
            {GPIO_P2_13   , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList mat1[] = {
            {GPIO_P2_14   , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList mat2[] = {
            {GPIO_P2_15   , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList mat3[] = {
            {GPIO_P2_7    , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register Timer1
    HALInterfaceRegister("Timer1", (T_halInterface *)&LPC43xx_Timer1_Interface,
            0, 0);
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[0], cap0, ARRAY_COUNT(cap0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[1], cap1, ARRAY_COUNT(cap1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[2], cap2, ARRAY_COUNT(cap2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[3], cap3, ARRAY_COUNT(cap3));

    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[0], mat0, ARRAY_COUNT(mat0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[1], mat1, ARRAY_COUNT(mat1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[2], mat2, ARRAY_COUNT(mat2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[3], mat3, ARRAY_COUNT(mat3));
}

void LPC43xx_Timer2_Require(const T_LPC43xx_Timer_Settings *aSettings)
{
    static const T_LPC43xx_SCU_ConfigList cap0[] = {
            {GPIO_P3_0     , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList cap1[] = {
            {GPIO_P3_1     , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList cap2[] = {
            {GPIO_P3_2     , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList cap3[] = {
            {GPIO_P0_5     , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };

    static const T_LPC43xx_SCU_ConfigList mat0[] = {
            {GPIO_P5_15    , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList mat1[] = {
            {GPIO_P5_16    , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList mat2[] = {
            {GPIO_P3_5     , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };
    static const T_LPC43xx_SCU_ConfigList mat3[] = {
            {GPIO_P3_7     , SCU_NORMAL_DRIVE_DEFAULT(5)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register Timer2
    HALInterfaceRegister("Timer2", (T_halInterface *)&LPC43xx_Timer2_Interface,
            0, 0);
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[0], cap0, ARRAY_COUNT(cap0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[1], cap1, ARRAY_COUNT(cap1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[2], cap2, ARRAY_COUNT(cap2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[3], cap3, ARRAY_COUNT(cap3));

    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[0], mat0, ARRAY_COUNT(mat0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[1], mat1, ARRAY_COUNT(mat1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[2], mat2, ARRAY_COUNT(mat2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[3], mat3, ARRAY_COUNT(mat3));
}

void LPC43xx_Timer3_Require(const T_LPC43xx_Timer_Settings *aSettings)
{
    static const T_LPC43xx_SCU_ConfigList cap0[] = {
            {GPIO_P6_0    , SCU_NORMAL_DRIVE_DEFAULT(6)},
    };
    static const T_LPC43xx_SCU_ConfigList cap1[] = {
            {GPIO_P5_1    , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P6_3    , SCU_NORMAL_DRIVE_DEFAULT(6)},
    };
    static const T_LPC43xx_SCU_ConfigList cap2[] = {
            {GPIO_P5_2    , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P6_4    , SCU_NORMAL_DRIVE_DEFAULT(6)},
    };
    static const T_LPC43xx_SCU_ConfigList cap3[] = {
            {GPIO_P5_6    , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P6_5    , SCU_NORMAL_DRIVE_DEFAULT(6)},
    };

    static const T_LPC43xx_SCU_ConfigList mat0[] = {
            {GPIO_P5_3    , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P6_6    , SCU_NORMAL_DRIVE_DEFAULT(6)},
    };
    static const T_LPC43xx_SCU_ConfigList mat1[] = {
            {GPIO_P5_4    , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P6_7    , SCU_NORMAL_DRIVE_DEFAULT(6)},
    };
    static const T_LPC43xx_SCU_ConfigList mat2[] = {
            {GPIO_P5_5    , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P6_8    , SCU_NORMAL_DRIVE_DEFAULT(6)},
    };
    static const T_LPC43xx_SCU_ConfigList mat3[] = {
            {GPIO_P0_7    , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P6_9    , SCU_NORMAL_DRIVE_DEFAULT(6)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register Timer3
    HALInterfaceRegister("Timer3", (T_halInterface *)&LPC43xx_Timer3_Interface,
            0, 0);
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[0], cap0, ARRAY_COUNT(cap0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[1], cap1, ARRAY_COUNT(cap1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[2], cap2, ARRAY_COUNT(cap2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iCAP[3], cap3, ARRAY_COUNT(cap3));

    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[0], mat0, ARRAY_COUNT(mat0));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[1], mat1, ARRAY_COUNT(mat1));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[2], mat2, ARRAY_COUNT(mat2));
    LPC43xx_SCU_ConfigPinOrNone(aSettings->iMAT[3], mat3, ARRAY_COUNT(mat3));
}

/*-------------------------------------------------------------------------*
 * End of File:  Timer.c
 *-------------------------------------------------------------------------*/

