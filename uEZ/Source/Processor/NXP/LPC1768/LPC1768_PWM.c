/*-------------------------------------------------------------------------*
 * File:  PWM.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1768 PWMs Interface.
 * Implementation:
 *      Two PWMs are created, PWM0 and PWM1.
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
#include <Config.h>
#include <uEZTypes.h>
#include <HAL/GPIO.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_PWM.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iIR;       // 0x00
    TVUInt32 iTCR;      // 0x04
        #define PWM_TCR_COUNTER_ENABLE  (1<<0)
        #define PWM_TCR_COUNTER_RESET   (1<<1)
        #define PWM_TCR_PWM_ENABLE      (1<<3)
        #define PWM_TCR_MASTER_DISABLE  (1<<4)  // PWM0 only!
    TVUInt32 iTC;       // 0x08
    TVUInt32 iPR;       // 0x0C
    TVUInt32 iPC;       // 0x10
    TVUInt32 iMCR;      // 0x14
    TVUInt32 iMR0;      // 0x18
    TVUInt32 iMR1;      // 0x1C
    TVUInt32 iMR2;      // 0x20
    TVUInt32 iMR3;      // 0x24
    TVUInt32 iCCR;      // 0x28
    TVUInt32 iCR0;      // 0x2C
    TVUInt32 iCR1;      // 0x30
    TVUInt32 iCR2;      // 0x34
    TVUInt32 iCR3;      // 0x38
    TVUInt32 ireserved; // 0x3C
    TVUInt32 iMR4;      // 0x40
    TVUInt32 iMR5;      // 0x44
    TVUInt32 iMR6;      // 0x48
    TVUInt32 iPCR;      // 0x4C
    TVUInt32 iLER;      // 0x50
    TVUInt32 iresered2[7];  // 0x54, 0x58, 0x5C, 0x60, 0x64, 0x68, 0x6C
    TVUInt32 iCTCR;     // 0x70
} old_T_LPC1768_PWM_Registers;

typedef struct {
    const HAL_PWM *iHAL;
//    PWM_TypeDef *iReg;
    old_T_LPC1768_PWM_Registers *iReg;
    TUInt32 iPeriod;
} T_LPC1768_PWM_Workspace;

static T_uezError LPC1768_PWM_SetMatchRegister(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop);

static void ISetMatchReg(
            old_T_LPC1768_PWM_Registers *r,
            TUInt8 aMatchReg,
            TUInt32 aValue)
{
    // Set the toggle point for this pin
    switch (aMatchReg) {
        case 0:
            r->iMR0 = aValue;
            break;
        case 1:
            r->iMR1 = aValue;
            break;
        case 2:
            r->iMR2 = aValue;
            break;
        case 3:
            r->iMR3 = aValue;
            break;
        case 4:
            r->iMR4 = aValue;
            break;
        case 5:
            r->iMR5 = aValue;
            break;
        case 6:
            r->iMR6 = aValue;
            break;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_PWM_SetMaster
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to start -- chip select enabled for device
 * Outputs:
 *      void *aWorkspace        -- Workspace for PWM
 *      TUInt32 aPeriod         -- Number of Fpclk intervals per period
 *      TUInt8 aMatchRegister   -- Which register is used for master counter
 *---------------------------------------------------------------------------*/
static T_uezError LPC1768_PWM_SetMaster(
        void *aWorkspace,
        TUInt32 aPeriod,
        TUInt8 aMatchRegister)
{
    T_LPC1768_PWM_Workspace *p = (T_LPC1768_PWM_Workspace *)aWorkspace;
    old_T_LPC1768_PWM_Registers *r = p->iReg;

    // No prescaling
    r->iPR = 0;

    // Stop the timer, otherwise we cannot change it
    r->iTCR &= ~(PWM_TCR_PWM_ENABLE|PWM_TCR_COUNTER_ENABLE);

    // Set the master match register
    LPC1768_PWM_SetMatchRegister(
            aWorkspace,
            aMatchRegister,
            aPeriod,
            EFalse,
            ETrue,
            EFalse);

    // Reset the counter
    r->iTC = 0;
    // Let the counter run
    r->iTCR = PWM_TCR_PWM_ENABLE|PWM_TCR_COUNTER_ENABLE;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_PWM_SetMatchRegister
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure one of the several match registers for this PWM bank.
 * Outputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Index to match register (0-7)
 *      TUInt32 aMatchPoint     -- Number of PWM cycles until match
 *      TBool aDoInterrupt      -- ETrue if want an interrupt, else EFalse
 *                                  (NOTE: Interrupts currently not
 *                                  implemented)
 *      TBool aDoCounterReset   -- ETrue if counter for this PWM bank is
 *                                  to be reset on match.
 *      TBool aDoStop           -- ETrue if counter is to be stopped
 *                                  when match occurs.
 *---------------------------------------------------------------------------*/
static T_uezError LPC1768_PWM_SetMatchRegister(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop)
{
    T_LPC1768_PWM_Workspace *p = (T_LPC1768_PWM_Workspace *)aWorkspace;
    old_T_LPC1768_PWM_Registers *r = p->iReg;
    TUInt32 aFlags = 0;

    if (aDoInterrupt)
        aFlags |= (1<<0);
    if (aDoCounterReset)
        aFlags |= (1<<1);
    if (aDoStop)
        aFlags |= (1<<2);

    // Set the condition
    r->iMCR &= ~(7<<(aMatchRegister*3));
    r->iMCR |= (aFlags<<(aMatchRegister*3));

    ISetMatchReg(r, aMatchRegister, aMatchPoint);

    // Latch it
    r->iLER = (1<<aMatchRegister);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_PWM_EnableSingleEdgeOutput
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 * Inputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Point in Fpclk cycles where PWM switches
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError LPC1768_PWM_EnableSingleEdgeOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_LPC1768_PWM_Workspace *p = (T_LPC1768_PWM_Workspace *)aWorkspace;
    old_T_LPC1768_PWM_Registers *r = p->iReg;

    // Set to single output mode
    r->iPCR &= ~(1<<aMatchRegister);

    // Enable output
    r->iPCR |= (1<<(8+aMatchRegister));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_PWM_DisableOutput
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 * Inputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Point in Fpclk cycles where PWM switches
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError LPC1768_PWM_DisableOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_LPC1768_PWM_Workspace *p = (T_LPC1768_PWM_Workspace *)aWorkspace;
    old_T_LPC1768_PWM_Registers *r = p->iReg;

    // Disable output
    r->iPCR &= ~(1<<(8+aMatchRegister));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_PWM_PWM1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1768 PWM1 workspace.
 * Inputs:
 *      void *aW                    -- Particular PWM workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_PWM_PWM1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1768_PWM_Workspace *p = (T_LPC1768_PWM_Workspace *)aWorkspace;
    p->iReg = (old_T_LPC1768_PWM_Registers *)LPC_PWM1_BASE;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_PWM PWM_LPC1768_Port1_Interface = {
    {
    "NXP:LPC1768:PWM1",
    0x0100,
    LPC1768_PWM_PWM1_InitializeWorkspace,
    sizeof(T_LPC1768_PWM_Workspace),
    },

    LPC1768_PWM_SetMaster,
    LPC1768_PWM_SetMatchRegister,
    LPC1768_PWM_EnableSingleEdgeOutput,
    LPC1768_PWM_DisableOutput,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1768_PWM1_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    // Register PWM1
    HALInterfaceRegister("PWM1",
            (T_halInterface *)&PWM_LPC1768_Port1_Interface, 0, 0);
    // Set the PWM divider to be 1
    LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~(3<<12))|(1<<12);
}

void LPC1768_PWM1_1_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1768_IOCON_ConfigList pins[] = {
            {GPIO_P1_18, IOCON_D_DEFAULT(2)},
            {GPIO_P2_0,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1768_PWM1_Require();
    LPC1768_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1768_PWM1_2_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1768_IOCON_ConfigList pins[] = {
            {GPIO_P1_20, IOCON_D_DEFAULT(2)},
            {GPIO_P2_1,  IOCON_D_DEFAULT(1)},
            {GPIO_P3_25, IOCON_D_DEFAULT(2)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1768_PWM1_Require();
    LPC1768_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1768_PWM1_3_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1768_IOCON_ConfigList pins[] = {
            {GPIO_P1_21, IOCON_D_DEFAULT(2)},
            {GPIO_P2_2,  IOCON_D_DEFAULT(1)},
            {GPIO_P3_26, IOCON_D_DEFAULT(2)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1768_PWM1_Require();
    LPC1768_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1768_PWM1_4_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1768_IOCON_ConfigList pins[] = {
            {GPIO_P1_23, IOCON_D_DEFAULT(2)},
            {GPIO_P2_3,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1768_PWM1_Require();
    LPC1768_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1768_PWM1_5_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1768_IOCON_ConfigList pins[] = {
            {GPIO_P1_24, IOCON_D_DEFAULT(2)},
            {GPIO_P2_4,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1768_PWM1_Require();
    LPC1768_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1768_PWM1_6_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1768_IOCON_ConfigList pins[] = {
            {GPIO_P1_26, IOCON_D_DEFAULT(2)},
            {GPIO_P2_5,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1768_PWM1_Require();
    LPC1768_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1768_PWM1_CAP0_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1768_IOCON_ConfigList pins[] = {
            {GPIO_P1_28, IOCON_D_DEFAULT(2)},
            {GPIO_P2_6,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1768_PWM1_Require();
    LPC1768_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1768_PWM1_CAP1_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1768_IOCON_ConfigList pins[] = {
            {GPIO_P1_29, IOCON_D_DEFAULT(2)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1768_PWM1_Require();
    LPC1768_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}


/*-------------------------------------------------------------------------*
 * End of File:  PWM.c
 *-------------------------------------------------------------------------*/

