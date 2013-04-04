/*-------------------------------------------------------------------------*
 * File:  ADC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC2478 ADC Bus Interface.
 * Implementation:
 *      A single ADC0 is created, but the code can be easily changed for
 *      other processors to use multiple ADC busses.
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
#include <uEZ.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC2478/LPC2478_ADCBank.h>
#include <Source/Processor/NXP/LPC2478/LPC2478_GPIO.h>
#include <HAL/Interrupt.h>
#include <uEZPlatform.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iCR;       // 0x00 - A/D Control Register
    TVUInt32 iGDR;      // 0x04 - A/D Global Data Register
    TVUInt32 reserved;  // 0x08
    TVUInt32 iINTEN;    // 0x0C - A/D Interrupt Enable
    TVUInt32 iDR[8];    // 0x10-0x2C - A/D Channel 0 through 7 Data Registers
    TVUInt32 iSTAT;     // 0x30 - A/D Status Register
} T_LPC2478_ADC_Registers;

typedef struct {
    const HAL_ADCBank *iHAL;
    T_LPC2478_ADC_Registers *iReg;
    ADC_RequestSingle iRequest; // Current request settings
    void *iCallbackWorkspace;
    const T_adcBankCallbackInterface *iCallbackAPI;
    TUInt32 iInterruptChannel;
} T_LPC2478_ADC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_LPC2478_ADC_Workspace *G_adc0Workspace = 0;

/*---------------------------------------------------------------------------*
 * Routine:  ILPC2478_ADC_ADC0_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 ADC0 workspace.
 * Inputs:
 *      T_LPC2478_ADC_Workspace *aW -- ADC Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void ILPC2478_ADC_ADC0_ISR(T_LPC2478_ADC_Workspace *p)
{
    TVUInt32 *p_dregs;
    #define ADC_TO_DATA(x)  (((x)>>6)&0x3FF)
    TVUInt32 stats;
    TUInt32 gdr;
    TUInt32 channel;

    // We got a group of readings.  Copy over the data
    gdr = p->iReg->iGDR;
    channel = (gdr>>24)&0x7;
    p_dregs = p->iReg->iDR;
    *p->iRequest.iCapturedData = (p_dregs[channel]>>6)&0x03FF;
    stats = p->iReg->iSTAT;

    // Tell the callback routine the data is ready
    p->iCallbackAPI->CaptureComplete(
            p->iCallbackWorkspace,
            &p->iRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  IADCConfig
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC for reading but don't start it
 * Outputs:
 *      T_LPC2478_ADC_Workspace *aW -- Particular ADC workspace
 *      ADC_RequestSingle *aRequest -- Configuration of ADC device
 *---------------------------------------------------------------------------*/
static void IADCConfig(
                T_LPC2478_ADC_Workspace *p,
                ADC_RequestSingle *aRequest)
{
    static const TUInt32 bitTable[16] = {
        0,
        0,
        0,
        (7<<17),    // 3 bits

        (6<<17),    // 4 bits
        (5<<17),    // 5 bits
        (4<<17),    // 6 bits
        (3<<17),    // 7 bits

        (2<<17),    // 8 bits
        (1<<17),    // 9 bits
        (0<<17),    // 10 bits
        0,

        0,
        0,
        0,
        0,
    };

    // Ensure the power is on
    PCONP |= (1<<PCONP_PCAD_BIT);

    // Setup control register
    p->iReg->iCR =
        // Which channels to activate
        (1<<aRequest->iADCChannel)|
        // Rate of sampling clock.  Aims to be 4.5 MHz
        ((((Fpclk / 4500000)+1)&0x7F)<<8)|
        // Continuous readings or just one?
        0|
        // Number of bits per sample (less bits equals faster sampling)
        (bitTable[aRequest->iBitSampleSize])|
        // operational, not powerdown
        (1<<21)|
        // When to start (mapping is one to one) ... for now, force no triggering
        (0 << 24)|
        ((aRequest->iTriggerOnFallingEdge)?(1<<27):0);
}

/*---------------------------------------------------------------------------*
 * Routine:  ADC_LPC2478_Request
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
T_uezError ADC_LPC2478_RequestSingle(void *aWorkspace, ADC_RequestSingle *aRequest)
{
    T_LPC2478_ADC_Workspace *p = (T_LPC2478_ADC_Workspace *)aWorkspace;

    // Ensure the interrupt is disabled
    InterruptDisable(INTERRUPT_CHANNEL_ADC0);

    // Transfer the configuration information to the ADC
    p->iRequest = *aRequest;

    // Setup the hardware for the reading type (but don't start)
    IADCConfig(p, &p->iRequest);

    // At this point, nothing is enabled.
    // Are we starting or stopping?
    if (aRequest->iTrigger != ADC_TRIGGER_STOP) {
        // Setup the interrupts for these
        p->iReg->iINTEN = (1<<aRequest->iADCChannel);

        // Allow interrupt to be enabled (but it won't at this point)
        InterruptEnable(INTERRUPT_CHANNEL_ADC0);

        // Start trigger (by filling in the trigger type)
        p->iReg->iCR |= (aRequest->iTrigger << 24);
    }

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ILPC2478_ADC_ADC0_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch ADC0 interrupt and use the ISR routine for it
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC2478_ADC_ADC0_Interrupt)
{
    IRQ_START();
    ILPC2478_ADC_ADC0_ISR(G_adc0Workspace);
    IRQ_END();
}

T_uezError ADC_LPC2478_Configure_ADC0(
            void *aWorkspace,
            void *aCallbackWorkspace,
            const T_adcBankCallbackInterface * const aCallbackAPI)
{
    T_LPC2478_ADC_Workspace *p = (T_LPC2478_ADC_Workspace *)aWorkspace;
    p->iCallbackWorkspace = aCallbackWorkspace;
    p->iCallbackAPI = aCallbackAPI;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_ADC_ADC0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 ADC0 workspace.
 * Inputs:
 *      void *aW                    -- Particular ADC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_ADC_ADC0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_ADC_Workspace *p = (T_LPC2478_ADC_Workspace *)aWorkspace;
    p->iReg = (T_LPC2478_ADC_Registers *)AD0_BASE;
    G_adc0Workspace = p;
    p->iInterruptChannel = INTERRUPT_CHANNEL_ADC0;

    // Register an interrupt for this ADC but don't start it.
    InterruptRegister(
        INTERRUPT_CHANNEL_ADC0,
        ILPC2478_ADC_ADC0_Interrupt,
        INTERRUPT_PRIORITY_NORMAL,
        "ADC0");

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC2478_ADC0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("ADC0", (T_halInterface *)&ADC_LPC2478_ADC0_Interface,
            0, 0);
}

void LPC2478_ADC0_0_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC2478_PINSEL_ConfigList adc0_0[] = {
            {GPIO_P0_23, 1}, // ADC0.0
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC2478_GPIO0_Require();
    LPC2478_ADC0_Require();
    LPC2478_PINSEL_ConfigPin(aPin, adc0_0, ARRAY_COUNT(adc0_0));
}
void LPC2478_ADC0_1_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC2478_PINSEL_ConfigList adc0_1[] = {
            {GPIO_P0_24, 1}, // ADC0.1
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC2478_GPIO0_Require();
    LPC2478_ADC0_Require();
    LPC2478_PINSEL_ConfigPin(aPin, adc0_1, ARRAY_COUNT(adc0_1));
}
void LPC2478_ADC0_2_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC2478_PINSEL_ConfigList adc0_2[] = {
            {GPIO_P0_25, 1}, // ADC0.2
    };

    HAL_DEVICE_REQUIRE_ONCE();
    LPC2478_GPIO0_Require();
    LPC2478_ADC0_Require();
    LPC2478_PINSEL_ConfigPin(aPin, adc0_2, ARRAY_COUNT(adc0_2));
}
void LPC2478_ADC0_3_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC2478_PINSEL_ConfigList adc0_3[] = {
            {GPIO_P0_26, 1}, // ADC0.3
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC2478_GPIO0_Require();
    LPC2478_ADC0_Require();
    LPC2478_PINSEL_ConfigPin(aPin, adc0_3, ARRAY_COUNT(adc0_3));
}
void LPC2478_ADC0_4_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC2478_PINSEL_ConfigList adc0_4[] = {
            {GPIO_P1_30, 3}, // ADC0.4
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC2478_GPIO1_Require();
    LPC2478_ADC0_Require();
    LPC2478_PINSEL_ConfigPin(aPin, adc0_4, ARRAY_COUNT(adc0_4));
}
void LPC2478_ADC0_5_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC2478_PINSEL_ConfigList adc0_5[] = {
            {GPIO_P1_31, 3}, // ADC0.5
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC2478_GPIO1_Require();
    LPC2478_ADC0_Require();
    LPC2478_PINSEL_ConfigPin(aPin, adc0_5, ARRAY_COUNT(adc0_5));
}
void LPC2478_ADC0_6_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC2478_PINSEL_ConfigList adc0_6[] = {
            {GPIO_P0_12, 3}, // ADC0.6
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC2478_GPIO0_Require();
    LPC2478_ADC0_Require();
    LPC2478_PINSEL_ConfigPin(aPin, adc0_6, ARRAY_COUNT(adc0_6));
}
void LPC2478_ADC0_7_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC2478_PINSEL_ConfigList adc0_7[] = {
            {GPIO_P0_13, 3}, // ADC0.7
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC2478_GPIO0_Require();
    LPC2478_ADC0_Require();
    LPC2478_PINSEL_ConfigPin(aPin, adc0_7, ARRAY_COUNT(adc0_7));
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_ADCBank ADC_LPC2478_ADC0_Interface = {
    "ADC:LPC2478:ADC0",
    0x0100,
    LPC2478_ADC_ADC0_InitializeWorkspace,
    sizeof(T_LPC2478_ADC_Workspace),

    ADC_LPC2478_Configure_ADC0,
    ADC_LPC2478_RequestSingle,
};

/*-------------------------------------------------------------------------*
 * End of File:  ADC.c
 *-------------------------------------------------------------------------*/

