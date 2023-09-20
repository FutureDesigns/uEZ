/*****************************************************************************
 * Copyright (c) 2013 Rowley Associates Limited.                             *
 *                                                                           *
 * This file may be distributed under the terms of the License Agreement     *
 * provided with this software.                                              *
 *                                                                           *
 * THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE   *
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. *
 *****************************************************************************/

 /*****************************************************************************
 *                           Preprocessor Definitions
 *                           ------------------------
 *
 *
 * __NO_SYSTEM_INIT
 *
 *   If defined, the SystemInit() function will NOT be called. By default SystemInit()
 *   is called after reset to enable the clocks and memories to be initialised 
 *   prior to any C startup initialisation.
 *
 * VECTORS_IN_RAM
 *
 *   If defined then the exception vectors are copied from Flash to RAM
 *
 *
 */

.macro ISR_HANDLER name=
  .section .vectors, "ax"
  .word \name
  .section .init, "ax"
  .thumb_func
  .weak \name
\name:
1: b 1b /* endless loop */
.endm

.macro ISR_RESERVED
  .section .vectors, "ax"
  .word 0
.endm

  .syntax unified
  .global reset_handler

  .section .vectors, "ax"
  .code 16
  .global _vectors

_vectors:
  .word __stack_end__
  .word reset_handler
ISR_HANDLER NMI_Handler
ISR_HANDLER HardFault_Handler
ISR_HANDLER MemManage_Handler
ISR_HANDLER BusFault_Handler
ISR_HANDLER UsageFault_Handler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER vPortSVCHandler // SVC_Handler
ISR_HANDLER DebugMon_Handler
ISR_RESERVED
ISR_HANDLER xPortPendSVHandler // PendSV_Handler
ISR_HANDLER SysTick_Handler // No Systick on CM0 on this part!
// External interrupts start her 
ISR_HANDLER M0S_DAC_IRQHandler
ISR_HANDLER M0S_M4CORE_IRQHandler
ISR_HANDLER M0S_DMA_IRQHandler
ISR_RESERVED
ISR_HANDLER M0S_SGPIO_INPUT_IRQHandler
ISR_HANDLER M0S_SGPIO_MATCH_IRQHandler
ISR_HANDLER M0S_SGPIO_SHIFT_IRQHandler
ISR_HANDLER M0S_SGPIO_POS_IRQHandler
ISR_HANDLER M0S_USB0_IRQHandler
ISR_HANDLER M0S_USB1_IRQHandler
ISR_HANDLER M0S_SCT_IRQHandler
ISR_HANDLER M0S_RITIMER_IRQHandler
ISR_HANDLER M0S_GINT1_IRQHandler
ISR_HANDLER M0S_TIMER1_IRQHandler 
ISR_HANDLER M0S_TIMER2_IRQHandler 
ISR_HANDLER M0S_PIN_INT5_IRQHandler
ISR_HANDLER M0S_MCPWM_IRQHandler
ISR_HANDLER M0S_ADC0_IRQHandler
ISR_HANDLER M0S_I2C0_IRQHandler
ISR_HANDLER M0S_I2C1_IRQHandler
ISR_HANDLER M0S_SPI_IRQHandler
ISR_HANDLER M0S_ADC1_IRQHandler
ISR_HANDLER M0S_SSP0_OR_SSP1_IRQHandler
ISR_HANDLER M0S_EVENTROUTER_IRQHandler
ISR_HANDLER M0S_USART0_IRQHandler
ISR_HANDLER M0S_UART1_IRQHandler
ISR_HANDLER M0S_USART2_OR_C_CAN1_IRQHandler
ISR_HANDLER M0S_USART3_IRQHandler
ISR_HANDLER M0S_I2S0_OR_I2S1_OR_QEI_IRQHandler
ISR_HANDLER M0S_C_CAN0_IRQHandler
ISR_HANDLER M0S_SPIFI_OR_ADCHS_IRQHandler
ISR_HANDLER M0S_M0APP_IRQHandler
  .section .vectors, "ax"
_vectors_end:

  .section .init, "ax"
  .align 2
  .thumb_func
  reset_handler:

#ifndef __NO_SYSTEM_INIT
  ldr r0, =__SRAM2_segment_end__
  mov sp, r0
  bl SystemInit
#endif

  // CREG->M0SUBMEMMAP = &_vectors
  ldr r0, =0x40043308
  ldr r1, =_vectors
  str r1, [r0]

  b _start

#ifndef __NO_SYSTEM_INIT
  .thumb_func
  .weak SystemInit
SystemInit:
  bx lr
#endif

#ifdef VECTORS_IN_RAM
  .section .vectors_ram, "ax"
  .global __VECTOR_RAM
__VECTOR_RAM:
  .space _vectors_end-_vectors, 0
#endif
