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
} T_LPC546xx_ADC_Registers;

typedef struct {
    const HAL_ADCBank *iHAL;
    T_LPC546xx_ADC_Registers *iReg;
    ADC_RequestSingle iRequest; // Current request settings
    void *iCallbackWorkspace;
    const T_adcBankCallbackInterface *iCallbackAPI;
    TUInt32 iInterruptChannel;
} T_LPC546xx_ADC_Workspace;

#define IOCON_A_AD(func) \
    SCU_NORMAL_DRIVE(func, \
                    SCU_EPD_DISABLE, \
                    SCU_EPUN_DISABLE, \
                    SCU_EHS_SLOW, \
                    SCU_EZI_DISABLE, \
                    SCU_ZIF_ENABLE)

    // IOCON_A(func, IOCON_NO_PULL, IOCON_HYS_DISABLE, IOCON_INVERT_OFF, /
    //    IOCON_ANALOG, IOCON_FILTER_OFF, IOCON_PUSH_PULL, IOCON_DAC_DISABLE)
/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_LPC546xx_ADC_Workspace *G_adc0Workspace = 0;
T_LPC546xx_ADC_Workspace *G_adc1Workspace = 0;

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
    TVUInt32 *p_dregs;
//    TVUInt32 stats;
    TUInt32 gdr;
    TUInt32 channel;
    TUInt32 v;

    // We got a group of readings.  Copy over the data
    gdr = p->iReg->iGDR;
    channel = (gdr >> 24) & 0x7;
    p_dregs = p->iReg->iDR;

    // The bit size is always 12 bits from the LPC546xx, but
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
 *      T_LPC546xx_ADC_Workspace *aW -- Particular ADC workspace
 *      ADC_RequestSingle *aRequest -- Configuration of ADC device
 *---------------------------------------------------------------------------*/
static void IADCConfig(T_LPC546xx_ADC_Workspace *p, ADC_RequestSingle *aRequest)
{

    // Ensure the power is on
    LPC546xxPowerOn(1 << 12);

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

    // Ensure the interrupt is disabled
    InterruptDisable(p->iInterruptChannel);

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
        InterruptEnable(p->iInterruptChannel);

        // Start trigger (by filling in the trigger type)
        p->iReg->iCR |= (aRequest->iTrigger << 24);
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

/*---------------------------------------------------------------------------*
 * Interrupt Routine:  ILPC546xx_ADC_ADC1_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Catch ADC0 interrupt and use the ISR routine for it
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC546xx_ADC_ADC1_Interrupt)
{
    IRQ_START();
    ILPC546xx_ADC_ADC_ISR(G_adc1Workspace);
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
    p->iReg = (T_LPC546xx_ADC_Registers *)LPC_ADC0_BASE;
    G_adc0Workspace = p;
    p->iInterruptChannel = ADC0_IRQn;

    //Turn on peripheral clock
    LPC_CGU->BASE_VADC_CLK = (9<<24) | (1<<11) | (0<<0);

    // Register an interrupt for this ADC but don't start it.
    InterruptRegister(ADC0_IRQn, ILPC546xx_ADC_ADC0_Interrupt,
            INTERRUPT_PRIORITY_NORMAL, "ADC0");

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
T_uezError LPC546xx_ADC_ADC1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_ADC_Workspace *p = (T_LPC546xx_ADC_Workspace *)aWorkspace;
    p->iReg = (T_LPC546xx_ADC_Registers *)LPC_ADC1_BASE;
    G_adc1Workspace = p;
    p->iInterruptChannel = ADC1_IRQn;

    //Turn on peripheral clock
    LPC_CGU->BASE_VADC_CLK = (9<<24) | (1<<11) | (0<<0);

    // Register an interrupt for this ADC but don't start it.
    InterruptRegister(ADC1_IRQn, ILPC546xx_ADC_ADC1_Interrupt,
            INTERRUPT_PRIORITY_NORMAL, "ADC1");

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

const HAL_ADCBank ADC_LPC546xx_ADC1_Interface = {
        {
        "ADC:LPC546xx:ADC1",
        0x0100,
        LPC546xx_ADC_ADC1_InitializeWorkspace,
        sizeof(T_LPC546xx_ADC_Workspace),
        },

        ADC_LPC546xx_Configure_ADC0,
        ADC_LPC546xx_RequestSingle, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_ADC0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("ADC0", (T_halInterface *)&ADC_LPC546xx_ADC0_Interface,
            0, 0);
}

void LPC546xx_ADC1_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("ADC1", (T_halInterface *)&ADC_LPC546xx_ADC1_Interface,
            0, 0);
}

void LPC546xx_ADC0_0_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc0_0[] = {
            {GPIO_P2_3, IOCON_A_AD(0)}, // ADC0.0
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO2_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc0_0, ARRAY_COUNT(adc0_0));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO0 |= (1 << 0);
    }
}
void LPC546xx_ADC0_1_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc0_1[] = {
            {GPIO_P2_1, IOCON_A_AD(0)}, // ADC0.1
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO2_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc0_1, ARRAY_COUNT(adc0_1));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO0 |= (1 << 1);
    }
}
void LPC546xx_ADC0_2_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc0_2[] = {
            {GPIO_P7_22, IOCON_A_AD(4)}, // ADC0.2
    };

    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO7_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc0_2, ARRAY_COUNT(adc0_2));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO0 |= (1 << 2);
    }
}
void LPC546xx_ADC0_3_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc0_3[] = {
            {GPIO_P3_13, IOCON_A_AD(0)}, // ADC0.3
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO3_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc0_3, ARRAY_COUNT(adc0_3));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO0 |= (1 << 3);
    }
}
void LPC546xx_ADC0_4_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc0_4[] = {
            {GPIO_P3_12, IOCON_A_AD(0)}, // ADC0.4
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO3_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc0_4, ARRAY_COUNT(adc0_4));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO0 |= (1 << 4);
    }
}
void LPC546xx_ADC0_5_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc0_5[] = {
            {GPIO_P7_24, IOCON_A_AD(4)}, // ADC0.5
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO7_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc0_5, ARRAY_COUNT(adc0_5));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO0 |= (1 << 5);
    }
}
void LPC546xx_ADC0_6_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc0_6[] = {
            {GPIO_P5_26, IOCON_A_AD(4)}, // ADC0.6
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO5_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc0_6, ARRAY_COUNT(adc0_6));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO0 |= (1 << 6);
    }
}
void LPC546xx_ADC0_7_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc0_7[] = {
            {GPIO_P5_26, IOCON_A_AD(4)}, // ADC0.7
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO5_Require();
    }
    LPC546xx_ADC0_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc0_7, ARRAY_COUNT(adc0_7));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO0 |= (1 << 6);
    }
}
void LPC546xx_ADC1_0_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc1_0[] = {
            {GPIO_P6_2, IOCON_A_AD(4)}, // ADC1.0
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO6_Require();
    }
    LPC546xx_ADC1_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc1_0, ARRAY_COUNT(adc1_0));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO1 |= (1 << 0);
    }
}
void LPC546xx_ADC1_1_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc1_1[] = {
            {GPIO_PZ_Z_PC_0, IOCON_A_AD(1)}, // ADC1.1
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO7_Require();
    }
    LPC546xx_ADC1_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc1_1, ARRAY_COUNT(adc1_1));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO1 |= (1 << 1);
    }
}
void LPC546xx_ADC1_2_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc1_2[] = {
            {GPIO_P7_23, IOCON_A_AD(4)}, // ADC1.2
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO7_Require();
    }
    LPC546xx_ADC1_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc1_2, ARRAY_COUNT(adc1_2));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO1 |= (1 << 2);
    }
}
void LPC546xx_ADC1_3_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc1_3[] = {
            {GPIO_P7_20, IOCON_A_AD(4)}, // ADC1.3
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO7_Require();
    }
    LPC546xx_ADC1_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc1_3, ARRAY_COUNT(adc1_3));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO1 |= (1 << 3);
    }
}

void LPC546xx_ADC1_4_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc1_4[] = {
            {GPIO_P7_19, IOCON_A_AD(4)}, // ADC1.4
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO7_Require();
    }
    LPC546xx_ADC1_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc1_4, ARRAY_COUNT(adc1_4));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO1 |= (1 << 4);
    }
}

void LPC546xx_ADC1_5_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc1_5[] = {
            {GPIO_P7_25, IOCON_A_AD(4)}, // ADC1.4
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO7_Require();
    }
    LPC546xx_ADC1_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc1_5, ARRAY_COUNT(adc1_5));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO1 |= (1 << 5);
    }
}

void LPC546xx_ADC1_6_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc1_6[] = {
            {GPIO_P3_15, IOCON_A_AD(0)}, // ADC1.6
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO3_Require();
    }
    LPC546xx_ADC1_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc1_6, ARRAY_COUNT(adc1_6));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO1 |= (1 << 6);
    }
}

void LPC546xx_ADC1_7_Require(T_uezGPIOPortPin aPin)
{
    static const T_LPC546xx_SCU_ConfigList adc1_7[] = {
            {GPIO_P7_21, IOCON_A_AD(4)}, // ADC1.7
    };
    HAL_DEVICE_REQUIRE_ONCE();
    if(aPin != GPIO_NONE){
        LPC546xx_GPIO7_Require();
    }
    LPC546xx_ADC1_Require();
    LPC546xx_SCU_ConfigPinOrNone(aPin, adc1_7, ARRAY_COUNT(adc1_7));
    if(aPin != GPIO_NONE){
        LPC_SCU->ENAIO1 |= (1 << 7);
    }
}


/*-------------------------------------------------------------------------*
 * End of File:  ADC.c
 *-------------------------------------------------------------------------*/

