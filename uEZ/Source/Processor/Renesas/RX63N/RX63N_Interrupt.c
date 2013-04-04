/*-------------------------------------------------------------------------*
 * File:  RX63N_Interrupt.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Interrupt control for Renesas H8SX 1668RF.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
        /* IPR_BSC_BUSERR */ 0, // 16
        0, // 17
        0, // 18
        0, // 19
        0, // 20
        /* IPR_FCU_FIFERR */ 1, // 21
        0, // 22
        /* IPR_FCU_FRDYI */ 2, // 23
        0, // 24
        0, // 25
        0, // 26
        /* IPR_ICU_SWINT */ 3, // 27
        /* IPR_CMT0_CMI0 */ 4, // 28
        /* IPR_CMT1_CMI1 */ 5, // 29
        /* IPR_CMT2_CMI2 */ 6, // 30
        /* IPR_CMT3_CMI3 */ 7, // 31
        /* IPR_ETHER_EINT */ 32, // 32
        /* IPR_USB0_D0FIFO0 */ 33, // 33
        /* IPR_USB0_D1FIFO0 */ 34, // 34
        /* IPR_USB0_USBI0 */ 35, // 35
        /* IPR_USB1_D0FIFO1 */ 36, // 36
        /* IPR_USB1_D1FIFO1 */ 37, // 37
        /* IPR_USB1_USBI1 */ 38, // 38
        /* IPR_RSPI0_SPRI0 */ 39, // 39
        /* IPR_RSPI0_SPTI0 */ 39, // 40
        /* IPR_RSPI0_SPII0 */ 39, // 41
        /* IPR_RSPI1_SPRI1 */ 42, // 42
        /* IPR_RSPI1_SPTI1 */ 42, // 43
        /* IPR_RSPI1_SPII1 */ 42, // 44
        /* IPR_RSPI2_SPRI2 */ 45, // 45
        /* IPR_RSPI2_SPTI2 */ 45, // 46
        /* IPR_RSPI2_SPII2 */ 45, // 47
        /* IPR_CAN0_RXF0 */ 48, // 48
        /* IPR_CAN0_TXF0 */ 48, // 49
        /* IPR_CAN0_RXM0 */ 48, // 50
        /* IPR_CAN0_TXM0 */ 48, // 51
        /* IPR_CAN1_RXF1 */ 52, // 52
        /* IPR_CAN1_TXF1 */ 52, // 53
        /* IPR_CAN1_RXM1 */ 52, // 54
        /* IPR_CAN1_TXM1 */ 52, // 55
        /* IPR_CAN2_RXF2 */ 56, // 56
        /* IPR_CAN2_TXF2 */ 56, // 57
        /* IPR_CAN2_RXM2 */ 56, // 58
        /* IPR_CAN2_TXM2 */ 56, // 59
        0, // 60
        0, // 61
        /* IPR_RTC_CUP */ 62, // 62
        0, // 63
        /* IPR_ICU_IRQ0 */ 64, // 64
        /* IPR_ICU_IRQ1 */ 65, // 65
        /* IPR_ICU_IRQ2 */ 66, // 66
        /* IPR_ICU_IRQ3 */ 67, // 67
        /* IPR_ICU_IRQ4 */ 68, // 68
        /* IPR_ICU_IRQ5 */ 69, // 69
        /* IPR_ICU_IRQ6 */ 70, // 70
        /* IPR_ICU_IRQ7 */ 71, // 71
        /* IPR_ICU_IRQ8 */ 72, // 72
        /* IPR_ICU_IRQ9 */ 73, // 73
        /* IPR_ICU_IRQ10 */ 74, // 74
        /* IPR_ICU_IRQ11 */ 75, // 75
        /* IPR_ICU_IRQ12 */ 76, // 76
        /* IPR_ICU_IRQ13 */ 77, // 77
        /* IPR_ICU_IRQ14 */ 78, // 78
        /* IPR_ICU_IRQ15 */ 79, // 79
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
        /* IPR_USB_USBR0 */ 90, // 90
        /* IPR_USB_USBR1 */ 91, // 91
        /* IPR_RTC_ALM */ 92, // 92
        /* IPR_RTC_PRD */ 93, // 93
        0, // 94
        0, // 95
        0, // 96
        0, // 97
        /* IPR_AD0_ADI0 */ 98, // 98
        0, // 99
        0, // 100
        0, // 101
        /* IPR_S12AD0_S12ADI0 */ 102, // 102
        0, // 103
        0, // 104
        0, // 105
        /* IPR_ICU_GROUPE0 */ 106, // 106
        /* IPR_ICU_GROUPE1 */ 107, // 107
        /* IPR_ICU_GROUPE2 */ 108, // 108
        /* IPR_ICU_GROUPE3 */ 109, // 109
        /* IPR_ICU_GROUPE4 */ 110, // 110
        /* IPR_ICU_GROUPE5 */ 111, // 111
        /* IPR_ICU_GROUPE6 */ 112, // 112
        0, // 113
        /* IPR_ICU_GROUPL0 */ 114, // 114
        0, // 115
        0, // 116
        0, // 117
        0, // 118
        0, // 119
        0, // 120
        0, // 121
        /* IPR_SCIX_SCIX0 */ 122, // 122
        /* IPR_SCIX_SCIX1 */ 122, // 123
        /* IPR_SCIX_SCIX2 */ 122, // 124
        /* IPR_SCIX_SCIX3 */ 122, // 125
        /* IPR_TPU0_TGI0A */ 126, // 126
        /* IPR_TPU0_TGI0B */ 126, // 127
        /* IPR_TPU0_TGI0C */ 126, // 128
        /* IPR_TPU0_TGI0D */ 126, // 129
        /* IPR_TPU1_TGI1A */ 130, // 130
        /* IPR_TPU1_TGI1B */ 130, // 131
        /* IPR_TPU2_TGI2A */ 132, // 132
        /* IPR_TPU2_TGI2B */ 132, // 133
        /* IPR_TPU3_TGI3A */ 134, // 134
        /* IPR_TPU3_TGI3B */ 134, // 135
        /* IPR_TPU3_TGI3C */ 134, // 136
        /* IPR_TPU3_TGI3D */ 134, // 137
        /* IPR_TPU4_TGI4A */ 138, // 138
        /* IPR_TPU4_TGI4B */ 138, // 139
        /* IPR_TPU5_TGI5A */ 140, // 140
        /* IPR_TPU5_TGI5B */ 140, // 141
        /* IPR_TPU6_TGI6A / IPR_MTU0_TGIA0 */ 142, // 142
        /* IPR_TPU6_TGI6B / IPR_MTU0_TGIB0 */ 142, // 143
        /* IPR_TPU6_TGI6C / IPR_MTU0_TGIC0 */ 142, // 144
        /* IPR_TPU6_TGI6D / IPR_MTU0_TGID0 */ 142, // 145
        /* IPR_MTU0_TGIE0 */ 146, // 146
        /* IPR_MTU0_TGIF0 */ 146, // 147
        /* IPR_TPU7_TGI7A / IPR_MTU1_TGIA1 */ 148, // 148
        /* IPR_TPU7_TGI7B / IPR_MTU1_TGIB1 */ 148, // 149
        /* IPR_TPU8_TGI8A / IPR_MTU2_TGIA2 */ 150, // 150
        /* IPR_TPU8_TGI8B / IPR_MTU2_TGIB2 */ 150, // 151
        /* IPR_TPU9_TGI9A / IPR_MTU3_TGIA3 */ 152, // 152
        /* IPR_TPU9_TGI9B / IPR_MTU3_TGIB3 */ 152, // 153
        /* IPR_TPU9_TGI9C / IPR_MTU3_TGIC3 */ 154, // 154
        /* IPR_TPU9_TGI9D / IPR_MTU3_TGID3 */ 154, // 155
        /* IPR_TPU10_TGI10A / IPR_MTU4_TGIA4 */ 156, // 156
        /* IPR_TPU10_TGI10B / IPR_MTU4_TGIB4 */ 156, // 157
        /* IPR_MTU4_TGIC4 */ 156, // 158
        /* IPR_MTU4_TGID4 */ 156, // 159
        /* IPR_MTU4_TCIV4 */ 160, // 160
        /* IPR_MTU5_TGIU5 */ 161, // 161
        /* IPR_MTU5_TGIV5 */ 161, // 162
        /* IPR_MTU5_TGIW5 */ 161, // 163
        /* IPR_TPU11_TGI11A */ 164, // 164
        /* IPR_TPU11_TGI11B */ 164, // 165
        /* IPR_POE_OEI1 */ 166, // 166
        /* IPR_POE_OEI2 */ 166, // 167
        0, // 168
        0, // 169
        /* IPR_TMR0_CMIA0 */ 170, // 170
        /* IPR_TMR0_CMIB0 */ 170, // 171
        /* IPR_TMR0_OVI0 */ 170, // 172
        /* IPR_TMR1_CMIA1 */ 173, // 173
        /* IPR_TMR1_CMIB1 */ 173, // 174
        /* IPR_TMR1_OVI1 */ 173, // 175
        /* IPR_TMR2_CMIA2 */ 176, // 176
        /* IPR_TMR2_CMIB2 */ 176, // 177
        /* IPR_TMR2_OVI2 */ 176, // 178
        /* IPR_TMR3_CMIA3 */ 179, // 179
        /* IPR_TMR3_CMIB3 */ 179, // 180
        /* IPR_TMR3_OVI3 */ 179, // 181
        /* IPR_RIIC0_EEI0 */ 182, // 182
        /* IPR_RIIC0_RXI0 */ 183, // 183
        /* IPR_RIIC0_TXI0 */ 184, // 184
        /* IPR_RIIC0_TEI0 */ 185, // 185
        /* IPR_RIIC1_EEI1 */ 186, // 186
        /* IPR_RIIC1_RXI1 */ 187, // 187
        /* IPR_RIIC1_TXI1 */ 188, // 188
        /* IPR_RIIC1_TEI1 */ 189, // 189
        /* IPR_RIIC2_EEI2 */ 190, // 190
        /* IPR_RIIC2_RXI2 */ 191, // 191
        /* IPR_RIIC2_TXI2 */ 192, // 192
        /* IPR_RIIC2_TEI2 */ 193, // 193
        /* IPR_RIIC3_EEI3 */ 194, // 194
        /* IPR_RIIC3_RXI3 */ 195, // 195
        /* IPR_RIIC3_TXI3 */ 196, // 196
        /* IPR_RIIC3_TEI3 */ 197, // 197
        /* IPR_DMAC_DMAC0I */ 198, // 198
        /* IPR_DMAC_DMAC1I */ 199, // 199
        /* IPR_DMAC_DMAC2I */ 200, // 200
        /* IPR_DMAC_DMAC3I */ 201, // 201
        /* IPR_EXDMAC_EXDMAC0I */ 202, // 202
        /* IPR_EXDMAC_EXDMAC1I */ 203, // 203
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
        /* IPR_SCI0_RXI0 */ 214, // 214
        /* IPR_SCI0_TXI0 */ 214, // 215
        /* IPR_SCI0_TEI0 */ 214, // 216
        /* IPR_SCI1_RXI1 */ 217, // 217
        /* IPR_SCI1_TXI1 */ 217, // 218
        /* IPR_SCI1_TEI1 */ 217, // 219
        /* IPR_SCI2_RXI2 */ 220, // 220
        /* IPR_SCI2_TXI2 */ 220, // 221
        /* IPR_SCI2_TEI2 */ 220, // 222
        /* IPR_SCI3_RXI3 */ 223, // 223
        /* IPR_SCI3_TXI3 */ 223, // 224
        /* IPR_SCI3_TEI3 */ 223, // 225
        /* IPR_SCI4_RXI4 */ 226, // 226
        /* IPR_SCI4_TXI4 */ 226, // 227
        /* IPR_SCI4_TEI4 */ 226, // 228
        /* IPR_SCI5_RXI5 */ 229, // 229
        /* IPR_SCI5_TXI5 */ 229, // 230
        /* IPR_SCI5_TEI5 */ 229, // 231
        /* IPR_SCI6_RXI6 */ 232, // 232
        /* IPR_SCI6_TXI6 */ 232, // 233
        /* IPR_SCI6_TEI6 */ 232, // 234
        /* IPR_SCI7_RXI7 */ 235, // 235
        /* IPR_SCI7_TXI7 */ 235, // 236
        /* IPR_SCI7_TEI7 */ 235, // 237
        /* IPR_SCI8_RXI8 */ 238, // 238
        /* IPR_SCI8_TXI8 */ 238, // 239
        /* IPR_SCI8_TEI8 */ 238, // 240
        /* IPR_SCI9_RXI9 */ 241, // 241
        /* IPR_SCI9_TXI9 */ 241, // 242
        /* IPR_SCI9_TEI9 */ 241, // 243
        /* IPR_SCI10_RXI10 */ 244, // 244
        /* IPR_SCI10_TXI10 */ 244, // 245
        /* IPR_SCI10_TEI10 */ 244, // 246
        /* IPR_SCI11_RXI11 */ 247, // 247
        /* IPR_SCI11_TXI11 */ 247, // 248
        /* IPR_SCI11_TEI11 */ 247, // 249
        /* IPR_SCI12_RXI12 */ 250, // 250
        /* IPR_SCI12_TXI12 */ 250, // 251
        /* IPR_SCI12_TEI12 */ 250, // 252
        /* IPR_IEB_IEBINT */ 253, // 253
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
	ICU.IPR[G_IPRTable[aInterruptChannel]].BIT.IPR  = 15-aPriority;
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
 * Routine:  RX63N_IRQHandler
 *---------------------------------------------------------------------------*
 * Description:
 *      All IRQs are trapped and come here so that interrupts can be
 *      remapped to other functions.  If no function mapping has been
 *      declared, then this causes a fatal error.
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to
 *                                   disable.
 *---------------------------------------------------------------------------*/
void RX63N_IRQHandler(unsigned char aNumber)
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
