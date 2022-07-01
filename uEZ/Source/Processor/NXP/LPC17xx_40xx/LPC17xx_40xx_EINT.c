/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_EINT.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/EINT.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_EINT.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const HAL_EINT *iHAL;
    EINT_Callback aCallback;
    void* aCallbackWorkspace;
    TUInt32 iBitOffset;
    T_irqChannel iInterruptChannel;
    TBool iConfigured;
}T_LPC17xx_40xx_EINT_Workspace;


T_LPC17xx_40xx_EINT_Workspace *G_EINT_EINT0_Workspace;
T_LPC17xx_40xx_EINT_Workspace *G_EINT_EINT1_Workspace;
/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static void IENITProcessInterrupt(T_LPC17xx_40xx_EINT_Workspace *aWorkspace)
{
    aWorkspace->aCallback((void*)aWorkspace->aCallbackWorkspace, aWorkspace->iBitOffset);
}

IRQ_ROUTINE( ILPC17xx_40xx_ENT0_Interrupt )
{
    IRQ_START();
    IENITProcessInterrupt(G_EINT_EINT0_Workspace);
    LPC_SC->EXTINT |= (1<<G_EINT_EINT0_Workspace->iBitOffset);
    IRQ_END();
}

IRQ_ROUTINE( ILPC17xx_40xx_ENT1_Interrupt )
{
    IRQ_START();
    IENITProcessInterrupt(G_EINT_EINT1_Workspace);
    LPC_SC->EXTINT |= (1<<G_EINT_EINT1_Workspace->iBitOffset);
    IRQ_END();
}

T_uezError LPC17xx_40xx_EINT_EITN0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC17xx_40xx_EINT_Workspace *p = (T_LPC17xx_40xx_EINT_Workspace*)aWorkspace;

    p->iBitOffset = 0;
    p->iInterruptChannel = EINT0_IRQn;
    p->iConfigured = EFalse;
    G_EINT_EINT0_Workspace = p;
    return UEZ_ERROR_NONE;
}

T_uezError LPC17xx_40xx_EINT_EITN1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC17xx_40xx_EINT_Workspace *p = (T_LPC17xx_40xx_EINT_Workspace*)aWorkspace;

    p->iBitOffset = 1;
    p->iInterruptChannel = EINT1_IRQn;
    p->iConfigured = EFalse;
    G_EINT_EINT1_Workspace = p;
    return UEZ_ERROR_NONE;
}

T_uezError LPC17xx_40xx_EINT_Set(
        void *aWorkspace,
        TUInt32 aChannel,
        T_EINTTrigger aTrigger,
        EINT_Callback aCallbackFunc,
        void *aCallbackWorkspace,
        T_EINTPriority aPriority,
        const char *aName)
{
    T_LPC17xx_40xx_EINT_Workspace *p = (T_LPC17xx_40xx_EINT_Workspace*)aWorkspace;

    if(aCallbackFunc !=0){
        p->aCallback = aCallbackFunc;
    } else{
        p->aCallback = 0;
    }

    if(aCallbackWorkspace){
        p->aCallbackWorkspace = aCallbackWorkspace;
    } else{
        p->aCallbackWorkspace = 0;
    }

    switch (aTrigger){
        case EINT_TRIGGER_EDGE_FALLING:
            LPC_SC->EXTMODE |= (1 << p->iBitOffset); //Edge sensitive
            LPC_SC->EXTPOLAR &= ~(1 << p->iBitOffset); //Falling Edge sensitive
            break;
        case EINT_TRIGGER_EDGE_RISING:
            LPC_SC->EXTMODE |= (1 << p->iBitOffset); //Edge sensitive
            LPC_SC->EXTPOLAR |= (1 << p->iBitOffset); //Rising Edge sensitive
            break;
        case EINT_TRIGGER_LEVEL_LOW:
            LPC_SC->EXTMODE &= ~(1 << p->iBitOffset); //Level sensitive
            LPC_SC->EXTPOLAR &= ~(1 << p->iBitOffset); //Active Low
            break;
        case EINT_TRIGGER_LEVEL_HIGH:
            LPC_SC->EXTMODE &= ~(1 << p->iBitOffset); //Level sensitive
            LPC_SC->EXTPOLAR |= (1 << p->iBitOffset); //Active High
            break;
        case EINT_TRIGGER_EDGE_BOTH:
            return UEZ_ERROR_NOT_SUPPORTED;
            break;
    }
    p->iConfigured = ETrue;
    return UEZ_ERROR_NONE;
}

T_uezError LPC17xx_40xx_EINT_Clear(void *aWorkspace, TUInt32 aChannel)
{
    T_LPC17xx_40xx_EINT_Workspace *p = (T_LPC17xx_40xx_EINT_Workspace*)aWorkspace;

    LPC_SC->EXTINT |= (1<<p->iBitOffset);
    return UEZ_ERROR_NONE;
}

T_uezError LPC17xx_40xx_EINT_Enable(void *aWorkspace, TUInt32 aChannel)
{
    T_LPC17xx_40xx_EINT_Workspace *p = (T_LPC17xx_40xx_EINT_Workspace*)aWorkspace;

    if(!p->iConfigured){
        return UEZ_ERROR_INCOMPLETE_CONFIGURATION;
    }
    LPC_SC->EXTINT |= (1<<p->iBitOffset);
    InterruptEnable(p->iInterruptChannel);
    return UEZ_ERROR_NONE;
}

T_uezError LPC17xx_40xx_EINT_Disable(void *aWorkspace, TUInt32 aChannel)
{
    T_LPC17xx_40xx_EINT_Workspace *p = (T_LPC17xx_40xx_EINT_Workspace*)aWorkspace;

    if(!p->iConfigured){
        return UEZ_ERROR_INCOMPLETE_CONFIGURATION;
    }
    InterruptDisable(p->iInterruptChannel);
    return UEZ_ERROR_NONE;
}

T_uezError LPC17xx_40xx_EINT_Reset(void *aWorkspace, TUInt32 aChannel)
{
    //T_LPC17xx_40xx_EINT_Workspace *p = (T_LPC17xx_40xx_EINT_Workspace*)aWorkspace;

    return UEZ_ERROR_NONE;
}

const HAL_EINT EINT_LPC17xx_40xx_EINT0_Interface = {
        {
        "NXP:LPC17xx_40xx:EINT0",
        0x0100,
        LPC17xx_40xx_EINT_EITN0_InitializeWorkspace,
        sizeof(T_LPC17xx_40xx_EINT_Workspace),
        },
        LPC17xx_40xx_EINT_Set,
        LPC17xx_40xx_EINT_Clear,
        LPC17xx_40xx_EINT_Enable,
        LPC17xx_40xx_EINT_Disable,
        LPC17xx_40xx_EINT_Reset,
};

const HAL_EINT EINT_LPC17xx_40xx_EINT1_Interface = {
        {
        "NXP:LPC17xx_40xx:EINT1",
        0x0100,
        LPC17xx_40xx_EINT_EITN1_InitializeWorkspace,
        sizeof(T_LPC17xx_40xx_EINT_Workspace),
        },
        LPC17xx_40xx_EINT_Set,
        LPC17xx_40xx_EINT_Clear,
        LPC17xx_40xx_EINT_Enable,
        LPC17xx_40xx_EINT_Disable,
        LPC17xx_40xx_EINT_Reset,
};


void LPC17xx_40xx_EINT0_Require(T_uezGPIOPortPin aGPIOPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList eint0[] = {
            {GPIO_P0_29,   IOCON_U_DEFAULT(2)},
            {GPIO_P2_10,   IOCON_D_DEFAULT(1)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aGPIOPin, eint0, ARRAY_COUNT(eint0));
    HALInterfaceRegister("EINT0",
            (T_halInterface *)&EINT_LPC17xx_40xx_EINT0_Interface, 0, 0);

    // Setup interrupt, but do not enable
    InterruptRegister(EINT0_IRQn, ILPC17xx_40xx_ENT0_Interrupt, INTERRUPT_PRIORITY_HIGH, "EINT0");
    InterruptDisable(EINT0_IRQn);
}

void LPC17xx_40xx_EINT1_Require(T_uezGPIOPortPin aGPIOPin)
{
    static const T_LPC17xx_40xx_IOCON_ConfigList eint1[] = {
            {GPIO_P0_30,   IOCON_U_DEFAULT(2)},
            {GPIO_P2_11,   IOCON_D_DEFAULT(1)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    LPC17xx_40xx_IOCON_ConfigPinOrNone(aGPIOPin, eint1, ARRAY_COUNT(eint1));
    HALInterfaceRegister("EINT1",
            (T_halInterface *)&EINT_LPC17xx_40xx_EINT1_Interface, 0, 0);

    // Setup interrupt, but do not enable
    InterruptRegister(EINT1_IRQn, ILPC17xx_40xx_ENT1_Interrupt, INTERRUPT_PRIORITY_HIGH, "EINT1");
    InterruptDisable(EINT1_IRQn);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_EINT.c
 *-------------------------------------------------------------------------*/
