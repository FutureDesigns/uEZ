/*-------------------------------------------------------------------------*
 * File:  uEZProcessor_LPC2478.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZPROCESSOR_LPC2478_H_
#define UEZPROCESSOR_LPC2478_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
// Include the IO define file
#include <Config.h>
#include "LPC2478.h"
#include <Types/GPIO.h>
#include "LPC2478_UtilityFuncs.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
// Enumerate all available pins on the processor and
// convert to shorthand
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

#define UEZ_MAX_IRQ_CHANNELS      32

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/

#if (COMPILER_TYPE==RowleyARM)
#define NOP()                   __asm volatile ( "NOP" )
#endif
#if (COMPILER_TYPE==IAR)
#define NOP()                   asm ( "NOP" )
#endif

#define Fpclk   (PROCESSOR_OSCILLATOR_FREQUENCY / 2)

// Macros for defining an interrupt context switch
#if (COMPILER_TYPE==RowleyARM)
#define     IRQ_ROUTINE(name)  void name(void) __attribute__((naked)); \
                                void name(void)
#define     IRQ_START() RTOS_SAVE_CONTEXT()
#define     IRQ_END() \
                    VICAddress = 0; \
                    RTOS_RESTORE_CONTEXT()
#define     IRQ_HANDLER(funcname, param) \
                    IRQ_ROUTINE(funcname##_irq) \
                    IRQ_START() \
                    funcname(param);\
                    IRQ_END()
#define     __packed        __attribute__((packed))
#define     INLINE          inline
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
#endif // (COMPILER_TYPE==RowleyARM)
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

#define PINSEL_MASK(pin)    (3 << (((pin) & 15) * 2))
#define PINSEL_MUX(pin, mux)     ((mux) << (((pin) & 15) * 2))

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

#endif // UEZPROCESSOR_LPC2478_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZProcessor_LPC2478.h
 *-------------------------------------------------------------------------*/
