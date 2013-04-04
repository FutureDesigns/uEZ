/*****************************************************************************
  IRQ handler for a VIC located at 0xFFFFF000

  Copyright (c) 2005, 2007 Rowley Associates Limited.

  This file may be distributed under the terms of the License Agreement
  provided with this software. 
 
  THIS FILE IS PROVIDED AS IS WITH NO WARRANTY OF ANY KIND, INCLUDING THE
  WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *****************************************************************************/

#include "Config.h"

  .code 32
  .global irq_handler

#ifdef PL192
#define VICVECTADDR 0xF00
#else
#define VICVECTADDR 0x30
#endif

irq_handler:
#ifndef PL192
  // check if I bit is set in SPSR to detect spurious interrupts
  stmfd sp!, {r0}
  mrs r0, spsr
  tst r0, #0x80
  ldmfd sp!, {r0}
  subnes pc, lr, #4
#endif
#ifdef CTL_TASKING
  // store all registers in the tasking case
  stmfd sp!, {r0-r12, lr}
  mrs r0, spsr
  stmfd sp!, {r0}
  //ctl_interrupt_count++;
  ldr r2, =ctl_interrupt_count
  ldrb r3, [r2]
  add r3, r3, #1
  strb r3, [r2]
#else 
  // store the APCS registers in the non-tasking case
  stmfd sp!, {r0-r4, r12, lr}
#endif   
  //((CTL_ISR_FN_t)VICVectAddr)(VICIRQStatus);
  ldr r0, =0xFFFFF000
  ldr r1, [r0, #VICVECTADDR]
  ldr r0, [r0]
#ifdef NESTED_INTERRUPTS
  mrs r2, spsr
  stmfd sp!, {r2}
  msr cpsr_c, #0x1f // system mode, interrupts enabled
  stmfd sp!, {lr}
#endif
  mov lr, pc
  bx r1
#ifdef NESTED_INTERRUPTS
  ldmfd sp!, {lr}
  msr cpsr_c, #0x92 // irq mode, interrupts disabled
  ldmfd sp!, {r2}
  msr spsr_cxsf, r2
#endif
  // VICVectAddr = 0;
  mov r3, #0
  ldr r2, =0xFFFFF000
  str r3, [r2, #VICVECTADDR]
#ifdef CTL_TASKING
  // return from interrupt via ctl_exit_isr
  mov r0, sp;
  ldr r1, =ctl_exit_isr
  bx r1
#else
  // return from interrupt
  ldmfd sp!, {r0-r4, r12, lr}
  subs pc, lr, #4
#endif

