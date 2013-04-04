/*-------------------------------------------------------------------------*
 * File:  LPC1788_USBHost.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1788 USBHost Interface as pulled
 *      from NXP's USB Host Stack.
 * Implementation:
 *      This implementation still assumes that there is one major form
 *      of communication needed.  Descriptors are described and allocated
 *      by the next level up.
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

/*---------------------------------------------------------------------------*
 * Include Files:
 *---------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <uEZ.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1788/LPC1788_USBHost.h>

/*---------------------------------------------------------------------------*
 * Registers:
 *---------------------------------------------------------------------------*/
#define TIMEOUT_STANDARD        10000   // 10 SECONDS

/*---------------------------------------------------------------------------*
 * Macros and Constants:
 *---------------------------------------------------------------------------*/

/* ------------------ HcControl Register ---------------------  */
#define  OR_CONTROL_CLE                 0x00000010
#define  OR_CONTROL_BLE                 0x00000020
#define  OR_CONTROL_HCFS                0x000000C0
#define  OR_CONTROL_HC_OPER             0x00000080
/* ----------------- HcCommandStatus Register ----------------- */
#define  OR_CMD_STATUS_HCR              0x00000001
#define  OR_CMD_STATUS_CLF              0x00000002
#define  OR_CMD_STATUS_BLF              0x00000004
/* --------------- HcInterruptStatus Register ----------------- */
#define  OR_INTR_STATUS_WDH             0x00000002
#define  OR_INTR_STATUS_RHSC            0x00000040
/* --------------- HcInterruptEnable Register ----------------- */
#define  OR_INTR_ENABLE_WDH             0x00000002
#define  OR_INTR_ENABLE_RHSC            0x00000040
#define  OR_INTR_ENABLE_MIE             0x80000000
/* ---------------- HcRhDescriptorA Register ------------------ */
#define  OR_RH_STATUS_LPSC              0x00010000
/* -------------- HcRhPortStatus[1:NDP] Register -------------- */
#define  OR_RH_PORT_CCS                 0x00000001
#define  OR_RH_PORT_PRS                 0x00000010
#define  OR_RH_PORT_CSC                 0x00010000
#define  OR_RH_PORT_PRSC                0x00100000

#define  FI                     0x2EDF           /* 12000 bits per frame (-1)                               */
#define  DEFAULT_FMINTERVAL     ((((6 * (FI - 210)) / 7) << 16) | FI)

// Transfer Descriptor control fields
#define  TD_ROUNDING        (TUInt32) (0x00040000)        /* Buffer Rounding                             */
#define  TD_SETUP           (TUInt32)(0)                  /* Direction of Setup Packet                   */
#define  TD_IN              (TUInt32)(0x00100000)         /* Direction In                                */
#define  TD_OUT             (TUInt32)(0x00080000)         /* Direction Out                               */
#define  TD_DELAY_INT(x)    (TUInt32)((x) << 21)          /* Delay Interrupt                             */
#define  TD_TOGGLE_0        (TUInt32)(0x02000000)         /* Toggle 0                                    */
#define  TD_TOGGLE_1        (TUInt32)(0x03000000)         /* Toggle 1                                    */
#define  TD_CC              (TUInt32)(0xF0000000)         /* Completion Code                             */

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#define  DESC_LENGTH(x)  x[0]
#define  DESC_TYPE(x)    x[1]


/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

typedef enum {
    USB_HOST_PORT_INDEX_PORT_A=0,
    USB_HOST_PORT_INDEX_PORT_B=1
} T_usbHostPortIndex;

typedef struct {
    const HAL_USBHost *iHAL;

    TUInt8 *iAllocBase;
    TUInt32 iAllocRemaining;
    TUInt8 *iResetAllocBase;
    TUInt32 iResetAllocRemaining;

    volatile TUInt32 *iPortStatus;
    T_usbHostPortIndex iPortIndex;
    volatile  T_usbHostDeviceState iState;

    // Callbacks
    const T_USBHostCallbacks *iCallbackAPI;
    void *iCallbackWorkspace;
} T_LPC1788_USBHost_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void IHost_TDInit (volatile  HCTD *td);
static void IHost_EDInit (volatile  HCED *aEndDesc);
static void IHost_HCCAInit (volatile  HCCA  *hcca);
static void *IAllocBuffer(
        T_LPC1788_USBHost_Workspace *p,
        TUInt32 aBufferSize);
static void  Host_DelayUS (TUInt32  delay);
void  WriteLE16U (volatile  TUInt8  *pmem,
                            TUInt16   val);
extern void UEZBSPDelayMS(unsigned int aMilliseconds);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
//volatile  TUInt32   HOST_RhscIntr = 0;         /* Root Hub Status Change interrupt                       */
static volatile  HCED      *EDCtrl;                    /* Control endpoint descriptor structure                  */
static volatile  HCED      *EDBulkIn;                  /* BulkIn endpoint descriptor  structure                  */
static volatile  HCED      *EDBulkOut;                 /* BulkOut endpoint descriptor structure                  */
static volatile  HCTD      *TDHead;                    /* Head transfer descriptor structure                     */
static volatile  HCTD      *TDTail;                    /* Tail transfer descriptor structure                     */
static volatile  HCCA      *Hcca;                      /* Host Controller Communications Area structure          */
static volatile  TUInt8  *TDBuffer;                  /* Current Buffer Pointer of transfer descriptor          */

static T_LPC1788_USBHost_Workspace *G_usbHostWorkspacePortA = 0;
static T_LPC1788_USBHost_Workspace *G_usbHostWorkspacePortB = 0;

/*
**************************************************************************************************************
*                                         DELAY IN MILLI SECONDS
*
* Description: This function provides a delay in milli seconds
*
* Arguments  : delay    The delay required
*
* Returns    : None
*
**************************************************************************************************************
*/

// TBD: What should we do with this routine, we're in a HAL, but later and need a good delay
// that is not RTOS dependentant!  Probably not good to use timer 3?
static void  Host_DelayMS (TUInt32  delay)
{
    volatile  TUInt32  i;


    for (i = 0; i < delay; i++) {
        Host_DelayUS(1000);
    }
}

/*
**************************************************************************************************************
*                                         DELAY IN MICRO SECONDS
*
* Description: This function provides a delay in micro seconds
*
* Arguments  : delay    The delay required
*
* Returns    : None
*
**************************************************************************************************************
*/

static void  Host_DelayUS (TUInt32  delay)
{
    volatile  TUInt32  i;
    /* This logic was tested. It gives app. 1 micro sec delay        */
    for (i = 0; i < (4 * delay); i++) {
        ;
    }
}

/*
**************************************************************************************************************
*                                         INTERRUPT SERVICE ROUTINE
*
* Description: This function services the interrupt caused by host controller
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/

static void Host_Isr_Process (T_LPC1788_USBHost_Workspace *p)
{
    TUInt32   int_status;
    TUInt32   ie_status;

    //if ((LPC_USB->InterruptStatus & 0x00000008) > 0) {
        int_status    = LPC_USB->HcInterruptStatus;                          /* Read Interrupt Status                */
        ie_status     = LPC_USB->HcInterruptEnable;                          /* Read Interrupt enable status         */

        if (!(int_status & ie_status)) {
            return;
        } else {
            int_status = int_status & ie_status;

            if (int_status & OR_INTR_STATUS_RHSC) {                 /* Root hub status change interrupt     */
                if (*(p->iPortStatus) & OR_RH_PORT_CSC) {             // ConnectStatusChange--> 0 = no change
                    if (*(p->iPortStatus) & OR_RH_PORT_CCS) {         // CurrentConnectStatus--> 1 = device connected
                        p->iState = USB_HOST_STATE_CONNECTED;

                        // Use connect callback (if one is available)
                        if (p->iCallbackAPI)
                            if (p->iCallbackAPI->Connected)
                                p->iCallbackAPI->Connected(p->iCallbackWorkspace);
                    } else {
                        p->iState = USB_HOST_STATE_DISCONNECTED;

                        // Use disconnect callback (if one is available)
                        if (p->iCallbackAPI)
                            if (p->iCallbackAPI->Disconnected)
                                p->iCallbackAPI->Disconnected(p->iCallbackWorkspace);
                    }
                    *(p->iPortStatus) = OR_RH_PORT_CSC;                  // clear CSC interrupt bit
                }
                if (*(p->iPortStatus) & OR_RH_PORT_PRSC) {           //PortResetStatusChange--> 1 = port reset complete
                    *(p->iPortStatus) = OR_RH_PORT_PRSC;
                }
            }
            if (int_status & OR_INTR_STATUS_WDH) {                  /* Writeback Done Head interrupt        */
                p->iCallbackAPI->FlagIRQEvent(p->iCallbackWorkspace);
            }
            LPC_USB->HcInterruptStatus = int_status;                         /* Clear interrupt status register      */
        }
    //}
}

void LPC1788_InterruptProcess_USBHost(void)
{
    if (G_usbHostWorkspacePortA)
        Host_Isr_Process(G_usbHostWorkspacePortA);
    if (G_usbHostWorkspacePortB)
        Host_Isr_Process(G_usbHostWorkspacePortB);
}

/*
**************************************************************************************************************
*                                          FILL SETUP PACKET
*
* Description: This function is used to fill the setup packet
*
* Arguments  : None
*
* Returns    : OK		              if Success
*              ERR_INVALID_BOOTSIG    if Failed
*
**************************************************************************************************************
*/

/*---------------------------------------------------------------------------*
 * Routine:  IHost_FillSetup
 *---------------------------------------------------------------------------*
 * Description:
 *      Fill a setup packet
 * Inputs:
 *      T_LPC1788_USBHost_Workspace *p -- USB Host controller workspace
 *      TUInt8 aBMRequestType       -- Type of request
 *      TUInt8 aRequest             -- Request
 *      TUInt16 aValue              -- Value for request
 *      TUInt16 aIndex              -- Index for request
 *      TUInt16 aLength             -- Length of data in command
 * Outputs:
 *      void *                      -- Returned allocation, or 0 for out
 *                                      of memory.
 *---------------------------------------------------------------------------*/
static void IHost_FillSetup(
        T_LPC1788_USBHost_Workspace *p,
        TUInt8   aBMRequestType,
        TUInt8   aRequest,
        TUInt16   aValue,
        TUInt16   aIndex,
        TUInt16   aLength)
{
    TDBuffer[0] = aBMRequestType;
    TDBuffer[1] = aRequest;
    WriteLE16U(&TDBuffer[2], aValue);
    WriteLE16U(&TDBuffer[4], aIndex);
    WriteLE16U(&TDBuffer[6], aLength);
}



/*
**************************************************************************************************************
*                                         INITIALIZE THE TRANSFER DESCRIPTOR
*
* Description: This function initializes transfer descriptor
*
* Arguments  : Pointer to TD structure
*
* Returns    : None
*
**************************************************************************************************************
*/
static void IHost_TDInit(volatile  HCTD *td)
{
    td->Control    = 0;
    td->CurrBufPtr = 0;
    td->Next       = 0;
    td->BufEnd     = 0;
}

/*
**************************************************************************************************************
*                                         INITIALIZE THE ENDPOINT DESCRIPTOR
*
* Description: This function initializes endpoint descriptor
*
* Arguments  : Pointer to ED strcuture
*
* Returns    : None
*
**************************************************************************************************************
*/
static void IHost_EDInit (volatile  HCED *aEndDesc)
{
    aEndDesc->Control = 0;
    aEndDesc->TailTd  = 0;
    aEndDesc->HeadTd  = 0;
    aEndDesc->Next    = 0;
}

/*
**************************************************************************************************************
*                                 INITIALIZE HOST CONTROLLER COMMUNICATIONS AREA
*
* Description: This function initializes host controller communications area
*
* Arguments  : Pointer to HCCA
*
* Returns    :
*
**************************************************************************************************************
*/
static void IHost_HCCAInit (volatile  HCCA  *hcca)
{
    TUInt32  i;

    for (i = 0; i < 32; i++) {
        hcca->IntTable[i] = 0;
        hcca->FrameNumber = 0;
        hcca->DoneHead    = 0;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  IAllocBuffer
 *---------------------------------------------------------------------------*
 * Description:
 *      Allocate data in the USB Data area (if room for allocation)
 * Inputs:
 *      T_LPC1788_USBHost_Workspace *p -- USB Host controller workspace
 *      TUInt32 aBufferSize         -- Number of bytes required
 * Outputs:
 *      void *                      -- Returned allocation, or 0 for out
 *                                      of memory.
 *---------------------------------------------------------------------------*/
static void *IAllocBuffer(
        T_LPC1788_USBHost_Workspace *p,
        TUInt32 aBufferSize)
{
    void *p_buffer = p->iAllocBase;

    // Determine if there is enough room
    if (p->iAllocRemaining < aBufferSize)
        return 0;

    // There is enough room
    // Move the base to the next location
    // and mark off the space used
    p->iAllocBase += aBufferSize;
    p->iAllocRemaining -= aBufferSize;

    // Return the original base address
    return p_buffer;
}

/*---------------------------------------------------------------------------*
 * Routine:  Host_AllocBuffer
 *---------------------------------------------------------------------------*
 * Description:
 *      Allocate USB memory.
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 *      TUInt32 aSize               -- Number of bytes required
 * Outputs:
 *      void *                      -- Returned allocation, or 0 for out
 *                                      of memory.
 *---------------------------------------------------------------------------*/
static void *Host_AllocBuffer(void *aWorkspace, TUInt32 aSize)
{
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;
    return IAllocBuffer(p, aSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_USBHost_InitializeWorkspace_PortA
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host to work on Port A
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 * Outputs:
 *      T_uezError                  -- Any error if failed
 *---------------------------------------------------------------------------*/
static T_uezError LPC1788_USBHost_InitializeWorkspace_PortA(void *aWorkspace)
{
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;
    p->iPortIndex = USB_HOST_PORT_INDEX_PORT_A;
    p->iPortStatus = (volatile TUInt32 *)&LPC_USB->HcRhPortStatus1;
    p->iAllocRemaining = 0; // Not ready yet
    p->iCallbackAPI = 0;
    p->iCallbackWorkspace = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_USBHost_InitializeWorkspace_PortB
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host to work on Port B
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 * Outputs:
 *      T_uezError                  -- Any error if failed
 *---------------------------------------------------------------------------*/
static T_uezError LPC1788_USBHost_InitializeWorkspace_PortB(void *aWorkspace)
{
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;
    p->iPortIndex = USB_HOST_PORT_INDEX_PORT_B;
    p->iPortStatus = (volatile TUInt32 *)&LPC_USB->HcRhPortStatus2;
    p->iAllocRemaining = 0; // Not ready yet
    p->iCallbackAPI = 0;
    p->iCallbackWorkspace = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Host_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initializes the LPC1788 host controller with buffers
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 *---------------------------------------------------------------------------*/
static T_uezError Host_Init(void *aWorkspace)
{
    extern void USBSharedInterruptSetup(void);
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    // Setup memory from start
    p->iAllocBase = (TUInt8 *)(AHBSRAM1_BASE+8192+2*1024);
    p->iAllocRemaining = 6*1024;

    // Ensure power to the USB is on
    LPC1788PowerOn(1UL<<31);

    // Enable the USB interrupt source
    if (p->iPortIndex == USB_HOST_PORT_INDEX_PORT_A) {
        LPC_USB->OTGClkCtrl = 0x0000001B;

        while ((LPC_USB->OTGClkSt & 0x0000001B) != 0x1B) {
            ;
        }

        LPC_USB->OTGStCtrl = 0x00000003;
    } else if (p->iPortIndex == USB_HOST_PORT_INDEX_PORT_B) {
        LPC_USB->OTGClkCtrl = 0x00000001;

        while ((LPC_USB->OTGClkSt & 0x00000001) == 0) {
            ;
        }
    }

    // Allocate standard buffers
    Hcca       = IAllocBuffer(p, sizeof(HCCA));
    TDHead     = IAllocBuffer(p, sizeof(HCTD));
    TDTail     = IAllocBuffer(p, sizeof(HCTD));
    EDCtrl     = IAllocBuffer(p, sizeof(HCED));
    EDBulkIn   = IAllocBuffer(p, sizeof(HCED));
    EDBulkOut  = IAllocBuffer(p, sizeof(HCED));
    TDBuffer   = IAllocBuffer(p, 128);

    // And then initialize those buffers
    IHost_EDInit(EDCtrl);
    IHost_EDInit(EDBulkIn);
    IHost_EDInit(EDBulkOut);
    IHost_TDInit(TDHead);
    IHost_TDInit(TDTail);
    IHost_HCCAInit(Hcca);

    // About to reset, first delay
    Host_DelayMS(50);
    LPC_USB->HcControl       = 0;
    LPC_USB->HcControlHeadED = 0;
    LPC_USB->HcBulkHeadED    = 0;

    // Now do the software reset and go back to the largest packet size
    LPC_USB->HcCommandStatus = OR_CMD_STATUS_HCR;
    LPC_USB->HcFmInterval    = DEFAULT_FMINTERVAL;

    // Put HC in operational state and set global power
    LPC_USB->HcControl  = (LPC_USB->HcControl & (~OR_CONTROL_HCFS)) | OR_CONTROL_HC_OPER;
    LPC_USB->HcRhStatus = OR_RH_STATUS_LPSC;
    LPC_USB->HcHCCA = (TUInt32)Hcca;

    // Remember the workspace for the interrupt routine
    // This also indirectly flags the interrupt routine by letting
    // the system know that we can now process interrupts.
    if (p->iPortIndex == USB_HOST_PORT_INDEX_PORT_A) {
        G_usbHostWorkspacePortA = p;
    } else {
        G_usbHostWorkspacePortB = p;
    }

    // Enable these interrupts
    LPC_USB->HcInterruptStatus |= LPC_USB->HcInterruptStatus;
    LPC_USB->HcInterruptEnable  = OR_INTR_ENABLE_MIE |
                         OR_INTR_ENABLE_WDH |
                         OR_INTR_ENABLE_RHSC;

    p->iState = USB_HOST_STATE_CLEAR;

    USBSharedInterruptSetup();

    // Remember the reset condition
    p->iResetAllocBase = p->iAllocBase;
    p->iResetAllocRemaining = p->iAllocRemaining;


    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Host_ResetBuffers
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the buffers back into a reset state
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 *---------------------------------------------------------------------------*/
static void Host_ResetBuffers(void *aWorkspace)
{
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    p->iAllocBase = p->iResetAllocBase;
    p->iAllocRemaining = p->iResetAllocRemaining;
}

/*---------------------------------------------------------------------------*
 * Routine:  Host_ProcessTD
 *---------------------------------------------------------------------------*
 * Description:
 *      Process a single transfer descriptor
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 *      volatile HCED *aEndDesc     -- Endpoint descriptor that contains this
 *                                      transfer descriptor.
 *      TUInt32 aToken              -- SETUP, IN, or OUT
 *      volatile TUInt8 *buffer     -- Current buffer pointer of the
 *                                      transfer descriptor.
 *      TUInt32 buffer_len          -- Length of the buffer.
 * Outputs:
 *      T_uezError                  -- UEZ_ERROR_NONE if successful, else
 *                                      error code.
 *---------------------------------------------------------------------------*/
static T_uezError Host_ProcessTD(
        void *aWorkspace,
        volatile HCED *aEndDesc,
        TUInt32 aToken,
        volatile TUInt8 *buffer,
        TUInt32 buffer_len,
        TUInt32 aTimeout)
{
    volatile TUInt32 td_toggle;
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    PARAM_NOT_USED(aTimeout);

    if (aEndDesc == EDCtrl) {
        if (aToken == TD_SETUP) {
            td_toggle = TD_TOGGLE_0;
        } else {
            td_toggle = TD_TOGGLE_1;
        }
    } else {
        td_toggle = 0;
    }

    // Setup a head and tail.  The tail points to nothing.
    // The head points ot the descriptor being sent.
    TDHead->Control = (TD_ROUNDING    |
                      aToken          |
                      TD_DELAY_INT(0) |
                      td_toggle       |
                      TD_CC);
    TDTail->Control = 0;
    TDHead->CurrBufPtr = (TUInt32) buffer;
    TDTail->CurrBufPtr = 0;
    TDHead->Next = (TUInt32) TDTail;
    TDTail->Next = 0;
    TDHead->BufEnd = (TUInt32)(buffer + (buffer_len - 1));
    TDTail->BufEnd = 0;

    // Setup the endpoint descriptor
    aEndDesc->HeadTd  = (TUInt32)TDHead | ((aEndDesc->HeadTd) & 0x00000002);
    aEndDesc->TailTd  = (TUInt32)TDTail;
    aEndDesc->Next    = 0;

    // Are we a control endpoint or a bulk endpoint?
    if (aEndDesc == EDCtrl) {
        // If control, setup for control transfer
        LPC_USB->HcControlHeadED = (TUInt32)aEndDesc;
        LPC_USB->HcCommandStatus = LPC_USB->HcCommandStatus | OR_CMD_STATUS_CLF;
        LPC_USB->HcControl       = LPC_USB->HcControl       | OR_CONTROL_CLE;
    } else {
        // Otherwise, setup for bulk transfer
        LPC_USB->HcBulkHeadED    = (TUInt32)aEndDesc;
        LPC_USB->HcCommandStatus = LPC_USB->HcCommandStatus | OR_CMD_STATUS_BLF;
        LPC_USB->HcControl       = LPC_USB->HcControl       | OR_CONTROL_BLE;
    }

    // Wait for transfer to complete
    if (p->iCallbackAPI->WaitForIRQEvent(p->iCallbackWorkspace, aTimeout)
        == UEZ_ERROR_TIMEOUT)
        return UEZ_ERROR_TIMEOUT;

    // Check if we had a failure.
    if (!(TDHead->Control & 0xF0000000)) {
        return (UEZ_ERROR_NONE);
    } else if (TDHead->Control & 0x40000000) {
        return (UEZ_ERROR_STALL);
    } else {
        // We're currently not returning error codes, just return
        // an unknown code.
        return (UEZ_ERROR_UNKNOWN);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Host_CtrlSend
 *---------------------------------------------------------------------------*
 * Description:
 *      Send control information.  Data in the common transfer descriptor
 *      is sent (if aLength > 0).
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 *      TUInt8 aBMRequestType       -- Type of request
 *      TUInt8 aRequest             -- Request
 *      TUInt16 aValue              -- Value for request
 *      TUInt16 aIndex              -- Index for request
 *      TUInt16 aLength             -- Length of data in command
 *                                      (currently in TDBuffer)
 * Outputs:
 *      T_uezError                  -- UEZ_ERROR_NONE if successful, else
 *                                      error code.
 *---------------------------------------------------------------------------*/
static T_uezError  Host_CtrlSend(
            void *aWorkspace,
            void *aBuffer,
            TUInt8 aDeviceAddress,
            TUInt8 aBMRequestType,
            TUInt8 aRequest,
            TUInt16 aValue,
            TUInt16 aIndex,
            TUInt16 aLength,
            TUInt32 aTimeout)
{
    T_uezError error;
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    IHost_FillSetup(p, aBMRequestType, aRequest, aValue, aIndex, aLength);
    error = Host_ProcessTD(aWorkspace, EDCtrl, TD_SETUP, TDBuffer, 8, aTimeout);
    if (error == UEZ_ERROR_NONE) {
        if (aLength)
            error = Host_ProcessTD(aWorkspace, EDCtrl, TD_OUT, aBuffer, aLength, aTimeout);
        if (error == UEZ_ERROR_NONE)
            error = Host_ProcessTD(aWorkspace, EDCtrl, TD_IN, 0, 0, aTimeout);
    }
    return (error);
}

T_uezError Host_SetAddress(void *aWorkspace, TUInt8 aAddress)
{
    T_uezError error;

    error = Host_CtrlSend(
            aWorkspace,
            0,
            0,
            USB_HOST_TO_DEVICE | USB_RECIPIENT_DEVICE,
            SET_ADDRESS,
            aAddress,
            0,
            0,
            TIMEOUT_STANDARD);

    if (error != UEZ_ERROR_NONE)
        return error;

    // Wait a little bit (2 ms)
    UEZBSPDelayMS(2);

    // Change the control to go to the address
    EDCtrl->Control = (EDCtrl->Control & (~0x7F)) | aAddress;

    return error;
}


/*---------------------------------------------------------------------------*
 * Routine:  Host_CtrlRecv
 *---------------------------------------------------------------------------*
 * Description:
 *      Receive control information.  Data is received in the common
 *      transfer descriptor (if aLength > 0).
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 *      TUInt8 aBMRequestType       -- Type of request
 *      TUInt8 aRequest             -- Request
 *      TUInt16 aValue              -- Value for request
 *      TUInt16 aIndex              -- Index for request
 *      TUInt16 aLength             -- Length of data in command
 *                                      (currently in TDBuffer)
 * Outputs:
 *      T_uezError                  -- UEZ_ERROR_NONE if successful, else
 *                                      error code.
 *---------------------------------------------------------------------------*/
static T_uezError Host_CtrlRecv(
            void *aWorkspace,
            void *aBuffer,
            TUInt8 aDeviceAddress,
            TUInt8 aBMRequestType,
            TUInt8 aRequest,
            TUInt16 aValue,
            TUInt16 aIndex,
            TUInt32 aLength,
            TUInt32 aTimeout)
{
    T_uezError error;
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    IHost_FillSetup(p, aBMRequestType, aRequest, aValue, aIndex, aLength);
    error = Host_ProcessTD(aWorkspace, EDCtrl, TD_SETUP, TDBuffer, 8, aTimeout);
    if (error == UEZ_ERROR_NONE) {
        // Get the data (if any)
        if (aBuffer)
            error = Host_ProcessTD(aWorkspace, EDCtrl, TD_IN, aBuffer, aLength, aTimeout);

        // Send an ACK
        if (error == UEZ_ERROR_NONE)
            error = Host_ProcessTD(aWorkspace, EDCtrl, TD_OUT, 0, 0, aTimeout);
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Host_SetCallbacks
 *---------------------------------------------------------------------------*
 * Description:
 *      Declare callbacks for this workspace.
 * Inputs:
 *      void *aWorkspace            -- USB Host controller workspace
 *      void *aCallbackWorkspace    -- Workspace used when call back called
 *      const T_USBHostCallbacks *aCallbackAPI -- Structure holding callback
 *                                      routines to be called.
 * Outputs:
 *      T_uezError                  -- UEZ_ERROR_NONE if successful, else
 *                                      error code.
 *---------------------------------------------------------------------------*/
static T_uezError Host_SetCallbacks(
        void *aWorkspace,
        void *aCallbackWorkspace,
        const T_USBHostCallbacks *aCallbackAPI)
{
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    p->iCallbackAPI = aCallbackAPI;
    p->iCallbackWorkspace = aCallbackWorkspace;

    return UEZ_ERROR_NONE;
}

static T_usbHostDeviceState Host_GetState(void *aWorkspace)
{
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;
    return p->iState;
}

static void Host_ResetPort(void *aWorkspace)
{
    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    UEZTaskDelay(100);

    // Apply port reset
    *p->iPortStatus |= OR_RH_PORT_PRS;

    // Wait for 100 MS after port reset
    UEZTaskDelay(100);

    // Default max packet size to 8 to ensure compatibility
    EDCtrl->Control = (8 << 16);
}

T_uezError Host_Control(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aBMRequestType,
            TUInt8 aRequest,
            TUInt16 aValue,
            TUInt16 aIndex,
            TUInt16 aLength,
            void *aBuffer,
            TUInt32 aTimeout)
{
    return Host_CtrlRecv(
            aWorkspace,
            aBuffer,
            aDeviceAddress,
            aBMRequestType,
            aRequest,
            aValue,
            aIndex,
            aLength,
            aTimeout);
}

T_uezError Host_GetDescriptor(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aType,
            TUInt8 aIndex,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    return Host_CtrlRecv(
        aWorkspace,
        aBuffer,
        aDeviceAddress,
        USB_DEVICE_TO_HOST | USB_RECIPIENT_DEVICE,
        GET_DESCRIPTOR,
        (aType << 8)|(aIndex),
        0,
        aSize,
        aTimeout);
}

T_uezError Host_GetString(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt32 aIndex,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    return Host_CtrlRecv(
        aWorkspace,
        aBuffer,
        aDeviceAddress,
        USB_DEVICE_TO_HOST | USB_RECIPIENT_DEVICE,
        GET_DESCRIPTOR,
        (3 << 8)|(aIndex),
        0,
        aSize,
        aTimeout);
}

T_uezError Host_BulkOut(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    T_uezError error;
    volatile HCED *hced = EDBulkOut;
//    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    // Configure the bulk end point
    // with the proper end point and address
    hced->Control =
        (hced->Control & ~((0x7F<<0)|(0x7F<<7))) |
        (aDeviceAddress << 0) |
        ((aEndpoint & 0x7F) << 7);

    // Now process that end point
    error = Host_ProcessTD(aWorkspace, hced, TD_OUT, aBuffer, aSize, aTimeout);

    return error;
}

T_uezError Host_BulkIn(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    T_uezError error;
    volatile HCED *hced = EDBulkIn;
//    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    // Configure the bulk end point
    // with the proper end point and address
    hced->Control =
        (hced->Control & ~((0x7F<<0)|(0x7F<<7))) |
        (aDeviceAddress << 0) |
        ((aEndpoint & 0x7F) << 7);

    // Now process that end point
    error = Host_ProcessTD(aWorkspace, hced, TD_IN, aBuffer, aSize, aTimeout);

    return error;
}

T_uezError Host_InterruptOut(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            const TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    // TBD:
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError Host_InterruptIn(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt8 aEndpoint,
            TUInt8 *aBuffer,
            TUInt32 aSize,
            TUInt32 aTimeout)
{
    // TBD:
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError Host_SetConfiguration(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt32 aConfigurationNum)
{
    return Host_CtrlSend(
            aWorkspace,
            0,
            aDeviceAddress,
            USB_HOST_TO_DEVICE | USB_RECIPIENT_DEVICE,
            SET_CONFIGURATION,
            aConfigurationNum,
            0,
            0,
            TIMEOUT_STANDARD);
}

T_uezError Host_SetControlPacketSize(
            void *aWorkspace,
            TUInt8 aDeviceAddress,
            TUInt16 aPacketSize)
{
    PARAM_NOT_USED(aWorkspace);

    if (aPacketSize > 256)
        return UEZ_ERROR_ILLEGAL_PARAMETER;

    EDCtrl->Control = (aPacketSize << 16);

    return UEZ_ERROR_NONE;
}

T_uezError Host_ConfigureEndpoint(
        void *aWorkspace,
        TUInt8 aDeviceAddress,
        TUInt8 aEndpointAndInOut,   // bit 0x80 is set if IN
        TUInt16 aMaxPacketSize)
{
//    T_LPC1788_USBHost_Workspace *p = (T_LPC1788_USBHost_Workspace *)aWorkspace;

    // For now, we'll set the one endpoint
    if (aEndpointAndInOut & 0x80) {
        EDBulkIn->Control =
            aDeviceAddress | // USB Address
            ((aEndpointAndInOut & 0x7F) << 7) | // Endpoint address
            (2 << 11) | // direction
            (aMaxPacketSize << 16);
    } else {
        EDBulkOut->Control =
            aDeviceAddress | // USB Address
            ((aEndpointAndInOut & 0x7F) << 7) | // Endpoint address
            (1 << 11) | // direction
            (aMaxPacketSize << 16);
    }

    return UEZ_ERROR_NONE;
}


const HAL_USBHost G_LPC1788_USBHost_Interface_PortA = {
    // T_halInterface iInterface
    {
        "LPC1788:USBHost:PortA",
        0x0100,
        LPC1788_USBHost_InitializeWorkspace_PortA,
        sizeof(T_LPC1788_USBHost_Workspace),
    },

    Host_SetCallbacks,
    Host_Init,
    Host_ResetPort,
    Host_ResetBuffers,
    Host_AllocBuffer,
    Host_ProcessTD,
    Host_GetState,

    Host_SetControlPacketSize,
    Host_SetAddress,
    Host_ConfigureEndpoint,
    Host_SetConfiguration,

    Host_Control,
    Host_GetDescriptor,
    Host_GetString,
    Host_BulkOut,
    Host_BulkIn,
    Host_InterruptOut,
    Host_InterruptIn,
};


const HAL_USBHost G_LPC1788_USBHost_Interface_PortB = {
    // T_halInterface iInterface
    {
        "LPC1788:USBHost:PortB",
        0x0100,
        LPC1788_USBHost_InitializeWorkspace_PortB,
        sizeof(T_LPC1788_USBHost_Workspace),
    },

    Host_SetCallbacks,
    Host_Init,
    Host_ResetPort,
    Host_ResetBuffers,
    Host_AllocBuffer,
    Host_ProcessTD,
    Host_GetState,

    Host_SetControlPacketSize,
    Host_SetAddress,
    Host_ConfigureEndpoint,
    Host_SetConfiguration,

    Host_Control,
    Host_GetDescriptor,
    Host_GetString,
    Host_BulkOut,
    Host_BulkIn,
    Host_InterruptOut,
    Host_InterruptIn,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
//void LPC1788_USBHost_VBUS_Require(T_uezGPIOPortPin aPin)
//{
//    static const T_LPC1788_IOCON_ConfigList pins[] = {
//            {GPIO_P1_30, IOCON_A_DEFAULT(2)}, // USB_VBUS
//    };
//    HAL_DEVICE_REQUIRE_ONCE();
//    LPC1788_IOCON_ConfigPinOrNone(aPin, pins, ARRAY_COUNT(pins));
//
//}
void LPC1788_USBHost_PortA_Require(const T_LPC1788_USBHost_Settings *aSettings)
{
    static const T_LPC1788_IOCON_ConfigList pinsDP1[] = {
            {GPIO_P0_29, IOCON_U_DEFAULT(1)}, // USB_D+1
    };
    static const T_LPC1788_IOCON_ConfigList pinsDM1[] = {
            {GPIO_P0_30, IOCON_U_DEFAULT(1)}, // USB_D-1
    };
    static const T_LPC1788_IOCON_ConfigList pinsCONNECT1[] = {
            {GPIO_P2_9,  IOCON_D_DEFAULT(1)}, // USB_CONNECT1
    };
    static const T_LPC1788_IOCON_ConfigList pinsUP_LED1[] = {
            {GPIO_P1_18, IOCON_D_DEFAULT(1)}, // USB_UP_LED1
    };
    static const T_LPC1788_IOCON_ConfigList pinsINT1[] = {
            {GPIO_P1_27, IOCON_D_DEFAULT(1)}, // USB_INT1n
    };
    static const T_LPC1788_IOCON_ConfigList pinsSCL1[] = {
            {GPIO_P1_28, IOCON_D_DEFAULT(1)}, // USB_SCL1
            {GPIO_P0_28, IOCON_I_DEFAULT(2)}, // USB_SCL1
    };
    static const T_LPC1788_IOCON_ConfigList pinsSDA1[] = {
            {GPIO_P1_29, IOCON_D_DEFAULT(1)}, // USB_SDA1
            {GPIO_P0_27, IOCON_I_DEFAULT(2)}, // USB_SDA1
    };
    static const T_LPC1788_IOCON_ConfigList pinsTX_E1[] = {
            {GPIO_P1_19, IOCON_D_DEFAULT(1)}, // USB_TX_E1
    };
    static const T_LPC1788_IOCON_ConfigList pinsTX_DP1[] = {
            {GPIO_P1_20, IOCON_D_DEFAULT(1)}, // USB_TX_DP1
    };
    static const T_LPC1788_IOCON_ConfigList pinsTX_DM1[] = {
            {GPIO_P1_21, IOCON_D_DEFAULT(1)}, // USB_TX_DM1
    };
    static const T_LPC1788_IOCON_ConfigList pinsRCV1[] = {
            {GPIO_P1_22, IOCON_D_DEFAULT(1)}, // USB_RCV1
    };
    static const T_LPC1788_IOCON_ConfigList pinsRX_DP1[] = {
            {GPIO_P1_23, IOCON_D_DEFAULT(1)}, // USB_RX_DP1
    };
    static const T_LPC1788_IOCON_ConfigList pinsRX_DM1[] = {
            {GPIO_P1_24, IOCON_D_DEFAULT(1)}, // USB_RX_DM1
    };
    static const T_LPC1788_IOCON_ConfigList pinsRX_LS1[] = {
            {GPIO_P1_25, IOCON_D_DEFAULT(1)}, // USB_LS1n
    };
    static const T_LPC1788_IOCON_ConfigList pinsSSPND1[] = {
            {GPIO_P1_26, IOCON_D_DEFAULT(1)}, // USB_SSPND1n
    };
    static const T_LPC1788_IOCON_ConfigList pinsPPWR1[] = {
            {GPIO_P1_19, IOCON_D_DEFAULT(2)}, // USB_PPWR1n
    };
    static const T_LPC1788_IOCON_ConfigList pinsPWRD1[] = {
            {GPIO_P1_22, IOCON_D_DEFAULT(2)}, // USB_PWRD1
    };
    static const T_LPC1788_IOCON_ConfigList pinsOVRCR1[] = {
            {GPIO_P1_22, IOCON_D_DEFAULT(2)}, // USB_OVRCR1n
            {GPIO_P1_27, IOCON_D_DEFAULT(2)}, // USB_OVRCR1n
    };
    static const T_LPC1788_IOCON_ConfigList pinsHSTEN1[] = {
            {GPIO_P1_25, IOCON_D_DEFAULT(2)}, // USB_HSTEN1n
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register USB Host driver ports for LPC24xx
    HALInterfaceRegister("USBHost:PortA",
            (T_halInterface *)&G_LPC1788_USBHost_Interface_PortA, 0, 0);
//    LPC1788_USBHost_VBUS_Require(aSettings->iVBUS);
    LPC1788_IOCON_ConfigPin(aSettings->iDP, pinsDP1, ARRAY_COUNT(pinsDP1));
    LPC1788_IOCON_ConfigPin(aSettings->iDM, pinsDM1, ARRAY_COUNT(pinsDM1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iCONNECT, pinsCONNECT1,
            ARRAY_COUNT(pinsCONNECT1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iUP_LED, pinsUP_LED1,
            ARRAY_COUNT(pinsUP_LED1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iINTn, pinsINT1,
            ARRAY_COUNT(pinsINT1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iSCL, pinsSCL1,
            ARRAY_COUNT(pinsSCL1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iSDA, pinsSDA1,
            ARRAY_COUNT(pinsSDA1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iTX_E, pinsTX_E1,
            ARRAY_COUNT(pinsTX_E1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iTX_DP, pinsTX_DP1,
            ARRAY_COUNT(pinsTX_DP1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iTX_DM, pinsTX_DM1,
            ARRAY_COUNT(pinsTX_DM1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iRCV, pinsRCV1,
            ARRAY_COUNT(pinsRCV1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iRX_DP, pinsRX_DP1,
            ARRAY_COUNT(pinsRX_DP1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iRX_DM, pinsRX_DM1,
            ARRAY_COUNT(pinsRX_DM1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iLSn, pinsRX_LS1,
            ARRAY_COUNT(pinsRX_LS1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iSSPNDn, pinsSSPND1,
            ARRAY_COUNT(pinsSSPND1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iPPWRn, pinsPPWR1,
            ARRAY_COUNT(pinsPPWR1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iPWRDn, pinsPWRD1,
            ARRAY_COUNT(pinsPWRD1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iOVRCRn, pinsOVRCR1,
            ARRAY_COUNT(pinsOVRCR1));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iHSTENn, pinsHSTEN1,
            ARRAY_COUNT(pinsHSTEN1));
}

void LPC1788_USBHost_PortB_Require(const T_LPC1788_USBHost_Settings *aSettings)
{
    static const T_LPC1788_IOCON_ConfigList pinsDP2[] = {
            {GPIO_P0_31, IOCON_U_DEFAULT(1)}, // USB_D+2
    };
    static const T_LPC1788_IOCON_ConfigList pinsCONNECT2[] = {
            {GPIO_P0_14, IOCON_D_DEFAULT(3)}, // USB_CONNECT2
    };
    static const T_LPC1788_IOCON_ConfigList pinsUP_LED2[] = {
            {GPIO_P0_13, IOCON_A_DEFAULT(1)}, // USB_UP_LED2
    };
    static const T_LPC1788_IOCON_ConfigList pinsPPWR2[] = {
            {GPIO_P0_12, IOCON_A_DEFAULT(1)}, // USB_PPWR2n
    };
    static const T_LPC1788_IOCON_ConfigList pinsPWRD2[] = {
            {GPIO_P1_30, IOCON_A_DEFAULT(1)}, // USB_PWRD2
    };
    static const T_LPC1788_IOCON_ConfigList pinsOVRCR2[] = {
            {GPIO_P1_31, IOCON_A_DEFAULT(1)}, // USB_OVRCR2n
    };
    static const T_LPC1788_IOCON_ConfigList pinsHSTEN2[] = {
            {GPIO_P0_14, IOCON_D_DEFAULT(1)}, // USB_HSTEN2n
    };
    HAL_DEVICE_REQUIRE_ONCE();
    // Register USB Host driver ports for LPC24xx
    HALInterfaceRegister("USBHost:PortB",
            (T_halInterface *)&G_LPC1788_USBHost_Interface_PortB, 0, 0);

//    LPC1788_USBHost_VBUS_Require(aSettings->iVBUS);
    LPC1788_IOCON_ConfigPin(aSettings->iDP, pinsDP2, ARRAY_COUNT(pinsDP2));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iDM, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iCONNECT, pinsCONNECT2,
            ARRAY_COUNT(pinsCONNECT2));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iUP_LED, pinsUP_LED2,
            ARRAY_COUNT(pinsUP_LED2));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iINTn, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iSCL, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iSDA, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iTX_E, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iTX_DP, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iTX_DM, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iRCV, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iRX_DP, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iRX_DM, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iLSn, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iSSPNDn, 0, 0);
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iPPWRn, pinsPPWR2,
            ARRAY_COUNT(pinsPPWR2));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iPWRDn, pinsPWRD2,
            ARRAY_COUNT(pinsPWRD2));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iOVRCRn, pinsOVRCR2,
            ARRAY_COUNT(pinsOVRCR2));
    LPC1788_IOCON_ConfigPinOrNone(aSettings->iHSTENn, pinsHSTEN2,
            ARRAY_COUNT(pinsHSTEN2));
}
/*-------------------------------------------------------------------------*
 * End of File:  LPC1788_USBHost.c
 *-------------------------------------------------------------------------*/
