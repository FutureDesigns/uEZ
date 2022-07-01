/**************************************************
 *
 * Part one of the system initialization code, contains low-level
 * initialization, plain thumb variant.
 *
 * Copyright 2011 IAR Systems. All rights reserved.
 *
 * $Revision: 50291 $
 *
 **************************************************/

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; Cortex-M version
;


        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        DATA

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
        DCD     SVC_Handler
        DCD     DebugMon_Handler
        DCD     0
        DCD     PendSV_Handler
        DCD     SysTick_Handler

        ; External Interrupts
        DCD   M0_RTC_IRQHandler                 ; 16 RTC Converter
        DCD   M0_M4CORE_IRQHandler              ; 17 CortexM4
        DCD   M0_DMA_IRQHandler                 ; 18 General Purpose DMA
        DCD   0                                 ; 19 Reserved
        DCD   0                                 ; 20 Reserved
        DCD   M0_ETH_IRQHandler                 ; 21 Ethernet
        DCD   M0_SDIO_IRQHandler                ; 22 SD/MMC
        DCD   M0_LCD_IRQHandler                 ; 23 LCD
        DCD   M0_USB0_IRQHandler                ; 24 USB0
        DCD   M0_USB1_IRQHandler                ; 25 USB1
        DCD   M0_SCT_IRQHandler                 ; 26 State Configurable Timer
        DCD   M0_RITIMER_OR_WWDT_IRQHandler     ; 27 Repetitive Interrupt Timer & WWDT
        DCD   M0_TIMER0_IRQHandler              ; 28 Timer0
        DCD   M0_GINT1_IRQHandler               ; 29 GPIO Global 1
        DCD   M0_PIN_INT4_IRQHandler            ; 30 GPIO PIN 4
        DCD   M0_TIMER3_IRQHandler              ; 31 Timer3
        DCD   M0_MCPWM_IRQHandler               ; 32 Motor Control PWM
        DCD   M0_ADC0_IRQHandler                ; 33 A/D Converter 0
        DCD   M0_I2C0_OR_I2C1_IRQHandler        ; 34 I2C0/1
        DCD   M0_SGPIO_IRQHandler               ; 35 SGPIO
        DCD   M0_SPI_OR_DAC_IRQHandler          ; 36 SPI & DAC
        DCD   M0_ADC1_IRQHandler                ; 37 A/D Converter 1
        DCD   M0_SSP0_OR_SSP1_IRQHandler        ; 38 SSP0
        DCD   M0_EVENTROUTER_IRQHandler         ; 39 Event Router
        DCD   M0_USART0_IRQHandler              ; 40 UART0
        DCD   M0_UART1_IRQHandler               ; 41 UART1
        DCD   M0_USART2_OR_C_CAN1_IRQHandler    ; 42 UART2 & C_CAN1
        DCD   M0_USART3_IRQHandler              ; 43 UART3
        DCD   M0_I2S0_OR_I2S1_OR_QEI_IRQHandler ; 44 I2S0/1 & QEI
        DCD   M0_C_CAN0_IRQHandler              ; 45 C_CAN0
        DCD   0                                 ; 46 SPIFI
        DCD   0                                 ; 47 
__Vectors_End

__Vectors       EQU   __vector_table
__Vectors_Size  EQU   __Vectors_End - __Vectors

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER(2)
Reset_Handler
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK NMI_Handler
        PUBWEAK HardFault_Handler
        PUBWEAK MemManage_Handler
        PUBWEAK BusFault_Handler
        PUBWEAK UsageFault_Handler
        PUBWEAK SVC_Handler
        PUBWEAK DebugMon_Handler
        PUBWEAK PendSV_Handler
        PUBWEAK SysTick_Handler
        PUBWEAK M0_RTC_IRQHandler
        PUBWEAK M0_M4CORE_IRQHandler
        PUBWEAK M0_DMA_IRQHandler
        PUBWEAK M0_ETH_IRQHandler
        PUBWEAK M0_SDIO_IRQHandler
        PUBWEAK M0_LCD_IRQHandler
        PUBWEAK M0_USB0_IRQHandler
        PUBWEAK M0_USB1_IRQHandler
        PUBWEAK M0_SCT_IRQHandler
        PUBWEAK M0_RITIMER_OR_WWDT_IRQHandler
        PUBWEAK M0_TIMER0_IRQHandler
        PUBWEAK M0_GINT1_IRQHandler
        PUBWEAK M0_PIN_INT4_IRQHandler
        PUBWEAK M0_TIMER3_IRQHandler
        PUBWEAK M0_MCPWM_IRQHandler
        PUBWEAK M0_ADC0_IRQHandler
        PUBWEAK M0_I2C0_OR_I2C1_IRQHandler
        PUBWEAK M0_SGPIO_IRQHandler
        PUBWEAK M0_SPI_OR_DAC_IRQHandler
        PUBWEAK M0_ADC1_IRQHandler
        PUBWEAK M0_SSP0_OR_SSP1_IRQHandler
        PUBWEAK M0_EVENTROUTER_IRQHandler
        PUBWEAK M0_USART0_IRQHandler
        PUBWEAK M0_UART1_IRQHandler
        PUBWEAK M0_UART2_OR_C_CAN1_IRQHandler
        PUBWEAK M0_UART3_IRQHandler
        PUBWEAK M0_I2S0_OR_I2S1_OR_QEI_IRQHandler
        PUBWEAK M0_C_CAN0_IRQHandler
        SECTION .text:CODE:REORDER(1)
NMI_Handler
        B NMI_Handler
SVC_Handler
        B SVC_Handler
DebugMon_Handler
        B DebugMon_Handler
PendSV_Handler
        B PendSV_Handler
SysTick_Handler
        B SysTick_Handler
HardFault_Handler
        B HardFault_Handler
MemManage_Handler
        B MemManage_Handler
BusFault_Handler
        B BusFault_Handler
UsageFault_Handler
M0_RTC_IRQHandler
M0_M4CORE_IRQHandler
M0_DMA_IRQHandler
M0_ETH_IRQHandler
M0_SDIO_IRQHandler
M0_LCD_IRQHandler
M0_USB0_IRQHandler
M0_USB1_IRQHandler
M0_SCT_IRQHandler
M0_RITIMER_OR_WWDT_IRQHandler
M0_TIMER0_IRQHandler
M0_GINT1_IRQHandler
M0_PIN_INT4_IRQHandler
M0_TIMER3_IRQHandler
M0_MCPWM_IRQHandler
M0_ADC0_IRQHandler
M0_I2C0_OR_I2C1_IRQHandler
M0_SGPIO_IRQHandler
M0_SPI_OR_DAC_IRQHandler
M0_ADC1_IRQHandler
M0_SSP0_IRQHandler
M0_EVENTROUTER_IRQHandler
M0_USART0_IRQHandler
M0_UART1_IRQHandler
M0_UART2_OR_C_CAN1_IRQHandler
M0_UART3_IRQHandler
M0_I2S0_OR_I2S1_OR_QEI_IRQHandler
M0_C_CAN0_IRQHandler
Default_IRQHandler
        B Default_IRQHandler

/* CRP Section - not needed for flashless devices */

;;;        SECTION .crp:CODE:ROOT(2)
;;;        DATA
/* Code Read Protection
NO_ISP  0x4E697370 -  Prevents sampling of pin PIO0_1 for entering ISP mode
CRP1    0x12345678 - Write to RAM command cannot access RAM below 0x10000300.
                   - Copy RAM to flash command can not write to Sector 0.
                   - Erase command can erase Sector 0 only when all sectors
                     are selected for erase.
                   - Compare command is disabled.
                   - Read Memory command is disabled.
CRP2    0x87654321 - Read Memory is disabled.
                   - Write to RAM is disabled.
                   - "Go" command is disabled.
                   - Copy RAM to flash is disabled.
                   - Compare is disabled.
CRP3    0x43218765 - Access to chip via the SWD pins is disabled. ISP entry
                     by pulling PIO0_1 LOW is disabled if a valid user code is
                     present in flash sector 0.
Caution: If CRP3 is selected, no future factory testing can be
performed on the device.
*/
;;;     DCD   0xFFFFFFFF
;;;

        END
 