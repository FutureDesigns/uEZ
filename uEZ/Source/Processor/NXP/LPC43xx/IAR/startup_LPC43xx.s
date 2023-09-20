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
        EXTERN  SystemInit
        EXTERN  Lpc43xxSystemInit
        EXTERN xPortSysTickHandler
		EXTERN vPortSVCHandler
		EXTERN  vPortSysTickHandler
        EXTERN  UEZBSP_FatalError
        EXTERN  HardFault_HandlerC
		EXTERN xPortPendSVHandler
        EXTERN  HardFault_HandlerC
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c
        PUBLIC  __Vectors
        PUBLIC  __Vectors_End
        PUBLIC  __Vectors_Size

        //IMPORT xPortSysTickHandler
		//IMPORT vPortSVCHandler
		//IMPORT xPortPendSVHandler

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
        DCD     vPortSVCHandler ;SVC_Handler
        DCD     DebugMon_Handler
        DCD     0
        DCD     xPortPendSVHandler ;PendSV_Handler
        DCD     xPortSysTickHandler ;SysTick_Handler

        ; External Interrupts
        DCD   DAC_IRQHandler         ; 16 D/A Converter
        DCD   M0CORE_IRQHandler      ; 17 CortexM0
        DCD   DMA_IRQHandler         ; 18 General Purpose DMA
        DCD   0                      ; 19 Reserved
        DCD   FLASHEEPROM_IRQHandler ; 20 FlashEEPROM
        DCD   ETH_IRQHandler         ; 21 Ethernet
        DCD   SDIO_IRQHandler        ; 22 SD/MMC
        DCD   LCD_IRQHandler         ; 23 LCD
        DCD   USB0_IRQHandler        ; 24 USB0
        DCD   USB1_IRQHandler        ; 25 USB1
        DCD   SCT_IRQHandler         ; 26 State Configurable Timer
        DCD   RITIMER_IRQHandler     ; 27 Repetitive Interrupt Timer
        DCD   TIMER0_IRQHandler      ; 28 Timer0
        DCD   TIMER1_IRQHandler      ; 29 Timer1
        DCD   TIMER2_IRQHandler      ; 30 Timer2
        DCD   TIMER3_IRQHandler      ; 31 Timer3
        DCD   MCPWM_IRQHandler       ; 32 Motor Control PWM
        DCD   ADC0_IRQHandler        ; 33 A/D Converter 0
        DCD   I2C0_IRQHandler        ; 34 I2C0
        DCD   I2C1_IRQHandler        ; 35 I2C1
        DCD   SPI_IRQHandler         ; 36 Reserved
        DCD   ADC1_IRQHandler        ; 37 A/D Converter 1
        DCD   SSP0_IRQHandler        ; 38 SSP0
        DCD   SSP1_IRQHandler        ; 39 SSP1
        DCD   USART0_IRQHandler      ; 40 UART0
        DCD   UART1_IRQHandler       ; 41 UART1
        DCD   UART2_IRQHandler       ; 42 UART2
        DCD   UART3_IRQHandler       ; 43 UART3
        DCD   I2S0_IRQHandler        ; 44 I2S0
        DCD   I2S1_IRQHandler        ; 45 I2S1
        DCD   SPIFI_IRQHandler       ; 46 SPIFI
        DCD   SGPIO_IRQHandler       ; 47 SGPIO
        DCD   GPIO0_IRQHandler       ; 48 GPIO0
        DCD   GPIO1_IRQHandler       ; 49 GPIO1
        DCD   GPIO2_IRQHandler       ; 50 GPIO2
        DCD   GPIO3_IRQHandler       ; 51 GPIO3
        DCD   GPIO4_IRQHandler       ; 52 GPIO4
        DCD   GPIO5_IRQHandler       ; 53 GPIO5
        DCD   GPIO6_IRQHandler       ; 54 GPIO6
        DCD   GPIO7_IRQHandler       ; 55 GPIO7
        DCD   GINT0_IRQHandler       ; 56 GINT0
        DCD   GINT1_IRQHandler       ; 57 GINT1
        DCD   EVENTROUTER_IRQHandler ; 58 Event Router
        DCD   C_CAN1_IRQHandler      ; 59 CCAN1
        DCD   0                      ; 60 Reserved
        DCD   ADCHS_IRQHandler       ; 61 ADC High Speed
        DCD   ATIMER_IRQHandler      ; 62 Alarm Timer
        DCD   RTC_IRQHandler         ; 63 RTC
        DCD   0
        DCD   WWDT_IRQHandler        ; 65 WWDT
        DCD   M0SUB_IRQHandler       ; M0SUB
        DCD   C_CAN0_IRQHandler      ; 67 CCAN0
        DCD   QEI_IRQHandler         ; 68 QEI
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
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =__vector_table
        LDR     R1, =0xE000ED08 ; VTOR
        STR     R0, [R1]
/* Call LPC init function so that we can initialize the FPU on CM4 */
        LDR     R0, =Lpc43xxSystemInit
        BLX     R0 
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

  		PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
NMI_Handler
        mov r0, #4
        b UEZBSP_FatalError

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        ;mov r0, #5
        ;b UEZBSP_FatalError
        TST LR, #4
        ITE EQ
        MRSEQ r0, MSP
        MRSNE R0, PSP
        b HardFault_HandlerC
        PUBWEAK MemManage_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
MemManage_Handler
        mov r0, #6
        b UEZBSP_FatalError

        PUBWEAK BusFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
BusFault_Handler
        mov r0, #7
        b UEZBSP_FatalError

        PUBWEAK UsageFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
UsageFault_Handler
        mov r0, #8
        b UEZBSP_FatalError

        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        mov r0, #9
        b UEZBSP_FatalError

        PUBWEAK DebugMon_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
DebugMon_Handler
        mov r0, #10
        b UEZBSP_FatalError

        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        mov r0, #11
        b UEZBSP_FatalError

        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler

        PUBWEAK DAC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DAC_IRQHandler
        B DAC_IRQHandler

        PUBWEAK M0CORE_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
M0CORE_IRQHandler
        B M0CORE_IRQHandler

        PUBWEAK DMA_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
DMA_IRQHandler
        B DMA_IRQHandler

        PUBWEAK FLASHEEPROM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
FLASHEEPROM_IRQHandler
        B FLASHEEPROM_IRQHandler

        PUBWEAK ETH_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ETH_IRQHandler
        B ETH_IRQHandler

        PUBWEAK SDIO_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SDIO_IRQHandler
        B SDIO_IRQHandler

        PUBWEAK LCD_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
LCD_IRQHandler
        B LCD_IRQHandler

        PUBWEAK USB0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USB0_IRQHandler
        B USB0_IRQHandler

        PUBWEAK USB1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USB1_IRQHandler
        B USB1_IRQHandler

        PUBWEAK SCT_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SCT_IRQHandler
        B SCT_IRQHandler

        PUBWEAK RITIMER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RITIMER_IRQHandler
        B RITIMER_IRQHandler

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

        PUBWEAK MCPWM_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
MCPWM_IRQHandler
        B MCPWM_IRQHandler

        PUBWEAK ADC0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC0_IRQHandler
        B ADC0_IRQHandler

        PUBWEAK I2C0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C0_IRQHandler
        B I2C0_IRQHandler

        PUBWEAK I2C1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2C1_IRQHandler
        B I2C1_IRQHandler

        PUBWEAK SPI_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPI_IRQHandler
        B SPI_IRQHandler

        PUBWEAK ADC1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADC1_IRQHandler
        B ADC1_IRQHandler

        PUBWEAK SSP0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SSP0_IRQHandler
        B SSP0_IRQHandler

        PUBWEAK SSP1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SSP1_IRQHandler
        B SSP1_IRQHandler

        PUBWEAK USART0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USART0_IRQHandler
        B USART0_IRQHandler

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

        PUBWEAK I2S0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2S0_IRQHandler
        B I2S0_IRQHandler

        PUBWEAK I2S1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
I2S1_IRQHandler
        B I2S1_IRQHandler

        PUBWEAK SPIFI_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SPIFI_IRQHandler
        B SPIFI_IRQHandler

        PUBWEAK SGPIO_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SGPIO_IRQHandler
        B SGPIO_IRQHandler

        PUBWEAK GPIO0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO0_IRQHandler
        B GPIO0_IRQHandler

        PUBWEAK GPIO1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO1_IRQHandler
        B GPIO1_IRQHandler

        PUBWEAK GPIO2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO2_IRQHandler
        B GPIO2_IRQHandler

        PUBWEAK GPIO3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO3_IRQHandler
        B GPIO3_IRQHandler

        PUBWEAK GPIO4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO4_IRQHandler
        B GPIO4_IRQHandler

        PUBWEAK GPIO5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO5_IRQHandler
        B UART2_IRQHandler

        PUBWEAK GPIO6_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO6_IRQHandler
        B GPIO6_IRQHandler

        PUBWEAK GPIO7_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GPIO7_IRQHandler
        B GPIO7_IRQHandler

        PUBWEAK GINT0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GINT0_IRQHandler
        B GINT0_IRQHandler

        PUBWEAK GINT1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
GINT1_IRQHandler
        B GINT1_IRQHandler

        PUBWEAK EVENTROUTER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
EVENTROUTER_IRQHandler
        B EVENTROUTER_IRQHandler

        PUBWEAK C_CAN1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
C_CAN1_IRQHandler
        B C_CAN1_IRQHandler

        PUBWEAK ADCHS_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ADCHS_IRQHandler
        B ADCHS_IRQHandler

        PUBWEAK ATIMER_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ATIMER_IRQHandler
        B ATIMER_IRQHandler

        PUBWEAK RTC_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
RTC_IRQHandler
        B RTC_IRQHandler

        PUBWEAK WWDT_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
WWDT_IRQHandler
        B WWDT_IRQHandler

        PUBWEAK M0SUB_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
M0SUB_IRQHandler
        B M0SUB_IRQHandler

        PUBWEAK C_CAN0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
C_CAN0_IRQHandler
        B C_CAN0_IRQHandler

        PUBWEAK QEI_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
QEI_IRQHandler
        B QEI_IRQHandler

        PUBWEAK Default_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
Default_IRQHandler
        B Default_IRQHandler


        THUMB

        PUBLIC getPC
        SECTION .text:CODE:REORDER(2)
getPC
        MOV     R0,LR
        BX      LR


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
;;;	    DCD	0xFFFFFFFF
;;;

        END

