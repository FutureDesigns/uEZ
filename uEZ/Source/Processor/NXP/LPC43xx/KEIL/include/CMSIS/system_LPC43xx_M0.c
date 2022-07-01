/* -----------------------------------------------------------------------------
 * Copyright (c) 2013 - 2015 ARM Ltd.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * $Date:        26. August 2015
 * $Revision:    V5.0.1
 *
 * Project:      NXP LPC43xx System functions for M0APP and M0SUB core
 * -------------------------------------------------------------------------- */

#include "LPC43xx.h"

/*----------------------------------------------------------------------------
  Clock source selection definitions (do not change)
 *----------------------------------------------------------------------------*/
#define CLK_SRC_32KHZ       0x00
#define CLK_SRC_IRC         0x01
#define CLK_SRC_ENET_RX     0x02
#define CLK_SRC_ENET_TX     0x03
#define CLK_SRC_GP_CLKIN    0x04
#define CLK_SRC_XTAL        0x06
#define CLK_SRC_PLL0U       0x07
#define CLK_SRC_PLL0A       0x08
#define CLK_SRC_PLL1        0x09
#define CLK_SRC_IDIVA       0x0C
#define CLK_SRC_IDIVB       0x0D
#define CLK_SRC_IDIVC       0x0E
#define CLK_SRC_IDIVD       0x0F
#define CLK_SRC_IDIVE       0x10


/*----------------------------------------------------------------------------
  Define external input frequency values
 *----------------------------------------------------------------------------*/
#define CLK_32KHZ            32768UL    /* 32 kHz oscillator frequency        */
#define CLK_IRC           12000000UL    /* Internal oscillator frequency      */
#define CLK_ENET_RX       50000000UL    /* Ethernet Rx frequency              */
#define CLK_ENET_TX       50000000UL    /* Ethernet Tx frequency              */
#define CLK_GP_CLKIN      12000000UL    /* General purpose clock input freq.  */
#define CLK_XTAL          12000000UL    /* Crystal oscilator frequency        */


/*----------------------------------------------------------------------------
  System Core Clock variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = CLK_IRC;    /* System Clock Frequency (Core Clock) */


/*----------------------------------------------------------------------------
  Get PLL1 (divider and multiplier) parameters
 *----------------------------------------------------------------------------*/
static __inline uint32_t GetPLL1Param (void) {
  uint32_t ctrl;
  uint32_t p;
  uint32_t div, mul;

  ctrl = LPC_CGU->PLL1_CTRL;
  div = ((ctrl >> 12) & 0x03) + 1;
  mul = ((ctrl >> 16) & 0xFF) + 1;
  p = 1 << ((ctrl >>  8) & 0x03);

  if (ctrl & (1 << 1)) {
    /* Bypass = 1, PLL1 input clock sent to post-dividers */
    if (ctrl & (1 << 7)) {
      div *= (2*p);
    }
  }
  else {
    /* Direct and integer mode */
    if (((ctrl & (1 << 7)) == 0) && ((ctrl & (1 << 6)) == 0)) {
      /* Non-integer mode */
      div *= (2*p);
    }
  }
  return ((div << 8) | (mul));
}


/*----------------------------------------------------------------------------
  Get input clock source for specified clock generation block
 *----------------------------------------------------------------------------*/
int32_t GetClkSel (uint32_t clk_src) {
  uint32_t reg;
  int32_t clk_sel = -1;

  switch (clk_src) {
    case CLK_SRC_IRC:
    case CLK_SRC_ENET_RX:
    case CLK_SRC_ENET_TX:
    case CLK_SRC_GP_CLKIN:
      return (clk_src);

    case CLK_SRC_32KHZ:
      return ((LPC_CREG->CREG0 & 0x0A) != 0x02) ? (-1) : (CLK_SRC_32KHZ);
    case CLK_SRC_XTAL:
     return  (LPC_CGU->XTAL_OSC_CTRL & 1)       ? (-1) : (CLK_SRC_XTAL);

    case CLK_SRC_PLL0U: reg = LPC_CGU->PLL0USB_CTRL;    break;
    case CLK_SRC_PLL0A: reg = LPC_CGU->PLL0AUDIO_CTRL;  break;
    case CLK_SRC_PLL1:  reg = (LPC_CGU->PLL1_STAT & 1) ? (LPC_CGU->PLL1_CTRL) : (0); break;

    case CLK_SRC_IDIVA: reg = LPC_CGU->IDIVA_CTRL;      break;
    case CLK_SRC_IDIVB: reg = LPC_CGU->IDIVB_CTRL;      break;
    case CLK_SRC_IDIVC: reg = LPC_CGU->IDIVC_CTRL;      break;
    case CLK_SRC_IDIVD: reg = LPC_CGU->IDIVD_CTRL;      break;
    case CLK_SRC_IDIVE: reg = LPC_CGU->IDIVE_CTRL;      break;

    default:
      return (clk_sel);
  }
  if (!(reg & 1)) {
    clk_sel = (reg >> 24) & 0x1F;
  }
  return (clk_sel);
}


/*----------------------------------------------------------------------------
  Get clock frequency for specified clock source
 *----------------------------------------------------------------------------*/
uint32_t GetClockFreq (uint32_t clk_src) {
  uint32_t tmp;
  uint32_t mul        =  1;
  uint32_t div        =  1;
  uint32_t main_freq  =  0;
  int32_t  clk_sel    = clk_src;

  do {
    switch (clk_sel) {
      case CLK_SRC_32KHZ:    main_freq = CLK_32KHZ;     break;
      case CLK_SRC_IRC:      main_freq = CLK_IRC;       break;
      case CLK_SRC_ENET_RX:  main_freq = CLK_ENET_RX;   break;
      case CLK_SRC_ENET_TX:  main_freq = CLK_ENET_TX;   break;
      case CLK_SRC_GP_CLKIN: main_freq = CLK_GP_CLKIN;  break;
      case CLK_SRC_XTAL:     main_freq = CLK_XTAL;      break;

      case CLK_SRC_IDIVA: div *= ((LPC_CGU->IDIVA_CTRL >> 2) & 0x03) + 1; break;
      case CLK_SRC_IDIVB: div *= ((LPC_CGU->IDIVB_CTRL >> 2) & 0x0F) + 1; break;
      case CLK_SRC_IDIVC: div *= ((LPC_CGU->IDIVC_CTRL >> 2) & 0x0F) + 1; break;
      case CLK_SRC_IDIVD: div *= ((LPC_CGU->IDIVD_CTRL >> 2) & 0x0F) + 1; break;
      case CLK_SRC_IDIVE: div *= ((LPC_CGU->IDIVE_CTRL >> 2) & 0xFF) + 1; break;

      case CLK_SRC_PLL0U: /* Not implemented */  break;
      case CLK_SRC_PLL0A: /* Not implemented */  break;

      case CLK_SRC_PLL1:
        tmp = GetPLL1Param ();
        mul *= (tmp     ) & 0xFF;       /* PLL input clock multiplier         */
        div *= (tmp >> 8) & 0xFF;       /* PLL input clock divider            */
        break;

      default:
        return (0);                     /* Clock not running or not supported */
    }
    if (main_freq == 0) {
      clk_sel = GetClkSel (clk_sel);
    }
  }
  while (main_freq == 0);

  return ((main_freq * mul) / div);
}


/*----------------------------------------------------------------------------
  System Core Clock update
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void) {
  uint32_t base_src;
  
#ifdef CORE_M0SUB
  base_src = (LPC_CGU->BASE_PERIPH_CLK >> 24) & 0x1F;
#else
  base_src = (LPC_CGU->BASE_M4_CLK     >> 24) & 0x1F;
#endif

  /* Update core clock frequency */
  SystemCoreClock = GetClockFreq (base_src);
}


/*----------------------------------------------------------------------------
  Initialize the system
 *----------------------------------------------------------------------------*/
void SystemInit (void) {

  /* Update SystemCoreClock variable */
  SystemCoreClockUpdate();
}
