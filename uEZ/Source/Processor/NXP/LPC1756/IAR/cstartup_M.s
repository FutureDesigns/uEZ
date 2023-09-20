/**************************************************
 *
 * Part one of the system initialization code, contains low-level
 * initialization, plain thumb variant.
 *
 * Copyright 2009 IAR Systems. All rights reserved.
 *
 * $Revision: 28532 $
 *
 **************************************************/

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;
        INCLUDE Config.h
        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
#if ( RTOS == SafeRTOS )
        EXTERN  vMainMPUFaultHandler
        EXTERN  vSafeRTOSSVCHandler
        EXTERN  vSafeRTOSPendSVHandler
        EXTERN  vPortSysTickHandler
#elif (RTOS == FreeRTOS )
		IMPORT xPortSysTickHandler
		IMPORT vPortSVCHandler
		IMPORT xPortPendSVHandler
#endif
		EXTERN vPortSVCHandler
		//EXTERN vEMAC_ISR
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  RestartApp

        DATA
#if (RTOS == FreeRTOS )
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler
        DCD     HardFault_Handler
        DCD     vMainMPUFaultHandler
        DCD     BusFault_Handler
        DCD     UsageFault_Handler
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     vPortSVCHandler           ;SVC_Handler
        DCD     DebugMon_Handler
        DCD     0
        DCD     xPortPendSVHandler        ;PendSV_Handler
        DCD     xPortSysTickHandler       ;SysTick_Handler
#elif ( RTOS == SafeRTOS )
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler
        DCD     HardFault_Handler
        DCD     vMainMPUFaultHandler
        DCD     BusFault_Handler
        DCD     UsageFault_Handler
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     vSafeRTOSSVCHandler       ;SVC_Handler
        DCD     DebugMon_Handler
        DCD     0
        DCD     vSafeRTOSPendSVHandler    ;PendSV_Handler
        DCD     vPortSysTickHandler       ;SysTick_Handler
#else
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler

        DCD     NMI_Handler
        DCD     HardFault_Handler
        DCD     MemManage_Handler
        DCD     BusFault_Handler
        DCD     UsageFault_Handler
__vector_table_0x1c
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     0
        DCD     0
#endif        
        DCD     WDT_IRQHandler              ; Watchdog Handler
        DCD     TIMER0_IRQHandler           ; TIMER0 Handler
        DCD     TIMER1_IRQHandler           ; TIMER1 Handler
        DCD     TIMER2_IRQHandler           ; TIMER2 Handler
        DCD     TIMER3_IRQHandler           ; TIMER3 Handler
        DCD     UART0_IRQHandler            ; UART0 Handler
        DCD     UART1_IRQHandler            ; UART1 Handler
        DCD     UART2_IRQHandler            ; UART2 Handler
        DCD     UART3_IRQHandler            ; UART3 Handler
        DCD     PWM1_IRQHandler             ; PWM1 Handler
        DCD     I2C0_IRQHandler             ; I2C0 Handler
        DCD     I2C1_IRQHandler             ; I2C1 Handler
        DCD     I2C2_IRQHandler             ; I2C2 Handler
        DCD     SPI_IRQHandler              ; SPI Handler
        DCD     SSP0_IRQHandler             ; SSP0 Handler
        DCD     SSP1_IRQHandler             ; SSP1 Handler
        DCD     PLL0_IRQHandler             ; PLL0 Handler
        DCD     RTC_IRQHandler              ; RTC Handler
        DCD     EINT0_IRQHandler            ; EXT Interupt 0 Handler
        DCD     EINT1_IRQHandler            ; EXT Interupt 1 Handler
        DCD     EINT2_IRQHandler            ; EXT Interupt 2 Handler
        DCD     EINT3_IRQHandler            ; EXT Interupt 3 Handler
        DCD     ADC_IRQHandler              ; ADC Handler
        DCD     BOD_IRQHandler              ; BOD Handler
        DCD     USB_IRQHandler              ; USB Handler
        DCD     CAN_IRQHandler              ; CAN Handler
        DCD     GPDMA_IRQHandler            ; General Purpose DMA Handler
        DCD     I2S_IRQHandler              ; I2S Handler
        DCD     ENET_IRQHandler  			; Ethernet Handler
        DCD     RIT_IRQHandler              ; Repetitive Interrupt Timer Handler
        DCD     MCPWM_IRQHandler  			; Motor Control PWM Handler
        DCD     QEI_IRQHandler              ; Quadrature Encoder Handler
        DCD     PLL1_IRQHandler             ; PLL1 Handler

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB
        
        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        B MemManage_Handler

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        B BusFault_Handler

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        B UsageFault_Handler

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        B DebugMon_Handler

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK WDT_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
WDT_IRQHandler
        B WDT_IRQHandler

        PUBWEAK TIMER0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER0_IRQHandler
        B TIMER0_IRQHandler

        PUBWEAK TIMER1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER1_IRQHandler
        B TIMER1_IRQHandler

        PUBWEAK TIMER2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER2_IRQHandler
        B TIMER2_IRQHandler

        PUBWEAK TIMER3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
TIMER3_IRQHandler
        B TIMER3_IRQHandler

        PUBWEAK UART0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART0_IRQHandler
        B UART0_IRQHandler

        PUBWEAK UART1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART1_IRQHandler
        B UART1_IRQHandler

        PUBWEAK UART2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART2_IRQHandler
        B UART2_IRQHandler

        PUBWEAK UART3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
UART3_IRQHandler
        B UART3_IRQHandler

        PUBWEAK PWM1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PWM1_IRQHandler
        B PWM1_IRQHandler

        PUBWEAK I2C0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C0_IRQHandler
        B I2C0_IRQHandler

        PUBWEAK I2C1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_IRQHandler
        B I2C1_IRQHandler

        PUBWEAK I2C2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C2_IRQHandler
        B I2C2_IRQHandler

        PUBWEAK SPI_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI_IRQHandler
        B SPI_IRQHandler

        PUBWEAK SSP0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SSP0_IRQHandler
        B SSP0_IRQHandler

        PUBWEAK SSP1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SSP1_IRQHandler
        B SSP1_IRQHandler

        PUBWEAK PLL0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PLL0_IRQHandler
        B PLL0_IRQHandler

        PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_IRQHandler
        B RTC_IRQHandler

        PUBWEAK EINT0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EINT0_IRQHandler
        B EINT0_IRQHandler

        PUBWEAK EINT1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EINT1_IRQHandler
        B EINT1_IRQHandler

        PUBWEAK EINT2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EINT2_IRQHandler
        B EINT2_IRQHandler

        PUBWEAK EINT3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EINT3_IRQHandler
        B EINT3_IRQHandler

        PUBWEAK ADC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC_IRQHandler
        B ADC_IRQHandler

        PUBWEAK BOD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
BOD_IRQHandler
        B BOD_IRQHandler

        PUBWEAK USB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USB_IRQHandler
        B USB_IRQHandler

        PUBWEAK CAN_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CAN_IRQHandler
        B CAN_IRQHandler

        PUBWEAK GPDMA_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPDMA_IRQHandler
        B GPDMA_IRQHandler
        PUBWEAK I2S_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2S_IRQHandler
        B I2S_IRQHandler

        PUBWEAK ENET_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ENET_IRQHandler
        B ENET_IRQHandler

        PUBWEAK RIT_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RIT_IRQHandler
        B RIT_IRQHandler

        PUBWEAK MCPWM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MCPWM_IRQHandler
        B MCPWM_IRQHandler

        PUBWEAK QEI_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
QEI_IRQHandler
        B QEI_IRQHandler

        PUBWEAK PLL1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
PLL1_IRQHandler
        B PLL1_IRQHandler

        SECTION .text:CODE:NOROOT(2)
__startup:
		; Set the vector address at the above vector location
        LDR      R0,VTOR  ;; 0xe000ed08
        LDR      R1,VectorBaseAddress
        STR      R1,[R0, #+0]
        B __iar_program_start

RestartApp:
		; Turn off interrupts
        CPSID    I
        LDR     SP, [R0, #+0]
        MOV     R7, #0
        MSR     PRIMASK, R7
        LDR     R7, [R0, #+4]
        BLX      R7

        DATA
VTOR:
        DC32     0xe000ed08
VectorBaseAddress:
        DC32     __vector_table


        END
