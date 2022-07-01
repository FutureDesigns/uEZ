/*-------------------------------------------------------------------------*
* File:  uEZProcessor_LPC1768.h
*--------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
*-------------------------------------------------------------------------*/
#ifndef _UEZPROCESSOR_LPC1768_H_
#define _UEZPROCESSOR_LPC1768_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
#include <CMSIS/LPC17xx.h>
#include <Config.h>
#include <Types/GPIO.h>
#include "LPC1768_UtilityFuncs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AHBSRAM0_BASE       0x2007C000  // Generally for USB
#define AHBSRAM1_BASE       0x20080000  // Generally for Ethernet

#define UEZ_PROCESSOR_CORE_TYPE                 CORE_TYPE_CORTEX_M3
#define UEZ_PROCESSOR_CORE_MODE                 CORE_MODE_THUMB
#define UEZ_PROCESSOR_CORE_SUBTYPE              CORE_SUBTYPE_NONE

#if (COMPILER_TYPE==RowleyARM)
#define __TARGET_PROCESSOR                      LPC1768
#endif

// External processor speed
#ifndef OSCILLATOR_CLOCK_FREQUENCY
#define OSCILLATOR_CLOCK_FREQUENCY          12000000  // Hz
#endif

#ifndef PROCESSOR_OSCILLATOR_FREQUENCY
#define PROCESSOR_OSCILLATOR_FREQUENCY      100000000 // Hz, LPC1769 is 120MHz
#endif

// Option to put the name of the interrupts in the interrupt
// table for internal debugging
#ifndef LPC1768_INTERRUPT_TRACK_NAMES
#define LPC1768_INTERRUPT_TRACK_NAMES       0
#endif

#define UEZ_MAX_IRQ_CHANNELS      35    // 0 - 34

// Macros for defining an interrupt context switch
#if (COMPILER_TYPE==CodeRed)
#define     IRQ_ROUTINE(name)  void name(void) ;\
                                void name(void)
#define     IRQ_START()
#define     IRQ_END()
#define     IRQ_HANDLER(funcname, param) \
                    IRQ_ROUTINE(funcname##_irq) \
                    IRQ_START() \
                    funcname(param);\
                    IRQ_END()
#define     __packed        __attribute__((packed))
#define     INLINE          inline
#define     IN_INTERNAL_RAM  // tbd
#endif // (COMPILER_TYPE==CodeRed)

// Macros for defining an interrupt context switch
#if (COMPILER_TYPE==RowleyARM)
#define     IRQ_ROUTINE(name)  void name(void) ;\
                                void name(void)
#define     IRQ_START()
#define     IRQ_END()
#define     IRQ_HANDLER(funcname, param) \
                    IRQ_ROUTINE(funcname##_irq) \
                    IRQ_START() \
                    funcname(param);\
                    IRQ_END()
#define     __packed        __attribute__((packed))
#define INLINE inline __attribute__((always_inline))
#define     IN_INTERNAL_RAM  __attribute__((section(".IRAM")))
#endif // (COMPILER_TYPE==RowleyARM)

#if (COMPILER_TYPE==IAR)
#define     IRQ_ROUTINE(name)  void name(void)
#define     IRQ_START() // context is saved by lower assembly routine
#define     IRQ_END()

#define     IRQ_HANDLER(funcname, param) \
                    IRQ_ROUTINE(funcname##_irq) \
                    IRQ_START() \
                    funcname(param);\
                    IRQ_END()
#define     INLINE          inline
#define     IN_INTERNAL_RAM  // tbd
#endif // (COMPILER_TYPE==IAR)

// Macros for defining an interrupt context switch
#if (COMPILER_TYPE==Keil4)
#define     IRQ_ROUTINE(name)  void name(void) ;\
                                void name(void)
#define     IRQ_START()
#define     IRQ_END()
#define     IRQ_HANDLER(funcname, param) \
                    IRQ_ROUTINE(funcname##_irq) \
                    IRQ_START() \
                    funcname(param);\
                    IRQ_END()
#define     __packed        __attribute__((packed))
#define     INLINE          __inline
#define     IN_INTERNAL_RAM  // tbd
#endif // (COMPILER_TYPE==Keil4)

#define Fpclk	(PROCESSOR_OSCILLATOR_FREQUENCY / 2)

#define INTERRUPT_CHANNEL_ETHERNET  ENET_IRQn

#define PCLK_FREQUENCY  UEZPlatform_GetPCLKFrequency()

#if(COMPILER_TYPE == RowleyARM)
	#define NOP() __NOP()
#endif
#if (COMPILER_TYPE==IAR)
    #define NOP() __no_operation()
#endif
#if(COMPILER_TYPE == Keil4)
	#define __NOP       __nop
	#define NOP()	__NOP
#endif

// Enumerate all available pins on the processor and convert to shorthand
// Port 0:
#define GPIO_P0_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 0)
#define GPIO_P0_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 1)
#define GPIO_P0_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 2)
#define GPIO_P0_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 3)
#define GPIO_P0_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 4)
#define GPIO_P0_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 5)
#define GPIO_P0_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 6)
#define GPIO_P0_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 7)
#define GPIO_P0_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 8)
#define GPIO_P0_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 9)
#define GPIO_P0_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 10)
#define GPIO_P0_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 11)

#define GPIO_P0_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 15)
#define GPIO_P0_16      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 16)
#define GPIO_P0_17      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 17)
#define GPIO_P0_18      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 18)
#define GPIO_P0_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 19)
#define GPIO_P0_20      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 20)
#define GPIO_P0_21      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 21)
#define GPIO_P0_22      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 22)
#define GPIO_P0_23      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 23)
#define GPIO_P0_24      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 24)
#define GPIO_P0_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 25)
#define GPIO_P0_26      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 26)
#define GPIO_P0_27      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 27)
#define GPIO_P0_28      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 28)
#define GPIO_P0_29      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 29)
#define GPIO_P0_30      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 30)

// Port 1:
#define GPIO_P1_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 0)
#define GPIO_P1_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 1)

#define GPIO_P1_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 4)

#define GPIO_P1_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 8)
#define GPIO_P1_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 9)
#define GPIO_P1_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 10)

#define GPIO_P1_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 14)
#define GPIO_P1_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 15)
#define GPIO_P1_16      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 16)
#define GPIO_P1_17      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 17)
#define GPIO_P1_18      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 18)
#define GPIO_P1_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 19)
#define GPIO_P1_20      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 20)
#define GPIO_P1_21      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 21)
#define GPIO_P1_22      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 22)
#define GPIO_P1_23      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 23)
#define GPIO_P1_24      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 24)
#define GPIO_P1_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 25)
#define GPIO_P1_26      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 26)
#define GPIO_P1_27      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 27)
#define GPIO_P1_28      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 28)
#define GPIO_P1_29      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 29)
#define GPIO_P1_30      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 30)
#define GPIO_P1_31      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 31)

// Port 2:
#define GPIO_P2_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 0)
#define GPIO_P2_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 1)
#define GPIO_P2_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 2)
#define GPIO_P2_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 3)
#define GPIO_P2_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 4)
#define GPIO_P2_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 5)
#define GPIO_P2_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 6)
#define GPIO_P2_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 7)
#define GPIO_P2_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 8)
#define GPIO_P2_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 9)
#define GPIO_P2_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 10)
#define GPIO_P2_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 11)
#define GPIO_P2_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 12)
#define GPIO_P2_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 13)

// Port 3:
#define GPIO_P3_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 25)
#define GPIO_P3_26      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 26)

// Port 4:
#define GPIO_P4_28      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 28)
#define GPIO_P4_29      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 29)

// LPC_IOCON requires very precise settings
// The following defines and macros combine multiple fields into one
// field so a pin can be configured.
// 2 bits for pin function
// 3 bits for pull type
// 1 bit for open drain enabled
#define IOCON_FUNC(n)               ((n)&3) // FUNC: Pin function
#define IOCON_NO_PULL               (GPIO_PULL_NONE<<2)  // MODE: No Pull-down/pull-up resistors
#define IOCON_PULL_DOWN             (GPIO_PULL_DOWN<<2)  // MODE: Pull-down resistor enabled
#define IOCON_PULL_UP               (GPIO_PULL_UP<<2)  // MODE: Pull-up resistor enabled
#define IOCON_REPEATER              (GPIO_REPEATER<<2)  // MODE: Repeater mode
#define IOCON_PUSH_PULL             (0<<5) // OD: Normal push-pull output
#define IOCON_OPEN_DRAIN            (1<<5) // OD: Simulated open-drain output (high drive disabled)

// Default settings for the type of pins.  Technically all pins default
// to function 0, but we can use these to reconfigure the pins to their
// default setting AND a new function.
#define IOCON_D_DEFAULT(funcNum)  (IOCON_FUNC(funcNum) | IOCON_PUSH_PULL | IOCON_PULL_UP)
#define IOCON_A_DEFAULT(funcNum)  (IOCON_FUNC(funcNum) | IOCON_PUSH_PULL | IOCON_NO_PULL)
#define IOCON_U_DEFAULT(funcNum)  (IOCON_FUNC(funcNum) | IOCON_PUSH_PULL | IOCON_PULL_UP)
#define IOCON_I_DEFAULT(funcNum)  (IOCON_FUNC(funcNum) | IOCON_OPEN_DRAIN | IOCON_PULL_UP)
#define IOCON_W_DEFAULT(funcNum)  (IOCON_FUNC(funcNum) | IOCON_PUSH_PULL | IOCON_PULL_UP)

#ifdef __cplusplus
}
#endif

#endif // _UEZPROCESSOR_LPC1768_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZProcessor_LPC1768.h
 *-------------------------------------------------------------------------*/
