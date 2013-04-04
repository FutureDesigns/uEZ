/*-------------------------------------------------------------------------*
 * File:  GPDMA.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1768 GP DMA controller.
 * Implementation:
 *
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
#include <LPC1768_UtilityFuncs.h>
#include <stddef.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_GPDMA.h>

// TBD:  not here!
#include <uEZRTOS.h>

/*-------------------------------------------------------------------------*
 * Defines:
 *-------------------------------------------------------------------------*/
#define INTERRUPT_CHANNEL_GP_DMA                            DMA_IRQn

#define GPDMA_FLOW_CONTROL_DMA_CONTROLLER                   (0<<2)
#define GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER            (1<<2)
#define GPDMA_FLOW_CONTROL_SOURCE_IS_MEMORY                 (0<<1)
#define GPDMA_FLOW_CONTROL_SOURCE_IS_PERIPHERAL             (1<<1)
#define GPDMA_FLOW_CONTROL_DESTINATION_IS_MEMORY            (0<<0)
#define GPDMA_FLOW_CONTROL_DESTINATION_IS_PERIPHERAL        (1<<0)

#define GPDMA_CHANNEL0 0
#define GPDMA_CHANNEL1 1
#define GPDMA_CHANNEL2 2
#define GPDMA_CHANNEL3 3
#define GPDMA_CHANNEL4 4
#define GPDMA_CHANNEL5 5
#define GPDMA_CHANNEL6 6
#define GPDMA_CHANNEL7 7

#define DMACDMACCDMACConfiguration_FlowCntrl_BIT    11
#define DMACDMACCDMACConfiguration_OFFSET 0x110
#define DMACDMACCDMACConfiguration_E_MASK 0x1
#define DMACDMACCDMACConfiguration_E 0x1
#define DMACDMACCDMACConfiguration_E_BIT 0
#define DMACDMACCDMACConfiguration_SrcPeripheral_MASK 0x1E
#define DMACDMACCDMACConfiguration_SrcPeripheral_BIT 1
#define DMACDMACCDMACConfiguration_DestPeripheral_MASK 0x3C0
#define DMACDMACCDMACConfiguration_DestPeripheral_BIT 6
#define DMACDMACCDMACConfiguration_FlowCntrl_MASK 0x3800
#define DMACDMACCDMACConfiguration_FlowCntrl_BIT 11
#define DMACDMACCDMACConfiguration_IE_MASK 0x4000
#define DMACDMACCDMACConfiguration_IE 0x4000
#define DMACDMACCDMACConfiguration_IE_BIT 14
#define DMACDMACCDMACConfiguration_ITC_MASK 0x8000
#define DMACDMACCDMACConfiguration_ITC 0x8000
#define DMACDMACCDMACConfiguration_ITC_BIT 15
#define DMACDMACCDMACConfiguration_L_MASK 0x10000
#define DMACDMACCDMACConfiguration_L 0x10000
#define DMACDMACCDMACConfiguration_L_BIT 16
#define DMACDMACCDMACConfiguration_A_MASK 0x20000
#define DMACDMACCDMACConfiguration_A 0x20000
#define DMACDMACCDMACConfiguration_A_BIT 17
#define DMACDMACCDMACConfiguration_H_MASK 0x40000
#define DMACDMACCDMACConfiguration_H 0x40000
#define DMACDMACCDMACConfiguration_H_BIT 18

#define DMACDMACCControl_TransferSize_MASK 0xFFF
#define DMACDMACCControl_TransferSize_BIT 0
#define DMACDMACCControl_Prot_MASK 0x70000000
#define DMACDMACCControl_Prot_BIT 28

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const HAL_GPDMA *iHAL;
    // Control word for DMA channel
    TUInt32 iControl;

    // DMACConfiguration word for DMA channel
    TUInt32 iDMACConfiguration;

    // source address
    TUInt32 iSource;

    // destination address
    TUInt32 iDestination;

    // address of linked list
    TUInt32 iLinkedList;

    // gpdma iChannel
    TUInt8 iChannel;

    T_gpdmaPeripheral iSourcePeripheral;
    T_gpdmaPeripheral iDestinationPeripheral;

    // ISR hook function
    T_gpdmaCallback iISRCallback;
    void *iISRCallbackWorkspace;
} T_LPC1768_GPDMA_Workspace;

/*---------------------------------------------------------------------------*
 * Forward declaration:
 *---------------------------------------------------------------------------*/
static void LPC1768_GPDMA_Enable(void *aWorkspace);
/*static void LPC1768_GPDMA_Disable(void *aWorkspace);*/
static TBool LPC1768_GPDMA_ChannelIsEnabled(void *aWorkspace);
static void IGPDMAProcessInterrupt(T_LPC1768_GPDMA_Workspace *p);
IRQ_ROUTINE( IGPDMA)
;

/*---------------------------------------------------------------------------*
 * Static global variables:
 *---------------------------------------------------------------------------*/
// Number of G_opened dma channels.
static TUInt8 G_opened = 0;
static GPDMACH_TypeDef * const G_channelRegs[8] = {
        GPDMACH0,
        GPDMACH1,
        GPDMACH2,
        GPDMACH3,
        GPDMACH4,
        GPDMACH5,
        GPDMACH6,
        GPDMACH7, };

static T_LPC1768_GPDMA_Workspace * G_workspaceGPDMAChannel[8];

T_uezError LPC1768_GPDMA_InitializeWorkspace_ChannelX(
        void *aWorkspace,
        TUInt8 aChannel)
{
    T_LPC1768_GPDMA_Workspace *p = (T_LPC1768_GPDMA_Workspace *)aWorkspace;

    // Setup GPDMA peripheral
    LPC1768_GPDMA_Enable(aWorkspace);

    // channel X
    p->iChannel = aChannel;

    // Fill the structure with default value for control and attributes.
    p->iControl = 0;
    p->iISRCallback = 0;
    p->iLinkedList = 0;
    p->iSource = 0;
    p->iDestination = 0;

    // Make local reference to the created workspace. Used in interrupt handler.
    G_workspaceGPDMAChannel[aChannel] = aWorkspace;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPDMA_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1768 GPDMA iChannel workspaces.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPDMA_InitializeWorkspace_Channel0(void *aWorkspace)
{
    return LPC1768_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
            GPDMA_CHANNEL0);
}
T_uezError LPC1768_GPDMA_InitializeWorkspace_Channel1(void *aWorkspace)
{
    return LPC1768_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
            GPDMA_CHANNEL1);
}
T_uezError LPC1768_GPDMA_InitializeWorkspace_Channel2(void *aWorkspace)
{
    return LPC1768_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
            GPDMA_CHANNEL2);
}
T_uezError LPC1768_GPDMA_InitializeWorkspace_Channel3(void *aWorkspace)
{
    return LPC1768_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
            GPDMA_CHANNEL3);
}
T_uezError LPC1768_GPDMA_InitializeWorkspace_Channel4(void *aWorkspace)
{
    return LPC1768_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
            GPDMA_CHANNEL4);
}
T_uezError LPC1768_GPDMA_InitializeWorkspace_Channel5(void *aWorkspace)
{
    return LPC1768_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
            GPDMA_CHANNEL5);
}
T_uezError LPC1768_GPDMA_InitializeWorkspace_Channel6(void *aWorkspace)
{
    return LPC1768_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
            GPDMA_CHANNEL6);
}
T_uezError LPC1768_GPDMA_InitializeWorkspace_Channel7(void *aWorkspace)
{
    return LPC1768_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
            GPDMA_CHANNEL7);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPDMA_Stop
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable GPDMA iChannel. Data is lost if is in DMA FIFO.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPDMA_Stop(void *aWorkspace)
{

    T_LPC1768_GPDMA_Workspace *p = (T_LPC1768_GPDMA_Workspace *)aWorkspace;

    // Zero out the enable bit
    G_channelRegs[p->iChannel]->DMACCConfig &= ~(1 << 0);

#if 0 // currently not supported
    if (p->iDestinationPeripheral == GPDMA_PERIPHERAL_SSP0_TX)
    SSP0DMACR &= ~SSP0DMACR_TXDMAE_MASK;
    if (p->iSourcePeripheral == GPDMA_PERIPHERAL_SSP0_RX)
    SSP0DMACR &= ~SSP0DMACR_RXDMAE_MASK;
    if (p->iDestinationPeripheral == GPDMA_PERIPHERAL_SSP1_TX)
    SSP1DMACR &= ~SSP1DMACR_TXDMAE_MASK;
    if (p->iSourcePeripheral == GPDMA_PERIPHERAL_SSP1_RX)
    SSP1DMACR &= ~SSP1DMACR_RXDMAE_MASK;
#endif
    // Fill the structure with default value for iControl and attributes.
    p->iControl = 0;

    return UEZ_ERROR_NONE;

}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPDMA_ChannelIsEnabled
 *---------------------------------------------------------------------------*
 * Description:
 *      Check if GPDMA iChannel is enabled -> transfer in progress.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      TBool                   -- Enabled ETrue else EFalse
 *---------------------------------------------------------------------------*/
static TBool LPC1768_GPDMA_ChannelIsEnabled(void *aWorkspace)
{
    T_LPC1768_GPDMA_Workspace *p = (T_LPC1768_GPDMA_Workspace *)aWorkspace;

    // Enabled iChannel
    return (GPDMA->DMACEnbldChns & (1 << p->iChannel)) ? ETrue : EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPDMA_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable GPDMA iChannel -> begin new transfer.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPDMA_Start(void *aWorkspace)
{
//    TUInt32 regValue = 0;
    T_LPC1768_GPDMA_Workspace *p = (T_LPC1768_GPDMA_Workspace *)aWorkspace;
    GPDMACH_TypeDef *p_channel;

    // Test if we can enable the iChannel.
    if (p->iControl == 0 || ETrue == LPC1768_GPDMA_ChannelIsEnabled(aWorkspace))
        return UEZ_ERROR_NOT_READY;

    //Clear any pending interrupts on the iChannel
    GPDMA->DMACIntTCClear = 1 << p->iChannel;
    GPDMA->DMACIntErrClr = 1 << p->iChannel;

    // Memory to memory transfer;
//    regValue |= ((0x0 & DMACDMACCDMACConfiguration_FlowCntrl_MASK)
//            << DMACDMACCDMACConfiguration_FlowCntrl_BIT) | DMACDMACCDMACConfiguration_E;

    // If the callback is set, process interrupts.
//    if (NULL != p->iISRCallback)
//        regValue |= DMACDMACCDMACConfiguration_IE | DMACDMACCDMACConfiguration_ITC;

    p_channel = G_channelRegs[p->iChannel];
    p_channel->DMACCSrcAddr = p->iSource;
    p_channel->DMACCDestAddr = p->iDestination;

    // linked list?
    p_channel->DMACCLLI = p->iLinkedList;

    // Write the iControl information into the DMACCxControl Register
    p_channel->DMACCControl = p->iControl;

    // Enable iChannel.
    p_channel->DMACCConfig = p->iDMACConfiguration;

#if 0 // Not currently supported
    if (p->iDestinationPeripheral == GPDMA_PERIPHERAL_SSP0_TX)
    SSP0DMACR |= SSP0DMACR_TXDMAE_MASK;
    if (p->iSourcePeripheral == GPDMA_PERIPHERAL_SSP0_RX)
    SSP0DMACR |= SSP0DMACR_RXDMAE_MASK;
    if (p->iDestinationPeripheral == GPDMA_PERIPHERAL_SSP1_TX)
    SSP1DMACR |= SSP1DMACR_TXDMAE_MASK;
    if (p->iSourcePeripheral == GPDMA_PERIPHERAL_SSP1_RX)
    SSP1DMACR |= SSP1DMACR_RXDMAE_MASK;
#endif

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPDMA_Enable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable GPDMA controller and register interrupt handler.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static void LPC1768_GPDMA_Enable(void *aWorkspace)
{
    (void)aWorkspace;

    // Increase number of G_opened channels.
    G_opened++;

    // Enable peripheral power (PCGPDMA)
    LPC1768PowerOn(1 << 29);

    // Was registered already?
    if (EFalse == InterruptIsRegistered(INTERRUPT_CHANNEL_GP_DMA)) {
        // Enable interrupt.
        InterruptRegister(INTERRUPT_CHANNEL_GP_DMA, (TISRFPtr)IGPDMA,
                INTERRUPT_PRIORITY_NORMAL, "GPDMA");
        InterruptEnable(INTERRUPT_CHANNEL_GP_DMA);
    }

    // Enable GPDMA by writing ENABLE bit.
    // Always little-endian.
    GPDMA->DMACConfig = DMACDMACCDMACConfiguration_E;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPDMA_PrepareRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Set up GPDMA iChannel.
 * Inputs:
 *      void *aW                       -- Particular GPDMA workspace
 *      const T_gpdmaRequest *aRequest -- Request to prepare
 *      T_gpdmaCallback aISRCallback   -- Routine to call when complete, or 0
 *                                       for none.
 *      void *aISRCallbackWorksspace   -- Pointer to data passed to callback
 *                                         routine.
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_GPDMA_PrepareRequest(
        void *aWorkspace,
        const T_gpdmaRequest *aRequest,
        T_gpdmaCallback aISRCallback,
        void *aISRCallbackWorkspace)
{
    TUInt32 control;
    TUInt32 flow;

    T_LPC1768_GPDMA_Workspace *p = (T_LPC1768_GPDMA_Workspace *)aWorkspace;

    // Load the destination & source address into the iControl block.
    // (Note: These only apply to memory transfers)
    p->iSource = (TUInt32)aRequest->iSourceAddr;
    p->iDestination = (TUInt32)aRequest->iDestinationAddr;

    // Load the iControl word.
    control = aRequest->iControl;

    // Channel 0 and 1 have the same defines, so only iChannel 0 is used.

    // Set the transfer size and mode in the iControl word.
    control |= (aRequest->iNumTransferItems << DMACDMACCControl_TransferSize_BIT)
            & DMACDMACCControl_TransferSize_MASK;

    // DMA Scatter-Gather?
    p->iLinkedList = (TUInt32)aRequest->iLinkedList;

    // Callback for DMA end or error?
    p->iISRCallback = aISRCallback;
    p->iISRCallbackWorkspace = aISRCallbackWorkspace;
    if (aISRCallback)
        control |= GPDMA_INT_ENABLE;

    // Bufferable and Cacheable. 0b110.
    control |= (0x6 << DMACDMACCControl_Prot_BIT) & DMACDMACCControl_Prot_MASK;

    // Store iControl value.
    p->iControl = control;

    // Flow control is to be determined by the various types
    // General output to match Table 675 from UM102347: LPC1768 user's manual
    flow = 0;
    p->iSourcePeripheral = aRequest->iSourcePeripheral;
    p->iDestinationPeripheral = aRequest->iDestinationPeripheral;

    if (aRequest->iSourcePeripheral != GPDMA_PERIPHERAL_MEMORY)
        flow |= GPDMA_FLOW_CONTROL_SOURCE_IS_PERIPHERAL;
    if (aRequest->iDestinationPeripheral != GPDMA_PERIPHERAL_MEMORY)
        flow |= GPDMA_FLOW_CONTROL_DESTINATION_IS_PERIPHERAL;

    if (aRequest->iFlowControl == GPDMA_FLOW_CONTROL_DMA) {
        flow |= GPDMA_FLOW_CONTROL_DMA_CONTROLLER;
    } else if (aRequest->iFlowControl == GPDMA_FLOW_CONTROL_SOURCE_PERIPHERAL) {
        flow |= GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER;
    } else if (aRequest->iFlowControl
            == GPDMA_FLOW_CONTROL_DESTINATION_PERIPHERAL) {
        if (aRequest->iSourcePeripheral != GPDMA_PERIPHERAL_MEMORY)
            flow = GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER;
        else
            flow |= GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER;
    }

    // Setup flow control
    p->iDMACConfiguration = ((flow << DMACDMACCDMACConfiguration_FlowCntrl_BIT)
            & DMACDMACCDMACConfiguration_FlowCntrl_MASK) | DMACDMACCDMACConfiguration_E;

    // If the callback is set, process interrupts.
    if (aISRCallback)
        p->iDMACConfiguration |= DMACDMACCDMACConfiguration_IE | DMACDMACCDMACConfiguration_ITC;

    // If the source is a peripheral, DMACConfigure the type
    if (aRequest->iSourcePeripheral != GPDMA_PERIPHERAL_MEMORY) {
        p->iDMACConfiguration |= ((aRequest->iSourcePeripheral
                << DMACDMACCDMACConfiguration_SrcPeripheral_BIT)
                & DMACDMACCDMACConfiguration_SrcPeripheral_MASK);
    }

    // If the destination is a peripheral, DMACConfigure the type
    if (aRequest->iDestinationPeripheral != GPDMA_PERIPHERAL_MEMORY) {
        p->iDMACConfiguration |= ((aRequest->iDestinationPeripheral
                << DMACDMACCDMACConfiguration_DestPeripheral_BIT)
                & DMACDMACCDMACConfiguration_DestPeripheral_MASK);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_GPDMA_IsValidAddress
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if the GPDMA is valid
 * Inputs:
 *      TUInt32 aAddr               -- Address to check if GPDMA works here
 * Outputs:
 *      TBool                       -- ETrue if valid, else EFalse
 *---------------------------------------------------------------------------*/
static TBool LPC1768_GPDMA_IsValidAddress(TUInt32 aAddr)
{
    // On the LPC1768, all addresses are valid
    return ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  IGPDMAProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Generic routine for handling any of the dma channels (as passed
 *      into the routine.
 *      NOTE:  This routine is ALWAYS processed inside of a interrupt
 *              service routine.
 * Inputs:
 *      T_LPC1768_GPDMA_Workspace *p      -- Port's definition to process interrupt
 *---------------------------------------------------------------------------*/
static void IGPDMAProcessInterrupt(T_LPC1768_GPDMA_Workspace *p)
{
    T_gpdmaState state = GPDMA_UNKNOWN;

    // Check error state. Higher priority.
    if (GPDMA->DMACIntErrStat & (1 << p->iChannel)) {
        state = GPDMA_FAIL;
    } else if (GPDMA->DMACIntTCStat & (1 << p->iChannel)) {
        // Completed.
        state = GPDMA_SUCCESS;
    }

    // Call the callback function.
    if (NULL != p->iISRCallback)
        p->iISRCallback(p->iISRCallbackWorkspace, state);

    // Clear possible interrupt sources.
    GPDMA->DMACIntTCClear = (1 << p->iChannel);
    GPDMA->DMACIntErrClr = (1 << p->iChannel);
}

/*---------------------------------------------------------------------------*
 * Routine:  IGPDMA
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the GPDMA interrupt and forward to IGPDMAProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(IGPDMA)
{
    IRQ_START();
    // Determine interrupt source.
    if (GPDMA->DMACIntStat & (1 << GPDMA_CHANNEL0)) {
        //iChannel 0 - Enable callback
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[0]);
    }
    if (GPDMA->DMACIntStat & (1 << GPDMA_CHANNEL1)) {
        //iChannel 1
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[1]);
    }
    if (GPDMA->DMACIntStat & (1 << GPDMA_CHANNEL2)) {
        //iChannel 2
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[2]);
    }
    if (GPDMA->DMACIntStat & (1 << GPDMA_CHANNEL3)) {
        //iChannel 3
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[3]);
    }
    if (GPDMA->DMACIntStat & (1 << GPDMA_CHANNEL4)) {
        //iChannel 4
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[4]);
    }
    if (GPDMA->DMACIntStat & (1 << GPDMA_CHANNEL5)) {
        //iChannel 5
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[5]);
    }
    if (GPDMA->DMACIntStat & (1 << GPDMA_CHANNEL6)) {
        //iChannel 6
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[6]);
    }
    if (GPDMA->DMACIntStat & (1 << GPDMA_CHANNEL7)) {
        //iChannel 7
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[7]);
    }
    IRQ_END()
    ;
}

const HAL_GPDMA LPC1768_GPDMA_Channel0_Interface = {
        {
                "GPDMA:LPC1768:CH0",
                0x0100,
                LPC1768_GPDMA_InitializeWorkspace_Channel0,
                sizeof(T_LPC1768_GPDMA_Workspace), },
        LPC1768_GPDMA_PrepareRequest,
        LPC1768_GPDMA_Start,
        LPC1768_GPDMA_Stop,
        LPC1768_GPDMA_IsValidAddress, };

const HAL_GPDMA LPC1768_GPDMA_Channel1_Interface = {
        {
                "GPDMA:LPC1768:CH1",
                0x0100,
                LPC1768_GPDMA_InitializeWorkspace_Channel1,
                sizeof(T_LPC1768_GPDMA_Workspace), },
        LPC1768_GPDMA_PrepareRequest,
        LPC1768_GPDMA_Start,
        LPC1768_GPDMA_Stop,
        LPC1768_GPDMA_IsValidAddress, };

const HAL_GPDMA LPC1768_GPDMA_Channel2_Interface = {
        {
                "GPDMA:LPC1768:CH2",
                0x0100,
                LPC1768_GPDMA_InitializeWorkspace_Channel2,
                sizeof(T_LPC1768_GPDMA_Workspace), },
        LPC1768_GPDMA_PrepareRequest,
        LPC1768_GPDMA_Start,
        LPC1768_GPDMA_Stop,
        LPC1768_GPDMA_IsValidAddress, };

const HAL_GPDMA LPC1768_GPDMA_Channel3_Interface = {
        {
                "GPDMA:LPC1768:CH3",
                0x0100,
                LPC1768_GPDMA_InitializeWorkspace_Channel3,
                sizeof(T_LPC1768_GPDMA_Workspace), },
        LPC1768_GPDMA_PrepareRequest,
        LPC1768_GPDMA_Start,
        LPC1768_GPDMA_Stop,
        LPC1768_GPDMA_IsValidAddress, };

const HAL_GPDMA LPC1768_GPDMA_Channel4_Interface = {
        {
                "GPDMA:LPC1768:CH4",
                0x0100,
                LPC1768_GPDMA_InitializeWorkspace_Channel4,
                sizeof(T_LPC1768_GPDMA_Workspace), },
        LPC1768_GPDMA_PrepareRequest,
        LPC1768_GPDMA_Start,
        LPC1768_GPDMA_Stop,
        LPC1768_GPDMA_IsValidAddress, };

const HAL_GPDMA LPC1768_GPDMA_Channel5_Interface = {
        {
                "GPDMA:LPC1768:CH5",
                0x0100,
                LPC1768_GPDMA_InitializeWorkspace_Channel5,
                sizeof(T_LPC1768_GPDMA_Workspace), },
        LPC1768_GPDMA_PrepareRequest,
        LPC1768_GPDMA_Start,
        LPC1768_GPDMA_Stop,
        LPC1768_GPDMA_IsValidAddress, };

const HAL_GPDMA LPC1768_GPDMA_Channel6_Interface = {
        {
                "GPDMA:LPC1768:CH6",
                0x0100,
                LPC1768_GPDMA_InitializeWorkspace_Channel6,
                sizeof(T_LPC1768_GPDMA_Workspace), },
        LPC1768_GPDMA_PrepareRequest,
        LPC1768_GPDMA_Start,
        LPC1768_GPDMA_Stop,
        LPC1768_GPDMA_IsValidAddress, };

const HAL_GPDMA LPC1768_GPDMA_Channel7_Interface = {
        {
                "GPDMA:LPC1768:CH7",
                0x0100,
                LPC1768_GPDMA_InitializeWorkspace_Channel7,
                sizeof(T_LPC1768_GPDMA_Workspace), },
        LPC1768_GPDMA_PrepareRequest,
        LPC1768_GPDMA_Start,
        LPC1768_GPDMA_Stop,
        LPC1768_GPDMA_IsValidAddress, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1768_GPDMA0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA0",
            (T_halInterface *)&LPC1768_GPDMA_Channel0_Interface, 0, 0);
}

void LPC1768_GPDMA1_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA1",
            (T_halInterface *)&LPC1768_GPDMA_Channel1_Interface, 0, 0);
}

void LPC1768_GPDMA2_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA2",
            (T_halInterface *)&LPC1768_GPDMA_Channel2_Interface, 0, 0);
}

void LPC1768_GPDMA3_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA3",
            (T_halInterface *)&LPC1768_GPDMA_Channel3_Interface, 0, 0);
}

void LPC1768_GPDMA4_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA4",
            (T_halInterface *)&LPC1768_GPDMA_Channel4_Interface, 0, 0);
}

void LPC1768_GPDMA5_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA5",
            (T_halInterface *)&LPC1768_GPDMA_Channel5_Interface, 0, 0);
}

void LPC1768_GPDMA6_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA6",
            (T_halInterface *)&LPC1768_GPDMA_Channel6_Interface, 0, 0);
}

void LPC1768_GPDMA7_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA7",
            (T_halInterface *)&LPC1768_GPDMA_Channel7_Interface, 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  GPDMA.c
 *-------------------------------------------------------------------------*/
