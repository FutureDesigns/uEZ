/*-------------------------------------------------------------------------*
 * File:  uEZProcessor_LPC546xx.h
 *-------------------------------------------------------------------------*/

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
#ifndef _UEZPROCESSOR_LPC546xx_H_
#define _UEZPROCESSOR_LPC546xx_H_

#include <Config.h>
#include <CMSIS/LPC54608.h>
#include <Types/GPIO.h>
#include "LPC546xx_UtilityFuncs.h"

// Information about the processor
#define UEZ_PROCESSOR_CORE_TYPE             CORE_TYPE_CORTEX_M4

#define UEZ_PROCESSOR_CORE_MODE             CORE_MODE_THUMB
#define UEZ_PROCESSOR_CORE_SUBTYPE          CORE_SUBTYPE_NONE

//#define AHBSRAM0_BASE       0x20000000  // 16K: Ethernet descriptors/memory
//#define AHBSRAM1_BASE       0x20004000  // 16K: Ethernet memp in first 10K
//      USB in last 6K

// External processor speed
#ifndef OSCILLATOR_CLOCK_FREQUENCY
#define OSCILLATOR_CLOCK_FREQUENCY          12000000    // Hz
#endif

#ifndef PROCESSOR_OSCILLATOR_FREQUENCY
#define PROCESSOR_OSCILLATOR_FREQUENCY      180000000    // Hz
#endif

// Option to put the name of the interrupts in the interrupt
// table for internal debugging
#ifndef LPC546xx_INTERRUPT_TRACK_NAMES
#define LPC546xx_INTERRUPT_TRACK_NAMES       0
#endif

#define UEZ_MAX_IRQ_CHANNELS      57    // 0 - 56

//#ifndef PCLK_DIVIDER
//#define PCLK_DIVIDER
//#endif

#define PCLK_FREQUENCY  UEZPlatform_GetPCLKFrequency()
#define Fpclk           PCLK_FREQUENCY

#define INTERRUPT_CHANNEL_ETHERNET  ENET_IRQn

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
#define     INLINE          inline
#define     IN_INTERNAL_RAM  __attribute__((section(".IRAM")))
#endif // (COMPILER_TYPE==RowleyARM)

#if (COMPILER_TYPE==GCC)
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
#define     IN_INTERNAL_RAM  __attribute__((section(".IRAM")))
#endif // (COMPILER_TYPE==GCC)

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

#include "IAR/fsl_power.h" //TODO: Figure out what we need for other compilers.
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


#if(COMPILER_TYPE == RowleyARM)
	#define NOP() __NOP()
#endif
#if(COMPILER_TYPE == GCC)
	#define NOP() __NOP()
#endif
#if (COMPILER_TYPE==IAR)
    #define NOP() __no_operation()
#endif
#if(COMPILER_TYPE == Keil4)
	#define __NOP                             __nop
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
#define GPIO_P0_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 12)
#define GPIO_P0_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 13)
#define GPIO_P0_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 14)
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
#define GPIO_P0_31      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 31)

// Port 1:
#define GPIO_P1_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 0)
#define GPIO_P1_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 1)
#define GPIO_P1_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 2)
#define GPIO_P1_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 3)
#define GPIO_P1_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 4)
#define GPIO_P1_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 5)
#define GPIO_P1_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 6)
#define GPIO_P1_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 7)
#define GPIO_P1_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 8)
#define GPIO_P1_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 9)
#define GPIO_P1_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 10)
#define GPIO_P1_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 11)
#define GPIO_P1_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 12)
#define GPIO_P1_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 13)
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
#define GPIO_P2_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 14)
#define GPIO_P2_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 15)
#define GPIO_P2_16      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 16)
#define GPIO_P2_17      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 17)
#define GPIO_P2_18      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 18)
#define GPIO_P2_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 19)
#define GPIO_P2_20      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 20)
#define GPIO_P2_21      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 21)
#define GPIO_P2_22      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 22)
#define GPIO_P2_23      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 23)
#define GPIO_P2_24      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 24)
#define GPIO_P2_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 25)
#define GPIO_P2_26      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 26)
#define GPIO_P2_27      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 27)
#define GPIO_P2_28      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 28)
#define GPIO_P2_29      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 29)
#define GPIO_P2_30      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 30)
#define GPIO_P2_31      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 31)

// Port 3:
#define GPIO_P3_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 0)
#define GPIO_P3_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 1)
#define GPIO_P3_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 2)
#define GPIO_P3_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 3)
#define GPIO_P3_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 4)
#define GPIO_P3_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 5)
#define GPIO_P3_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 6)
#define GPIO_P3_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 7)
#define GPIO_P3_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 8)
#define GPIO_P3_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 9)
#define GPIO_P3_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 10)
#define GPIO_P3_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 11)
#define GPIO_P3_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 12)
#define GPIO_P3_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 13)
#define GPIO_P3_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 14)
#define GPIO_P3_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 15)
#define GPIO_P3_16      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 16)
#define GPIO_P3_17      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 17)
#define GPIO_P3_18      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 18)
#define GPIO_P3_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 19)
#define GPIO_P3_20      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 20)
#define GPIO_P3_21      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 21)
#define GPIO_P3_22      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 22)
#define GPIO_P3_23      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 23)
#define GPIO_P3_24      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 24)
#define GPIO_P3_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 25)
#define GPIO_P3_26      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 26)
#define GPIO_P3_27      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 27)
#define GPIO_P3_28      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 28)
#define GPIO_P3_29      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 29)
#define GPIO_P3_30      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 30)
#define GPIO_P3_31      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 31)

// Port 4:
#define GPIO_P4_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 0)
#define GPIO_P4_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 1)
#define GPIO_P4_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 2)
#define GPIO_P4_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 3)
#define GPIO_P4_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 4)
#define GPIO_P4_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 5)
#define GPIO_P4_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 6)
#define GPIO_P4_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 7)
#define GPIO_P4_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 8)
#define GPIO_P4_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 9)
#define GPIO_P4_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 10)
#define GPIO_P4_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 11)
#define GPIO_P4_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 12)
#define GPIO_P4_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 13)
#define GPIO_P4_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 14)
#define GPIO_P4_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 15)
#define GPIO_P4_16      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 16)
#define GPIO_P4_17      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 17)
#define GPIO_P4_18      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 18)
#define GPIO_P4_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 19)
#define GPIO_P4_20      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 20)
#define GPIO_P4_21      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 21)
#define GPIO_P4_22      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 22)
#define GPIO_P4_23      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 23)
#define GPIO_P4_24      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 24)
#define GPIO_P4_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 25)
#define GPIO_P4_26      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 26)
#define GPIO_P4_27      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 27)
#define GPIO_P4_28      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 28)
#define GPIO_P4_29      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 29)
#define GPIO_P4_30      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 30)
#define GPIO_P4_31      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 31)

// Port 5:
#define GPIO_P5_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 0)
#define GPIO_P5_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 1)
#define GPIO_P5_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 2)
#define GPIO_P5_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 3)
#define GPIO_P5_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 4)
#define GPIO_P5_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 5)
#define GPIO_P5_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 6)
#define GPIO_P5_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 7)
#define GPIO_P5_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 8)
#define GPIO_P5_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 9)
#define GPIO_P5_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 10)
#define GPIO_P5_16      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 16)
#define GPIO_P5_17      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 17)
#define GPIO_P5_18      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 18)
#define GPIO_P5_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 19)
#define GPIO_P5_20      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 20)
#define GPIO_P5_21      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 21)
#define GPIO_P5_22      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 22)
#define GPIO_P5_23      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 23)
#define GPIO_P5_24      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 24)
#define GPIO_P5_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 25)
#define GPIO_P5_26      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 26)

// LPC_IOCON requires very precise settings
// The following defines and macros combine multiple fields into one
// field so a pin can be configured.
#define IOCON_FUNC(n)               ((n)&0xF) // FUNC: Pin function
#define IOCON_NO_PULL               (0<<4)  // MODE: No Pull-down/pull-up resistors
#define IOCON_PULL_DOWN             (1<<4)  // MODE: Pull-down resistor enabled
#define IOCON_PULL_UP               (2<<4)  // MODE: Pull-up resistor enabled
#define IOCON_REPEATER              (3<<4)  // MODE: Repeater mode
#define IOCON_INVERT_DISABLE        (0<<7)  // INVERT: Disable
#define IOCON_INVERT_ENABLE         (1<<7)  // INVERT: Enable
#define IOCON_ANALOG                (0<<8)  // DGIMODE: Analog mode
#define IOCON_DIGITAL               (1<<8)  // DGIMODE: Digital mode
#define IOCON_FILTER_ON             (0<<9)  // FILTER: On
#define IOCON_FILTER_OFF            (1<<9)  // FILTER: Off
#define IOCON_SLEW_STANDARD         (0<<9)  // SLEW: Standard mode
#define IOCON_SLEW_FAST             (1<<9)  // SLEW: Fast mode
#define IOCON_NORMAL                (0<<11) // OD: off
#define IOCON_OPEN_DRAIN            (1<<11) // OD: Simulated open-drain output (high drive disabled)

//Type I IOCON Registers only
#define I2CSLEW_I2C                 (0<<6)  //I2CSlew: I2C mode
#define I2CSLEW_GPIO                (1<<6)  //I2CSlew: GPIO mode
#define I2CDRIVE_LOW                (0<<10) //I2CDrive: Low Drive 4mA sink
#define I2CDRIVE_HIGH               (1<<10) //I2CDrive: High Drive 20mA sing
#define I2CFILTER_ENABLED           (0<<11) //I2CFilter: On Fast mode and Fast mode+
#define I2CFILTER_DISABLED          (1<<11) //I2CFilter: Off standard I2C

// Standard set of Digital pin configs
#define IOCON_D(func, mode, inv, dig, filter, slew, od) \
    (func | mode | inv | dig | filter | slew | od)

// Standard set of Analog pin configs
#define IOCON_A(func, mode, invert, dig, filter, od) \
    (func | mode | invert | dig | filter | od )

// Standard set of I2C pin configs
#define IOCON_I(func, i2cslew, invert, filter, i2cdrive, i2cfilter) \
    (func | i2cslew | invert | filter | i2cdrive | i2cfilter)

// Default settings for the type of pins.  Technically all pins default
// to function 0, but we can use these to reconfigure the pins to their
// default setting AND a new function.
#define IOCON_D_DEFAULT(funcNum)  \
    IOCON_D(IOCON_FUNC(funcNum), \
            IOCON_PULL_UP, \
            IOCON_INVERT_DISABLE, \
            IOCON_DIGITAL, \
            IOCON_FILTER_ON, \
            IOCON_SLEW_STANDARD, \
            IOCON_NORMAL)

#define IOCON_A_DEFAULT(funcNum)  \
    IOCON_A(IOCON_FUNC(funcNum), \
            IOCON_PULL_UP, \
            IOCON_INVERT_DISABLE, \
            IOCON_DIGITAL, \
            IOCON_FILTER_ON, \ \
            IOCON_NORMAL)

#define IOCON_I_DEFAULT(funcNum)  \
    IOCON_I(IOCON_FUNC(funcNum), \
            I2CSLEW_GPIO, \
            IOCON_INVERT_DISABLE, \
            IOCON_DIGITAL, \
            IOCON_FILTER_ON, \
            I2CDRIVE_LOW, \
            I2CFILTER_DISABLED)

#endif // _UEZPROCESSOR_LPCLPC546xx_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZProcessor_LPCLPC546xx.h
 *-------------------------------------------------------------------------*/