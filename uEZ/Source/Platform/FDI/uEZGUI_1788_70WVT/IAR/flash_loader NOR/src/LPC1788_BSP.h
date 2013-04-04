/*-------------------------------------------------------------------------*
 * File:  LPC1788_BSP.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC1788_BSP_H_
#define LPC1788_BSP_H_

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

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000

#if (PROCESSOR_OSCILLATOR_FREQUENCY==96000000)
    #define CLOCK_SETUP           1
    #define SCS_Val               0x00000020
    #define CLKSRCSEL_Val         0x00000001
    #define PLL0_SETUP            1
    #define PLL0CFG_Val           0x00000007
    #define PLL1_SETUP            1
    #define PLL1CFG_Val           0x00000023
    #define CCLKSEL_Val           0x00000001
    #define USBSEL_Val            0x00000001
    #define PCLKSEL_Val           0x00000002
    #define EMCCLKSEL_Val         1 // 0 // was 0x00000001
    #define SPIFISEL_Val          0x00000001
    #define PCONP_Val             0x042887DE
    #define CLKOUTCFG_Val         0x00000100
    #if (EMCCLKSEL_Val&1)
        #define SDRAM_CLOCK_FREQUENCY   (PROCESSOR_OSCILLATOR_FREQUENCY/2)
    #else
        #define SDRAM_CLOCK_FREQUENCY   (PROCESSOR_OSCILLATOR_FREQUENCY/1)
    #endif
#elif (PROCESSOR_OSCILLATOR_FREQUENCY==120000000)
    #define CLOCK_SETUP           1
    #define SCS_Val               0x00000020
    #define CLKSRCSEL_Val         0x00000001
    #define PLL0_SETUP            1
    #define PLL0CFG_Val           (10-1) // Multiple by 10
    #define PLL1_SETUP            1
    #define PLL1CFG_Val           0x00000023
    #define CCLKSEL_Val           1 // divide by 1
    #define USBSEL_Val            0x00000001
    #define PCLKSEL_Val           0x00000002
    #define EMCCLKSEL_Val         1 // 0 // was 0x00000001
    #define SPIFISEL_Val          0x00000001
    #define PCONP_Val             0x042887DE
    #define CLKOUTCFG_Val         0x00000100
    #if (EMCCLKSEL_Val&1)
        #define SDRAM_CLOCK_FREQUENCY   (PROCESSOR_OSCILLATOR_FREQUENCY/2)
    #else
        #define SDRAM_CLOCK_FREQUENCY   (PROCESSOR_OSCILLATOR_FREQUENCY/1)
    #endif
#elif (PROCESSOR_OSCILLATOR_FREQUENCY==80000000)
    #define CLOCK_SETUP           1
    #define SCS_Val               0x00000020
    #define CLKSRCSEL_Val         0x00000001
    #define PLL0_SETUP            1
    #define PLL0CFG_Val           (20-1) // Multiple by 20 (20*12=240)
    #define PLL1_SETUP            1
    #define PLL1CFG_Val           0x00000023
    #define CCLKSEL_Val           3  // divide by 3 (240/3 = 80 MHz)
    #define USBSEL_Val            0x00000001
    #define PCLKSEL_Val           0x00000002
    #define EMCCLKSEL_Val         0 // was 0x00000001
    #define SPIFISEL_Val          0x00000001
    #define PCONP_Val             0x042887DE
    #define CLKOUTCFG_Val         0x00000100
    #if (EMCCLKSEL_Val&1)
        #define SDRAM_CLOCK_FREQUENCY   (PROCESSOR_OSCILLATOR_FREQUENCY/2)
    #else
        #define SDRAM_CLOCK_FREQUENCY   (PROCESSOR_OSCILLATOR_FREQUENCY/1)
    #endif
#else
    #error "Do not know how to configure that processor clock frequency"
#endif

#define FLASH_CLOCK_FREQUENCY       SDRAM_CLOCK_FREQUENCY
#define FLASH_SETUP           1
#define FLASHCFG_Val          0x0000503A

#define XTAL        PROCESSOR_OSCILLATOR_FREQUENCY        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define RTC_CLK     (   32000UL)        /* RTC oscillator frequency           */
#define IRC_OSC     (12000000UL)        /* Internal RC oscillator frequency   */

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern TUInt32 SystemFrequency; /*!< System Clock Frequency (Core Clock)  */
extern TUInt32 PeripheralFrequency;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC1788_BSP_PLLConfigure(void);
void LPC1788_BSP_InitializeNORFlash(void);
void LPC1788_BSP_PinConfigInit(void);
void LPC1788_BSP_SDRAMInit(void);
void LPC1788_BSP_UartInit(void);

#endif // LPC1788_BSP_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1788_BSP.h
 *-------------------------------------------------------------------------*/
