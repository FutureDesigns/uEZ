/**************************************************************************//**
 * @file     lpc43xx_m0s.h (M0 serial core inside LPC43xx)
 * @brief    CMSIS Cortex-M0 Core Peripheral Access Layer Header File for
 *           NXP LPC11xx Device Series
 * @version  V1.00
 * @date     17. November 2009
 *
 * @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef __lpc43xx_m0s_H__
#define __lpc43xx_m0s_H__

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup LPC11xx_Definitions LPC11xx Definitions
  This file defines all structures and symbols for LPC11xx:
    - Registers and bitfields
    - peripheral base address
    - peripheral ID
    - PIO definitions
  @{
*/


/******************************************************************************/
/*                Processor and Core Peripherals                              */
/******************************************************************************/
/** @addtogroup LPC11xx_CMSIS LPC11xx CMSIS Definitions
  Configuration of the Cortex-M0 Processor and Core Peripherals
  @{
*/

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

typedef enum IRQn
{
// -------------------------  Cortex-M0 Processor Exceptions Numbers  -----------------------------
  NonMaskableInt_IRQn           = -14,    /*!< 2 Non Maskable Interrupt                           */
  HardFault_IRQn                = -13,    /*!< 3 Cortex-M0 Hard Fault Interrupt                   */
  SVCall_IRQn                   = -5,     /*!< 11 Cortex-M0 SV Call Interrupt                     */
  PendSV_IRQn                   = -2,     /*!< 14 Cortex-M0 Pend SV Interrupt                     */
  SysTick_IRQn                  = -1,     /*!< 15 Cortex-M0 System Tick Interrupt                 */
// ---------------------------  lpc43xx_M0s Specific Interrupt Numbers  ---------------------------
  DAC_IRQn                          = 0,    /*!<   0  DAC                              */
  M3_IRQn                           = 1,    /*!<   1  M3                               */
  DMA_IRQn                          = 2,    /*!<   2  DMA                              */
  EZH_IRQn                          = 3,    /*!<   3  EZH/EDM                          */
  SGPIO_INPUT_IRQn                  = 4,    /*!<   4  SGPIO input                      */
  SGPIO_MATCH_IRQn                  = 5,    /*!<   4  SGPIO match                      */
  SGPIO_SHIFT_IRQn                  = 6,    /*!<   4  SGPIO shift                      */
  SGPIO_POS_IRQn                    = 7,    /*!<   4  SGPIO pos                        */
  USB0_IRQn                         = 8,    /*!<   8  USB0                             */
  USB1_IRQn                         = 9,    /*!<   9  USB1                             */
  SCT_IRQn                          = 10,   /*!<  10  SCT                              */
  RITIMER_IRQn                      = 11,   /*!<  11  RITIMER                          */
  GINT0_IRQn                        = 12,   /*!<  12  GINT0                            */
  TIMER1_IRQn                       = 13,   /*!<  13  TIMER1_INT                       */
  TIMER2_IRQn                       = 14,   /*!<  14  TIMER2_INT                       */
  PIN_INT5_IRQn                     = 15,   /*!<  15  PIN_INT5                         */
  MCPWM_IRQn                        = 16,   /*!<  16  MCPWM                            */
  ADC0_IRQn                         = 17,   /*!<  17  ADC0                             */
  I2C0_IRQn                         = 18,   /*!<  18  I2C0                             */
  I2C1_IRQn                         = 19,   /*!<  19  I2C1                             */
  SPI_IRQn                          = 20,   /*!<  20  SPI                              */
  ADC1_IRQn                         = 21,   /*!<  21  ADC1                             */
  SSP0_SSP1_IRQn                    = 22,   /*!<  22  SSP0 and SSP1 or'ed              */
  EVENTROUTER_INT_IRQn              = 23,   /*!<  23  EVENTROUTER_INT                  */
  USART0_IRQn                       = 24,   /*!<  24  USART0                           */
  UART1_IRQn                        = 25,   /*!<  25  UART1                            */
  USART2_C_CAN1_IRQn                = 26,   /*!<  26  USART2 and C_CAN1 or'ed          */
  USART3_IRQn                       = 27,   /*!<  27  USART3                           */
  I2S0_I2S1_QEI_IRQn                = 28,   /*!<  28  I2S0, I2S1 and QEI or'ed         */
  C_CAN0_IRQn                       = 29,   /*!<  29  C_CAN0                           */
  SPIFI_VADC_IRQn                   = 30,   /*!<  30  SPIFI and VADC or'ed             */
  M0a_IRQn                          = 31,   /*!<  31  M0a                              */
} IRQn_Type;

#include "lpc43xx.h"

#ifdef __cplusplus
}
#endif

#endif  /* __lpc43xx_m0s_H__ */
