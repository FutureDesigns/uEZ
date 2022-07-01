/*-------------------------------------------------------------------------*
 * File:  uEZProcessor_RX63N.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZPROCESSOR_RX63N
#define UEZPROCESSOR_RX63N

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

#ifdef __cplusplus
extern "C" {
#endif


#define UEZ_MAX_IRQ_CHANNELS      256    // 0 - 255

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


// Enumerate all available pins on the processor and
// convert to shorthand
// Port 0:
#define GPIO_P00       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 0)
#define GPIO_P01       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 1)
#define GPIO_P02       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 2)
#define GPIO_P03       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 3)
#define GPIO_P05       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 5)
#define GPIO_P07       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_0, 7)

// Port 1:
#define GPIO_P10       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 0)
#define GPIO_P11       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 1)
#define GPIO_P12       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 2)
#define GPIO_P13       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 3)
#define GPIO_P14       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 4)
#define GPIO_P15       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 5)
#define GPIO_P16       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 6)
#define GPIO_P17       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_1, 7)

// Port 2:
#define GPIO_P20       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 0)
#define GPIO_P21       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 1)
#define GPIO_P22       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 2)
#define GPIO_P23       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 3)
#define GPIO_P24       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 4)
#define GPIO_P25       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 5)
#define GPIO_P26       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 6)
#define GPIO_P27       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_2, 7)

// Port 3:
#define GPIO_P30       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 0)
#define GPIO_P31       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 1)
#define GPIO_P32       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 2)
#define GPIO_P33       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 3)
#define GPIO_P34       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 4)
#define GPIO_P35       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 5)
#define GPIO_P36       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 6)
#define GPIO_P37       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_3, 7)

// Port 4:
#define GPIO_P40       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 0)
#define GPIO_P41       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 1)
#define GPIO_P42       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 2)
#define GPIO_P43       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 3)
#define GPIO_P44       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 4)
#define GPIO_P45       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 5)
#define GPIO_P46       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 6)
#define GPIO_P47       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_4, 7)

// Port 5:
#define GPIO_P50       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 0)
#define GPIO_P51       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 1)
#define GPIO_P52       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 2)
#define GPIO_P53       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 3)
#define GPIO_P54       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 4)
#define GPIO_P55       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 5)
#define GPIO_P56       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 6)
#define GPIO_P57       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_5, 7)

// Port 6:
#define GPIO_P60       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 0)
#define GPIO_P61       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 1)
#define GPIO_P62       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 2)
#define GPIO_P63       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 3)
#define GPIO_P64       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 4)
#define GPIO_P65       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 5)
#define GPIO_P66       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 6)
#define GPIO_P67       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_6, 7)

// Port 7:
#define GPIO_P70       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 0)
#define GPIO_P71       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 1)
#define GPIO_P72       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 2)
#define GPIO_P73       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 3)
#define GPIO_P74       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 4)
#define GPIO_P75       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 5)
#define GPIO_P76       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 6)
#define GPIO_P77       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_7, 7)

// Port 8:
#define GPIO_P80       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_8, 0)
#define GPIO_P81       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_8, 1)
#define GPIO_P82       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_8, 2)
#define GPIO_P83       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_8, 3)
#define GPIO_P84       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_8, 4)
#define GPIO_P85       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_8, 5)
#define GPIO_P86       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_8, 6)
#define GPIO_P87       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_8, 7)

// Port 9:
#define GPIO_P90       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_9, 0)
#define GPIO_P91       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_9, 1)
#define GPIO_P92       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_9, 2)
#define GPIO_P93       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_9, 3)
#define GPIO_P94       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_9, 4)
#define GPIO_P95       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_9, 5)
#define GPIO_P96       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_9, 6)
#define GPIO_P97       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_9, 7)

// Port A:
#define GPIO_PA0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_A, 0)
#define GPIO_PA1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_A, 1)
#define GPIO_PA2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_A, 2)
#define GPIO_PA3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_A, 3)
#define GPIO_PA4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_A, 4)
#define GPIO_PA5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_A, 5)
#define GPIO_PA6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_A, 6)
#define GPIO_PA7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_A, 7)

// Port B:
#define GPIO_PB0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_B, 0)
#define GPIO_PB1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_B, 1)
#define GPIO_PB2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_B, 2)
#define GPIO_PB3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_B, 3)
#define GPIO_PB4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_B, 4)
#define GPIO_PB5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_B, 5)
#define GPIO_PB6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_B, 6)
#define GPIO_PB7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_B, 7)

// Port C:
#define GPIO_PC0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_C, 0)
#define GPIO_PC1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_C, 1)
#define GPIO_PC2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_C, 2)
#define GPIO_PC3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_C, 3)
#define GPIO_PC4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_C, 4)
#define GPIO_PC5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_C, 5)
#define GPIO_PC6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_C, 6)
#define GPIO_PC7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_C, 7)

// Port D:
#define GPIO_PD0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_D, 0)
#define GPIO_PD1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_D, 1)
#define GPIO_PD2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_D, 2)
#define GPIO_PD3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_D, 3)
#define GPIO_PD4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_D, 4)
#define GPIO_PD5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_D, 5)
#define GPIO_PD6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_D, 6)
#define GPIO_PD7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_D, 7)

// Port E:
#define GPIO_PE0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_E, 0)
#define GPIO_PE1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_E, 1)
#define GPIO_PE2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_E, 2)
#define GPIO_PE3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_E, 3)
#define GPIO_PE4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_E, 4)
#define GPIO_PE5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_E, 5)
#define GPIO_PE6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_E, 6)
#define GPIO_PE7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_E, 7)

// Port F:
#define GPIO_PF0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_F, 0)
#define GPIO_PF1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_F, 1)
#define GPIO_PF2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_F, 2)
#define GPIO_PF3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_F, 3)
#define GPIO_PF4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_F, 4)
#define GPIO_PF5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_F, 5)

// Port G:
#define GPIO_PG0       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_G, 0)
#define GPIO_PG1       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_G, 1)
#define GPIO_PG2       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_G, 2)
#define GPIO_PG3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_G, 3)
#define GPIO_PG4       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_G, 4)
#define GPIO_PG5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_G, 5)
#define GPIO_PG6       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_G, 6)
#define GPIO_PG7       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_G, 7)

// Port J:
#define GPIO_PJ3       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_J, 3)
#define GPIO_PJ5       UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_J, 5)

#ifdef __cplusplus
}
#endif

#endif // UEZPROCESSOR_RX63N
/*-------------------------------------------------------------------------*
 * End of File:  uEZProcessor_RX63N.h
 *-------------------------------------------------------------------------*/