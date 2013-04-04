/******************************************************************************
 * @file:    system_LPC177x_8x.h
 * @purpose: CMSIS Cortex-M3 Device Peripheral Access Layer Header File
 *           for the NXP LPC177x_8x Device Series
 * @version: V1.10
 * @date:    24. September 2010
 *----------------------------------------------------------------------------
 *
 * @note
 * Copyright (C) 2010 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#ifndef __SYSTEM_LPC177x_8x_H
#define __SYSTEM_LPC177x_8x_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemCoreClock variable.
 */
extern void SystemInit (void);

/**
 * Update SystemCoreClock variable
 *
 * @param  none
 * @return none
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from cpu registers.
 */
extern void SystemCoreClockUpdate (void);

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/
/*--------------------- Clock Configuration ----------------------------------
//
// <e> Clock Configuration
//   <h> System Controls and Status Register (SCS)
//     <o1.0>       EMC_SHIFT: EMC Shift enable
//                     <0=> Static CS addresses match bus width; AD[1] = 0 for 32 bit, AD[0] = 0 for 16+32 bit
//                     <1=> Static CS addresses start at LSB 0 regardless of memory width
//     <o1.1>       EMC_RESET: EMC Reset disable
//                     <0=> EMC will be reset by any chip reset
//                     <1=> Portions of EMC will only be reset by POR or BOR
//     <o1.2>       EMC_BURST: EMC Burst disable
//     <o1.3>       MCIPWR_LEVEL: SD card interface signal SD_PWR Active Level selection
//                     <0=> SD_PWR is active low
//                     <1=> SD_PWR is active high
//     <o1.4>       OSCRANGE: Main Oscillator Range Select
//                     <0=>  1 MHz to 20 MHz
//                     <1=> 15 MHz to 25 MHz
//     <o1.5>       OSCEN: Main Oscillator enable
//   </h>
//
//   <h> Clock Source Select Register (CLKSRCSEL)
//     <o2.0>       CLKSRC: sysclk and PLL0 clock source selection
//                     <0=> Internal RC oscillator
//                     <1=> Main oscillator
//   </h>
//
//   <e3> PLL0 Configuration (Main PLL)
//     <h> PLL0 Configuration Register (PLL0CFG)
//                     <i> PLL out clock = (F_cco / (2 * P))
//                     <i> F_cco = (F_in * M * 2 * P)
//                     <i> F_in  must be in the range of 1 MHz to 25 MHz
//                     <i> F_cco must be in the range of 9.75 MHz to 160 MHz
//       <o4.0..4>   M: PLL Multiplier Selection
//                     <i> M Value
//                     <1-32><#-1>
//       <o4.5..6>   P: PLL Divider Selection
//                     <i> P Value
//                     <0=> 1
//                     <1=> 2
//                     <2=> 4
//                     <3=> 8
//     </h>
//   </e>
//
//   <e5> PLL1 Configuration (Alt PLL)
//     <h> PLL1 Configuration Register (PLL1CFG)
//                     <i> PLL out clock = (F_cco / (2 * P))
//                     <i> F_cco = (F_in * M * 2 * P)
//                     <i> F_in  must be in the range of 1 MHz to 25 MHz
//                     <i> F_cco must be in the range of 9.75 MHz to 160 MHz
//       <o6.0..4>   M: PLL Multiplier Selection
//                     <i> M Value
//                     <1-32><#-1>
//       <o6.5..6>   P: PLL Divider Selection
//                     <i> P Value
//                     <0=> 1
//                     <1=> 2
//                     <2=> 4
//                     <3=> 8
//     </h>
//   </e>
//
//   <h> CPU Clock Selection Register (CCLKSEL)
//     <o7.0..4>    CCLKDIV: CPU clock (CCLK) divider
//                     <i> 0: The divider is turned off. No clock will be provided to the CPU
//                     <i> n: The input clock is divided by n to produce the CPU clock
//                     <0-31>
//     <o7.8>       CCLKSEL: CPU clock divider input clock selection
//                     <0=> sysclk clock
//                     <1=> PLL0 clock
//   </h>
//
//   <h> USB Clock Selection Register (USBCLKSEL)
//     <o8.0..4>    USBDIV: USB clock (source PLL0) divider selection
//                     <0=> USB clock off
//                     <4=> PLL0 / 4 (PLL0 must be 192Mhz)
//                     <6=> PLL0 / 6 (PLL0 must be 288Mhz)
//     <o8.8..9>    USBSEL: USB clock divider input clock selection
//                     <i> When CPU clock is selected, the USB can be accessed
//                     <i> by software but cannot perform USB functions
//                     <0=> CPU clock
//                     <1=> PLL0 clock
//                     <2=> PLL1 clock
//   </h>
//
//   <h> EMC Clock Selection Register (EMCCLKSEL)
//     <o9.0>       EMCDIV: EMC clock selection
//                     <0=> CPU clock
//                     <1=> CPU clock / 2
//   </h>
//
//   <h> Peripheral Clock Selection Register (PCLKSEL)
//     <o10.0..4>   PCLKDIV: APB Peripheral clock divider
//                     <i> 0: The divider is turned off. No clock will be provided to APB peripherals
//                     <i> n: The input clock is divided by n to produce the APB peripheral clock
//                     <0-31>
//   </h>
//
//   <h> Power Control for Peripherals Register (PCONP)
//     <o11.0>      PCLCD: LCD controller power/clock enable
//     <o11.1>      PCTIM0: Timer/Counter 0 power/clock enable
//     <o11.2>      PCTIM1: Timer/Counter 1 power/clock enable
//     <o11.3>      PCUART0: UART 0 power/clock enable
//     <o11.4>      PCUART1: UART 1 power/clock enable
//     <o11.5>      PCPWM0: PWM0 power/clock enable
//     <o11.6>      PCPWM1: PWM1 power/clock enable
//     <o11.7>      PCI2C0: I2C 0 interface power/clock enable
//     <o11.8>      PCUART4: UART 4 power/clock enable
//     <o11.9>      PCRTC: RTC and Event Recorder power/clock enable
//     <o11.10>     PCSSP1: SSP 1 interface power/clock enable
//     <o11.11>     PCEMC: External Memory Controller power/clock enable
//     <o11.12>     PCADC: A/D converter power/clock enable
//     <o11.13>     PCCAN1: CAN controller 1 power/clock enable
//     <o11.14>     PCCAN2: CAN controller 2 power/clock enable
//     <o11.15>     PCGPIO: IOCON, GPIO, and GPIO interrupts power/clock enable
//     <o11.16>     PCSPIFI: SPI Flash Interface power/clock enable
//     <o11.17>     PCMCPWM: Motor Control PWM power/clock enable
//     <o11.18>     PCQEI: Quadrature encoder interface power/clock enable
//     <o11.19>     PCI2C1: I2C 1 interface power/clock enable
//     <o11.20>     PCSSP2: SSP 2 interface power/clock enable
//     <o11.21>     PCSSP0: SSP 0 interface power/clock enable
//     <o11.22>     PCTIM2: Timer 2 power/clock enable
//     <o11.23>     PCTIM3: Timer 3 power/clock enable
//     <o11.24>     PCUART2: UART 2 power/clock enable
//     <o11.25>     PCUART3: UART 3 power/clock enable
//     <o11.26>     PCI2C2: I2C 2 interface power/clock enable
//     <o11.27>     PCI2S: I2S interface power/clock enable
//     <o11.28>     PCSDC: SD Card interface power/clock enable
//     <o11.29>     PCGPDMA: GPDMA function power/clock enable
//     <o11.30>     PCENET: Ethernet block power/clock enable
//     <o11.31>     PCUSB: USB interface power/clock enable
//   </h>
//
//   <h> Clock Output Configuration Register (CLKOUTCFG)
//     <o12.0..3>   CLKOUTSEL: Clock Source for CLKOUT Selection
//                     <0=> CPU clock
//                     <1=> Main Oscillator
//                     <2=> Internal RC Oscillator
//                     <3=> USB clock
//                     <4=> RTC Oscillator
//                     <5=> SPIFI clock
//                     <6=> Watchdog Oscillator
//     <o12.4..7>   CLKOUTDIV: Output Clock Divider
//                     <1-16><#-1>
//     <o12.8>      CLKOUT_EN: CLKOUT enable
//   </h>
//
// </e>
*/
#if 0
#define CLOCK_SETUP           1
#define SCS_Val               0x00000021
#define CLKSRCSEL_Val         0x00000001
#define PLL0_SETUP            1
#define PLL0CFG_Val           0x00000009
#define PLL1_SETUP            1
#define PLL1CFG_Val           0x00000023
#define CCLKSEL_Val           0x00000101
#define USBCLKSEL_Val         0x00000200
#define EMCCLKSEL_Val         0x00000001
#define PCLKSEL_Val           0x00000002
#define PCONP_Val             0x04288FDE
#define CLKOUTCFG_Val         0x00000000


/*--------------------- Flash Accelerator Configuration ----------------------
//
// <e> Flash Accelerator Configuration
//   <o1.12..15> FLASHTIM: Flash Access Time
//               <0=> 1 CPU clock (for CPU clock up to 20 MHz)
//               <1=> 2 CPU clocks (for CPU clock up to 40 MHz)
//               <2=> 3 CPU clocks (for CPU clock up to 60 MHz)
//               <3=> 4 CPU clocks (for CPU clock up to 80 MHz)
//               <4=> 5 CPU clocks (for CPU clock up to 100 MHz)
//               <5=> 6 CPU clocks (for any CPU clock)
// </e>
*/
#define FLASH_SETUP           1
#define FLASHCFG_Val          0x00005000
#endif

/*
//-------- <<< end of configuration section >>> ------------------------------
*/

#ifdef __cplusplus
}
#endif

#endif /* __SYSTEM_LPC177x_8x_H */
