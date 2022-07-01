/*-------------------------------------------------------------------------*
 * File:  Interrupt.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Interrupt control for LPC2xxx.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>

#include <string.h>
#include <HAL/Interrupt.h>
#include <uEZProcessor.h>
#if ( RTOS == SafeRTOS )
#include <SafeRTOSConfig.h>
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iInterruptChannel;
    TISRFPtr iISR;
#if LPC43xx_INTERRUPT_TRACK_NAMES
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
    for (i=DAC_IRQn; i<=QEI_IRQn; i++) {
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
#if LPC43xx_INTERRUPT_TRACK_NAMES
    p->iName = aName;
#endif

    // Set the interrupt priority.
#if (RTOS == SafeRTOS)
	/* <<< WHIS >>> Assignment of interrupt levels is critical to the correct 
	operation of UEZ and SafeRTOS. Interrupt.h defines levels between 0-15, 
	whereas the LPC43xx has 32 interrupt levels. However all UEZ interrupts
	use the semaphore release and therefore need to be greater than 
	configSYSTEM_INTERRUPT_PRIORITY_LVL. configSYSTEM_INTERRUPT_PRIORITY_LVL is
	defined as 16 so we can just add the uEZ priority to the RTOS threshold
	to arrive a legal priority between 16 and 31. */
    NVIC_SetPriority((IRQn_Type)aInterruptChannel, configSYSTEM_INTERRUPT_PRIORITY_LVL + aPriority);
#else
    NVIC_SetPriority((IRQn_Type)aInterruptChannel, 2+aPriority);
#endif	
    //NVIC_EnableIRQ((IRQn_Type)aInterruptChannel);
}

#define DEFAULT_IRQ_HANDLER(func, channel)  \
    void func(void) { (*G_isrArray[channel].iISR)(); }

DEFAULT_IRQ_HANDLER(DAC_IRQHandler , DAC_IRQn);
DEFAULT_IRQ_HANDLER(M0CORE_IRQHandler , M0APP_IRQn);
#if !TOUCHGFX
DEFAULT_IRQ_HANDLER(DMA_IRQHandler , DMA_IRQn);//Disable for Touch GFX
#endif
DEFAULT_IRQ_HANDLER(ETH_IRQHandler , ETHERNET_IRQn); //commented out to deal with current state of code (LPCOpen)
//DEFAULT_IRQ_HANDLER(SDIO_IRQHandler , SDIO_IRQn); //commented out to deal with current state of code (LPCOpen)
DEFAULT_IRQ_HANDLER(SDIO_IRQHandler , SDIO_IRQn); // Put back in!
#if !TOUCHGFX
DEFAULT_IRQ_HANDLER(LCD_IRQHandler , LCD_IRQn); //Disable for Touch GFX
#endif
DEFAULT_IRQ_HANDLER(USB0_IRQHandler , USB0_IRQn);
DEFAULT_IRQ_HANDLER(USB1_IRQHandler , USB1_IRQn);
DEFAULT_IRQ_HANDLER(SCT_IRQHandler , SCT_IRQn);
DEFAULT_IRQ_HANDLER(RITIMER_IRQHandler , RITIMER_IRQn);
DEFAULT_IRQ_HANDLER(TIMER0_IRQHandler , TIMER0_IRQn);
DEFAULT_IRQ_HANDLER(TIMER1_IRQHandler , TIMER1_IRQn);
DEFAULT_IRQ_HANDLER(TIMER2_IRQHandler , TIMER2_IRQn);
DEFAULT_IRQ_HANDLER(TIMER3_IRQHandler , TIMER3_IRQn);
DEFAULT_IRQ_HANDLER(MCPWM_IRQHandler , MCPWM_IRQn);
DEFAULT_IRQ_HANDLER(ADC0_IRQHandler , ADC0_IRQn);
DEFAULT_IRQ_HANDLER(ADC1_IRQHandler , ADC1_IRQn);
DEFAULT_IRQ_HANDLER(I2C0_IRQHandler , I2C0_IRQn);
DEFAULT_IRQ_HANDLER(I2C1_IRQHandler , I2C1_IRQn);
DEFAULT_IRQ_HANDLER(SPI_IRQHandler , SPI_IRQn);
DEFAULT_IRQ_HANDLER(SSP0_IRQHandler , SSP0_IRQn);
DEFAULT_IRQ_HANDLER(SSP1_IRQHandler , SSP1_IRQn);
DEFAULT_IRQ_HANDLER(USART0_IRQHandler , USART0_IRQn);
DEFAULT_IRQ_HANDLER(UART1_IRQHandler , UART1_IRQn);
DEFAULT_IRQ_HANDLER(UART2_IRQHandler , USART2_IRQn);
DEFAULT_IRQ_HANDLER(UART3_IRQHandler , USART3_IRQn);
DEFAULT_IRQ_HANDLER(I2S0_IRQHandler , I2S0_IRQn);
DEFAULT_IRQ_HANDLER(I2S1_IRQHandler , I2S1_IRQn);
DEFAULT_IRQ_HANDLER(SGPIO_IRQHandler , SGPIO_IRQn);
DEFAULT_IRQ_HANDLER(GPIO0_IRQHandler , PIN_INT0_IRQn);
DEFAULT_IRQ_HANDLER(GPIO1_IRQHandler , PIN_INT1_IRQn);
DEFAULT_IRQ_HANDLER(GPIO2_IRQHandler , PIN_INT2_IRQn);
DEFAULT_IRQ_HANDLER(GPIO3_IRQHandler , PIN_INT3_IRQn);
DEFAULT_IRQ_HANDLER(GPIO4_IRQHandler , PIN_INT4_IRQn);
DEFAULT_IRQ_HANDLER(GPIO5_IRQHandler , PIN_INT5_IRQn);
DEFAULT_IRQ_HANDLER(GPIO6_IRQHandler , PIN_INT6_IRQn);
DEFAULT_IRQ_HANDLER(GPIO7_IRQHandler , PIN_INT7_IRQn);
DEFAULT_IRQ_HANDLER(GINT0_IRQHandler , GINT0_IRQn);
DEFAULT_IRQ_HANDLER(GINT1_IRQHandler , GINT1_IRQn);
DEFAULT_IRQ_HANDLER(EVENTROUTER_IRQHandler , EVENTROUTER_IRQn);
DEFAULT_IRQ_HANDLER(C_CAN1_IRQHandler , C_CAN1_IRQn);
DEFAULT_IRQ_HANDLER(ATIMER_IRQHandler , ATIMER_IRQn);
DEFAULT_IRQ_HANDLER(RTC_IRQHandler , RTC_IRQn);
DEFAULT_IRQ_HANDLER(WWDT_IRQHandler , WWDT_IRQn);
DEFAULT_IRQ_HANDLER(C_CAN0_IRQHandler , C_CAN0_IRQn);
DEFAULT_IRQ_HANDLER(QEI_IRQHandler , QEI_IRQn);

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

