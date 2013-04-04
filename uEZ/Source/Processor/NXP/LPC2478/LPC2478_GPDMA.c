/*-------------------------------------------------------------------------*
* File:  GPDMA.c
*-------------------------------------------------------------------------*
* Description:
*      HAL implementation of the LPC2478 GP DMA controller.
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
#include <Config.h>
#include <stddef.h>

#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <uEZRTOS.h>
#include <Source/Processor/NXP/LPC2478/LPC2478_GPDMA.h>

/*-------------------------------------------------------------------------*
* Defines:
*-------------------------------------------------------------------------*/
#define GPDMA_FLOW_CONTROL_DMA_CONTROLLER                 (0<<2)
#define GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER          (1<<2)
#define GPDMA_FLOW_CONTROL_SOURCE_IS_MEMORY               (0<<1)
#define GPDMA_FLOW_CONTROL_SOURCE_IS_PERIPHERAL           (1<<1)
#define GPDMA_FLOW_CONTROL_DESTINATION_IS_MEMORY          (0<<0)
#define GPDMA_FLOW_CONTROL_DESTINATION_IS_PERIPHERAL      (1<<0)

#define GPDMA_CHANNEL0 0
#define GPDMA_CHANNEL1 1

/*---------------------------------------------------------------------------*
* Types:
*---------------------------------------------------------------------------*/
typedef struct {
    const HAL_GPDMA *iHAL;
    // Control word for DMA channel
    TUInt32 iControl; 

    // Configuration word for DMA channel
    TUInt32 iConfiguration;

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
} T_LPC2478_GPDMA_Workspace;


/*---------------------------------------------------------------------------*
* Forward declaration:
*---------------------------------------------------------------------------*/
static void LPC2478_GPDMA_Enable(void *aWorkspace);
/*static void LPC2478_GPDMA_Disable(void *aWorkspace);*/
static TBool LPC2478_GPDMA_ChannelIsEnabled(void *aWorkspace);
static void IGPDMAProcessInterrupt(T_LPC2478_GPDMA_Workspace *p);
IRQ_ROUTINE(IGPDMA);

/*---------------------------------------------------------------------------*
* Static global variables:
*---------------------------------------------------------------------------*/
// Number of G_opened dma channels.
static TUInt8 G_opened = 0;

static T_LPC2478_GPDMA_Workspace * G_workspaceGPDMAChannel0;
static T_LPC2478_GPDMA_Workspace * G_workspaceGPDMAChannel1;

/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_InitializeWorkspace
*---------------------------------------------------------------------------*
* Description:
*      Setup the LPC2478 GPDMA iChannel 0 workspace.
* Inputs:
*      void *aW                    -- Particular GPDMA workspace
* Outputs:
*      T_uezError                  -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC2478_GPDMA_InitializeWorkspace_Channel0(void *aWorkspace) {

    T_LPC2478_GPDMA_Workspace *p =
            (T_LPC2478_GPDMA_Workspace *) aWorkspace;

    // Setup GPDMA peripheral
    LPC2478_GPDMA_Enable(aWorkspace);

    // channel 0
    p->iChannel = GPDMA_CHANNEL0;

    // Fill the structure with default value for control and attributes.
    p->iControl = 0;
    p->iISRCallback = 0;
    p->iLinkedList = 0;
    p->iSource = 0;
    p->iDestination = 0;

    // Make local reference to the created workspace. Used in interrupt handler.
    G_workspaceGPDMAChannel0 = aWorkspace;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_InitializeWorkspace
*---------------------------------------------------------------------------*
* Description:
*      Setup the LPC2478 GPDMA iChannel 0 workspace.
* Inputs:
*      void *aW                    -- Particular GPDMA workspace
* Outputs:
*      T_uezError                  -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC2478_GPDMA_InitializeWorkspace_Channel1(void *aWorkspace) {

    T_LPC2478_GPDMA_Workspace *p =
            (T_LPC2478_GPDMA_Workspace *) aWorkspace;

    // Setup GPDMA peripheral
    LPC2478_GPDMA_Enable(aWorkspace);

    // iChannel 1
    p->iChannel = GPDMA_CHANNEL1;

    // Fill the structure with default value for iControl and attributes.
    p->iControl = 0;
    p->iISRCallback = 0;
    p->iLinkedList = 0;
    p->iSource = 0;
    p->iDestination = 0;

    // Make local reference to the created workspace. Used in interrupt handler.
    G_workspaceGPDMAChannel1 = aWorkspace;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_Stop
*---------------------------------------------------------------------------*
* Description:
*      Disable GPDMA iChannel. Data is lost if is in DMA FIFO.
* Inputs:
*      void *aW                    -- Particular GPDMA workspace
* Outputs:
*      T_uezError                  -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC2478_GPDMA_Stop(void *aWorkspace)
{

    T_LPC2478_GPDMA_Workspace *p =
                    (T_LPC2478_GPDMA_Workspace *) aWorkspace;

    switch (p->iChannel) {
        case GPDMA_CHANNEL0:
            // Disable iChannel.
            DMACC0Configuration = (0 << DMACC0Configuration_E_BIT);
            break;
        case GPDMA_CHANNEL1:
            // Disable iChannel.
            DMACC1Configuration = (0 << DMACC1Configuration_E_BIT);
            break;
        default:
            return UEZ_ERROR_ILLEGAL_PARAMETER;
    }

    if (p->iDestinationPeripheral == GPDMA_PERIPHERAL_SSP0_TX)
        SSP0DMACR &= ~SSP0DMACR_TXDMAE_MASK;
    if (p->iSourcePeripheral == GPDMA_PERIPHERAL_SSP0_RX)
        SSP0DMACR &= ~SSP0DMACR_RXDMAE_MASK;
    if (p->iDestinationPeripheral == GPDMA_PERIPHERAL_SSP1_TX)
        SSP1DMACR &= ~SSP1DMACR_TXDMAE_MASK;
    if (p->iSourcePeripheral == GPDMA_PERIPHERAL_SSP1_RX)
        SSP1DMACR &= ~SSP1DMACR_RXDMAE_MASK;

    // Fill the structure with default value for iControl and attributes.
    p->iControl = 0;

    return UEZ_ERROR_NONE;


}

/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_ChannelIsEnabled
*---------------------------------------------------------------------------*
* Description:
*      Check if GPDMA iChannel is enabled -> transfer in progress.
* Inputs:
*      void *aW                    -- Particular GPDMA workspace
* Outputs:
*      TBool                   -- Enabled ETrue else EFalse
*---------------------------------------------------------------------------*/
static TBool LPC2478_GPDMA_ChannelIsEnabled(void *aWorkspace)
{
    T_LPC2478_GPDMA_Workspace *p = (T_LPC2478_GPDMA_Workspace *) aWorkspace;

      // Enabled iChannel
     return (DMACEnbldChns & (1 << p->iChannel)) ? ETrue : EFalse;
}

/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_Start
*---------------------------------------------------------------------------*
* Description:
*      Enable GPDMA iChannel -> begin new transfer.
* Inputs:
*      void *aW                    -- Particular GPDMA workspace
* Outputs:
*      T_uezError                  -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC2478_GPDMA_Start(void *aWorkspace)
{
    TUInt32 regValue = 0;
    T_LPC2478_GPDMA_Workspace *p = (T_LPC2478_GPDMA_Workspace *) aWorkspace;

    // Test if we can enable the iChannel.
    if (p->iControl == 0 || ETrue == LPC2478_GPDMA_ChannelIsEnabled(aWorkspace))
        return UEZ_ERROR_NOT_READY;

    //Clear any pending interrupts on the iChannel
    DMACIntTCClear = 1 << p->iChannel;
    DMACIntErrClr = 1 << p->iChannel;

    // Memory to memory transfer;
    regValue |= ((0x0 & DMACC0Configuration_FlowCntrl_MASK)
            << DMACC0Configuration_FlowCntrl_BIT) | DMACC0Configuration_E;

    // If the callback is set, process interrupts.
    if(NULL != p->iISRCallback)
        regValue |= DMACC0Configuration_IE    | DMACC0Configuration_ITC;

    switch (p->iChannel) {
        case GPDMA_CHANNEL0:
        {
            // Source and destination address
            DMACC0SrcAddr = p->iSource;
            DMACC0DestAddr = p->iDestination;
    
            // linked list?
            DMACC0LLI = p->iLinkedList;
    
            // Write the iControl information into the DMACCxControl Register
            DMACC0Control = p->iControl;
    
            // Enable iChannel.
            DMACC0Configuration = p->iConfiguration;
//            DMACC0Configuration = regValue;
            break;
        }
        case GPDMA_CHANNEL1:
        {
            // Source and destination address
            DMACC1SrcAddr = p->iSource;
            DMACC1DestAddr = p->iDestination;
    
            // linked list?
            DMACC1LLI = p->iLinkedList;
    
            // Write the iControl information into the DMACCxControl Register
            DMACC1Control = p->iControl;
    
            // Enable iChannel.
            DMACC1Configuration = p->iConfiguration;
//            DMACC1Configuration = regValue;
            break;
        }
        default:
            return UEZ_ERROR_ILLEGAL_PARAMETER;
    }
    if (p->iDestinationPeripheral == GPDMA_PERIPHERAL_SSP0_TX)
        SSP0DMACR |= SSP0DMACR_TXDMAE_MASK;
    if (p->iSourcePeripheral == GPDMA_PERIPHERAL_SSP0_RX)
        SSP0DMACR |= SSP0DMACR_RXDMAE_MASK;
    if (p->iDestinationPeripheral == GPDMA_PERIPHERAL_SSP1_TX)
        SSP1DMACR |= SSP1DMACR_TXDMAE_MASK;
    if (p->iSourcePeripheral == GPDMA_PERIPHERAL_SSP1_RX)
        SSP1DMACR |= SSP1DMACR_RXDMAE_MASK;

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_Enable
*---------------------------------------------------------------------------*
* Description:
*      Enable GPDMA controller and register interrupt handler.
* Inputs:
*      void *aW                    -- Particular GPDMA workspace
* Outputs:
*      T_uezError                  -- Error code
*---------------------------------------------------------------------------*/
static void LPC2478_GPDMA_Enable(void *aWorkspace)
{
    (void)aWorkspace;

    // Increase number of G_opened channels.
    G_opened++;

    // Enable peripheral power
    PCONP |= PCONP_PCGPDMA;

    // Was registered already?
    if(EFalse == InterruptIsRegistered(INTERRUPT_CHANNEL_GP_DMA)) {
        // Enable interrupt.
        InterruptRegister(INTERRUPT_CHANNEL_GP_DMA, (TISRFPtr)IGPDMA, INTERRUPT_PRIORITY_NORMAL, "GPDMA");
        InterruptEnable(INTERRUPT_CHANNEL_GP_DMA);
    }

    // Enable GPDMA by writing ENABLE bit.
    // Always little-endian.
    DMACConfiguration = DMACConfiguration_E;
}

/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_Disable
*---------------------------------------------------------------------------*
* Description:
*      Disable GPDMA controller and deregister interrupt handler.
* Inputs:
*      void *aW                    -- Particular GPDMA workspace
* Outputs:
*      T_uezError                  -- Error code
*---------------------------------------------------------------------------*/
/* Not used currently
static void LPC2478_GPDMA_Disable(void *aWorkspace)
{
    (void)aWorkspace;

    // Decrease number of G_opened channels.
    G_opened--;

    if (0 == G_opened) {
        // Disable GPDMA by clearing ENABLE bit.
        DMACConfiguration = (0 << DMACConfiguration_E_BIT);
        // Disable interrupt
        InterruptDisable(INTERRUPT_CHANNEL_GP_DMA);
        InterruptUnregister(INTERRUPT_CHANNEL_GP_DMA);
    }
}
*/

#if 0
/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_PrepareMemToMem
*---------------------------------------------------------------------------*
* Description:
*      Set up GPDMA iChannel.
* Inputs:
*      void *aW                     -- Particular GPDMA workspace
*      T_gpdmaControl aControl                 -- Mode of operation
*      void *aSource                 -- Address of source
*      void *aDestination                 -- Address of destination
*      TUInt32 aNumTransferItems         -- Parameter is the number of data items, not the number of bytes.
*      void *aLinkedList                     -- Address of linked list
*      void (*aISRCallback)(TUInt32) -- Callback isr function
* Outputs:
*      T_uezError                  -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC2478_GPDMA_PrepareMemToMem(
        void *aWorkspace,
        T_gpdmaControl aControl, 
        void *aSource, 
        void *aDestination,
        TUInt32 aNumTransferItems, 
        T_gpdmaLinkedList *aLinkedList,
        void (*aISRCallback)(TUInt32)) 
{
    TUInt32 control = 0;
    T_LPC2478_GPDMA_Workspace *p =
                (T_LPC2478_GPDMA_Workspace *) aWorkspace;

    // Load the destination & source address into the iControl block.
    p->iSource = (TUInt32)aSource;
    p->iDestination = (TUInt32)aDestination;

    // Load the iControl word.
    control = aControl;

    // Channel 0 and 1 have the same defines, so only iChannel 0 is used.

    // Set the transfer size and mode in the iControl word.
    control |=  (aNumTransferItems << DMACC0Control_TransferSize_BIT) & DMACC0Control_TransferSize_MASK;

    // DMA Scatter-Gather?
    p->iLinkedList = (TUInt32)aLinkedList;

    // Callback for DMA end or error?
    p->iISRCallback = aISRCallback;
    if (aISRCallback)
        control |= GPDMA_INT_ENABLE;

    // Bufferable and Cacheable. 0b110.
    control |= (0x6 << DMACC0Control_Prot_BIT) & DMACC0Control_Prot_MASK;

    // Store iControl value.
    p->iControl = control;

    // Memory to memory transfer only
    p->iConfiguration = ((0x0 & DMACC0Configuration_FlowCntrl_MASK)
            << DMACC0Configuration_FlowCntrl_BIT) | DMACC0Configuration_E;

    // If the callback is set, process interrupts.
    if (aISRCallback)
        p->iConfiguration |= DMACC0Configuration_IE | DMACC0Configuration_ITC;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_PrepareAnyToAny
*---------------------------------------------------------------------------*
* Description:
*      Set up GPDMA iChannel.
* Inputs:
*      void *aW                       -- Particular GPDMA workspace
*      T_gpdmaControl aControl                  -- Mode of operation
*      void *aSource                  -- Address of source
*      void *aDestination             -- Address of destination
*      TUInt32 aNumTransferItems      -- Parameter is the number of data items, not the number of bytes.
*      void *aLinkedList              -- Address of linked list
*      void (*aISRCallback)(TUInt32)  -- Callback isr function
* Outputs:
*      T_uezError                  -- Error code
*---------------------------------------------------------------------------*/
T_uezError LPC2478_GPDMA_PrepareAnyToAny(
            void *aWorkspace,
            T_gpdmaControl aControl, 
            T_gpdmaPeripheral aSourcePeripheral,
            TUInt32 aSourceAddr, 
            T_gpdmaPeripheral aDestinationPeripheral,
            TUInt32 aDestinationAddr,
            T_gpdmaFlowControl aFlowControl,
            TUInt32 aNumTransferItems, 
            T_gpdmaLinkedList *aLinkedList,
            void (*aISRCallback)(T_gpdmaState))
{
    TUInt32 control;
    TUInt32 flow;

    T_LPC2478_GPDMA_Workspace *p =
                (T_LPC2478_GPDMA_Workspace *) aWorkspace;

    // Load the destination & source address into the iControl block.
    // (Note: These only apply to memory transfers)
    p->iSource = (TUInt32)aSourceAddr;
    p->iDestination = (TUInt32)aDestinationAddr;

    // Load the iControl word.
    control = aControl;

    // Channel 0 and 1 have the same defines, so only iChannel 0 is used.

    // Set the transfer size and mode in the iControl word.
    control |=  (aNumTransferItems << DMACC0Control_TransferSize_BIT) & DMACC0Control_TransferSize_MASK;

    // DMA Scatter-Gather?
    p->iLinkedList = (TUInt32)aLinkedList;

    // Callback for DMA end or error?
    p->iISRCallback = aISRCallback;
    if (aISRCallback)
        control |= GPDMA_INT_ENABLE;

    // Bufferable and Cacheable. 0b110.
    control |= (0x6 << DMACC0Control_Prot_BIT) & DMACC0Control_Prot_MASK;

    // Store iControl value.
    p->iControl = control;

    // Flow control is to be determined by the various types
    // General output to match Table 675 from UM102347: LPC2478 user's manual
    flow = 0;
    if (aSourcePeripheral != GPDMA_PERIPHERAL_MEMORY)
        flow |= GPDMA_FLOW_CONTROL_SOURCE_IS_PERIPHERAL;
    if (aDestinationPeripheral != GPDMA_PERIPHERAL_MEMORY)
        flow |= GPDMA_FLOW_CONTROL_DESTINATION_IS_PERIPHERAL;

    if (aFlowControl == GPDMA_FLOW_CONTROL_DMA) {
        flow |= GPDMA_FLOW_CONTROL_DMA_CONTROLLER;
    } else if (aFlowControl == GPDMA_FLOW_CONTROL_SOURCE_PERIPHERAL) {
        flow |= GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER;
    } else if (aFlowControl == GPDMA_FLOW_CONTROL_DESTINATION_PERIPHERAL) {
        if (aSourcePeripheral != GPDMA_PERIPHERAL_MEMORY)
            flow = GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER;
        else
            flow |= GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER;
    }

    // Setup flow control
    p->iConfiguration = ((flow << DMACC0Configuration_FlowCntrl_BIT) & DMACC0Configuration_FlowCntrl_MASK)
            | DMACC0Configuration_E;

    // If the callback is set, process interrupts.
    if (aISRCallback)
        p->iConfiguration |= DMACC0Configuration_IE | DMACC0Configuration_ITC;

    // If the source is a peripheral, configure the type
    if (aSourcePeripheral != GPDMA_PERIPHERAL_MEMORY) {
        p->iConfiguration |= 
            ((aSourcePeripheral << DMACC0Configuration_SrcPeripheral_BIT) 
                & DMACC0Configuration_SrcPeripheral_MASK);
    }

    // If the destination is a peripheral, configure the type
    if (aDestinationPeripheral != GPDMA_PERIPHERAL_MEMORY) {
        p->iConfiguration |= 
            ((aDestinationPeripheral << DMACC0Configuration_DestPeripheral_BIT) 
                & DMACC0Configuration_DestPeripheral_MASK);
    }

    return UEZ_ERROR_NONE;
}
#endif

/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_PrepareRequest
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
T_uezError LPC2478_GPDMA_PrepareRequest(
            void *aWorkspace,
            const T_gpdmaRequest *aRequest,
            T_gpdmaCallback aISRCallback,
            void *aISRCallbackWorkspace)
{
    TUInt32 control;
    TUInt32 flow;

    T_LPC2478_GPDMA_Workspace *p =
                (T_LPC2478_GPDMA_Workspace *) aWorkspace;

    // Load the destination & source address into the iControl block.
    // (Note: These only apply to memory transfers)
    p->iSource = (TUInt32)aRequest->iSourceAddr;
    p->iDestination = (TUInt32)aRequest->iDestinationAddr;

    // Load the iControl word.
    control = aRequest->iControl;

    // Channel 0 and 1 have the same defines, so only iChannel 0 is used.

    // Set the transfer size and mode in the iControl word.
    control |=  (aRequest->iNumTransferItems << DMACC0Control_TransferSize_BIT) & DMACC0Control_TransferSize_MASK;

    // DMA Scatter-Gather?
    p->iLinkedList = (TUInt32)aRequest->iLinkedList;

    // Callback for DMA end or error?
    p->iISRCallback = aISRCallback;
    p->iISRCallbackWorkspace = aISRCallbackWorkspace;
    if (aISRCallback)
        control |= GPDMA_INT_ENABLE;

    // Bufferable and Cacheable. 0b110.
    control |= (0x6 << DMACC0Control_Prot_BIT) & DMACC0Control_Prot_MASK;

    // Store iControl value.
    p->iControl = control;

    // Flow control is to be determined by the various types
    // General output to match Table 675 from UM102347: LPC2478 user's manual
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
    } else if (aRequest->iFlowControl == GPDMA_FLOW_CONTROL_DESTINATION_PERIPHERAL) {
        if (aRequest->iSourcePeripheral != GPDMA_PERIPHERAL_MEMORY)
            flow = GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER;
        else
            flow |= GPDMA_FLOW_CONTROL_PERIPHERAL_CONTROLLER;
    }

    // Setup flow control
    p->iConfiguration = ((flow << DMACC0Configuration_FlowCntrl_BIT) & DMACC0Configuration_FlowCntrl_MASK)
            | DMACC0Configuration_E;

    // If the callback is set, process interrupts.
    if (aISRCallback)
        p->iConfiguration |= DMACC0Configuration_IE | DMACC0Configuration_ITC;

    // If the source is a peripheral, configure the type
    if (aRequest->iSourcePeripheral != GPDMA_PERIPHERAL_MEMORY) {
        p->iConfiguration |= 
            ((aRequest->iSourcePeripheral << DMACC0Configuration_SrcPeripheral_BIT) 
                & DMACC0Configuration_SrcPeripheral_MASK);
    }

    // If the destination is a peripheral, configure the type
    if (aRequest->iDestinationPeripheral != GPDMA_PERIPHERAL_MEMORY) {
        p->iConfiguration |= 
            ((aRequest->iDestinationPeripheral << DMACC0Configuration_DestPeripheral_BIT) 
                & DMACC0Configuration_DestPeripheral_MASK);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
* Routine:  LPC2478_GPDMA_IsValidAddress
*---------------------------------------------------------------------------*
* Description:
*      Determine if the GPDMA is valid
* Inputs:
*      TUInt32 aAddr               -- Address to check if GPDMA works here
* Outputs:
*      TBool                       -- ETrue if valid, else EFalse
*---------------------------------------------------------------------------*/
static TBool LPC2478_GPDMA_IsValidAddress(TUInt32 aAddr)
{
    // The LPC2478 has 3 main regions where GPDMA can occur
    // USB/Ethernet memory          0x7FD00000 - 0x7FD03FFF  
    // Static Memory banks 0 - 3    0x80000000 - 0x83FFFFFF
    // Dynamic memory banks 0 - 3   0xA0000000 - 0xDFFFFFFF
    if ((aAddr >= 0x7FD00000) && (aAddr <= 0x7FD03FFF))
        return ETrue;
    if ((aAddr >= 0x80000000) && (aAddr <= 0x8EFFFFFF))
        return ETrue;
    if ((aAddr >= 0xA0000000) && (aAddr <= 0xDFFFFFFF))
        return ETrue;
    return EFalse;
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
*      T_LPC2478_GPDMA_Workspace *p      -- Port's definition to process interrupt
*---------------------------------------------------------------------------*/
static void IGPDMAProcessInterrupt(T_LPC2478_GPDMA_Workspace *p)
{
    T_gpdmaState state = GPDMA_UNKNOWN;

    // Check error state. Higher priority.
    if (DMACIntErrorStatus & (1 << p->iChannel)) {
        state = GPDMA_FAIL;
    } else if (DMACIntTCStatus & (1 << p->iChannel)) {
        // Completed.
        state = GPDMA_SUCCESS;
    }

    // Call the callback function.
    if (NULL != p->iISRCallback)
        p->iISRCallback(p->iISRCallbackWorkspace, state);

    // Clear possible interrupt sources.
    DMACIntTCClear = (1 << p->iChannel);
    DMACIntErrClr = (1 << p->iChannel);
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
    if (DMACIntStatus & (1 << GPDMA_CHANNEL0)) {
        //iChannel 0 - Enable callback
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel0);
    }
    if (DMACIntStatus & (1 << GPDMA_CHANNEL1)) {
        //iChannel 1
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel1);
    }
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC2478_GPDMA0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA0",
            (T_halInterface *)&LPC2478_GPDMA_Channel0_Interface, 0, 0);
}

void LPC2478_GPDMA1_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("GPDMA1",
            (T_halInterface *)&LPC2478_GPDMA_Channel1_Interface, 0, 0);
}

/*---------------------------------------------------------------------------*
 * Interfaces:
 *---------------------------------------------------------------------------*/
const HAL_GPDMA LPC2478_GPDMA_Channel0_Interface = {
    {
        "GPDMA:LPC2478:CH0", 
        0x0100,
        LPC2478_GPDMA_InitializeWorkspace_Channel0,
        sizeof(T_LPC2478_GPDMA_Workspace), 
    },
//    LPC2478_GPDMA_PrepareMemToMem,
//    LPC2478_GPDMA_PrepareAnyToAny,
    LPC2478_GPDMA_PrepareRequest,
    LPC2478_GPDMA_Start,
    LPC2478_GPDMA_Stop,
    LPC2478_GPDMA_IsValidAddress,
};

const HAL_GPDMA LPC2478_GPDMA_Channel1_Interface = { 
    { 
        "GPDMA:LPC2478:CH1", 
        0x0100,
        LPC2478_GPDMA_InitializeWorkspace_Channel1,
        sizeof(T_LPC2478_GPDMA_Workspace), 
    }, 
//    LPC2478_GPDMA_PrepareMemToMem,
//    LPC2478_GPDMA_PrepareAnyToAny,
    LPC2478_GPDMA_PrepareRequest,
    LPC2478_GPDMA_Start,
    LPC2478_GPDMA_Stop, 
    LPC2478_GPDMA_IsValidAddress,
};


/*-------------------------------------------------------------------------*
* End of File:  GPDMA.c
*-------------------------------------------------------------------------*/
