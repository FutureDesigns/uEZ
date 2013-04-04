/*-------------------------------------------------------------------------*
 * File:  RX62N_I2C.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the Renesas RX62N I2C Interface using RIIC.
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
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/Interrupt.h>
#include "RX62N_UtilityFuncs.h"
#include "RX62N_I2C.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef volatile struct st_riic __evenaccess T_RX62N_i2cRegs;

typedef enum {
    I2C_STATE_IDLE,
    I2C_STATE_START,
    I2C_STATE_ADDR,
    I2C_STATE_DATA,
    I2C_STATE_END,
    I2C_STATE_STOP,
} T_i2cState;

typedef enum {
    I2C_INTERRUPT_ICEEI,
    I2C_INTERRUPT_ICRXI,
    I2C_INTERRUPT_ICTXI,
    I2C_INTERRUPT_ICTEI,
} T_i2cInterrupt;

typedef struct {
    const HAL_I2CBus *iHAL;
    T_RX62N_i2cRegs *iRegs;
    I2C_Request *iRequest;
    TUInt8 iAddress;
    TUInt8 *iData;
    TUInt8 iDataLen;
    TUInt8 iIndex;
    I2CRequestCompleteCallback iCompleteFunc;
    void *iCompleteWorkspace;
    TUInt8 iMSTPBBit;
    TBool iFirstInit;
    T_i2cState iState;
    volatile TBool iDoneFlag;
} T_RX62N_I2C_Workspace;

typedef TUInt8 T_RX62N_i2cMode;
#define I2C_MODE_READ           1
#define I2C_MODE_WRITE          0

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
#define BIT_TDRE    (1<<7)
#define BIT_TEND    (1<<6)
#define BIT_RDRF    (1<<5)
#define BIT_NACKF   (1<<4)
#define BIT_STOP    (1<<3)
#define BIT_START   (1<<2)
#define BIT_AL      (1<<1)
#define BIT_TMOF    (1<<0)

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_RX62N_I2C_Workspace *G_RX62N_i2c0Workspace;
static T_RX62N_I2C_Workspace *G_RX62N_i2c1Workspace;

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
IRQ_ROUTINE( IRX62N_I2C0InterruptHandler_ICEEI);
IRQ_ROUTINE( IRX62N_I2C0InterruptHandler_ICTXI);
IRQ_ROUTINE( IRX62N_I2C0InterruptHandler_ICRXI);
IRQ_ROUTINE( IRX62N_I2C0InterruptHandler_ICTEI);
IRQ_ROUTINE( IRX62N_I2C1InterruptHandler_ICEEI);
IRQ_ROUTINE( IRX62N_I2C1InterruptHandler_ICTXI);
IRQ_ROUTINE( IRX62N_I2C1InterruptHandler_ICRXI);
IRQ_ROUTINE( IRX62N_I2C1InterruptHandler_ICTEI);

static void IRX62N_ProcessState(
        T_RX62N_I2C_Workspace *p,
        T_i2cInterrupt aInterrupt);
static void IRX62N_ProcessStateWrite(
        T_RX62N_I2C_Workspace *p,
        T_i2cInterrupt aInterrupt);

#if 0 // #if DEBUG_I2C_TRACK_INTERRUPTS
#define TRACK_SIZE 200
char G_track[TRACK_SIZE];
static int track_num = 0;
static void track_start(void)
{
    int i;
    for (i=0; i<TRACK_SIZE; i++)
    G_track[i] = 0;
    track_num = 0;
}
static void track(char c)
{
    if (track_num < TRACK_SIZE)
    G_track[track_num++] = c;
}
static void track_hex(TUInt8 aHex)
{
    static const TUInt8 hex[] = "0123456789ABCDEF";
    if (track_num < TRACK_SIZE)
    G_track[track_num++] = hex[aHex>>4];
    if (track_num < TRACK_SIZE)
    G_track[track_num++] = hex[aHex&0x0F];
}
#else
#define track_start()       ((void)0)
#define track(c)            ((void)0)
#define track_hex(hex)      ((void)0)
#endif

/*-------------------------------------------------------------------------*/
void IRX62N_I2CSetSpeed(T_RX62N_i2cRegs *aRegs, TUInt16 aSpeed)
{
    T_RX62N_i2cRegs *p_regs = aRegs;
    TUInt32 clocks;
    TUInt8 n;

    clocks = (PCLK_FREQUENCY / 1000) / aSpeed;
    n = 0;
    while (clocks >= 32) {
        n++;
        clocks /= 2;
    }

    // TBD: The following assumes perfect rise and fall times on the
    //      low and high parts of the I2C timing.  If this is not the case,
    //      the resulting speed will be somewhat slower.
    // 7 works
    p_regs->ICMR1.BIT.CKS = 3; //n;
    p_regs->ICBRL.BIT.BRL = 25; //(clocks>>1)-1;
    p_regs->ICBRH.BIT.BRH = 25; // (clocks>>1)-1;
}

void IRX62N_I2CInit(T_RX62N_I2C_Workspace *p)
{
    T_RX62N_i2cRegs *p_regs = p->iRegs;
    TUInt32 delay;

    if (!p->iFirstInit) {
        // Turn on the I2C peripheral (by changing MSTPCRB bit to zero)
        SYSTEM.MSTPCRB.LONG &= ~(1 << p->iMSTPBBit);

        // Disable the RIIC
        p_regs->ICCR1.BIT.ICE = 0;

        // Do an internal reset
        p_regs->ICCR1.BIT.IICRST = 1;
        // Wait for reset
        for (delay = 0; delay < 1000000; delay++) {
        }
        // Cancel reset
        p_regs->ICCR1.BIT.IICRST = 0;

        // Disable slave addr (we are master mode)
        p_regs->ICSER.BIT.SAR0E = 0;

        // Configure the speed
        IRX62N_I2CSetSpeed(p->iRegs, p->iRequest->iSpeed);

        // Enable ACKBT
        p_regs->ICMR3.BIT.ACKWP = 1;

        // Don't worry about timeout
        //p_regs->ICFER.BIT.TMOE = 0;
        // No, enable timeout
        p_regs->ICFER.BIT.TMOE = 1;

        // Enable master arbitration lost detection
        p_regs->ICFER.BIT.MALE = 1;
        ///	    p_regs->ICFER.BIT.MALE = 0;

        // Done with configuration, turn on I2C
        p_regs->ICCR1.BIT.ICE = 1;

        // Initialized/Reset
        p->iFirstInit = ETrue;
    } else {
        // Configure the speed
        IRX62N_I2CSetSpeed(p->iRegs, p->iRequest->iSpeed);

        // Done with configuration, turn on I2C
        //	    p_regs->ICCR1.BIT.ICE = 1;
    }
}

static T_uezError IRX62N_I2C_ForceBusReset(void *aWorkspace)
{
    T_RX62N_I2C_Workspace *p = (T_RX62N_I2C_Workspace *)aWorkspace;
    T_RX62N_i2cRegs *p_regs = p->iRegs;
    TUInt32 i;
    TUInt8 numTries;
    TUInt16 timeout;

    // Determine if the SDA line is low
//    if (p_regs->ICCR1.BIT.SDAI == 1) {
        // If SDA is high, I don't know what the problem is
//        return UEZ_ERROR_COULD_NOT_RESET;
//    }

    // Let's force a reset of the I2C bus.
    // But, first, reset the RIIC itself. This requires several steps.
    p_regs->ICCR1.BIT.ICE = 0;
    p_regs->ICCR1.BIT.IICRST = 1;
    // wait a bit here
    for (i = 0; i < 10000; i++)
        ;
    p_regs->ICCR1.BIT.IICRST = 0;
    // wait a bit here
    for (i = 0; i < 10000; i++)
        ;
    // Turn back on the lines
    p_regs->ICCR1.BIT.ICE = 1;

    // Initialize the I2C fully again
    p->iFirstInit = EFalse;
    IRX62N_I2CInit(p);

    // If after reset the clock is stuck low, we cannot fix it this way
    if (p_regs->ICCR1.BIT.SCLI == 0) {
        return UEZ_ERROR_COULD_NOT_RESET;
    }

    // Alright, now we are ready to do the bus reset
    // What has to happen is we need to output extra clocks
    // on SCLO.  The slave device that is holding the device
    // just needs to be released.  We'll know when the slave released
    // by the SDA rising again.
    // Do this a limited number of times.
    for (numTries = 0; numTries < 16; numTries++) {
        // Force out a clock cycle (CLO will go zero automatically)
        p_regs->ICCR1.BIT.CLO = 1;
        // See if the SDA drops
        timeout = 10000;
        while (timeout--) {
            // Did the SDA line rise?
            if (p_regs->ICCR1.BIT.SDAI == 1)
                break;
        }
        // If we didn't timeout, then it worked
        if (timeout)
            break;
    }

    // If we got here and SDA is low, then we failed
    if (p_regs->ICCR1.BIT.SDAI == 0)
        return UEZ_ERROR_COULD_NOT_RESET;

    // Otherwise, we have success
    return UEZ_ERROR_NONE;
}

static void IRX62N_I2C_StartWriteInterrupts(
        void *aWorkspace,
        I2C_Request *iRequest,
        void *aCallbackWorkspace,
        I2CRequestCompleteCallback aCallbackFunc)
{
    T_RX62N_I2C_Workspace *p = (T_RX62N_I2C_Workspace *)aWorkspace;
    T_RX62N_i2cRegs *p_regs = p->iRegs;
    TUInt32 timeout;

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
    IRX62N_I2CInit(p);
    track_start();

    // Wait a bit of time to if busy or low sda line
    timeout = 10000;
    while (((p_regs->ICCR2.BIT.BBSY) || (p_regs->ICCR1.BIT.SDAI == 0)) && (timeout)) {
        timeout--;
    }
    
    // Don't process if the bus is busy!
    if ((p_regs->ICCR2.BIT.BBSY) || (p_regs->ICCR1.BIT.SDAI == 0)) {
        // Assume we are locked up (we should never overlap actions!)
        // Try to reset the bus
        if (IRX62N_I2C_ForceBusReset(aWorkspace) != UEZ_ERROR_NONE) {
            // Reset of the bus failed
            p->iDoneFlag = ETrue;
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            p->iRequest->iStatus = I2C_NOT_READY;
            return;
        }
    }

    // Issue a start condition and let the interrupts do the rest
    p_regs->ICSR2.BIT.STOP = 0;
    p->iState = I2C_STATE_START;
    p_regs->ICCR2.BIT.ST = 1;

    // Start receiving interrupts
    // I2C Bus Interrupt Enable Register (ICIER)
    // 1--- ----   TIE = Transmit data empty interrupt request (ICTXI) is enabled
    // -1-- ----   TEIE = Transmit end interrupt request (ICTEI) is enabled
    // --1- ----   RIE = Receive data full interrupt request (ICRXI) is enabled
    // ---1 ----   NAKIE = NACK reception interrupt request (NAKI) is enabled
    // ---- 1---   SPIE = Stop condition detection interrupt request (SPI) is enabled
    // ---- -1--   STIE = Start condition detection interrupt request (STI) is enabled
    // ---- --1-   ALIE = Arbitration lost interrupt request (ALI) is enabled
    // ---- ---1   TMOIE = Timeout interrupt request (TMOI) is enabled
    p_regs->ICIER.BYTE = 0xFF; // 0xFB;
}

static void IRX62N_I2C_StartReadInterrupts(
        void *aWorkspace,
        I2C_Request *iRequest,
        void *aCallbackWorkspace,
        I2CRequestCompleteCallback aCallbackFunc)
{
    T_RX62N_I2C_Workspace *p = (T_RX62N_I2C_Workspace *)aWorkspace;
    T_RX62N_i2cRegs *p_regs = p->iRegs;
    TUInt32 timeout;

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
    IRX62N_I2CInit(p);
    track_start();
    track('|');

    // Wait a bit of time to if busy or low sda line
    timeout = 10000;
    while (((p_regs->ICCR2.BIT.BBSY) || (p_regs->ICCR1.BIT.SDAI == 0)) && (timeout)) {
        timeout--;
    }
    
    // Don't process if the bus is busy!
    if ((p_regs->ICCR2.BIT.BBSY) || (p_regs->ICCR1.BIT.SDAI == 0)) {
        // Assume we are locked up (we should never overlap actions!)
        // Try to reset the bus
        if (IRX62N_I2C_ForceBusReset(aWorkspace) != UEZ_ERROR_NONE) {
            // Reset of the bus failed
            p->iDoneFlag = ETrue;
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
            p->iRequest->iStatus = I2C_NOT_READY;
            return;
        }
    }

    // Issue a start condition
    p_regs->ICSR2.BIT.STOP = 0;
    p->iState = I2C_STATE_START;
    p_regs->ICCR2.BIT.ST = 1;

    // Start receiving interrupts
    // I2C Bus Interrupt Enable Register (ICIER)
    // 1--- ----   TIE = Transmit data empty interrupt request (ICTXI) is enabled
    // -1-- ----   TEIE = Transmit end interrupt request (ICTEI) is enabled
    // --1- ----   RIE = Receive data full interrupt request (ICRXI) is enabled
    // ---1 ----   NAKIE = NACK reception interrupt request (NAKI) is enabled
    // ---- 1---   SPIE = Stop condition detection interrupt request (SPI) is enabled
    // ---- -1--   STIE = Start condition detection interrupt request (STI) is enabled
    // ---- --1-   ALIE = Arbitration lost interrupt request (ALI) is enabled
    // ---- ---1   TMOIE = Timeout interrupt request (TMOI) is enabled
    p_regs->ICIER.BYTE = 0xFF; // 0xFB;
}

/*-------------------------------------------------------------------------*
 * Routine:  IRX62N_ProcessState
 *-------------------------------------------------------------------------*
 * Description:
 *      Core I2C processing (typically done in interrupt routine)
 *-------------------------------------------------------------------------*/
static void IRX62N_ProcessStateWrite(
        T_RX62N_I2C_Workspace *p,
        T_i2cInterrupt aInterrupt)
{
    T_RX62N_i2cRegs *p_regs = p->iRegs;
    TBool doEnd = EFalse;
    TBool doState = ETrue;
    TBool doStop = EFalse;
    TUInt8 status = p_regs->ICSR2.BYTE;
    TUInt8 dummy;

    dummy = dummy;
    track('-');
    track('0' + aInterrupt);
    if (status & BIT_TMOF) {
        track('/');
        track('T');
        p_regs->ICCR2.BIT.ST = 0;
        p_regs->ICCR2.BIT.RS = 0;
        p_regs->ICCR2.BIT.SP = 0;
        p_regs->ICSR2.BIT.TMOF = 0;
        p->iRequest->iStatus = I2C_ERROR;
        doState = EFalse;
        doEnd = ETrue;
    }
    if (status & BIT_AL) {
        track('/');
        track('L');
        p_regs->ICCR2.BIT.ST = 0;
        p_regs->ICCR2.BIT.RS = 0;
        p_regs->ICCR2.BIT.SP = 0;
        p_regs->ICSR2.BIT.TMOF = 0;
        p->iRequest->iStatus = I2C_ERROR;
        doState = EFalse;
        doEnd = ETrue;
    }
    if (doState) {
        switch (p->iState) {
            case I2C_STATE_START:
                track('S');
                if (status & BIT_STOP) {
                    // We got a stop just as we are starting!
                    // Don't try to continue, but try to abort
                    doStop = ETrue;
                    track('?');
                } else if (status & BIT_TDRE) { // (aInterrupt == I2C_INTERRUPT_ICTXI) {
                    // We just sent the start bit, now send the address
                    p->iState = I2C_STATE_ADDR;
                    p_regs->ICDRT = p->iAddress;
                    track('a');
                } else {
                    track('e');
                    // Got some type of error
                    p->iRequest->iStatus = I2C_ERROR;
                    doStop = ETrue;
                }
                if (status & BIT_START)
                    p_regs->ICSR2.BIT.START = 0;
                break;
            case I2C_STATE_ADDR:
                p_regs->ICSR2.BIT.START = 0;
                track('A');
                if ((aInterrupt == I2C_INTERRUPT_ICTXI)
                        && (p_regs->ICSR2.BIT.TDRE)) {
                    // Finished sending slave address, and got ack
                    // Now send the first byte of the transfer (if any data, else abort)
                    p->iState = I2C_STATE_DATA;
                    if (p->iDataLen) {
                        // Got data, send the first byte and a new interrupt will occur
                        p->iIndex = 0;
                        p_regs->ICDRT = p->iData[p->iIndex++];
                        track('d');
                    } else {
                        // No data payload, just stop here
                        p->iRequest->iStatus = I2C_OK;
                        doStop = ETrue;
                        track('e');
                    }
                } else /* if (aInterrupt == I2C_INTERRUPT_ICEEI) */{
                    // NAK or some type of error
                    p->iRequest->iStatus = I2C_ERROR;
                    doStop = ETrue;
                    track('x');
                }
                break;
            case I2C_STATE_DATA:
                track('D');
                // Trying to send data, did it go through?
                if ((aInterrupt == I2C_INTERRUPT_ICTXI)
                        && (p_regs->ICSR2.BIT.TDRE)) {
                    // Finished sending data and got ack
                    // Now send the next byte of the transfer (if any)
                    p->iState = I2C_STATE_DATA;
                    if (p->iIndex < p->iDataLen) {
                        // Got data to send, send the next byte
                        // and a new interrupt will occur
                        p_regs->ICDRT = p->iData[p->iIndex++];
                        track('d');
                    } else {
                        // We're done with the write.  Let's wait for the final
                        // ending
                        p->iState = I2C_STATE_END;
                        track('e');
                    }
                } else /* if (aInterrupt == I2C_INTERRUPT_ICEEI) */{
                    // NAK or some type of error (NAK noted below)
                    p->iRequest->iStatus = I2C_ERROR;
                    doStop = ETrue;
                }
                break;
            case I2C_STATE_END:
                track('E');
                if ((aInterrupt == I2C_INTERRUPT_ICTEI)
                        && (p_regs->ICSR2.BIT.TEND)) {
                    p->iRequest->iStatus = I2C_OK;
                    doStop = ETrue;
                    track('.');
                } else {
                    p->iRequest->iStatus = I2C_ERROR;
                    doStop = ETrue;
                    track('e');
                }
                break;
            case I2C_STATE_STOP:
                track('P');
                p->iState = I2C_STATE_IDLE;
                // End the stop
                p_regs->ICCR2.BIT.ST = 0;
                p_regs->ICSR2.BIT.STOP = 0;
                doEnd = ETrue;
                break;
        }
    }
    if (doStop) {
        track('T');
        // If we ended with a NAK, mark it as such (otherwise it is an I2C_ERROR)
        if (status & BIT_NACKF)
            p->iRequest->iStatus = I2C_NAK;

        // Done, now send a stop bit (for one more interrupt)
        p_regs->ICSR2.BIT.STOP = 0;
        p_regs->ICCR2.BIT.SP = 1;
        // Clear the receive register
        dummy = p_regs->ICDRR;
        p->iState = I2C_STATE_STOP;
        //doEnd = ETrue;
    }
    if (doEnd) {
        track('!');
        // Done, Clear NACK and STOP flags
        p_regs->ICSR2.BIT.NACKF = 0;
        p_regs->ICSR2.BIT.STOP = 0;

        // We are now done, call the callback routine to report
        // completion.  The callback knows this is inside an ISR
        p->iState = I2C_STATE_IDLE;
        p->iDoneFlag = ETrue;
        p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
        track(';');
        // Stop receiving all interrupts
        p_regs->ICIER.BYTE = 0x00;
    }
}

static void IRX62N_ProcessStateRead(
        T_RX62N_I2C_Workspace *p,
        T_i2cInterrupt aInterrupt)
{
    T_RX62N_i2cRegs *p_regs = p->iRegs;
    TBool doEnd = EFalse;
    TBool doStop = EFalse;
    TBool doState = ETrue;
    TUInt8 dummy;
    TUInt8 status = p_regs->ICSR2.BYTE;

    track(' ');
    track('0' + aInterrupt);
    track_hex(status);
    track_hex(p->iState);
    if (status & BIT_TMOF) {
        track('/');
        track('T');
        p_regs->ICCR2.BIT.ST = 0;
        p_regs->ICCR2.BIT.RS = 0;
        p_regs->ICCR2.BIT.SP = 0;
        p_regs->ICSR2.BIT.TMOF = 0;
        p->iRequest->iStatus = I2C_ERROR;
        doState = EFalse;
        doEnd = ETrue;
    }
    if (status & BIT_AL) {
        track('/');
        track('L');
        p_regs->ICCR2.BIT.ST = 0;
        p_regs->ICCR2.BIT.RS = 0;
        p_regs->ICCR2.BIT.SP = 0;
        p_regs->ICSR2.BIT.TMOF = 0;
        p->iRequest->iStatus = I2C_ERROR;
        doState = EFalse;
        doEnd = ETrue;
    }
    if (doState) {
        switch (p->iState) {
            case I2C_STATE_START:
                //track_start();
                track('S');
                if (status & BIT_STOP) {
                    // We got a stop just as we are starting!
                    // Don't try to continue, but try to abort
                    doStop = ETrue;
                    track('?');
                } else if (status & BIT_TDRE) { // (aInterrupt == I2C_INTERRUPT_ICTXI) {
                    // We just sent the start bit, now send the address
                    p->iState = I2C_STATE_ADDR;
                    p_regs->ICDRT = p->iAddress;
                } else {
                    track('e');
                    // Got some type of error
                    p->iRequest->iStatus = I2C_ERROR;
                    doStop = ETrue;
                }
                if (status & BIT_START)
                    p_regs->ICSR2.BIT.START = 0;
                break;
            case I2C_STATE_ADDR:
                track('A');
                if (status & BIT_NACKF) {
                    track('e');
                    // NAK or some type of error
                    p->iRequest->iStatus = I2C_ERROR;
                    doStop = ETrue;
                } else if (status & BIT_RDRF) {
                    track('t');
                    // Finished sending slave address for read, and got ack (read style)
                    // Now send the first byte of the transfer (if any data, else abort)
                    p->iState = I2C_STATE_DATA;
                    if (p->iDataLen) {
                        track('d');
                        // Request the first byte and the rest the next interrupt
                        // will produce it
                        p->iIndex = 0;
                        p->iState = I2C_STATE_DATA;
                        dummy = p_regs->ICDRR;
                    } else {
                        track('.');
                        // No data payload, just stop here
                        p->iRequest->iStatus = I2C_OK;
                        doStop = ETrue;
                    }
                } else {
                    // Ignore this interrupt
                }
                break;
            case I2C_STATE_DATA:
                track('D');
                // Trying to send data, did it go through?
                if ((aInterrupt == I2C_INTERRUPT_ICRXI) && (status & BIT_RDRF)) {
                    track('r');
                    // Finished receiving data and got ack
                    // Do we have more than 2 bytes to go?
                    if ((p->iDataLen - p->iIndex) > 2) {
                        track('d');
                        // Still more than 2 bytes to go
                        // Store this byte and trigger the next interrupt
                        track('0' + p->iIndex);
                        dummy = p_regs->ICDRR;
                        track_hex(dummy);
                        p->iData[p->iIndex++] = dummy;
                    } else {
                        track('l');
                        // We've got one more byte to go, go ahead and put into
                        // ending mode
                        p->iState = I2C_STATE_END;
                        // Store this byte and trigger the next one but put in wait
                        // mode and put out an ack to the slave.
                        //p_regs->ICMR3.BIT.WAIT = 1;
                        // Ack that this is the last byte
                        p_regs->ICMR3.BIT.ACKBT = 1;
                        // Store this byte
                        track('0' + p->iIndex);
                        dummy = p_regs->ICDRR;
                        track_hex(dummy);
                        p->iData[p->iIndex++] = dummy;
                    }
                } else /* if (aInterrupt == I2C_INTERRUPT_ICEEI) */{
                    track('e');
                    // NAK or some type of error (NAK noted below)
                    p->iRequest->iStatus = I2C_ERROR;
                    doStop = ETrue;
                }
                break;
            case I2C_STATE_END:
                track('E');
                // Get the last byte in
                if (/*(aInterrupt == I2C_INTERRUPT_ICRXI) && */(status
                        & BIT_RDRF)) {
                    track('r');
                    // Got the last byte and ack
                    // Output a stop bit
                    p_regs->ICSR2.BIT.STOP = 0;
                    p_regs->ICCR2.BIT.SP = 1;

                    // read last received byte
                    track('0' + p->iIndex);
                    track_hex(dummy);
                    dummy = p_regs->ICDRR;
                    p->iData[p->iIndex++] = dummy;

                    // Turn off the wait mode
                    p_regs->ICMR3.BIT.WAIT = 0;
                    // Everything is good
                    p->iRequest->iStatus = I2C_OK;
                    p->iState = I2C_STATE_STOP;
                    //doEnd = ETrue;
                } else /* if (aInterrupt == I2C_INTERRUPT_ICEEI) */{
                    track('e');
                    // NAK or some type of error (NAK noted below)
                    p->iRequest->iStatus = I2C_ERROR;
                    doStop = ETrue;
                }
                break;
            case I2C_STATE_STOP:
                track('P');
                p->iState = I2C_STATE_IDLE;
                // End the stop
                p_regs->ICCR2.BIT.ST = 0;
                p_regs->ICSR2.BIT.STOP = 0;
                doEnd = ETrue;
                break;
            case I2C_STATE_IDLE:
                if (status & BIT_STOP) {
                    // Clear any spurious stops
                    p_regs->ICCR2.BIT.SP = 0;
                    p_regs->ICSR2.BIT.STOP = 0;
                }
                break;
        }
    }
    if (doStop) {
        track('T');
        // If we ended with a NAK, mark it as such (otherwise it is an I2C_ERROR)
        if (status & BIT_NACKF)
            p->iRequest->iStatus = I2C_NAK;

        // Done, now send a stop bit (for one more interrupt)
        p_regs->ICSR2.BIT.STOP = 0;
        p_regs->ICCR2.BIT.SP = 1;
        // Clear the receive register
        dummy = p_regs->ICDRR;
        p->iState = I2C_STATE_STOP;
        //doEnd = ETrue;
    }
    if (doEnd) {
        track('!');
        // Wait for stop bit to en quickly
        //        while (!p_regs->ICSR2.BIT.STOP)
        //            {}
        // Done, Clear NACK and STOP flags
        p_regs->ICSR2.BIT.NACKF = 0;
        p_regs->ICSR2.BIT.STOP = 0;

        // We are now done, call the callback routine to report
        // completion.  The callback knows this is inside an ISR
        p->iState = I2C_STATE_IDLE;
        p->iDoneFlag = ETrue;
        p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);

        // Stop receiving all interrupts
        p_regs->ICIER.BYTE = 0x00;
    }
}

static void IRX62N_ProcessState(
        T_RX62N_I2C_Workspace *p,
        T_i2cInterrupt aInterrupt)
{
    if (p->iAddress & 1)
        IRX62N_ProcessStateRead(p, aInterrupt);
    else
        IRX62N_ProcessStateWrite(p, aInterrupt);
}

IRQ_ROUTINE(IRX62N_I2C0InterruptHandler_ICEEI)
{
    IRQ_START();
    IRX62N_ProcessState(G_RX62N_i2c0Workspace, I2C_INTERRUPT_ICEEI);
    IRQ_END();
}
IRQ_ROUTINE(IRX62N_I2C0InterruptHandler_ICRXI)
{
    IRQ_START();
    IRX62N_ProcessState(G_RX62N_i2c0Workspace, I2C_INTERRUPT_ICRXI);
    IRQ_END();
}
IRQ_ROUTINE(IRX62N_I2C0InterruptHandler_ICTXI)
{
    IRQ_START();
    IRX62N_ProcessState(G_RX62N_i2c0Workspace, I2C_INTERRUPT_ICTXI);
    IRQ_END();
}
IRQ_ROUTINE(IRX62N_I2C0InterruptHandler_ICTEI)
{
    IRQ_START();
    IRX62N_ProcessState(G_RX62N_i2c0Workspace, I2C_INTERRUPT_ICTEI);
    IRQ_END();
}

IRQ_ROUTINE(IRX62N_I2C1InterruptHandler_ICEEI)
{
    IRQ_START();
    IRX62N_ProcessState(G_RX62N_i2c1Workspace, I2C_INTERRUPT_ICEEI);
    IRQ_END();
}
IRQ_ROUTINE(IRX62N_I2C1InterruptHandler_ICRXI)
{
    IRQ_START();
    IRX62N_ProcessState(G_RX62N_i2c1Workspace, I2C_INTERRUPT_ICRXI);
    IRQ_END();
}
IRQ_ROUTINE(IRX62N_I2C1InterruptHandler_ICTXI)
{
    IRQ_START();
    IRX62N_ProcessState(G_RX62N_i2c1Workspace, I2C_INTERRUPT_ICTXI);
    IRQ_END();
}
IRQ_ROUTINE(IRX62N_I2C1InterruptHandler_ICTEI)
{
    IRQ_START();
    IRX62N_ProcessState(G_RX62N_i2c1Workspace, I2C_INTERRUPT_ICTEI);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_I2C0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N I2C Bus0 workspace.
 * Inputs:
 *      void *aW                    -- Particular I2C workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_I2C0_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_I2C_Workspace *p = (T_RX62N_I2C_Workspace *)aWorkspace;

    p->iRegs = (T_RX62N_i2cRegs *)&RIIC0;
    p->iDoneFlag = ETrue;
    p->iCompleteFunc = 0;
    p->iMSTPBBit = 21;
    p->iFirstInit = EFalse;
    PORT1.ICR.BIT.B2 = 1;
    PORT1.ICR.BIT.B3 = 1;
    p->iState = I2C_STATE_IDLE;

    G_RX62N_i2c0Workspace = p;

    // Setup interrupt vectors
    InterruptRegister(VECT_RIIC0_ICEEI0, IRX62N_I2C0InterruptHandler_ICEEI,
            INTERRUPT_PRIORITY_LOW, "I2C0:ICEEI");
    InterruptEnable(VECT_RIIC0_ICEEI0);

    InterruptRegister(VECT_RIIC0_ICRXI0, IRX62N_I2C0InterruptHandler_ICRXI,
            INTERRUPT_PRIORITY_LOW, "I2C0:ICRXI");
    InterruptEnable(VECT_RIIC0_ICRXI0);

    InterruptRegister(VECT_RIIC0_ICTXI0, IRX62N_I2C0InterruptHandler_ICTXI,
            INTERRUPT_PRIORITY_LOW, "I2C0:ICTXI");
    InterruptEnable(VECT_RIIC0_ICTXI0);

    InterruptRegister(VECT_RIIC0_ICTEI0, IRX62N_I2C0InterruptHandler_ICTEI,
            INTERRUPT_PRIORITY_LOW, "I2C0:ICTEI");
    InterruptEnable(VECT_RIIC0_ICTEI0);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_I2C1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N I2C Bus1 workspace.
 * Inputs:
 *      void *aW                    -- Particular I2C workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_I2C1_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_I2C_Workspace *p = (T_RX62N_I2C_Workspace *)aWorkspace;

    p->iRegs = (T_RX62N_i2cRegs *)&RIIC1;
    p->iDoneFlag = ETrue;
    p->iCompleteFunc = 0;
    p->iMSTPBBit = 20;
    p->iFirstInit = EFalse;
    PORT2.ICR.BIT.B2 = 1;
    PORT2.ICR.BIT.B1 = 1;

    p->iState = I2C_STATE_IDLE;

    G_RX62N_i2c1Workspace = p;

    // Setup interrupt vectors
    InterruptRegister(VECT_RIIC1_ICEEI1, IRX62N_I2C1InterruptHandler_ICEEI,
            INTERRUPT_PRIORITY_NORMAL, "I2C1:ICEEI");
    InterruptEnable(VECT_RIIC1_ICEEI1);

    InterruptRegister(VECT_RIIC1_ICRXI1, IRX62N_I2C1InterruptHandler_ICRXI,
            INTERRUPT_PRIORITY_NORMAL, "I2C1:ICRXI");
    InterruptEnable(VECT_RIIC1_ICRXI1);

    InterruptRegister(VECT_RIIC1_ICTXI1, IRX62N_I2C1InterruptHandler_ICTXI,
            INTERRUPT_PRIORITY_NORMAL, "I2C1:ICTXI");
    InterruptEnable(VECT_RIIC1_ICTXI1);

    InterruptRegister(VECT_RIIC1_ICTEI1, IRX62N_I2C1InterruptHandler_ICTEI,
            INTERRUPT_PRIORITY_NORMAL, "I2C1:ICTEI");
    InterruptEnable(VECT_RIIC1_ICTEI1);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_I2CBus I2C_RX62N_RIIC0_Interface = {
        "RX62N:I2C0",
        0x0100,
        RX62N_I2C0_InitializeWorkspace,
        sizeof(T_RX62N_I2C_Workspace),

        IRX62N_I2C_StartReadInterrupts,
        IRX62N_I2C_StartWriteInterrupts,
        };

const HAL_I2CBus I2C_RX62N_RIIC1_Interface = {
        "RX62N:I2C1",
        0x0100,
        RX62N_I2C1_InitializeWorkspace,
        sizeof(T_RX62N_I2C_Workspace),

        IRX62N_I2C_StartReadInterrupts,
        IRX62N_I2C_StartWriteInterrupts,
        };
		
void RX62N_RIIC0_Require(void)
{
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_P12,  	// SCL0
			GPIO_P13,	// SDA0
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("RIIC0", (T_halInterface *)&I2C_RX62N_RIIC0_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
}

void RX62N_RIIC1_Require(void)
{
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_P21,  	// SCL1
			GPIO_P20,	// SDA1
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("RIIC1", (T_halInterface *)&I2C_RX62N_RIIC1_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
}

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_I2C.c
 *-------------------------------------------------------------------------*/
