/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_PLL.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZMemory.h>
#include "LPC17xx_40xx_PLL.h"
#include "LPC17xx_40xx_UtilityFuncs.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
/* Force the variables into internal RAM. The "IN_INTERNAL_RAM" isn't
 * completely setup. So use out own placement macro instead. */
UEZ_PUT_SECTION(".IRAM", TUInt32 G_OscillatorFrequency);
UEZ_PUT_SECTION(".IRAM", TUInt32 G_ProcessorFrequency);
UEZ_PUT_SECTION(".IRAM", TUInt32 G_PeripheralFrequency);
UEZ_PUT_SECTION(".IRAM", TUInt32 G_EMCFrequency);
UEZ_PUT_SECTION(".IRAM", TUInt32 G_USBFrequency);

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    __IO uint32_t CON; /*!< Offset: 0x080 (R/W)  PLL Control Register */
    __IO uint32_t CFG; /*!< Offset: 0x084 (R/W)  PLL Configuration Register */
    __I uint32_t STAT; /*!< Offset: 0x088 (R/ )  PLL Status Register */
    __O uint32_t FEED; /*!< Offset: 0x08C ( /W)  PLL Feed Register */
} T_pllRegs;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static void IPLLSetup(T_pllRegs *pll, TUInt32 aGoalFreq, TUInt32 aRefFreq)
{
    TUInt32 m = aGoalFreq / aRefFreq;
    TUInt32 p = 0;
    TUInt32 fcco;

    do {
        fcco = aRefFreq * (1 << p) * 2 * m;
        if ((fcco >= 156000000) && (fcco <= 320000000))
            break;
        p++;
    } while (1);
    pll->CFG = (p << 5) | (m - 1);

    // Enable
    pll->CON = 0x01;
    pll->FEED = 0xAA;
    pll->FEED = 0x55;

    /* Wait for PLOCK0 */
    while (!(pll->STAT & (1 << 10)))
        ;

    // Enable & Connect
    pll->CON = 0x03;
    pll->FEED = 0xAA;
    pll->FEED = 0x55;
}

void LPC17xx_40xx_PLL_SetFrequencies(const T_LPC17xx_40xx_PLL_Frequencies *aFreq)
{
    TUInt32 pll0_clk;
    TUInt32 pll1_clk;
    TUInt32 f_clk;
    TUInt32 usbsel_clk;

    /* Start with the safest FLASHCFG setting */
    LPC_SC->FLASHCFG = 0x00005000;

    /* Turn on the main oscillator */
    LPC_SC->SCS = 0x00000020;
    /* Wait for Oscillator to be ready */
    while (
        (LPC_SC->SCS & (1 << 6)) == 0)
        ;

    /* Peripheral Clock Selection */
    LPC_SC->PCLKSEL = aFreq->iPeripheralClockDivider;

    /* EMC Clock Selection */
    LPC_SC->EMCCLKSEL = aFreq->iEMCClockDivider - 1;

    /* Select Clock Source for PLL0 */
    LPC_SC->CLKSRCSEL = aFreq->iClockSourceSelect;
    if (aFreq->iClockSourceSelect)
        G_OscillatorFrequency = aFreq->iOscillatorClockHz;
    else
        G_OscillatorFrequency = LPC17xx_40xx_IRC_FREQUENCY;

    /* Setup Clock Divider */
    LPC_SC->CCLKSEL = aFreq->iCPUClockDivider;

    // Setup the PLL0 and connect to the CPU
    IPLLSetup((T_pllRegs *)&LPC_SC->PLL0CON, aFreq->iPLL0Frequency,
            aFreq->iOscillatorClockHz);
    LPC_SC->CCLKSEL |= 0x100;

    /* It seems that the LPC_SC->SPIFICLKSEL is W only */
#if (UEZ_PROCESSOR == NXP_LPC4088)
    LPC_SC->SPIFICLKSEL = (1 << 8) | 2; //60Mhz clock (divide by 2) will allow 84MHz QSPI in spec, as long as we don't use READ command
    //LPC_SC->SPIFICLKSEL = (1 << 8) | 3; //40MHz clock (divide by 3) - This would be needed for some really old parts or for READ command.
#endif
    /* Setup USB Clock Divider on PLL1 with a divide by 1 */
    LPC_SC->USBCLKSEL = 1;
    IPLLSetup((T_pllRegs *)&LPC_SC->PLL1CON, aFreq->iPLL1Frequency,
            aFreq->iOscillatorClockHz);
    LPC_SC->USBCLKSEL |= (aFreq->iUSBClockSelect << 8);
    //  LPC_SC->SPIFISEL = 0x200 | SPIFISEL_Val; /* It seems that the LPC_SC->SPIFISEL is W only */

    /* Clock Output Configuration */
    LPC_SC->CLKOUTCFG = aFreq->iClockOutSelect
            | (((aFreq->iClockOutDivider - 1) & 15) << 4)
            | (aFreq->iClockOutEnable ? 0x100 : 0);

    /* Determine clock frequency according to clock register values */
    /* If PLL0 enabled and connected */
    if (((LPC_SC->PLL0STAT >> 8) & 1) == 1) {
        pll0_clk = G_OscillatorFrequency * ((LPC_SC->PLL0STAT & 0x1F) + 1);
        f_clk = pll0_clk;
    } else {
        f_clk = G_OscillatorFrequency;
    }
    G_ProcessorFrequency = f_clk / (LPC_SC->CCLKSEL & 0x1F);
    G_PeripheralFrequency = f_clk / (LPC_SC->PCLKSEL & 0x1F);
    G_EMCFrequency = f_clk / (1 + (LPC_SC->EMCCLKSEL & 1));
    if (((LPC_SC->USBCLKSEL >> 8) & 3) == 0) {
        usbsel_clk = G_OscillatorFrequency;
    } else if (((LPC_SC->USBCLKSEL >> 8) & 3) == 1) {
        pll0_clk = G_OscillatorFrequency * ((LPC_SC->PLL0STAT & 0x1F) + 1);
        usbsel_clk = pll0_clk;
    } else {
        pll1_clk = G_OscillatorFrequency * ((LPC_SC->PLL1STAT & 0x1F) + 1);
        usbsel_clk = pll1_clk;
    }
    G_USBFrequency = usbsel_clk / (LPC_SC->USBCLKSEL & 0x1F);

    // --------------------- Flash Accelerator Configuration ----------------------
    //   <o1.0..1>   FETCHCFG: Fetch Configuration
    //               <0=> Instruction fetches from flash are not buffered
    //               <1=> One buffer is used for all instruction fetch buffering
    //               *<2=> All buffers may be used for instruction fetch buffering
    //               <3=> Reserved (do not use this setting)
    //   <o1.2..3>   DATACFG: Data Configuration
    //               <0=> Data accesses from flash are not buffered
    //               <1=> One buffer is used for all data access buffering
    //               *<2=> All buffers may be used for data access buffering
    //               <3=> Reserved (do not use this setting)
    //   <o1.4>      *ACCEL: Acceleration Enable
    //   <o1.5>      *PREFEN: Prefetch Enable
    //   <o1.6>      PREFOVR: Prefetch Override
    //   <o1.12..15> FLASHTIM: Flash Access Time
    //               <0=> 1 CPU clock (for CPU clock up to 20 MHz)
    //               <1=> 2 CPU clocks (for CPU clock up to 40 MHz)
    //               <2=> 3 CPU clocks (for CPU clock up to 60 MHz)
    //               <3=> 4 CPU clocks (for CPU clock up to 80 MHz)
    //               <4=> 5 CPU clocks (for CPU clock up to 100 MHz)
    //               <5=> 6 CPU clocks (for any CPU clock)
    // And turn on the Flash Accelerator functions
#if 1
    if(G_ProcessorFrequency < 20000000){
        LPC_SC->FLASHCFG = (0<<12) | 0x3A;
    } else if(G_ProcessorFrequency < 40000000){
        LPC_SC->FLASHCFG = (1<<12) | 0x3A;
    } else if(G_ProcessorFrequency < 60000000){
        LPC_SC->FLASHCFG = (2<<12) | 0x3A;
    } else if(G_ProcessorFrequency < 80000000){
        LPC_SC->FLASHCFG = (3<<12) | 0x3A;
    } else if(G_ProcessorFrequency <= 1200000000){ //Assumes power boost is left on
        LPC_SC->FLASHCFG = (4<<12) | 0x3A;
    } else {
        LPC_SC->FLASHCFG = (5<<12) | 0x3A;
    }
#else
    LPC_SC->FLASHCFG = ((((G_ProcessorFrequency + 19999999) / 20000000) - 1)
            << 12) | 0x3A;
#endif
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_PLL.c
 *-------------------------------------------------------------------------*/
