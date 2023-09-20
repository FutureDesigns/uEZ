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
#include <HAL/HAL.h>
#include <HAL/Interrupt.h>
#include <uEZTypes.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Serial.h>

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
    __IO uint32_t  DLL;                     /*!< (@ 0x400xx000) Divisor Latch LSB. Least significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1). */
    __O  uint32_t  THR;                     /*!< (@ 0x400xx000) Transmit Holding Register. The next character to be transmitted is written here (DLAB = 0). */
    __I  uint32_t  RBR;                     /*!< (@ 0x400xx000) Receiver Buffer Register. Contains the next received character to be read (DLAB = 0). */
  } reg00;
  
  union {
    __IO uint32_t IER;                      /*!< (@ 0x400xx004) Interrupt Enable Register. Contains individual interrupt enable bits for the 7 potential UART interrupts (DLAB = 0). */
    __IO uint32_t DLM;                      /*!< (@ 0x400xx004) Divisor Latch MSB. Most significant byte of the baud rate divisor value. The full divisor is used to generate a baud rate from the fractional rate divider (DLAB = 1). */
  } reg04;
  
  union {
    __O  uint32_t FCR;                      /*!< (@ 0x400xx008) FIFO Control Register. Controls UART FIFO usage and modes. */
    __I  uint32_t IIR;                      /*!< (@ 0x400xx008) Interrupt ID Register. Identifies which interrupt(s) are pending. */
  } reg08;
  __IO uint32_t LCR;                        /*!< (@ 0x400xx00C) Line Control Register. Contains controls for frame formatting and break generation. */
  __IO uint32_t  MCR;                       /*!< (@ 0x40082010) Modem Control Register. Contains controls for flow control handshaking and loopback mode. */
  __I  uint32_t LSR;                        /*!< (@ 0x400xx014) Line Status Register. Contains flags for transmit and receive status, including line errors. */
  __I  uint32_t  MSR;                       /*!< (@ 0x40082018) Modem Status Register. Contains handshake signal status flags. */
  __IO uint32_t SCR;                        /*!< (@ 0x400xx01C) Scratch Pad Register. Eight-bit temporary storage for software. */
  __IO uint32_t ACR;                        /*!< (@ 0x400xx020) Auto-baud Control Register. Contains controls for the auto-baud feature. */
  __IO uint32_t ICR;                        /*!< (@ 0x400xx024) IrDA control register (UART3 only) */
  __IO uint32_t FDR;                        /*!< (@ 0x400xx028) Fractional Divider Register. Generates a clock input for the baud rate divider. */
  __IO uint32_t OSR;                        /*!< (@ 0x400xx02C) Oversampling Register. Controls the degree of oversampling during each bit time. */
  __I  uint32_t  RESERVED2[4];
  __IO uint32_t HDEN;                       /*!< (@ 0x400xx03C) Half-duplex enable Register */
  __I  uint32_t RESERVED3[1];
  __IO uint32_t SCICTRL;                    /*!< (@ 0x400xx048) Smart card interface control register */
  __IO uint32_t RS485CTRL;                  /*!< (@ 0x400xx04C) RS-485/EIA-485 Control. Contains controls to configure various aspects of RS-485/EIA-485 modes. */
  __IO uint32_t RS485ADRMATCH;              /*!< (@ 0x400xx050) RS-485/EIA-485 address match. Contains the address match value for RS-485/EIA-485 mode. */
  __IO uint32_t RS485DLY;                   /*!< (@ 0x400xx054) RS-485/EIA-485 direction control delay. */
  __IO uint32_t SYNCCTRL;                   /*!< (@ 0x400xx058) Synchronous mode control register. */
  __IO uint32_t TER;
} T_16550UART;

// Entry of serial port settings
typedef struct {
    TUInt32 iInterruptChannel;
    T_irqPriority iPriority;
    T_16550UART *iUART;
    TISRFPtr iISR;
} T_Serial_LPC43xx_SerialInfo;

typedef struct {
    const HAL_Serial *iHAL;
    const T_Serial_LPC43xx_SerialInfo *iInfo;
    HAL_Serial_Callback_Transmit_Empty iTransmitEmptyFunc;
    HAL_Serial_Callback_Received_Byte iReceivedByteFunc;
    void *iCallbackWorkspace;
    T_Serial_Settings iSettings;
} T_Serial_LPC43xx_Workspace;

/*---------------------------------------------------------------------------*
 * Function Prototypes:
 *---------------------------------------------------------------------------*/
//#define ISerialGrab(entry)         InterruptDisable(entry->iInterruptChannel);
//#define ISerialRelease(entry)      InterruptEnable(entry->iInterruptChannel);
IRQ_ROUTINE(ISerialUART0);
IRQ_ROUTINE(ISerialUART1);
IRQ_ROUTINE(ISerialUART2);
IRQ_ROUTINE(ISerialUART3);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern const HAL_Serial G_LPC43xx_Serial_UART0;
extern const HAL_Serial G_LPC43xx_Serial_UART1;
extern const HAL_Serial G_LPC43xx_Serial_UART2;
extern const HAL_Serial G_LPC43xx_Serial_UART3;

extern const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART0;
extern const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART1;
extern const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART2;
extern const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART3;

T_Serial_LPC43xx_Workspace *G_UART0Workspace = 0;
T_Serial_LPC43xx_Workspace *G_UART1Workspace = 0;
T_Serial_LPC43xx_Workspace *G_UART2Workspace = 0;
T_Serial_LPC43xx_Workspace *G_UART3Workspace = 0;

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
static void ISerialProcessInterrupt(T_Serial_LPC43xx_Workspace *p)
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
T_uezError LPC43xx_Serial_OutputByte(void *aWorkspace, TUInt8 aByte)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
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
T_uezError LPC43xx_Serial_Deactivate(void *aWorkspace)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
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
T_uezError LPC43xx_Serial_Activate(void *aWorkspace)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
    InterruptEnable(p->iInfo->iInterruptChannel);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC43xx_Serial_InitializeWorkspace_UART0
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART0
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Serial_InitializeWorkspace_UART0(void *aWorkspace)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
    G_UART0Workspace = p;
    p->iInfo = &G_LPC43xx_Serial_Info_UART0;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    //Turn on peripheral clock
    LPC_CGU->BASE_UART0_CLK = (9<<24) | (1<<11);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC43xx_Serial_InitializeWorkspace_UART1
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART1
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Serial_InitializeWorkspace_UART1(void *aWorkspace)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
    G_UART1Workspace = p;
    p->iInfo = &G_LPC43xx_Serial_Info_UART1;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    //Turn on peripheral clock
    LPC_CGU->BASE_UART1_CLK = (9<<24) | (1<<11);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC43xx_Serial_InitializeWorkspace_UART2
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART2
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Serial_InitializeWorkspace_UART2(void *aWorkspace)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
    G_UART2Workspace = p;
    p->iInfo = &G_LPC43xx_Serial_Info_UART2;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    //Turn on peripheral clock
    LPC_CGU->BASE_UART2_CLK = (9<<24) | (1<<11);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC43xx_Serial_InitializeWorkspace_UART3
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART3
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Serial_InitializeWorkspace_UART3(void *aWorkspace)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
    G_UART3Workspace = p;
    p->iInfo = &G_LPC43xx_Serial_Info_UART3;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    //Turn on peripheral clock
    LPC_CGU->BASE_UART3_CLK = (9<<24) | (1<<11);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC43xx_Serial_Configure
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
T_uezError LPC43xx_Serial_Configure(
        void *aWorkspace,
        void *aCallbackWorkspace,
        HAL_Serial_Callback_Received_Byte aReceivedByteCallback,
        HAL_Serial_Callback_Transmit_Empty aTransmitEmptyCallback)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
    const T_Serial_LPC43xx_SerialInfo *p_info = p->iInfo;
    TUInt32 divider;

    InterruptDisable(p->iInfo->iInterruptChannel);


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
 * Routine: LPC43xx_Serial_SetSerialSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the serial port settings
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_Serial_Settings *aSettings -- Settings to use for serial port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Serial_SetSerialSettings(
        void *aWorkspace,
        T_Serial_Settings *aSettings)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
    const T_Serial_LPC43xx_SerialInfo *p_info = p->iInfo;
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

	switch (aSettings->iFlowControl) {
        case SERIAL_FLOW_CONTROL_NONE:
            // default register settings are for none
            p_info->iUART->MCR |= (0 << 6); // Disable auto RTS
            p_info->iUART->MCR |= (0 << 7); // Disable auto CTS
            break;
        case SERIAL_FLOW_CONTROL_XON_XOFF:
            return UEZ_ERROR_NOT_SUPPORTED; // TODO
        case SERIAL_FLOW_CONTROL_HARDWARE:
            p_info->iUART->MCR |= (1 << 6); // Auto RTS enable bit
            p_info->iUART->MCR |= (1 << 7); // Auto CTS enable bit
            break;
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
    }    
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC43xx_Serial_GetSerialSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the previously configured serial port
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_Serial_Settings *aSettings -- Settings used for serial port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Serial_GetSerialSettings(
        void *aWorkspace,
        T_Serial_Settings *aSettings)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;

    // Record these settings
    *aSettings = p->iSettings;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC43xx_Serial_SetHandshakingControl
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the handshaking control for this serial port (if it has it)
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_serialHandshakingControl -- Control bits to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Serial_SetHandshakingControl(
        void *aWorkspace,
        T_serialHandshakingControl aControl)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
    const T_Serial_LPC43xx_SerialInfo *p_info = p->iInfo;

    p_info->iUART->MCR = (aControl & 0xFF);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC43xx_Serial_GetHandshakingStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the handshaking control for this serial port (if it has it)
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_serialHandshakingStatus *aStatus -- Current status
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_Serial_GetHandshakingStatus(
        void *aWorkspace,
        T_serialHandshakingStatus *aStatus)
{
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;
    const T_Serial_LPC43xx_SerialInfo *p_info = p->iInfo;

    *aStatus = (p_info->iUART->MSR & 0xFF);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine: LPC43xx_Serial_GetStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the status of the port
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 * Outputs:
 *      T_serialStatusByte          -- Status byte returned
 *---------------------------------------------------------------------------*/
T_serialStatusByte LPC43xx_Serial_GetStatus(void *aWorkspace)
{
    T_serialStatusByte status = 0;
    T_Serial_LPC43xx_Workspace *p = (T_Serial_LPC43xx_Workspace *)aWorkspace;

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
const HAL_Serial G_LPC43xx_Serial_UART0 = {
        {
        "Serial:LPC43XX UART0",
        0x0100,
        LPC43xx_Serial_InitializeWorkspace_UART0,
        sizeof(T_Serial_LPC43xx_Workspace),
        },

        LPC43xx_Serial_Configure,
        LPC43xx_Serial_SetSerialSettings,
        LPC43xx_Serial_GetSerialSettings,
        LPC43xx_Serial_Activate,
        LPC43xx_Serial_Deactivate,
        LPC43xx_Serial_OutputByte,
        0,
        0,
        LPC43xx_Serial_GetStatus, };

const HAL_Serial G_LPC43xx_Serial_UART1 = {
        {
        "Serial:LPC43XX UART1",
        0x0100,
        LPC43xx_Serial_InitializeWorkspace_UART1,
        sizeof(T_Serial_LPC43xx_Workspace),
        },

        LPC43xx_Serial_Configure,
        LPC43xx_Serial_SetSerialSettings,
        LPC43xx_Serial_GetSerialSettings,
        LPC43xx_Serial_Activate,
        LPC43xx_Serial_Deactivate,
        LPC43xx_Serial_OutputByte,
        LPC43xx_Serial_SetHandshakingControl,
        LPC43xx_Serial_GetHandshakingStatus,
        LPC43xx_Serial_GetStatus, };

const HAL_Serial G_LPC43xx_Serial_UART2 = {
        {
        "Serial:LPC43XX UART2",
        0x0100,
        LPC43xx_Serial_InitializeWorkspace_UART2,
        sizeof(T_Serial_LPC43xx_Workspace),
        },

        LPC43xx_Serial_Configure,
        LPC43xx_Serial_SetSerialSettings,
        LPC43xx_Serial_GetSerialSettings,
        LPC43xx_Serial_Activate,
        LPC43xx_Serial_Deactivate,
        LPC43xx_Serial_OutputByte,
        0,
        0,
        LPC43xx_Serial_GetStatus, };

const HAL_Serial G_LPC43xx_Serial_UART3 = {
        {
        "Serial:LPC43XX UART3",
        0x0100,
        LPC43xx_Serial_InitializeWorkspace_UART3,
        sizeof(T_Serial_LPC43xx_Workspace),
        },

        LPC43xx_Serial_Configure,
        LPC43xx_Serial_SetSerialSettings,
        LPC43xx_Serial_GetSerialSettings,
        LPC43xx_Serial_Activate,
        LPC43xx_Serial_Deactivate,
        LPC43xx_Serial_OutputByte,
        0,
        0,
        LPC43xx_Serial_GetStatus, };
	



#ifdef CORE_M4
const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART0 = {
        USART0_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_USART0_BASE,
        (TISRFPtr)ISerialUART0,
        };

const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART1 = {
        UART1_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_UART1_BASE,
        (TISRFPtr)ISerialUART1,
        };

const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART2 = {
        USART2_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_USART2_BASE,
        (TISRFPtr)ISerialUART2,
        };

const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART3 = {
        USART3_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_USART3_BASE,
        (TISRFPtr)ISerialUART3,
        };
#endif
#ifdef CORE_M0
const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART0 = {
        M0_USART0_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_USART0_BASE,
        (TISRFPtr)ISerialUART0,
        };

const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART1 = {
        M0_UART1_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_UART1_BASE,
        (TISRFPtr)ISerialUART1,
        };

const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART2 = {
        M0_USART2_OR_C_CAN1_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_USART2_BASE,
        (TISRFPtr)ISerialUART2,
        };

const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART3 = {
        M0_USART3_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_USART3_BASE,
        (TISRFPtr)ISerialUART3,
        };
#endif
#ifdef CORE_M0SUB
const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART0 = {
        M0S_USART0_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_USART0_BASE,
        (TISRFPtr)ISerialUART0,
        };

const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART1 = {
        M0S_UART1_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_UART1_BASE,
        (TISRFPtr)ISerialUART1,
        };

const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART2 = {
        MS0_USART2_OR_C_CAN1_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_USART2_BASE,
        (TISRFPtr)ISerialUART2,
        };

const T_Serial_LPC43xx_SerialInfo G_LPC43xx_Serial_Info_UART3 = {
        M0S_USART3_IRQn,
        INTERRUPT_PRIORITY_HIGH,
        (T_16550UART *)LPC_USART3_BASE,
        (TISRFPtr)ISerialUART3,
        };
#endif


/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC43xx_UART0_Require(
        T_uezGPIOPortPin aPinTXD0,
        T_uezGPIOPortPin aPinRXD0,
        T_uezGPIOPortPin aPinDIR0,
        T_uezGPIOPortPin aPinUCLK0)
{
    static const T_LPC43xx_SCU_ConfigList txd0[] = {
            {GPIO_P5_0,   SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P3_3,   SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P5_18,   SCU_NORMAL_DRIVE_DEFAULT(7)},
            {GPIO_P7_24,   SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    static const T_LPC43xx_SCU_ConfigList rxd0[] = {
            {GPIO_P5_1,   SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P3_4,   SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_25,   SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P4_11,   SCU_NORMAL_DRIVE_DEFAULT(7)},
    };
    static const T_LPC43xx_SCU_ConfigList dir0[] = {
            {GPIO_P5_6, SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P3_1, SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_23, SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    static const T_LPC43xx_SCU_ConfigList uclk0[] = {
            {GPIO_P5_2, SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P3_0, SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_22, SCU_NORMAL_DRIVE_DEFAULT(1)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART0", (T_halInterface *)&G_LPC43xx_Serial_UART0, 0,
            0);

    LPC43xx_SCU_ConfigPinOrNone(aPinTXD0, txd0, ARRAY_COUNT(txd0));
    LPC43xx_SCU_ConfigPinOrNone(aPinRXD0, rxd0, ARRAY_COUNT(rxd0));
    LPC43xx_SCU_ConfigPinOrNone(aPinDIR0, dir0, ARRAY_COUNT(dir0));
    LPC43xx_SCU_ConfigPinOrNone(aPinUCLK0, uclk0, ARRAY_COUNT(uclk0));
}

void LPC43xx_UART1_Require(
        T_uezGPIOPortPin aPinTXD1,
        T_uezGPIOPortPin aPinRXD1,
        T_uezGPIOPortPin aPinCTS,
        T_uezGPIOPortPin aPinDCD,
        T_uezGPIOPortPin aPinDSR,
        T_uezGPIOPortPin aPinDTR,
        T_uezGPIOPortPin aPinRI,
        T_uezGPIOPortPin aPinRTS)
{
    static const T_LPC43xx_SCU_ConfigList cts1[] = {
            {GPIO_P2_13 , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P7_7  , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P1_4  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P6_1  , SCU_NORMAL_DRIVE_DEFAULT(2)},
    };
    static const T_LPC43xx_SCU_ConfigList dcd1[] = {
            {GPIO_P1_5  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P2_14 , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P6_10 , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_9  , SCU_NORMAL_DRIVE_DEFAULT(2)},
    };
    static const T_LPC43xx_SCU_ConfigList dsr1[] = {
            {GPIO_P2_9  , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P1_0  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P6_9  , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_8  , SCU_NORMAL_DRIVE_DEFAULT(2)},
    };
    static const T_LPC43xx_SCU_ConfigList dtr1[] = {
            {GPIO_P1_1  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P2_10 , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P6_11 , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_10 , SCU_NORMAL_DRIVE_DEFAULT(2)},
    };
    static const T_LPC43xx_SCU_ConfigList ri1[] = {
            {GPIO_P1_3  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P2_12 , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P6_0  , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_6  , SCU_NORMAL_DRIVE_DEFAULT(2)},
    };
    static const T_LPC43xx_SCU_ConfigList rts1[] = {
            {GPIO_P1_2  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P2_11 , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P6_2  , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_5  , SCU_NORMAL_DRIVE_DEFAULT(2)},
    };
    static const T_LPC43xx_SCU_ConfigList rxd1[] = {
            {GPIO_P1_7  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P1_15 , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P2_7  , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P6_13 , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_12 , SCU_NORMAL_DRIVE_DEFAULT(2)},
    };
    static const T_LPC43xx_SCU_ConfigList txd1[] = {
            {GPIO_P1_6  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P1_14 , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P2_15 , SCU_NORMAL_DRIVE_DEFAULT(4)},
            {GPIO_P6_12 , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P7_11 , SCU_NORMAL_DRIVE_DEFAULT(2)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART1", (T_halInterface *)&G_LPC43xx_Serial_UART1, 0,
            0);

    LPC43xx_SCU_ConfigPinOrNone(aPinTXD1, txd1, ARRAY_COUNT(txd1));
    LPC43xx_SCU_ConfigPinOrNone(aPinRXD1, rxd1, ARRAY_COUNT(rxd1));
    LPC43xx_SCU_ConfigPinOrNone(aPinCTS, cts1, ARRAY_COUNT(cts1));
    LPC43xx_SCU_ConfigPinOrNone(aPinDCD, dcd1, ARRAY_COUNT(dcd1));
    LPC43xx_SCU_ConfigPinOrNone(aPinDSR, dsr1, ARRAY_COUNT(dsr1));
    LPC43xx_SCU_ConfigPinOrNone(aPinDTR, dtr1, ARRAY_COUNT(dtr1));
    LPC43xx_SCU_ConfigPinOrNone(aPinRI, ri1, ARRAY_COUNT(ri1));
    LPC43xx_SCU_ConfigPinOrNone(aPinRTS, rts1, ARRAY_COUNT(rts1));
}

void LPC43xx_UART2_Require(
        T_uezGPIOPortPin aPinTXD2,
        T_uezGPIOPortPin aPinRXD2,
        T_uezGPIOPortPin aPinDIR2,
        T_uezGPIOPortPin aPinUCLK2)
{
    static const T_LPC43xx_SCU_ConfigList dir2[] = {
            {GPIO_P0_13 , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P1_13 , SCU_NORMAL_DRIVE_DEFAULT(7)},
    };
    static const T_LPC43xx_SCU_ConfigList rxd2[] = {
            {GPIO_P0_3  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P3_10 , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P4_9  , SCU_NORMAL_DRIVE_DEFAULT(3)},
    };
    static const T_LPC43xx_SCU_ConfigList txd2[] = {
            {GPIO_P0_2  , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P3_9  , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P4_8  , SCU_NORMAL_DRIVE_DEFAULT(3)},
    };
    static const T_LPC43xx_SCU_ConfigList uclk2[] = {
            {GPIO_P0_12 , SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P1_12 , SCU_NORMAL_DRIVE_DEFAULT(7)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART2", (T_halInterface *)&G_LPC43xx_Serial_UART2, 0,
            0);
    LPC43xx_SCU_ConfigPinOrNone(aPinTXD2, txd2, ARRAY_COUNT(txd2));
    LPC43xx_SCU_ConfigPinOrNone(aPinRXD2, rxd2, ARRAY_COUNT(rxd2));
    LPC43xx_SCU_ConfigPinOrNone(aPinDIR2, dir2, ARRAY_COUNT(dir2));
    LPC43xx_SCU_ConfigPinOrNone(aPinUCLK2, uclk2, ARRAY_COUNT(uclk2));
}

void LPC43xx_UART3_Require(
        T_uezGPIOPortPin aPinTXD3,
        T_uezGPIOPortPin aPinRXD3,
        T_uezGPIOPortPin aPinBAUD3,
        T_uezGPIOPortPin aPinDIR3,
        T_uezGPIOPortPin aPinUCLK3)
{
    static const T_LPC43xx_SCU_ConfigList buad3[] = {
            {GPIO_P1_10 , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P2_3  , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P7_21 , SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    static const T_LPC43xx_SCU_ConfigList dir3[] = {
            {GPIO_P5_7  , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P2_4  , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P7_20 , SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    static const T_LPC43xx_SCU_ConfigList rxd3[] = {
            {GPIO_P1_11 , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P5_4  , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P2_2  , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P5_17 , SCU_NORMAL_DRIVE_DEFAULT(7)},
            {GPIO_P7_18 , SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    static const T_LPC43xx_SCU_ConfigList txd3[] = {
            {GPIO_P0_14 , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P5_3  , SCU_NORMAL_DRIVE_DEFAULT(2)},
            {GPIO_P2_1  , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P4_15 , SCU_NORMAL_DRIVE_DEFAULT(7)},
            {GPIO_P7_17 , SCU_NORMAL_DRIVE_DEFAULT(1)},
    };
    static const T_LPC43xx_SCU_ConfigList uclk3[] = {
            {GPIO_P0_7  , SCU_NORMAL_DRIVE_DEFAULT(5)},
            {GPIO_P2_0  , SCU_NORMAL_DRIVE_DEFAULT(6)},
            {GPIO_P7_19 , SCU_NORMAL_DRIVE_DEFAULT(1)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART3", (T_halInterface *)&G_LPC43xx_Serial_UART3, 0,
            0);

    LPC43xx_SCU_ConfigPinOrNone(aPinTXD3, txd3, ARRAY_COUNT(txd3));
    LPC43xx_SCU_ConfigPinOrNone(aPinRXD3, rxd3, ARRAY_COUNT(rxd3));
    LPC43xx_SCU_ConfigPinOrNone(aPinBAUD3, buad3, ARRAY_COUNT(buad3));
    LPC43xx_SCU_ConfigPinOrNone(aPinDIR3, dir3, ARRAY_COUNT(dir3));
    LPC43xx_SCU_ConfigPinOrNone(aPinUCLK3, uclk3, ARRAY_COUNT(uclk3));
}

/*===========================================================================*
 * END OF FILE: SerialPorts.c
 *===========================================================================*/
