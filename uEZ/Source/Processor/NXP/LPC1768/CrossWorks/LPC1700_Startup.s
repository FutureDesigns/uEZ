/*****************************************************************************
 * Copyright (c) 2009 Rowley Associates Limited.                             *
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
 * STARTUP_FROM_RESET
 *
 *   If defined, the program will startup from power-on/reset. If not defined
 *   the program will just loop endlessly from power-on/reset.
 *
 *   This definition is not defined by default on this target because the
 *   debugger is unable to reset this target and maintain control of it over the
 *   JTAG interface. The advantage of doing this is that it allows the debugger
 *   to reset the CPU and run programs from a known reset CPU state on each run.
 *   It also acts as a safety net if you accidently download a program in FLASH
 *   that crashes and prevents the debugger from taking control over JTAG
 *   rendering the target unusable over JTAG. The obvious disadvantage of doing
 *   this is that your application will not startup without the debugger.
 *
 *   We advise that on this target you keep STARTUP_FROM_RESET undefined whilst
 *   you are developing and only define STARTUP_FROM_RESET when development is
 *   complete.
 *
 * NO_SYSTEM_INIT
 *
 *   If defined, the SystemInit() function will NOT be called. By default 
 *   SystemInit() is called after reset to enable the clocks and memories to
 *   be initialised prior to any C startup initialisation.
 *
 *
 *****************************************************************************/

  .global reset_handler

  .syntax unified

  .section .vectors, "ax"
  .code 16
  .align 0
  .global _vectors

.macro DEFAULT_ISR_HANDLER name=
  .thumb_func
  .weak \name
\name:
1: b 1b /* endless loop */
.endm

_vectors:
  .word __stack_end__
#ifdef STARTUP_FROM_RESET
  .word reset_handler
#else
  .word reset_wait
#endif /* STARTUP_FROM_RESET */
  .word NMI_Handler
  .word HardFault_Handler
  .word MemManage_Handler
  .word BusFault_Handler
  .word UsageFault_Handler
  .word 0 // Reserved
  .word 0 // Reserved
  .word 0 // Reserved
  .word 0 // Reserved
  .word SVC_Handler
  .word DebugMon_Handler
  .word 0 // Reserved
  .word PendSV_Handler
  .word SysTick_Handler 
  .word WDT_IRQHandler
  .word TIMER0_IRQHandler
  .word TIMER1_IRQHandler
  .word TIMER2_IRQHandler
  .word TIMER3_IRQHandler
  .word UART0_IRQHandler
  .word UART1_IRQHandler
  .word UART2_IRQHandler
  .word UART3_IRQHandler
  .word PWM1_IRQHandler
  .word I2C0_IRQHandler
  .word I2C1_IRQHandler
  .word I2C2_IRQHandler
  .word SPI_IRQHandler
  .word SSP0_IRQHandler
  .word SSP1_IRQHandler
  .word PLL0_IRQHandler
  .word RTC_IRQHandler
  .word EINT0_IRQHandler
  .word EINT1_IRQHandler
  .word EINT2_IRQHandler
  .word EINT3_IRQHandler
  .word ADC_IRQHandler
  .word BOD_IRQHandler
  .word USB_IRQHandler
  .word CAN_IRQHandler
  .word GPDMA_IRQHandler
  .word I2S_IRQHandler
  .word ENET_IRQHandler
  .word RIT_IRQHandler
  .word MCPWM_IRQHandler
  .word QEI_IRQHandler
  .word PLL1_IRQHandler
  .word USBACT_IRQHandler
  .word CANACT_IRQHandler

  .section .init, "ax"
  .thumb_func

reset_handler:
#ifndef NO_SYSTEM_INIT
  ldr sp, =__RAM_segment_end__
  ldr r0, =SystemInit
  blx r0
#endif

  /* Configure vector table offset register */
  ldr r0, =0xE000ED08
  ldr r1, =_vectors
  str r1, [r0]

  b _start

DEFAULT_ISR_HANDLER NMI_Handler
DEFAULT_ISR_HANDLER HardFault_Handler
DEFAULT_ISR_HANDLER MemManage_Handler
DEFAULT_ISR_HANDLER BusFault_Handler
DEFAULT_ISR_HANDLER UsageFault_Handler
DEFAULT_ISR_HANDLER SVC_Handler 
DEFAULT_ISR_HANDLER DebugMon_Handler
DEFAULT_ISR_HANDLER PendSV_Handler
DEFAULT_ISR_HANDLER SysTick_Handler 
DEFAULT_ISR_HANDLER WDT_IRQHandler
DEFAULT_ISR_HANDLER TIMER0_IRQHandler
DEFAULT_ISR_HANDLER TIMER1_IRQHandler
DEFAULT_ISR_HANDLER TIMER2_IRQHandler
DEFAULT_ISR_HANDLER TIMER3_IRQHandler
DEFAULT_ISR_HANDLER UART0_IRQHandler
DEFAULT_ISR_HANDLER UART1_IRQHandler
DEFAULT_ISR_HANDLER UART2_IRQHandler
DEFAULT_ISR_HANDLER UART3_IRQHandler
DEFAULT_ISR_HANDLER PWM1_IRQHandler
DEFAULT_ISR_HANDLER I2C0_IRQHandler
DEFAULT_ISR_HANDLER I2C1_IRQHandler
DEFAULT_ISR_HANDLER I2C2_IRQHandler
DEFAULT_ISR_HANDLER SPI_IRQHandler
DEFAULT_ISR_HANDLER SSP0_IRQHandler
DEFAULT_ISR_HANDLER SSP1_IRQHandler
DEFAULT_ISR_HANDLER PLL0_IRQHandler
DEFAULT_ISR_HANDLER RTC_IRQHandler
DEFAULT_ISR_HANDLER EINT0_IRQHandler
DEFAULT_ISR_HANDLER EINT1_IRQHandler
DEFAULT_ISR_HANDLER EINT2_IRQHandler
DEFAULT_ISR_HANDLER EINT3_IRQHandler
DEFAULT_ISR_HANDLER ADC_IRQHandler
DEFAULT_ISR_HANDLER BOD_IRQHandler
DEFAULT_ISR_HANDLER USB_IRQHandler
DEFAULT_ISR_HANDLER CAN_IRQHandler
DEFAULT_ISR_HANDLER GPDMA_IRQHandler
DEFAULT_ISR_HANDLER I2S_IRQHandler
DEFAULT_ISR_HANDLER ENET_IRQHandler
DEFAULT_ISR_HANDLER RIT_IRQHandler
DEFAULT_ISR_HANDLER MCPWM_IRQHandler
DEFAULT_ISR_HANDLER QEI_IRQHandler
DEFAULT_ISR_HANDLER PLL1_IRQHandler
DEFAULT_ISR_HANDLER USBACT_IRQHandler
DEFAULT_ISR_HANDLER CANACT_IRQHandler

#ifndef STARTUP_FROM_RESET
DEFAULT_ISR_HANDLER reset_wait
#endif /* STARTUP_FROM_RESET */

