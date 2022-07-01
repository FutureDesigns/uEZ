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
 * __NO_RUNFAST_MODE
 *
 *   If defined do NOT turn on flush-to-zero and default NaN modes
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
#ifdef STARTUP_FROM_RESET
  .word reset_handler
#else
  .word reset_wait
#endif /* STARTUP_FROM_RESET */
ISR_HANDLER NMI_Handler
ISR_HANDLER HardFault_Handler
        /*mov r0, #5 */
        /*b UEZBSP_FatalError */
        TST LR, #4
        ITE EQ
        MRSEQ r0, MSP
        MRSNE R0, PSP
        b HardFault_HandlerC
ISR_HANDLER MemManage_Handler
ISR_HANDLER BusFault_Handler
ISR_HANDLER UsageFault_Handler
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_RESERVED
ISR_HANDLER vPortSVCHandler //SVC_Handler
ISR_HANDLER DebugMon_Handler
ISR_RESERVED
ISR_HANDLER xPortPendSVHandler //PendSV_Handler
ISR_HANDLER xPortSysTickHandler //SysTick_Handler
// External interrupts start her 
ISR_HANDLER DAC_IRQHandler
ISR_HANDLER M0CORE_IRQHandler
ISR_HANDLER DMA_IRQHandler
ISR_RESERVED
ISR_HANDLER FLASHEEPROM_IRQHandler
ISR_HANDLER ETH_IRQHandler
ISR_HANDLER SDIO_IRQHandler
ISR_HANDLER LCD_IRQHandler
ISR_HANDLER USB0_IRQHandler
ISR_HANDLER USB1_IRQHandler
ISR_HANDLER SCT_IRQHandler
ISR_HANDLER RITIMER_IRQHandler
ISR_HANDLER TIMER0_IRQHandler
ISR_HANDLER TIMER1_IRQHandler
ISR_HANDLER TIMER2_IRQHandler
ISR_HANDLER TIMER3_IRQHandler
ISR_HANDLER MCPWM_IRQHandler
ISR_HANDLER ADC0_IRQHandler
ISR_HANDLER I2C0_IRQHandler
ISR_HANDLER I2C1_IRQHandler
ISR_HANDLER SPI_IRQHandler
ISR_HANDLER ADC1_IRQHandler
ISR_HANDLER SSP0_IRQHandler
ISR_HANDLER SSP1_IRQHandler
ISR_HANDLER USART0_IRQHandler
ISR_HANDLER UART1_IRQHandler
ISR_HANDLER USART2_IRQHandler
ISR_HANDLER USART3_IRQHandler
ISR_HANDLER I2S0_IRQHandler
ISR_HANDLER I2S1_IRQHandler
ISR_HANDLER SPIFI_IRQHandler
ISR_HANDLER SGPIO_IRQHandler
ISR_HANDLER GPIO0_IRQHandler
ISR_HANDLER GPIO1_IRQHandler
ISR_HANDLER GPIO2_IRQHandler
ISR_HANDLER GPIO3_IRQHandler
ISR_HANDLER GPIO4_IRQHandler
ISR_HANDLER GPIO5_IRQHandler
ISR_HANDLER GPIO6_IRQHandler
ISR_HANDLER GPIO7_IRQHandler
ISR_HANDLER GINT0_IRQHandler
ISR_HANDLER GINT1_IRQHandler
ISR_HANDLER EVENTROUTER_IRQHandler
ISR_HANDLER C_CAN1_IRQHandler
ISR_RESERVED
ISR_HANDLER ADCHS_IRQHandler
ISR_HANDLER ATIMER_IRQHandler
ISR_HANDLER RTC_IRQHandler
ISR_RESERVED
ISR_HANDLER WWDT_IRQHandler
ISR_HANDLER M0SUB_IRQHandler
ISR_HANDLER C_CAN0_IRQHandler
ISR_HANDLER QEI_IRQHandler
  .section .vectors, "ax"
_vectors_end:

  .section .init, "ax"  
  .align 2
  .thumb_func
  reset_handler:

#ifndef __NO_SYSTEM_INIT
  ldr sp, =__SRAM2_segment_end__
  bl SystemInit
#endif

#if defined(__FPU_PRESENT) && !defined(__SOFTFP__)
  // Enable CP11 and CP10 with CPACR |= (0xf<<20)
  movw r0, 0xED88
  movt r0, 0xE000
  ldr r1, [r0]
  orrs r1, r1, #(0xf << 20)
  str r1, [r0]
#ifndef __NO_RUNFAST_MODE
  nop
  nop
  nop  
  vmrs r0, fpscr
  orrs r0, r0, #(0x3 << 24) // FZ and DN
  vmsr fpscr, r0
  // clear the CONTROL.FPCA bit
  mov r0, #0
  msr control, r0 
  // FPDSCR similarly
  movw r1, 0xEF3C
  movt r1, 0xE000
  ldr r0, [r1]
  orrs r0, r0, #(0x3 << 24) // FZ and DN
  str r0, [r1]
#endif
#endif

#ifdef VECTORS_IN_RAM
  ldr r0, =__vectors_load_start__
  ldr r1, =__vectors_load_end__
  ldr r2, =__VECTOR_RAM
l0:
  cmp r0, r1
  beq l1
  ldr r3, [r0], #4
  str r3, [r2], #4
  b l0
l1:
#endif

  /* Configure vector table offset register */
  ldr r0, =0xE000ED08
#ifdef VECTORS_IN_RAM
  ldr r1, =__vectors_ram_start__
#else
  ldr r1, =_vectors
#endif
  str r1, [r0]

  b _start

#ifndef STARTUP_FROM_RESET
  .thumb_func
reset_wait:
1: b 1b /* endless loop */
#endif /* STARTUP_FROM_RESET */

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
