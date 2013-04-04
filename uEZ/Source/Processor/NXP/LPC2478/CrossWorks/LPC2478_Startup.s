/*****************************************************************************
 * Copyright (c) 2001, 2002 Rowley Associates Limited.                       *
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
 * VECTORED_IRQ_INTERRUPTS
 *
 *   Enable vectored IRQ interrupts. If defined, the PC register will be loaded
 *   with the contents of the VICVectAddr register on an IRQ exception.
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
 * SRAM_EXCEPTIONS
 *
 *   If defined, enable copying and re-mapping of interrupt vectors from User 
 *   FLASH to SRAM. If undefined, interrupt vectors will be mapped in User 
 *   FLASH.
 *
 *****************************************************************************/

//#include "Config.h"

#define SCB_BASE 0xE01FC000
#define MEMMAP (*(volatile unsigned char *)0xE01FC040)
#define MEMMAP_OFFSET 0x40
#define MAMCR (*(volatile unsigned char *)0xE01FC000)
#define MAMCR_OFFSET 0x0
#define MAMTIM (*(volatile unsigned char *)0xE01FC004)
#define MAMTIM_OFFSET 0x4

#ifndef MAMCR_VAL
#define MAMCR_VAL 2
#endif

#ifndef MAMTIM_VAL
#define MAMTIM_VAL 3
#endif

  .section .vectors, "ax"
  .code 32
  .align 0
  .global _vectors
  .global reset_handler
  .global UEZBSP_FatalError

/*****************************************************************************
 * Exception Vectors                                                         *
 *****************************************************************************/
_vectors:
  ldr pc, [pc, #reset_handler_address - . - 8]  /* reset */
  ldr pc, [pc, #undef_handler_address - . - 8]  /* undefined instruction */
  ldr pc, [pc, #swi_handler_address - . - 8]    /* swi handler */
  ldr pc, [pc, #pabort_handler_address - . - 8] /* abort prefetch */
  ldr pc, [pc, #dabort_handler_address - . - 8] /* abort data */
#ifdef VECTORED_IRQ_INTERRUPTS
  .word 0xB9206E58                              /* boot loader checksum */
  ldr pc, [pc, #-0x120]                         /* irq handler */
#else
  .word 0xB8A06F60                              /* boot loader checksum */
  ldr pc, [pc, #irq_handler_address - . - 8]    /* irq handler */
#endif
  ldr pc, [pc, #fiq_handler_address - . - 8]    /* fiq handler */

reset_handler_address:
#ifdef STARTUP_FROM_RESET
  .word reset_handler
#else
  .word reset_wait
#endif
undef_handler_address:
  .word undef_handler
swi_handler_address:
  .word swi_handler
pabort_handler_address:
  .word pabort_handler
dabort_handler_address:
  .word dabort_handler
#ifndef VECTORED_IRQ_INTERRUPTS
irq_handler_address:
  .word irq_handler
#endif
fiq_handler_address:
  .word fiq_handler

  .section .init, "ax"
  .code 32
  .align 0

/******************************************************************************
 *                                                                            *
 * Default exception handlers                                                 *
 *                                                                            *
 ******************************************************************************/
reset_handler:
  ldr r0, =SCB_BASE

  /* Initialise memory accelerator module */
  mov r1, #0
  str r1, [r0, #MAMCR_OFFSET]
  ldr r1, =MAMTIM_VAL
  str r1, [r0, #MAMTIM_OFFSET]
  ldr r1, =MAMCR_VAL
  str r1, [r0, #MAMCR_OFFSET]
 
#if defined(SRAM_EXCEPTIONS)
  /* Copy exception vectors into SRAM */
  mov r8, #0x40000000
  ldr r9, =_vectors
  ldmia r9!, {r0-r7}
  stmia r8!, {r0-r7}
  ldmia r9!, {r0-r6}
  stmia r8!, {r0-r6}

  /* Re-map interrupt vectors from SRAM */
  ldr r0, =SCB_BASE
  mov r1, #2 /* User RAM Mode. Interrupt vectors are re-mapped from SRAM */
  str r1, [r0, #MEMMAP_OFFSET]
#endif /* SRAM_EXCEPTIONS */

  b _start

#ifndef STARTUP_FROM_RESET
reset_wait:
  b reset_wait
#endif

/******************************************************************************
 *                                                                            *
 * Default exception handlers                                                 *
 * These are declared weak symbols so they can be redefined in user code.     * 
 *                                                                            *
 ******************************************************************************/
undef_handler:
  mov r0, #4
  b UEZBSP_FatalError
  
swi_handler:
  mov r0, #5
  b UEZBSP_FatalError
  
pabort_handler:
  mov r0, #6
  b UEZBSP_FatalError
  
dabort_handler:
  mov r0, #7
  b UEZBSP_FatalError
  
fiq_handler:
  mov r0, #8
  b UEZBSP_FatalError

irq_handler:
  mov r0, #9
  b UEZBSP_FatalError

  .weak undef_handler, swi_handler, pabort_handler, dabort_handler, fiq_handler, irq_handler
                                                    

                  
