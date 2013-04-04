/*-------------------------------------------------------------------------*
 * File:  LPC1788_BSP.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdio.h>

#include <uEZ.h>
#include "LPC1788_BSP.h"
#include "CMSIS/LPC1788.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define FLASH0_ADDR 0x80000000
#define FLASH0      ((volatile TUInt16 *)FLASH0_ADDR)
#define FLASH1_ADDR 0x90000000
#define FLASH1      ((volatile TUInt16 *)FLASH1_ADDR)
#define FLASH2_ADDR 0x98000000
#define FLASH2      ((volatile TUInt16 *)FLASH2_ADDR)
#define FLASH3_ADDR 0x9C000000
#define FLASH3      ((volatile TUInt16 *)FLASH3_ADDR)

#define UEZBSP_SDRAM_SIZE                   (8*1024*1024)
#define UEZBSP_SDRAM_BASE_ADDR              0xA0000000

// This is a calibrated constant for MS loop
#define NOPS_PER_MS     \
    ((TUInt32)(41.67*(PROCESSOR_OSCILLATOR_FREQUENCY/1000000.0)))

// This is a calibrated constant for US loop
#define NOPS_PER_MS2    \
    ((TUInt32)(43.31*(PROCESSOR_OSCILLATOR_FREQUENCY/1000000.0)))

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define SDRAM_CYCLES(x) \
    (1+((unsigned int)(((double)x)*(1.0/1000000000.0)*((double)SDRAM_CLOCK_FREQUENCY))))
#define SDRAM_CLOCKS(x) (x)

#define FLASH_CYCLES(x) \
    (1+((unsigned int)(((double)(x))*(1.0/1000000000.0)*((double)FLASH_CLOCK_FREQUENCY))))
#define FLASH_CLOCKS(x) (x)

static inline void NOP()
{
    __ASM("nop");
}

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
TUInt32 G_BSPNORFlashSize;
TUInt32 G_BSPNORFlashBitWidth;
TUInt32 SystemFrequency = IRC_OSC; /*!< System Clock Frequency (Core Clock)  */
TUInt32 PeripheralFrequency = IRC_OSC;
uint32_t ringosccount[2] = { 0, 0 };

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSPDelayMS
 *---------------------------------------------------------------------------*
 * Description:
 *      Use a delay loop to approximate the time to delay.
 *      Should use UEZTaskDelayMS() when in a task instead.
 *---------------------------------------------------------------------------*/
void UEZBSPDelay1MS(void)
{
    TUInt32 i;

    // Approximate delays here
    for (i = 0; i < NOPS_PER_MS; i++)
        __NOP();
}

void UEZBSPDelayMS(unsigned int aMilliseconds)
{
    while (aMilliseconds--) {
        UEZBSPDelay1MS();
    }
}

void UEZBSPDelayUS(unsigned int aMicroseconds)
{
#if (PROCESSOR_OSCILLATOR_FREQUENCY==100000000)
    // This is a close estimate
    while (aMicroseconds--) {
        __ASM volatile ("nop"); // 100 nops in internal flash = uEZ
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
        __ASM volatile ("nop");
    }
#elif (PROCESSOR_OSCILLATOR_FREQUENCY==120000000)
    // This is a close estimate
    while (aMicroseconds--) {
        // 120 nops in internal flash = uEZ
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
        NOP();
    }
#endif
}

/*****************************************************************************
 ** Function name:       sdram_test
 **
 ** Descriptions:        sdram test
 **
 ** parameters:          None
 **
 ** Returned value:      1 if test passed, otherwise 0
 **
 *****************************************************************************/
uint32_t sdram_test(void)
{
    volatile uint32_t *wr_ptr;
    volatile uint16_t *short_wr_ptr;
    uint32_t data;
    uint32_t i, j;

    wr_ptr = (uint32_t *)UEZBSP_SDRAM_BASE_ADDR;
    short_wr_ptr = (uint16_t *)wr_ptr;
    /* Clear content before 16 bit access test */
    //  for (i = 0; i < UEZBSP_SDRAM_SIZE/4; i++)
    //  {
    //  *wr_ptr++ = 0;
    //  }

    /* 16 bit write */
    for (i = 0; i < UEZBSP_SDRAM_SIZE / 0x40000; i++) {
        for (j = 0; j < 0x100; j++) {
            *short_wr_ptr++ = (i + j);
            *short_wr_ptr++ = (i + j) + 1;
        }
    }

    /* Verifying */
    wr_ptr = (uint32_t *)UEZBSP_SDRAM_BASE_ADDR;
    for (i = 0; i < UEZBSP_SDRAM_SIZE / 0x40000; i++) {
        for (j = 0; j < 0x100; j++) {
            data = *wr_ptr;
            if (data != (((((i + j) + 1) & 0xFFFF) << 16) | ((i + j) & 0xFFFF))) {
                return 0x0;
            }
            wr_ptr++;
        }
    }
    return 0x1;
}

/*****************************************************************************
 ** Function name:       find_cmddly
 **
 ** Descriptions:        find CMDDLY
 **
 ** parameters:          None
 **
 ** Returned value:      1 if test passed, otherwise 0
 **
 *****************************************************************************/
uint32_t find_cmddly(void)
{
    uint32_t cmddly, cmddlystart, cmddlyend, dwtemp;
    uint32_t ppass = 0x0, pass = 0x0;

    cmddly = 0x0;
    cmddlystart = cmddlyend = 0xFF;

    while (cmddly < 32) {
        dwtemp = LPC_SC->EMCDLYCTL & ~0x1F;
        LPC_SC->EMCDLYCTL = dwtemp | cmddly;

        if (sdram_test() == 0x1) {
            /* Test passed */
            if (cmddlystart == 0xFF) {
                cmddlystart = cmddly;
            }
            ppass = 0x1;
        } else {
            /* Test failed */
            if (ppass == 1) {
                cmddlyend = cmddly;
                pass = 0x1;
                ppass = 0x0;
            }
        }

        /* Try next value */
        cmddly++;
    }

    /* If the test passed, the we can use the average of the min and max values to get an optimal DQSIN delay */
    if (pass == 0x1) {
        cmddly = (cmddlystart + cmddlyend) / 2;
    } else if (ppass == 0x1) {
        cmddly = (cmddlystart + 0x1F) / 2;
    } else {
        /* A working value couldn't be found, just pick something safe so the system doesn't become unstable */
        cmddly = 0x10;
    }

    dwtemp = LPC_SC->EMCDLYCTL & ~0x1F;
    LPC_SC->EMCDLYCTL = dwtemp | cmddly;

    return (pass | ppass);
}

/*****************************************************************************
 ** Function name:       find_fbclkdly
 **
 ** Descriptions:        find FBCLKDLY
 **
 ** parameters:          None
 **
 ** Returned value:      1 if test passed, otherwise 0
 **
 *****************************************************************************/
uint32_t find_fbclkdly(void)
{
    uint32_t fbclkdly, fbclkdlystart, fbclkdlyend, dwtemp;
    uint32_t ppass = 0x0, pass = 0x0;

    fbclkdly = 0x0;
    fbclkdlystart = fbclkdlyend = 0xFF;

    while (fbclkdly < 32) {
        dwtemp = LPC_SC->EMCDLYCTL & ~0x1F00;
        LPC_SC->EMCDLYCTL = dwtemp | (fbclkdly << 8);

        if (sdram_test() == 0x1) {
            /* Test passed */
            if (fbclkdlystart == 0xFF) {
                fbclkdlystart = fbclkdly;
            }
            ppass = 0x1;
        } else {
            /* Test failed */
            if (ppass == 1) {
                fbclkdlyend = fbclkdly;
                pass = 0x1;
                ppass = 0x0;
            }
        }

        /* Try next value */
        fbclkdly++;
    }

    /* If the test passed, the we can use the average of the min and max values to get an optimal DQSIN delay */
    if (pass == 0x1) {
        fbclkdly = (fbclkdlystart + fbclkdlyend) / 2;
    } else if (ppass == 0x1) {
        fbclkdly = (fbclkdlystart + 0x1F) / 2;
    } else {
        /* A working value couldn't be found, just pick something safe so the system doesn't become unstable */
        fbclkdly = 0x10;
    }

    dwtemp = LPC_SC->EMCDLYCTL & ~0x1F00;
    LPC_SC->EMCDLYCTL = dwtemp | (fbclkdly << 8);

    return (pass | ppass);
}

/*****************************************************************************
 ** Function name:       calibration
 **
 ** Descriptions:        Calibration
 **
 ** parameters:          None
 **
 ** Returned value:      current ring osc count
 **
 *****************************************************************************/
uint32_t calibration(void)
{
    uint32_t dwtemp, i;
    uint32_t cnt = 0;

    for (i = 0; i < 10; i++) {
        dwtemp = LPC_SC->EMCCAL & ~0x4000;
        LPC_SC->EMCCAL = dwtemp | 0x4000;

        dwtemp = LPC_SC->EMCCAL;
        while ((dwtemp & 0x8000) == 0x0000) {
            dwtemp = LPC_SC->EMCCAL;
        }
        cnt += (dwtemp & 0xFF);
    }
    return (cnt / 10);
}

/*****************************************************************************
 ** Function name:       adjust_timing
 **
 ** Descriptions:        Adjust timing
 **
 ** parameters:          None
 **
 ** Returned value:      None
 **
 *****************************************************************************/
void adjust_timing(void)
{
    uint32_t dwtemp, cmddly, fbclkdly;

    /* Current value */
    ringosccount[1] = calibration();

    dwtemp = LPC_SC->EMCDLYCTL;
    cmddly = (((dwtemp & 0x1F) * ringosccount[0]) / ringosccount[1]) & 0x1F;
    fbclkdly = (((dwtemp & 0x1F00) * ringosccount[0]) / ringosccount[1])
            & 0x1F00;
    LPC_SC->EMCDLYCTL = (dwtemp & ~0x1F1F) | fbclkdly | cmddly;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_CPU_PinConfigInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Immediately configure the port pins to their default settings
 *      to support hardware.
 *---------------------------------------------------------------------------*/
void LPC1788_BSP_PinConfigInit(void)
{
#include "LPC17xx_pin_macros.h"
#include "LPC1788_pinConfig.h"

    // P3 is data lines (0-31)
    // P4 is address lines (0-23)

    LPC_IOCON->P2_16 |= 1; // EMC_CASn
    LPC_IOCON->P2_17 |= 1; // EMC_RASn
    LPC_IOCON->P2_18 |= 1; // EMC_CLK_0
    LPC_IOCON->P2_20 |= 1; // EMC_DYCS0n
    LPC_IOCON->P2_24 |= 1; // EMC_CKE0
    LPC_IOCON->P2_28 |= 1; // EMC_DQM0
    LPC_IOCON->P2_29 |= 1; // EMC_DQM1
    LPC_IOCON->P2_30 |= 1; // EMC_DQM2
    LPC_IOCON->P2_31 |= 1; // EMC_DQM3
    LPC_IOCON->P3_0 |= 1; // EMC_D0
    LPC_IOCON->P3_1 |= 1; // EMC_D1
    LPC_IOCON->P3_2 |= 1; // EMC_D2
    LPC_IOCON->P3_3 |= 1; // EMC_D3
    LPC_IOCON->P3_4 |= 1; // EMC_D4
    LPC_IOCON->P3_5 |= 1; // EMC_D5
    LPC_IOCON->P3_6 |= 1; // EMC_D6
    LPC_IOCON->P3_7 |= 1; // EMC_D7
    LPC_IOCON->P3_8 |= 1; // EMC_D8
    LPC_IOCON->P3_9 |= 1; // EMC_D9
    LPC_IOCON->P3_10 |= 1; // EMC_D10
    LPC_IOCON->P3_11 |= 1; // EMC_D11
    LPC_IOCON->P3_12 |= 1; // EMC_D12
    LPC_IOCON->P3_13 |= 1; // EMC_D13
    LPC_IOCON->P3_14 |= 1; // EMC_D14
    LPC_IOCON->P3_15 |= 1; // EMC_D15
    LPC_IOCON->P3_16 |= 1; // EMC_D16
    LPC_IOCON->P3_17 |= 1; // EMC_D17
    LPC_IOCON->P3_18 |= 1; // EMC_D18
    LPC_IOCON->P3_19 |= 1; // EMC_D19
    LPC_IOCON->P3_20 |= 1; // EMC_D20
    LPC_IOCON->P3_21 |= 1; // EMC_D21
    LPC_IOCON->P3_22 |= 1; // EMC_D22
    LPC_IOCON->P3_23 |= 1; // EMC_D23
    LPC_IOCON->P3_24 |= 1; // EMC_D24
    LPC_IOCON->P3_25 |= 1; // EMC_D25
    LPC_IOCON->P3_26 |= 1; // EMC_D26
    LPC_IOCON->P3_27 |= 1; // EMC_D27
    LPC_IOCON->P3_28 |= 1; // EMC_D28
    LPC_IOCON->P3_29 |= 1; // EMC_D29
    LPC_IOCON->P3_30 |= 1; // EMC_D30
    LPC_IOCON->P3_31 |= 1; // EMC_D31
    LPC_IOCON->P4_0 |= 1; // EMC_A0
    LPC_IOCON->P4_1 |= 1; // EMC_A1
    LPC_IOCON->P4_2 |= 1; // EMC_A2
    LPC_IOCON->P4_3 |= 1; // EMC_A3
    LPC_IOCON->P4_4 |= 1; // EMC_A4
    LPC_IOCON->P4_5 |= 1; // EMC_A5
    LPC_IOCON->P4_6 |= 1; // EMC_A6
    LPC_IOCON->P4_7 |= 1; // EMC_A7
    LPC_IOCON->P4_8 |= 1; // EMC_A8
    LPC_IOCON->P4_9 |= 1; // EMC_A9
    LPC_IOCON->P4_10 |= 1; // EMC_A10
    LPC_IOCON->P4_11 |= 1; // EMC_A11
    LPC_IOCON->P4_12 |= 1; // EMC_A12
    LPC_IOCON->P4_13 |= 1; // EMC_A13
    LPC_IOCON->P4_14 |= 1; // EMC_A14
    LPC_IOCON->P4_15 |= 1; // EMC_A15
    LPC_IOCON->P4_16 |= 1; // EMC_A16
    LPC_IOCON->P4_17 |= 1; // EMC_A17
    LPC_IOCON->P4_18 |= 1; // EMC_A18
    LPC_IOCON->P4_19 |= 1; // EMC_A19
    LPC_IOCON->P4_20 |= 1; // EMC_A20
    LPC_IOCON->P4_21 |= 1; // EMC_A21
    LPC_IOCON->P4_22 |= 1; // EMC_A22
    LPC_IOCON->P4_23 |= 1; // EMC_A23
    LPC_IOCON->P4_24 |= 1; // EMC_OEn
    LPC_IOCON->P4_25 |= 1; // EMC_WEn
    LPC_IOCON->P4_30 |= 1; // EMC_CS0n

#if 0
    // Any pins that need to be set high, set them now
    LPC_GPIO0->SET = PINCFG_PINSET(P0);
    LPC_GPIO1->SET = PINCFG_PINSET(P1);
    LPC_GPIO2->SET = PINCFG_PINSET(P2);
    LPC_GPIO3->SET = PINCFG_PINSET(P3);
    LPC_GPIO4->SET = PINCFG_PINSET(P4);
    LPC_GPIO5->SET = PINCFG_PINSET(P5);

    // Any pins that need to be set low, set them now
    LPC_GPIO0->CLR = PINCFG_PINCLR(P0);
    LPC_GPIO1->CLR = PINCFG_PINCLR(P1);
    LPC_GPIO2->CLR = PINCFG_PINCLR(P2);
    LPC_GPIO3->CLR = PINCFG_PINCLR(P3);
    LPC_GPIO4->CLR = PINCFG_PINCLR(P4);
    LPC_GPIO5->CLR = PINCFG_PINCLR(P5);

    // Set the pin direction (input/output)
    // For most cases, none of the above drives until this point
    LPC_GPIO0->DIR = PINCFG_PINDIR(P0);
    LPC_GPIO1->DIR = PINCFG_PINDIR(P1);
    LPC_GPIO2->DIR = PINCFG_PINDIR(P2);
    LPC_GPIO3->DIR = PINCFG_PINDIR(P3);
    LPC_GPIO4->DIR = PINCFG_PINDIR(P4);
    LPC_GPIO5->DIR = PINCFG_PINDIR(P5);
#endif
}

void LPC1788_BSP_PLLConfigure(void)
{
    int i;
    TUInt32 pll0stat;
    TUInt32 cclksel;

    // Artificial delay for testing
    for (i = 0; i < 100000; i++)
        ;

#if (FLASH_SETUP == 1)                  /* Flash Accelerator Setup            */
    LPC_SC->FLASHCFG = FLASHCFG_Val;
#endif
#if (CLOCK_SETUP)                       /* Clock Setup                        */
    LPC_SC->SCS = SCS_Val;
    if (SCS_Val & (1 << 5)) { /* If Main Oscillator is enabled      */
        while ((LPC_SC->SCS & (1 << 6)) == 0)
            ;/* Wait for Oscillator to be ready    */
    }

    LPC_SC->PCLKSEL = PCLKSEL_Val; /* Peripheral Clock Selection         */

    LPC_SC->EMCCLKSEL = EMCCLKSEL_Val; /* EMC Clock Selection                */

    LPC_SC->SPIFISEL = SPIFISEL_Val; /* SPIFI Clock Selection              */
    LPC_SC->CLKSRCSEL = CLKSRCSEL_Val; /* Select Clock Source for PLL0       */
    LPC_SC->CCLKSEL = CCLKSEL_Val; /* Setup Clock Divider                */
#if (PLL0_SETUP)
    LPC_SC->PLL0CFG = PLL0CFG_Val;
    LPC_SC->PLL0CON = 0x01; /* PLL0 Enable                        */
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;
    while (!(LPC_SC->PLL0STAT & (1 << 10)))
        ;/* Wait for PLOCK0                    */

    LPC_SC->PLL0CON = 0x03; /* PLL0 Enable & Connect              */
    LPC_SC->PLL0FEED = 0xAA;
    LPC_SC->PLL0FEED = 0x55;

    LPC_SC->CCLKSEL |= 0x100;
    LPC_SC->SPIFISEL = 0x100 | SPIFISEL_Val; /* It seems that the LPC_SC->SPIFISEL is W only */
#endif

#if (PLL1_SETUP)
    LPC_SC->USBCLKSEL = USBSEL_Val; /* Setup USB Clock Divider            */

    LPC_SC->PLL1CFG = PLL1CFG_Val;
    LPC_SC->PLL1CON = 0x01; /* PLL1 Enable                        */
    LPC_SC->PLL1FEED = 0xAA;
    LPC_SC->PLL1FEED = 0x55;
    while (!(LPC_SC->PLL1STAT & (1 << 10)))
        ;/* Wait for PLOCK1                    */

    LPC_SC->PLL1CON = 0x03; /* PLL1 Enable & Connect              */
    LPC_SC->PLL1FEED = 0xAA;
    LPC_SC->PLL1FEED = 0x55;

    LPC_SC->USBCLKSEL |= 0x200;
    //  LPC_SC->SPIFISEL = 0x200 | SPIFISEL_Val; /* It seems that the LPC_SC->SPIFISEL is W only */
#else
    LPC_SC->USBSEL = USBSEL_Val; /* Setup USB Clock Divider            */
#endif

    LPC_SC->PCONP = PCONP_Val; /* Power Control for Peripherals      */

    LPC_SC->CLKOUTCFG = CLKOUTCFG_Val; /* Clock Output Configuration         */
#endif

    /* Determine clock frequency according to clock register values             */
    if (((LPC_SC->PLL0STAT >> 8) & 3) == 3) {/* If PLL0 enabled and connected      */
        pll0stat = (LPC_SC->PLL0STAT & 0x1F) + 1;
        cclksel = LPC_SC->CCLKSEL & 0x1F;
        switch (LPC_SC->CLKSRCSEL & 0x03) {
            case 0: /* Internal RC oscillator => PLL0     */
                SystemFrequency = ((IRC_OSC * pll0stat) / cclksel);
                PeripheralFrequency = (IRC_OSC * pll0stat / cclksel);
                break;
            case 1: /* Main oscillator => PLL0            */
                SystemFrequency = ((OSC_CLK * pll0stat) / cclksel);
                PeripheralFrequency = (OSC_CLK * pll0stat / cclksel);
                break;
        }
    } else {
        switch (LPC_SC->CLKSRCSEL & 0x03) {
            case 0: /* Internal RC oscillator => PLL0     */
                SystemFrequency = IRC_OSC / (LPC_SC->CCLKSEL & 0x1F);
                break;
            case 1: /* Main oscillator => PLL0            */
                SystemFrequency = OSC_CLK / (LPC_SC->CCLKSEL & 0x1F);
                break;
        }
    }
}

void LPC1788_BSP_InitializeNORFlash(void)
{
    // --- M29W128G NOR Flash ---
#if 1 // Faster configuration
    // Configuration the NOR Flash like a static memory
    LPC_EMC->StaticConfig0 = (1 << 0) | // 1=16-bit
            (0 << 3) | // 0=page mode disabled (TBD: for now)
            (0 << 6) | // 0=Acive LOW chip select (CS0n -> En)
            (1 << 7) | // 0=For Reads BLSn[3:0] HIGH, For writes BLSn[3:0] LOW (not used)
            (0 << 8) | // 0=Extended Wait disabled (POR reset value)
            (0 << 19) | // 0=Buffer disabled (POR reset value, TBD)
            (0 << 20); // 0=Write not protected

    // Delay from chip select or address change, whichever is later to
    // the output enable
    // Use tAVQV = tACC = Address valid to Output Valid
    LPC_EMC->StaticWaitOen0 = 0; // FLASH_CYCLES(5)-0; //was 45

    // Delay from the chip select to the read access
    // Use tELQV = tEn = Address valid to Next Address Valid
    LPC_EMC->StaticWaitRd0 = FLASH_CYCLES(90) - 1; // 90 ns

    // Page Mode Read Delay register
    // Delay for asynchronous page mode sequential access
    // use tRC = address valid to address valid
    LPC_EMC->StaticWaitPage0 = 0; // FLASH_CYCLES(90)-1; // 90 ns

    // Delay from Chip select to write enable time
    // Use tELWL = tCS = Chip Enable Low to Write Enable Low
    LPC_EMC->StaticWaitWen0 = 0; // FLASH_CYCLES(45)-1;

    // Write Delay Register
    // Delay from the Chip Select to the Write Access
    // use tWLRH = tWP = Write Enable Low to Write Enable High
    LPC_EMC->StaticWaitWr0 = FLASH_CYCLES(90) - 2; // was 90 ns

    // Turn Round Delay register
    // Number of bus turnaround cycles, cycles between read and write access
    // use tWLRH = tWP = Write Enable Low to Write Enable High
    LPC_EMC->StaticWaitTurn0 = 1; // FLASH_CYCLES(45)-1;
#else
    // SLOW NOR FLASH!  WILL CAUSE LCD TO FLICKER!  TESTING ONLY

    // Configuration the NOR Flash like a static memory
    LPC_EMC->StaticConfig0 =
    (1<<0)| // 1=16-bit
    (0<<3)| // 0=page mode disabled (TBD: for now)
    (0<<6)| // 0=Acive LOW chip select (CS0n -> En)
    (1<<7)| // 0=For Reads BLSn[3:0] HIGH, FOr writes BLSn[3:0] LOW (not used)
    // NOTE!  This MUST be a 1 to get a pronounced WEn even though
    // a target may not use the BLSn[0:3] lines at all!
    (0<<8)| // 0=Extended Wait disabled (POR reset value)
    (0<<19)| // 0=Buffer disabled (POR reset value, TBD)
    (0<<20); // 0=Write not protected

    // Delay from Chip select to write enable time
    // Use tELWL = tCS = Chip Enable Low to Write Enable Low
    LPC_EMC->StaticWaitWen0 = 0x0F;

    // Delay from chip select or address change, whichever is later to
    // the output enable
    // Use tAVQV = tACC = Address valid to Output Valid
    LPC_EMC->StaticWaitOen0 = 0x0F;

    // Delay from the chip select to the read access
    // Use tELQV = tEn = Address valid to Next Address Valid
    LPC_EMC->StaticWaitRd0 = 0x1F;

    // Page Mode Read Delay register
    // Delay for asynchronous page mode sequential access
    LPC_EMC->StaticWaitPage0 = 0x0F;

    // Write Delay Register
    // Delay from the Chip Select to the Write Access
    LPC_EMC->StaticWaitWr0 = 0x1F;

    // Turn Round Delay register
    // Number of bus turnaround cycles, cycles between read and write access
    LPC_EMC->StaticWaitTurn0 = 0x0F;
#endif

    // Make sure address are byte wise on the address bus
    LPC_SC->SCS |= (1 << 0);

    // Which of type of NOR flash do we have installed?
    // Read CFI information and verify we can get QRY in the query area
    FLASH0[0x55] = 0x0098;
    if ((FLASH0[0x10] == 'Q') && (FLASH0[0x11] == 'R') && (FLASH0[0x12] == 'Y')) {
        // Valid header, now read the settings.
        G_BSPNORFlashSize = 1 << FLASH0[0x27];
        G_BSPNORFlashBitWidth = FLASH0[0x28];
    } else {
        // Use unknown values
        G_BSPNORFlashSize = 0;
        G_BSPNORFlashBitWidth = 0;
    }

    // Take NOR flash out of this mode (reset)
    // Reset the NOR flash to take us out of this mode
    FLASH0[0x555] = 0x00AA;
    FLASH0[0x2AA] = 0x0055;
    FLASH0[0x0] = 0x00F0;
}

void LPC1788_BSP_SDRAMInit(void)
{
    volatile unsigned int i, dummy = dummy;
    // EMC Reset Disable = Enabled, Both EMC resets are asserted
    // when any type of reset occurs.
    // [Ref: LPC178x_7x_UM_0.01 page 30, Table 14]
    LPC_SC->SCS &= ~(1 << 1);

    // Turn on EMC PCLK (if not already on)
    // [Ref: LPC178x_7x_UM_0.01 page 58, Table 36]
    LPC_SC->PCONP |= (1 << 11);

    // Setup slow/default delays
    LPC_SC->EMCDLYCTL = 0x00001010;

    // Reset the EMC and put configuration
    // back to power up reset (little-endian mode, 1:1 clock)
    // [Ref: LPC178x_7x_UM_0.01 page 156]
    LPC_EMC->Control = 0x00000001;
    LPC_EMC->Config = 0x00000000;

#if 0
    // Configure for 32-bit data bus
    // Each pin is set to alternative function 1
    LPC_IOCON->P3_0 |= 1; /* D0 @ P3.0 */
    LPC_IOCON->P3_1 |= 1; /* D1 @ P3.1 */
    LPC_IOCON->P3_2 |= 1; /* D2 @ P3.2 */
    LPC_IOCON->P3_3 |= 1; /* D3 @ P3.3 */

    LPC_IOCON->P3_4 |= 1; /* D4 @ P3.4 */
    LPC_IOCON->P3_5 |= 1; /* D5 @ P3.5 */
    LPC_IOCON->P3_6 |= 1; /* D6 @ P3.6 */
    LPC_IOCON->P3_7 |= 1; /* D7 @ P3.7 */

    LPC_IOCON->P3_8 |= 1; /* D8 @ P3.8 */
    LPC_IOCON->P3_9 |= 1; /* D9 @ P3.9 */
    LPC_IOCON->P3_10 |= 1; /* D10 @ P3.10 */
    LPC_IOCON->P3_11 |= 1; /* D11 @ P3.11 */

    LPC_IOCON->P3_12 |= 1; /* D12 @ P3.12 */
    LPC_IOCON->P3_13 |= 1; /* D13 @ P3.13 */
    LPC_IOCON->P3_14 |= 1; /* D14 @ P3.14 */
    LPC_IOCON->P3_15 |= 1; /* D15 @ P3.15 */

    LPC_IOCON->P3_16 |= 1; /* D16 @ P3.16 */
    LPC_IOCON->P3_17 |= 1; /* D17 @ P3.17 */
    LPC_IOCON->P3_18 |= 1; /* D18 @ P3.18 */
    LPC_IOCON->P3_19 |= 1; /* D19 @ P3.19 */

    LPC_IOCON->P3_20 |= 1; /* D20 @ P3.20 */
    LPC_IOCON->P3_21 |= 1; /* D21 @ P3.21 */
    LPC_IOCON->P3_22 |= 1; /* D22 @ P3.22 */
    LPC_IOCON->P3_23 |= 1; /* D23 @ P3.23 */

    LPC_IOCON->P3_24 |= 1; /* D24 @ P3.24 */
    LPC_IOCON->P3_25 |= 1; /* D25 @ P3.25 */
    LPC_IOCON->P3_26 |= 1; /* D26 @ P3.26 */
    LPC_IOCON->P3_27 |= 1; /* D27 @ P3.27 */

    LPC_IOCON->P3_28 |= 1; /* D28 @ P3.28 */
    LPC_IOCON->P3_29 |= 1; /* D29 @ P3.29 */
    LPC_IOCON->P3_30 |= 1; /* D30 @ P3.30 */
    LPC_IOCON->P3_31 |= 1; /* D31 @ P3.31 */

    // Configure for 32-bit address lines A0 to A19
    // Each pin is set to alternative function 1
    LPC_IOCON->P4_0 |= 1; /* A0 @ P4.0 */
    LPC_IOCON->P4_1 |= 1; /* A1 @ P4.1 */
    LPC_IOCON->P4_2 |= 1; /* A2 @ P4.2 */
    LPC_IOCON->P4_3 |= 1; /* A3 @ P4.3 */

    LPC_IOCON->P4_4 |= 1; /* A4 @ P4.4 */
    LPC_IOCON->P4_5 |= 1; /* A5 @ P4.5 */
    LPC_IOCON->P4_6 |= 1; /* A6 @ P4.6 */
    LPC_IOCON->P4_7 |= 1; /* A7 @ P4.7 */

    LPC_IOCON->P4_8 |= 1; /* A8 @ P4.8 */
    LPC_IOCON->P4_9 |= 1; /* A9 @ P4.9 */
    LPC_IOCON->P4_10 |= 1; /* A10 @ P4.10 */
    LPC_IOCON->P4_11 |= 1; /* A11 @ P4.11 */

    LPC_IOCON->P4_12 |= 1; /* A12 @ P4.12 */
    LPC_IOCON->P4_13 |= 1; /* A13 @ P4.13 */
    LPC_IOCON->P4_14 |= 1; /* A14 @ P4.14 */
    LPC_IOCON->P4_15 |= 1; /* A15 @ P4.15 */

    LPC_IOCON->P4_16 |= 1; /* A16 @ P4.16 */
    LPC_IOCON->P4_17 |= 1; /* A17 @ P4.17 */
    LPC_IOCON->P4_18 |= 1; /* A18 @ P4.18 */
    LPC_IOCON->P4_19 |= 1; /* A19 @ P4.19 */

    //      LPC_IOCON->P4_20 |= 1; /* A20 @ P4.20 */
    //      LPC_IOCON->P4_21 |= 1; /* A21 @ P4.21 */
    //      LPC_IOCON->P4_22 |= 1; /* A22 @ P4.22 */
    //      LPC_IOCON->P4_23 |= 1; /* A23 @ P4.23 */

    // Configure control pins (we really only care for bus lane 0)
    LPC_IOCON->P4_24 |= 1; /* OEN @ P4.24 */
    LPC_IOCON->P4_25 |= 1; /* WEN @ P4.25 */
    //      LPC_IOCON->P4_26 |= 1; /* BLSN[0] @ P4.26 */
    //      LPC_IOCON->P4_27 |= 1; /* BLSN[1] @ P4.27 */

    //      LPC_IOCON->P4_28 |= 1; /* BLSN[2] @ P4.28 */
    //      LPC_IOCON->P4_29 |= 1; /* BLSN[3] @ P4.29 */
    LPC_IOCON->P4_30 |= 1; /* CSN[0] @ P4.30 */
    //      LPC_IOCON->P4_31 |= 1; /* CSN[1] @ P4.31 */

    //      LPC_IOCON->P2_14 |= 1; /* CSN[2] @ P2.14 */
    //      LPC_IOCON->P2_15 |= 1; /* CSN[3] @ P2.15 */

    LPC_IOCON->P2_16 |= 1; /* CASN @ P2.16 */
    LPC_IOCON->P2_17 |= 1; /* RASN @ P2.17 */
    LPC_IOCON->P2_18 |= 1; /* CLK[0] @ P2.18 */
    //      LPC_IOCON->P2_19 |= 1; /* CLK[1] @ P2.19 */

    LPC_IOCON->P2_20 |= 1; /* DYCSN[0] @ P2.20 */
    //      LPC_IOCON->P2_21 |= 1; /* DYCSN[1] @ P2.21 */
    //      LPC_IOCON->P2_22 |= 1; /* DYCSN[2] @ P2.22 */
    //      LPC_IOCON->P2_23 |= 1; /* DYCSN[3] @ P2.23 */

    LPC_IOCON->P2_24 |= 1; /* CKE[0] @ P2.24 */
    //      LPC_IOCON->P2_25 |= 1; /* CKE[1] @ P2.25 */
    //      LPC_IOCON->P2_26 |= 1; /* CKE[2] @ P2.26 */
    //      LPC_IOCON->P2_27 |= 1; /* CKE[3] @ P2.27 */

    LPC_IOCON->P2_28 |= 1; /* DQM[0] @ P2.28 */
    LPC_IOCON->P2_29 |= 1; /* DQM[1] @ P2.29 */
    LPC_IOCON->P2_30 |= 1; /* DQM[2] @ P2.30 */
    LPC_IOCON->P2_31 |= 1; /* DQM[3] @ P2.31 */
#endif

#if (UEZBSP_SDRAM_SIZE==(8*1024*1024))
    LPC_EMC->DynamicConfig0 = 0x00005300;
#elif (UEZBSP_SDRAM_SIZE==(16*1024*1024))
    LPC_EMC->DynamicConfig0 = 0x00005500;
#elif (UEZBSP_SDRAM_SIZE==(32*1024*1024))
    LPC_EMC->DynamicConfig0 = 0x00005480;
#else
#error "Unsupported UEZBSP_SDRAM_SIZE!"
#endif

    // RAS=2, CAS=2
    LPC_EMC->DynamicRasCas0 = 0x00000202;
    LPC_EMC->DynamicReadConfig = 0x00000001; /* Command delayed strategy, using EMCCLKDELAY */

    // -6 timings
    LPC_EMC->DynamicRP = SDRAM_CYCLES(20); // (1) tRP min = 20 ns
    LPC_EMC->DynamicRAS = SDRAM_CYCLES(42); // (2) tRAS min = 42 ns
    LPC_EMC->DynamicSREX = SDRAM_CYCLES(70); // (4) use tXSR min = 70 ns
    LPC_EMC->DynamicAPR = SDRAM_CYCLES(18); // (1) no tAPR, using tRCD = 18 ns
    LPC_EMC->DynamicDAL = SDRAM_CLOCKS(4); // (3) for CL 2, tDAL = 4 tCK
    LPC_EMC->DynamicWR = (SDRAM_CYCLES(6) + SDRAM_CLOCKS(1)); // (1) tWR = tCK + 6ns
    LPC_EMC->DynamicRC = SDRAM_CYCLES(60); // (4) tRC min = 60 ns
    LPC_EMC->DynamicRFC = SDRAM_CYCLES(60); // (4) tRFC min = 60 ns
    LPC_EMC->DynamicXSR = SDRAM_CYCLES(70); // (4) tXSR min = 70 ns
    LPC_EMC->DynamicRRD = SDRAM_CYCLES(12); // (0) tRRD = 12 ns
    LPC_EMC->DynamicMRD = SDRAM_CLOCKS(2); // (1) tMRD = 2 tCK

    //Send command: NOP
    LPC_EMC->DynamicControl = 0x00000183;

    //wait 200mS
    UEZBSPDelayMS(200);

    //Send command: PRECHARGE-ALL, shortest possible refresh period
    LPC_EMC->DynamicControl = 0x00000100;
    LPC_EMC->DynamicRefresh = 0x00000001;

    //wait at least 128 ABH clock cycles
    for (i = 0; i < 0x100; i++)
        NOP();

    //Set correct refresh period
#define SDRAM_REFRESH_PERIOD    64 // ms
#define SDRAM_REFRESH_CYCLES    8192
    LPC_EMC->DynamicRefresh = (unsigned int)((((SDRAM_CLOCK_FREQUENCY / 1000)
            * SDRAM_REFRESH_PERIOD) / SDRAM_REFRESH_CYCLES) / 16) + 1;

    //wait at least 128 ABH clock cycles
    for (i = 0; i < 0x40; i++)
        NOP();

    //Send command: MODE
    LPC_EMC->DynamicControl = 0x00000080;

    //Set mode register in SDRAM
#if (UEZBSP_SDRAM_SIZE==(32*1024*1024))
    dummy = *((volatile unsigned int*)(UEZBSP_SDRAM_BASE_ADDR | (0x22 << 11)));
#else
    dummy = *((volatile unsigned int*)(UEZBSP_SDRAM_BASE_ADDR | (0x22 << 10)));
#endif
    //wait 128 ABH clock cycles
    for (i = 0; i < 0x40; i++)
        NOP();

    //Send command: NORMAL
    LPC_EMC->DynamicControl = 0x00000000;

    //Enable buffer
    LPC_EMC->DynamicConfig0 |= 0x00080000;

    //initial system delay
    UEZBSPDelayMS(1);

    ringosccount[0] = calibration();

    if (find_cmddly() == 0x0) {
        while (1)
            ; /* fatal error */
    }

    if (find_fbclkdly() == 0x0) {
        while (1)
            ; /* fatal error */
    }

    adjust_timing();

}

#define PCLK_FREQUENCY          (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#define BAUD_DIVIDER(b)         ((PCLK_FREQUENCY/1)/((b)*16))
#define SERIAL_PORTS_DEFAULT_BAUD       115200

void LPC1788_BSP_UartInit(void)
{
    TUInt32 divider ;
    LPC_UART_TypeDef * p_uart = LPC_UART0;

    // Ensure power is on to the part
    //LPC1788PowerOn(1UL << p->iInfo->iPowerBitIndex);

    divider = BAUD_DIVIDER(SERIAL_PORTS_DEFAULT_BAUD);

    // Set the FIFO enable bit in the FCR register. This bit must be set for
    // proper UART operation.
    p_uart->FCR = 7; // FCRFE|RFR|TFR

    // Set baudrate
    p_uart->LCR |= 0x80;
    p_uart->DLL = divider & 0x00ff;
    p_uart->DLM = (divider >> 8) & 0x00ff;
    p_uart->LCR &= ~0x80;

    // Set default mode (8 bits, 1 stop bit, no parity)
    p_uart->LCR = 0x03;

    //Enable UART0 interrupts
    //p_info->iUART->reg04.IER = 0x03; // Interrupts and TX and RX

    //TBD: For now, leave serial port deactivated
    //    InterruptEnable(p_info->iInterruptChannel);
    //return UEZ_ERROR_NONE;

}

#if DEBUG
size_t __write(int Handle, const unsigned char * Buf, size_t Bufsize)
{
    size_t nChars = 0;
    LPC_UART_TypeDef * p_uart = LPC_UART0;

    for (/*Empty */; Bufsize > 0; --Bufsize)
    {
        while(( p_uart->LSR & (1<<5))==0); //Wait on THRE bit
        p_uart->THR = * Buf++;
        ++nChars;
    }
    return nChars;
}
#endif

/*-------------------------------------------------------------------------*
 * End of File:  LPC1788_BSP.c
 *-------------------------------------------------------------------------*/
