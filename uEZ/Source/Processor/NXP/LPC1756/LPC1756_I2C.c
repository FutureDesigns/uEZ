/*-------------------------------------------------------------------------*
 * File:  LPC1756_I2C.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1756 I2C Interface.
 * Implementation:
 *      A single I2C is created, but the code can be easily changed for
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
 /**
 *    @addtogroup LPC1756_I2C
 *  @{
 *  @brief     uEZ LPC1756 I2C Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    HAL implementation of the LPC1756 I2C Interface.
 * Implementation:
 *      A single I2C is created, but the code can be easily changed for
 *      other processors to use multiple I2C busses.
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
#include <uEZ.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_I2C.h>
#include <uEZPlatformAPI.h>

/** Setup Master mode only.  Slave mode is a future version but we'll
 *  leave some of the code in so we don't have to write it later. */
#define COMPILE_I2C_MASTER_MODE 1
#ifndef COMPILE_I2C_SLAVE_MODE
#define COMPILE_I2C_SLAVE_MODE  1
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
/** I2CONSET: */
/** Assert acknowledge flag */
#define I2CONSET_AA     (1<<2)
/** I2C interrupt flag */
#define I2CONSET_SI     (1<<3)
/** STOP flag */
#define I2CONSET_STO    (1<<4)
/** START flag */
#define I2CONSET_STA    (1<<5)
/** I2C interface enable */
#define I2CONSET_I2EN   (1<<6)
/** I2STAT: */
/** Status */
#define I2STAT_STATUS_MASK  (0x1F<<3)
/** I2DAT: */
/** I2ADR: */
#define I2ADR_GC        (1<<0)
/** I2SCLH: */
/** I2SCLL: */
/** I2CONCLR: */
/** Assert acknowledge Clear bit */
#define I2CONCLR_AAC    (1<<2)
/** I2C interrupt Clear bit */
#define I2CONCLR_SIC    (1<<3)
/** START flag Clear bit */
#define I2CONCLR_STAC   (1<<5)
/** I2C interface Disable bit */
#define I2CONCLR_I2ENC  (1<<6)

typedef struct {
    const HAL_I2CBus *iHAL;
    I2C_TypeDef *iRegs;
    I2C_Request *iRequest;
    TUInt8 iAddress;
    TUInt8 *iData;
    TUInt8 iDataLen;
    TUInt8 iIndex;
    I2CRequestCompleteCallback iCompleteFunc;
    void *iCompleteWorkspace;

    volatile TBool iDoneFlag;
#if COMPILE_I2C_SLAVE_MODE
    void *iI2CSlaveCallbackWorkspace;
    I2CSlaveIsLastReceiveByte iI2CSlaveIsLastReceiveByte;
    I2CSlaveIsLastTransmitByte iI2CSlaveIsLastTransmitByte;
    I2CTransferComplete iI2CSlaveTransferComplete;
    I2CSlaveReceiveByte iI2CSlaveReceiveByte;
    I2CSlaveGetTransmitByte iI2CSlaveGetTransmitByte;
#endif
} T_LPC1756_I2C_Workspace;

typedef TUInt8 T_lpc1756_i2cMode;
#define I2C_MODE_READ           1
#define I2C_MODE_WRITE          0

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
#define I2C_ENABLE(p)       (((p)->iRegs)->I2CONSET) = I2CONSET_I2EN
#define I2C_DISABLE(p)      (((p)->iRegs)->I2CONCLR) = I2CONCLR_I2ENC
#define STA_SET(p)          (((p)->iRegs)->I2CONSET) = I2CONSET_STA
#define STA_CLEAR(p)        (((p)->iRegs)->I2CONCLR) = I2CONCLR_STAC
#define STO_SET(p)          (((p)->iRegs)->I2CONSET) = I2CONSET_STO
#define STO_CLEAR(p)        /** automatically cleared by hardware */
#define AA_SET(p)           (((p)->iRegs)->I2CONSET) = I2CONSET_AA
#define AA_CLEAR(p)         (((p)->iRegs)->I2CONCLR) = I2CONCLR_AAC
#define SI_CLEAR(p)         (((p)->iRegs)->I2CONCLR) = I2CONCLR_SIC
#define I2DAT_WRITE(p, v)   (((p)->iRegs)->I2DAT) = (v)
#define I2DAT_READ(p)       (((p)->iRegs)->I2DAT)
#define I2C_SET_ADDR0(p, v) (((p)->iRegs)->I2ADR0) = (v)
#define I2C_SET_MASK0(p, v) (((p)->iRegs)->I2MASK0) = (v)

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_LPC1756_I2C_Workspace *G_lpc1756_i2c1Workspace;
static T_LPC1756_I2C_Workspace *G_lpc1756_i2c2Workspace;

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
IRQ_ROUTINE( ILPC1756_I2C1InterruptHandler);
IRQ_ROUTINE( ILPC1756_I2C2InterruptHandler);
void ILPC1756_ProcessState(T_LPC1756_I2C_Workspace *p);

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_I2CSetSpeed
 *---------------------------------------------------------------------------*/
/**
 *  Set the I2C speed and (by default) make it 50% high, 50% low.
 *
 *  @param [in]    *aRegs 		I2C registers
 *
 *  @param [in]    aSpeed		Speed of I2C in kHz (100 or 400 typically)
 *
 *  @return        
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ILPC1756_I2CSetSpeed(I2C_TypeDef *aRegs, TUInt16 aSpeed)
{
    // Calculate the speed value to use
    TUInt16 v;

    // Calculate from kHz to cycles (based on PCLK)
    v = (PROCESSOR_OSCILLATOR_FREQUENCY / 1000) / aSpeed;

    // Set 50% duty cycle
    aRegs->I2SCLL = v / 2;
    aRegs->I2SCLH = v / 2;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_I2CInit
 *---------------------------------------------------------------------------*/
/**
 *  Setup the I2C for processing (master)
 *
 *  @param [in]    *p -- Workspace
 *      
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ILPC1756_I2CInit(T_LPC1756_I2C_Workspace *p)
{
    // Clear all the flags
    p->iRegs->I2CONCLR = I2CONSET_AA | I2CONSET_SI | I2CONSET_STA
        |I2CONSET_I2EN;

    // Program the speed
    ILPC1756_I2CSetSpeed(p->iRegs, p->iRequest->iSpeed);

    // Enable the I2C
    I2C_ENABLE(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_I2C_StartWrite
 *---------------------------------------------------------------------------*/
/**
 *  Start an I2C Write transaction in the given request and report
 *      to the given callback function when complete.
 *
 *  @param [in]    *aWorkspace 		I2C Workspace
 *
 *  @param [in]    *iRequest 		I2C request with write settings
 *
 *  @param [in]    *aCallback 		Workspace of callback to call
 *
 *  @param [in]    aCallbackFunc 	Function to call when complete
 *
 *  @return        
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ILPC1756_I2C_StartWrite(
    void *aWorkspace,
    I2C_Request *iRequest,
    void *aCallbackWorkspace,
    I2CRequestCompleteCallback aCallbackFunc)
{
    T_LPC1756_I2C_Workspace *p = (T_LPC1756_I2C_Workspace *)aWorkspace;

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
    ILPC1756_I2CInit(p);

    // Kick off the transfer with a start bit
    STA_SET(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_I2C_StartRead
 *---------------------------------------------------------------------------*/
/**
 *  Start an I2C Read transaction in the given request and report
 *      to the given callback function when complete.
 *
 *  @param [in]    *aWorkspace 		I2C Workspace
 *
 *  @param [in]    *iRequest 		I2C request with read settings
 *
 *  @param [in]    *aCallback 		Workspace of callback to call
 *
 *  @param [in]    aCallbackFunc 	Function to call when complete
 *    
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ILPC1756_I2C_StartRead(
    void *aWorkspace,
    I2C_Request *iRequest,
    void *aCallbackWorkspace,
    I2CRequestCompleteCallback aCallbackFunc)
{
    T_LPC1756_I2C_Workspace *p = (T_LPC1756_I2C_Workspace *)aWorkspace;

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
    ILPC1756_I2CInit(p);

    // Kick off the transfer with a start bit
    STA_SET(p);
}

/*-------------------------------------------------------------------------*
 * Routine:  ILPC1756_ProcessState
 *---------------------------------------------------------------------------*/
/**
 *  Core I2C processing (typically done in interrupt routine)
 *
 *  @param [in]    *p 				I2C Workspace
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ILPC1756_ProcessState(T_LPC1756_I2C_Workspace *p)
{
    TUInt8 c;
    TUInt8 status = p->iRegs->I2STAT;

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
            //            (((p)->iRegs)->I2DAT) = (p->iAddress);
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
            if (p->iI2CSlaveIsLastReceiveByte(p->iI2CSlaveCallbackWorkspace)) {
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
            p->iI2CSlaveReceiveByte(p->iI2CSlaveCallbackWorkspace, I2DAT_READ(p));
            p->iIndex++;
            STA_CLEAR(p);
            STO_CLEAR(p);
            // Is this the last byte?
            if (p->iI2CSlaveIsLastReceiveByte(p->iI2CSlaveCallbackWorkspace)) {
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
            p->iI2CSlaveReceiveByte(p->iI2CSlaveCallbackWorkspace,
                I2DAT_READ(p));
            p->iIndex++;
            //p->iRequest->iStatus = I2C_OK;
            STA_CLEAR(p);
            STO_CLEAR(p);
            AA_SET(p);
            p->iDoneFlag = ETrue;
            p->iI2CSlaveTransferComplete(p->iI2CSlaveCallbackWorkspace);
            break;
        case 0xA0:
            // Stop condition received
            //p->iRequest->iStatus = I2C_OK; 
            STA_CLEAR(p);
            STO_CLEAR(p);
            AA_SET(p);
            p->iDoneFlag = ETrue;
            p->iI2CSlaveTransferComplete(p->iI2CSlaveCallbackWorkspace);
            break;

            // SLAVE TRANSMITTER 
            // ACK returned on Slave Address + R received; or
            // arbitration lost, slave address + R received, ACK returned; or
            // data byte transmitted, ACK received
        case 0xA8:
        case 0xB0:
            p->iIndex = 0;
        case 0xB8:
            c = p->iI2CSlaveGetTransmitByte(p->iI2CSlaveCallbackWorkspace);
            p->iIndex++;
            I2DAT_WRITE(p, c);
            STA_CLEAR(p);
            STO_CLEAR(p);
            if (p->iI2CSlaveIsLastTransmitByte(p->iI2CSlaveCallbackWorkspace)) {
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
            //p->iRequest->iStatus = I2C_OK;
            STA_CLEAR(p);
            STO_CLEAR(p);
            AA_SET(p);
            p->iDoneFlag = ETrue;
            p->iI2CSlaveTransferComplete(p->iI2CSlaveCallbackWorkspace);
            break;
#endif // #if COMPILE_I2C_SLAVE_MODE
            // Unhandled state
        default:
            // Generate a stop condition
            STA_CLEAR(p);
            STO_CLEAR(p);
            AA_SET(p);
            //p->iRequest->iStatus = I2C_ERROR;
            p->iDoneFlag = ETrue;
#if COMPILE_I2C_SLAVE_MODE
            p->iI2CSlaveTransferComplete(p->iI2CSlaveCallbackWorkspace);
#endif
            if (p->iCompleteFunc)
                p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            break;
    }

    // clear interrupt flag
    SI_CLEAR(p);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_ConfigureSlave
 *---------------------------------------------------------------------------*/
/**
 *  Configure the I2C Slave callback functions and callback workspace.
 *      Must call Enable in order to enable slave commands.
 *
 *  @param [in]    *aWorkspace 				I2C Workspace
 *
 *  @param [in]    aAddressLower7Bits 		Slave address to receive I2C commands
 *
 *  @param [in]    *aCallbackWorkspace 		Callback's workspace, or 0 for none
 *
 *  @param [in]    *aCallbacks 				List of callback functions
 *
 *  @return        T_uezError 				Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError ILPC1756_ConfigureSlave(
    void *aWorkspace,
    TUInt8 aAddressLower7Bits,
    void *aCallbackWorkspace,
    const T_I2CSlaveCallbacks *aCallbacks)
{
    T_LPC1756_I2C_Workspace *p = (T_LPC1756_I2C_Workspace *)aWorkspace;

    p->iI2CSlaveIsLastReceiveByte = aCallbacks->iIsLastReceiveByteCallback;
    p->iI2CSlaveIsLastTransmitByte = aCallbacks->iIsLastTransmitByteCallback;
    p->iI2CSlaveReceiveByte = aCallbacks->iReceiveByteCallback;
    p->iI2CSlaveGetTransmitByte = aCallbacks->iGetTransmitByteCallback;
    p->iI2CSlaveTransferComplete = aCallbacks->iTransferCompleteCallback;
    p->iI2CSlaveCallbackWorkspace = aCallbackWorkspace;

    I2C_SET_MASK0(p, 0xFE);
    I2C_SET_ADDR0(p, (aAddressLower7Bits<<1));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_Enable
 *---------------------------------------------------------------------------*/
/**
 *  Enables the I2C channel.
 *
 *  @param [in]    *aWorkspace 		I2C Workspace
 *
 *  @return        T_uezError		Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError ILPC1756_Enable(void *aWorkspace)
{
    T_LPC1756_I2C_Workspace *p = (T_LPC1756_I2C_Workspace *)aWorkspace;

    I2C_ENABLE(p);
    AA_SET(p);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILPC1756_Disable
 *---------------------------------------------------------------------------*/
/**
 *  Disables the I2C channel.
 *
 *  @param [in]    *aWorkspace 			I2C Workspace
 *
 *  @return        T_uezError 			Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError ILPC1756_Disable(void *aWorkspace)
{
    T_LPC1756_I2C_Workspace *p = (T_LPC1756_I2C_Workspace *)aWorkspace;

    I2C_DISABLE(p);

    return UEZ_ERROR_NONE;
}

IRQ_ROUTINE(ILPC1756_I2C1InterruptHandler)
{
    IRQ_START();
    ILPC1756_ProcessState(G_lpc1756_i2c1Workspace);
    IRQ_END();
}

IRQ_ROUTINE(ILPC1756_I2C2InterruptHandler)
{
    IRQ_START();
    ILPC1756_ProcessState(G_lpc1756_i2c2Workspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_I2C_Bus1_InitializeWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Setup the LPC1756 I2C Bus1 workspace.
 *
 *  @param [in]    *aWorkspace  	Particular I2C workspace
 *
 *  @return        T_uezError   	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_I2C_Bus1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1756_I2C_Workspace *p = (T_LPC1756_I2C_Workspace *)aWorkspace;
    p->iRegs = I2C1;
    p->iDoneFlag = ETrue;
    p->iCompleteFunc = 0;
    G_lpc1756_i2c1Workspace = p;

    // Setup interrupt vector
    InterruptRegister(I2C1_IRQn, ILPC1756_I2C1InterruptHandler,
        INTERRUPT_PRIORITY_NORMAL, "I2C1");
    InterruptEnable(I2C1_IRQn);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1756_I2C_Bus2_InitializeWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Setup the LPC1756 I2C Bus2 workspace.
 *
 *  @param [in]    *aWorkspace    	Particular I2C workspace
 *
 *  @return        T_uezError      	Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError LPC1756_I2C_Bus2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1756_I2C_Workspace *p = (T_LPC1756_I2C_Workspace *)aWorkspace;
    p->iRegs = I2C2;
    p->iDoneFlag = ETrue;
    p->iCompleteFunc = 0;
    G_lpc1756_i2c2Workspace = p;

    // Setup interrupt vector
    InterruptRegister(I2C2_IRQn, ILPC1756_I2C2InterruptHandler,
        INTERRUPT_PRIORITY_NORMAL, "I2C2");
    InterruptEnable(I2C2_IRQn);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_I2CBus I2C_LPC1756_Bus1_Interface = {
    {
        "LPC1756 I2C Bus1",
        0x0100,
        LPC1756_I2C_Bus1_InitializeWorkspace,
        sizeof(T_LPC1756_I2C_Workspace), },

    ILPC1756_I2C_StartRead,
    ILPC1756_I2C_StartWrite,
    ILPC1756_ConfigureSlave,
    ILPC1756_Enable,
    ILPC1756_Disable, };

const HAL_I2CBus I2C_LPC1756_Bus2_Interface = {
    {
        "LPC1756 I2C Bus2",
        0x0100,
        LPC1756_I2C_Bus2_InitializeWorkspace,
        sizeof(T_LPC1756_I2C_Workspace), },

    ILPC1756_I2C_StartRead,
    ILPC1756_I2C_StartWrite,
    ILPC1756_ConfigureSlave,
    ILPC1756_Enable,
    ILPC1756_Disable, };

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC1756_I2C1_Require(T_uezGPIOPortPin aPinSDA1, T_uezGPIOPortPin aPinSCL1)
{
    static const T_LPC1756_IOCON_ConfigList sda1[] = { {
        GPIO_P0_0,
        IOCON_I_DEFAULT(3) }, { GPIO_P0_19, IOCON_I_DEFAULT(3) }, };
    static const T_LPC1756_IOCON_ConfigList scl1[] = { {
        GPIO_P0_1,
        IOCON_I_DEFAULT(3) }, { GPIO_P0_20, IOCON_I_DEFAULT(3) }, };

    HAL_DEVICE_REQUIRE_ONCE();

    /** Register I2C Bus driver */
    HALInterfaceRegister("I2C1", (T_halInterface *)&I2C_LPC1756_Bus1_Interface,
        0, 0);
    LPC1756_IOCON_ConfigPin(aPinSDA1, sda1, ARRAY_COUNT(sda1));
    LPC1756_IOCON_ConfigPin(aPinSCL1, scl1, ARRAY_COUNT(scl1));
}

void LPC1756_I2C2_Require(T_uezGPIOPortPin aPinSDA2, T_uezGPIOPortPin aPinSCL2)
{
    static const T_LPC1756_IOCON_ConfigList sda2[] = { {
        GPIO_P0_10,
        IOCON_I_DEFAULT(2) }, };
    static const T_LPC1756_IOCON_ConfigList scl2[] = { {
        GPIO_P0_11,
        IOCON_I_DEFAULT(2) }, };

    HAL_DEVICE_REQUIRE_ONCE();

    /** Register I2C Bus driver */
    HALInterfaceRegister("I2C2", (T_halInterface *)&I2C_LPC1756_Bus2_Interface,
        0, 0);

    LPC1756_IOCON_ConfigPin(aPinSDA2, sda2, ARRAY_COUNT(sda2));
    LPC1756_IOCON_ConfigPin(aPinSCL2, scl2, ARRAY_COUNT(scl2));
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_I2C.c
 *-------------------------------------------------------------------------*/
