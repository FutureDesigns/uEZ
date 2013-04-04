;/*****************************************************************************
; * @file:    startup_LPC177x_8x.s
; * @purpose: CMSIS Cortex-M3 Core Device Startup File
; *           for the NXP LPC177x_8x Device Series
; * @version: V1.0
; * @date:    25. Nov. 2008
; *------- <<< Use Configuration Wizard in Context Menu >>> ------------------
; *
; * Copyright (C) 2008 ARM Limited. All rights reserved.
; * ARM Limited (ARM) is supplying this software for use with Cortex-M3
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; *****************************************************************************/


; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000800

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp


; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00070000  ; 458,752 bytes

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit


                PRESERVE8
                THUMB


; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors
				IMPORT xPortSysTickHandler
				IMPORT vPortSVCHandler
				IMPORT xPortPendSVHandler

__Vectors       DCD     __initial_sp              ; Top of Stack
                DCD     Reset_Handler             ; Reset Handler
                DCD     NMI_Handler               ; NMI Handler
                DCD     HardFault_Handler         ; Hard Fault Handler
                DCD     MemManage_Handler         ; MPU Fault Handler
                DCD     BusFault_Handler          ; Bus Fault Handler
                DCD     UsageFault_Handler        ; Usage Fault Handler
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     0                         ; Reserved
                DCD     vPortSVCHandler;SVC_Handler               ; SVCall Handler
                DCD     DebugMon_Handler          ; Debug Monitor Handler
                DCD     0                         ; Reserved
                DCD     xPortPendSVHandler;PendSV_Handler            ; PendSV Handler
                DCD     xPortSysTickHandler;SysTick_Handler           ; SysTick Handler

                ; External Interrupts
                DCD     WDT_IRQHandler            ; 16: Watchdog Timer
                DCD     TIMER0_IRQHandler         ; 17: Timer0
                DCD     TIMER1_IRQHandler         ; 18: Timer1
                DCD     TIMER2_IRQHandler         ; 19: Timer2
                DCD     TIMER3_IRQHandler         ; 20: Timer3
                DCD     UART0_IRQHandler          ; 21: UART0
                DCD     UART1_IRQHandler          ; 22: UART1
                DCD     UART2_IRQHandler          ; 23: UART2
                DCD     UART3_IRQHandler          ; 24: UART3
                DCD     PWM1_IRQHandler           ; 25: PWM1
                DCD     I2C0_IRQHandler           ; 26: I2C0
                DCD     I2C1_IRQHandler           ; 27: I2C1
                DCD     I2C2_IRQHandler           ; 28: I2C2
                DCD     SPIFI_IRQHandler          ; 29: SPIFI
                DCD     SSP0_IRQHandler           ; 30: SSP0
                DCD     SSP1_IRQHandler           ; 31: SSP1
                DCD     PLL0_IRQHandler           ; 32: PLL0 Lock (Main PLL)
                DCD     RTC_IRQHandler            ; 33: Real Time Clock
                DCD     EINT0_IRQHandler          ; 34: External Interrupt 0
                DCD     EINT1_IRQHandler          ; 35: External Interrupt 1
                DCD     EINT2_IRQHandler          ; 36: External Interrupt 2
                DCD     EINT3_IRQHandler          ; 37: External Interrupt 3
                DCD     ADC_IRQHandler            ; 38: A/D Converter
                DCD     BOD_IRQHandler            ; 39: Brown-Out Detect
                DCD     USB_IRQHandler            ; 40: USB
                DCD     CAN_IRQHandler            ; 41: CAN
                DCD     GPDMA_IRQHandler          ; 42: General Purpose DMA
                DCD     I2S_IRQHandler            ; 43: I2S
                DCD     ENET_IRQHandler           ; 44: Ethernet
                DCD     MCI_IRQHandler            ; 45: SD/MMC card I/F
                DCD     MCPWM_IRQHandler          ; 46: Motor Control PWM
                DCD     QEI_IRQHandler            ; 47: Quadrature Encoder Interface
                DCD     PLL1_IRQHandler           ; 48: PLL1 Lock (USB PLL)
                DCD     USBACT_IRQHandler    ; 49: USB Activity interrupt to wakeup
                DCD     CANACT_IRQHandler    ; 50: CAN Activity interrupt to wakeup
                DCD     UART4_IRQHandler          ; 51: UART4
                DCD     SSP2_IRQHandler           ; 52: SSP2
                DCD     LCD_IRQHandler            ; 53: LCD
                DCD     GPIO_IRQHandler           ; 54: GPIO
                DCD     PWM0_IRQHandler           ; 55: PWM0
                DCD     KFLASH_IRQHandler         ; 56: KFLASH SHELL


                IF      :LNOT::DEF:NO_CRP
                AREA    |.ARM.__at_0x02FC|, CODE, READONLY
CRP_Key         DCD     0xFFFFFFFF
                ENDIF


                AREA    |.text|, CODE, READONLY


; Reset Handler

Reset_Handler   PROC
                EXPORT  Reset_Handler             [WEAK]
                IMPORT  __main
                IMPORT  SystemInit
                LDR     R0, =SystemInit
                BLX     R0
                LDR     R0, =__main
                BX      R0
                ENDP


; Dummy Exception Handlers (infinite loops which can be modified)

	IMPORT UEZBSP_FatalError
NMI_Handler     PROC
                ;EXPORT  NMI_Handler               [WEAK]
				MOV		R0	,	#4
                B       UEZBSP_FatalError	;.
                ENDP
HardFault_Handler\
                PROC
                ;EXPORT  HardFault_Handler         [WEAK]
				MOV		R0	,	#5
                B       UEZBSP_FatalError	;.
                ENDP
MemManage_Handler\
                PROC
                ;EXPORT  MemManage_Handler         [WEAK]
				MOV		R0	,	#6
                B       UEZBSP_FatalError	;.
                ENDP
BusFault_Handler\
                PROC
                ;EXPORT  BusFault_Handler          [WEAK]
				MOV		R0	,	#7
                B       UEZBSP_FatalError	;.
                ENDP
UsageFault_Handler\
                PROC
                ;EXPORT  UsageFault_Handler        [WEAK]
				MOV		R0	,	#8
                B       UEZBSP_FatalError	;.
                ENDP
SVC_Handler     PROC
                ;EXPORT  SVC_Handler               [WEAK]
				MOV		R0	,	#9
                B       UEZBSP_FatalError	;.
                ENDP
DebugMon_Handler\
                PROC
                ;EXPORT  DebugMon_Handler          [WEAK]
				MOV		R0	,	#10
                B       UEZBSP_FatalError	;.
                ENDP
PendSV_Handler  PROC
                ;EXPORT  PendSV_Handler            [WEAK]
				MOV		R0	,	#11
                B       UEZBSP_FatalError	;.
                ENDP
SysTick_Handler PROC
                ;EXPORT  SysTick_Handler           [WEAK]
				MOV		R0	,	#12
                B       UEZBSP_FatalError	;.
                ENDP

Default_Handler PROC

                EXPORT  WDT_IRQHandler            [WEAK]
                EXPORT  TIMER0_IRQHandler         [WEAK]
                EXPORT  TIMER1_IRQHandler         [WEAK]
                EXPORT  TIMER2_IRQHandler         [WEAK]
                EXPORT  TIMER3_IRQHandler         [WEAK]
                EXPORT  UART0_IRQHandler          [WEAK]
                EXPORT  UART1_IRQHandler          [WEAK]
                EXPORT  UART2_IRQHandler          [WEAK]
                EXPORT  UART3_IRQHandler          [WEAK]
                EXPORT  PWM1_IRQHandler           [WEAK]
                EXPORT  I2C0_IRQHandler           [WEAK]
                EXPORT  I2C1_IRQHandler           [WEAK]
                EXPORT  I2C2_IRQHandler           [WEAK]
                EXPORT  SPIFI_IRQHandler          [WEAK]
                EXPORT  SSP0_IRQHandler           [WEAK]
                EXPORT  SSP1_IRQHandler           [WEAK]
                EXPORT  PLL0_IRQHandler           [WEAK]
                EXPORT  RTC_IRQHandler            [WEAK]
                EXPORT  EINT0_IRQHandler          [WEAK]
                EXPORT  EINT1_IRQHandler          [WEAK]
                EXPORT  EINT2_IRQHandler          [WEAK]
                EXPORT  EINT3_IRQHandler          [WEAK]
                EXPORT  ADC_IRQHandler            [WEAK]
                EXPORT  BOD_IRQHandler            [WEAK]
                EXPORT  USB_IRQHandler            [WEAK]
                EXPORT  CAN_IRQHandler            [WEAK]
                EXPORT  GPDMA_IRQHandler          [WEAK]
                EXPORT  I2S_IRQHandler            [WEAK]
                EXPORT  ENET_IRQHandler           [WEAK]
                EXPORT  MCI_IRQHandler            [WEAK]
                EXPORT  MCPWM_IRQHandler          [WEAK]
                EXPORT  QEI_IRQHandler            [WEAK]
                EXPORT  PLL1_IRQHandler           [WEAK]
                EXPORT  USBACT_IRQHandler         [WEAK]
                EXPORT  CANACT_IRQHandler         [WEAK]
                EXPORT  UART4_IRQHandler          [WEAK]
                EXPORT  SSP2_IRQHandler           [WEAK]
                EXPORT  LCD_IRQHandler            [WEAK]
                EXPORT  GPIO_IRQHandler           [WEAK]
                EXPORT  PWM0_IRQHandler           [WEAK]
                EXPORT  KFLASH_IRQHandler         [WEAK]

WDT_IRQHandler
                B       .
TIMER0_IRQHandler
                B       .
TIMER1_IRQHandler
                B       .
TIMER2_IRQHandler
                B       .
TIMER3_IRQHandler
                B       .
UART0_IRQHandler
                B       .
UART1_IRQHandler
                B       .
UART2_IRQHandler
                B       .
UART3_IRQHandler
                B       .
PWM1_IRQHandler
                B       .
I2C0_IRQHandler
                B       .
I2C1_IRQHandler
                B       .
I2C2_IRQHandler
                B       .
SPIFI_IRQHandler
                B       .
SSP0_IRQHandler
                B       .
SSP1_IRQHandler
                B       .
PLL0_IRQHandler
                B       .
RTC_IRQHandler
                B       .
EINT0_IRQHandler
                B       .
EINT1_IRQHandler
                B       .
EINT2_IRQHandler
                B       .
EINT3_IRQHandler
                B       .
ADC_IRQHandler
                B       .
BOD_IRQHandler
                B       .
USB_IRQHandler
                B       .
CAN_IRQHandler
                B       .
GPDMA_IRQHandler
                B       .
I2S_IRQHandler
                B       .
ENET_IRQHandler
                B       .
MCI_IRQHandler
                B       .
MCPWM_IRQHandler
                B       .
QEI_IRQHandler
                B       .
PLL1_IRQHandler
                B       .
USBACT_IRQHandler
                B       .
CANACT_IRQHandler
                B       .
UART4_IRQHandler
                B       .
SSP2_IRQHandler
                B       .
LCD_IRQHandler
                B       .
GPIO_IRQHandler
                B       .
PWM0_IRQHandler
                B       .
KFLASH_IRQHandler

                B       .

                ENDP


                ALIGN


; User Initial Stack & Heap

                IF      :DEF:__MICROLIB

                EXPORT  __initial_sp
                EXPORT  __heap_base
                EXPORT  __heap_limit

                ELSE

                IMPORT  __use_two_region_memory
                EXPORT  __user_initial_stackheap
__user_initial_stackheap

                LDR     R0, =  Heap_Mem
                LDR     R1, =(Stack_Mem + Stack_Size)
                LDR     R2, = (Heap_Mem +  Heap_Size)
                LDR     R3, = Stack_Mem
                BX      LR

                ALIGN

                ENDIF


                END
