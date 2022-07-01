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
#if LPC546xx_INTERRUPT_TRACK_NAMES
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
    __disable_interrupt();
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
    for (i=WDT_BOD_IRQn; i<=SMARTCARD1_IRQn; i++) {
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
#if LPC546xx_INTERRUPT_TRACK_NAMES
    p->iName = aName;
#endif

    // Set the interrupt priority.
#if (RTOS == SafeRTOS)
	/* <<< WHIS >>> Assignment of interrupt levels is critical to the correct 
	operation of UEZ and SafeRTOS. Interrupt.h defines levels between 0-15, 
	whereas the LPC546xx has 32 interrupt levels. However all UEZ interrupts
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

DEFAULT_IRQ_HANDLER(WDT_BOD_IRQHandler,WDT_BOD_IRQn);
DEFAULT_IRQ_HANDLER(DMA0_IRQHandler,DMA0_IRQn);
DEFAULT_IRQ_HANDLER(GINT0_IRQHandler,GINT0_IRQn);
DEFAULT_IRQ_HANDLER(GINT1_IRQHandler,GINT1_IRQn);
DEFAULT_IRQ_HANDLER(PIN_INT0_IRQHandler,PIN_INT0_IRQn);
DEFAULT_IRQ_HANDLER(PIN_INT1_IRQHandler,PIN_INT1_IRQn);
DEFAULT_IRQ_HANDLER(PIN_INT2_IRQHandler,PIN_INT2_IRQn);
DEFAULT_IRQ_HANDLER(PIN_INT3_IRQHandler,PIN_INT3_IRQn);
DEFAULT_IRQ_HANDLER(UTICK0_IRQHandler,UTICK0_IRQn);
DEFAULT_IRQ_HANDLER(MRT0_IRQHandler,MRT0_IRQn);
DEFAULT_IRQ_HANDLER(CTIMER0_IRQHandler,CTIMER0_IRQn);
DEFAULT_IRQ_HANDLER(CTIMER1_IRQHandler,CTIMER1_IRQn);
DEFAULT_IRQ_HANDLER(SCT0_IRQHandler,SCT0_IRQn);
DEFAULT_IRQ_HANDLER(CTIMER3_IRQHandler,CTIMER3_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM0_IRQHandler,FLEXCOMM0_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM1_IRQHandler,FLEXCOMM1_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM2_IRQHandler,FLEXCOMM2_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM3_IRQHandler,FLEXCOMM3_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM4_IRQHandler,FLEXCOMM4_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM5_IRQHandler,FLEXCOMM5_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM6_IRQHandler,FLEXCOMM6_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM7_IRQHandler,FLEXCOMM7_IRQn);
DEFAULT_IRQ_HANDLER(ADC0_SEQA_IRQHandler,ADC0_SEQA_IRQn);
DEFAULT_IRQ_HANDLER(ADC0_SEQB_IRQHandler,ADC0_SEQB_IRQn);
DEFAULT_IRQ_HANDLER(ADC0_THCMP_IRQHandler,ADC0_THCMP_IRQn);
DEFAULT_IRQ_HANDLER(DMIC0_IRQHandler,DMIC0_IRQn);
DEFAULT_IRQ_HANDLER(HWVAD0_IRQHandler,HWVAD0_IRQn);
DEFAULT_IRQ_HANDLER(USB0_NEEDCLK_IRQHandler,USB0_NEEDCLK_IRQn)
DEFAULT_IRQ_HANDLER(USB0_IRQHandler,USB0_IRQn);
DEFAULT_IRQ_HANDLER(RTC_IRQHandler,RTC_IRQn);
DEFAULT_IRQ_HANDLER(Reserved46_IRQHandler,Reserved46_IRQn);
DEFAULT_IRQ_HANDLER(Reserved47_IRQHandler,Reserved47_IRQn);
DEFAULT_IRQ_HANDLER(PIN_INT4_IRQHandler,PIN_INT4_IRQn);
DEFAULT_IRQ_HANDLER(PIN_INT5_IRQHandler,PIN_INT5_IRQn);
DEFAULT_IRQ_HANDLER(PIN_INT6_IRQHandler,PIN_INT6_IRQn);
DEFAULT_IRQ_HANDLER(PIN_INT7_IRQHandler,PIN_INT7_IRQn);
DEFAULT_IRQ_HANDLER(CTIMER2_IRQHandler,CTIMER2_IRQn);
DEFAULT_IRQ_HANDLER(CTIMER4_IRQHandler,CTIMER4_IRQn);
DEFAULT_IRQ_HANDLER(RIT_IRQHandler,RIT_IRQn);
DEFAULT_IRQ_HANDLER(SPIFI0_IRQHandler,SPIFI0_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM8_IRQHandler,FLEXCOMM8_IRQn);
DEFAULT_IRQ_HANDLER(FLEXCOMM9_IRQHandler,FLEXCOMM9_IRQn);
DEFAULT_IRQ_HANDLER(SDIO_IRQHandler,SDIO_IRQn);
DEFAULT_IRQ_HANDLER(CAN0_IRQ0_IRQHandler,CAN0_IRQ0_IRQn);
DEFAULT_IRQ_HANDLER(CAN0_IRQ1_IRQHandler,CAN0_IRQ1_IRQn);
DEFAULT_IRQ_HANDLER(CAN1_IRQ0_IRQHandler,CAN1_IRQ0_IRQn);
DEFAULT_IRQ_HANDLER(CAN1_IRQ1_IRQHandler,CAN1_IRQ1_IRQn);
DEFAULT_IRQ_HANDLER(USB1_IRQHandler,USB1_IRQn);
DEFAULT_IRQ_HANDLER(USB1_NEEDCLK_IRQHandler,USB1_NEEDCLK_IRQn)
DEFAULT_IRQ_HANDLER(ETHERNET_IRQHandler,ETHERNET_IRQn);
DEFAULT_IRQ_HANDLER(ETHERNET_PMT_IRQHandler,ETHERNET_PMT_IRQn);
DEFAULT_IRQ_HANDLER(ETHERNET_MACLP_IRQHandler,ETHERNET_MACLP_IRQn);
DEFAULT_IRQ_HANDLER(EEPROM_IRQHandler,EEPROM_IRQn);
DEFAULT_IRQ_HANDLER(LCD_IRQHandler,LCD_IRQn);
DEFAULT_IRQ_HANDLER(SHA_IRQHandler,SHA_IRQn);
DEFAULT_IRQ_HANDLER(SMARTCARD0_IRQHandler,SMARTCARD0_IRQn);
DEFAULT_IRQ_HANDLER(SMARTCARD1_IRQHandler,SMARTCARD1_IRQn);

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

