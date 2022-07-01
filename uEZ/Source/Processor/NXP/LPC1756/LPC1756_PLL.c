/*-------------------------------------------------------------------------*
 * File:  LPC1756_PLL.c
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
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
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
#include "LPC1756_PLL.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define PLL0STAT_PLLC0_STAT 0x2000000
#define PLL0STAT_PLOCK0 0x4000000
#define PLL0CFG_MSEL0_BIT 0
#define PLL0CFG_NSEL0_BIT 16

#define PLL0CON_PLLE0 0x1
#define PLL0CON_PLLC0 0x2

#define SCS_OSCEN 0x20
#define SCS_OSCSTAT 0x40

#define PLL1CON_PLLE1 0x1
#define PLL1CON_PLLC1 0x2
#define PLL1CFG_MSEL1_BIT 0
#define PLL1CFG_PSEL1_BIT 5
#define PLL1CON_PLLE1 0x1
#define PLL1STAT_PLLC1_STAT 0x200
#define PLL1STAT_PLOCK1 0x400

/* Fosc = 12Mhz, Fcco = 400Mhz, cclk = 100Mhz */
#ifndef PLL0CFG_VAL
    #define PLL0CFG_VAL ((49 << PLL0CFG_MSEL0_BIT) | (2 << PLL0CFG_NSEL0_BIT))
#endif
#ifndef CCLKCFG_VAL
    #define CCLKCFG_VAL 3
#endif
#ifndef FLASHCFG_VAL
    #define FLASHCFG_VAL 0x0000403A
#endif
#ifndef CONFIGURE_USB
    #define CONFIGURE_USB
#endif
/* Fosc = 12Mhz, Fcco = 192Mhz, usbclk = 48Mhz */
#ifndef PLL1CFG_VAL
    #define PLL1CFG_VAL ((3 << PLL1CFG_MSEL1_BIT) | (1 << PLL1CFG_PSEL1_BIT))
#endif

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
TUInt32 G_OscillatorFrequency IN_INTERNAL_RAM;
TUInt32 G_ProcessorFrequency IN_INTERNAL_RAM;
TUInt32 G_PeripheralFrequency IN_INTERNAL_RAM;
TUInt32 G_EMCFrequency IN_INTERNAL_RAM;
TUInt32 G_USBFrequency IN_INTERNAL_RAM;

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
//static void IPLLSetup(T_pllRegs *pll, TUInt32 aGoalFreq, TUInt32 aRefFreq)
//{
//    TUInt32 m = aGoalFreq / aRefFreq;
//    TUInt32 p = 0;
//    TUInt32 fcco;
//
//    do {
//        fcco = aRefFreq * (1 << p) * 2 * m;
//        if ((fcco >= 156000000) && (fcco <= 320000000))
//            break;
//        p++;
//    } while (1);
//    pll->CFG = (p << 5) | (m - 1);
//
//    // Enable
//    pll->CON = 0x01;
//    pll->FEED = 0xAA;
//    pll->FEED = 0x55;
//
//    /* Wait for PLOCK0 */
//    while (!(pll->STAT & (1 << 10)))
//        ;
//
//    // Enable & Connect
//    pll->CON = 0x03;
//    pll->FEED = 0xAA;
//    pll->FEED = 0x55;
//}
//
void LPC1756_PLL_SetFrequencies(const T_LPC1756_PLL_Frequencies *aFreq)
{
    // Right now, its all hard coded
    G_OscillatorFrequency =  12000000;
    G_ProcessorFrequency = 100000000;
    G_PeripheralFrequency = 100000000;
    G_USBFrequency = 12000000;
    G_EMCFrequency = 0;

    // Configure PLL0 Multiplier/Divider
    if (SC->PLL0STAT & PLL0STAT_PLLC0_STAT) {
        // Disconnect PLL0
        SC->PLL0CON = PLL0CON_PLLE0;
        SC->PLL0FEED = 0xAA;
        SC->PLL0FEED = 0x55;
    }

    // Disable PLL0
    SC->PLL0CON = 0;
    SC->PLL0FEED = 0xAA;
    SC->PLL0FEED = 0x55;

    // Enable main oscillator
    SC->SCS |= SCS_OSCEN;
    while ((SC->SCS & SCS_OSCSTAT) == 0)
        {}

    // Select main oscillator as the PLL0 clock source
    SC->CLKSRCSEL = 1;

    // Set PLL0 Config
    SC->PLL0CFG = PLL0CFG_VAL;
    SC->PLL0FEED = 0xAA;
    SC->PLL0FEED = 0x55;

    // Enable PLL0
    SC->PLL0CON = PLL0CON_PLLE0;
    SC->PLL0FEED = 0xAA;
    SC->PLL0FEED = 0x55;

#ifdef CCLKCFG_VAL
    // Set the CPU clock divider
    SC->CCLKCFG = CCLKCFG_VAL;
#endif

#ifdef FLASHCFG_VAL
    SC->FLASHCFG = FLASHCFG_VAL;
#endif

    // Wait for PLL0 to lock
    while ((SC->PLL0STAT & PLL0STAT_PLOCK0)==0)
        {}

    // PLL0 Locked, connect PLL as clock source
    SC->PLL0CON = (PLL0CON_PLLE0 | PLL0CON_PLLC0);
    SC->PLL0FEED = 0xAA;
    SC->PLL0FEED = 0x55;

    // Wait for PLL0 to connect
    while ((SC->PLL0STAT & PLL0STAT_PLLC0_STAT) == 0)
        {}

#ifdef CONFIGURE_USB
    // Configure PLL1 Multiplier/Divider
    if (SC->PLL1STAT & PLL1STAT_PLLC1_STAT) {
        // Disconnect PLL1
        SC->PLL1CON = PLL1CON_PLLE1;
        SC->PLL1FEED = 0xAA;
        SC->PLL1FEED = 0x55;
    }

    // Disable PLL1
    SC->PLL1CON = 0;
    SC->PLL1FEED = 0xAA;
    SC->PLL1FEED = 0x55;

    // Set PLL1CFG
    SC->PLL1CFG = PLL1CFG_VAL;
    SC->PLL1FEED = 0xAA;
    SC->PLL1FEED = 0x55;

    // Enable PLL1
    SC->PLL1CON = PLL1CON_PLLE1;
    SC->PLL1FEED = 0xAA;
    SC->PLL1FEED = 0x55;

    // Wait for PLL1 to lock
    while ((SC->PLL1STAT & PLL1STAT_PLOCK1)==0)
        {}

    // PLL1 locked, connect PLL as clock source
    SC->PLL1CON = (PLL1CON_PLLE1 | PLL1CON_PLLC1);
    SC->PLL1FEED = 0xAA;
    SC->PLL1FEED = 0x55;

    // Wait for PLL1 to connect
    while ((SC->PLL1STAT & PLL1STAT_PLLC1_STAT)==0)
        {}
#endif
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_PLL.c
 *-------------------------------------------------------------------------*/



