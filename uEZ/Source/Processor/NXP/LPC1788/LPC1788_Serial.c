/*-------------------------------------------------------------------------*
 * File: Serial.c
 *-------------------------------------------------------------------------*
 * Description:
 *      LPC23xx and LPC 17XX 16550(?) Style serial ports
 * Implementation:
 *      Four serial ports are available (although they can be individually
 *      setup as needed).  Buffers/Queues are not stored here but handled
 *      by the layer above.  Only callback functions from within
 *      the IRQ are provided.
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
#include <HAL/HAL.h>
#include <HAL/Interrupt.h>
#include <uEZTypes.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1788/LPC1788_Serial.h>

// TBD: not here!
#include <uEZRTOS.h>

/*---------------------------------------------------------------------------*
 * Constants and Macros:
 *---------------------------------------------------------------------------*/
#define BAUD_DIVIDER(b)         ((PCLK_FREQUENCY/1)/((b)*16))

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
// Mapping of 16550 style registers in memory
typedef struct {
    /* 0x00 */
    union {
        volatile unsigned long RBR;
        volatile unsigned long THR;
        volatile unsigned long DLL;
    } reg00;
    /* 0x04 */
    union {
        volatile unsigned long DLM;
        volatile unsigned long IER;
    } reg04;
    /* 0x08 */
    union {
        volatile unsigned long IIR;
        volatile unsigned long FCR;
    } reg08;
    /* 0x0C */
    volatile unsigned long LCR;

    /* 0x10 */
    volatile unsigned long MCR;

    /* 0x14 */
    volatile unsigned long LSR;

    /* 0x18 */
    volatile unsigned long MSR;

    /* 0x1C */
    volatile unsigned long SCR;
    /* 0x20 */
    volatile unsigned long ACR;
    /* 0x24 */
    volatile unsigned long ICR;
    /* 0x28 */
    volatile unsigned long FDR;
    /* 0x2C */
    volatile unsigned long reserved_0x2C;
    /* 0x30 */
    volatile unsigned long TER;
} T_16550UART;

// Entry of serial port settings
typedef struct {
    TUInt32 iInterruptChannel;
    T_irqPriority iPriority;
    T_16550UART *iUART;
    TISRFPtr iISR;
    TUInt8 iPowerBitIndex; // PCONP bit to set
} T_Serial_LPC1788_SerialInfo;

typedef struct {
    const HAL_Serial *iHAL;
    const T_Serial_LPC1788_SerialInfo *iInfo;
    HAL_Serial_Callback_Transmit_Empty iTransmitEmptyFunc;
    HAL_Serial_Callback_Received_Byte iReceivedByteFunc;
    void *iCallbackWorkspace;
    T_Serial_Settings iSettings;
} T_Serial_LPC1788_Workspace;

/*---------------------------------------------------------------------------*
 * Function Prototypes:
 *---------------------------------------------------------------------------*/
//#define ISerialGrab(entry)         InterruptDisable(entry->iInterruptChannel);
//#define ISerialRelease(entry)      InterruptEnable(entry->iInterruptChannel);
IRQ_ROUTINE(ISerialUART0)
;
IRQ_ROUTINE(ISerialUART1)
;
IRQ_ROUTINE(ISerialUART2)
;
IRQ_ROUTINE(ISerialUART3)
;
IRQ_ROUTINE(ISerialUART4)
;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const HAL_Serial G_LPC1788_Serial_UART0;
extern const HAL_Serial G_LPC1788_Serial_UART1;
extern const HAL_Serial G_LPC1788_Serial_UART2;
extern const HAL_Serial G_LPC1788_Serial_UART3;
extern const HAL_Serial G_LPC1788_Serial_UART4;

extern const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART0;
extern const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART1;
extern const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART2;
extern const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART3;
extern const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART4;

T_Serial_LPC1788_Workspace *G_UART0Workspace = 0;
T_Serial_LPC1788_Workspace *G_UART1Workspace = 0;
T_Serial_LPC1788_Workspace *G_UART2Workspace = 0;
T_Serial_LPC1788_Workspace *G_UART3Workspace = 0;
T_Serial_LPC1788_Workspace *G_UART4Workspace = 0;

/*---------------------------------------------------------------------------*
 * Routine: ISerialProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Generic routine for handling any of the serial ports (as passed
 *      into the routine.
 *      NOTE: This routine is ALWAYS processed inside of a interrupt
 *              service routine.
 * Inputs:
 *      T_SerialPortEntry *p      -- Port's definition to process interrupt
 *---------------------------------------------------------------------------*/
static void ISerialProcessInterrupt(T_Serial_LPC1788_Workspace *p)
{
    TUInt8 iir;

    // Process states until completely done (no more interrupts)
    while (((iir = p->iInfo->iUART->reg08.IIR) & 0x01) == 0) {
        // Process that state or return
        switch ((iir >> 1) & 7) {
            case 0x1:
                // Just ran out of bytes, please transmit more
                // (or we stop here)
                p->iTransmitEmptyFunc(p->iCallbackWorkspace, 8);
                break;
            case 0x2:
                // We have received 1 or more bytes.  Please do
                // something with this received byte
                p->iReceivedByteFunc(p->iCallbackWorkspace,
                        p->iInfo->iUART->reg00.RBR);
                break;
            case 0x0: //Modem interrupt
            case 0x3: //Receive line status interrupt (RDA)
            case 0x6: //Character time out indicator interrupt (CTI)
            default:
                // Ignore rest of states
                return;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine: ISerialUART0
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the UART0 interrupt and forward to ISerialProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ISerialUART0)
{
    IRQ_START();
    ISerialProcessInterrupt(G_UART0Workspace);
    IRQ_END()
    ;
}

/*---------------------------------------------------------------------------*
 * Routine: ISerialUART1
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the UART1 interrupt and forward to ISerialProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ISerialUART1)
{
    IRQ_START();
    ISerialProcessInterrupt(G_UART1Workspace);
    IRQ_END()
    ;
}

/*---------------------------------------------------------------------------*
 * Routine: ISerialUART2
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the UART2 interrupt and forward to ISerialProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ISerialUART2)
{
    IRQ_START();
    ISerialProcessInterrupt(G_UART2Workspace);
    IRQ_END()
    ;
}

/*---------------------------------------------------------------------------*
 * Routine: ISerialUART3
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the UART3 interrupt and forward to ISerialProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ISerialUART3)
{
    IRQ_START();
    ISerialProcessInterrupt(G_UART3Workspace);
    IRQ_END()
    ;
}

/*---------------------------------------------------------------------------*
 * Routine: ISerialUART4
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the UART4 interrupt and forward to ISerialProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ISerialUART4)
{
    IRQ_START();
    ISerialProcessInterrupt(G_UART4Workspace);
    IRQ_END()
    ;
}

/*---------------------------------------------------------------------------*
 * Routine: SerialSendChar
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a single character out the given serial port.
 *      NOTE: Does not block even if output queue is full!
 * Inputs:
 *      T_serialPort aPort        -- Serial port to use
 *      TUInt8 c                  -- Character to send
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_OutputByte(void *aWorkspace, TUInt8 aByte)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    p->iInfo->iUART->reg00.THR = aByte;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: SerialDisable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable serial processing on this port.
 * Inputs:
 *      T_serialPort aPort        -- Serial port to disable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_Deactivate(void *aWorkspace)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    InterruptDisable(p->iInfo->iInterruptChannel);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: SerialEnable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable serial processing on this port.
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_Activate(void *aWorkspace)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    InterruptEnable(p->iInfo->iInterruptChannel);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_InitializeWorkspace_UART0
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART0
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_InitializeWorkspace_UART0(void *aWorkspace)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    G_UART0Workspace = p;
    p->iInfo = &G_LPC1788_Serial_Info_UART0;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_InitializeWorkspace_UART1
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART1
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_InitializeWorkspace_UART1(void *aWorkspace)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    G_UART1Workspace = p;
    p->iInfo = &G_LPC1788_Serial_Info_UART1;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_InitializeWorkspace_UART2
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART2
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_InitializeWorkspace_UART2(void *aWorkspace)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    G_UART2Workspace = p;
    p->iInfo = &G_LPC1788_Serial_Info_UART2;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_InitializeWorkspace_UART3
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART3
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_InitializeWorkspace_UART3(void *aWorkspace)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    G_UART3Workspace = p;
    p->iInfo = &G_LPC1788_Serial_Info_UART3;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_InitializeWorkspace_UART4
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART4
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_InitializeWorkspace_UART4(void *aWorkspace)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    G_UART4Workspace = p;
    p->iInfo = &G_LPC1788_Serial_Info_UART4;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Link the serial port to a pair of callback routines and an
 *      associated callback workspace.  The callbacks are called when
 *      a byte is received and when the transmit buffer becomes empty.
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      void *aCallbackWorkspace  -- Callback's workspace
 *      HAL_Serial_Callback_Received_Byte aReceivedByteCallback
 *                                -- Callback routine for received bytes
 *      HAL_Serial_Callback_Transmit_Empty aTransmitEmptyCallback
 *                                -- Callback routine for transmit empty
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_Configure(
        void *aWorkspace,
        void *aCallbackWorkspace,
        HAL_Serial_Callback_Received_Byte aReceivedByteCallback,
        HAL_Serial_Callback_Transmit_Empty aTransmitEmptyCallback)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    const T_Serial_LPC1788_SerialInfo *p_info = p->iInfo;
    TUInt32 divider;

    InterruptDisable(p->iInfo->iInterruptChannel);

    // Ensure power is on to the part
    LPC1788PowerOn(1UL << p->iInfo->iPowerBitIndex);
    LPC1788PowerOn(1UL << p->iInfo->iPowerBitIndex);

    p->iReceivedByteFunc = aReceivedByteCallback;
    p->iTransmitEmptyFunc = aTransmitEmptyCallback;
    p->iCallbackWorkspace = aCallbackWorkspace;

    divider = BAUD_DIVIDER(SERIAL_PORTS_DEFAULT_BAUD);

    // Set the FIFO enable bit in the FCR register. This bit must be set for
    // proper UART operation.
    p_info->iUART->reg08.FCR = 7; // FCRFE|RFR|TFR

    // Set baudrate
    p_info->iUART->LCR |= 0x80;
    p_info->iUART->reg00.DLL = divider & 0x00ff;
    p_info->iUART->reg04.DLM = (divider >> 8) & 0x00ff;
    p_info->iUART->LCR &= ~0x80;

    // Set default mode (8 bits, 1 stop bit, no parity)
    p_info->iUART->LCR = 0x03;

    //Enable UART0 interrupts
    p_info->iUART->reg04.IER = 0x03; // Interrupts and TX and RX

    InterruptRegister(p_info->iInterruptChannel, p_info->iISR,
            p_info->iPriority, p->iHAL->iInterface.iName);

    //TBD: For now, leave serial port deactivated
    //    InterruptEnable(p_info->iInterruptChannel);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_SetSerialSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the serial port settings
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_Serial_Settings *aSettings -- Settings to use for serial port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_SetSerialSettings(
        void *aWorkspace,
        T_Serial_Settings *aSettings)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    const T_Serial_LPC1788_SerialInfo *p_info = p->iInfo;
    TUInt32 divider;

    // Record these settings
    p->iSettings = *aSettings;

    //TBD: Do the other settings!

    // Change the baud rate
    divider = BAUD_DIVIDER(aSettings->iBaud);
    p_info->iUART->LCR |= 0x80;
    p_info->iUART->reg00.DLL = divider & 0x00ff;
    p_info->iUART->reg04.DLM = (divider >> 8) & 0x00ff;
    p_info->iUART->LCR &= 0x00;

    switch (aSettings->iParity) {
        case SERIAL_PARITY_ODD:
            p_info->iUART->LCR |= ((1 << 3) | (0 << 4));
            break;
        case SERIAL_PARITY_EVEN:
            p_info->iUART->LCR |= ((1 << 3) | (1 << 4));
            break;
        case SERIAL_PARITY_MARK:
            p_info->iUART->LCR |= ((1 << 3) | (2 << 4));
            break;
        case SERIAL_PARITY_SPACE:
            p_info->iUART->LCR |= ((1 << 3) | (3 << 4));
            break;
        case SERIAL_PARITY_NONE:
            p_info->iUART->LCR |= ((0 << 3) | (0 << 4));
            break;
        default:
            return UEZ_ERROR_INVALID_PARAMETER;
    }

    switch (aSettings->iStopBit) {
        case SERIAL_STOP_BITS_1:
            p_info->iUART->LCR |= (0 << 2);
            break;
        case SERIAL_STOP_BITS_2:
            p_info->iUART->LCR |= (1 << 2);
            break;
        case SERIAL_STOP_BITS_1_POINT_5:
            return UEZ_ERROR_NOT_SUPPORTED;
        default:
            return UEZ_ERROR_INVALID_PARAMETER;
    }

    switch (aSettings->iWordLength) {
        case 5:
            p_info->iUART->LCR |= (0 << 0);
            break;
        case 6:
            p_info->iUART->LCR |= (1 << 0);
            break;
        case 7:
            p_info->iUART->LCR |= (2 << 0);
            break;
        case 8:
            p_info->iUART->LCR |= (3 << 0);
            break;
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_GetSerialSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the previously configured serial port
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_Serial_Settings *aSettings -- Settings used for serial port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_GetSerialSettings(
        void *aWorkspace,
        T_Serial_Settings *aSettings)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;

    // Record these settings
    *aSettings = p->iSettings;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_SetHandshakingControl
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the handshaking control for this serial port (if it has it)
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_serialHandshakingControl -- Control bits to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_SetHandshakingControl(
        void *aWorkspace,
        T_serialHandshakingControl aControl)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    const T_Serial_LPC1788_SerialInfo *p_info = p->iInfo;

    p_info->iUART->MCR = (aControl & 0xFF);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_GetHandshakingStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the handshaking control for this serial port (if it has it)
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_serialHandshakingStatus *aStatus -- Current status
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_Serial_GetHandshakingStatus(
        void *aWorkspace,
        T_serialHandshakingStatus *aStatus)
{
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;
    const T_Serial_LPC1788_SerialInfo *p_info = p->iInfo;

    *aStatus = (p_info->iUART->MSR & 0xFF);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC1788_Serial_GetStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the status of the port
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 * Outputs:
 *      T_serialStatusByte          -- Status byte returned
 *---------------------------------------------------------------------------*/
T_serialStatusByte LPC1788_Serial_GetStatus(void *aWorkspace)
{
    T_serialStatusByte status = 0;
    T_Serial_LPC1788_Workspace *p = (T_Serial_LPC1788_Workspace *)aWorkspace;

    // Only empty if both TEMT and THRE are empty
    if (p->iInfo->iUART->LSR & ((1 << 6) | (1 << 5))) {
        // not empty
    } else {
        status |= SERIAL_STATUS_TRANSMIT_EMPTY;
    }

    return status;
}

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
// List of serial port HAL interfaces
const HAL_Serial G_LPC1788_Serial_UART0 = {
        {
        "Serial:LCP1788 UART0",
        0x0100,
        LPC1788_Serial_InitializeWorkspace_UART0,
        sizeof(T_Serial_LPC1788_Workspace),
        },

        LPC1788_Serial_Configure,
        LPC1788_Serial_SetSerialSettings,
        LPC1788_Serial_GetSerialSettings,
        LPC1788_Serial_Activate,
        LPC1788_Serial_Deactivate,
        LPC1788_Serial_OutputByte,
        0,
        0,
        LPC1788_Serial_GetStatus, };

const HAL_Serial G_LPC1788_Serial_UART1 = {
        {
        "Serial:LCP1788 UART1",
        0x0100,
        LPC1788_Serial_InitializeWorkspace_UART1,
        sizeof(T_Serial_LPC1788_Workspace),
        },

        LPC1788_Serial_Configure,
        LPC1788_Serial_SetSerialSettings,
        LPC1788_Serial_GetSerialSettings,
        LPC1788_Serial_Activate,
        LPC1788_Serial_Deactivate,
        LPC1788_Serial_OutputByte,
        LPC1788_Serial_SetHandshakingControl,
        LPC1788_Serial_GetHandshakingStatus,
        LPC1788_Serial_GetStatus, };

const HAL_Serial G_LPC1788_Serial_UART2 = {
        {
        "Serial:LCP1788 UART2",
        0x0100,
        LPC1788_Serial_InitializeWorkspace_UART2,
        sizeof(T_Serial_LPC1788_Workspace),
        },

        LPC1788_Serial_Configure,
        LPC1788_Serial_SetSerialSettings,
        LPC1788_Serial_GetSerialSettings,
        LPC1788_Serial_Activate,
        LPC1788_Serial_Deactivate,
        LPC1788_Serial_OutputByte,
        0,
        0,
        LPC1788_Serial_GetStatus, };

const HAL_Serial G_LPC1788_Serial_UART3 = {
        {
        "Serial:LCP1788 UART3",
        0x0100,
        LPC1788_Serial_InitializeWorkspace_UART3,
        sizeof(T_Serial_LPC1788_Workspace),
        },

        LPC1788_Serial_Configure,
        LPC1788_Serial_SetSerialSettings,
        LPC1788_Serial_GetSerialSettings,
        LPC1788_Serial_Activate,
        LPC1788_Serial_Deactivate,
        LPC1788_Serial_OutputByte,
        0,
        0,
        LPC1788_Serial_GetStatus, };
	
const HAL_Serial G_LPC1788_Serial_UART4 = {
        {
        "Serial:LCP1788 UART4",
        0x0100,
        LPC1788_Serial_InitializeWorkspace_UART4,
        sizeof(T_Serial_LPC1788_Workspace),
        },

        LPC1788_Serial_Configure,
        LPC1788_Serial_SetSerialSettings,
        LPC1788_Serial_GetSerialSettings,
        LPC1788_Serial_Activate,
        LPC1788_Serial_Deactivate,
        LPC1788_Serial_OutputByte,
        0,
        0,
        LPC1788_Serial_GetStatus, };

const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART0 = {
        UART0_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_UART0_BASE,
        (TISRFPtr)ISerialUART0,
        3, // PCUART0
        };

const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART1 = {
        UART1_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_UART1_BASE,
        (TISRFPtr)ISerialUART1,
        4, // PCUART1
        };

const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART2 = {
        UART2_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_UART2_BASE,
        (TISRFPtr)ISerialUART2,
        24, // PCUART2
        };

const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART3 = {
        UART3_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_UART3_BASE,
        (TISRFPtr)ISerialUART3,
        25, // PCUART3
        };

const T_Serial_LPC1788_SerialInfo G_LPC1788_Serial_Info_UART4 = {
        UART4_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_UART4_BASE,
        (TISRFPtr)ISerialUART4,
        8, // PCUART4
        };
	
/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1788_UART0_Require(
        T_uezGPIOPortPin aPinTXD0,
        T_uezGPIOPortPin aPinRXD0)
{
    static const T_LPC1788_IOCON_ConfigList txd0[] = {
            {GPIO_P0_0,   IOCON_D_DEFAULT(4)},
            {GPIO_P0_2,   IOCON_D_DEFAULT(1)},
    };
    static const T_LPC1788_IOCON_ConfigList rxd0[] = {
            {GPIO_P0_1,   IOCON_D_DEFAULT(4)},
            {GPIO_P0_3,   IOCON_D_DEFAULT(1)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART0", (T_halInterface *)&G_LPC1788_Serial_UART0, 0,
            0);
    LPC1788_IOCON_ConfigPinOrNone(aPinTXD0, txd0, ARRAY_COUNT(txd0));
    LPC1788_IOCON_ConfigPinOrNone(aPinRXD0, rxd0, ARRAY_COUNT(rxd0));
}

void LPC1788_UART1_Require(
        T_uezGPIOPortPin aPinTXD1,
        T_uezGPIOPortPin aPinRXD1,
        T_uezGPIOPortPin aPinCTS,
        T_uezGPIOPortPin aPinDCD,
        T_uezGPIOPortPin aPinDSR,
        T_uezGPIOPortPin aPinDTR,
        T_uezGPIOPortPin aPinRI,
        T_uezGPIOPortPin aPinRTS)
{
    static const T_LPC1788_IOCON_ConfigList txd1[] = {
            {GPIO_P0_15, IOCON_D_DEFAULT(1)},
            {GPIO_P2_0,  IOCON_D_DEFAULT(2)},
            {GPIO_P3_16, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1788_IOCON_ConfigList rxd1[] = {
            {GPIO_P0_16, IOCON_D_DEFAULT(1)},
            {GPIO_P2_1,  IOCON_D_DEFAULT(2)},
            {GPIO_P3_17, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1788_IOCON_ConfigList cts[] = {
            {GPIO_P0_17, IOCON_D_DEFAULT(1)},
            {GPIO_P2_2,  IOCON_D_DEFAULT(2)},
            {GPIO_P2_8,  IOCON_D_DEFAULT(3)},
            {GPIO_P3_18, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1788_IOCON_ConfigList dcd[] = {
            {GPIO_P0_18, IOCON_D_DEFAULT(1)},
            {GPIO_P2_3,  IOCON_D_DEFAULT(2)},
            {GPIO_P3_19, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1788_IOCON_ConfigList dsr[] = {
            {GPIO_P0_19, IOCON_D_DEFAULT(1)},
            {GPIO_P2_4,  IOCON_D_DEFAULT(2)},
            {GPIO_P3_20, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1788_IOCON_ConfigList dtr[] = {
            {GPIO_P0_20, IOCON_D_DEFAULT(1)},
            {GPIO_P2_5,  IOCON_D_DEFAULT(2)},
            {GPIO_P3_21, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1788_IOCON_ConfigList ri[] = {
            {GPIO_P0_21, IOCON_D_DEFAULT(1)},
            {GPIO_P2_6,  IOCON_D_DEFAULT(2)},
            {GPIO_P3_22, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1788_IOCON_ConfigList rts[] = {
            {GPIO_P0_6,  IOCON_D_DEFAULT(4)},
            {GPIO_P0_22, IOCON_D_DEFAULT(1)},
            {GPIO_P2_7,  IOCON_D_DEFAULT(2)},
            {GPIO_P3_30, IOCON_D_DEFAULT(3)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART1", (T_halInterface *)&G_LPC1788_Serial_UART1, 0,
            0);
    LPC1788_IOCON_ConfigPinOrNone(aPinTXD1, txd1, ARRAY_COUNT(txd1));
    LPC1788_IOCON_ConfigPinOrNone(aPinRXD1, rxd1, ARRAY_COUNT(rxd1));
    LPC1788_IOCON_ConfigPinOrNone(aPinCTS, cts, ARRAY_COUNT(cts));
    LPC1788_IOCON_ConfigPinOrNone(aPinDCD, dcd, ARRAY_COUNT(dcd));
    LPC1788_IOCON_ConfigPinOrNone(aPinDSR, dsr, ARRAY_COUNT(dsr));
    LPC1788_IOCON_ConfigPinOrNone(aPinDTR, dtr, ARRAY_COUNT(dtr));
    LPC1788_IOCON_ConfigPinOrNone(aPinRI, ri, ARRAY_COUNT(ri));
    LPC1788_IOCON_ConfigPinOrNone(aPinRTS, rts, ARRAY_COUNT(rts));
}

void LPC1788_UART2_Require(
        T_uezGPIOPortPin aPinTXD2,
        T_uezGPIOPortPin aPinRXD2)
{
    static const T_LPC1788_IOCON_ConfigList txd2[] = {
            {GPIO_P0_10, IOCON_D_DEFAULT(1)},
            {GPIO_P2_8,  IOCON_D_DEFAULT(2)},
            {GPIO_P4_22, IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1788_IOCON_ConfigList rxd2[] = {
            {GPIO_P0_11, IOCON_D_DEFAULT(1)},
            {GPIO_P2_9,  IOCON_D_DEFAULT(2)},
            {GPIO_P4_23, IOCON_D_DEFAULT(2)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART2", (T_halInterface *)&G_LPC1788_Serial_UART2, 0,
            0);
    LPC1788_IOCON_ConfigPinOrNone(aPinTXD2, txd2, ARRAY_COUNT(txd2));
    LPC1788_IOCON_ConfigPinOrNone(aPinRXD2, rxd2, ARRAY_COUNT(rxd2));
}

void LPC1788_UART3_Require(
        T_uezGPIOPortPin aPinTXD3,
        T_uezGPIOPortPin aPinRXD3)
{
    static const T_LPC1788_IOCON_ConfigList txd3[] = {
            {GPIO_P0_0,  IOCON_D_DEFAULT(2)},
            {GPIO_P0_2,  IOCON_D_DEFAULT(2)},
            {GPIO_P0_25, IOCON_A_DEFAULT(3)},
            {GPIO_P4_28, IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1788_IOCON_ConfigList rxd3[] = {
            {GPIO_P0_1,  IOCON_D_DEFAULT(2)},
            {GPIO_P0_3,  IOCON_D_DEFAULT(2)},
            {GPIO_P0_26, IOCON_A_DEFAULT(3)},
            {GPIO_P4_29, IOCON_D_DEFAULT(2)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART3", (T_halInterface *)&G_LPC1788_Serial_UART3, 0,
            0);
    LPC1788_IOCON_ConfigPinOrNone(aPinTXD3, txd3, ARRAY_COUNT(txd3));
    LPC1788_IOCON_ConfigPinOrNone(aPinRXD3, rxd3, ARRAY_COUNT(rxd3));
}

void LPC1788_UART4_Require(
        T_uezGPIOPortPin aPinTXD4,
        T_uezGPIOPortPin aPinRXD4)
{
    static const T_LPC1788_IOCON_ConfigList txd4[] = {
            {GPIO_P0_22,  IOCON_D_DEFAULT(3)},
            {GPIO_P1_29,  IOCON_D_DEFAULT(5)},
            {GPIO_P5_4,   IOCON_A_DEFAULT(4)},
    };
    static const T_LPC1788_IOCON_ConfigList rxd4[] = {
            {GPIO_P2_9,  IOCON_D_DEFAULT(3)},
            {GPIO_P5_3,  IOCON_D_DEFAULT(4)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART4", (T_halInterface *)&G_LPC1788_Serial_UART4, 0,
            0);
    LPC1788_IOCON_ConfigPinOrNone(aPinTXD4, txd4, ARRAY_COUNT(txd4));
    LPC1788_IOCON_ConfigPinOrNone(aPinRXD4, rxd4, ARRAY_COUNT(rxd4));
}

/*===========================================================================*
 * END OF FILE: SerialPorts.c
 *===========================================================================*/
