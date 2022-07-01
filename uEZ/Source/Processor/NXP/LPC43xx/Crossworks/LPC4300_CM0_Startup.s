/*****************************************************************************
 * Copyright (c) 2012 Rowley Associates Limited.                             *
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
ISR_HANDLER SVC_Handler
ISR_HANDLER DebugMon_Handler
ISR_RESERVED
ISR_HANDLER PendSV_Handler
ISR_HANDLER SysTick_Handler
// External interrupts start her 
ISR_HANDLER RTC_IRQHandler
ISR_HANDLER M4CORE_IRQHandler
ISR_HANDLER DMA_IRQHandler
ISR_RESERVED
ISR_HANDLER FLASHEEPROM_IRQHandler
ISR_HANDLER ETHERNET_IRQHandler
ISR_HANDLER SDIO_IRQHandler
ISR_HANDLER LCD_IRQHandler
ISR_HANDLER USB0_IRQHandler
ISR_HANDLER USB1_IRQHandler
ISR_HANDLER SCT_IRQHandler
ISR_HANDLER RITIMER_OR_WWDT_IRQHandler
ISR_HANDLER TIMER0_IRQHandler
ISR_HANDLER GINT1_IRQHandler 
ISR_HANDLER PIN_INT4_IRQHandler 
ISR_HANDLER TIMER3_IRQHandler
ISR_HANDLER MCPWM_IRQHandler
ISR_HANDLER ADC0_IRQHandler
ISR_HANDLER I2C0_OR_I2C1_IRQHandler
ISR_HANDLER SGPIO_IRQHandler
ISR_HANDLER SPI_OR_DAC_IRQHandler
ISR_HANDLER ADC1_IRQHandler
ISR_HANDLER SSP0_OR_SSP1_IRQHandler
ISR_HANDLER EVENTROUTER_IRQHandler
ISR_HANDLER USART0_IRQHandler
ISR_HANDLER UART1_IRQHandler
ISR_HANDLER USART2_OR_C_CAN1_IRQHandler
ISR_HANDLER USART3_IRQHandler
ISR_HANDLER I2S0_OR_I2S1_OR_QEI_IRQHandler
ISR_HANDLER C_CAN0_IRQHandler
ISR_HANDLER SPIFI_OR_ADCHS_IRQHandler
ISR_HANDLER M0SUB_IRQHandler
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

  // CREG->M0APPMEMMAP = &_vectors
  ldr r0, =0x40043404
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
