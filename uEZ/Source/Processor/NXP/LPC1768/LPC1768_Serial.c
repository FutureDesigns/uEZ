/*-------------------------------------------------------------------------*
 * File:  Serial.c
 *-------------------------------------------------------------------------*
 * Description:
 *      LPC23xx 16550(?) Style serial ports
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
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <uEZTypes.h>
#include <HAL/HAL.h>
#include <HAL/Interrupt.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_Serial.h>
#include <uEZPlatformAPI.h>
// TBD:  not here!
#include <uEZRTOS.h>

/*---------------------------------------------------------------------------*
 * Constants and Macros:
 *---------------------------------------------------------------------------*/
#define BAUD_DIVIDER(b)         ((PROCESSOR_OSCILLATOR_FREQUENCY/1)/((b)*16))

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
// Entry of serial port settings
typedef struct {
    TUInt32 iInterruptChannel;
    T_irqPriority iPriority;
    LPC_UART1_TypeDef *iUART;
    TISRFPtr iISR;
} T_Serial_LPC1768_SerialInfo;

typedef struct {
    const HAL_Serial *iHAL;
    const T_Serial_LPC1768_SerialInfo *iInfo;
    HAL_Serial_Callback_Transmit_Empty iTransmitEmptyFunc;
    HAL_Serial_Callback_Received_Byte iReceivedByteFunc;
    void *iCallbackWorkspace;
    T_Serial_Settings iSettings;
} T_Serial_LPC1768_Workspace;

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
extern const HAL_Serial G_LPC1768_Serial_UART0;
extern const HAL_Serial G_LPC1768_Serial_UART1;
extern const HAL_Serial G_LPC1768_Serial_UART2;
extern const HAL_Serial G_LPC1768_Serial_UART3;

extern const T_Serial_LPC1768_SerialInfo G_LPC1768_Serial_Info_UART0;
extern const T_Serial_LPC1768_SerialInfo G_LPC1768_Serial_Info_UART1;
extern const T_Serial_LPC1768_SerialInfo G_LPC1768_Serial_Info_UART2;
extern const T_Serial_LPC1768_SerialInfo G_LPC1768_Serial_Info_UART3;

T_Serial_LPC1768_Workspace *G_UART0Workspace=0;
T_Serial_LPC1768_Workspace *G_UART1Workspace=0;
T_Serial_LPC1768_Workspace *G_UART2Workspace=0;
T_Serial_LPC1768_Workspace *G_UART3Workspace=0;

/*---------------------------------------------------------------------------*
 * Routine:  ISerialProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Generic routine for handling any of the serial ports (as passed
 *      into the routine.
 *      NOTE:  This routine is ALWAYS processed inside of a interrupt
 *              service routine.
 * Inputs:
 *      T_SerialPortEntry *p      -- Port's definition to process interrupt
 *---------------------------------------------------------------------------*/
static void ISerialProcessInterrupt(T_Serial_LPC1768_Workspace *p)
{
    TUInt8 iir;

    // Process states until completely done (no more interrupts)
    while (((iir = p->iInfo->iUART->IIR) & 0x01) == 0) {
        // Process that state or return
        switch ((iir>>1) & 7)
        {
            case 0x1:
                // Just ran out of bytes, please transmit more
                // (or we stop here)
                p->iTransmitEmptyFunc(p->iCallbackWorkspace, 8);
                break;
            case 0x2:
                // We have received 1 or more bytes.  Please do
                // something with this received byte
                p->iReceivedByteFunc(
                    p->iCallbackWorkspace,
                    p->iInfo->iUART->RBR);
                break;
            case 0x0:  //Modem interrupt
            case 0x3:  //Receive line status interrupt (RDA)
            case 0x6:  //Character time out indicator interrupt (CTI)
            default:
                // Ignore rest of states
                return;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ISerialUART0
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the UART0 interrupt and forward to ISerialProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ISerialUART0)
{
    IRQ_START();
    ISerialProcessInterrupt(G_UART0Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  ISerialUART1
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the UART1 interrupt and forward to ISerialProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ISerialUART1)
{
    IRQ_START();
    ISerialProcessInterrupt(G_UART1Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  ISerialUART2
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the UART2 interrupt and forward to ISerialProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ISerialUART2)
{
    IRQ_START();
    ISerialProcessInterrupt(G_UART2Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  ISerialUART3
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the UART3 interrupt and forward to ISerialProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(ISerialUART3)
{
    IRQ_START();
    ISerialProcessInterrupt(G_UART3Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  SerialSendChar
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a single character out the given serial port.
 *      NOTE:  Does not block even if output queue is full!
 * Inputs:
 *      T_serialPort aPort        -- Serial port to use
 *      TUInt8 c                  -- Character to send
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_OutputByte(void *aWorkspace, TUInt8 aByte)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    p->iInfo->iUART->THR = aByte;
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SerialDisable
 *---------------------------------------------------------------------------*
 * Description:
 *      Disable serial processing on this port.
 * Inputs:
 *      T_serialPort aPort        -- Serial port to disable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_Deactivate(void *aWorkspace)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    InterruptDisable(p->iInfo->iInterruptChannel);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SerialEnable
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable serial processing on this port.
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_Activate(void *aWorkspace)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    InterruptEnable(p->iInfo->iInterruptChannel);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_InitializeWorkspace_UART0
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART0
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_InitializeWorkspace_UART0(void *aWorkspace)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    G_UART0Workspace = p;
    p->iInfo = &G_LPC1768_Serial_Info_UART0;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_InitializeWorkspace_UART1
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART1
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_InitializeWorkspace_UART1(void *aWorkspace)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    G_UART1Workspace = p;
    p->iInfo = &G_LPC1768_Serial_Info_UART1;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_InitializeWorkspace_UART2
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART2
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_InitializeWorkspace_UART2(void *aWorkspace)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    G_UART2Workspace = p;
    p->iInfo = &G_LPC1768_Serial_Info_UART2;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_InitializeWorkspace_UART3
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize a serial port workspace for UART3
 * Inputs:
 *      T_serialPort aPort        -- Serial port to enable
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_InitializeWorkspace_UART3(void *aWorkspace)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    G_UART3Workspace = p;
    p->iInfo = &G_LPC1768_Serial_Info_UART3;
    p->iCallbackWorkspace = 0;
    p->iReceivedByteFunc = 0;
    p->iTransmitEmptyFunc = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_Configure
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
T_uezError LPC1768_Serial_Configure(
        void *aWorkspace,
        void *aCallbackWorkspace,
        HAL_Serial_Callback_Received_Byte aReceivedByteCallback,
        HAL_Serial_Callback_Transmit_Empty aTransmitEmptyCallback)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    const T_Serial_LPC1768_SerialInfo *p_info = p->iInfo;
    TUInt32 divider ;

    InterruptDisable(p->iInfo->iInterruptChannel);

    // Ensure power is on to the part
//    PCONP |= p->iPowerBit;

// Configure the PCLK divider here to be CCLK/1 on UART0, 1/
// TBD: UART3 and 4 should be CCLK/1!
LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~(3<<6)) | (1<<6);
LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & ~(3<<8)) | (1<<8);
LPC_SC->PCLKSEL1 = (LPC_SC->PCLKSEL1 & ~(3<<16)) | (1<<16);
LPC_SC->PCLKSEL1 = (LPC_SC->PCLKSEL1 & ~(3<<18)) | (1<<18);

    p->iReceivedByteFunc = aReceivedByteCallback;
    p->iTransmitEmptyFunc = aTransmitEmptyCallback;
    p->iCallbackWorkspace = aCallbackWorkspace;

    divider = BAUD_DIVIDER(SERIAL_PORTS_DEFAULT_BAUD);

    // Set the FIFO enable bit in the FCR register. This bit must be set for
    // proper UART operation.
    p_info->iUART->FCR = 7; // FCRFE|RFR|TFR

    // Set baudrate
    p_info->iUART->LCR |= 0x80;
    p_info->iUART->DLL = divider & 0x00ff;
    p_info->iUART->DLM = (divider >> 8) & 0x00ff;
    p_info->iUART->LCR &= ~0x80;

    // Set default mode (8 bits, 1 stop bit, no parity)
    p_info->iUART->LCR = 0x03;

    //Enable UART0 interrupts
    p_info->iUART->IER = 0x03; // Interrupts and TX and RX

    InterruptRegister(
        p_info->iInterruptChannel,
        p_info->iISR,
        p_info->iPriority,
        p->iHAL->iInterface.iName);

//TBD: For now, leave serial port deactivated
//    InterruptEnable(p_info->iInterruptChannel);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_SetSerialSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the serial port settings
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_Serial_Settings *aSettings -- Settings to use for serial port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_SetSerialSettings(
        void *aWorkspace,
        T_Serial_Settings *aSettings)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    const T_Serial_LPC1768_SerialInfo *p_info = p->iInfo;
    TUInt32 divider ;

    // Record these settings
    p->iSettings = *aSettings;

    //TBD: Do the other settings!

    // Change the baud rate
    divider = BAUD_DIVIDER(aSettings->iBaud);
    p_info->iUART->LCR |= 0x80;
    p_info->iUART->DLL = divider & 0x00ff;
    p_info->iUART->DLM = (divider >> 8) & 0x00ff;
    p_info->iUART->LCR &= ~0x80;

	switch (aSettings->iFlowControl) {
        case SERIAL_FLOW_CONTROL_NONE:
            // default register settings are for none, need to figure out how to set it back
            //p_info->iUART->MCR |= (0 << 6); // Disable auto RTS
            //p_info->iUART->MCR |= (0 << 7); // Disable auto CTS
            break;
        case SERIAL_FLOW_CONTROL_XON_XOFF:
            return UEZ_ERROR_NOT_SUPPORTED; // TODO
        case SERIAL_FLOW_CONTROL_HARDWARE:
			return UEZ_ERROR_NOT_SUPPORTED; // TODO, example config from 43xx
            //p_info->iUART->MCR |= (1 << 6); // Auto RTS enable bit
            //p_info->iUART->MCR |= (1 << 7); // Auto CTS enable bit
            //break;			
        default:
            return UEZ_ERROR_NOT_SUPPORTED;
    }
	
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_GetSerialSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the previously configured serial port
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_Serial_Settings *aSettings -- Settings used for serial port
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_GetSerialSettings(
        void *aWorkspace,
        T_Serial_Settings *aSettings)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;

    // Record these settings
    *aSettings = p->iSettings;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_SetHandshakingControl
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the handshaking control for this serial port (if it has it)
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_serialHandshakingControl -- Control bits to use
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_SetHandshakingControl(
        void *aWorkspace,
        T_serialHandshakingControl aControl)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    const T_Serial_LPC1768_SerialInfo *p_info = p->iInfo;

    p_info->iUART->MCR = (aControl & 0xFF);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_GetHandshakingStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the handshaking control for this serial port (if it has it)
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 *      T_serialHandshakingStatus *aStatus -- Current status
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_Serial_GetHandshakingStatus(
        void *aWorkspace,
        T_serialHandshakingStatus *aStatus)
{
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;
    const T_Serial_LPC1768_SerialInfo *p_info = p->iInfo;

    *aStatus = (p_info->iUART->MSR & 0xFF);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_Serial_GetStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Return the status of the port
 * Inputs:
 *      void *aWorkspace          -- Serial port workspace
 * Outputs:
 *      T_serialStatusByte          -- Status byte returned
 *---------------------------------------------------------------------------*/
T_serialStatusByte LPC1768_Serial_GetStatus(void *aWorkspace)
{
    T_serialStatusByte status = 0;
    T_Serial_LPC1768_Workspace *p = (T_Serial_LPC1768_Workspace *)aWorkspace;

    // Only empty if both TEMT and THRE are empty
    if (p->iInfo->iUART->LSR & ((1<<6)|(1<<5))) {
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
const HAL_Serial G_LPC1768_Serial_UART0 = {
    {
    "Serial:LCP247x UART0",
    0x0100,
    LPC1768_Serial_InitializeWorkspace_UART0,
    sizeof(T_Serial_LPC1768_Workspace),
    },

    LPC1768_Serial_Configure,
    LPC1768_Serial_SetSerialSettings,
    LPC1768_Serial_GetSerialSettings,
    LPC1768_Serial_Activate,
    LPC1768_Serial_Deactivate,
    LPC1768_Serial_OutputByte,
    0,
    0,
    LPC1768_Serial_GetStatus,
};

const HAL_Serial G_LPC1768_Serial_UART1 = {
    {
    "Serial:LCP247x UART1",
    0x0100,
    LPC1768_Serial_InitializeWorkspace_UART1,
    sizeof(T_Serial_LPC1768_Workspace),
    },

    LPC1768_Serial_Configure,
    LPC1768_Serial_SetSerialSettings,
    LPC1768_Serial_GetSerialSettings,
    LPC1768_Serial_Activate,
    LPC1768_Serial_Deactivate,
    LPC1768_Serial_OutputByte,
    LPC1768_Serial_SetHandshakingControl,
    LPC1768_Serial_GetHandshakingStatus,
    LPC1768_Serial_GetStatus,
};

const HAL_Serial G_LPC1768_Serial_UART2 = {
    {
    "Serial:LCP247x UART2",
    0x0100,
    LPC1768_Serial_InitializeWorkspace_UART2,
    sizeof(T_Serial_LPC1768_Workspace),
    },

    LPC1768_Serial_Configure,
    LPC1768_Serial_SetSerialSettings,
    LPC1768_Serial_GetSerialSettings,
    LPC1768_Serial_Activate,
    LPC1768_Serial_Deactivate,
    LPC1768_Serial_OutputByte,
    0,
    0,
    LPC1768_Serial_GetStatus,
};

const HAL_Serial G_LPC1768_Serial_UART3 = {
    {
    "Serial:LCP247x UART3",
    0x0100,
    LPC1768_Serial_InitializeWorkspace_UART3,
    sizeof(T_Serial_LPC1768_Workspace),
    },

    LPC1768_Serial_Configure,
    LPC1768_Serial_SetSerialSettings,
    LPC1768_Serial_GetSerialSettings,
    LPC1768_Serial_Activate,
    LPC1768_Serial_Deactivate,
    LPC1768_Serial_OutputByte,
    0,
    0,
    LPC1768_Serial_GetStatus,
};

const T_Serial_LPC1768_SerialInfo G_LPC1768_Serial_Info_UART0 = {
    UART0_IRQn,
    INTERRUPT_PRIORITY_HIGH,
    (LPC_UART1_TypeDef *)LPC_UART0_BASE,
    (TISRFPtr)ISerialUART0,
};

const T_Serial_LPC1768_SerialInfo G_LPC1768_Serial_Info_UART1 = {
    UART1_IRQn,
    INTERRUPT_PRIORITY_HIGH,
    (LPC_UART1_TypeDef *)LPC_UART1_BASE,
    (TISRFPtr)ISerialUART1,
};

const T_Serial_LPC1768_SerialInfo G_LPC1768_Serial_Info_UART2 = {
    UART2_IRQn,
    INTERRUPT_PRIORITY_HIGH,
    (LPC_UART1_TypeDef *)LPC_UART2_BASE,
    (TISRFPtr)ISerialUART2,
};

const T_Serial_LPC1768_SerialInfo G_LPC1768_Serial_Info_UART3 = {
    UART3_IRQn,
    INTERRUPT_PRIORITY_HIGH,
    (LPC_UART1_TypeDef *)LPC_UART3_BASE,
    (TISRFPtr)ISerialUART3,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1768_UART0_Require(
        T_uezGPIOPortPin aPinTXD0,
        T_uezGPIOPortPin aPinRXD0)
{
    static const T_LPC1768_IOCON_ConfigList txd0[] = {
            {GPIO_P0_2,   IOCON_D_DEFAULT(1)},
    };
    static const T_LPC1768_IOCON_ConfigList rxd0[] = {
            {GPIO_P0_3,   IOCON_D_DEFAULT(1)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART0", (T_halInterface *)&G_LPC1768_Serial_UART0, 0,
            0);
    LPC1768_IOCON_ConfigPinOrNone(aPinTXD0, txd0, ARRAY_COUNT(txd0));
    LPC1768_IOCON_ConfigPinOrNone(aPinRXD0, rxd0, ARRAY_COUNT(rxd0));
}

void LPC1768_UART1_Require(
        T_uezGPIOPortPin aPinTXD1,
        T_uezGPIOPortPin aPinRXD1,
        T_uezGPIOPortPin aPinCTS,
        T_uezGPIOPortPin aPinDCD,
        T_uezGPIOPortPin aPinDSR,
        T_uezGPIOPortPin aPinDTR,
        T_uezGPIOPortPin aPinRI,
        T_uezGPIOPortPin aPinRTS)
{
    static const T_LPC1768_IOCON_ConfigList txd1[] = {
            {GPIO_P0_15, IOCON_D_DEFAULT(1)},
            {GPIO_P2_0,  IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList rxd1[] = {
            {GPIO_P0_16, IOCON_D_DEFAULT(1)},
            {GPIO_P2_1,  IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList cts[] = {
            {GPIO_P0_17, IOCON_D_DEFAULT(1)},
            {GPIO_P2_2,  IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList dcd[] = {
            {GPIO_P0_18, IOCON_D_DEFAULT(1)},
            {GPIO_P2_3,  IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList dsr[] = {
            {GPIO_P0_19, IOCON_D_DEFAULT(1)},
            {GPIO_P2_4,  IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList dtr[] = {
            {GPIO_P0_20, IOCON_D_DEFAULT(1)},
            {GPIO_P2_5,  IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList ri[] = {
            {GPIO_P0_21, IOCON_D_DEFAULT(1)},
            {GPIO_P2_6,  IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList rts[] = {
            {GPIO_P0_22, IOCON_D_DEFAULT(1)},
            {GPIO_P2_7,  IOCON_D_DEFAULT(2)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART1", (T_halInterface *)&G_LPC1768_Serial_UART1, 0,
            0);
    LPC1768_IOCON_ConfigPinOrNone(aPinTXD1, txd1, ARRAY_COUNT(txd1));
    LPC1768_IOCON_ConfigPinOrNone(aPinRXD1, rxd1, ARRAY_COUNT(rxd1));
    LPC1768_IOCON_ConfigPinOrNone(aPinCTS, cts, ARRAY_COUNT(cts));
    LPC1768_IOCON_ConfigPinOrNone(aPinDCD, dcd, ARRAY_COUNT(dcd));
    LPC1768_IOCON_ConfigPinOrNone(aPinDSR, dsr, ARRAY_COUNT(dsr));
    LPC1768_IOCON_ConfigPinOrNone(aPinDTR, dtr, ARRAY_COUNT(dtr));
    LPC1768_IOCON_ConfigPinOrNone(aPinRI, ri, ARRAY_COUNT(ri));
    LPC1768_IOCON_ConfigPinOrNone(aPinRTS, rts, ARRAY_COUNT(rts));
}

void LPC1768_UART2_Require(
        T_uezGPIOPortPin aPinTXD2,
        T_uezGPIOPortPin aPinRXD2)
{
    static const T_LPC1768_IOCON_ConfigList txd2[] = {
            {GPIO_P0_10, IOCON_D_DEFAULT(1)},
            {GPIO_P2_8,  IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList rxd2[] = {
            {GPIO_P0_11, IOCON_D_DEFAULT(1)},
            {GPIO_P2_9,  IOCON_D_DEFAULT(2)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART2", (T_halInterface *)&G_LPC1768_Serial_UART2, 0,
            0);
    LPC1768_IOCON_ConfigPinOrNone(aPinTXD2, txd2, ARRAY_COUNT(txd2));
    LPC1768_IOCON_ConfigPinOrNone(aPinRXD2, rxd2, ARRAY_COUNT(rxd2));
}

void LPC1768_UART3_Require(
        T_uezGPIOPortPin aPinTXD3,
        T_uezGPIOPortPin aPinRXD3)
{
    static const T_LPC1768_IOCON_ConfigList txd3[] = {
            {GPIO_P0_0,  IOCON_D_DEFAULT(2)},
            {GPIO_P0_25, IOCON_A_DEFAULT(3)},
            {GPIO_P4_28, IOCON_D_DEFAULT(2)},
    };
    static const T_LPC1768_IOCON_ConfigList rxd3[] = {
            {GPIO_P0_1,  IOCON_D_DEFAULT(2)},
            {GPIO_P0_26, IOCON_A_DEFAULT(3)},
            {GPIO_P4_29, IOCON_D_DEFAULT(2)},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("UART3", (T_halInterface *)&G_LPC1768_Serial_UART3, 0,
            0);
    LPC1768_IOCON_ConfigPinOrNone(aPinTXD3, txd3, ARRAY_COUNT(txd3));
    LPC1768_IOCON_ConfigPinOrNone(aPinRXD3, rxd3, ARRAY_COUNT(rxd3));
}

/*-------------------------------------------------------------------------*
 * End of File:  SerialPorts.c
 *-------------------------------------------------------------------------*/
