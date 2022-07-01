/*-------------------------------------------------------------------------*
 * File:  LPC546xx_PLL.c
 *-------------------------------------------------------------------------*
 * Description:
 *
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>
#include "LPC546xx_PLL.h"

//Testing
#include "iar/include/CMSIS/LPC54608.h"
#include "iar/fsl_power.h"
#define IN_INTERNAL_RAM

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define NVALMAX         (0x100U)
#define PVALMAX         (0x20U)
#define MVALMAX         (0x8000U)
#define PLL_PDEC_VAL_P  (0U)                                /*!<  PDEC is in bits 6:0 */
#define PLL_PDEC_VAL_M  (0x3FFUL << PLL_PDEC_VAL_P)
#define PLL_NDEC_VAL_P  (0U)                                /*!<  NDEC is in bits  9:0 */
#define PLL_NDEC_VAL_M  (0x3FFUL << PLL_NDEC_VAL_P)
#define PLL_MDEC_VAL_P  (0U)                                /*!<  MDEC is in bits  16 downto 0 */
#define PLL_MDEC_VAL_M  (0x1FFFFUL << PLL_MDEC_VAL_P)       /*!<  NDEC is in bits  9 downto 0 */

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
/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

static TUInt32 Encode_Raw_P_Value(TUInt32 aValue)
{
    TUInt32 x, i;

    /* Find PDec */
    switch (aValue) {
        case 0:
            x = 0xFF;
            break;
        case 1:
            x = 0x62;
            break;
        case 2:
            x = 0x42;
            break;
        default:
            x = 0x10;
            for (i = aValue; i <= PVALMAX; i++) {
                x = (((x ^ (x >> 2)) & 1) << 4) | ((x >> 1) & 0xF);
            }
            break;
    }

    return x & (PLL_PDEC_VAL_M >> PLL_PDEC_VAL_P);
}

static TUInt32 Encode_Raw_N_Value(TUInt32 aValue)
{
    TUInt32 x, i;

    switch (aValue) {
        case 0:
            x = 0xFFF;
            break;
        case 1:
            x = 0x302;
            break;
        case 2:
            x = 0x202;
            break;
        default:
            x = 0x080;
            for (i = aValue; i <= NVALMAX; i++) {
                x = (((x ^ (x >> 2) ^ (x >> 3) ^ (x >> 4)) & 1) << 7)
                        | ((x >> 1) & 0x7F);
            }
    }
    return x;
}

static uint32_t Encode_Raw_M_Value(TUInt32 aValue)
{
    uint32_t i, x;

    /* Find MDec */
    switch (aValue) {
        case 0:
            x = 0xFFFFF;
            break;
        case 1:
            x = 0x18003;
            break;
        case 2:
            x = 0x10003;
            break;
        default:
            x = 0x04000;
            for (i = aValue; i <= MVALMAX; i++) {
                x = (((x ^ (x >> 1)) & 1) << 14) | ((x >> 1) & 0x3FFF);
            }
            break;
    }

    return x & (PLL_MDEC_VAL_M >> PLL_MDEC_VAL_P);
}

static void GetControlRegisterValues(TUInt32 aMultiplier, TUInt32* aSELI, TUInt32* aSELP, TUInt32* aSELR)
{
    if(aMultiplier < 60){
        *aSELP = (aMultiplier >> 1) + 1;
    } else {
        *aSELP = 31;
    }

    if(aMultiplier >= 60){
        *aSELI = 4 * (1024 / (aMultiplier + 9));
    } else if (aMultiplier >= 501){
        *aSELI = 8;
    } else if (aMultiplier > 2048){
        *aSELI = 4;
    } else if (aMultiplier > 8192){
        *aSELI = 2;
    }else if (aMultiplier >16384){
        *aSELI = 1;
    }else {
        *aSELI = (aMultiplier & 0x3C) + 4;
    }
    *aSELR = 0;
}

#define INDEX_SECTOR_TRIM48 ((uint32_t *)0x01000448U)
#define INDEX_SECTOR_TRIM96 ((uint32_t *)0x0100044CU)
void LPC546xx_PLL_SetFrequencies(const T_LPC546xx_PLL_Frequencies *aFreq)
{
    TUInt32 clockAccess = 0;
    TUInt32 multiplier = aFreq->iPLL0Frequency / aFreq->iOscillatorClockHz;
    TUInt32 SELP, SELI, SELR;
    TUInt32 usb_adj;

    if(aFreq->iPLL0Frequency == 96000000){
        //Default to FRO for testing.
        SYSCON->MAINCLKSELA = 0x00;
        POWER_DisablePD(kPDRUNCFG_PD_FRO_EN);
        POWER_SetVoltageForFreq(96000000); /*!< Set voltage for core */

        clockAccess |= ((TUInt32)4 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);//96MHz
        SYSCON->FLASHCFG = clockAccess;

        /* Power up the FRO and set this as the base clock */
        POWER_DisablePD(kPDRUNCFG_PD_FRO_EN);

        /* back up the value of whether USB adj is selected, in which case we will have a value of 1 else 0 */
        usb_adj = ((SYSCON->FROCTRL) & SYSCON_FROCTRL_USBCLKADJ_MASK) >> SYSCON_FROCTRL_USBCLKADJ_SHIFT;

        SYSCON->FROCTRL = ((SYSCON_FROCTRL_TRIM_MASK | SYSCON_FROCTRL_FREQTRIM_MASK) & *INDEX_SECTOR_TRIM96) |
                            SYSCON_FROCTRL_SEL(1) | SYSCON_FROCTRL_WRTRIM(1) | SYSCON_FROCTRL_USBCLKADJ(usb_adj) |
                            SYSCON_FROCTRL_HSPDCLK(1);

        SYSCON->MAINCLKSELA = 0x03;
        SYSCON->MAINCLKSELB = 0x00;
        G_ProcessorFrequency = aFreq->iPLL0Frequency;
    } else {
        switch(aFreq->iClockSourceSelect){
            case LPC546xx_CLKSRC_SELECT_INTERNAL_RC:
                SYSCON->MAINCLKSELA = 0x00;
                break;
            case LPC546xx_CLKSRC_SELECT_MAIN_OSCILLATOR:
                POWER_EnablePD(kPDRUNCFG_PD_SYS_OSC);
                SYSCON->SYSPLLCLKSEL = 0x01;
                /* Turn on the ext clock if system pll input select clk_in */
                 SYSCON->PDRUNCFGCLR[0] |= SYSCON_PDRUNCFG_PDEN_VD2_ANA_MASK;
                 SYSCON->PDRUNCFGCLR[1] |= SYSCON_PDRUNCFG_PDEN_SYSOSC_MASK;

                SYSCON->MAINCLKSELA = 0x01; //use internal while using NXP board//0x01;
                break;
            default:
                while(1); //not supported
        }

        /* Enable power for PLLs */
        POWER_SetPLL();
        /* Power off PLL during setup changes */
        POWER_EnablePD(kPDRUNCFG_PD_SYS_PLL0);

        /*!< Set FLASH waitstates for core */
        clockAccess = SYSCON->FLASHCFG & ~(SYSCON_FLASHCFG_FLASHTIM_MASK);
        if (aFreq->iPLL0Frequency <= 12000000U) {
            clockAccess |= ((TUInt32)0 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
        } else if (aFreq->iPLL0Frequency <= 24000000U) {
            clockAccess |= ((TUInt32)1 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
        } else if (aFreq->iPLL0Frequency <= 36000000U) {
            clockAccess |= ((TUInt32)2 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
        } else if (aFreq->iPLL0Frequency <= 60000000U) {
            clockAccess |= ((TUInt32)3 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
        } else if (aFreq->iPLL0Frequency <= 96000000U) {
            clockAccess |= ((TUInt32)4 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
        } else if (aFreq->iPLL0Frequency <= 120000000U) {
            clockAccess |= ((TUInt32)5 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
        } else if (aFreq->iPLL0Frequency <= 144000000U) {
            clockAccess |= ((TUInt32)6 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
        } else if (aFreq->iPLL0Frequency <= 168000000U) {
            clockAccess |= ((TUInt32)7 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
        } else {
            clockAccess |= ((TUInt32)8 << SYSCON_FLASHCFG_FLASHTIM_SHIFT);
        }
        SYSCON->FLASHCFG = clockAccess;

        GetControlRegisterValues(multiplier, &SELI, &SELP, &SELR);
        /* Write PLL setup data */
        SYSCON->SYSPLLCTRL = (0 << 15) | (1 << 20) | ((SELR & 0xF) << 0) | ((SELI & 0x3F) << 4) | ((SELP & 0x1F) << 10);

        SYSCON->SYSPLLNDEC = Encode_Raw_N_Value(1); //No pre-divider //may need to change if more frequnacys are supported in uEZ
        SYSCON->SYSPLLNDEC |= (1U << SYSCON_SYSPLLNDEC_NREQ_SHIFT); /* latch */

        SYSCON->SYSPLLPDEC = Encode_Raw_P_Value(1); //No post-divider
        SYSCON->SYSPLLPDEC |= (1U << SYSCON_SYSPLLPDEC_PREQ_SHIFT); /* latch */

        SYSCON->SYSPLLMDEC = Encode_Raw_M_Value(multiplier);
        SYSCON->SYSPLLMDEC |= (1U << SYSCON_SYSPLLMDEC_MREQ_SHIFT); /* latch */

        /* If turning the PLL back on, perform the following sequence to accelerate PLL lock */
        volatile uint32_t delayX;
        uint32_t maxCCO = (1U << 18U) | 0x5dd2U; /* CCO = 1.6Ghz + MDEC enabled*/
        uint32_t curSSCTRL = SYSCON->SYSPLLMDEC & ~(1U << 17U);

        /* Initialize  and power up PLL */
        SYSCON->SYSPLLMDEC = maxCCO;
        POWER_DisablePD(kPDRUNCFG_PD_SYS_PLL0);

        /* Set mreq to activate */
        SYSCON->SYSPLLMDEC = maxCCO | (1U << 17U);

        /* Delay for 72 uSec @ 12Mhz */
        for (delayX = 0U; delayX < 172U; ++delayX)
        {
        }

        /* clear mreq to prepare for restoring mreq */
        SYSCON->SYSPLLMDEC = curSSCTRL;

        /* set original value back and activate */
        SYSCON->SYSPLLMDEC = curSSCTRL | (1U << 17U);

        /* Enable peripheral states by setting low */
        POWER_DisablePD(kPDRUNCFG_PD_SYS_PLL0);

        while (!SYSCON->SYSPLLSTAT & SYSCON_SYSPLLSTAT_LOCK_MASK)
        {
        }

        /* System voltage adjustment, occurs prior to setting main system clock */
        POWER_SetVoltageForFreq(aFreq->iPLL0Frequency);
        G_ProcessorFrequency = aFreq->iPLL0Frequency;

        switch (aFreq->iClockOutSelect){
            case LPC546xx_CLOCK_OUT_SELECT_CPU:
            case LPC546xx_CLOCK_OUT_SELECT_MAIN_OSC:
                SYSCON->MAINCLKSELB = 0x02;
                break;
            case LPC546xx_CLOCK_OUT_SELECT_IRC_OSC:
                SYSCON->MAINCLKSELB = 0x00;
                break;
            default:
                while(1); //not supported
        }
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC546xx_PLL.c
 *-------------------------------------------------------------------------*/
