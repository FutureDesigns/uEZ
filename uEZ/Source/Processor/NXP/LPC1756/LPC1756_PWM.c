/*-------------------------------------------------------------------------*
 * File:  LPC1756_PWM.c
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
 /*
 *  @{
 *  @brief     uEZ LPC1756 PWM Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    HAL implementation of the LPC1756 PWMs Interface.
 *    Implementation: Two PWMs are created, PWM0 and PWM1.
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
#include <HAL/GPIO.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_PWM.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iIR;       // 0x00
    TVUInt32 iTCR;      // 0x04
        #define PWM_TCR_COUNTER_ENABLE  (1<<0)
        #define PWM_TCR_COUNTER_RESET   (1<<1)
        #define PWM_TCR_PWM_ENABLE      (1<<3)
        #define PWM_TCR_MASTER_DISABLE  (1<<4)  /** PWM0 only! */
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
    TVUInt32 iresered2[7];  /** 0x54, 0x58, 0x5C, 0x60, 0x64, 0x68, 0x6C */
    TVUInt32 iCTCR;     // 0x70
} old_T_LPC1756_PWM_Registers;

typedef struct {
    const HAL_PWM *iHAL;
//    PWM_TypeDef *iReg;
    PWM_TypeDef *iReg;
    TUInt32 iPeriod;
} T_LPC1756_PWM_Workspace;

static T_uezError LPC1756_PWM_SetMatchRegister(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop);

static void ISetMatchReg(
            PWM_TypeDef *r,
            TUInt8 aMatchReg,
            TUInt32 aValue)
{
    // Set the toggle point for this pin
    switch (aMatchReg) {
        case 0:
            r->MR0 = aValue;
            break;
        case 1:
            r->MR1 = aValue;
            break;
        case 2:
            r->MR2 = aValue;
            break;
        case 3:
            r->MR3 = aValue;
            break;
        case 4:
            r->MR4 = aValue;
            break;
        case 5:
            r->MR5 = aValue;
            break;
        case 6:
            r->MR6 = aValue;
            break;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_PWM_SetMaster
 *---------------------------------------------------------------------------*/
/**
 *  PWM is going to start -- chip select enabled for device
 *
 *  @param [in]    *aWorkspace   	Workspace for PWM
 *
 *  @param [in]    aPeriod        	Number of Fpclk intervals per period
 *
 *  @param [in]    aMatchRegister   Which register is used for master counter
 *
 *  @return        
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_PWM_SetMaster(
        void *aWorkspace,
        TUInt32 aPeriod,
        TUInt8 aMatchRegister)
{
    T_LPC1756_PWM_Workspace *p = (T_LPC1756_PWM_Workspace *)aWorkspace;
    PWM_TypeDef *r = p->iReg;

    // No prescaling
    r->PR = 0;

    // Stop the timer, otherwise we cannot change it
    r->TCR &= ~(PWM_TCR_PWM_ENABLE|PWM_TCR_COUNTER_ENABLE);

    // Set the master match register
    LPC1756_PWM_SetMatchRegister(
            aWorkspace,
            aMatchRegister,
            aPeriod,
            EFalse,
            ETrue,
            EFalse);

    // Reset the counter
    r->TC = 0;
    // Let the counter run
    r->TCR = PWM_TCR_PWM_ENABLE|PWM_TCR_COUNTER_ENABLE;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_PWM_SetMatchRegister
 *---------------------------------------------------------------------------*/
/**
 *  Configure one of the several match registers for this PWM bank.
 *
 *  @param [in]    *aWorkspace      	PWM's workspace
 *
 *  @param [in]    aMatchRegister   	Index to match register (0-7)
 *
 *  @param [in]    aMatchPoint     		Number of PWM cycles until match
 *
 *  @param [in]    aDoInterrupt      	ETrue if want an interrupt, else EFalse
 *                                  	(NOTE: Interrupts currently not 
 *                                  	implemented)
 *  @param [in]    aDoCounterReset  	ETrue if counter for this PWM bank is
 *                                  	to be reset on match.
 *  @param [in]    aDoStop           	ETrue if counter is to be stopped
 *                                  	when match occurs.
 *  @return        T_uezError      		UEZ_ERROR_NONE
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_PWM_SetMatchRegister(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop)
{
    T_LPC1756_PWM_Workspace *p = (T_LPC1756_PWM_Workspace *)aWorkspace;
    PWM_TypeDef *r = p->iReg;
    TUInt32 aFlags = 0;

    if (aDoInterrupt)
        aFlags |= (1<<0);
    if (aDoCounterReset)
        aFlags |= (1<<1);
    if (aDoStop)
        aFlags |= (1<<2);

    // Set the condition
    r->MCR &= ~(7<<(aMatchRegister*3));
    r->MCR |= (aFlags<<(aMatchRegister*3));

    ISetMatchReg(r, aMatchRegister, aMatchPoint);

    // Latch it
    r->LER = (1<<aMatchRegister);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_PWM_EnableSingleEdgeOutput
 *---------------------------------------------------------------------------*/
/**
 *  PWM is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 *
 *  @param [in]    *aWorkspace      PWM's workspace
 *
 *  @param [in]    aMatchRegister   Point in Fpclk cycles where PWM switches
 *
 *  @return        T_uezError       UEZ_ERROR_NONE
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_PWM_EnableSingleEdgeOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_LPC1756_PWM_Workspace *p = (T_LPC1756_PWM_Workspace *)aWorkspace;
    PWM_TypeDef *r = p->iReg;

    // Set to single output mode
    r->PCR &= ~(1<<aMatchRegister);

    // Enable output
    r->PCR |= (1<<(8+aMatchRegister));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_PWM_DisableOutput
 *---------------------------------------------------------------------------*/
/**
 *  PWM is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 *
 *  @param [in]    *aWorkspace      PWM's workspace
 *
 *  @param [in]    aMatchRegister   Point in Fpclk cycles where PWM switches
 *
 *  @return        T_uezError      	UEZ_ERROR_NONE
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_PWM_DisableOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_LPC1756_PWM_Workspace *p = (T_LPC1756_PWM_Workspace *)aWorkspace;
    PWM_TypeDef *r = p->iReg;

    // Disable output
    r->PCR &= ~(1<<(8+aMatchRegister));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_PWM_PWM1_InitializeWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Setup the LPC1756 PWM1 workspace.
 *
 *  @param [in]    *aWorkspace       	Particular PWM workspace
 *
 *  @return        T_uezError       	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_PWM_PWM1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1756_PWM_Workspace *p = (T_LPC1756_PWM_Workspace *)aWorkspace;
    p->iReg = (PWM_TypeDef *)PWM1_BASE;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_PWM PWM_LPC1756_Port1_Interface = {
    {
        "NXP:LPC1756:PWM1",
        0x0100,
        LPC1756_PWM_PWM1_InitializeWorkspace,
        sizeof(T_LPC1756_PWM_Workspace), },

    LPC1756_PWM_SetMaster,
    LPC1756_PWM_SetMatchRegister,
    LPC1756_PWM_EnableSingleEdgeOutput,
    LPC1756_PWM_DisableOutput, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1756_PWM1_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    // Register PWM1
    HALInterfaceRegister("PWM1",
            (T_halInterface *)&PWM_LPC1756_Port1_Interface, 0, 0);
    // Set the PWM divider to be 1
    SC->PCLKSEL0 = (SC->PCLKSEL0 & ~(3<<12))|(1<<12);
}

void LPC1756_PWM1_1_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1756_IOCON_ConfigList pins[] = {
            {GPIO_P1_18, IOCON_D_DEFAULT(2)},
            {GPIO_P2_0,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1756_PWM1_Require();
    LPC1756_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1756_PWM1_2_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1756_IOCON_ConfigList pins[] = {
            {GPIO_P1_20, IOCON_D_DEFAULT(2)},
            {GPIO_P2_1,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1756_PWM1_Require();
    LPC1756_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1756_PWM1_3_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1756_IOCON_ConfigList pins[] = {
            {GPIO_P2_2,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1756_PWM1_Require();
    LPC1756_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1756_PWM1_4_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1756_IOCON_ConfigList pins[] = {
            {GPIO_P1_23, IOCON_D_DEFAULT(2)},
            {GPIO_P2_3,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1756_PWM1_Require();
    LPC1756_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1756_PWM1_5_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1756_IOCON_ConfigList pins[] = {
            {GPIO_P1_24, IOCON_D_DEFAULT(2)},
            {GPIO_P2_4,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1756_PWM1_Require();
    LPC1756_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1756_PWM1_6_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1756_IOCON_ConfigList pins[] = {
            {GPIO_P1_26, IOCON_D_DEFAULT(2)},
            {GPIO_P2_5,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1756_PWM1_Require();
    LPC1756_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1756_PWM1_CAP0_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1756_IOCON_ConfigList pins[] = {
            {GPIO_P1_28, IOCON_D_DEFAULT(2)},
            {GPIO_P2_6,  IOCON_D_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1756_PWM1_Require();
    LPC1756_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC1756_PWM1_CAP1_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC1756_IOCON_ConfigList pins[] = {
            {GPIO_P1_29, IOCON_D_DEFAULT(2)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC1756_PWM1_Require();
    LPC1756_IOCON_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_PWM.c
 *-------------------------------------------------------------------------*/

