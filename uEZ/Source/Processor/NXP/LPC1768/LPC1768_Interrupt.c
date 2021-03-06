/*-------------------------------------------------------------------------*
 * File:  Interrupt.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Interrupt control for LPC2xxx.
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
#include <Config.h>
#include <string.h>
#include <HAL/Interrupt.h>
#include <CMSIS/LPC17xx.h>
#include "uEZProcessor_LPC1768.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iInterruptChannel;
    TISRFPtr iISR;
#if LPC2478_INTERRUPT_TRACK_NAMES
    const char *iName;
#endif
} T_irqHandleStruct;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_irqHandleStruct G_isrArray[UEZ_MAX_IRQ_CHANNELS];

/*---------------------------------------------------------------------------*
 * Routine:  InterruptFatalError
 *---------------------------------------------------------------------------*
 * Description:
 *      An interrupt has failed to process correctly.
 *---------------------------------------------------------------------------*/
void InterruptFatalError(void)
{
    // Disable all interrupts
    __disable_irq();
    while(1)  {
        //TBD: Report error in some form
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
    IRQn_Type i;

    // Disable all interrupts
    for (i=WDT_IRQn; i<=QEI_IRQn; i++) {
        NVIC_DisableIRQ(i);
    }
    // Reset the array
    memset(G_isrArray, 0, sizeof(G_isrArray));
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
        TISRFPtr aISR,
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
    if (p->iISR != 0)
        InterruptFatalError();

    p->iInterruptChannel = aInterruptChannel;
    p->iISR = aISR;
#if LPC1768_INTERRUPT_TRACK_NAMES
    p->iName = aName;
#endif

    // Set the interrupt priority.
    NVIC_SetPriority((IRQn_Type)aInterruptChannel, 6+aPriority);
    //NVIC_EnableIRQ((IRQn_Type)aInterruptChannel);
}

#define DEFAULT_IRQ_HANDLER(func, channel)  \
    void func(void) { (*G_isrArray[channel].iISR)(); }

DEFAULT_IRQ_HANDLER(WDT_IRQHandler, WDT_IRQn);
DEFAULT_IRQ_HANDLER(TIMER0_IRQHandler, TIMER0_IRQn);
DEFAULT_IRQ_HANDLER(TIMER1_IRQHandler, TIMER1_IRQn);
DEFAULT_IRQ_HANDLER(TIMER2_IRQHandler, TIMER2_IRQn);
DEFAULT_IRQ_HANDLER(TIMER3_IRQHandler, TIMER3_IRQn);
DEFAULT_IRQ_HANDLER(UART0_IRQHandler, UART0_IRQn);
DEFAULT_IRQ_HANDLER(UART1_IRQHandler, UART1_IRQn);
DEFAULT_IRQ_HANDLER(UART2_IRQHandler, UART2_IRQn);
DEFAULT_IRQ_HANDLER(UART3_IRQHandler, UART3_IRQn);
DEFAULT_IRQ_HANDLER(PWM1_IRQHandler, PWM1_IRQn);
DEFAULT_IRQ_HANDLER(I2C0_IRQHandler, I2C0_IRQn);
DEFAULT_IRQ_HANDLER(I2C1_IRQHandler, I2C1_IRQn);
DEFAULT_IRQ_HANDLER(I2C2_IRQHandler, I2C2_IRQn);
DEFAULT_IRQ_HANDLER(SPI_IRQHandler, SPI_IRQn);
DEFAULT_IRQ_HANDLER(SSP0_IRQHandler, SSP0_IRQn);
DEFAULT_IRQ_HANDLER(SSP1_IRQHandler, SSP1_IRQn);
DEFAULT_IRQ_HANDLER(PLL0_IRQHandler, PLL0_IRQn);
DEFAULT_IRQ_HANDLER(RTC_IRQHandler, RTC_IRQn);
DEFAULT_IRQ_HANDLER(EINT0_IRQHandler, EINT0_IRQn);
DEFAULT_IRQ_HANDLER(EINT1_IRQHandler, EINT1_IRQn);
DEFAULT_IRQ_HANDLER(EINT2_IRQHandler, EINT2_IRQn);
DEFAULT_IRQ_HANDLER(EINT3_IRQHandler, EINT3_IRQn);
DEFAULT_IRQ_HANDLER(ADC_IRQHandler, ADC_IRQn);
DEFAULT_IRQ_HANDLER(BOD_IRQHandler, BOD_IRQn);
DEFAULT_IRQ_HANDLER(USB_IRQHandler, USB_IRQn);
DEFAULT_IRQ_HANDLER(CAN_IRQHandler, CAN_IRQn);
DEFAULT_IRQ_HANDLER(GPDMA_IRQHandler, DMA_IRQn);
DEFAULT_IRQ_HANDLER(I2S_IRQHandler, I2S_IRQn);
DEFAULT_IRQ_HANDLER(ENET_IRQHandler, ENET_IRQn);
DEFAULT_IRQ_HANDLER(RIT_IRQHandler, RIT_IRQn);
DEFAULT_IRQ_HANDLER(MCPWM_IRQHandler, MCPWM_IRQn);
DEFAULT_IRQ_HANDLER(QEI_IRQHandler, QEI_IRQn);
DEFAULT_IRQ_HANDLER(PLL1_IRQHandler, PLL1_IRQn);
DEFAULT_IRQ_HANDLER(USBACT_IRQHandler, USBACT_IRQn);
DEFAULT_IRQ_HANDLER(CANACT_IRQHandler, CANACT_IRQn);

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

    // Look up the channel
    p = G_isrArray+aInterruptChannel;

    // Is there an interrupt here?
    if (p->iISR == 0)
        InterruptFatalError();

    NVIC_DisableIRQ((IRQn_Type)aInterruptChannel);

    // Mark it as freed
    p->iISR = 0;
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
    if (aChannel >= UEZ_MAX_IRQ_CHANNELS)
        InterruptFatalError();

    NVIC_EnableIRQ((IRQn_Type)aChannel);
}

/*---------------------------------------------------------------------------*
 * Routine:  InterruptDisable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable an existing interrupt.
 * Inputs:
 *      T_irqChannel aChannel       -- Handle of registered interrupt to
 *                                   disable.
 *---------------------------------------------------------------------------*/
void InterruptDisable(T_irqChannel aChannel)
{
    if (aChannel >= UEZ_MAX_IRQ_CHANNELS)
        InterruptFatalError();

    NVIC_DisableIRQ((IRQn_Type)aChannel);
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
    TUInt32 channel;
    T_irqHandleStruct *p;

    for (channel=0; channel<UEZ_MAX_IRQ_CHANNELS; channel++)  {
        p = G_isrArray+channel;
        if (p->iISR)
            InterruptDisable((T_irqChannel)channel);
    }
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
    TUInt32 channel;
    T_irqHandleStruct *p;

    for (channel=0; channel<UEZ_MAX_IRQ_CHANNELS; channel++)  {
        p = G_isrArray+channel;
        if (p->iISR)
            InterruptEnable((T_irqChannel)channel);
    }
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
    if (aInterruptChannel <= UEZ_MAX_IRQ_CHANNELS)
    {
        // Create a pointer to the the interrupt handler.
        p = G_isrArray+aInterruptChannel;

        // Interrupt already exists for this one.
        if (p->iISR != 0)
        {
            return ETrue;
        }
    }

    return EFalse;
}

/*-------------------------------------------------------------------------*
 * End of File:  Interrupt.c
 *-------------------------------------------------------------------------*/

