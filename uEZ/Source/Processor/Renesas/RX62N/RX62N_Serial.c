/*-------------------------------------------------------------------------*
 * File:  RX62N_Serial.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Renesas RX62N Serial Communications Interface (SCIa)
 * Implementation:
 *      Six serial interfaces are provided here.
 *      Buffers/Queues are not stored here but handled
 *      by the layer above.  Only callback functions from within
 *      the IRQ are provided.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <HAL/HAL.h>
#include <HAL/Serial.h>
#include <HAL/Interrupt.h>
#include <uEZProcessor.h>
#include "RX62N_UtilityFuncs.h"
#include "RX62N_Serial.h"

// TBD:  not here!
#include <uEZRTOS.h>

#if (COMPILER_TYPE==RenesasRX)
typedef volatile struct st_sci __evenaccess T_RX62N_SCI;
#endif

/*---------------------------------------------------------------------------*
 * Constants and Macros:
 *---------------------------------------------------------------------------*/
#define BAUD_DIVIDER(b)         (((PCLK_FREQUENCY/1)/((b)*32))-1)

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
// Entry of SCI_ port settings
typedef struct {
    TUInt32 iInterruptChannel_RX;
    TUInt32 iInterruptChannel_TX;
    T_irqPriority iPriority;
    T_RX62N_SCI *iSCIRegs;
    TISRFPtr iISR_RX;
    TISRFPtr iISR_TX;
    TUInt16 iMSTPBit;
    TUInt8 iPFFSCIBit;
} T_Serial_RX62N_Info;

typedef struct {
    const HAL_Serial *iHAL;
    const T_Serial_RX62N_Info *iInfo;
    HAL_Serial_Callback_Transmit_Empty iTransmitEmptyFunc;
    HAL_Serial_Callback_Received_Byte iReceivedByteFunc;
    void *iCallbackWorkspace;
    T_Serial_Settings iSettings;
} T_Serial_RX62N_Workspace;

/*---------------------------------------------------------------------------*
 * Function Prototypes:
 *---------------------------------------------------------------------------*/

IRQ_ROUTINE(Serial_SCI0_RX);
IRQ_ROUTINE(Serial_SCI0_TX);

IRQ_ROUTINE(Serial_SCI1_RX);
IRQ_ROUTINE(Serial_SCI1_TX);

IRQ_ROUTINE(Serial_SCI2_RX);
IRQ_ROUTINE(Serial_SCI2_TX);

IRQ_ROUTINE(Serial_SCI3_RX);
IRQ_ROUTINE(Serial_SCI3_TX);

IRQ_ROUTINE(Serial_SCI5_RX);
IRQ_ROUTINE(Serial_SCI5_TX);

IRQ_ROUTINE(Serial_SCI6_RX);
IRQ_ROUTINE(Serial_SCI6_TX);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

T_Serial_RX62N_Workspace *G_Serial0Workspace=0;
extern const HAL_Serial Serial_RX62N_SCI0_Interface;
extern const T_Serial_RX62N_Info G_RX62N_Serial_SCI0_Info;

T_Serial_RX62N_Workspace *G_Serial1Workspace=0;
extern const HAL_Serial Serial_RX62N_SCI1_Interface;
extern const T_Serial_RX62N_Info G_RX62N_Serial_SCI1_Info;

T_Serial_RX62N_Workspace *G_Serial2Workspace=0;
extern const HAL_Serial Serial_RX62N_SCI2_Interface;
extern const T_Serial_RX62N_Info G_RX62N_Serial_SCI2_Info;

T_Serial_RX62N_Workspace *G_Serial3Workspace=0;
extern const HAL_Serial Serial_RX62N_SCI3_Interface;
extern const T_Serial_RX62N_Info G_RX62N_Serial_SCI3_Info;

T_Serial_RX62N_Workspace *G_Serial5Workspace=0;
extern const HAL_Serial Serial_RX62N_SCI5_Interface;
extern const T_Serial_RX62N_Info G_RX62N_Serial_SCI5_Info;

T_Serial_RX62N_Workspace *G_Serial6Workspace=0;
extern const HAL_Serial Serial_RX62N_SCI6_Interface;
extern const T_Serial_RX62N_Info G_RX62N_Serial_SCI6_Info;


/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_ProcessInterrupt_RX
 *---------------------------------------------------------------------------*
 * Description:
 *      Generic routine for handling any of the serial ports receive
 *      interrupts.
 *      NOTE:  This routine is ALWAYS processed inside of a interrupt
 *              service routine.
 * Inputs:
 *      T_SCI_PortEntry *p      -- Port's definition to process interrupt
 *---------------------------------------------------------------------------*/
static void RX62N_Serial_ProcessInterrupt_RX(T_Serial_RX62N_Workspace *p)
{
    // We have received 1 or more bytes.  Please do
    // something with this received byte
    p->iReceivedByteFunc(
        p->iCallbackWorkspace,
        p->iInfo->iSCIRegs->RDR);
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_ProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Generic routine for handling any of the serial ports transmit
 *      interrupts.
 *      NOTE:  This routine is ALWAYS processed inside of a interrupt
 *              service routine.
 * Inputs:
 *      T_SCI_PortEntry *p      -- Port's definition to process interrupt
 *---------------------------------------------------------------------------*/
static void RX62N_Serial_ProcessInterrupt_TX(T_Serial_RX62N_Workspace *p)
{
    // Just ran out of bytes, please transmit more (1 byte for now)
    // (or we stop here)
    p->iTransmitEmptyFunc(p->iCallbackWorkspace, 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  Serial_SCI0_RX
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the Serial0 interrupt and forward to RX62N_Serial_ProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(Serial_SCI0_RX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_RX(G_Serial0Workspace);
    IRQ_END();
}
IRQ_ROUTINE(Serial_SCI0_TX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_TX(G_Serial0Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  Serial_SCI1_RX
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the Serial1 interrupt and forward to RX62N_Serial_ProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(Serial_SCI1_RX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_RX(G_Serial1Workspace);
    IRQ_END();
}
IRQ_ROUTINE(Serial_SCI1_TX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_TX(G_Serial1Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  Serial_SCI2_RX
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the Serial2 interrupt and forward to RX62N_Serial_ProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(Serial_SCI2_RX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_RX(G_Serial2Workspace);
    IRQ_END();
}
IRQ_ROUTINE(Serial_SCI2_TX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_TX(G_Serial2Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  Serial_SCI3_RX
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the Serial3 interrupt and forward to RX62N_Serial_ProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(Serial_SCI3_RX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_RX(G_Serial3Workspace);
    IRQ_END();
}
IRQ_ROUTINE(Serial_SCI3_TX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_TX(G_Serial3Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  Serial_SCI5_RX
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the Serial5 interrupt and forward to RX62N_Serial_ProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(Serial_SCI5_RX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_RX(G_Serial5Workspace);
    IRQ_END();
}
IRQ_ROUTINE(Serial_SCI5_TX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_TX(G_Serial5Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  Serial_SCI6_RX
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the Serial6 interrupt and forward to RX62N_Serial_ProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(Serial_SCI6_RX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_RX(G_Serial6Workspace);
    IRQ_END();
}
IRQ_ROUTINE(Serial_SCI6_TX)
{
    IRQ_START();
    RX62N_Serial_ProcessInterrupt_TX(G_Serial6Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  SCI_SendChar
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a single character out the given SCI_ port.
 *      NOTE:  Does not block even if output queue is full!
 * Inputs:
 *      T_SCI_Port aPort        -- SCI_ port to use
 *      TUInt8 c                  -- Character to send
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_OutputByte(void *aWorkspace, TUInt8 aByte)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;

    p->iInfo->iSCIRegs->TDR = aByte;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SCI_Disable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable SCI_ processing on this port.
 * Inputs:
 *      T_SCI_Port aPort        -- SCI_ port to disable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_Deactivate(void *aWorkspace)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;
    const T_Serial_RX62N_Info *p_info = p->iInfo;

    //p_info->iSCIRegs->SCR.BIT.TIE = 0;
    //p_info->iSCIRegs->SCR.BIT.RIE = 0;
    InterruptDisable(p_info->iInterruptChannel_RX);
    InterruptDisable(p_info->iInterruptChannel_TX);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SCI_Enable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable SCI_ processing on this port.
 * Inputs:
 *      T_SCI_Port aPort        -- SCI_ port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_Activate(void *aWorkspace)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;
    const T_Serial_RX62N_Info *p_info = p->iInfo;

    InterruptEnable(p_info->iInterruptChannel_RX);
    InterruptEnable(p_info->iInterruptChannel_TX);
    //p_info->iSCIRegs->SCR.BIT.TIE = 1;
    //p_info->iSCIRegs->SCR.BIT.RIE = 1;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_InitializeWorkspace_SCI0
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a SCI_ port workspace for Serial0
 * Inputs:
 *      T_SCI_Port aPort        -- SCI_ port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_InitializeWorkspace_SCI0(void *aWorkspace)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;

    G_Serial0Workspace = p;
    p->iInfo = &G_RX62N_Serial_SCI0_Info;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_InitializeWorkspace_SCI1
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a SCI_ port workspace for Serial1
 * Inputs:
 *      T_SCI_Port aPort        -- SCI_ port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_InitializeWorkspace_SCI1(void *aWorkspace)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;

    G_Serial1Workspace = p;
    p->iInfo = &G_RX62N_Serial_SCI1_Info;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_InitializeWorkspace_SCI2
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a SCI_ port workspace for Serial2
 * Inputs:
 *      T_SCI_Port aPort        -- SCI_ port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_InitializeWorkspace_SCI2(void *aWorkspace)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;

    G_Serial2Workspace = p;
    p->iInfo = &G_RX62N_Serial_SCI2_Info;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_InitializeWorkspace_SCI3
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a SCI_ port workspace for Serial3
 * Inputs:
 *      T_SCI_Port aPort        -- SCI_ port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_InitializeWorkspace_SCI3(void *aWorkspace)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;

    G_Serial3Workspace = p;
    p->iInfo = &G_RX62N_Serial_SCI3_Info;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_InitializeWorkspace_SCI5
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a SCI_ port workspace for Serial5
 * Inputs:
 *      T_SCI_Port aPort        -- SCI_ port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_InitializeWorkspace_SCI5(void *aWorkspace)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;

    G_Serial5Workspace = p;
    p->iInfo = &G_RX62N_Serial_SCI5_Info;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_InitializeWorkspace_SCI6
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a SCI_ port workspace for Serial6
 * Inputs:
 *      T_SCI_Port aPort        -- SCI_ port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_InitializeWorkspace_SCI6(void *aWorkspace)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;

    G_Serial6Workspace = p;
    p->iInfo = &G_RX62N_Serial_SCI6_Info;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Link the SCI_ port to a pair of callback routines and an
 *      associated callback workspace.  The callbacks are called when
 *      a byte is received and when the transmit buffer becomes empty.
 * Inputs:
 *      void *aWorkspace          -- SCI_ port workspace
 *      void *aCallbackWorkspace  -- Callback's workspace
 *      HAL_Serial_Callback_Received_Byte aReceivedByteCallback
 *                                -- Callback routine for received bytes
 *      HAL_Serial_Callback_Transmit_Empty aTransmitEmptyCallback
 *                                -- Callback routine for transmit empty
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_Configure(
    void *aWorkspace,
    void *aCallbackWorkspace,
    HAL_Serial_Callback_Received_Byte aReceivedByteCallback,
    HAL_Serial_Callback_Transmit_Empty aTransmitEmptyCallback)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;
    const T_Serial_RX62N_Info *p_info = p->iInfo;
    TUInt32 divider;
    TUInt32 n;
    TUInt8 cks;

    // Ensure interrupts for this channel is off
    InterruptDisable(p->iInfo->iInterruptChannel_RX);
    InterruptDisable(p->iInfo->iInterruptChannel_TX);

    // Ensure MSTP is on for this component (by making 0)
    SYSTEM.MSTPCRB.LONG &= ~(1UL << p->iInfo->iMSTPBit);

    p->iReceivedByteFunc = aReceivedByteCallback;
    p->iTransmitEmptyFunc = aTransmitEmptyCallback;
    p->iCallbackWorkspace = aCallbackWorkspace;

    // Serial Control Register (SCR)
    // Clear all the SCR settings (and turn off interrupts)
    // and stop all processing.
    p_info->iSCIRegs->SCR.BYTE = 0x00;

    // Serial Mode Register (SMR)
    //  0--- ----   CM =
    //                  0: Asynchronous mode
    //                  1: Clock synchronous mode
    //  -x-- ----   CHR =
    //                  0: 8 bits data length
    //                  1: 7 bits data length
    //  --x- ----   PE =
    //                  0:  Parity bit addition is not performed /
    //                      Parity bit checking is not performed
    //                  1:  Parity bit addition is performed /
    //                      Parity bit checking is performed
    //  ---x ----   PM = Parity mode
    //                  0: Selects even parity
    //                  1: Selects odd parity
    //  ---- x---   STOP = Stop bit length
    //                  0: 1 stop bit
    //                  1: 2 stop bits
    //  ---- -0--   MP = Multi-processor mode
    //                  0: Multi-processor communications function is disabled
    //                  1: Multi-processor communications function is enabled
    //  ---- --xx   CKS[0:1] =
    //                  00: PCLK clock / 1 (n=0)
    //                  01: PCLK clock / 2 (n=1)
    //                  10: PCLK clock / 4 (n=2)
    //                  11: PCLK clock / 8 (n=3)
    //
    // Determine the baud rate and PCLK divider CKS
    divider = BAUD_DIVIDER(SERIAL_PORTS_DEFAULT_BAUD);
    cks = 0;
    while (divider >= 256) {
        divider /= 4;
        cks++;
    }

    // Use a default group of settings
    p_info->iSCIRegs->SMR.BYTE =
    // Serial is asynchronous for this driver
        (0 << 7) |
        // Set the word length to 8 bits
            (0 << 6) |
        // No parity
            (0 << 5) |
        // Partiy (does not matter)
            (0 << 4) |
        // 1 stop bit
            (0 << 3) |
        // Not multi-processor mode
            (0 << 2) |
        // PCK divider
            (cks & 0x03);
    p_info->iSCIRegs->BRR = divider;

    /* Wait for at least 1 bit time */
    for (n = 0; n < 1000000; n++)
        ; // TBD: This is not very good

    // Set up the SCR to turn on Tx and Rx interrupts
    p_info->iSCIRegs->SCR.BIT.TIE = 1;
    p_info->iSCIRegs->SCR.BIT.RIE = 1;

    InterruptRegister(p_info->iInterruptChannel_RX, p_info->iISR_RX,
        p_info->iPriority, p->iHAL->iInterface.iName);
    InterruptRegister(p_info->iInterruptChannel_TX, p_info->iISR_TX,
        p_info->iPriority, p->iHAL->iInterface.iName);

    // Enable sending/receive data (must be both done at the same time)
    p_info->iSCIRegs->SCR.BYTE |= 0x30;

    IEN(SCI0, RXI0) = 1;
    IEN(SCI0, TXI0) = 1;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_SetSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the SCI_ port settings
 * Inputs:
 *      void *aWorkspace          -- SCI_ port workspace
 *      T_SCI_Settings *aSettings -- Settings to use for SCI_ port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_SetSettings(
    void *aWorkspace,
    T_Serial_Settings *aSettings)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;
    const T_Serial_RX62N_Info *p_info = p->iInfo;
    TUInt32 divider;
    TUInt32 n;
    TUInt8 cks;

    // Record these settings
    p->iSettings = *aSettings;

    // Clear all the SCR settings (and turn off interrupts)
    // and stop all processing.
    p_info->iSCIRegs->SCR.BYTE = 0x00;

    // Serial Mode Register (SMR)
    //  0--- ----   CM =
    //                  0: Asynchronous mode
    //                  1: Clock synchronous mode
    //  -x-- ----   CHR =
    //                  0: 8 bits data length
    //                  1: 7 bits data length
    //  --x- ----   PE =
    //                  0:  Parity bit addition is not performed /
    //                      Parity bit checking is not performed
    //                  1:  Parity bit addition is performed /
    //                      Parity bit checking is performed
    //  ---x ----   PM = Parity mode
    //                  0: Selects even parity
    //                  1: Selects odd parity
    //  ---- x---   STOP = Stop bit length
    //                  0: 1 stop bit
    //                  1: 2 stop bits
    //  ---- -0--   MP = Multi-processor mode
    //                  0: Multi-processor communications function is disabled
    //                  1: Multi-processor communications function is enabled
    //  ---- --xx   CKS[0:1] =
    //                  00: PCLK clock / 1 (n=0)
    //                  01: PCLK clock / 2 (n=1)
    //                  10: PCLK clock / 4 (n=2)
    //                  11: PCLK clock / 8 (n=3)
    //
    // Determine the baud rate and PCLK divider CKS
    divider = BAUD_DIVIDER(aSettings->iBaud);
    cks = 0;
    while (divider >= 256) {
        divider /= 4;
        cks++;
    }
    p_info->iSCIRegs->SMR.BYTE =
    // Serial is asynchronous for this driver
        (0 << 7) |
        // Set the word length (7 or 8 bits)
            ((aSettings->iWordLength == 7) ? (1 << 6) : (0 << 6)) |
        // Enable parity checking if not none
            ((aSettings->iParity != SERIAL_PARITY_NONE) ? (1 << 5) : (0 << 5)) |
        // Even or odd parity only
            ((aSettings->iParity == SERIAL_PARITY_EVEN) ? (1 << 4) : (0 << 4)) |
        // 1 or 2 stop bits?
            ((aSettings->iStopBit == SERIAL_STOP_BITS_2) ? (1 << 3) : (0 << 3))
            |
            // Not multi-processor mode
            (0 << 2) |
        // PCK divider
            (cks & 0x03);
    p_info->iSCIRegs->BRR = divider;

    /* Wait for at least 1 bit time */
    for (n = 0; n < 1000000; n++)
        ; // TBD: This is not very good

    // Set up the SCR to turn on Tx and Rx interrupts
    p_info->iSCIRegs->SCR.BIT.TIE = 1;
    p_info->iSCIRegs->SCR.BIT.RIE = 1;

    // Enable sending/receive data
    p_info->iSCIRegs->SCR.BIT.TE = 1;
    p_info->iSCIRegs->SCR.BIT.RE = 1;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_GetSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the previously configured SCI_ port
 * Inputs:
 *      void *aWorkspace          -- SCI_ port workspace
 *      T_SCI_Settings *aSettings -- Settings used for SCI_ port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_GetSettings(
    void *aWorkspace,
    T_Serial_Settings *aSettings)
{
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;

    // Record these settings
    *aSettings = p->iSettings;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_SetHandshakingControl
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the handshaking control for this SCI_ port (if it has it)
 * Inputs:
 *      void *aWorkspace          -- SCI_ port workspace
 *      T_SCI_HandshakingControl -- Control bits to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_SetHandshakingControl(
    void *aWorkspace,
    T_serialHandshakingControl aControl)
{
    PARAM_NOT_USED(aWorkspace); PARAM_NOT_USED(aControl);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_GetHandshakingStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the handshaking control for this SCI_ port (if it has it)
 * Inputs:
 *      void *aWorkspace          -- SCI_ port workspace
 *      T_SCI_HandshakingStatus *aStatus -- Current status
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_Serial_GetHandshakingStatus(
    void *aWorkspace,
    T_serialHandshakingStatus *aStatus)
{
    PARAM_NOT_USED(aWorkspace); PARAM_NOT_USED(aStatus);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_Serial_GetStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the status of the port
 * Inputs:
 *      void *aWorkspace          -- SCI_ port workspace
 * Outputs:
 *      T_SCI_StatusByte          -- Status byte returned
 *---------------------------------------------------------------------------*/
T_serialStatusByte RX62N_Serial_GetStatus(void *aWorkspace)
{
    T_serialStatusByte status = 0;
    T_Serial_RX62N_Workspace *p = (T_Serial_RX62N_Workspace *)aWorkspace;

    // Only empty if both TEMT and THRE are empty
    if (p->iInfo->iSCIRegs->SSR.BYTE & 0x80) {
        status |= SERIAL_STATUS_TRANSMIT_EMPTY;
    } else {
        // not empty
    }

    return status;
}

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
// List of SCI_ port HAL interfaces
const HAL_Serial Serial_RX62N_SCI0_Interface = {
    "RX62N:SCI0",
    0x0100,
    RX62N_Serial_InitializeWorkspace_SCI0,
    sizeof(T_Serial_RX62N_Workspace),

    RX62N_Serial_Configure,
    RX62N_Serial_SetSettings,
    RX62N_Serial_GetSettings,
    RX62N_Serial_Activate,
    RX62N_Serial_Deactivate,
    RX62N_Serial_OutputByte,
    0,
    0,
    RX62N_Serial_GetStatus,
};

const HAL_Serial Serial_RX62N_SCI1_Interface = {
    "RX62N:SCI1",
    0x0100,
    RX62N_Serial_InitializeWorkspace_SCI1,
    sizeof(T_Serial_RX62N_Workspace),

    RX62N_Serial_Configure,
    RX62N_Serial_SetSettings,
    RX62N_Serial_GetSettings,
    RX62N_Serial_Activate,
    RX62N_Serial_Deactivate,
    RX62N_Serial_OutputByte,
    0,
    0,
    RX62N_Serial_GetStatus,
};

const HAL_Serial Serial_RX62N_SCI2_Interface = {
    "RX62N:SCI2",
    0x0100,
    RX62N_Serial_InitializeWorkspace_SCI2,
    sizeof(T_Serial_RX62N_Workspace),

    RX62N_Serial_Configure,
    RX62N_Serial_SetSettings,
    RX62N_Serial_GetSettings,
    RX62N_Serial_Activate,
    RX62N_Serial_Deactivate,
    RX62N_Serial_OutputByte,
    0,
    0,
    RX62N_Serial_GetStatus,
};

const HAL_Serial Serial_RX62N_SCI3_Interface = {
    "RX62N:SCI3",
    0x0100,
    RX62N_Serial_InitializeWorkspace_SCI3,
    sizeof(T_Serial_RX62N_Workspace),

    RX62N_Serial_Configure,
    RX62N_Serial_SetSettings,
    RX62N_Serial_GetSettings,
    RX62N_Serial_Activate,
    RX62N_Serial_Deactivate,
    RX62N_Serial_OutputByte,
    0,
    0,
    RX62N_Serial_GetStatus,
};

const HAL_Serial Serial_RX62N_SCI5_Interface = {
    "RX62N:SCI5",
    0x0100,
    RX62N_Serial_InitializeWorkspace_SCI5,
    sizeof(T_Serial_RX62N_Workspace),

    RX62N_Serial_Configure,
    RX62N_Serial_SetSettings,
    RX62N_Serial_GetSettings,
    RX62N_Serial_Activate,
    RX62N_Serial_Deactivate,
    RX62N_Serial_OutputByte,
    0,
    0,
    RX62N_Serial_GetStatus,
};

const HAL_Serial Serial_RX62N_SCI6_Interface = {
    "RX62N:SCI6",
    0x0100,
    RX62N_Serial_InitializeWorkspace_SCI6,
    sizeof(T_Serial_RX62N_Workspace),

    RX62N_Serial_Configure,
    RX62N_Serial_SetSettings,
    RX62N_Serial_GetSettings,
    RX62N_Serial_Activate,
    RX62N_Serial_Deactivate,
    RX62N_Serial_OutputByte,
    0,
    0,
    RX62N_Serial_GetStatus,
};

const T_Serial_RX62N_Info G_RX62N_Serial_SCI0_Info = {
    VECT_SCI0_RXI0,
    VECT_SCI0_TXI0,
    INTERRUPT_PRIORITY_HIGH,
    (T_RX62N_SCI *)&SCI0,
    (TISRFPtr)Serial_SCI0_RX,
    (TISRFPtr)Serial_SCI0_TX,
    31, // MSTPB31
    0, // None
};

const T_Serial_RX62N_Info G_RX62N_Serial_SCI1_Info = {
    VECT_SCI1_RXI1,
    VECT_SCI1_TXI1,
    INTERRUPT_PRIORITY_HIGH,
    (T_RX62N_SCI *)&SCI1,
    (TISRFPtr)Serial_SCI1_RX,
    (TISRFPtr)Serial_SCI1_TX,
    30, // MSTPB30
    (1<<1), // Bit 1
};

const T_Serial_RX62N_Info G_RX62N_Serial_SCI2_Info = {
    VECT_SCI2_RXI2,
    VECT_SCI2_TXI2,
    INTERRUPT_PRIORITY_HIGH,
    (T_RX62N_SCI *)&SCI2,
    (TISRFPtr)Serial_SCI2_RX,
    (TISRFPtr)Serial_SCI2_TX,
    29, // MSTPB29
    (1<<2), // Bit 2
};

const T_Serial_RX62N_Info G_RX62N_Serial_SCI3_Info = {
    VECT_SCI3_RXI3,
    VECT_SCI3_TXI3,
    INTERRUPT_PRIORITY_HIGH,
    (T_RX62N_SCI *)&SCI3,
    (TISRFPtr)Serial_SCI3_RX,
    (TISRFPtr)Serial_SCI3_TX,
    28, // MSTPB28
    (1<<3), // Bit 3
};

const T_Serial_RX62N_Info G_RX62N_Serial_SCI5_Info = {
    VECT_SCI5_RXI5,
    VECT_SCI5_TXI5,
    INTERRUPT_PRIORITY_HIGH,
    (T_RX62N_SCI *)&SCI5,
    (TISRFPtr)Serial_SCI5_RX,
    (TISRFPtr)Serial_SCI5_TX,
    26, // MSTPB26
    0, // None
};

const T_Serial_RX62N_Info G_RX62N_Serial_SCI6_Info = {
    VECT_SCI6_RXI6,
    VECT_SCI6_TXI6,
    INTERRUPT_PRIORITY_HIGH,
    (T_RX62N_SCI *)&SCI6,
    (TISRFPtr)Serial_SCI6_RX,
    (TISRFPtr)Serial_SCI6_TX,
    25, // MSTPB25
    (1<<6), // Bit 6
};

void RX62N_SCI0_Require(void)
{
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_P21,  	// RXD0
			GPIO_P20	// TXD0
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI0", (T_halInterface *)&Serial_RX62N_SCI0_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	PORT2.ICR.BIT.B1 = 1; 			// Turn on P21 (RXD0) input buffer
}

void RX62N_SCI1_A_Require(void)
{
    static const T_uezGPIOPortPin pins[] = {
           	GPIO_P30,  	// RXD1-A
			GPIO_P26	// TXD1-A
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI1", (T_halInterface *)&Serial_RX62N_SCI1_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFFSCI.BIT.SCI1S = 0;	// Select SCI1-A pins
	PORT3.ICR.BIT.B0 = 1; 			// Turn on P30 (RXD1-A) input buffer
}

void RX62N_SCI1_B_Require(void)
{
    static const T_uezGPIOPortPin pins[] = {
           	GPIO_PF2,  	// RXD1-B
			GPIO_PF0	// TXD1-B
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI1", (T_halInterface *)&Serial_RX62N_SCI1_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFFSCI.BIT.SCI1S = 1;	// Select SCI1-B pins
	PORTF.ICR.BIT.B2 = 1; 			// Turn on PF2 (RXD1-B) input buffer 
}

void RX62N_SCI2_A_Require(void)
{
    static const T_uezGPIOPortPin pins[] = {
           	GPIO_P12,  	// RXD2-A
			GPIO_P13	// TXD2-A
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI2", (T_halInterface *)&Serial_RX62N_SCI2_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFFSCI.BIT.SCI2S = 0;	// Select SCI2-A pins
	PORT1.ICR.BIT.B2 = 1; 			// Turn on P12 (RXD2-A) input buffer 
}

void RX62N_SCI2_B_Require(void)
{
    static const T_uezGPIOPortPin pins[] = {
           	GPIO_P52,  	// RXD2-B
			GPIO_P50	// TXD2-B
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI2", (T_halInterface *)&Serial_RX62N_SCI2_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFFSCI.BIT.SCI2S = 1;	// Select SCI2-B pins
	PORT5.ICR.BIT.B2 = 1; 			// Turn on P52 (RXD2-B) input buffer 
}

void RX62N_SCI3_A_Require(void)
{
    static const T_uezGPIOPortPin pins[] = {
           	GPIO_P16,  	// RXD3-A
			GPIO_P17	// TXD3-A
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI3", (T_halInterface *)&Serial_RX62N_SCI3_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFFSCI.BIT.SCI3S = 0;	// Select SCI3-A pins
	PORT1.ICR.BIT.B6 = 1; 			// Turn on P16 (RXD3-A) input buffer 
}

void RX62N_SCI3_B_Require(void)
{
    static const T_uezGPIOPortPin pins[] = {
           	GPIO_P25,  	// RXD3-B
			GPIO_P23	// TXD3-B
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI3", (T_halInterface *)&Serial_RX62N_SCI3_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFFSCI.BIT.SCI3S = 1;	// Select SCI3-B pins
	PORT2.ICR.BIT.B5 = 1; 			// Turn on P25 (RXD3-B) input buffer 
}

void RX62N_SCI5_Require(void)
{
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_PC2,  	// RXD0
			GPIO_PC3	// TXD0
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI5", (T_halInterface *)&Serial_RX62N_SCI5_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	PORTC.ICR.BIT.B2 = 1;	// Turn on PC2 (RXD5) input buffer 
}

void RX62N_SCI6_A_Require(void)
{
    static const T_uezGPIOPortPin pins[] = {
           	GPIO_P01,  	// RXD6-A
			GPIO_P00	// TXD6-A
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI6", (T_halInterface *)&Serial_RX62N_SCI6_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFFSCI.BIT.SCI6S = 0;	// Select SCI6-A pins
	PORT0.ICR.BIT.B1 = 1; 			// Turn on P01 (RXD6-B) input buffer 
}

void RX62N_SCI6_B_Require(void)
{
    static const T_uezGPIOPortPin pins[] = {
           	GPIO_P33,  	// RXD6-B
			GPIO_P32	// TXD6-B
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("SCI6", (T_halInterface *)&Serial_RX62N_SCI6_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFFSCI.BIT.SCI6S = 1;	// Select SCI6-B pins
	PORT3.ICR.BIT.B3 = 1; 			// Turn on P33 (RXD6-B) input buffer 
}


/*-------------------------------------------------------------------------*
 * End of File:  RX62N_Serial.c
 *-------------------------------------------------------------------------*/
