;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Part one of the system initialization code,
;; contains low-level
;; initialization.
;;
;; Copyright 2007 IAR Systems. All rights reserved.
;;
;; $Revision: 23570 $
;;

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION IRQ_STACK:DATA:NOROOT(3)
        SECTION FIQ_STACK:DATA:NOROOT(3)
        SECTION ABT_STACK:DATA:NOROOT(3)
        SECTION CSTACK:DATA:NOROOT(3)

;
; The module in this file are included in the libraries, and may be
; replaced by any user-defined modules that define the PUBLIC symbol
; __iar_program_start or a user defined start symbol.
;
; To override the cstartup defined in the library, simply add your
; modified version to the workbench project.

        SECTION .intvec:CODE:NOROOT(2)

        PUBLIC  __vector
        PUBLIC  __vector_0x14
        PUBLIC  __iar_program_start
        //EXTERN  Undefined_Handler
        EXTERN  SWI_Handler
        //EXTERN  Prefetch_Handler
        //EXTERN  Abort_Handler
        //EXTERN  IRQ_Handler
        //EXTERN  FIQ_Handler
        EXTERN  vPortYieldProcessor
        //EXTERN  SWI_Handler
        EXTERN  UEZBSP_FatalError
        EXTERN  LPC2478_AbortHandler
        EXTERN  LPC2478_PrefetchHandler
        EXTERN G_dabort_dump
        EXTERN SystemInit
        
        ARM

__iar_init$$done:               ; The vector table is not needed
                                ; until after copy initialization is done

__vector:
        ; All default exception handlers (except reset) are
        ; defined as weak symbol definitions.
        ; If a handler is defined by the application it will take precedence.
        LDR     PC,Reset_Addr           ; Reset
        LDR     PC,Undefined_Addr       ; Undefined instructions
        LDR     PC,SWI_Addr             ; Software interrupt (SWI/SVC)
        LDR     PC,Prefetch_Addr        ; Prefetch abort
        LDR     PC,Abort_Addr           ; Data abort
__vector_0x14:
        DCD     0                       ; RESERVED
;        LDR     PC,IRQ_Addr             ; IRQ
        LDR     PC, [PC, #-0x120] ; go to the vector table
        LDR     PC,FIQ_Addr             ; FIQ

	DATA
	
Reset_Addr:     DCD   __iar_program_start
Undefined_Addr: DCD   Undefined_Handler
SWI_Addr:       DCD   SWI_Handler
//SWI_Addr:       DCD   vPortYieldProcessor
Prefetch_Addr:  DCD   Prefetch_Handler
Abort_Addr:     DCD   Abort_Handler
IRQ_Addr:       DCD   IRQ_Handler
FIQ_Addr:       DCD   FIQ_Handler


; --------------------------------------------------
; ?cstartup -- low-level system initialization code.
;
; After a reset execution starts here, the mode is ARM, supervisor
; with interrupts disabled.
;



        SECTION .text:CODE:NOROOT(2)

;        PUBLIC  ?cstartup
        EXTERN  ?main
	REQUIRE __vector

        ARM

__iar_program_start:
?cstartup:

;
; Add initialization needed before setup of stackpointers here.
;

;
; Initialize the stack pointers.
; The pattern below can be used for any of the exception stacks:
; FIQ, IRQ, SVC, ABT, UND, SYS.
; The USR mode uses the same stack as SYS.
; The stack segments must be defined in the linker command file,
; and be declared above.
;


; --------------------
; Mode, correspords to bits 0-5 in CPSR

MODE_MSK DEFINE 0x1F            ; Bit mask for mode bits in CPSR

USR_MODE DEFINE 0x10            ; User mode
FIQ_MODE DEFINE 0x11            ; Fast Interrupt Request mode
IRQ_MODE DEFINE 0x12            ; Interrupt Request mode
SVC_MODE DEFINE 0x13            ; Supervisor mode
ABT_MODE DEFINE 0x17            ; Abort mode
UND_MODE DEFINE 0x1B            ; Undefined Instruction mode
SYS_MODE DEFINE 0x1F            ; System mode

#define SCB_BASE 0xE01FC000
#define PLLCON (*(volatile unsigned char *)0xE01FC080)
#define PLLCON_OFFSET 0x80
#define PLLCON_PLLE_MASK 0x1
#define PLLCON_PLLE 0x1
#define PLLCON_PLLE_BIT 0
#define PLLCON_PLLC_MASK 0x2
#define PLLCON_PLLC 0x2
#define PLLCON_PLLC_BIT 1

#define PLLCFG (*(volatile unsigned long *)0xE01FC084)
#define PLLCFG_OFFSET 0x84
#define PLLCFG_MSEL_MASK 0x7FFF
#define PLLCFG_MSEL_BIT 0
#define PLLCFG_NSEL_MASK 0xFF0000
#define PLLCFG_NSEL_BIT 16

#define PLLSTAT (*(volatile unsigned long *)0xE01FC088)
#define PLLSTAT_OFFSET 0x88
#define PLLSTAT_MSEL_MASK 0x7FFF
#define PLLSTAT_MSEL_BIT 0
#define PLLSTAT_NSEL_MASK 0xFF0000
#define PLLSTAT_NSEL_BIT 16
#define PLLSTAT_PLLE_MASK 0x1000000
#define PLLSTAT_PLLE 0x1000000
#define PLLSTAT_PLLE_BIT 24
#define PLLSTAT_PLLC_MASK 0x2000000
#define PLLSTAT_PLLC 0x2000000
#define PLLSTAT_PLLC_BIT 25
#define PLLSTAT_PLOCK_MASK 0x4000000
#define PLLSTAT_PLOCK 0x4000000
#define PLLSTAT_PLOCK_BIT 26

#define PLLFEED (*(volatile unsigned char *)0xE01FC08C)
#define PLLFEED_OFFSET 0x8C

#define SCS (*(volatile unsigned long *)0xE01FC1A0)
#define SCS_OFFSET 0x1A0
#define SCS_GPIOM_MASK 0x1
#define SCS_GPIOM 0x1
#define SCS_GPIOM_BIT 0
#define SCS_EMC_Reset_Disable_MASK 0x2
#define SCS_EMC_Reset_Disable 0x2
#define SCS_EMC_Reset_Disable_BIT 1
#define SCS_MCIPWR_Active_Level_MASK 0x8
#define SCS_MCIPWR_Active_Level 0x8
#define SCS_MCIPWR_Active_Level_BIT 3
#define SCS_OSCRANGE_MASK 0x10
#define SCS_OSCRANGE 0x10
#define SCS_OSCRANGE_BIT 4
#define SCS_OSCEN_MASK 0x20
#define SCS_OSCEN 0x20
#define SCS_OSCEN_BIT 5
#define SCS_OSCSTAT_MASK 0x40
#define SCS_OSCSTAT 0x40
#define SCS_OSCSTAT_BIT 6

#define CCLKCFG (*(volatile unsigned long *)0xE01FC104)
#define CCLKCFG_OFFSET 0x104
#define CCLKCFG_CCLKSEL_MASK 0xFF
#define CCLKCFG_CCLKSEL_BIT 0

#define USBCLKCFG (*(volatile unsigned long *)0xE01FC108)
#define USBCLKCFG_OFFSET 0x108
#define USBCLKCFG_USBSEL_MASK 0xF
#define USBCLKCFG_USBSEL_BIT 0

#define CLKSRCSEL (*(volatile unsigned char *)0xE01FC10C)
#define CLKSRCSEL_OFFSET 0x10C
#define CLKSRCSEL_CLKSRC_MASK 0x3
#define CLKSRCSEL_CLKSRC_BIT 0

#define PROCESSOR_OSCILLATOR_FREQUENCY  72000000
#define OSCILLATOR_CLOCK_FREQUENCY 12000000



#ifndef PLLCFG_VAL
    #define PLLCFG_VAL ((11 << PLLCFG_MSEL_BIT) | (0 << PLLCFG_NSEL_BIT))
#endif
#ifndef CCLKCFG_VAL
    #if (PROCESSOR_OSCILLATOR_FREQUENCY==72000000)
        #define CCLKCFG_VAL 3 // divide Fcco by 4 for 72 MHz
    #elif (PROCESSOR_OSCILLATOR_FREQUENCY==57600000)
        #define CCLKCFG_VAL 4 // divide Fcco by 5 for 57.6 MHz
    #endif
#endif

#if OSCILLATOR_CLOCK_FREQUENCY==12000000

/* Fosc = 12Mhz, Fcco = 288Mhz, cclk = 72Mhz, usbclk = 48Mhz */
#ifndef PLLCFG_VAL
    #define PLLCFG_VAL ((11 << PLLCFG_MSEL_BIT) | (0 << PLLCFG_NSEL_BIT))
#endif
#ifndef CCLKCFG_VAL
    #if (PROCESSOR_OSCILLATOR_FREQUENCY==72000000)
        #define CCLKCFG_VAL 3 // divide Fcco by 4 for 72 MHz
    #elif (PROCESSOR_OSCILLATOR_FREQUENCY==57600000)
        #define CCLKCFG_VAL 4 // divide Fcco by 5 for 57.6 MHz
    #endif
#endif

#ifndef USBCLKCFG_VAL
#define USBCLKCFG_VAL 5 // divide Fcco by 6 for 48 MHz
#endif

#endif

#ifndef MAMCR_VAL
#define MAMCR_VAL 2
#endif

#ifndef MAMTIM_VAL
#define MAMTIM_VAL 3
#endif


#if 0
// Do a very short delay

        MOV      R0,#+31232
        ORR      R0,R0,#0x240
??TestDelay_0:
        MOVS     R1,R0
        SUBS     R0,R1,#+1
        CMP      R1,#+0
        BNE      ??TestDelay_0
#endif

#if 1
  ldr r0, =SCB_BASE

#if defined(PLLCFG_VAL) && !defined(NO_PLL_ENABLE)
  /* Configure PLL Multiplier/Divider */
  ldr r1, [r0, #PLLSTAT_OFFSET]
  tst r1, #PLLSTAT_PLLC
  beq disconnect_done

  /* Disconnect PLL */
  ldr r1, =PLLCON_PLLE
  str r1, [r0, #PLLCON_OFFSET]
  mov r1, #0xAA
  str r1, [r0, #PLLFEED_OFFSET]
  mov r1, #0x55
  str r1, [r0, #PLLFEED_OFFSET]

disconnect_done:
  /* Disable PLL */
  ldr r1, =0
  str r1, [r0, #PLLCON_OFFSET]
  mov r1, #0xAA
  str r1, [r0, #PLLFEED_OFFSET]
  mov r1, #0x55
  str r1, [r0, #PLLFEED_OFFSET]

  /* Enable main oscillator */
  ldr r1, [r0, #SCS_OFFSET]
  orr r1, r1, #SCS_OSCEN
  str r1, [r0, #SCS_OFFSET]
wait_main_osc:
  ldr r1, [r0, #SCS_OFFSET]
  tst r1, #SCS_OSCSTAT
  beq wait_main_osc

  /* Select main oscillator as the PLL clock source */
  ldr r1, =1
  str r1, [r0, #CLKSRCSEL_OFFSET]

  /* Set PLLCFG */
  ldr r1, =PLLCFG_VAL
  str r1, [r0, #PLLCFG_OFFSET]
  mov r1, #0xAA
  str r1, [r0, #PLLFEED_OFFSET]
  mov r1, #0x55
  str r1, [r0, #PLLFEED_OFFSET]

  /* Enable PLL */
  ldr r1, =PLLCON_PLLE
  str r1, [r0, #PLLCON_OFFSET]
  mov r1, #0xAA
  str r1, [r0, #PLLFEED_OFFSET]
  mov r1, #0x55
  str r1, [r0, #PLLFEED_OFFSET]

#ifdef CCLKCFG_VAL
  /* Set the CPU clock divider */
  ldr r1, =CCLKCFG_VAL
  str r1, [r0, #CCLKCFG_OFFSET]
#endif

#ifdef USBCLKCFG_VAL
  /* Set the CPU clock divider */
  ldr r1, =USBCLKCFG_VAL
  str r1, [r0, #USBCLKCFG_OFFSET]
#endif

  /* Wait for PLL to lock */
wait_pll_lock:
  ldr r1, [r0, #PLLSTAT_OFFSET]
  tst r1, #PLLSTAT_PLOCK
  beq wait_pll_lock
  /* PLL Locked, connect PLL as clock source */
  mov r1, #(PLLCON_PLLE | PLLCON_PLLC)
  str r1, [r0, #PLLCON_OFFSET]
  mov r1, #0xAA
  str r1, [r0, #PLLFEED_OFFSET]
  mov r1, #0x55
  str r1, [r0, #PLLFEED_OFFSET]
  /* Wait for PLL to connect */
wait_pll_connect:
  ldr r1, [r0, #PLLSTAT_OFFSET]
  tst r1, #PLLSTAT_PLLC
  beq wait_pll_connect
#endif
#endif

        MRS     r0, cpsr                ; Original PSR value

        ;; Set up the interrupt stack pointer.

        BIC     r0, r0, #MODE_MSK       ; Clear the mode bits
        ORR     r0, r0, #IRQ_MODE       ; Set IRQ mode bits
;        MSR     cpsr_c, r0              ; Change the mode
        MSR     cpsr_cxsf, r0              ; Change the mode
        LDR     sp, =SFE(IRQ_STACK)     ; End of IRQ_STACK
        BIC     sp,sp,#0x7              ; Make sure SP is 8 aligned
	
        ;; Set up the fast interrupt stack pointer.

        BIC     r0, r0, #MODE_MSK       ; Clear the mode bits
        ORR     r0, r0, #FIQ_MODE       ; Set FIR mode bits
;        MSR     cpsr_c, r0              ; Change the mode
        MSR     cpsr_cxsf, r0              ; Change the mode
        LDR     sp, =SFE(FIQ_STACK)     ; End of FIQ_STACK
        BIC     sp,sp,#0x7              ; Make sure SP is 8 aligned

        ;; Set up the normal stack pointer.

        BIC     r0 ,r0, #MODE_MSK       ; Clear the mode bits
;        ORR     r0 ,r0, #SYS_MODE       ; Set System mode bits
; lshields 5/24/2009 -- need to start in supervisor mode
        ORR     r0 ,r0, #SVC_MODE       ; Set System mode bits
;        MSR     cpsr_c, r0              ; Change the mode
        MSR     cpsr_cxsf, r0              ; Change the mode
        LDR     sp, =SFE(CSTACK)        ; End of CSTACK
        BIC     sp,sp,#0x7              ; Make sure SP is 8 aligned

#ifdef __ARMVFP__
        ;; Enable the VFP coprocessor.

        MOV     r0, #0x40000000         ; Set EN bit in VFP
        FMXR    fpexc, r0               ; FPEXC, clear others.
	
;
; Disable underflow exceptions by setting flush to zero mode.
; For full IEEE 754 underflow compliance this code should be removed
; and the appropriate exception handler installed.
;

        MOV     r0, #0x03000000         ; Set FZ and DN bits in VFP
        FMXR    fpscr, r0               ; FPSCR, clear others.
#endif

;
; Add more initialization here
;

; Continue to ?main for C-level initialization.

        BL      SystemInit
        B       ?main

Undefined_Handler:
        MOV     R0, #4
        B       UEZBSP_FatalError
Prefetch_HandlerOld:
        MOV     R0, #5
        B       UEZBSP_FatalError
Prefetch_Handler:
//        MOV     R0, #6
        STMFD   sp!, {r0-r12, lr}
        ldr sp, =(G_dabort_dump+5*4)
        stmia sp, {r0-r12}
        sub r0, lr, #8
        mrs r5, cpsr
        mrs r6, spsr
        
        mov r2, r6
        tst r6, #0xF
        orreq r6, r6, #0xF
        bic r7, r6, #(1<<5)
        msr cpsr_c, r7
        mov r3, lr
        mov r4, sp
        msr cpsr_c, r5
        tst r6, #(1<<5)
        ldrneh r1, [r0]
        ldreq r1, [r0]
        ldr sp, =G_dabort_dump
        
        stmia sp, {r0-r4}
        
        ldr sp, =SFE(ABT_STACK)
        ldmdb sp, {r0-r12, lr}
        BL       LPC2478_PrefetchHandler
Abort_Handler:
//        MOV     R0, #6
        STMFD   sp!, {r0-r12, lr}
        ldr sp, =(G_dabort_dump+5*4)
        stmia sp, {r0-r12}
        sub r0, lr, #8
        mrs r5, cpsr
        mrs r6, spsr
        
        mov r2, r6
        tst r6, #0xF
        orreq r6, r6, #0xF
        bic r7, r6, #(1<<5)
        msr cpsr_c, r7
        mov r3, lr
        mov r4, sp
        msr cpsr_c, r5
        tst r6, #(1<<5)
        ldrneh r1, [r0]
        ldreq r1, [r0]
        ldr sp, =G_dabort_dump
        
        stmia sp, {r0-r4}
        
        ldr sp, =SFE(ABT_STACK)
        ldmdb sp, {r0-r12, lr}
        BL       LPC2478_AbortHandler
Abort_HandlerLoop:        
        B Abort_HandlerLoop
IRQ_Handler:
        MOV     R0, #7
        B       UEZBSP_FatalError
FIQ_Handler:
        MOV     R0, #8
        B       UEZBSP_FatalError
        
        END
