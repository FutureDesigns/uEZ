/*-------------------------------------------------------------------------*
 * File:  GPDMA.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC17xx_40xx GP DMA controller.
 * Implementation:
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
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
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_GPDMA.h>

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

#define DMACCConfiguration_FlowCntrl_BIT    11
#define DMACCConfiguration_OFFSET 0x110
#define DMACCConfiguration_E_MASK 0x1
#define DMACCConfiguration_E 0x1
#define DMACCConfiguration_E_BIT 0
#define DMACCConfiguration_SrcPeripheral_MASK 0x1E
#define DMACCConfiguration_SrcPeripheral_BIT 1
#define DMACCConfiguration_DestPeripheral_MASK 0x3C0
#define DMACCConfiguration_DestPeripheral_BIT 6
#define DMACCConfiguration_FlowCntrl_MASK 0x3800
#define DMACCConfiguration_FlowCntrl_BIT 11
#define DMACCConfiguration_IE_MASK 0x4000
#define DMACCConfiguration_IE 0x4000
#define DMACCConfiguration_IE_BIT 14
#define DMACCConfiguration_ITC_MASK 0x8000
#define DMACCConfiguration_ITC 0x8000
#define DMACCConfiguration_ITC_BIT 15
#define DMACCConfiguration_L_MASK 0x10000
#define DMACCConfiguration_L 0x10000
#define DMACCConfiguration_L_BIT 16
#define DMACCConfiguration_A_MASK 0x20000
#define DMACCConfiguration_A 0x20000
#define DMACCConfiguration_A_BIT 17
#define DMACCConfiguration_H_MASK 0x40000
#define DMACCConfiguration_H 0x40000
#define DMACCConfiguration_H_BIT 18

#define DMACCControl_TransferSize_MASK 0xFFF
#define DMACCControl_TransferSize_BIT 0
#define DMACCControl_Prot_MASK 0x70000000
#define DMACCControl_Prot_BIT 28

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

    TUInt32 iSourcePeripheral;
    TUInt32 iDestinationPeripheral;

    // ISR hook function
    T_gpdmaCallback iISRCallback;
    void *iISRCallbackWorkspace;
} T_LPC17xx_40xx_GPDMA_Workspace;

/*---------------------------------------------------------------------------*
 * Forward declaration:
 *---------------------------------------------------------------------------*/
static void LPC17xx_40xx_GPDMA_Enable(void *aWorkspace);
/*static void LPC17xx_40xx_GPDMA_Disable(void *aWorkspace);*/
static TBool LPC17xx_40xx_GPDMA_ChannelIsEnabled(void *aWorkspace);
static void IGPDMAProcessInterrupt(T_LPC17xx_40xx_GPDMA_Workspace *p);
IRQ_ROUTINE( IGPDMA)
;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
#define GPDMA_PERIPHERAL_PRIMARY          0x0000
#define GPDMA_PERIPHERAL_SECONDARY        0x8000
#define GPDMA_PERIPHERAL_MASK             0x7FFF
const TUInt32 G_peripheralMap[GPDMA_PERIPHERAL_UART4_RX + 1] = {
    GPDMA_PERIPHERAL_PRIMARY | 2,  //#define GPDMA_PERIPHERAL_SSP0_TX        0
    GPDMA_PERIPHERAL_PRIMARY | 3,  //#define GPDMA_PERIPHERAL_SSP0_RX        1
    GPDMA_PERIPHERAL_PRIMARY | 4,  //#define GPDMA_PERIPHERAL_SSP1_TX        2
    GPDMA_PERIPHERAL_PRIMARY | 5,  //#define GPDMA_PERIPHERAL_SSP1_RX        3
    GPDMA_PERIPHERAL_PRIMARY | 1,  //#define GPDMA_PERIPHERAL_SD_MMC         4
    GPDMA_PERIPHERAL_SECONDARY | 6,  //#define GPDMA_PERIPHERAL_I2S0           5
    GPDMA_PERIPHERAL_SECONDARY | 7,  //#define GPDMA_PERIPHERAL_I2S1           6
    GPDMA_PERIPHERAL_PRIMARY | 6,  //#define GPDMA_PERIPHERAL_SSP2_TX        7
    GPDMA_PERIPHERAL_PRIMARY | 7,  //#define GPDMA_PERIPHERAL_SSP2_RX        8
    GPDMA_PERIPHERAL_PRIMARY | 10, //#define GPDMA_PERIPHERAL_UART0_TX       9
    GPDMA_PERIPHERAL_PRIMARY | 11, //#define GPDMA_PERIPHERAL_UART0_RX       10
    GPDMA_PERIPHERAL_PRIMARY | 12, //#define GPDMA_PERIPHERAL_UART1_TX       11
    GPDMA_PERIPHERAL_PRIMARY | 13, //#define GPDMA_PERIPHERAL_UART1_RX       12
    GPDMA_PERIPHERAL_PRIMARY | 14, //#define GPDMA_PERIPHERAL_UART2_TX       13
    GPDMA_PERIPHERAL_PRIMARY | 15, //#define GPDMA_PERIPHERAL_UART2_RX       14
    GPDMA_PERIPHERAL_PRIMARY | 8,  //#define GPDMA_PERIPHERAL_ADC            15
    GPDMA_PERIPHERAL_PRIMARY | 9,  //#define GPDMA_PERIPHERAL_DAC            16
    GPDMA_PERIPHERAL_SECONDARY | 0, //#define GPDMA_PERIPHERAL_TIMER0_MAT0    17
    GPDMA_PERIPHERAL_SECONDARY | 1, //#define GPDMA_PERIPHERAL_TIMER0_MAT1    18
    GPDMA_PERIPHERAL_SECONDARY | 2, //#define GPDMA_PERIPHERAL_TIMER1_MAT0    19
    GPDMA_PERIPHERAL_SECONDARY | 3, //#define GPDMA_PERIPHERAL_TIMER1_MAT1    20
    GPDMA_PERIPHERAL_SECONDARY | 4, //#define GPDMA_PERIPHERAL_TIMER2_MAT0    21
    GPDMA_PERIPHERAL_SECONDARY | 5, //#define GPDMA_PERIPHERAL_TIMER2_MAT1    22
    GPDMA_PERIPHERAL_SECONDARY | 14, //#define GPDMA_PERIPHERAL_TIMER3_MAT0    23
    GPDMA_PERIPHERAL_SECONDARY | 15, //#define GPDMA_PERIPHERAL_TIMER3_MAT1    24
    GPDMA_PERIPHERAL_SECONDARY | 10, //#define GPDMA_PERIPHERAL_UART3_TX       27
    GPDMA_PERIPHERAL_SECONDARY | 11, //#define GPDMA_PERIPHERAL_UART3_RX       28
    GPDMA_PERIPHERAL_SECONDARY | 12, //#define GPDMA_PERIPHERAL_UART4_TX       29
    GPDMA_PERIPHERAL_SECONDARY | 13, //#define GPDMA_PERIPHERAL_UART4_RX       30
    };

/*---------------------------------------------------------------------------*
 * Static global variables:
 *---------------------------------------------------------------------------*/
// Number of G_opened dma channels.
static TUInt8 G_opened = 0;
static LPC_GPDMACH_TypeDef * const G_channelRegs[8] = {
    LPC_GPDMACH0,
    LPC_GPDMACH1,
    LPC_GPDMACH2,
    LPC_GPDMACH3,
    LPC_GPDMACH4,
    LPC_GPDMACH5,
    LPC_GPDMACH6,
    LPC_GPDMACH7, };

static T_LPC17xx_40xx_GPDMA_Workspace * G_workspaceGPDMAChannel[8];

T_uezError LPC17xx_40xx_GPDMA_InitializeWorkspace_ChannelX(
    void *aWorkspace,
    TUInt8 aChannel)
{
    T_LPC17xx_40xx_GPDMA_Workspace *p = (T_LPC17xx_40xx_GPDMA_Workspace *)aWorkspace;

    // Setup GPDMA peripheral
    LPC17xx_40xx_GPDMA_Enable(aWorkspace);

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
 * Routine:  LPC17xx_40xx_GPDMA_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC17xx_40xx GPDMA iChannel workspaces.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel0(void *aWorkspace)
{
    return LPC17xx_40xx_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
        GPDMA_CHANNEL0);
}
T_uezError LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel1(void *aWorkspace)
{
    return LPC17xx_40xx_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
        GPDMA_CHANNEL1);
}
T_uezError LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel2(void *aWorkspace)
{
    return LPC17xx_40xx_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
        GPDMA_CHANNEL2);
}
T_uezError LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel3(void *aWorkspace)
{
    return LPC17xx_40xx_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
        GPDMA_CHANNEL3);
}
T_uezError LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel4(void *aWorkspace)
{
    return LPC17xx_40xx_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
        GPDMA_CHANNEL4);
}
T_uezError LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel5(void *aWorkspace)
{
    return LPC17xx_40xx_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
        GPDMA_CHANNEL5);
}
T_uezError LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel6(void *aWorkspace)
{
    return LPC17xx_40xx_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
        GPDMA_CHANNEL6);
}
T_uezError LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel7(void *aWorkspace)
{
    return LPC17xx_40xx_GPDMA_InitializeWorkspace_ChannelX(aWorkspace,
        GPDMA_CHANNEL7);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_GPDMA_Stop
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable GPDMA iChannel. Data is lost if is in DMA FIFO.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_GPDMA_Stop(void *aWorkspace)
{

    T_LPC17xx_40xx_GPDMA_Workspace *p = (T_LPC17xx_40xx_GPDMA_Workspace *)aWorkspace;

    // Zero out the enable bit
    G_channelRegs[p->iChannel]->CConfig &= ~(1 << 0);

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
 * Routine:  LPC17xx_40xx_GPDMA_ChannelIsEnabled
 *---------------------------------------------------------------------------*
 * Description:
 *      Check if GPDMA iChannel is enabled -> transfer in progress.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      TBool                   -- Enabled ETrue else EFalse
 *---------------------------------------------------------------------------*/
static TBool LPC17xx_40xx_GPDMA_ChannelIsEnabled(void *aWorkspace)
{
    T_LPC17xx_40xx_GPDMA_Workspace *p = (T_LPC17xx_40xx_GPDMA_Workspace *)aWorkspace;

    // Enabled iChannel
    return (LPC_GPDMA->EnbldChns & (1 << p->iChannel)) ? ETrue : EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_GPDMA_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable GPDMA iChannel -> begin new transfer.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_GPDMA_Start(void *aWorkspace)
{
//    TUInt32 regValue = 0;
    T_LPC17xx_40xx_GPDMA_Workspace *p = (T_LPC17xx_40xx_GPDMA_Workspace *)aWorkspace;
    LPC_GPDMACH_TypeDef *p_channel;

    // Test if we can enable the iChannel.
    if (p->iControl == 0 || ETrue == LPC17xx_40xx_GPDMA_ChannelIsEnabled(aWorkspace))
        return UEZ_ERROR_NOT_READY;

    //Clear any pending interrupts on the iChannel
    LPC_GPDMA->IntTCClear = 1 << p->iChannel;
    LPC_GPDMA->IntErrClr = 1 << p->iChannel;

    // Memory to memory transfer;
//    regValue |= ((0x0 & DMACCConfiguration_FlowCntrl_MASK)
//            << DMACCConfiguration_FlowCntrl_BIT) | DMACCConfiguration_E;

    // If the callback is set, process interrupts.
//    if (NULL != p->iISRCallback)
//        regValue |= DMACCConfiguration_IE | DMACCConfiguration_ITC;

    p_channel = G_channelRegs[p->iChannel];
    p_channel->CSrcAddr = p->iSource;
    p_channel->CDestAddr = p->iDestination;

    // linked list?
    p_channel->CLLI = p->iLinkedList;

    // Write the iControl information into the DMACCxControl Register
    p_channel->CControl = p->iControl;

    // Enable iChannel.
    p_channel->CConfig = p->iConfiguration;

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
 * Routine:  LPC17xx_40xx_GPDMA_Enable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable GPDMA controller and register interrupt handler.
 * Inputs:
 *      void *aW                    -- Particular GPDMA workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static void LPC17xx_40xx_GPDMA_Enable(void *aWorkspace)
{
    (void)aWorkspace;

    // Increase number of G_opened channels.
    G_opened++;

    // Enable peripheral power (PCGPDMA)
    LPC17xx_40xxPowerOn(1 << 29);

    // Was registered already?
    if (EFalse == InterruptIsRegistered(INTERRUPT_CHANNEL_GP_DMA)) {
        // Enable interrupt.
        InterruptRegister(INTERRUPT_CHANNEL_GP_DMA, (TISRFPtr)IGPDMA,
            INTERRUPT_PRIORITY_NORMAL, "GPDMA");
        InterruptEnable(INTERRUPT_CHANNEL_GP_DMA);
    }

    // Enable GPDMA by writing ENABLE bit.
    // Always little-endian.
    LPC_GPDMA->Config = DMACCConfiguration_E;
}

/*---------------------------------------------------------------------------*
 * Routine:  IPeripheralToIndex
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert a peripheral number into an index in the DMA registers.
 * Inputs:
 *      T_gpdmaRequest aPeripherl   -- Peripheral type from Types/GPDMA.h
 * Outputs:
 *      TUInt32 -- Index or GPDMA_PERIPHERAL_UNKNOWN if unknown
 *---------------------------------------------------------------------------*/
static TUInt32 IPeripheralToIndex(T_gpdmaPeripheral aPeripheral)
{
    TUInt32 index = GPDMA_PERIPHERAL_UNKNOWN;
    if (aPeripheral <= GPDMA_PERIPHERAL_UART4_RX) {
        return G_peripheralMap[aPeripheral] & GPDMA_PERIPHERAL_MASK;
    }
    return index;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_GPDMA_PrepareRequest
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
T_uezError LPC17xx_40xx_GPDMA_PrepareRequest(
    void *aWorkspace,
    const T_gpdmaRequest *aRequest,
    T_gpdmaCallback aISRCallback,
    void *aISRCallbackWorkspace)
{
    TUInt32 control;
    TUInt32 flow;
    TUInt32 perip;

    T_LPC17xx_40xx_GPDMA_Workspace *p = (T_LPC17xx_40xx_GPDMA_Workspace *)aWorkspace;

    // Load the destination & source address into the iControl block.
    // (Note: These only apply to memory transfers)
    p->iSource = (TUInt32)aRequest->iSourceAddr;
    p->iDestination = (TUInt32)aRequest->iDestinationAddr;

    // Load the iControl word.
    control = aRequest->iControl;

    // Channel 0 and 1 have the same defines, so only iChannel 0 is used.

    // Set the transfer size and mode in the iControl word.
    control |= (aRequest->iNumTransferItems << DMACCControl_TransferSize_BIT)
        & DMACCControl_TransferSize_MASK;

    // DMA Scatter-Gather?
    p->iLinkedList = (TUInt32)aRequest->iLinkedList;

    // Callback for DMA end or error?
    p->iISRCallback = aISRCallback;
    p->iISRCallbackWorkspace = aISRCallbackWorkspace;
    if (aISRCallback)
        control |= GPDMA_INT_ENABLE;

    // Bufferable and Cacheable. 0b110.
    control |= (0x6 << DMACCControl_Prot_BIT) & DMACCControl_Prot_MASK;

    // Store iControl value.
    p->iControl = control;

    // Flow control is to be determined by the various types
    // General output to match Table 675 from UM102347: LPC17xx_40xx user's manual
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
    p->iConfiguration = ((flow << DMACCConfiguration_FlowCntrl_BIT)
        & DMACCConfiguration_FlowCntrl_MASK) | DMACCConfiguration_E;

    // If the callback is set, process interrupts.
    if (aISRCallback)
        p->iConfiguration |= DMACCConfiguration_IE | DMACCConfiguration_ITC;

    // If the source is a peripheral, configure the type
    if (aRequest->iSourcePeripheral != GPDMA_PERIPHERAL_MEMORY) {
        perip = IPeripheralToIndex(aRequest->iSourcePeripheral);
        if (perip == GPDMA_PERIPHERAL_UNKNOWN)
            return UEZ_ERROR_INVALID_PARAMETER;
        p->iConfiguration |= ((perip << DMACCConfiguration_SrcPeripheral_BIT)
            & DMACCConfiguration_SrcPeripheral_MASK);
    }

    // If the destination is a peripheral, configure the type
    if (aRequest->iDestinationPeripheral != GPDMA_PERIPHERAL_MEMORY) {
        perip = IPeripheralToIndex(aRequest->iDestinationPeripheral);
        if (perip == GPDMA_PERIPHERAL_UNKNOWN)
            return UEZ_ERROR_INVALID_PARAMETER;
        p->iConfiguration |= ((perip << DMACCConfiguration_DestPeripheral_BIT)
            & DMACCConfiguration_DestPeripheral_MASK);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_GPDMA_BurstRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a GPDMA Burst Request for the given peripheral
 * Inputs:
 *      void *aWorkspace                -- Particular GPDMA workspace
 *      T_gpdmaPeripheral aPeripheral   -- Peripheral to handle request
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_NOT_FOUND
 *                                      if peripheral not supported.
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_GPDMA_BurstRequest(
    void *aWorkspace,
    T_gpdmaPeripheral aPeripheral)
{
    TUInt32 b;
    PARAM_NOT_USED(aWorkspace);

    b = IPeripheralToIndex(aPeripheral);
    if (b == GPDMA_PERIPHERAL_UNKNOWN)
        return UEZ_ERROR_NOT_FOUND;
    LPC_GPDMA->SoftBReq = (1<<b);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_GPDMA_SingleRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a GPDMA Single Request for the given peripheral
 * Inputs:
 *      void *aWorkspace                -- Particular GPDMA workspace
 *      T_gpdmaPeripheral aPeripheral   -- Peripheral to handle request
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_NOT_FOUND
 *                                      if peripheral not supported.
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_GPDMA_SingleRequest(
    void *aWorkspace,
    T_gpdmaPeripheral aPeripheral)
{
    TUInt32 b;
    PARAM_NOT_USED(aWorkspace);

    b = IPeripheralToIndex(aPeripheral);
    if (b == GPDMA_PERIPHERAL_UNKNOWN)
        return UEZ_ERROR_NOT_FOUND;
    LPC_GPDMA->SoftSReq = (1<<b);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_GPDMA_LastBurstRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a GPDMA Last Burst Request for the given peripheral
 * Inputs:
 *      void *aWorkspace                -- Particular GPDMA workspace
 *      T_gpdmaPeripheral aPeripheral   -- Peripheral to handle request
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_NOT_FOUND
 *                                      if peripheral not supported.
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_GPDMA_LastBurstRequest(
    void *aWorkspace,
    T_gpdmaPeripheral aPeripheral)
{
    TUInt32 b;
    PARAM_NOT_USED(aWorkspace);

    b = IPeripheralToIndex(aPeripheral);
    if (b == GPDMA_PERIPHERAL_UNKNOWN)
        return UEZ_ERROR_NOT_FOUND;
    LPC_GPDMA->SoftLBReq = (1<<b);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_GPDMA_LastSingleRequest
 *---------------------------------------------------------------------------*
 * Description:
 *      Generate a GPDMA Last Single Request for the given peripheral
 * Inputs:
 *      void *aWorkspace                -- Particular GPDMA workspace
 *      T_gpdmaPeripheral aPeripheral   -- Peripheral to handle request
 * Outputs:
 *      T_uezError                  -- Error code, UEZ_ERROR_NOT_FOUND
 *                                      if peripheral not supported.
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_GPDMA_LastSingleRequest(
    void *aWorkspace,
    T_gpdmaPeripheral aPeripheral)
{
    TUInt32 b;
    PARAM_NOT_USED(aWorkspace);

    b = IPeripheralToIndex(aPeripheral);
    if (b == GPDMA_PERIPHERAL_UNKNOWN)
        return UEZ_ERROR_NOT_FOUND;
    LPC_GPDMA->SoftLSReq = (1<<b);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_GPDMA_IsValidAddress
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if the GPDMA is valid
 * Inputs:
 *      TUInt32 aAddr               -- Address to check if GPDMA works here
 * Outputs:
 *      TBool                       -- ETrue if valid, else EFalse
 *---------------------------------------------------------------------------*/
static TBool LPC17xx_40xx_GPDMA_IsValidAddress(TUInt32 aAddr)
{
    // On the LPC17xx_40xx, all addresses are valid
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
 *      T_LPC17xx_40xx_GPDMA_Workspace *p      -- Port's definition to process interrupt
 *---------------------------------------------------------------------------*/
static void IGPDMAProcessInterrupt(T_LPC17xx_40xx_GPDMA_Workspace *p)
{
    T_gpdmaState state = GPDMA_UNKNOWN;

    // Check error state. Higher priority.
    if (LPC_GPDMA->IntErrStat & (1 << p->iChannel)) {
        state = GPDMA_FAIL;
    } else if (LPC_GPDMA->IntTCStat & (1 << p->iChannel)) {
        // Completed.
        state = GPDMA_SUCCESS;
    }

    // Call the callback function.
    if (NULL != p->iISRCallback)
        p->iISRCallback(p->iISRCallbackWorkspace, state);

    // Clear possible interrupt sources.
    LPC_GPDMA->IntTCClear = (1 << p->iChannel);
    LPC_GPDMA->IntErrClr = (1 << p->iChannel);
}

/*---------------------------------------------------------------------------*
 * Routine:  IGPDMA
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the GPDMA interrupt and forward to IGPDMAProcessInterrupt.
 *---------------------------------------------------------------------------*/IRQ_ROUTINE(IGPDMA)
{
    IRQ_START();
    // Determine interrupt source.
    if (LPC_GPDMA->IntStat & (1 << GPDMA_CHANNEL0)) {
        //iChannel 0 - Enable callback
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[0]);
    }
    if (LPC_GPDMA->IntStat & (1 << GPDMA_CHANNEL1)) {
        //iChannel 1
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[1]);
    }
    if (LPC_GPDMA->IntStat & (1 << GPDMA_CHANNEL2)) {
        //iChannel 2
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[2]);
    }
    if (LPC_GPDMA->IntStat & (1 << GPDMA_CHANNEL3)) {
        //iChannel 3
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[3]);
    }
    if (LPC_GPDMA->IntStat & (1 << GPDMA_CHANNEL4)) {
        //iChannel 4
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[4]);
    }
    if (LPC_GPDMA->IntStat & (1 << GPDMA_CHANNEL5)) {
        //iChannel 5
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[5]);
    }
    if (LPC_GPDMA->IntStat & (1 << GPDMA_CHANNEL6)) {
        //iChannel 6
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[6]);
    }
    if (LPC_GPDMA->IntStat & (1 << GPDMA_CHANNEL7)) {
        //iChannel 7
        IGPDMAProcessInterrupt(G_workspaceGPDMAChannel[7]);
    } IRQ_END()
    ;
}

const HAL_GPDMA LPC17xx_40xx_GPDMA_Channel0_Interface = {
    {
        "GPDMA:LPC17xx_40xx:CH0",
        0x0100,
        LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel0,
        sizeof(T_LPC17xx_40xx_GPDMA_Workspace), },
    LPC17xx_40xx_GPDMA_PrepareRequest,
    LPC17xx_40xx_GPDMA_Start,
    LPC17xx_40xx_GPDMA_Stop,
    LPC17xx_40xx_GPDMA_IsValidAddress,

    // v2.04
    LPC17xx_40xx_GPDMA_BurstRequest,
    LPC17xx_40xx_GPDMA_SingleRequest,
    LPC17xx_40xx_GPDMA_LastBurstRequest,
    LPC17xx_40xx_GPDMA_LastSingleRequest };

const HAL_GPDMA LPC17xx_40xx_GPDMA_Channel1_Interface = {
    {
        "GPDMA:LPC17xx_40xx:CH1",
        0x0100,
        LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel1,
        sizeof(T_LPC17xx_40xx_GPDMA_Workspace), },
    LPC17xx_40xx_GPDMA_PrepareRequest,
    LPC17xx_40xx_GPDMA_Start,
    LPC17xx_40xx_GPDMA_Stop,
    LPC17xx_40xx_GPDMA_IsValidAddress,

    // v2.04
    LPC17xx_40xx_GPDMA_BurstRequest,
    LPC17xx_40xx_GPDMA_SingleRequest,
    LPC17xx_40xx_GPDMA_LastBurstRequest,
    LPC17xx_40xx_GPDMA_LastSingleRequest };

const HAL_GPDMA LPC17xx_40xx_GPDMA_Channel2_Interface = {
    {
        "GPDMA:LPC17xx_40xx:CH2",
        0x0100,
        LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel2,
        sizeof(T_LPC17xx_40xx_GPDMA_Workspace), },
    LPC17xx_40xx_GPDMA_PrepareRequest,
    LPC17xx_40xx_GPDMA_Start,
    LPC17xx_40xx_GPDMA_Stop,
    LPC17xx_40xx_GPDMA_IsValidAddress,

    // v2.04
    LPC17xx_40xx_GPDMA_BurstRequest,
    LPC17xx_40xx_GPDMA_SingleRequest,
    LPC17xx_40xx_GPDMA_LastBurstRequest,
    LPC17xx_40xx_GPDMA_LastSingleRequest };

const HAL_GPDMA LPC17xx_40xx_GPDMA_Channel3_Interface = {
    {
        "GPDMA:LPC17xx_40xx:CH3",
        0x0100,
        LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel3,
        sizeof(T_LPC17xx_40xx_GPDMA_Workspace), },
    LPC17xx_40xx_GPDMA_PrepareRequest,
    LPC17xx_40xx_GPDMA_Start,
    LPC17xx_40xx_GPDMA_Stop,
    LPC17xx_40xx_GPDMA_IsValidAddress,

    // v2.04
    LPC17xx_40xx_GPDMA_BurstRequest,
    LPC17xx_40xx_GPDMA_SingleRequest,
    LPC17xx_40xx_GPDMA_LastBurstRequest,
    LPC17xx_40xx_GPDMA_LastSingleRequest };

const HAL_GPDMA LPC17xx_40xx_GPDMA_Channel4_Interface = {
    {
        "GPDMA:LPC17xx_40xx:CH4",
        0x0100,
        LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel4,
        sizeof(T_LPC17xx_40xx_GPDMA_Workspace), },
    LPC17xx_40xx_GPDMA_PrepareRequest,
    LPC17xx_40xx_GPDMA_Start,
    LPC17xx_40xx_GPDMA_Stop,
    LPC17xx_40xx_GPDMA_IsValidAddress,

    // v2.04
    LPC17xx_40xx_GPDMA_BurstRequest,
    LPC17xx_40xx_GPDMA_SingleRequest,
    LPC17xx_40xx_GPDMA_LastBurstRequest,
    LPC17xx_40xx_GPDMA_LastSingleRequest };

const HAL_GPDMA LPC17xx_40xx_GPDMA_Channel5_Interface = {
    {
        "GPDMA:LPC17xx_40xx:CH5",
        0x0100,
        LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel5,
        sizeof(T_LPC17xx_40xx_GPDMA_Workspace), },
    LPC17xx_40xx_GPDMA_PrepareRequest,
    LPC17xx_40xx_GPDMA_Start,
    LPC17xx_40xx_GPDMA_Stop,
    LPC17xx_40xx_GPDMA_IsValidAddress,

    // v2.04
    LPC17xx_40xx_GPDMA_BurstRequest,
    LPC17xx_40xx_GPDMA_SingleRequest,
    LPC17xx_40xx_GPDMA_LastBurstRequest,
    LPC17xx_40xx_GPDMA_LastSingleRequest };

const HAL_GPDMA LPC17xx_40xx_GPDMA_Channel6_Interface = {
    {
        "GPDMA:LPC17xx_40xx:CH6",
        0x0100,
        LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel6,
        sizeof(T_LPC17xx_40xx_GPDMA_Workspace), },
    LPC17xx_40xx_GPDMA_PrepareRequest,
    LPC17xx_40xx_GPDMA_Start,
    LPC17xx_40xx_GPDMA_Stop,
    LPC17xx_40xx_GPDMA_IsValidAddress,

    // v2.04
    LPC17xx_40xx_GPDMA_BurstRequest,
    LPC17xx_40xx_GPDMA_SingleRequest,
    LPC17xx_40xx_GPDMA_LastBurstRequest,
    LPC17xx_40xx_GPDMA_LastSingleRequest };

const HAL_GPDMA LPC17xx_40xx_GPDMA_Channel7_Interface = {
    {
        "GPDMA:LPC17xx_40xx:CH7",
        0x0100,
        LPC17xx_40xx_GPDMA_InitializeWorkspace_Channel7,
        sizeof(T_LPC17xx_40xx_GPDMA_Workspace), },
    LPC17xx_40xx_GPDMA_PrepareRequest,
    LPC17xx_40xx_GPDMA_Start,
    LPC17xx_40xx_GPDMA_Stop,
    LPC17xx_40xx_GPDMA_IsValidAddress,

    // v2.04
    LPC17xx_40xx_GPDMA_BurstRequest,
    LPC17xx_40xx_GPDMA_SingleRequest,
    LPC17xx_40xx_GPDMA_LastBurstRequest,
    LPC17xx_40xx_GPDMA_LastSingleRequest };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_GPDMA0_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE()
    ;
    HALInterfaceRegister("GPDMA0",
        (T_halInterface *)&LPC17xx_40xx_GPDMA_Channel0_Interface, 0, 0);
}

void LPC17xx_40xx_GPDMA1_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE()
    ;
    HALInterfaceRegister("GPDMA1",
        (T_halInterface *)&LPC17xx_40xx_GPDMA_Channel1_Interface, 0, 0);
}

void LPC17xx_40xx_GPDMA2_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE()
    ;
    HALInterfaceRegister("GPDMA2",
        (T_halInterface *)&LPC17xx_40xx_GPDMA_Channel2_Interface, 0, 0);
}

void LPC17xx_40xx_GPDMA3_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE()
    ;
    HALInterfaceRegister("GPDMA3",
        (T_halInterface *)&LPC17xx_40xx_GPDMA_Channel3_Interface, 0, 0);
}

void LPC17xx_40xx_GPDMA4_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE()
    ;
    HALInterfaceRegister("GPDMA4",
        (T_halInterface *)&LPC17xx_40xx_GPDMA_Channel4_Interface, 0, 0);
}

void LPC17xx_40xx_GPDMA5_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE()
    ;
    HALInterfaceRegister("GPDMA5",
        (T_halInterface *)&LPC17xx_40xx_GPDMA_Channel5_Interface, 0, 0);
}

void LPC17xx_40xx_GPDMA6_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE()
    ;
    HALInterfaceRegister("GPDMA6",
        (T_halInterface *)&LPC17xx_40xx_GPDMA_Channel6_Interface, 0, 0);
}

void LPC17xx_40xx_GPDMA7_Require(void)
{
    HAL_DEVICE_REQUIRE_ONCE()
    ;
    HALInterfaceRegister("GPDMA7",
        (T_halInterface *)&LPC17xx_40xx_GPDMA_Channel7_Interface, 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  GPDMA.c
 *-------------------------------------------------------------------------*/
