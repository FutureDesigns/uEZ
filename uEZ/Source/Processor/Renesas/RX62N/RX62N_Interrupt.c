/*-------------------------------------------------------------------------*
 * File:  RX62N_Interrupt.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Interrupt control for Renesas H8SX 1668RF.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <HAL/Interrupt.h>
#include <uEZProcessor.h>
#include <uEZProcessor.h>
#include "portmacro.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
//    TUInt32 iInterruptChannel;
    TFPtr iISR;
    const char *iName;
} T_irqHandleStruct;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_irqHandleStruct G_isrArray[UEZ_MAX_IRQ_CHANNELS];

static const TUInt8 G_IPRTable[256] = {
        0, // 0
        0, // 1
        0, // 2
        0, // 3
        0, // 4
        0, // 5
        0, // 6
        0, // 7
        0, // 8
        0, // 9
        0, // 10
        0, // 11
        0, // 12
        0, // 13
        0, // 14
        0, // 15
        /* IPR_BSC_BUSERR */ 0x00, // 16
        0, // 17
        0, // 18
        0, // 19
        0, // 20
        /* IPR_FCU_FIFERR */ 0x01, // 21
        0, // 22
        /* IPR_FCU_FRDYI */ 0x02, // 23
        0, // 24
        0, // 25
        0, // 26
        /* IPR_ICU_SWINT */ 0x03, // 27
        /* IPR_CMT0_CMI0 */ 0x04, // 28
        /* IPR_CMT1_CMI1 */ 0x05, // 29
        /* IPR_CMT2_CMI2 */ 0x06, // 30
        /* IPR_CMT3_CMI3 */ 0x07, // 31
        /* IPR_ETHER_EINT */ 0x08, // 32
        0, // 33
        0, // 34
        0, // 35
        /* IPR_USB0_D0FIFO0 */ 0x0C, // 36
        /* IPR_USB0_D1FIFO0 */ 0x0D, // 37
        /* IPR_USB0_USBI0 */ 0x0E, // 38
        0, // 39
        /* IPR_USB1_D0FIFO1 */ 0x10, // 40
        /* IPR_USB1_D1FIFO1 */ 0x11, // 41
        /* IPR_USB1_USBI1 */ 0x12, // 42
        0, // 43
        /* IPR_RSPI0_SPEI0 */ 0x14, // 44
        /* IPR_RSPI0_SPRI0 */ 0x14, // 45
        /* IPR_RSPI0_SPTI0 */ 0x14, // 46
        /* IPR_RSPI0_SPII0 */ 0x14, // 47
        /* IPR_RSPI1_SPEI1 */ 0x15, // 48
        /* IPR_RSPI1_SPRI1 */ 0x15, // 49
        /* IPR_RSPI1_SPTI1 */ 0x15, // 50
        /* IPR_RSPI1_SPII1 */ 0x15, // 51
        0, // 52
        0, // 53
        0, // 54
        0, // 55
        /* IPR_CAN0_ERS0 */ 0x18, // 56
        /* IPR_CAN0_RXF0 */ 0x18, // 57
        /* IPR_CAN0_TXF0 */ 0x18, // 58
        /* IPR_CAN0_RXM0 */ 0x18, // 59
        /* IPR_CAN0_TXM0 */ 0x18, // 60
        0, // 61
        /* IPR_RTC_PRD */ 0x1E, // 62
        /* IPR_RTC_CUP */ 0x1F, // 63
        /* IPR_ICU_IRQ0 */ 0x20, // 64
        /* IPR_ICU_IRQ1 */ 0x21, // 65
        /* IPR_ICU_IRQ2 */ 0x22, // 66
        /* IPR_ICU_IRQ3 */ 0x23, // 67
        /* IPR_ICU_IRQ4 */ 0x24, // 68
        /* IPR_ICU_IRQ5 */ 0x25, // 69
        /* IPR_ICU_IRQ6 */ 0x26, // 70
        /* IPR_ICU_IRQ7 */ 0x27, // 71
        /* IPR_ICU_IRQ8 */ 0x28, // 72
        /* IPR_ICU_IRQ9 */ 0x29, // 73
        /* IPR_ICU_IRQ10 */ 0x2A, // 74
        /* IPR_ICU_IRQ11 */ 0x2B, // 75
        /* IPR_ICU_IRQ12 */ 0x2C, // 76
        /* IPR_ICU_IRQ13 */ 0x2D, // 77
        /* IPR_ICU_IRQ14 */ 0x2E, // 78
        /* IPR_ICU_IRQ15 */ 0x2F, // 79
        0, // 80
        0, // 81
        0, // 82
        0, // 83
        0, // 84
        0, // 85
        0, // 86
        0, // 87
        0, // 88
        0, // 89
        /* IPR_USB_USBR0 */ 0x3A, // 90
        /* IPR_USB_USBR1 */ 0x3B, // 91
        /* IPR_RTC_ALM */ 0x3C, // 92
        0, // 93
        0, // 94
        0, // 95
        /* IPR_WDT_WOVI */ 0x40, // 96
        0, // 97
        /* IPR_AD0_ADI0 */ 0x44, // 98
        /* IPR_AD1_ADI1 */ 0x45, // 99
        0, // 100
        0, // 101
        /* IPR_S12AD_ADI */ 0x48, // 102
        0, // 103
        0, // 104
        0, // 105
        0, // 106
        0, // 107
        0, // 108
        0, // 109
        0, // 110
        0, // 111
        0, // 112
        0, // 113
        /* IPR_MTU0_TGIA0 */ 0x51, // 114
        /* IPR_MTU0_TGIB0 */ 0x51, // 115
        /* IPR_MTU0_TGIC0 */ 0x51, // 116
        /* IPR_MTU0_TGID0 */ 0x51, // 117
        /* IPR_MTU0_TCIV0 */ 0x52, // 118
        /* IPR_MTU0_TGIE0 */ 0x52, // 119
        /* IPR_MTU0_TGIF0 */ 0x52, // 120
        /* IPR_MTU1_TGIA1 */ 0x53, // 121
        /* IPR_MTU1_TGIB1 */ 0x53, // 122
        /* IPR_MTU1_TCIV1 */ 0x54, // 123
        /* IPR_MTU1_TCIU1 */ 0x54, // 124
        /* IPR_MTU2_TGIA2 */ 0x55, // 125
        /* IPR_MTU2_TGIB2 */ 0x55, // 126
        /* IPR_MTU2_TCIV2 */ 0x56, // 127
        /* IPR_MTU2_TCIU2 */ 0x56, // 128
        /* IPR_MTU3_TGIA3 */ 0x57, // 129
        /* IPR_MTU3_TGIB3 */ 0x57, // 130
        /* IPR_MTU3_TGIC3 */ 0x57, // 131
        /* IPR_MTU3_TGID3 */ 0x57, // 132
        /* IPR_MTU3_TCIV3 */ 0x58, // 133
        /* IPR_MTU4_TGIA4 */ 0x59, // 134
        /* IPR_MTU4_TGIB4 */ 0x59, // 135
        /* IPR_MTU4_TGIC4 */ 0x59, // 136
        /* IPR_MTU4_TGID4 */ 0x59, // 137
        /* IPR_MTU4_TCIV4 */ 0x5A, // 138
        /* IPR_MTU5_TGIU5 */ 0x5B, // 139
        /* IPR_MTU5_TGIV5 */ 0x5B, // 140
        /* IPR_MTU5_TGIW5 */ 0x5B, // 141
        /* IPR_MTU6_TGIA6 */ 0x5C, // 142
        /* IPR_MTU6_TGIB6 */ 0x5C, // 143
        /* IPR_MTU6_TGIC6 */ 0x5C, // 144
        /* IPR_MTU6_TGID6 */ 0x5C, // 145
        /* IPR_MTU6_TCIV6 */ 0x5D, // 146
        /* IPR_MTU6_TGIE6 */ 0x5D, // 147
        /* IPR_MTU6_TGIF6 */ 0x5D, // 148
        /* IPR_MTU7_TGIA7 */ 0x5E, // 149
        /* IPR_MTU7_TGIB7 */ 0x5E, // 150
        /* IPR_MTU7_TCIV7 */ 0x5F, // 151
        /* IPR_MTU7_TCIU7 */ 0x5F, // 152
        /* IPR_MTU8_TGIA8 */ 0x60, // 153
        /* IPR_MTU8_TGIB8 */ 0x60, // 154
        /* IPR_MTU8_TCIV8 */ 0x61, // 155
        /* IPR_MTU8_TCIU8 */ 0x61, // 156
        /* IPR_MTU9_TGIA9 */ 0x62, // 157
        /* IPR_MTU9_TGIB9 */ 0x62, // 158
        /* IPR_MTU9_TGIC9 */ 0x62, // 159
        /* IPR_MTU9_TGID9 */ 0x62, // 160
        /* IPR_MTU9_TCIV9 */ 0x63, // 161
        /* IPR_MTU10_TGIA10 */ 0x64, // 162
        /* IPR_MTU10_TGIB10 */ 0x64, // 163
        /* IPR_MTU10_TGIC10 */ 0x64, // 164
        /* IPR_MTU10_TGID10 */ 0x64, // 165
        /* IPR_MTU10_TCIV10 */ 0x65, // 166
        /* IPR_MTU11_TGIU11 */ 0x66, // 167
        /* IPR_MTU11_TGIV11 */ 0x66, // 168
        /* IPR_MTU11_TGIW11 */ 0x66, // 169
        /* IPR_POE_OEI1 */ 0x67, // 170
        /* IPR_POE_OEI2 */ 0x67, // 171
        /* IPR_POE_OEI3 */ 0x67, // 172
        /* IPR_POE_OEI4 */ 0x67, // 173
        /* IPR_TMR0_CMIA0 */ 0x68, // 174
        /* IPR_TMR0_CMIB0 */ 0x68, // 175
        /* IPR_TMR0_OVI0 */ 0x68, // 176
        /* IPR_TMR1_CMIA1 */ 0x69, // 177
        /* IPR_TMR1_CMIB1 */ 0x69, // 178
        /* IPR_TMR1_OVI1 */ 0x69, // 179
        /* IPR_TMR2_CMIA2 */ 0x6A, // 180
        /* IPR_TMR2_CMIB2 */ 0x6A, // 181
        /* IPR_TMR2_OVI2 */ 0x6A, // 182
        /* IPR_TMR3_CMIA3 */ 0x6B, // 183
        /* IPR_TMR3_CMIB3 */ 0x6B, // 184
        /* IPR_TMR3_OVI3 */ 0x6B, // 185
        0, // 186
        0, // 187
        0, // 188
        0, // 189
        0, // 190
        0, // 191
        0, // 192
        0, // 193
        0, // 194
        0, // 195
        0, // 196
        0, // 197
        /* IPR_DMACA_DMAC0I */ 0x70, // 198
        /* IPR_DMACA_DMAC1I */ 0x71, // 199
        /* IPR_DMACA_DMAC2I */ 0x72, // 200
        /* IPR_DMACA_DMAC3I */ 0x73, // 201
        /* IPR_EXDMAC_EXDMAC0I */ 0x74, // 202
        /* IPR_EXDMAC_EXDMAC1I */ 0x75, // 203
        0, // 204
        0, // 205
        0, // 206
        0, // 207
        0, // 208
        0, // 209
        0, // 210
        0, // 211
        0, // 212
        0, // 213
        /* IPR_SCI0_ERI0 */ 0x80, // 214
        /* IPR_SCI0_RXI0 */ 0x80, // 215
        /* IPR_SCI0_TXI0 */ 0x80, // 216
        /* IPR_SCI0_TEI0 */ 0x80, // 217
        /* IPR_SCI1_ERI1 */ 0x81, // 218
        /* IPR_SCI1_RXI1 */ 0x81, // 219
        /* IPR_SCI1_TXI1 */ 0x81, // 220
        /* IPR_SCI1_TEI1 */ 0x81, // 221
        /* IPR_SCI2_ERI2 */ 0x82, // 222
        /* IPR_SCI2_RXI2 */ 0x82, // 223
        /* IPR_SCI2_TXI2 */ 0x82, // 224
        /* IPR_SCI2_TEI2 */ 0x82, // 225
        /* IPR_SCI3_ERI3 */ 0x83, // 226
        /* IPR_SCI3_RXI3 */ 0x83, // 227
        /* IPR_SCI3_TXI3 */ 0x83, // 228
        /* IPR_SCI3_TEI3 */ 0x83, // 229
        0, // 230
        0, // 231
        0, // 232
        0, // 233
        /* IPR_SCI5_ERI5 */ 0x85, // 234
        /* IPR_SCI5_RXI5 */ 0x85, // 235
        /* IPR_SCI5_TXI5 */ 0x85, // 236
        /* IPR_SCI5_TEI5 */ 0x85, // 237
        /* IPR_SCI6_ERI6 */ 0x86, // 238
        /* IPR_SCI6_RXI6 */ 0x86, // 239
        /* IPR_SCI6_TXI6 */ 0x86, // 240
        /* IPR_SCI6_TEI6 */ 0x86, // 241
        0, // 242
        0, // 243
        0, // 244
        0, // 245
        /* IPR_RIIC0_ICEEI0 */ 0x88, // 246
        /* IPR_RIIC0_ICRXI0 */ 0x89, // 247
        /* IPR_RIIC0_ICTXI0 */ 0x8A, // 248
        /* IPR_RIIC0_ICTEI0 */ 0x8B, // 249
        /* IPR_RIIC1_ICEEI1 */ 0x8C, // 250
        /* IPR_RIIC1_ICRXI1 */ 0x8D, // 251
        /* IPR_RIIC1_ICTXI1 */ 0x8E, // 252
        /* IPR_RIIC1_ICTEI1 */ 0x8F, // 253
        0, // 254
        0, // 255
};

/*---------------------------------------------------------------------------*
 * Routine:  InterruptFatalError
 *---------------------------------------------------------------------------*
 * Description:
 *      An interrupt has failed to process correctly.
 *---------------------------------------------------------------------------*/
void InterruptFatalError(void)
{
    // Disable all interrupts
    portDISABLE_INTERRUPTS();
    while(1)  {
        //TBD: Report error in some form
        wait();
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptsReset
 *---------------------------------------------------------------------------*
 * Description:
 *      Reset all registered interrupts
 *---------------------------------------------------------------------------*/
void InterruptsReset(void)
{
    TUInt32 channel;
    // Disable all interrupts
    portDISABLE_INTERRUPTS();

    // Reset the array
    memset(G_isrArray, 0, sizeof(G_isrArray));

    for (channel=0; channel<UEZ_MAX_IRQ_CHANNELS; channel++) {
        // Register ALL interrupts to go to InterruptFatalError so if they
        // are unexpectedly called, the system crashes nicely
        G_isrArray[channel].iISR = InterruptFatalError;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptRegister
 *---------------------------------------------------------------------------*
 * Description:
 *      Register an interrupt.
 * Inputs:
 *      TUInt32 aInterruptChannel -- Channel of interrupt to register
 *      TFPtr aISR                -- Pointer to interrupt service routine.
 *      T_irqPriority aPriority   -- Priority of interrupt.
 *      const char *aName         -- Name/descriptor of interrupt.
 *---------------------------------------------------------------------------*/
void InterruptRegister(
        TUInt32 aInterruptChannel,
        TFPtr aISR,
        T_irqPriority aPriority,
        const char * const aName)
{
    T_irqHandleStruct *p;

    // Reject attempts to allocate more ISRs than available
    if (aInterruptChannel >= UEZ_MAX_IRQ_CHANNELS)
        InterruptFatalError();

    // Create a new entry and return the handle
    p = G_isrArray+aInterruptChannel;
    // Interrupt already exists for this one!
    if (p->iISR != InterruptFatalError)
        InterruptFatalError();

    p->iISR = aISR;
    p->iName = aName;

    // Set the priority
	ICU.IPR[G_IPRTable[aInterruptChannel]].BIT.IPR  = 5-aPriority;
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptUnregister
 *---------------------------------------------------------------------------*
 * Description:
 *      The interrupt is no longer needed.  Disable and remove it.
 * Inputs:
 *      TUInt32 aInterruptChannel -- Channel of interrupt to unregister
 *---------------------------------------------------------------------------*/
void InterruptUnregister(TUInt32 aInterruptChannel)
{
    T_irqHandleStruct *p;

    if (aInterruptChannel >= UEZ_MAX_IRQ_CHANNELS)
        InterruptFatalError();

    // Ensure interrupt is turned off
    InterruptDisable(aInterruptChannel);

    // Look up the channel
    p = G_isrArray+aInterruptChannel;

    // Is there an interrupt here?
    if (p->iISR == InterruptFatalError)
        InterruptFatalError();

    // Mark it as freed
    p->iISR = InterruptFatalError;
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptEnable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable an existing interrupt
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to enable.
 *---------------------------------------------------------------------------*/
void InterruptEnable(T_irqChannel aChannel)
{
    ICU.IER[aChannel>>3].BYTE |= (1<<(aChannel & 7));
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptEnable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable an existing interrupt.
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to
 *                                   disable.
 *---------------------------------------------------------------------------*/
void InterruptDisable(T_irqChannel aChannel)
{
    ICU.IER[aChannel>>3].BYTE &= ~(1<<(aChannel & 7));
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptDisableAllRegistered
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable all interrupts that were registered with this system
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to
 *                                   disable.
 *---------------------------------------------------------------------------*/
void InterruptDisableAllRegistered(void)
{
    // Does not apply here for the RX chips, have to be handled
    // specifically on a per peripheral basis
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptEnableAllRegistered
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable all interrupts that were registered with this system
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to
 *                                   disable.
 *---------------------------------------------------------------------------*/
void InterruptEnableAllRegistered(void)
{
    // Does not apply here for the RX chips, have to be handled
    // specifically on a per peripheral basis
}

/*---------------------------------------------------------------------------*
* Routine:  InterruptIsRegistered
*---------------------------------------------------------------------------*
* Description:
*      Check if an interrupt is registered.
* Inputs:
*      TUInt32 aInterruptChannel -- Channel of interrupt to register
*
* Outputs:
*      TBool                  -- Registered ETrue else EFalse
*---------------------------------------------------------------------------*/
TBool InterruptIsRegistered(TUInt32 aInterruptChannel)
{
    T_irqHandleStruct *p;

    // Reject attempts to access out of array data.
    if (aInterruptChannel <= UEZ_MAX_IRQ_CHANNELS) {
        // Create a pointer to the the interrupt handler.
        p = G_isrArray + aInterruptChannel;

        // Interrupt already exists for this one.
        if (p->iISR != InterruptFatalError) {
            return ETrue;
        }
    }

    return EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_IRQHandler
 *---------------------------------------------------------------------------*
 * Description:
 *      All IRQs are trapped and come here so that interrupts can be
 *      remapped to other functions.  If no function mapping has been
 *      declared, then this causes a fatal error.
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to
 *                                   disable.
 *---------------------------------------------------------------------------*/
void RX62N_IRQHandler(unsigned char aNumber)
{
    T_irqHandleStruct *p =  G_isrArray+aNumber;
    if (p->iISR) {
        // Call the interrupt handler
        p->iISR();
    } else {
  		UEZBSP_FatalError(1);
    }
}


/*-------------------------------------------------------------------------*
 * End of File:  Interrupt.c
 *-------------------------------------------------------------------------*/
