;/**************************************************************************//**
; * @file     LPC43xx_M0Sub.s
; * @brief    CMSIS Cortex-M0 Core Device Startup File for
; *           NXP LPC43xxDevice Series
; * @version  V1.00
; * @date     03. September 2013
; *
; * @note
; * Copyright (C) 2009-2013 ARM Limited. All rights reserved.
; *
; * @par
; * ARM Limited (ARM) is supplying this software for use with Cortex-M
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * @par
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; * <<< Use Configuration Wizard in Context Menu >>>   
; ******************************************************************************/

; <h> Stack Configuration
;   <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Stack_Size      EQU     0x00000200

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   Stack_Size
__initial_sp

; <h> Heap Configuration
;   <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
; </h>

Heap_Size       EQU     0x00000000

                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

                PRESERVE8
                THUMB

; Vector Table Mapped to Address 0 at Reset

                AREA    RESET, DATA, READONLY
                EXPORT  __Vectors

Sign_Value      EQU     0x5A5A5A5A

__Vectors       DCD     __initial_sp                            ;  0  Top of Stack
                DCD     M0S_Reset_Handler                       ;  1  Reset Handler
                DCD     M0S_NMI_Handler                         ;  2  NMI Handler
                DCD     M0S_HardFault_Handler                   ;  3  Hard Fault Handler
                DCD     0                                       ;  4  Reserved
                DCD     0                                       ;  5  Reserved
                DCD     0                                       ;  6  Reserved
                DCD     0                                       ;  7  Reserved
                DCD     0                                       ;  8  Reserved
                DCD     0                                       ;  9  Reserved
                DCD     0                                       ; 10  Reserved
                DCD     M0S_SVC_Handler                         ; 11  SVCall Handler
                DCD     M0S_DebugMon_Handler                    ; 12  Debug Monitor Handler
                DCD     0                                       ; 13  Reserved
                DCD     M0S_PendSV_Handler                      ; 14  PendSV Handler
                DCD     M0S_SysTick_Handler                     ; 15  SysTick Handler

                ; External LPC43xx/M0Sub Interrupts
                DCD     M0S_RTC_IRQHandler                      ;  0  RTC interrupt
                DCD     M0S_M4CORE_IRQHandler                   ;  1  M4 core interrupt
                DCD     M0S_DMA_IRQHandler                      ;  2  DMA interrupt
                DCD     0
                DCD     M0S_SGPIO_INPUT_IRQHandler              ;  4  SGPIO input bit match interrupt
                DCD     M0S_SGPIO_MATCH_IRQHandler              ;  5  SGPIO pattern match interrupt
                DCD     M0S_SGPIO_SHIFT_IRQHandler              ;  6  SGPIO shift clock interrupt
                DCD     M0S_SGPIO_POS_IRQHandler                ;  7  SGPIO capture clock interrupt
                DCD     M0S_USB0_IRQHandler                     ;  8  OTG interrupt
                DCD     M0S_USB1_IRQHandler                     ;  9  USB1 interrupt
                DCD     M0S_SCT_IRQHandler                      ; 10  SCT combined interrupt
                DCD     M0S_RITIMER_IRQHandler                  ; 11  RI timer interrupt
                DCD     M0S_GINT1_IRQHandler                    ; 12  GPIO global interrupt 1
                DCD     M0S_TIMER1_IRQHandler                   ; 13  Timer 1 interrupt
                DCD     M0S_TIMER2_IRQHandler                   ; 14  Timer 2 interrupt
                DCD     M0S_PIN_INT5_IRQHandler                 ; 15  GPIO pin interrupt 5
                DCD     M0S_MCPWM_IRQHandler                    ; 16  Motor control PWM interrupt
                DCD     M0S_ADC0_IRQHandler                     ; 17  ADC0 interrupt
                DCD     M0S_I2C0_IRQHandler                     ; 18  I2C0 interrupt
                DCD     M0S_I2C1_IRQHandler                     ; 19  I2C1 interrupt
                DCD     M0S_SPI_IRQHandler                      ; 20  SPI interrupt
                DCD     M0S_ADC1_IRQHandler                     ; 21  ADC1 interrupt
                DCD     M0S_SSP0_OR_SSP1_IRQHandler             ; 22  SSP0, SSP1 ORed interrupt
                DCD     M0S_EVENTROUTER_IRQHandler              ; 23  Event router interrupt
                DCD     M0S_USART0_IRQHandler                   ; 24  USART0 interrupt
                DCD     M0S_UART1_IRQHandler                    ; 25  Modem/UART1 interrupt
                DCD     M0S_USART2_OR_C_CAN1_IRQHandler         ; 26  USART2, C_CAN1 ORed interrupt
                DCD     M0S_USART3_IRQHandler                   ; 27  USART3 interrupt
                DCD     M0S_I2S0_OR_I2S1_OR_QEI_IRQHandler      ; 28  I2S0, I2S1, QEI ORed interrupt
                DCD     M0S_C_CAN0_IRQHandler                   ; 29  C_CAN0 interrupt
                DCD     M0S_SPIFI_OR_ADCHS_IRQHandler           ; 30  SPIFI, ADCHS ORed interrupt
                DCD     M0S_M0APP_IRQHandler                    ; 31  M0APP core interrupt

                AREA    |.text|, CODE, READONLY

; Reset Handler

M0S_Reset_Handler\
                PROC
                EXPORT  M0S_Reset_Handler                   [WEAK]
                IMPORT  __main
                LDR     R0, =__main
                BX      R0
                ENDP

; Dummy Exception Handlers (infinite loops which can be modified)

M0S_NMI_Handler PROC
                EXPORT  M0S_NMI_Handler                     [WEAK]
                B       .
                ENDP

M0S_HardFault_Handler\
                PROC
                EXPORT  M0S_HardFault_Handler               [WEAK]
                B       .
                ENDP

M0S_SVC_Handler PROC
                EXPORT  M0S_SVC_Handler                     [WEAK]
                B       .
                ENDP

M0S_DebugMon_Handler\
                PROC
                EXPORT  M0S_DebugMon_Handler                [WEAK]
                B       .
                ENDP

M0S_PendSV_Handler\
                PROC
                EXPORT  M0S_PendSV_Handler                  [WEAK]
                B       .
                ENDP

M0S_SysTick_Handler\
                PROC
                EXPORT  M0S_SysTick_Handler                 [WEAK]
                B       .
                ENDP

M0S_Default_Handler\
                PROC
                EXPORT  M0S_RTC_IRQHandler                  [WEAK]
                EXPORT  M0S_M4CORE_IRQHandler               [WEAK]
                EXPORT  M0S_DMA_IRQHandler                  [WEAK]
                EXPORT  M0S_SGPIO_INPUT_IRQHandler          [WEAK]
                EXPORT  M0S_SGPIO_MATCH_IRQHandler          [WEAK]
                EXPORT  M0S_SGPIO_SHIFT_IRQHandler          [WEAK]
                EXPORT  M0S_SGPIO_POS_IRQHandler            [WEAK]
                EXPORT  M0S_USB0_IRQHandler                 [WEAK]
                EXPORT  M0S_USB1_IRQHandler                 [WEAK]
                EXPORT  M0S_SCT_IRQHandler                  [WEAK]
                EXPORT  M0S_RITIMER_IRQHandler              [WEAK]
                EXPORT  M0S_GINT1_IRQHandler                [WEAK]
                EXPORT  M0S_TIMER1_IRQHandler               [WEAK]
                EXPORT  M0S_TIMER2_IRQHandler               [WEAK]
                EXPORT  M0S_PIN_INT5_IRQHandler             [WEAK]
                EXPORT  M0S_MCPWM_IRQHandler                [WEAK]
                EXPORT  M0S_ADC0_IRQHandler                 [WEAK]
                EXPORT  M0S_I2C0_IRQHandler                 [WEAK]
                EXPORT  M0S_I2C1_IRQHandler                 [WEAK]
                EXPORT  M0S_SPI_IRQHandler                  [WEAK]
                EXPORT  M0S_ADC1_IRQHandler                 [WEAK]
                EXPORT  M0S_SSP0_OR_SSP1_IRQHandler         [WEAK]
                EXPORT  M0S_EVENTROUTER_IRQHandler          [WEAK]
                EXPORT  M0S_USART0_IRQHandler               [WEAK]
                EXPORT  M0S_UART1_IRQHandler                [WEAK]
                EXPORT  M0S_USART2_OR_C_CAN1_IRQHandler     [WEAK]
                EXPORT  M0S_USART3_IRQHandler               [WEAK]
                EXPORT  M0S_I2S0_OR_I2S1_OR_QEI_IRQHandler  [WEAK]
                EXPORT  M0S_C_CAN0_IRQHandler               [WEAK]
                EXPORT  M0S_SPIFI_OR_ADCHS_IRQHandler       [WEAK]
                EXPORT  M0S_M0APP_IRQHandler                [WEAK]

M0S_RTC_IRQHandler
M0S_M4CORE_IRQHandler
M0S_DMA_IRQHandler
M0S_SGPIO_INPUT_IRQHandler
M0S_SGPIO_MATCH_IRQHandler
M0S_SGPIO_SHIFT_IRQHandler
M0S_SGPIO_POS_IRQHandler
M0S_USB0_IRQHandler
M0S_USB1_IRQHandler
M0S_SCT_IRQHandler
M0S_RITIMER_IRQHandler
M0S_GINT1_IRQHandler
M0S_TIMER1_IRQHandler
M0S_TIMER2_IRQHandler
M0S_PIN_INT5_IRQHandler
M0S_MCPWM_IRQHandler
M0S_ADC0_IRQHandler
M0S_I2C0_IRQHandler
M0S_I2C1_IRQHandler
M0S_SPI_IRQHandler
M0S_ADC1_IRQHandler
M0S_SSP0_OR_SSP1_IRQHandler
M0S_EVENTROUTER_IRQHandler
M0S_USART0_IRQHandler
M0S_UART1_IRQHandler
M0S_USART2_OR_C_CAN1_IRQHandler
M0S_USART3_IRQHandler
M0S_I2S0_OR_I2S1_OR_QEI_IRQHandler
M0S_C_CAN0_IRQHandler
M0S_SPIFI_OR_ADCHS_IRQHandler
M0S_M0APP_IRQHandler

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

                AREA    |.text|,CODE, READONLY
getPC           PROC
                EXPORT  getPC

                MOV     R0,LR
                BX      LR

                ENDP

                END