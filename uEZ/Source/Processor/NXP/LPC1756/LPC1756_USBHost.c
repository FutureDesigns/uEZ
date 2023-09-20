/*-------------------------------------------------------------------------*
 * File:  LPC1756_USBHost.c
 *-------------------------------------------------------------------------*
 * Description:
 *      <DESCRIPTION>
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
/*
 *  @{
 *  @brief     uEZ USB Host Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    HAL implementation of the LPC1756 USBHost Interface as pulled
 *      from NXP's USB Host Stack.
 * Implementation:
 *      This implementation still assumes that there is one major form
 *      of communication needed.  Descriptors are described and allocated
 *      by the next level up.
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
#include <Config.h>

/*---------------------------------------------------------------------------*
 * Include Files:
 *---------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_USBHost.h>

/*---------------------------------------------------------------------------*
 * Registers:
 *---------------------------------------------------------------------------*/
#define USB_INT_BASE_ADDR	0xE01FC1C0
#define USB_INT_STAT    (SC->USBIntSt)

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

typedef struct {
    const HAL_USBHost *iHAL;

    TUInt8 *iAllocBase;
    TUInt32 iAllocRemaining;
    TUInt8 *iResetAllocBase;
    TUInt32 iResetAllocRemaining;

    volatile TUInt32 *iPortStatus;
    volatile T_usbHostDeviceState iState;

    // Callbacks
    const T_USBHostCallbacks *iCallbackAPI;
    void *iCallbackWorkspace;
} T_LPC1756_USBHost_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void IHost_TDInit (volatile  HCTD *td);
static void IHost_EDInit (volatile  HCED *aEndDesc);
static void IHost_HCCAInit (volatile  HCCA  *hcca);
static void *IAllocBuffer(
        T_LPC1756_USBHost_Workspace *p,
        TUInt32 aBufferSize);
static void  Host_DelayUS (TUInt32  delay);
void  WriteLE16U (volatile  TUInt8  *pmem,
                            TUInt16   val);
extern void UEZBSPDelayMS(uint32_t aMilliseconds);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
//volatile  TUInt32   HOST_RhscIntr = 0;         /* Root Hub Status Change interrupt                       */
static volatile  TUInt32   HOST_WdhIntr  = 0;         /* Semaphore to wait until the TD is submitted            */
static volatile  HCED      *EDCtrl;                    /* Control endpoint descriptor structure                  */
static volatile  HCED      *EDBulkIn;                  /* BulkIn endpoint descriptor  structure                  */
static volatile  HCED      *EDBulkOut;                 /* BulkOut endpoint descriptor structure                  */
static volatile  HCTD      *TDHead;                    /* Head transfer descriptor structure                     */
static volatile  HCTD      *TDTail;                    /* Tail transfer descriptor structure                     */
static volatile  HCCA      *Hcca;                      /* Host Controller Communications Area structure          */
static volatile  TUInt8  *TDBuffer;                  /* Current Buffer Pointer of transfer descriptor          */
//static volatile  TUInt8  *UsbXferBuffer;             /* Buffer used by application                             */

//static int32_t G_usbHostInit = 0;
static T_LPC1756_USBHost_Workspace *G_usbHostWorkspace = 0;

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

static void Host_Isr_Process (T_LPC1756_USBHost_Workspace *p)
{
    TUInt32   int_status;
    TUInt32   ie_status;

    if ((USB_INT_STAT & 0x00000008) > 0) {
        int_status    = USB->HcInterruptStatus;                          /* Read Interrupt Status                */
        ie_status     = USB->HcInterruptEnable;                          /* Read Interrupt enable status         */

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
                HOST_WdhIntr = 1;
            }
            USB->HcInterruptStatus = int_status;                         /* Clear interrupt status register      */
        }
    }
}

void LPC1756_InterruptProcess_USBHost(void)
{
    if (G_usbHostWorkspace)
        Host_Isr_Process(G_usbHostWorkspace);
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
 *---------------------------------------------------------------------------*/
/**
 *  Fill a setup packet
 *
 *  @param [in]    *p			   USB Host controller workspace
 *
 *  @param [in]    aBMRequestType  Type of request
 *
 *  @param [in]    aRequest        Request
 *
 *  @param [in]    aValue          Value for request
 *
 *  @param [in]    aIndex          Index for request
 *
 *  @param [in]    aLength         Length of data in command
 *
 *  @return        *void           Returned allocation, or 0 for out of memory.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void IHost_FillSetup(
        T_LPC1756_USBHost_Workspace *p,
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

/*
**************************************************************************************************************
*                                         WAIT FOR WDH INTERRUPT
*
* Description: This function is infinite loop which breaks when ever a WDH interrupt rises
*
* Arguments  : None
*
* Returns    : None
*
**************************************************************************************************************
*/
#if 0
static T_uezError IHost_WDHWait (TUInt32 aTimeout)
{
#if 0
    T_uezError error = UEZ_ERROR_NONE;
    while ((!HOST_WdhIntr) && (aTimeout > 10)) {
        UEZBSPDelayMS(10);
        if (aTimeout > 10) {
            aTimeout -= 10;
        } else {
            error = UEZ_ERROR_TIMEOUT;
            break;
        }
    }
    HOST_WdhIntr = 0;
    return error;
#else
    while ((!HOST_WdhIntr) && (p->iState == USB_HOST_STATE_CONNECTED)) {
    }
    HOST_WdhIntr = 0;
    return UEZ_ERROR_NONE;
#endif
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  IAllocBuffer
 *---------------------------------------------------------------------------*/
/**
 *  Allocate data in the USB Data area (if room for allocation)
 *
 *  @param [in]    *p 			   USB Host controller workspace
 *
 *  @param [in]    aBufferSize     Number of bytes required
 *
 *  @return        *void           Returned allocation, or 0 for out of memory.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void *IAllocBuffer(
        T_LPC1756_USBHost_Workspace *p,
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
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;
    return IAllocBuffer(p, aSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_USBHost_InitializeWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Setup the USB Host to work on Port A
 *
 *  @param [in]    *aWorkspace       USB Host controller workspace
 *
 *  @param [in]    T_uezError        Any error if failed
 *=
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError LPC1756_USBHost_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;
    p->iPortStatus = (volatile TUInt32 *)&USB->HcRhPortStatus1;
    p->iAllocRemaining = 0; // Not ready yet
    p->iCallbackAPI = 0;
    p->iCallbackWorkspace = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Host_Init
 *---------------------------------------------------------------------------*/
/**
 *  Initializes the LPC1756 host controller with buffers
 *
 *  @param [in]    *aWorkspace   	USB Host controller workspace
 *
 *  @return        T_uezError    	Any error if failed
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError Host_Init(void *aWorkspace)
{
    extern void USBSharedInterruptSetup(void);
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

    // Setup memory from start
    p->iAllocBase = (TUInt8 *)(AHBSRAM0_BASE+8192+2*1024);
    p->iAllocRemaining = 6*1024;

    // Ensure power to the USB is on
    SC->PCONP         |= (1UL<<31);

    // Enable the USB interrupt source
    USB->u1.OTGClkCtrl = 0x0000001B;

    while ((USB->u2.OTGClkSt & 0x0000001B) != 0x1B) {
        ;
    }

    USB->OTGStCtrl = 0x00000003;

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
    USB->HcControl       = 0;
    USB->HcControlHeadED = 0;
    USB->HcBulkHeadED    = 0;

    // Now do the software reset and go back to the largest packet size
    USB->HcCommandStatus = OR_CMD_STATUS_HCR;
    USB->HcFmInterval    = DEFAULT_FMINTERVAL;

    // Put HC in operational state and set global power
    USB->HcControl  = (USB->HcControl & (~OR_CONTROL_HCFS)) | OR_CONTROL_HC_OPER;
    USB->HcRhStatus = OR_RH_STATUS_LPSC;
    USB->HcHCCA = (TUInt32)Hcca;

    // Remember the workspace for the interrupt routine
    // This also indirectly flags the interrupt routine by letting
    // the system know that we can now process interrupts.
    G_usbHostWorkspace = p;

    // Enable these interrupts
    USB->HcInterruptStatus |= USB->HcInterruptStatus;
    USB->HcInterruptEnable  = OR_INTR_ENABLE_MIE |
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
 *---------------------------------------------------------------------------*/
/**
 *  Put the buffers back into a reset state
 *
 *  @param [in]    *aWorkspace 	USB Host controller workspace
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void Host_ResetBuffers(void *aWorkspace)
{
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

    p->iAllocBase = p->iResetAllocBase;
    p->iAllocRemaining = p->iResetAllocRemaining;
}

/*---------------------------------------------------------------------------*
 * Routine:  Host_ProcessTD
 *---------------------------------------------------------------------------*/
/**
 *  Process a single transfer descriptor
 *
 *  @param [in]    *aWorkspace    	USB Host controller workspace
 *
 *  @param [in]    *aEndDesc     	Endpoint descriptor that contains this
 *                                  transfer descriptor.
 *  @param [in]    aToken      		SETUP, IN, or OUT
 *
 *  @param [in]    *buffer    		Current buffer pointer of the
 *                                 	transfer descriptor.
 *  @param [in]    buffer_len   	Length of the buffer.
 *
 *  @return        T_uezError    	UEZ_ERROR_NONE if successful, else
 *                                  error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError Host_ProcessTD(
        void *aWorkspace,
        volatile HCED *aEndDesc,
        TUInt32 aToken,
        volatile TUInt8 *buffer,
        TUInt32 buffer_len,
        TUInt32 aTimeout)
{
    volatile TUInt32 td_toggle;
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

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
        USB->HcControlHeadED = (TUInt32)aEndDesc;
        USB->HcCommandStatus = USB->HcCommandStatus | OR_CMD_STATUS_CLF;
        USB->HcControl       = USB->HcControl       | OR_CONTROL_CLE;
    } else {
        // Otherwise, setup for bulk transfer
        USB->HcBulkHeadED    = (TUInt32)aEndDesc;
        USB->HcCommandStatus = USB->HcCommandStatus | OR_CMD_STATUS_BLF;
        USB->HcControl       = USB->HcControl       | OR_CONTROL_BLE;
    }

    // Wait for transfer to complete
    while ((!HOST_WdhIntr) && (p->iState == USB_HOST_STATE_CONNECTED)) {
    }
    HOST_WdhIntr = 0;

//    if (IHost_WDHWait(aTimeout))
//        return UEZ_ERROR_TIMEOUT;

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
 *---------------------------------------------------------------------------*/
/**
 *  Send control information.  Data in the common transfer descriptor
 *      is sent (if aLength > 0).
 *
 *  @param [in]    *aWorkspace     	USB Host controller workspace
 *
 *  @param [in]    aBMRequestType 	Type of request
 *
 *  @param [in]    aRequest      	Request
 *
 *  @param [in]    aValue       	Value for request
 *
 *  @param [in]    aIndex      		Index for request
 *
 *  @param [in]    aLength      	Length of data in command
 *                                  (currently in TDBuffer)
 *  @return        T_uezError   	UEZ_ERROR_NONE if successful, else
 *                                  error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
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
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

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
 *---------------------------------------------------------------------------*/
/**
 *  Receive control information.  Data is received in the common
 *      transfer descriptor (if aLength > 0).
 *
 *  @param [in]    *aWorkspace     	USB Host controller workspace
 *
 *  @param [in]    aBMRequestType 	Type of request
 *
 *  @param [in]    aRequest       	Request
 *
 *  @param [in]    aValue        	Value for request
 *
 *  @param [in]    aIndex         	Index for request
 *
 *  @param [in]    aLength       	Length of data in command
 *                                  currently in TDBuffer)
 *  @return        T_uezError     	UEZ_ERROR_NONE if successful, else
 *                                  error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
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
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

    IHost_FillSetup(p, aBMRequestType, aRequest, aValue, aIndex, aLength);
    error = Host_ProcessTD(aWorkspace, EDCtrl, TD_SETUP, TDBuffer, 8, aTimeout);
    if (error == UEZ_ERROR_NONE) {
        // Get the data (if any)
        if (aLength)
            error = Host_ProcessTD(aWorkspace, EDCtrl, TD_IN, aBuffer, aLength, aTimeout);

        // Send an ACK
        if (error == UEZ_ERROR_NONE)
            error = Host_ProcessTD(aWorkspace, EDCtrl, TD_OUT, 0, 0, aTimeout);
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Host_SetCallbacks
 *---------------------------------------------------------------------------*/
/**
 *  Declare callbacks for this workspace.
 *
 *  @param [in]    *aWorkspace       	USB Host controller workspace
 *
 *  @param [in]    *aCallbackWorkspace  Workspace used when call back called
 *
 *  @param [in]    *aCallbackAPI 		Structure holding callback
 *                                      routines to be called.
 *
 *  @return        T_uezError  			UEZ_ERROR_NONE if successful, else
 *                                      error code.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static T_uezError Host_SetCallbacks(
        void *aWorkspace,
        void *aCallbackWorkspace,
        const T_USBHostCallbacks *aCallbackAPI)
{
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

    p->iCallbackAPI = aCallbackAPI;
    p->iCallbackWorkspace = aCallbackWorkspace;

    return UEZ_ERROR_NONE;
}

static T_usbHostDeviceState Host_GetState(void *aWorkspace)
{
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;
    return p->iState;
}

static void Host_ResetPort(void *aWorkspace)
{
    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

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
//    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

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
//    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

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
//    T_LPC1756_USBHost_Workspace *p = (T_LPC1756_USBHost_Workspace *)aWorkspace;

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


const HAL_USBHost G_LPC1756_USBHost_Interface = {
    // T_halInterface iInterface
    {
        "LPC1756:USBHost",
        0x0100,
        LPC1756_USBHost_InitializeWorkspace,
        sizeof(T_LPC1756_USBHost_Workspace),
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
void LPC1756_USBHost_Require(const T_LPC1756_USBHost_Settings *aSettings)
{
    static const T_LPC1756_IOCON_ConfigList pinsDP1[] = {
            {GPIO_P0_29, IOCON_U_DEFAULT(1)}, // USB_D+1
    };
    static const T_LPC1756_IOCON_ConfigList pinsDM1[] = {
            {GPIO_P0_30, IOCON_U_DEFAULT(1)}, // USB_D-1
    };
    static const T_LPC1756_IOCON_ConfigList pinsCONNECT1[] = {
            {GPIO_P2_9,  IOCON_D_DEFAULT(1)}, // USB_CONNECT1
    };
    static const T_LPC1756_IOCON_ConfigList pinsUP_LED1[] = {
            {GPIO_P1_18, IOCON_D_DEFAULT(1)}, // USB_UP_LED1
    };
    static const T_LPC1756_IOCON_ConfigList pinsPPWR1[] = {
            {GPIO_P1_19, IOCON_D_DEFAULT(2)}, // USB_PPWR1n
    };
    static const T_LPC1756_IOCON_ConfigList pinsPWRD1[] = {
            {GPIO_P1_22, IOCON_D_DEFAULT(2)}, // USB_PWRD1
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register USB Host driver ports for LPC24xx
    HALInterfaceRegister("USBHost",
            (T_halInterface *)&G_LPC1756_USBHost_Interface, 0, 0);
//    LPC1756_USBHost_VBUS_Require(aSettings->iVBUS);
    LPC1756_IOCON_ConfigPin(aSettings->iDP, pinsDP1, ARRAY_COUNT(pinsDP1));
    LPC1756_IOCON_ConfigPin(aSettings->iDM, pinsDM1, ARRAY_COUNT(pinsDM1));
    LPC1756_IOCON_ConfigPinOrNone(aSettings->iCONNECT, pinsCONNECT1,
            ARRAY_COUNT(pinsCONNECT1));
    LPC1756_IOCON_ConfigPinOrNone(aSettings->iUP_LED, pinsUP_LED1,
            ARRAY_COUNT(pinsUP_LED1));
    LPC1756_IOCON_ConfigPinOrNone(aSettings->iPPWRn, pinsPPWR1,
            ARRAY_COUNT(pinsPPWR1));
    LPC1756_IOCON_ConfigPinOrNone(aSettings->iPWRDn, pinsPWRD1,
            ARRAY_COUNT(pinsPWRD1));
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_USBHost.c
 *-------------------------------------------------------------------------*/
