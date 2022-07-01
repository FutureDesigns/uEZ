/*-------------------------------------------------------------------------*
 * File:  I2C.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC546xx I2C Interface.
 * Implementation:
 *      A single I2C0 is created, but the code can be easily changed for
 *      other processors to use multiple I2C busses.
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
#include <uEZ.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZBSP.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_I2C.h>

//TODO: Remove
#include "iar/Include/CMSIS/LPC54608.h"

// Setup Master mode only.  Slave mode is a future version but we'll
// leave some of the code in so we don't have to write it later.
#define COMPILE_I2C_MASTER_MODE 1
#define COMPILE_I2C_SLAVE_MODE  0 //Not currently supported

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef enum{
    I2C_STATE_IDLE = 0,
    I2C_STATE_START,
    I2C_STATE_SUBADDRESS,
    I2C_STATE_TRANSMIT,
    I2C_STATE_RECEIVE,
    I2C_STATE_STOP,
}T_I2CState;

typedef enum{
    I2C_DIR_READ = 0,
    I2C_DIR_WRITE,
}T_Direction;

typedef struct{
    T_I2CState iState;
    T_Direction iDir;
    TUInt32 iTransferCount;
}T_TransferState;

#if 0
typedef TBool (*I2CSlaveIsLastReceiveByte)(T_LPC546xx_I2C_Workspace *aWorkspace);
typedef TBool (*I2CSlaveIsLastTransmitByte)(T_LPC546xx_I2C_Workspace *aWorkspace);
typedef void (*I2CTransferComplete)(T_LPC546xx_I2C_Workspace *aWorkspace);
typedef void (*I2CSlaveReceiveByte)(T_LPC546xx_I2C_Workspace *aWorkspace, TUInt8 aByte);
typedef TUInt8 (*I2CSlaveGetTransmitByte)(T_LPC546xx_I2C_Workspace *aWorkspace);
#endif

typedef struct {
    const HAL_I2CBus *iHAL;
    I2C_Type *iRegs;
    I2C_Request *iRequest;
    TUInt8 iAddress;
    TUInt8 *iData;
    TUInt8 iDataLen;
    TUInt8 iIndex;
    I2CRequestCompleteCallback iCompleteFunc;
    void *iCompleteWorkspace;
    TUInt16 iPowerBits;
    T_uezGPIOPortPin iSDAPin;
    T_uezGPIOPortPin iSCLPin;
    T_TransferState iTransferState;
    volatile TBool iDoneFlag;
#if COMPILE_I2C_SLAVE_MODE
I2CSlaveIsLastReceiveByte iI2CSlaveIsLastReceiveByte;
I2CSlaveIsLastTransmitByte iI2CSlaveIsLastTransmitByte;
I2CTransferComplete iI2CTransferComplete;
I2CSlaveReceiveByte iI2CSlaveReceiveByte;
I2CSlaveGetTransmitByte iI2CSlaveGetTransmitByte;
#endif
} T_LPC546xx_I2C_Workspace;

typedef TUInt8 T_LPC546xx_i2cMode;
#define I2C_MODE_READ           1
#define I2C_MODE_WRITE          0

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
#define STAT_MASK                   (I2C_STAT_MSTARBLOSS_MASK | I2C_STAT_MSTSTSTPERR_MASK)
#define I2C_STAT_MSTCODE_IDLE       (0)    /*!< Master Idle State Code */
#define I2C_STAT_MSTCODE_RXREADY    (1) /*!< Master Receive Ready State Code */
#define I2C_STAT_MSTCODE_TXREADY    (2) /*!< Master Transmit Ready State Code */
#define I2C_STAT_MSTCODE_NACKADR    (3) /*!< Master NACK by slave on address State Code */
#define I2C_STAT_MSTCODE_NACKDAT    (4) /*!< Master NACK by slave on data State Code */

#define I2C_SRC_CLOCK_RATE          (12000000)

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_LPC546xx_I2C_Workspace *G_LPC546xx_i2c2Workspace;

/*-------------------------------------------------------------------------*
 * Function Prototypes:
 *-------------------------------------------------------------------------*/
IRQ_ROUTINE(ILPC546xx_I2C2InterruptHandler);

void ILPC546xx_ProcessState(T_LPC546xx_I2C_Workspace *p);

/*-------------------------------------------------------------------------*/
#if 0 // COMPILE_I2C_SLAVE_MODE
static TBool ILPC546xx_I2CSlaveIsLastReceiveByte(T_LPC546xx_I2C_Workspace *aWorkspace)
{
    if (aWorkspace->iIndex >= aWorkspace->iDataLen)
    return ETrue;
    return EFalse;
}

static TBool ILPC546xx_I2CSlaveIsLastTransmitByte(T_LPC546xx_I2C_Workspace *aWorkspace)
{
    if (aWorkspace->iIndex >= aWorkspace->iDataLen)
    return ETrue;
    return EFalse;
}

static void ILPC546xx_I2CTransferComplete(T_LPC546xx_I2C_Workspace *aWorkspace)
{
    // Do nothing
    //    PARAM_NOT_USED(aWorkspace);
}

static void ILPC546xx_I2CSlaveReceiveByte(T_LPC546xx_I2C_Workspace *aWorkspace, TUInt8 aByte)
{
    // Only store if there is room
    if (aWorkspace->iIndex < aWorkspace->iDataLen)
    aWorkspace->iData[aWorkspace->iIndex] = aByte;
}

static TUInt8 ILPC546xx_I2CSlaveGetTransmitByte(T_LPC546xx_I2C_Workspace *aWorkspace)
{
    // Only read from memory if in bounds
    if (aWorkspace->iIndex < aWorkspace->iDataLen)
    return aWorkspace->iData[aWorkspace->iIndex];
    return 0;
}
#endif

void ILPC546xx_I2CSetSpeed(I2C_Type *aRegs, TUInt16 aSpeed)
{
    TUInt32 scl, divider;
    TUInt32 best_scl, best_sch, best_div;
    TUInt32 err, best_err;
    TUInt32 speed = aSpeed * 1000;

    best_err = 0;

    for (scl = 9; scl >= 2; scl--)
    {
        /* calculated ideal divider value for given scl */
        divider = I2C_SRC_CLOCK_RATE / (speed * scl * 2u);

        /* adjust it if it is out of range */
        divider = (divider > 0x10000u) ? 0x10000 : divider;

        /* calculate error */
        err = I2C_SRC_CLOCK_RATE - (speed * scl * 2u * divider);
        if ((err < best_err) || (best_err == 0))
        {
            best_div = divider;
            best_scl = scl;
            best_err = err;
        }

        if ((err == 0) || (divider >= 0x10000u))
        {
            /* either exact value was found
               or divider is at its max (it would even greater in the next iteration for sure) */
            break;
        }
    }

    if(aSpeed == 400){
        best_sch = best_scl - 2;
        best_scl -= 1;
    } else if (aSpeed == 100){
        best_sch = best_scl;   
    }
    aRegs->CLKDIV = I2C_CLKDIV_DIVVAL(best_div - 1);
    aRegs->MSTTIME = I2C_MSTTIME_MSTSCLLOW(best_scl - 2u) | I2C_MSTTIME_MSTSCLHIGH(best_sch - 2u);
}

void ILPC546xx_I2CInit(T_LPC546xx_I2C_Workspace *p)
{
    //Reset the controller
    p->iRegs->CFG &= ~(I2C_CFG_MSTEN_MASK);

    //Enable the controller
    p->iRegs->CFG = (p->iRegs->CFG & 0x1F) | I2C_CFG_MSTEN_MASK;

    // Program the speed
    ILPC546xx_I2CSetSpeed(p->iRegs, p->iRequest->iSpeed);
}

static void ILPC546xx_I2C_PrepStateMachine(T_TransferState *aState,
        T_Direction aDirection)
{
    aState->iState = I2C_STATE_START;
    aState->iTransferCount = 0;
    aState->iDir = aDirection;
}

static void ILPC546xx_I2C_StartWrite(
        void *aWorkspace,
        I2C_Request *iRequest,
        void *aCallbackWorkspace,
        I2CRequestCompleteCallback aCallbackFunc)
{
    T_LPC546xx_I2C_Workspace *p = (T_LPC546xx_I2C_Workspace *)aWorkspace;

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

    //Return if busy
    //TODO

    ILPC546xx_I2CInit(p);

    //Disable interrupts
    p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                         I2C_INTSTAT_MSTARBLOSS_MASK |
                         I2C_INTSTAT_MSTSTSTPERR_MASK;

    //Prepare transfer state machine
    ILPC546xx_I2C_PrepStateMachine(&p->iTransferState, I2C_DIR_WRITE);

    //Clear Interrupt status flag
    p->iRegs->STAT = I2C_STAT_MSTARBLOSS_MASK | I2C_STAT_MSTSTSTPERR_MASK;

    //Enable interrupts
    p->iRegs->INTENSET = I2C_INTSTAT_MSTPENDING_MASK |
                         I2C_INTSTAT_MSTARBLOSS_MASK |
                         I2C_INTSTAT_MSTSTSTPERR_MASK;
}

static void ILPC546xx_I2C_StartRead(
        void *aWorkspace,
        I2C_Request *iRequest,
        void *aCallbackWorkspace,
        I2CRequestCompleteCallback aCallbackFunc)
{
    T_LPC546xx_I2C_Workspace *p = (T_LPC546xx_I2C_Workspace *)aWorkspace;

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

    //Return if busy
    //TODO

    //Disable interrupts
    p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                         I2C_INTSTAT_MSTARBLOSS_MASK |
                         I2C_INTSTAT_MSTSTSTPERR_MASK;

    ILPC546xx_I2CInit(p);

    //Prepare transfer state machine
    ILPC546xx_I2C_PrepStateMachine(&p->iTransferState, I2C_DIR_READ);

    //Clear Interrupt status flag
    p->iRegs->STAT = I2C_STAT_MSTARBLOSS_MASK | I2C_STAT_MSTSTSTPERR_MASK;

    //Enable interrupts
    p->iRegs->INTENSET = I2C_INTSTAT_MSTPENDING_MASK |
                         I2C_INTSTAT_MSTARBLOSS_MASK |
                         I2C_INTSTAT_MSTSTSTPERR_MASK;
}

/*-------------------------------------------------------------------------*
 * Routine:  ILPC546xx_ProcessState
 *-------------------------------------------------------------------------*
 * Description:
 *      Core I2C processing (typically done in interrupt routine)
 *-------------------------------------------------------------------------*/
void ILPC546xx_ProcessState(T_LPC546xx_I2C_Workspace *p)
{
    TUInt32 status = 0;
    TUInt32 master_state = 0;

    status = p->iRegs->STAT;

    if(status & I2C_STAT_MSTARBLOSS_MASK){
        p->iRegs->STAT = (I2C_STAT_MSTARBLOSS_MASK & STAT_MASK);
        p->iRequest->iStatus = I2C_ERROR;
        if(p->iCompleteFunc){
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
        }
        //Disable interrupts
        p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                             I2C_INTSTAT_MSTARBLOSS_MASK |
                             I2C_INTSTAT_MSTSTSTPERR_MASK;
        return;
    }

    if(status & I2C_STAT_MSTSTSTPERR_MASK){
        p->iRegs->STAT = (I2C_STAT_MSTSTSTPERR_MASK & STAT_MASK);
        p->iRequest->iStatus = I2C_ERROR;
        if(p->iCompleteFunc){
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
        }
        //Disable interrupts
        p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                             I2C_INTSTAT_MSTARBLOSS_MASK |
                             I2C_INTSTAT_MSTSTSTPERR_MASK;
        return;
    }

    if((status & I2C_STAT_MSTPENDING_MASK) == 0){
        p->iRequest->iStatus = I2C_ERROR;
        if(p->iCompleteFunc){
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
        }
        //Disable interrupts
        p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                             I2C_INTSTAT_MSTARBLOSS_MASK |
                             I2C_INTSTAT_MSTSTSTPERR_MASK;
        return;
    }

    master_state = (p->iRegs->STAT & 0x06) >> I2C_STAT_MSTSTATE_SHIFT;

    if ((master_state == I2C_STAT_MSTCODE_NACKADR)
            || (master_state == I2C_STAT_MSTCODE_NACKDAT)) {
        p->iRegs->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
        p->iRequest->iStatus = I2C_NAK;
        if(p->iCompleteFunc){
            p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
        }
        //Disable interrupts
        p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                             I2C_INTSTAT_MSTARBLOSS_MASK |
                             I2C_INTSTAT_MSTSTSTPERR_MASK;
        return;
    }

    p->iRequest->iStatus = I2C_OK;
    switch(p->iTransferState.iState){
        case I2C_STATE_START:
            if(p->iTransferState.iDir == I2C_DIR_WRITE){
                p->iRegs->MSTDAT = (TUInt32)p->iRequest->iAddr << 1;
                p->iTransferState.iState = p->iDataLen ?
                        I2C_STATE_TRANSMIT : I2C_STATE_STOP;
            } else { //Read
                p->iRegs->MSTDAT = (TUInt32)p->iRequest->iAddr << 1 | 1;
                p->iTransferState.iState = p->iDataLen ?
                        I2C_STATE_RECEIVE : I2C_STATE_STOP;
            }
            /* Send start condition */
            p->iRegs->MSTCTL = I2C_MSTCTL_MSTSTART_MASK;
            break;
        case I2C_STATE_SUBADDRESS:
            //TODO: Decide if needed
            break;
        case I2C_STATE_TRANSMIT:
            if (master_state != I2C_STAT_MSTCODE_TXREADY){
                p->iRequest->iStatus = I2C_ERROR;
                if(p->iCompleteFunc){
                    p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
                    //Disable interrupts
                    p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                                         I2C_INTSTAT_MSTARBLOSS_MASK |
                                         I2C_INTSTAT_MSTSTSTPERR_MASK;
                    return;
                }
            }
            p->iRegs->MSTDAT = p->iData[p->iIndex++];
            p->iRegs->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK;
            p->iDataLen--;
            if(p->iDataLen == 0){
                p->iTransferState.iState = I2C_STATE_STOP;
            }
            break;
        case I2C_STATE_RECEIVE:
            if (master_state != I2C_STAT_MSTCODE_RXREADY){
                p->iRequest->iStatus = I2C_ERROR;
                if(p->iCompleteFunc){
                    p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
                    //Disable interrupts
                    p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                                         I2C_INTSTAT_MSTARBLOSS_MASK |
                                         I2C_INTSTAT_MSTSTSTPERR_MASK;
                    return;
                }
            }
            p->iData[p->iIndex++] = p->iRegs->MSTDAT;
            p->iDataLen--;
            if(p->iDataLen == 0){
                p->iTransferState.iState = I2C_STATE_STOP;
                p->iRegs->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
                p->iRequest->iStatus = I2C_OK;
                if(p->iCompleteFunc){
                    p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
                    //Disable interrupts
                    p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                                         I2C_INTSTAT_MSTARBLOSS_MASK |
                                         I2C_INTSTAT_MSTSTSTPERR_MASK;
                    return;
                }
            } else {
                p->iRegs->MSTCTL = I2C_MSTCTL_MSTCONTINUE_MASK;
            }
            break;
        case I2C_STATE_STOP:
            p->iRegs->MSTCTL = I2C_MSTCTL_MSTSTOP_MASK;
            p->iRequest->iStatus = I2C_OK;
            if(p->iCompleteFunc){
                p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
                //Disable interrupts
                p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                                     I2C_INTSTAT_MSTARBLOSS_MASK |
                                     I2C_INTSTAT_MSTSTSTPERR_MASK;
                return;
            }
            break;
        case I2C_STATE_IDLE:
        default:
            p->iRequest->iStatus = I2C_ERROR;
            if(p->iCompleteFunc){
                p->iCompleteFunc(p->iCompleteWorkspace, p->iRequest);
                //Disable interrupts
                p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                                     I2C_INTSTAT_MSTARBLOSS_MASK |
                                     I2C_INTSTAT_MSTSTSTPERR_MASK;
            }
            break;
    }
}

T_uezError ILPC546xx_I2C_IsHung(void *aWorkspace, TBool *aBool)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_LPC546xx_I2C_Workspace *p = (T_LPC546xx_I2C_Workspace *)aWorkspace;
    TBool sda, scl;

    if(p->iSCLPin != GPIO_NONE && p->iSDAPin != GPIO_NONE){
        sda = UEZGPIORead(p->iSDAPin);
        scl = UEZGPIORead(p->iSCLPin);

        *aBool = (!scl || !sda) ? ETrue : EFalse;
    } else {
        *aBool = EFalse;
    }

    return error;
}

T_uezError ILPC546xx_I2C_ResetBus(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_LPC546xx_I2C_Workspace *p = (T_LPC546xx_I2C_Workspace *)aWorkspace;

    //Disable interrupts
    p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                         I2C_INTSTAT_MSTARBLOSS_MASK |
                         I2C_INTSTAT_MSTSTSTPERR_MASK;

    return error;
}

T_uezError ILPC546xx_I2C_Disable(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_LPC546xx_I2C_Workspace *p = (T_LPC546xx_I2C_Workspace *)aWorkspace;

    //Disable interrupts
    p->iRegs->INTENCLR = I2C_INTSTAT_MSTPENDING_MASK |
                         I2C_INTSTAT_MSTARBLOSS_MASK |
                         I2C_INTSTAT_MSTSTSTPERR_MASK;

    return error;
}

IRQ_ROUTINE(ILPC546xx_I2C2InterruptHandler)
{
    IRQ_START();
    ILPC546xx_ProcessState(G_LPC546xx_i2c2Workspace);
    IRQ_END();
}


/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_I2C_Bus2_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx I2C Bus2 workspace.
 * Inputs:
 *      void *aW                    -- Particular I2C workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_I2C_Bus2_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_I2C_Workspace *p = (T_LPC546xx_I2C_Workspace *)aWorkspace;
    p->iRegs = I2C2;
    p->iDoneFlag = ETrue;
    p->iCompleteFunc = 0;
    G_LPC546xx_i2c2Workspace = p;
    p->iPowerBits = 19;

    // Setup interrupt vector
    InterruptRegister(FLEXCOMM2_IRQn, ILPC546xx_I2C2InterruptHandler,
            INTERRUPT_PRIORITY_NORMAL, "I2C1");
    InterruptEnable(FLEXCOMM2_IRQn);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HAL Interface tables:
 *---------------------------------------------------------------------------*/
const HAL_I2CBus I2C_LPC546xx_Bus2_Interface = { {
        "LPC546xx I2C Bus1",
        0x0100,
        LPC546xx_I2C_Bus2_InitializeWorkspace,
        sizeof(T_LPC546xx_I2C_Workspace), },

        ILPC546xx_I2C_StartRead,
        ILPC546xx_I2C_StartWrite,
        0,0,
        ILPC546xx_I2C_Disable,
        ILPC546xx_I2C_IsHung,
        ILPC546xx_I2C_ResetBus
};

#define IOCON_D_DEFAULT(funcNum)  \
    IOCON_D(IOCON_FUNC(funcNum), \
            IOCON_PULL_UP, \
            IOCON_INVERT_DISABLE, \
            IOCON_DIGITAL, \
            IOCON_FILTER_ON, \
            IOCON_SLEW_STANDARD, \
            IOCON_NORMAL)

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_I2C2_Require(T_uezGPIOPortPin aPinSDA, T_uezGPIOPortPin aPinSCL)
{
    T_LPC546xx_I2C_Workspace *p;

    static const T_LPC546xx_ICON_ConfigList sda1[] = {
            {GPIO_P4_20     , IOCON_D_DEFAULT(3)},
    };
    static const T_LPC546xx_ICON_ConfigList scl1[] = {
            {GPIO_P4_21     , IOCON_D_DEFAULT(3)},
    };

    HALInterfaceRegister("I2C2",
            (T_halInterface *)&I2C_LPC546xx_Bus2_Interface, 0,
            (T_halWorkspace **)&p);

    //Setup and lock the Flexcomm for I2C mode
    LPC546xxPowerOn(kCLOCK_FlexComm2);
    SYSCON->FCLKSEL[2] = 0x00; //FRO 12MHz
    FLEXCOMM2->PSELID = 0x03; //Lock, I2C mode
    FLEXCOMM2->PSELID |= (1<<3);

    LPC546xx_ICON_ConfigPinOrNone(aPinSDA, sda1, ARRAY_COUNT(sda1));
    LPC546xx_ICON_ConfigPinOrNone(aPinSCL, scl1, ARRAY_COUNT(scl1));

    p->iSCLPin = aPinSCL;
    p->iSDAPin = aPinSDA;
}

/*-------------------------------------------------------------------------*
 * End of File:  I2C.c
 *-------------------------------------------------------------------------*/
