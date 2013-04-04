/*-------------------------------------------------------------------------*
 * File:  I2C.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1788 I2C Interface.
 * Implementation:
 *      A single I2C0 is created, but the code can be easily changed for
 *      other processors to use multiple I2C busses.
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
#include <uEZ.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZBSP.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC1788/LPC1788_I2C.h>

// Setup Master mode only.  Slave mode is a future version but we'll
// leave some of the code in so we don't have to write it later.
#define COMPILE_I2C_MASTER_MODE 1
#define COMPILE_I2C_SLAVE_MODE  0

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
// I2CONSET:
// Assert acknowledge flag
#define I2CONSET_AA     (1<<2)
// I2C interrupt flag
#define I2CONSET_SI     (1<<3)
// STOP flag
#define I2CONSET_STO    (1<<4)
// START flag
#define I2CONSET_STA    (1<<5)
// I2C interface enable
#define I2CONSET_I2EN   (1<<6)
// I2STAT:
// Status
#define I2STAT_STATUS_MASK  (0x1F<<3)
// I2DAT:
// I2ADR:
#define I2ADR_GC        (1<<0)
// I2SCLH:
// I2SCLL:
// I2CONCLR:
// Assert acknowledge Clear bit
#define I2CONCLR_AAC    (1<<2)
// I2C interrupt Clear bit
#define I2CONCLR_SIC    (1<<3)
// START flag Clear bit
#define I2CONCLR_STAC   (1<<5)
// I2C interface Disable bit
#define I2CONCLR_I2ENC  (1<<6)

#if 0
typedef TBool (*I2CSlaveIsLastReceiveByte)(T_LPC1788_I2C_Workspace *aWorkspace);
typedef TBool (*I2CSlaveIsLastTransmitByte)(T_LPC1788_I2C_Workspace *aWorkspace);
typedef void (*I2CTransferComplete)(T_LPC1788_I2C_Workspace *aWorkspace);
typedef void (*I2CSlaveReceiveByte)(T_LPC1788_I2C_Workspace *aWorkspace, TUInt8 aByte);
typedef TUInt8 (*I2CSlaveGetTransmitByte)(T_LPC1788_I2C_Workspace *aWorkspace);
#endif

typedef struct {
    const HAL_I2CBus *iHAL;
    LPC_I2C_TypeDef *iRegs;
    I2C_Request *iRequest;
    TUInt8 iAddress;
    TUInt8 *iData;
    TUInt8 iDataLen;
    TUInt8 iIndex;
    I2CRequestCompleteCallback iCompleteFunc;
    void *iCompleteWorkspace;
    TUInt16 iPowerBits;

    volatile TBool iDoneFlag;
#if COMPILE_I2C_SLAVE_MODE
I2CSlaveIsLastReceiveByte iI2CSlaveIsLastReceiveByte;
I2CSlaveIsLastTransmitByte iI2CSlaveIsLastTransmitByte;
I2CTransferComplete iI2CTransferComplete;
I2CSlaveReceiveByte iI2CSlaveReceiveByte;
I2CSlaveGetTransmitByte iI2CSlaveGetTransmitByte;
#endif
} T_LPC1788_I2C_Workspace;

typedef TUInt8 T_lpc1788_i2cMode;
#define I2C_MODE_READ           1
#define I2C_MODE_WRITE          0

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
#define I2C_ENABLE(p)       (((p)->iRegs)->CONSET) = I2CONSET_I2EN
#define I2C_DISABLE(p)      (((p)->iRegs)->CONCLR) = I2CONCLR_I2ENC
#define STA_SET(p)          (((p)->iRegs)->CONSET) = I2CONSET_STA
#define STA_CLEAR(p)        (((p)->iRegs)->CONCLR) = I2CONCLR_STAC
#define STO_SET(p)          (((p)->iRegs)->CONSET) = I2CONSET_STO
#define STO_CLEAR(p)        /* automatically cleared by hardware */
#define AA_SET(p)           (((p)->iRegs)->CONSET) = I2CONSET_AA
#define AA_CLEAR(p)         (((p)->iRegs)->CONCLR) = I2CONCLR_AAC
#define SI_CLEAR(p)         (((p)->iRegs)->CONCLR) = I2CONCLR_SIC
#define I2DAT_WRITE(p, v)   (((p)->iRegs)->DAT) = (v)
#define I2DAT_READ(p)       (((p)->iRegs)->DAT)

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_LPC1788_I2C_Workspace *G_lpc1788_i2c0Workspace;
static T_LPC1788_I2C_Workspace *G_lpc1788_i2c1Workspace;
static T_LPC1788_I2C_Workspace *G_lpc1788_i2c2Workspace;

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC1788_I2C0InterruptHandler)
;
IRQ_ROUTINE(ILPC1788_I2C1InterruptHandler)
;
IRQ_ROUTINE(ILPC1788_I2C2InterruptHandler)
;
void ILPC1788_ProcessState(T_LPC1788_I2C_Workspace *p);

/*-------------------------------------------------------------------------*/
#if 0 // COMPILE_I2C_SLAVE_MODE
static TBool ILPC1788_I2CSlaveIsLastReceiveByte(T_LPC1788_I2C_Workspace *aWorkspace)
{
    if (aWorkspace->iIndex >= aWorkspace->iDataLen)
    return ETrue;
    return EFalse;
}

static TBool ILPC1788_I2CSlaveIsLastTransmitByte(T_LPC1788_I2C_Workspace *aWorkspace)
{
    if (aWorkspace->iIndex >= aWorkspace->iDataLen)
    return ETrue;
    return EFalse;
}

static void ILPC1788_I2CTransferComplete(T_LPC1788_I2C_Workspace *aWorkspace)
{
    // Do nothing
    //    PARAM_NOT_USED(aWorkspace);
}

static void ILPC1788_I2CSlaveReceiveByte(T_LPC1788_I2C_Workspace *aWorkspace, TUInt8 aByte)
{
    // Only store if there is room
    if (aWorkspace->iIndex < aWorkspace->iDataLen)
    aWorkspace->iData[aWorkspace->iIndex] = aByte;
}

static TUInt8 ILPC1788_I2CSlaveGetTransmitByte(T_LPC1788_I2C_Workspace *aWorkspace)
{
    // Only read from memory if in bounds
    if (aWorkspace->iIndex < aWorkspace->iDataLen)
    return aWorkspace->iData[aWorkspace->iIndex];
    return 0;
}
#endif

void ILPC1788_I2CSetSpeed(LPC_I2C_TypeDef *aRegs, TUInt16 aSpeed)
{
    // Calculate the speed value to use
    TUInt16 v;

    // Calculate from kHz to cycles (based on PCLK)
    v = (PCLK_FREQUENCY / 1000) / aSpeed;

    // Set 50% duty cycle
    aRegs->SCLL = v / 2;
    aRegs->SCLH = v / 2;
}

void ILPC1788_I2CInit(T_LPC1788_I2C_Workspace *p)
{
    // Turn on the power
    LPC1788PowerOn(1 << p->iPowerBits);

    // Clear all the flags
    p->iRegs->CONCLR = I2CONSET_AA | I2CONSET_SI | I2CONSET_STA | I2CONSET_I2EN;

    // Program the speed
    ILPC1788_I2CSetSpeed(p->iRegs, p->iRequest->iSpeed);

    // Enable the I2C
    I2C_ENABLE(p);
}

static void ILPC1788_I2C_StartWrite(
        void *aWorkspace,
        I2C_Request *iRequest,
        void *aCallbackWorkspace,
        I2CRequestCompleteCallback aCallbackFunc)
{
    T_LPC1788_I2C_Workspace *p = (T_LPC1788_I2C_Workspace *)aWorkspace;

    // Use an address (and make sure the lowest bit is zero)
    p->iRequest = iRequest;
    p->iCompleteFunc = aCallbackFunc;
    p->iCompleteWorkspace = aCallbackWorkspace;
    p->iAddress = (p->iRequest->iAddr << 1) | I2C_MODE_WRITE;
    p->iData = (TUInt8 *)p->iRequest->iWriteData;
    p->iDataLen = p->iRequest->iWriteLength;
    p->iIndex = 0;
    p->iDoneFlag = EFalse;
    p->iRequest->iStatus = I2C_BUSY;

    // Initialize the I2C
    ILPC1788_I2CInit(p);

    // Kick off the transfer with a start bit
    STA_SET(p);
}

static void ILPC1788_I2C_StartRead(
        void *aWorkspace,
        I2C_Request *iRequest,
        void *aCallbackWorkspace,
        I2CRequestCompleteCallback aCallbackFunc)
{
    T_LPC1788_I2C_Workspace *p = (T_LPC1788_I2C_Workspace *)aWorkspace;

    // Use an address (and make sure the lowest bit is zero)
    p->iRequest = iRequest;
    p->iCompleteFunc = aCallbackFunc;
    p->iCompleteWorkspace = aCallbackWorkspace;
    p->iAddress = (p->iRequest->iAddr << 1) | I2C_MODE_READ;
    p->iData = p->iRequest->iReadData;
    p->iDataLen = p->iRequest->iReadLength;
    p->iIndex = 0;
    p->iDoneFlag = EFalse;
    p->iRequest->iStatus = I2C_BUSY;

    // Initialize the I2C
    ILPC1788_I2CInit(p);

    // Kick off the transfer with a start bit
    STA_SET(p);
}

/*-------------------------------------------------------------------------*
 * Routine:  ILPC1788_ProcessState
 *-------------------------------------------------------------------------*
 * Description:
 *      Core I2C processing (typically done in interrupt routine)
 *-------------------------------------------------------------------------*/
void ILPC1788_ProcessState(T_LPC1788_I2C_Workspace *p)
{
    TUInt8 c;
    TUInt8 status = p->iRegs->STAT;

    //SerialSendChar(SERIAL_PORT_UART0, '$');
    //SerialSendHex(SERIAL_PORT_UART0, status);
    //ConsolePrintf("$%02X", status);
    switch (status) {
#if COMPILE_I2C_MASTER_MODE
        // Start condition tranmitted
        case 0x08:

            // Repeat start condition transmitted
        case 0x10:
            // Send the slave address
            I2DAT_WRITE(p, p->iAddress);
            //            (((p)->iRegs)->DAT) = (p->iAddress);
            STA_CLEAR(p);
            STO_CLEAR(p);
            p->iIndex = 0;
            break;

            // MASTER TRANSMITTER
            // Slave Address + Write transmitted, and got ACK from slave
        case 0x18:
            // Transmit a byte
            I2DAT_WRITE(p, p->iData[p->iIndex]);
            STA_CLEAR(p);
            STO_CLEAR(p);
            break;

            // Slave Address transmitted, no ACK received
        case 0x20:
            // Generate a stop condition and report error status
            STA_CLEAR(p);
            STO_SET(p);
            p->iRequest->iStatus = I2C_ERROR;
            p->iDoneFlag = ETrue;
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            break;

            // Data byte written and ACK received.
        case 0x28:
            // Last byte?
            p->iIndex++;
            if (p->iIndex >= p->iDataLen) {
                // Yes, last.  Generate a stop condition
                STA_CLEAR(p);
                STO_SET(p);
                p->iRequest->iStatus = I2C_OK;
                p->iDoneFlag = ETrue;
                p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            } else {
                // Send the next byte
                I2DAT_WRITE(p, p->iData[p->iIndex]);
                STA_CLEAR(p);
                STO_CLEAR(p);
            }
            break;

            // Data byte written but no ACK received
        case 0x30:
            // Generate a stop condition
            STA_CLEAR(p);
            STO_SET(p);
            p->iRequest->iStatus = I2C_ERROR;
            p->iDoneFlag = ETrue;
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            break;

            // Arbitration was lost
        case 0x38:
            // Generate a start condition and try again
            STA_SET(p);
            STO_CLEAR(p);
            break;

            // MASTER RECEIVER
            // Slave address + Read sent, ACK received
        case 0x40:
            // Are we to receive more?
            STA_CLEAR(p);
            STO_CLEAR(p);
            if ((p->iIndex + 1) >= p->iDataLen) {
                // No more needed.
                // return NACK for data byte
                AA_CLEAR(p);
            } else {
                // More bytes needed.
                // return ACK for data byte
                AA_SET(p);
            }
            break;

            // Slave address + Read sent, NO ACK received
        case 0x48:
            // Generate a stop condition
            STA_CLEAR(p);
            STO_SET(p);
            p->iRequest->iStatus = I2C_ERROR;
            p->iDoneFlag = ETrue;
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            break;

            // Data byte received with ACK
        case 0x50:
            // Store received byte
            c = I2DAT_READ(p);
            if (p->iIndex < p->iDataLen)
                p->iData[p->iIndex++] = c;

            // Are we to receive more?
            STA_CLEAR(p);
            STO_CLEAR(p);
            if ((p->iIndex + 1) >= p->iDataLen) {
                // return NACK for next data byte
                AA_CLEAR(p);
            } else {
                // return ACK for next data byte
                AA_SET(p);
            }
            break;

            // Data byte received with NACK (last byte)
        case 0x58:
            // Store last byte
            c = I2DAT_READ(p);
            if (p->iIndex < p->iDataLen)
                p->iData[p->iIndex++] = c;

            // Generate a stop condition, but report OK
            STA_CLEAR(p);
            STO_SET(p);
            p->iRequest->iStatus = I2C_OK;
            p->iDoneFlag = ETrue;
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            break;
#endif // COMPILE_I2C_MASTER_MODE
#if COMPILE_I2C_SLAVE_MODE
            // SLAVE RECEIVER
            case 0x60:
            case 0x68:
            case 0x70:
            case 0x78:
            // Start of slave transaction
            // slave address + W received, or general call address received,
            // or lost arbitration, slave address + W received, or
            // lost arbitration, general call address received.
            STA_CLEAR(p);
            STO_CLEAR(p);
            p->iIndex = 0;
            p->iDoneFlag = EFalse;
            // Is this the last byte?
            if (p->iI2CSlaveIsLastReceiveByte(p)) {
                // Not allowed to send more bytes
                AA_CLEAR(p);
            } else {
                // Allowed to send more bytes
                AA_SET(p);
            }
            break;
            case 0x80:
            case 0x90:
            // Data received
            p->iI2CSlaveReceiveByte(p, I2DAT_READ(p));
            p->iIndex++;
            STA_CLEAR(p);
            STO_CLEAR(p);
            // Is this the last byte?
            if (p->iI2CSlaveIsLastReceiveByte(p)) {
                // Not allowed to send more bytes
                AA_CLEAR(p);
            } else {
                // Allowed to send more bytes
                AA_SET(p);
            }
            break;
            case 0x88:
            case 0x98:
            // Data received, NACK returned (signaling last byte)
            p->iI2CSlaveReceiveByte(p, I2DAT_READ(p));
            p->iIndex++;
            p->iRequest->iStatus = I2C_OK;
            STA_CLEAR(p);
            STO_CLEAR(p);
            AA_SET(p);
            p->iDoneFlag = ETrue;
            p->iI2CTransferComplete(p);
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            break;
            case 0xA0:
            // Stop condition received
            p->iRequest->iStatus = I2C_OK;
            STA_CLEAR(p);
            STO_CLEAR(p);
            AA_SET(p);
            p->iDoneFlag = ETrue;
            p->iI2CTransferComplete(p);
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            break;

            // SLAVE TRANSMITTER
            // ACK returned on Slave Address + R received; or
            // arbitration lost, slave address + R received, ACK returned; or
            // data byte transmitted, ACK received
            case 0xA8:
            case 0xB0:
            p->iIndex = 0;
            case 0xB8:
            c = p->iI2CSlaveGetTransmitByte(p);
            p->iIndex++;
            I2DAT_WRITE(p, c);
            STA_CLEAR(p);
            STO_CLEAR(p);
            if (p->iI2CSlaveIsLastTransmitByte(p)) {
                // No more data
                AA_CLEAR(p);
            } else {
                // more data bytes to transmit
                AA_SET(p);
            }
            break;
            case 0xC0:
            case 0xC8:
            // data byte transmitted, NACK received
            // last data byte transmitted, NACK received
            p->iRequest->iStatus = I2C_OK;
            STA_CLEAR(p);
            STO_CLEAR(p);
            AA_SET(p);
            p->iDoneFlag = ETrue;
            p->iI2CTransferComplete(p);
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            break;
#endif // #if COMPILE_I2C_SLAVE_MODE
            // Unhandled state
        default:
            // Generate a stop condition
            STA_CLEAR(p);
            STO_CLEAR(p);
            AA_SET(p);
            p->iRequest->iStatus = I2C_ERROR;
            p->iDoneFlag = ETrue;
#if COMPILE_I2C_SLAVE_MODE
            p->iI2CTransferComplete(p);
#endif
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            break;
    }

    // clear interrupt flag
    SI_CLEAR(p);
}

IRQ_ROUTINE(ILPC1788_I2C0InterruptHandler)
{
    IRQ_START();
    ILPC1788_ProcessState(G_lpc1788_i2c0Workspace);
    IRQ_END()
    ;
}

IRQ_ROUTINE(ILPC1788_I2C1InterruptHandler)
{
    IRQ_START();
    ILPC1788_ProcessState(G_lpc1788_i2c1Workspace);
    IRQ_END()
    ;
}

IRQ_ROUTINE(ILPC1788_I2C2InterruptHandler)
{
    IRQ_START();
    ILPC1788_ProcessState(G_lpc1788_i2c2Workspace);
    IRQ_END()
    ;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_I2C_Bus0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 I2C Bus0 workspace.
 * Inputs:
 *      void *aW                    -- Particular I2C workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_I2C_Bus0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_I2C_Workspace *p = (T_LPC1788_I2C_Workspace *)aWorkspace;
    p->iRegs = LPC_I2C0;
    p->iDoneFlag = ETrue;
    p->iCompleteFunc = 0;
    G_lpc1788_i2c0Workspace = p;
    p->iPowerBits = 7;

    // Setup interrupt vector
    InterruptRegister(I2C0_IRQn, ILPC1788_I2C0InterruptHandler,
            INTERRUPT_PRIORITY_NORMAL, "I2C0");
    InterruptEnable(I2C0_IRQn);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_I2C_Bus1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 I2C Bus1 workspace.
 * Inputs:
 *      void *aW                    -- Particular I2C workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_I2C_Bus1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_I2C_Workspace *p = (T_LPC1788_I2C_Workspace *)aWorkspace;
    p->iRegs = LPC_I2C1;
    p->iDoneFlag = ETrue;
    p->iCompleteFunc = 0;
    G_lpc1788_i2c1Workspace = p;
    p->iPowerBits = 19;

    // Setup interrupt vector
    InterruptRegister(I2C1_IRQn, ILPC1788_I2C1InterruptHandler,
            INTERRUPT_PRIORITY_NORMAL, "I2C1");
    InterruptEnable(I2C1_IRQn);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1788_I2C_Bus2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1788 I2C Bus2 workspace.
 * Inputs:
 *      void *aW                    -- Particular I2C workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1788_I2C_Bus2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1788_I2C_Workspace *p = (T_LPC1788_I2C_Workspace *)aWorkspace;
    p->iRegs = LPC_I2C2;
    p->iDoneFlag = ETrue;
    p->iCompleteFunc = 0;
    G_lpc1788_i2c2Workspace = p;
    p->iPowerBits = 26;

    // Setup interrupt vector
    InterruptRegister(I2C2_IRQn, ILPC1788_I2C2InterruptHandler,
            INTERRUPT_PRIORITY_NORMAL, "I2C2");
    InterruptEnable(I2C2_IRQn);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_I2CBus I2C_LPC1788_Bus0_Interface = { {
        "LPC1788 I2C Bus0",
        0x0100,
        LPC1788_I2C_Bus0_InitializeWorkspace,
        sizeof(T_LPC1788_I2C_Workspace), },

ILPC1788_I2C_StartRead, ILPC1788_I2C_StartWrite, };

const HAL_I2CBus I2C_LPC1788_Bus1_Interface = { {
        "LPC1788 I2C Bus1",
        0x0100,
        LPC1788_I2C_Bus1_InitializeWorkspace,
        sizeof(T_LPC1788_I2C_Workspace), },

ILPC1788_I2C_StartRead, ILPC1788_I2C_StartWrite, };

const HAL_I2CBus I2C_LPC1788_Bus2_Interface = { {
        "LPC1788 I2C Bus2",
        0x0100,
        LPC1788_I2C_Bus2_InitializeWorkspace,
        sizeof(T_LPC1788_I2C_Workspace), },

ILPC1788_I2C_StartRead, ILPC1788_I2C_StartWrite, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1788_I2C0_Require(
        T_uezGPIOPortPin aPinSDA0,
        T_uezGPIOPortPin aPinSCL0)
{
    static const T_LPC1788_IOCON_ConfigList sda0[] = {
            {GPIO_P0_27,   IOCON_I_DEFAULT(1) | IOCON_OPEN_DRAIN},
            {GPIO_P1_30,   IOCON_A(4, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                                                    IOCON_INVERT_OFF, IOCON_DIGITAL, IOCON_FILTER_OFF,
                                                    IOCON_OPEN_DRAIN, IOCON_DAC_DISABLE)},
            {GPIO_P5_2, IOCON_I_DEFAULT(5) | IOCON_OPEN_DRAIN},
    };
    static const T_LPC1788_IOCON_ConfigList scl0[] = {
            {GPIO_P0_28,   IOCON_I_DEFAULT(1) | IOCON_OPEN_DRAIN},
            {GPIO_P1_31,   IOCON_A(4, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                                                    IOCON_INVERT_OFF, IOCON_DIGITAL, IOCON_FILTER_OFF,
                                                    IOCON_OPEN_DRAIN, IOCON_DAC_DISABLE)},
            {GPIO_P5_3, IOCON_I_DEFAULT(5) | IOCON_OPEN_DRAIN},
    };
    HAL_DEVICE_REQUIRE_ONCE();
    // Register I2C0 Bus driver
    HALInterfaceRegister("I2C0", (T_halInterface *)&I2C_LPC1788_Bus0_Interface,
            0, 0);
    LPC1788_IOCON_ConfigPin(aPinSDA0, sda0, ARRAY_COUNT(sda0));
    LPC1788_IOCON_ConfigPin(aPinSCL0, scl0, ARRAY_COUNT(scl0));
}

void LPC1788_I2C1_Require(
        T_uezGPIOPortPin aPinSDA1,
        T_uezGPIOPortPin aPinSCL1)
{
    static const T_LPC1788_IOCON_ConfigList sda1[] = {
            {GPIO_P0_0,     IOCON_D(3, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P0_19,   IOCON_D(3, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P2_14, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
    };
    static const T_LPC1788_IOCON_ConfigList scl1[] = {
            {GPIO_P0_1,     IOCON_D(3, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P0_20,   IOCON_D(3, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P2_15, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register I2C1 Bus driver
    HALInterfaceRegister("I2C1", (T_halInterface *)&I2C_LPC1788_Bus1_Interface,
            0, 0);
    LPC1788_IOCON_ConfigPin(aPinSDA1, sda1, ARRAY_COUNT(sda1));
    LPC1788_IOCON_ConfigPin(aPinSCL1, scl1, ARRAY_COUNT(scl1));
}

void LPC1788_I2C2_Require(
        T_uezGPIOPortPin aPinSDA2,
        T_uezGPIOPortPin aPinSCL2)
{
    static const T_LPC1788_IOCON_ConfigList sda2[] = {
            {GPIO_P0_10, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P1_15, IOCON_D(3, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P2_30, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P4_20, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
    };
    static const T_LPC1788_IOCON_ConfigList scl2[] = {
            {GPIO_P0_11, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P2_31, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P4_21, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
            {GPIO_P4_29, IOCON_D(4, IOCON_NO_PULL, IOCON_HYS_ENABLE,
                    IOCON_INVERT_OFF, IOCON_SLEW_STANDARD, IOCON_OPEN_DRAIN)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register I2C0 Bus driver
    HALInterfaceRegister("I2C2", (T_halInterface *)&I2C_LPC1788_Bus2_Interface,
            0, 0);

    LPC1788_IOCON_ConfigPin(aPinSDA2, sda2, ARRAY_COUNT(sda2));
    LPC1788_IOCON_ConfigPin(aPinSCL2, scl2, ARRAY_COUNT(scl2));
}

/*-------------------------------------------------------------------------*
 * End of File:  I2C.c
 *-------------------------------------------------------------------------*/
