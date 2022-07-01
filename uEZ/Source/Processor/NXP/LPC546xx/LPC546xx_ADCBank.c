/*-------------------------------------------------------------------------*
 * File:  ADC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC546xx ADC Bus Interface.
 * Implementation:
 *      A single ADC0 is created, but the code can be easily changed for
 *      other processors to use multiple ADC busses.
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

#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <HAL/GPIO.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_ADCBank.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_GPIO.h>

//TODO: remove
#include "iar/Include/CMSIS/LPC54608.h"

//TODO: Clean up
/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iCTRL;
    TVUInt32 iINSEL;
    TVUInt32 iSEQ_CTRL[2];
    TVUInt32 iSEQ_GDAT[2];
    TVUInt8 iRESERVED_0[8];
    TVUInt32 iDAT[12];
    TVUInt32 iTHR0_LOW;
    TVUInt32 iTHR1_LOW;
    TVUInt32 iTHR0_HIGH;
    TVUInt32 iTHR1_HIGH;
    TVUInt32 iCHAN_THRSEL;
    TVUInt32 iINTEN;
    TVUInt32 iFLAGS;
    TVUInt32 iSTARTUP;
    TVUInt32 iCALIB;
} T_LPC546xx_ADC_Registers;

typedef struct {
    const HAL_ADCBank *iHAL;
    T_LPC546xx_ADC_Registers *iReg;
    ADC_RequestSingle iRequest; // Current request settings
    void *iCallbackWorkspace;
    const T_adcBankCallbackInterface *iCallbackAPI;
    TUInt32 iInterruptChannel;
} T_LPC546xx_ADC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LPC546xx_ADC_Workspace *G_adc0Workspace = 0;
static TUInt32 G_ADC_CLK_Hz = 0;

/*---------------------------------------------------------------------------*
 * Routine:  ILPC546xx_ADC_ADC0_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx ADC0 workspace.
 * Inputs:
 *      T_LPC546xx_ADC_Workspace *aW -- ADC Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void ILPC546xx_ADC_ADC_ISR(T_LPC546xx_ADC_Workspace *p)
{
    TUInt32 v;

    v = p->iReg->iDAT[p->iRequest.iADCChannel];

    *p->iRequest.iCapturedData = (v >> 4) & 0xFF;

    p->iReg->iFLAGS = (1<<28);

    // Tell the callback routine the data is ready
    p->iCallbackAPI->CaptureComplete(p->iCallbackWorkspace, &p->iRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  IADCConfig
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC for reading but don't start it
 * Outputs:
 *      T_LPC546xx_ADC_Workspace *aW -- Particular ADC workspace
 *      ADC_RequestSingle *aRequest -- Configuration of ADC device
 *---------------------------------------------------------------------------*/
static void IADCConfig(T_LPC546xx_ADC_Workspace *p, ADC_RequestSingle *aRequest)
{
    T_LPC546xx_ADC_Registers *reg = p->iReg;
    TUInt32 temp = 1; //Default to ClockSynchronousMode, div 2

    reg->iINTEN = 0x00; //Disable Interrupts

    //Expecting the ADC clock source to be configured below 80MHz in the create function

    switch(aRequest->iBitSampleSize){
        case 12:
            temp |= ADC_CTRL_RESOL(3);
            break;
        case 10:
            temp |= ADC_CTRL_RESOL(2);
            break;
        case 8:
            temp |= ADC_CTRL_RESOL(1);
            break;
        case 6:
            temp |= ADC_CTRL_RESOL(0);
            break;
        default:
            temp |= ADC_CTRL_RESOL(2);//Default to 10
            break;
    }

    reg->iCTRL = temp;
}

/*---------------------------------------------------------------------------*
 * Routine:  ADC_LPC546xx_Request
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a ADC request either doing a single triggered request and stopping,
 *      doing continuous readings, or stop a previously started request.
 * Inputs:
 *      void *aW                    -- Particular ADC workspace
 *      ADC_RequestSingle *aRequest -- Request to perform
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ADC_LPC546xx_RequestSingle(
        void *aWorkspace,
        ADC_RequestSingle *aRequest)
{
    T_LPC546xx_ADC_Workspace *p = (T_LPC546xx_ADC_Workspace *)aWorkspace;
    TUInt32 temp;

    // Ensure the interrupt is disabled
    InterruptDisable(p->iInterruptChannel);

    // Transfer the configuration information to the ADC
    p->iRequest = *aRequest;

    // Setup the hardware for the reading type (but don't start)
    IADCConfig(p, &p->iRequest);

    // At this point, nothing is enabled.
    // Are we starting or stopping?
    if (aRequest->iTrigger != ADC_TRIGGER_STOP) {
        temp =  (1<<p->iRequest.iADCChannel);

        temp |= ADC_SEQ_CTRL_MODE_MASK;

        //Setup the SEQA CTRL
        p->iReg->iSEQ_CTRL[0] = temp;

        p->iReg->iSEQ_CTRL[0] |= (TUInt32)(1<<31); //Enable

        p->iReg->iINTEN = (1 << 0); //Using only SEQA

        // Allow interrupt to be enabled (but it won't at this point)
        InterruptEnable(p->iInterruptChannel);

        UEZBSPDelay1MS();
        p->iReg->iSEQ_CTRL[0] |= ADC_SEQ_CTRL_START_MASK;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ILPC546xx_ADC_ADC0_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch ADC0 interrupt and use the ISR routine for it
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC546xx_ADC_ADC0_Interrupt)
{
    IRQ_START();
    ILPC546xx_ADC_ADC_ISR(G_adc0Workspace);
    IRQ_END();
}

T_uezError ADC_LPC546xx_Configure_ADC0(
        void *aWorkspace,
        void *aCallbackWorkspace,
        const T_adcBankCallbackInterface * const aCallbackAPI)
{
    T_LPC546xx_ADC_Workspace *p = (T_LPC546xx_ADC_Workspace *)aWorkspace;
    p->iCallbackWorkspace = aCallbackWorkspace;
    p->iCallbackAPI = aCallbackAPI;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_ADC_ADC0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx ADC0 workspace.
 * Inputs:
 *      void *aW                    -- Particular ADC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_ADC_ADC0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_ADC_Workspace *p = (T_LPC546xx_ADC_Workspace *)aWorkspace;
    p->iReg = (T_LPC546xx_ADC_Registers *)ADC0_BASE;
    G_adc0Workspace = p;
    p->iInterruptChannel = ADC0_SEQA_IRQn;

    // Register an interrupt for this ADC but don't start it.
    InterruptRegister(p->iInterruptChannel, ILPC546xx_ADC_ADC0_Interrupt,
            INTERRUPT_PRIORITY_NORMAL, "ADC0");

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_ADCBank ADC_LPC546xx_ADC0_Interface = {
        {
        "ADC:LPC546xx:ADC0",
        0x0100,
        LPC546xx_ADC_ADC0_InitializeWorkspace,
        sizeof(T_LPC546xx_ADC_Workspace),
        },

        ADC_LPC546xx_Configure_ADC0,
        ADC_LPC546xx_RequestSingle, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_ADC0_Require(void)
{
    T_LPC546xx_ADC_Workspace *p;
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("ADC0", (T_halInterface *)&ADC_LPC546xx_ADC0_Interface,
            0, (T_halWorkspace **)&p);

    LPC546xxPowerOn(kCLOCK_Adc0);
    POWER_DisablePD(kPDRUNCFG_PD_VDDA);    /* Power on VDDA. */
    POWER_DisablePD(kPDRUNCFG_PD_ADC0);    /* Power on the ADC converter. */
    POWER_DisablePD(kPDRUNCFG_PD_VD2_ANA); /* Power on the analog power supply. */
    POWER_DisablePD(kPDRUNCFG_PD_VREFP);   /* Power on the reference voltage source. */

    if(((SYSCON->MAINCLKSELB & 0x3) == 0) && ((SYSCON->MAINCLKSELA & 0x03) == 0x03)){
        SYSCON->ADCCLKSEL = 0x00;
        if(PROCESSOR_OSCILLATOR_FREQUENCY == 96000000){
            SYSCON->ADCCLKDIV = 1;
            G_ADC_CLK_Hz = PROCESSOR_OSCILLATOR_FREQUENCY / 2;
        } else {
            SYSCON->ADCCLKDIV = 0;
            G_ADC_CLK_Hz = PROCESSOR_OSCILLATOR_FREQUENCY; //Get below 80MHZ
        }
    } else if(((SYSCON->MAINCLKSELB & 0x3) == 0x02)){
        SYSCON->ADCCLKSEL = 0x01; //Main PLL
        if(PROCESSOR_OSCILLATOR_FREQUENCY <= 80000000){
            SYSCON->ADCCLKDIV = 0;
            G_ADC_CLK_Hz = PROCESSOR_OSCILLATOR_FREQUENCY;
        } else if(PROCESSOR_OSCILLATOR_FREQUENCY <= 160000000){
            SYSCON->ADCCLKDIV = 1;
            G_ADC_CLK_Hz = PROCESSOR_OSCILLATOR_FREQUENCY/2;
        } else {
            SYSCON->ADCCLKDIV = 2;
            G_ADC_CLK_Hz = PROCESSOR_OSCILLATOR_FREQUENCY/3;
        }

    } else {
        UEZFailureMsg("Unknown clock configuration for ADC!");
    }
    
#if 1
    uint32_t i;

    /* Enable the converter. */
    /* This bit acn only be set 1 by software. It is cleared automatically whenever the ADC is powered down.
       This bit should be set after at least 10 ms after the ADC is powered on. */
    p->iReg->iSTARTUP = ADC_STARTUP_ADC_ENA_MASK;
    for (i = 0U; i < 0x10; i++) /* Wait a few clocks to startup up. */
    {
        __ASM("NOP");
    }
    if (!(p->iReg->iSTARTUP & ADC_STARTUP_ADC_ENA_MASK))
    {
        //return false; /* ADC is not powered up. */
    }

    /* If not in by-pass mode, do the calibration. */
    if ((ADC_CALIB_CALREQD_MASK == (p->iReg->iCALIB & ADC_CALIB_CALREQD_MASK)) &&
        (0U == (p->iReg->iCTRL & ADC_CTRL_BYPASSCAL_MASK)))
    {
        /* Calibration is needed, do it now. */
        p->iReg->iCALIB = ADC_CALIB_CALIB_MASK;
        i = 0xF0000;
        while ((ADC_CALIB_CALIB_MASK == (p->iReg->iCALIB & ADC_CALIB_CALIB_MASK)) && (--i))
        {
        }
        if (i == 0U)
        {
            //return false; /* Calibration timeout. */
        }
    }

    /* A dummy conversion cycle will be performed. */
    p->iReg->iSTARTUP |= ADC_STARTUP_ADC_INIT_MASK;
    i = 0x7FFFF;
    while ((ADC_STARTUP_ADC_INIT_MASK == (p->iReg->iSTARTUP & ADC_STARTUP_ADC_INIT_MASK)) && (--i))
    {
    }
    if (i == 0U)
    {
        //return false;
    }

   //return true;
#endif
}

#define ICON_A_AM(n)        (IOCON_A(n, IOCON_NO_PULL, \
                                        IOCON_INVERT_DISABLE, \
                                        IOCON_ANALOG, \
                                        IOCON_FILTER_ON, \
                                        IOCON_NORMAL))

void LPC546xx_ADC0_0_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_0[] = {
            {GPIO_P0_10, ICON_A_AM(0)}, // ADC0.0
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO2_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_0, ARRAY_COUNT(adc0_0));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 0);
    }
}
void LPC546xx_ADC0_1_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_1[] = {
            {GPIO_P0_11, ICON_A_AM(0)}, // ADC0.1
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO2_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_1, ARRAY_COUNT(adc0_1));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 1);
    }
}
void LPC546xx_ADC0_2_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_2[] = {
            {GPIO_P0_12, ICON_A_AM(4)}, // ADC0.2
    };

    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO0_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_2, ARRAY_COUNT(adc0_2));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 2);
    }
}
void LPC546xx_ADC0_3_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_3[] = {
            {GPIO_P0_15, ICON_A_AM(0)}, // ADC0.3
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO0_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_3, ARRAY_COUNT(adc0_3));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 3);
    }
}
void LPC546xx_ADC0_4_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_4[] = {
            {GPIO_P0_16, ICON_A_AM(0)}, // ADC0.4
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO0_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_4, ARRAY_COUNT(adc0_4));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 4);
    }
}
void LPC546xx_ADC0_5_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_5[] = {
            {GPIO_P0_31, ICON_A_AM(0)}, // ADC0.5
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO0_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_5, ARRAY_COUNT(adc0_5));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 5);
    }
}
void LPC546xx_ADC0_6_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_6[] = {
            {GPIO_P1_0, ICON_A_AM(0)}, // ADC0.6
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO1_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_6, ARRAY_COUNT(adc0_6));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 6);
    }
}
void LPC546xx_ADC0_7_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_7[] = {
            {GPIO_P2_0, ICON_A_AM(0)}, // ADC0.7
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO2_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_7, ARRAY_COUNT(adc0_7));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 6);
    }
}

void LPC546xx_ADC0_8_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_8[] = {
            {GPIO_P2_1, ICON_A_AM(0)}, // ADC0.8
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO2_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_8, ARRAY_COUNT(adc0_8));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 6);
    }
}

void LPC546xx_ADC0_9_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_9[] = {
            {GPIO_P3_21, ICON_A_AM(0)}, // ADC0.9
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO3_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_9, ARRAY_COUNT(adc0_9));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 6);
    }
}

void LPC546xx_ADC0_10_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_10[] = {
            {GPIO_P3_22, ICON_A_AM(0)}, // ADC0.10
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO3_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_10, ARRAY_COUNT(adc0_10));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 6);
    }
}

void LPC546xx_ADC0_11_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_ICON_ConfigList adc0_11[] = {
            {GPIO_P0_23, ICON_A_AM(0)}, // ADC0.11
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO0_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_ICON_ConfigPinOrNone(aPin, adc0_11, ARRAY_COUNT(adc0_11));
    if(aPin != GPIO_NONE){
        //LPC_SCU->ENAIO0 |= (1 << 6);
    }
}


/*-------------------------------------------------------------------------*
 * End of File:  ADC.c
 *-------------------------------------------------------------------------*/

