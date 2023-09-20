/*-------------------------------------------------------------------------*
 * File:  uEZProcessor_LPC43xx.h
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
#ifndef _UEZPROCESSOR_LPC43xx_H_
#define _UEZPROCESSOR_LPC43xx_H_

#include <Config.h>
#include <CMSIS/LPC43xx.h>
#include <Types/GPIO.h>
#include "LPC43xx_UtilityFuncs.h"
#include "LPC43xx_EMAC.h"

// Information about the processor

// Currently UEZ_PROCESSOR_CORE_TYPE isn't used anywhere and would be non-portable.
// We use CORE_M4/CORE_M3/CORE_M0 everywhere and that is "standard" and comes from CMSIS.
// If we used sub core type, it would only ever be 1 type on 43XX parts, even on triple core parts.

#ifdef CORE_M4
#define UEZ_PROCESSOR_CORE_TYPE             CORE_TYPE_CORTEX_M4

#define UEZ_PROCESSOR_CORE_MODE             CORE_MODE_THUMB
//#define UEZ_PROCESSOR_CORE_SUBTYPE          CORE_SUBTYPE_NONE // Not used today
#else
#define UEZ_PROCESSOR_CORE_TYPE             CORE_TYPE_CORTEX_M0

#define UEZ_PROCESSOR_CORE_MODE             CORE_MODE_THUMB
//#define UEZ_PROCESSOR_CORE_SUBTYPE          CORE_SUBTYPE_NONE // Not used today
#endif

#define AHBSRAM0_BASE       0x20000000  // 16K: Ethernet descriptors/memory
#define AHBSRAM1_BASE       0x20004000  // 16K: Ethernet memp in first 10K
//      USB in last 6K

// External processor speed
#ifndef OSCILLATOR_CLOCK_FREQUENCY
#define OSCILLATOR_CLOCK_FREQUENCY          12000000    // Hz
#endif

#ifndef PROCESSOR_OSCILLATOR_FREQUENCY
#define PROCESSOR_OSCILLATOR_FREQUENCY      204000000    // Hz
#endif

// Option to put the name of the interrupts in the interrupt
// table for internal debugging
#ifndef LPC43XX_INTERRUPT_TRACK_NAMES
#define LPC43XX_INTERRUPT_TRACK_NAMES       0
#endif

// See IRQn_Type in LPC43XX.h
#ifdef CORE_M4
#define UEZ_MAX_IRQ_CHANNELS      53    // 0 - 52
#else // Same number for both M0 and M0SUB
#define UEZ_MAX_IRQ_CHANNELS      32    // 0 - 31
#endif


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
#define INLINE inline __attribute__((always_inline))
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
#define INLINE inline __attribute__((always_inline))
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

// Enumerate all available GPIO pins on the processor and convert to shorthand, based on LBGA256
// Port 0:
#define GPIO_P0_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 0 ) //P0_0  func 0
#define GPIO_P0_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 1 ) //P0_1  func 0
#define GPIO_P0_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 2 ) //P1_15 func 0
#define GPIO_P0_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 3 ) //P1_16 func 0
#define GPIO_P0_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 4 ) //P1_0  func 0
#define GPIO_P0_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 5 ) //P6_6  func 0
#define GPIO_P0_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 6 ) //P3_6  func 0
#define GPIO_P0_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 7 ) //P2_7  func 0
#define GPIO_P0_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 8 ) //P1_1  func 0
#define GPIO_P0_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 9 ) //P1_2  func 0
#define GPIO_P0_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 10) //P1_3  func 0
#define GPIO_P0_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 11) //P1_4  func 0
#define GPIO_P0_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 12) //P1_17 func 0
#define GPIO_P0_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 13) //P1_18 func 0
#define GPIO_P0_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 14) //P2_10 func 0
#define GPIO_P0_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 15) //P1_20 func 0

// Port 1:
#define GPIO_P1_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 0 ) //P1_7  func 0
#define GPIO_P1_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 1 ) //P1_8  func 0
#define GPIO_P1_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 2 ) //P1_9  func 0
#define GPIO_P1_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 3 ) //P1_10 func 0
#define GPIO_P1_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 4 ) //P1_11 func 0
#define GPIO_P1_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 5 ) //P1_12 func 0
#define GPIO_P1_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 6 ) //P1_13 func 0
#define GPIO_P1_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 7 ) //P1_7  func 0
#define GPIO_P1_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 8 ) //P1_5  func 0
#define GPIO_P1_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 9 ) //P1_6  func 0
#define GPIO_P1_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 10) //P2_9  func 0
#define GPIO_P1_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 11) //P2_10 func 0
#define GPIO_P1_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 12) //P2_12 fucn 0
#define GPIO_P1_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 13) //P2_13 func 0
#define GPIO_P1_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 14) //P3_4  func 0
#define GPIO_P1_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 15) //P3_5  func 0

// Port 2:
#define GPIO_P2_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 0 ) //P4_0  func 0
#define GPIO_P2_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 1 ) //P4_1  func 0
#define GPIO_P2_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 2 ) //P4_2  func 0
#define GPIO_P2_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 3 ) //P4_3  func 0
#define GPIO_P2_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 4 ) //P4_4  func 0
#define GPIO_P2_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 5 ) //P4_5  func 0
#define GPIO_P2_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 6 ) //P4_6  func 0
#define GPIO_P2_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 7 ) //P5_7  func 0
#define GPIO_P2_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 8 ) //P6_12 func 0
#define GPIO_P2_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 9 ) //P5_0  func 0
#define GPIO_P2_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 10) //P5_1  func 0
#define GPIO_P2_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 11) //P5_2  func 0
#define GPIO_P2_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 12) //P5_3  func 0
#define GPIO_P2_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 13) //P5_4  func 0
#define GPIO_P2_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 14) //P5_5  func 0
#define GPIO_P2_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 15) //P5_6  func 0

// Port 3:
#define GPIO_P3_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 0 ) //P6_1  func 0
#define GPIO_P3_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 1 ) //P6_2  func 0
#define GPIO_P3_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 2 ) //P6_3  func 0
#define GPIO_P3_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 3 ) //P6_4  func 0
#define GPIO_P3_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 4 ) //P6_5  func 0
#define GPIO_P3_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 5 ) //P6_9  func 0
#define GPIO_P3_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 6 ) //P6_10 func 0
#define GPIO_P3_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 7 ) //P6_11 func 0
#define GPIO_P3_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 8 ) //P7_0  func 0
#define GPIO_P3_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 9 ) //P7_1  func 0
#define GPIO_P3_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 10) //P7_2  func 0
#define GPIO_P3_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 11) //P7_3  func 0
#define GPIO_P3_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 12) //P7_4  func 0
#define GPIO_P3_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 13) //P7_5  func 0
#define GPIO_P3_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 14) //P7_6  func 0
#define GPIO_P3_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 15) //P7_7  func 0

// Port 4:
#define GPIO_P4_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 0 ) //P8_0  func 0
#define GPIO_P4_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 1 ) //P8_1  func 0
#define GPIO_P4_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 2 ) //P8_2  func 0
#define GPIO_P4_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 3 ) //P8_3  func 0
#define GPIO_P4_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 4 ) //P8_4  func 0
#define GPIO_P4_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 5 ) //P8_5  func 0
#define GPIO_P4_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 6 ) //P8_6  func 0
#define GPIO_P4_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 7 ) //P8_7  func 0
#define GPIO_P4_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 8 ) //PA_1  func 0
#define GPIO_P4_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 9 ) //PA_2  func 0
#define GPIO_P4_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 10) //PA_3  func 0
#define GPIO_P4_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 11) //P9_6  func 0
#define GPIO_P4_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 12) //P9_0  func 0
#define GPIO_P4_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 13) //P9_1  func 0
#define GPIO_P4_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 14) //P9_2  func 0
#define GPIO_P4_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 15) //P9_3  func 0

// Port 5:
#define GPIO_P5_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 0 ) //P2_0  func 4
#define GPIO_P5_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 1 ) //P2_1  func 4
#define GPIO_P5_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 2 ) //P2_2  func 4
#define GPIO_P5_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 3 ) //P2_3  func 4
#define GPIO_P5_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 4 ) //P2_4  func 4
#define GPIO_P5_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 5 ) //P2_5  func 4
#define GPIO_P5_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 6 ) //P2_6  func 4
#define GPIO_P5_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 7 ) //P2_8  func 4
#define GPIO_P5_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 8 ) //P3_1  fucn 4
#define GPIO_P5_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 9 ) //P3_2  fucn 4
#define GPIO_P5_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 10) //P3_7  fucn 4
#define GPIO_P5_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 11) //P3_8  fucn 4
#define GPIO_P5_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 12) //P4_8  func 4
#define GPIO_P5_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 13) //P4_9  func 4
#define GPIO_P5_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 14) //P4_10 func 4
#define GPIO_P5_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 15) //P6_7  func 4
#define GPIO_P5_16      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 16) //P6_8  func 4
#define GPIO_P5_17      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 17) //P9_4  func 4
#define GPIO_P5_18      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 18) //P9_5  func 4
#define GPIO_P5_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 19) //PA_4  func 4
#define GPIO_P5_20      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 20) //PB_0  func 4
#define GPIO_P5_21      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 21) //PB_1  func 4
#define GPIO_P5_22      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 22) //PB_2  func 4
#define GPIO_P5_23      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 23) //PB_3  func 4
#define GPIO_P5_24      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 24) //PB_4  func 4
#define GPIO_P5_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 25) //PB_5  func 4
#define GPIO_P5_26      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 26) //PB_6  func 4

// Port 6:
#define GPIO_P6_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 0 ) //PC_1  func 4
#define GPIO_P6_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 1 ) //PC_2  func 4
#define GPIO_P6_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 2 ) //PC_3  func 4
#define GPIO_P6_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 3 ) //PC_4  func 4
#define GPIO_P6_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 4 ) //PC_5  func 4
#define GPIO_P6_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 5 ) //PC_6  func 4
#define GPIO_P6_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 6 ) //PC_7  func 4
#define GPIO_P6_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 7 ) //PC_8  func 4
#define GPIO_P6_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 8 ) //PC_9  func 4
#define GPIO_P6_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 9 ) //PC_10 func 4
#define GPIO_P6_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 10) //PC_11 func 4
#define GPIO_P6_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 11) //PC_12 func 4
#define GPIO_P6_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 12) //PC_13 func 4
#define GPIO_P6_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 13) //PC_14 func 4
#define GPIO_P6_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 14) //PD_0  func 4
#define GPIO_P6_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 15) //PD_1  func 4
#define GPIO_P6_16      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 16) //PD_2  func 4
#define GPIO_P6_17      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 17) //PD_3  func 4
#define GPIO_P6_18      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 18) //PD_4  func 4
#define GPIO_P6_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 19) //PD_5  func 4
#define GPIO_P6_20      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 20) //PD_6  func 4
#define GPIO_P6_21      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 21) //PD_7  func 4
#define GPIO_P6_22      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 22) //PD_8  func 4
#define GPIO_P6_23      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 23) //PD_9  func 4
#define GPIO_P6_24      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 24) //PD_10 func 4
#define GPIO_P6_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 25) //PD_11 func 4
#define GPIO_P6_26      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 26) //PD_12 func 4
#define GPIO_P6_27      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 27) //PD_13 func 4
#define GPIO_P6_28      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 28) //PD_14 func 4
#define GPIO_P6_29      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 29) //PD_15 func 4
#define GPIO_P6_30      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 30) //PD_16 func 4

// Port 7:
#define GPIO_P7_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 0 ) //PE_0  func 4
#define GPIO_P7_1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 1 ) //PE_1  func 4
#define GPIO_P7_2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 2 ) //PE_2  func 4
#define GPIO_P7_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 3 ) //PE_3  func 4
#define GPIO_P7_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 4 ) //PE_4  func 4
#define GPIO_P7_5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 5 ) //PE_5  func 4
#define GPIO_P7_6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 6 ) //PE_6  func 4
#define GPIO_P7_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 7 ) //PE_7  func 4
#define GPIO_P7_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 8 ) //PE_8  func 4
#define GPIO_P7_9       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 9 ) //PE_9  func 4
#define GPIO_P7_10      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 10) //PE_10 func 4
#define GPIO_P7_11      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 11) //PE_11 func 4
#define GPIO_P7_12      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 12) //PE_12 func 4
#define GPIO_P7_13      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 13) //PE_13 func 4
#define GPIO_P7_14      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 14) //PE_14 func 4
#define GPIO_P7_15      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 15) //PE_15 func 4
#define GPIO_P7_16      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 16) //PF_1  func 4
#define GPIO_P7_17      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 17) //PF_2  func 4
#define GPIO_P7_18      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 18) //PF_3  func 4
#define GPIO_P7_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 19) //PF_5  func 4
#define GPIO_P7_20      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 20) //PF_6  func 4
#define GPIO_P7_21      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 21) //PF_7  func 4
#define GPIO_P7_22      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 22) //PF_8  func 4
#define GPIO_P7_23      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 23) //PF_9  func 4
#define GPIO_P7_24      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 24) //PF_10 func 4
#define GPIO_P7_25      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 25) //PF_11 func 4

//Port Z, LPC4357 pins that do not have GPIO Functionality
//Cannot be used as a GPIO
#define GPIO_PZ_0_P1_19      UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 0 ) //P1_19
#define GPIO_PZ_1_P3_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 1 ) //P3_0
#define GPIO_PZ_2_P3_3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 2 ) //P3_3
#define GPIO_PZ_3_P4_7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 3 ) //P4_7
#define GPIO_PZ_4_P6_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 4 ) //P6_0
#define GPIO_PZ_5_P8_8       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 5 ) //P8_8
#define GPIO_PZ_6_PA_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 6 ) //PA_0
#define GPIO_PZ_Z_PC_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 7 ) //PC_0
#define GPIO_PZ_8_PF_0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 8 ) //PF_0
#define GPIO_PZ_9_PF_4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_Z, 9 ) //PF_4


//SCU configurations and registers
#define SCU_MODE(n)                 ((n)&7) //Function, defaults to 0 in most cases
#define SCU_EPD_DISABLE             (0 << 3) //EPD Enable/Disable pull-down
#define SCU_EPD_ENABLE              (1 << 3) //EPD Enable/Disable pull-down
#define SCU_EPUN_DISABLE            (1 << 4) //EPUN ENable/Disable pull-up
#define SCU_EPUN_ENABLE             (0 << 4) //EPUN ENable/Disable pull-up
#define SCU_EHS_SLOW                (0 << 5) //EHS Slew Rate Slow or Fast
#define SCU_EHS_FAST                (1 << 5) //EHS Slew Rate Slow or Fast
#define SCU_EZI_DISABLE             (0 << 6) //EZI Enable/Disable Input Buffer
#define SCU_EZI_ENABLE              (1 << 6) //EZI Enable/Disable Input Buffer
#define SCU_ZIF_DISABLE             (1 << 7) //ZID Enable/Disable Glitch Filter
#define SCU_ZIF_ENABLE              (0 << 7) //ZID Enable/Disable Glitch Filter
#define SCU_EHD_NORMAL              (0 << 8) //EHD Normal/Medium/High/UltraHigh drive strength
#define SCU_EHD_MEDIUM              (1 << 8) //EHD Normal/Medium/High/UltraHigh drive strength
#define SCU_EHD_HIGH                (2 << 8) //EHD Normal/Medium/High/UltraHigh drive strength
#define SCU_EHD_ULTRA               (3 << 8) //EHD Normal/Medium/High/UltraHigh drive strength

//USB1 Pins
#define SCU_USB_AIM_LOW             (0 << 0) //USB AIM Low/High differential data input
#define SCU_USB_AIM_HIGH            (1 << 0) //USB AIM Low/High differential data input
#define SCU_USB_ESEA_RESERVED       (0 << 0) //DO NOT USE
#define SCU_USB_ESEA_ENABLE         (1 << 1) //USB ESEA Enables USB1, use with on chip PHY
#define SCU_USB_EPD_PD_DISCONNECT   (0 << 2) //USB EPD Disconnect/Connect on pull-down
#define SCU_USB_EPD_PD_CONNECT      (1 << 2) //USB EPD Disconnect/Connect on pull-down
#define SCU_USB_EPWR_POWER_SAVE     (0 << 4) //USB EPWR Power Save/Normal mode
#define SCU_USB_EPWR_NORMAL         (1 << 4) //USB EPWR Power Save/Normal mode
#define SCU_USB_VBUS_LOW_ACTIVE     (0 << 5) //USB VBUS Low/High Active
#define SCU_USB_VBUS_HIGH_ACTIVE    (1 << 5) //USB VBUS Low/High Active

//I2C open drain pins
#define SCU_I2C_SCL_EFP_50          (0 << 0 ) //SCL EFP 50/3 nS glitch filter
#define SCU_I2C_SCL_EFP_3           (1 << 0 ) //SCL EFP 50/3 nS glitch filter
#define SCU_I2C_SCL_EHD_STD_FAST    (0 << 2 ) //SCL EHD (Standard / Fast)/Fast Plus mode
#define SCU_I2C_SCL_EHD_FAST_PLUS   (1 << 2 ) //SCL EHD (Standard / Fast)/Fast Plus mode
#define SCU_I2C_SCL_EZI_DISABLE     (0 << 3 ) //SCL EZI Enable / Disable input receiver
#define SCU_I2C_SCL_EZI_ENABLE      (1 << 3 ) //SCL EZI Enable / Disable input receiver
#define SCU_I2C_SCL_ZIF_ENABLE      (0 << 7 ) //SCL ZIF ENable / Disable input filter
#define SCU_I2C_SCL_ZIF_DISABLE     (1 << 7 ) //SCL ZIF ENable / Disable input filter
#define SCU_I2C_SDA_EFP_50          (0 << 8 ) //SDA EFP 50/3 nS glitch filter
#define SCU_I2C_SDA_EFP_3           (1 << 8 ) //SDA EFP 50/3 nS glitch filter
#define SCU_I2C_SDA_EHD_STD_FAST    (0 << 10) //SDA EHD (Standard / Fast)/Fast Plus mode
#define SCU_I2C_SDA_EHD_FAST_PLUS   (1 << 10) //SDA EHD (Standard / Fast)/Fast Plus mode
#define SCU_I2C_SDA_EZI_DISABLE     (0 << 11) //SDA Enable / Disable SDA pin
#define SCU_I2C_SDA_EZI_ENABLE      (1 << 11) //SDA Enable / Disable SDA pin
#define SCU_I2C_SDA_ZIF_ENABLE      (0 << 15) //SDA ZIF Enable / Disable input filter
#define SCU_I2C_SDA_ZIF_DISABLE     (1 << 15) //SDA ZIF Enable / Disable input filter

//Normal drive pins, most pins (others will be listed below)
#define SCU_NORMAL_DRIVE(mode, epd, epun, ehs, ezi, zif) \
        (mode | epd | epun | ehs | ezi | zif)

//High Drive pins, P1_17, P2_3 to P2_5, P8_0 to P8_2, PA_1 to PA_3
#define SCU_HIGH_DRIVE(mode, epd, epun, ezi, zif, ehd) \
        (mode | epd | epun | ezi | zif | ehd)

//High speed pins, P3_3 and CLK0 to CLK3
#define SCU_HIGH_SPEED(mode, epd, epun, ehs, ezi, zif) \
        (mode | epd | epun | ehs | ezi | zif)

//USB1 Pins
#define SCU_USB1_DP_DM(aim, esea, epd, epwr, vbus) \
        (aim | esea | epd | epwr | vbus)

//I2C Pins
#define SCU_I2C_SDA_SCL(scl_efp , scl_ehd , scl_ezi , scl_zif , sda_efp , sda_ehd , sda_ezi , sda_zif) \
                       (scl_efp | scl_ehd | scl_ezi | scl_zif | sda_efp | sda_ehd | sda_ezi | sda_zif)

// Normal pins using normal registers
#define SCU_NORMAL_DRIVE_DEFAULT(mode) \
        SCU_NORMAL_DRIVE(mode, \
                SCU_EPD_DISABLE, \
                SCU_EPUN_DISABLE, \
                SCU_EHS_FAST, \
                SCU_EZI_ENABLE, \
                SCU_ZIF_ENABLE)
// Normal pins using normal registers in high freq mode
#define SCU_NORMAL_DRIVE_HIGH_FREQ(mode) \
        SCU_NORMAL_DRIVE(mode, \
                SCU_EPD_DISABLE, \
                SCU_EPUN_DISABLE, \
                SCU_EHS_FAST, \
                SCU_EZI_ENABLE, \
                SCU_ZIF_DISABLE)  // Disable filter for 30+MHZ signals

// Normal pins using normal registers in high freq mode output only // Use for LCD, etc.
#define SCU_NORMAL_DRIVE_HIGH_FREQ_OUTPUT_ONLY(mode) \
        SCU_NORMAL_DRIVE(mode, \
                SCU_EPD_DISABLE, \
                SCU_EPUN_DISABLE, \
                SCU_EHS_FAST, \
                SCU_EZI_DISABLE, \
                SCU_ZIF_DISABLE)  // Disable filter for 30+MHZ signals

// High drive pins only: P1_17, P2_3 to P2_5, P8_0 to P8_2, PA_1 to PA_3
#define SCU_HIGH_DRIVE_DEFAULT(mode) \
        SCU_HIGH_DRIVE(mode, \
                SCU_EPD_DISABLE, \
                SCU_EPUN_DISABLE, \
                SCU_EZI_ENABLE, \
                SCU_ZIF_ENABLE, \
                SCU_EHD_NORMAL)
                
// High drive pins only in high freq mode // Use for LCD, etc.
#define SCU_HIGH_DRIVE_HIGH_FREQ_OUTPUT_ONLY(mode) \
        SCU_HIGH_DRIVE(mode, \
                SCU_EPD_DISABLE, \
                SCU_EPUN_DISABLE, \
                SCU_EZI_DISABLE, \
                SCU_ZIF_DISABLE, \
                SCU_EHD_NORMAL)

// P3_3 and pins CLK0 to CLK3 only
#define SCU_HIGH_SPEED_DEFAULT(mode) \
        SCU_HIGH_SPEED(mode, \
                SCU_EPD_DISABLE, \
                SCU_EPUN_DISABLE, \
                SCU_EHS_FAST, \
                SCU_EZI_ENABLE, \
                SCU_ZIF_ENABLE)

#define SCU_USB1_DP_DM_DEFAULT \
        SCU_USB1_DP_DM()

#define SCU_I2C_SDA_SCL_DEFAULT \
        SCU_I2C_SDA_SCL(SCU_I2C_SCL_EFP_50, SCU_I2C_SCL_EHD_STD_FAST, SCU_I2C_SCL_EZI_ENABLE, SCU_I2C_SCL_ZIF_DISABLE, \
                        SCU_I2C_SDA_EFP_50, SCU_I2C_SDA_EHD_STD_FAST, SCU_I2C_SDA_EZI_ENABLE, SCU_I2C_SDA_ZIF_DISABLE)


#endif // _UEZPROCESSOR_LPCLPC43xx_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZProcessor_LPCLPC43xx.h
 *-------------------------------------------------------------------------*/
