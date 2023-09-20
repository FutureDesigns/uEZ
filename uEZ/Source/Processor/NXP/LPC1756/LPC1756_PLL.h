/*-------------------------------------------------------------------------*
 * File:  LPC1756_PLL.h
 *-------------------------------------------------------------------------*
 * Description:
 *    See Figure 7. Clock Generation for the LPC178x/LPC177x
 *-------------------------------------------------------------------------*/
#ifndef LPC1756_PLL_H_
#define LPC1756_PLL_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
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

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define LPC1756_IRC_FREQUENCY       12000000 // Hz +/- 1%

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    LPC1756_CLKSRC_SELECT_INTERNAL_RC = 0,
    LPC1756_CLKSRC_SELECT_MAIN_OSCILLATOR = 1,
    LPC1756_CLKSRC_SELECT_RTC_OSCILLATOR = 2,
} T_LPC1756_ClockSourceSelect;

typedef enum {
    // sysclk
    LPC1756_CPU_CLOCK_SELECT_SYSCLK = 0,

    // pll_clk
    LPC1756_CPU_CLOCK_SELECT_PLL_CLK = 1,
} T_LPC1756_CPUClockSelect;

typedef enum {
    // sysclk
    LPC1756_USB_CLOCK_SELECT_SYSCLK = 0,

    // pll_clk
    LPC1756_USB_CLOCK_SELECT_PLL_CLK = 1,

    // alt_pll_clk
    LPC1756_USB_CLOCK_SELECT_ALT_PLL_CLK = 2,
} T_LPC1756_USBClockSelect;

typedef enum {
    LPC1756_CLOCK_OUT_SELECT_CPU = 0,
    LPC1756_CLOCK_OUT_SELECT_MAIN_OSC = 1,
    LPC1756_CLOCK_OUT_SELECT_IRC_OSC = 2,
    LPC1756_CLOCK_OUT_SELECT_USB = 3,
    LPC1756_CLOCK_OUT_SELECT_RTC = 4,
} T_LPC1756_ClockOutSelect;

typedef struct {
        // Input frequencies:
        TUInt32 iOscillatorClockHz;

        // PLL frequencies:
        // PLL0 is the processor frequency:
        TUInt32 iPLL0Frequency;
        // PLL1 is the USB frequency, usually 48 MHz
        TUInt32 iPLL1Frequency;

        // Selectors:
        T_LPC1756_ClockSourceSelect iClockSourceSelect;
        T_LPC1756_CPUClockSelect iCPUClockSelect;
        T_LPC1756_USBClockSelect iUSBClockSelect;

        // Dividers:
        TUInt8 iCPUClockDivider; // 1 to 31
        TUInt8 iPeripheralClockDivider; // 1 to 31
        TUInt8 iEMCClockDivider; // 1 or 2
        TUInt8 iUSBClockDivider;

        // Internal frequencies
        //   pll_clk     = output ClockSourceSelect -> PLL0
        //   sysclk      = output ClockSourceSelect
        //   alt_pll_clk = output ClockSourceSelect -> PLL1
        // Output:
        //   cclk    = output CPUClockSelect / CPUClockDivider
        //   pclk    = output CPUClockSelect / iPeripheralClockDivider
        //   emc_clk = output CPUClockSelect / iEMCClockDivider
        //   usb_clk = output USBClockSelect / iUSBClockDivider

        // CLK OUT selection:
        T_LPC1756_ClockOutSelect iClockOutSelect;
        TUInt8 iClockOutDivider; // 1-16
        TBool iClockOutEnable;
} T_LPC1756_PLL_Frequencies;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern TUInt32 G_OscillatorFrequency IN_INTERNAL_RAM;
extern TUInt32 G_ProcessorFrequency IN_INTERNAL_RAM;
extern TUInt32 G_PeripheralFrequency IN_INTERNAL_RAM;
extern TUInt32 G_EMCFrequency IN_INTERNAL_RAM;
extern TUInt32 G_USBFrequency IN_INTERNAL_RAM;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC1756_PLL_SetFrequencies(const T_LPC1756_PLL_Frequencies *aFreq);

#ifdef __cplusplus
}
#endif

#endif // LPC1756_PLL_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_PLL.h
 *-------------------------------------------------------------------------*/
