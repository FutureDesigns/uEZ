/*-------------------------------------------------------------------------*
 * File:  ADC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC17xx_40xx ADC Bus Interface.
 * Implementation:
 *      A single ADC0 is created, but the code can be easily changed for
 *      other processors to use multiple ADC busses.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
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
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_ADCBank.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_GPIO.h>
#include "LPC17xx_40xx_UtilityFuncs.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iCR; // 0x00 - A/D Control Register
    TVUInt32 iGDR; // 0x04 - A/D Global Data Register
    TVUInt32 reserved; // 0x08
    TVUInt32 iINTEN; // 0x0C - A/D Interrupt Enable
    TVUInt32 iDR[8]; // 0x10-0x2C - A/D Channel 0 through 7 Data Registers
    TVUInt32 iSTAT; // 0x30 - A/D Status Register
    TVUInt32 iADTRM; // 0x34 -
} T_LPC17xx_40xx_ADC_Registers;

typedef struct {
    const HAL_ADCBank *iHAL;
    T_LPC17xx_40xx_ADC_Registers *iReg;
    ADC_RequestSingle iRequest; // Current request settings
    void *iCallbackWorkspace;
    const T_adcBankCallbackInterface *iCallbackAPI;
    TUInt32 iInterruptChannel;
} T_LPC17xx_40xx_ADC_Workspace;

#define IOCON_A_AD(func) \
    IOCON_A(func, IOCON_NO_PULL, IOCON_HYS_DISABLE, IOCON_INVERT_OFF, \
            IOCON_ANALOG, IOCON_FILTER_OFF, IOCON_PUSH_PULL, IOCON_DAC_DISABLE)

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_LPC17xx_40xx_ADC_Workspace *G_adc0Workspace = 0;

/*---------------------------------------------------------------------------*
 * Routine:  ILPC17xx_40xx_ADC_ADC0_ISR
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC17xx_40xx ADC0 workspace.
 * Inputs:
 *      T_LPC17xx_40xx_ADC_Workspace *aW -- ADC Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void ILPC17xx_40xx_ADC_ADC0_ISR(T_LPC17xx_40xx_ADC_Workspace *p)
{
    TVUInt32 *p_dregs;
//    TVUInt32 stats;
    TUInt32 gdr;
    TUInt32 channel;
    TUInt32 v;

    // We got a group of readings.  Copy over the data
    gdr = p->iReg->iGDR;
    channel = (gdr >> 24) & 0x7;
    p_dregs = p->iReg->iDR;

    // The bit size is always 12 bits from the LPC17xx_40xx, but
    // raised by 4 bites of zeros, so it's like a 16 bit reading.
    // What we do here is shift the value up or down based on the
    // requested bit size
    v = p_dregs[channel] & 0xFFF0;

    // Shift up or down depending on requested bit size
    if (p->iRequest.iBitSampleSize < 16) {
        v >>= (16 - p->iRequest.iBitSampleSize);
    } else {
        v <<= (p->iRequest.iBitSampleSize - 16);
    }
    *p->iRequest.iCapturedData = v;

//    stats = p->iReg->iSTAT;
//    stats++;

    // Tell the callback routine the data is ready
    p->iCallbackAPI->CaptureComplete(p->iCallbackWorkspace, &p->iRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  IADCConfig
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC for reading but don't start it
 * Outputs:
 *      T_LPC17xx_40xx_ADC_Workspace *aW -- Particular ADC workspace
 *      ADC_RequestSingle *aRequest -- Configuration of ADC device
 *---------------------------------------------------------------------------*/
static void IADCConfig(T_LPC17xx_40xx_ADC_Workspace *p, ADC_RequestSingle *aRequest)
{

    // Ensure the power is on
    LPC17xx_40xxPowerOn(1 << 12);

    // Setup control register
    p->iReg->iCR =
    // Which channels to activate
            (1 << aRequest->iADCChannel) |
            // Rate of sampling clock.  Less than or equal to 12.4 MHz
                    (((((PCLK_FREQUENCY - 1) / 12400000) + 1) & 0x7F) << 8) |
            // Continuous readings or just one?  Just one
                    0 |
            // Number of bits per sample (less bits equals faster sampling)
                    // operational, not powerdown
                    (1 << 21) |
            // When to start (mapping is one to one) ... for now, force no triggering
                    (0 << 24) | ((aRequest->iTriggerOnFallingEdge) ? (1 << 27)
                    : 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  ADC_LPC17xx_40xx_Request
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
T_uezError ADC_LPC17xx_40xx_RequestSingle(
        void *aWorkspace,
        ADC_RequestSingle *aRequest)
{
    T_LPC17xx_40xx_ADC_Workspace *p = (T_LPC17xx_40xx_ADC_Workspace *)aWorkspace;

    // Ensure the interrupt is disabled
    InterruptDisable(ADC_IRQn);

    // Transfer the configuration information to the ADC
    p->iRequest = *aRequest;

    // Setup the hardware for the reading type (but don't start)
    IADCConfig(p, &p->iRequest);

    // At this point, nothing is enabled.
    // Are we starting or stopping?
    if (aRequest->iTrigger != ADC_TRIGGER_STOP) {
        // Setup the interrupts for these
        p->iReg->iINTEN = (1 << aRequest->iADCChannel);

        // Allow interrupt to be enabled (but it won't at this point)
        InterruptEnable(ADC_IRQn);

        // Start trigger (by filling in the trigger type)
        p->iReg->iCR |= (aRequest->iTrigger << 24);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ILPC17xx_40xx_ADC_ADC0_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch ADC0 interrupt and use the ISR routine for it
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC17xx_40xx_ADC_ADC0_Interrupt)
{
    IRQ_START();
    ILPC17xx_40xx_ADC_ADC0_ISR(G_adc0Workspace);
    IRQ_END()
    ;
}

T_uezError ADC_LPC17xx_40xx_Configure_ADC0(
        void *aWorkspace,
        void *aCallbackWorkspace,
        const T_adcBankCallbackInterface * const aCallbackAPI)
{
    T_LPC17xx_40xx_ADC_Workspace *p = (T_LPC17xx_40xx_ADC_Workspace *)aWorkspace;
    p->iCallbackWorkspace = aCallbackWorkspace;
    p->iCallbackAPI = aCallbackAPI;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_ADC_ADC0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC17xx_40xx ADC0 workspace.
 * Inputs:
 *      void *aW                    -- Particular ADC workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_ADC_ADC0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC17xx_40xx_ADC_Workspace *p = (T_LPC17xx_40xx_ADC_Workspace *)aWorkspace;
    p->iReg = (T_LPC17xx_40xx_ADC_Registers *)LPC_ADC_BASE;
    G_adc0Workspace = p;
    p->iInterruptChannel = ADC_IRQn;

    // Register an interrupt for this ADC but don't start it.
    InterruptRegister(ADC_IRQn, ILPC17xx_40xx_ADC_ADC0_Interrupt,
            INTERRUPT_PRIORITY_NORMAL, "ADC0");

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_ADCBank ADC_LPC17xx_40xx_ADC0_Interface = {
        {
        "ADC:LPC17xx_40xx:ADC0",
        0x0100,
        LPC17xx_40xx_ADC_ADC0_InitializeWorkspace,
        sizeof(T_LPC17xx_40xx_ADC_Workspace),
        },

        ADC_LPC17xx_40xx_Configure_ADC0,
        ADC_LPC17xx_40xx_RequestSingle, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_ADC0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("ADC0", (T_halInterface *)&ADC_LPC17xx_40xx_ADC0_Interface,
            0, 0);
}

void LPC17xx_40xx_ADC0_0_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList adc0_0[] = {
            {GPIO_P0_23, IOCON_A_AD(1)}, // ADC0.0
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_ADC0_Require();
    LPC17xx_40xx_IOCON_ConfigPin(aPin, adc0_0, ARRAY_COUNT(adc0_0));
}
void LPC17xx_40xx_ADC0_1_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList adc0_1[] = {
            {GPIO_P0_24, IOCON_A_AD(1)}, // ADC0.1
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_ADC0_Require();
    LPC17xx_40xx_IOCON_ConfigPin(aPin, adc0_1, ARRAY_COUNT(adc0_1));
}
void LPC17xx_40xx_ADC0_2_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList adc0_2[] = {
            {GPIO_P0_25, IOCON_A_AD(1)}, // ADC0.2
    };

    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_ADC0_Require();
    LPC17xx_40xx_IOCON_ConfigPin(aPin, adc0_2, ARRAY_COUNT(adc0_2));
}
void LPC17xx_40xx_ADC0_3_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList adc0_3[] = {
            {GPIO_P0_26, IOCON_A_AD(1)}, // ADC0.3
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_ADC0_Require();
    LPC17xx_40xx_IOCON_ConfigPin(aPin, adc0_3, ARRAY_COUNT(adc0_3));
}
void LPC17xx_40xx_ADC0_4_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList adc0_4[] = {
            {GPIO_P1_30, IOCON_A_AD(3)}, // ADC0.4
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_GPIO1_Require();
    LPC17xx_40xx_ADC0_Require();
    LPC17xx_40xx_IOCON_ConfigPin(aPin, adc0_4, ARRAY_COUNT(adc0_4));
}
void LPC17xx_40xx_ADC0_5_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList adc0_5[] = {
            {GPIO_P1_31, IOCON_A_AD(3)}, // ADC0.5
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_GPIO1_Require();
    LPC17xx_40xx_ADC0_Require();
    LPC17xx_40xx_IOCON_ConfigPin(aPin, adc0_5, ARRAY_COUNT(adc0_5));
}
void LPC17xx_40xx_ADC0_6_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList adc0_6[] = {
            {GPIO_P0_12, IOCON_A_AD(3)}, // ADC0.6
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_ADC0_Require();
    LPC17xx_40xx_IOCON_ConfigPin(aPin, adc0_6, ARRAY_COUNT(adc0_6));
}
void LPC17xx_40xx_ADC0_7_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList adc0_7[] = {
            {GPIO_P0_13, IOCON_A_AD(3)}, // ADC0.7
    };
    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_ADC0_Require();
    LPC17xx_40xx_IOCON_ConfigPin(aPin, adc0_7, ARRAY_COUNT(adc0_7));
}

/*-------------------------------------------------------------------------*
 * End of File:  ADC.c
 *-------------------------------------------------------------------------*/

