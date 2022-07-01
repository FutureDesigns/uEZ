/*-------------------------------------------------------------------------*
 * File:  PWM.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC546xx PWMs Interface.
 * Implementation:
 *      Two PWMs are created, PWM0 and PWM1. Capture inputs are not implemented.
 *      This uses the motor control PWM of the LPC4357 but is only setup to do
 *      basic PWM outputs. B outputs will be inverted.
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
#include <HAL/GPIO.h>
#include <uEZBSP.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_PWM.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
        TVUInt32  iCON;                   /*!< (@ 0x400A0000) PWM Control read address        */
        TVUInt32  iCON_SET;               /*!< (@ 0x400A0004) PWM Control set address         */
        TVUInt32  iCON_CLR;               /*!< (@ 0x400A0008) PWM Control clear address       */
        TVUInt32  iCAPCON;                /*!< (@ 0x400A000C) Capture Control read address    */
        TVUInt32  iCAPCON_SET;            /*!< (@ 0x400A0010) Capture Control set address     */
        TVUInt32  iCAPCON_CLR;            /*!< (@ 0x400A0014) Event Control clear address     */
        TVUInt32  iTC0;                   /*!< (@ 0x400A0018) Timer Counter register          */
        TVUInt32  iTC1;                   /*!< (@ 0x400A001C) Timer Counter register          */
        TVUInt32  iTC2;                   /*!< (@ 0x400A0020) Timer Counter register          */
        TVUInt32  iLIM0;                  /*!< (@ 0x400A0024) Limit register                  */
        TVUInt32  iLIM1;                  /*!< (@ 0x400A0028) Limit register                  */
        TVUInt32  iLIM2;                  /*!< (@ 0x400A002C) Limit register                  */
        TVUInt32  iMAT0;                  /*!< (@ 0x400A0030) Match register                  */
        TVUInt32  iMAT1;                  /*!< (@ 0x400A0034) Match register                  */
        TVUInt32  iMAT2;                  /*!< (@ 0x400A0038) Match register                  */
        TVUInt32  iDT;                    /*!< (@ 0x400A003C) Dead time register              */
        TVUInt32  iCCP;                   /*!< (@ 0x400A0040) Communication Pattern register  */
        TVUInt32  iCAP0;                  /*!< (@ 0x400A0044) Capture register                */
        TVUInt32  iCAP1;                  /*!< (@ 0x400A0048) Capture register                */
        TVUInt32  iCAP2;                  /*!< (@ 0x400A004C) Capture register                */
        TVUInt32  iINTEN;                 /*!< (@ 0x400A0050) Interrupt Enable read address   */
        TVUInt32  iINTEN_SET;             /*!< (@ 0x400A0054) Interrupt Enable set address    */
        TVUInt32  iINTEN_CLR;             /*!< (@ 0x400A0058) Interrupt Enable clear address  */
        TVUInt32  iCNTCON;                /*!< (@ 0x400A005C) Count Control read address      */
        TVUInt32  iCNTCON_SET;            /*!< (@ 0x400A0060) Count Control set address       */
        TVUInt32  iCNTCON_CLR;            /*!< (@ 0x400A0064) Count Control clear address     */
        TVUInt32  iINTF;                  /*!< (@ 0x400A0068) Interrupt flags read address    */
        TVUInt32  iINTF_SET;              /*!< (@ 0x400A006C) Interrupt flags set address     */
        TVUInt32  iINTF_CLR;              /*!< (@ 0x400A0070) Interrupt flags clear address   */
        TVUInt32  iCAP_CLR;               /*!< (@ 0x400A0074) Capture clear address           */
} T_LPC546xx_PWM_Registers;

typedef struct {
    const HAL_PWM *iHAL;
    T_LPC546xx_PWM_Registers *iReg;
    TUInt32 iPeriod;
    TUInt32 iPWM;
} T_LPC546xx_PWM_Workspace;

static T_uezError LPC546xx_PWM_SetMatchRegister(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop);

static void ISetMatchReg(
        T_LPC546xx_PWM_Registers *r,
        TUInt8 aMatchReg,
        TUInt32 aValue)
{
    // Set the toggle point for this pin
    switch (aMatchReg) {
        case 0:
            r->iMAT0 = aValue;
            break;
        case 1:
            r->iMAT1 = aValue;
            break;
        case 2:
            r->iMAT2 = aValue;
            break;
    }
}

static void ISetLimitReg(
        T_LPC546xx_PWM_Registers *r,
        TUInt8 aMatchReg,
        TUInt32 aValue)
{
    // Set the toggle point for this pin
    switch (aMatchReg) {
        case 0:
            r->iLIM0 = aValue;
            break;
        case 1:
            r->iLIM1 = aValue;
            break;
        case 2:
            r->iLIM2 = aValue;
            break;
    }
}

static void IResetCounter(
        T_LPC546xx_PWM_Registers *r,
        TUInt8 aMatchReg)
{
    // Set the toggle point for this pin
    switch (aMatchReg) {
        case 0:
            r->iTC0 = 0;
            break;
        case 1:
            r->iTC1 = 0;
            break;
        case 2:
            r->iTC2 = 0;
            break;
    }
}
/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_PWM_SetMaster
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM is going to start -- chip select enabled for device
 * Outputs:
 *      void *aWorkspace        -- Workspace for PWM
 *      TUInt32 aPeriod         -- Number of Fpclk intervals per period
 *      TUInt8 aMatchRegister   -- Which register is used for master counter
 *---------------------------------------------------------------------------*/
static T_uezError LPC546xx_PWM_SetMaster(
        void *aWorkspace,
        TUInt32 aPeriod,
        TUInt8 aMatchRegister)
{
    T_LPC546xx_PWM_Workspace *p = (T_LPC546xx_PWM_Workspace *)aWorkspace;

    // Stop the timer, otherwise we cannot change it
    p->iReg->iCON_CLR = (1 << (0 + (p->iPWM * 8)));


    // Set the master match register
    ISetLimitReg(p->iReg, aMatchRegister, aPeriod);

    // Reset the counter
    IResetCounter(p->iReg, aMatchRegister);

    p->iReg->iCON_SET = (1 << (1 + (p->iPWM * 8)));
    p->iReg->iCON_SET = (1 << (2 + (p->iPWM * 8)));

    // Let the counter run
    p->iReg->iCON_SET = (1 << (0 + (p->iPWM * 8)));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_PWM_SetMatchRegister
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
 *                                  (NOTE: not implemented)
 *      TBool aDoStop           -- ETrue if counter is to be stopped
 *                                  when match occurs.
 *                                  (NOTE: not implemented)
 *---------------------------------------------------------------------------*/
static T_uezError LPC546xx_PWM_SetMatchRegister(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        TUInt32 aMatchPoint,
        TBool aDoInterrupt,
        TBool aDoCounterReset,
        TBool aDoStop)
{
    T_LPC546xx_PWM_Workspace *p = (T_LPC546xx_PWM_Workspace *)aWorkspace;
    T_LPC546xx_PWM_Registers *r = p->iReg;

    if(aDoInterrupt){
        p->iReg->iINTEN_SET = (1 << (1 + (p->iPWM * 4)));
    } else {
        p->iReg->iINTEN_CLR = (1 << (1 + (p->iPWM * 4)));
    }

    // Set the condition
    p->iReg->iCAPCON_SET = (1 << (18 + (p->iPWM)));

    ISetMatchReg(r, aMatchRegister, aMatchPoint);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_PWM_EnableSingleEdgeOutput
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
static T_uezError LPC546xx_PWM_EnableSingleEdgeOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_LPC546xx_PWM_Workspace *p = (T_LPC546xx_PWM_Workspace *)aWorkspace;

    // Set to single output mode
    p->iReg->iCON_CLR = (1 << (1 + (p->iPWM * 8)));

    // Enable output
    p->iReg->iCON_SET = (1 << (0 + (p->iPWM * 8)));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_PWM_DisableOutput
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
static T_uezError LPC546xx_PWM_DisableOutput(
        void *aWorkspace,
        TUInt8 aMatchRegister)
{
    T_LPC546xx_PWM_Workspace *p = (T_LPC546xx_PWM_Workspace *)aWorkspace;

    // Disable output
    p->iReg->iCON_CLR = (1 << (0 + (p->iPWM * 8)));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_PWM_SetMatchCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      PWM Match callback
 * Inputs:
 *      void *aWorkspace        -- PWM's workspace
 *      TUInt8 aMatchRegister   -- Point in Fpclk cycles where PWM switches
 * Outputs:
 *      T_uezError              -- UEZ_ERROR_NONE
 *---------------------------------------------------------------------------*/
static T_uezError LPC546xx_PWM_SetMatchCallback(
        void *aWorkspace,
        TUInt8 aMatchRegister,
        void (*aCallback)(void *),
        void *aCallbackWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_PWM_PWM0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx PWM0 workspace.
 * Inputs:
 *      void *aW                    -- Particular PWM workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_PWM_PWM0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_PWM_Workspace *p = (T_LPC546xx_PWM_Workspace *)aWorkspace;
    p->iReg = (T_LPC546xx_PWM_Registers *)LPC_MCPWM_BASE;

    LPC_CCU1->CLK_APB1_MOTOCONPWM_CFG = 0x07;

    p->iPWM = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_PWM_PWM1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx PWM0 workspace.
 * Inputs:
 *      void *aW                    -- Particular PWM workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_PWM_PWM1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_PWM_Workspace *p = (T_LPC546xx_PWM_Workspace *)aWorkspace;
    p->iReg = (T_LPC546xx_PWM_Registers *)LPC_MCPWM_BASE;

    LPC_CCU1->CLK_APB1_MOTOCONPWM_CFG = 0x07;

    p->iPWM = 1;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_PWM_PWM2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx PWM2 workspace.
 * Inputs:
 *      void *aW                    -- Particular PWM workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_PWM_PWM2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_PWM_Workspace *p = (T_LPC546xx_PWM_Workspace *)aWorkspace;
    p->iReg = (T_LPC546xx_PWM_Registers *)LPC_MCPWM_BASE;

    LPC_CCU1->CLK_APB1_MOTOCONPWM_CFG = 0x07;

    p->iPWM = 2;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_PWM PWM_LPC546xx_Port0_Interface = {
    {
    "NXP:LPC546xx:PWM0",
    0x0100,
    LPC546xx_PWM_PWM0_InitializeWorkspace,
    sizeof(T_LPC546xx_PWM_Workspace),
    },

    LPC546xx_PWM_SetMaster,
    LPC546xx_PWM_SetMatchRegister,
    LPC546xx_PWM_EnableSingleEdgeOutput,
    LPC546xx_PWM_DisableOutput,
    LPC546xx_PWM_SetMatchCallback,
};

const HAL_PWM PWM_LPC546xx_Port1_Interface = {
    {
    "NXP:LPC546xx:PWM1",
    0x0100,
    LPC546xx_PWM_PWM1_InitializeWorkspace,
    sizeof(T_LPC546xx_PWM_Workspace),
    },

    LPC546xx_PWM_SetMaster,
    LPC546xx_PWM_SetMatchRegister,
    LPC546xx_PWM_EnableSingleEdgeOutput,
    LPC546xx_PWM_DisableOutput,
    LPC546xx_PWM_SetMatchCallback,
};

const HAL_PWM PWM_LPC546xx_Port2_Interface = {
    {
    "NXP:LPC546xx:PWM2",
    0x0100,
    LPC546xx_PWM_PWM2_InitializeWorkspace,
    sizeof(T_LPC546xx_PWM_Workspace),
    },

    LPC546xx_PWM_SetMaster,
    LPC546xx_PWM_SetMatchRegister,
    LPC546xx_PWM_EnableSingleEdgeOutput,
    LPC546xx_PWM_DisableOutput,
    LPC546xx_PWM_SetMatchCallback,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_PWM0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    // Register PWM0
    HALInterfaceRegister("PWM0",
            (T_halInterface *)&PWM_LPC546xx_Port0_Interface, 0, 0);

}

void LPC546xx_PWM1_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    // Register PWM1
    HALInterfaceRegister("PWM1",
            (T_halInterface *)&PWM_LPC546xx_Port1_Interface, 0, 0);

}

void LPC546xx_PWM2_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    // Register PWM2
    HALInterfaceRegister("PWM2",
            (T_halInterface *)&PWM_LPC546xx_Port2_Interface, 0, 0);

}
void LPC546xx_PWM0_A_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC546xx_SCU_ConfigList pins[] = {
            {GPIO_P2_0,  SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P4_15, SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC546xx_PWM0_Require();
    LPC546xx_SCU_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC546xx_PWM0_B_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC546xx_SCU_ConfigList pins[] = {
            {GPIO_P2_13,  SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P5_17, SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC546xx_PWM0_Require();
    LPC546xx_SCU_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC546xx_PWM1_A_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC546xx_SCU_ConfigList pins[] = {
            {GPIO_P2_14,  SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P5_18, SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC546xx_PWM1_Require();
    LPC546xx_SCU_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC546xx_PWM1_B_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC546xx_SCU_ConfigList pins[] = {
            {GPIO_P2_15,  SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P4_11, SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC546xx_PWM1_Require();
    LPC546xx_SCU_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC546xx_PWM2_A_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC546xx_SCU_ConfigList pins[] = {
            {GPIO_P2_7,  SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P4_13, SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC546xx_PWM2_Require();
    LPC546xx_SCU_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}

void LPC546xx_PWM2_B_Require(T_uezGPIOPortPin aPortPin)
{
    static const T_LPC546xx_SCU_ConfigList pins[] = {
            {GPIO_P2_9,  SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P4_14, SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC546xx_PWM2_Require();
    LPC546xx_SCU_ConfigPin(aPortPin, pins, ARRAY_COUNT(pins));
}


/*-------------------------------------------------------------------------*
 * End of File:  PWM.c
 *-------------------------------------------------------------------------*/

