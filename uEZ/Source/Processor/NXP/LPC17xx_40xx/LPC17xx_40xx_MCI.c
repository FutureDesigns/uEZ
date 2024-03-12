/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_MCI.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZMemory.h>
#include <uEZPlatformAPI.h>
#include <HAL/Interrupt.h>
#include <HAL/GPDMA.h>
#include "LPC17xx_40xx_MCI.h"
#include "LPC17xx_40xx_GPIO.h"
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
#include "LPC17xx_40xx_UtilityFuncs.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#if ((COMPILER_TYPE==GCC_ARM) || (COMPILER_TYPE==KEIL_UV))
    #define MCI_MEMORY __attribute__((section(".mcimem")));
#elif (COMPILER_TYPE==IAR)
    #define MCI_MEMORY @ ".mcimem"
#else
    #define MCI_MEMORY // no mods
#endif

#ifndef NUM_MAX_BLOCKS
/* Block transfer FIFO depth (>= 2), 20+ recommended */
#if (UEZ_PROCESSOR==NXP_LPC1788)
#define NUM_MAX_BLOCKS  16 // Set a new permanent setting on this LPC to ensure enough size for USB/Eth and correct placement on AHB
#elif (UEZ_PROCESSOR==NXP_LPC4088)
#define NUM_MAX_BLOCKS  16 // Set a new permanent setting on this LPC to ensure enough size for USB/Eth and correct placement on AHB
#else
#define NUM_MAX_BLOCKS  32 // ideal size
#endif
#endif

#ifndef SDCARD_MCI_RATE_FOR_RW_STATE
    #define SDCARD_MCI_RATE_FOR_RW_STATE       25000000UL // 18000000UL // 18 MHz  // TODO: Need to remove!
#endif

// Easy remap of registers:
#define MCI_POWER       LPC_MCI->POWER      // requires AHB delay
#define MCI_CLOCK       LPC_MCI->CLOCK      // requires AHB delay
#define MCI_ARGUMENT    LPC_MCI->ARGUMENT   // requires AHB delay
#define MCI_COMMAND     LPC_MCI->COMMAND
#define MCI_CLEAR       LPC_MCI->CLEAR
#define MCI_STATUS      LPC_MCI->STATUS
#define MCI_RESP0       LPC_MCI->RESP0
#define MCI_RESP1       LPC_MCI->RESP1
#define MCI_RESP2       LPC_MCI->RESP2
#define MCI_RESP3       LPC_MCI->RESP3
#define MCI_MASK0       LPC_MCI->MASK0
#define MCI_DATA_CTRL   LPC_MCI->DATACTRL   // requires AHB delay
#define MCI_DATA_LEN    LPC_MCI->DATALEN
#define MCI_DATA_TMR    LPC_MCI->DATATMR
#define MCI_FIFO        LPC_MCI->FIFO

/* MCIStatus register bits: */
#define MCI_STATUS_CMD_CRC_FAIL         (1<<0)
#define MCI_STATUS_DATA_CRC_FAIL        (1<<1)
#define MCI_STATUS_CMD_TIME_OUT         (1<<2)
#define MCI_STATUS_DATA_TIME_OUT        (1<<3)
#define MCI_STATUS_TX_UNDERRUN          (1<<4)
#define MCI_STATUS_RX_OVERRUN           (1<<5)
#define MCI_STATUS_CMD_RESP_END         (1<<6)
#define MCI_STATUS_CMD_CMD_SENT         (1<<7)
#define MCI_STATUS_DATA_END             (1<<8)
#define MCI_STATUS_START_BIT_ERR        (1<<9)
#define MCI_STATUS_DATA_BLOCK_END       (1<<10)
#define MCI_STATUS_CMD_ACTIVE           (1<<11)
#define MCI_STATUS_TX_ACTIVE            (1<<12)
#define MCI_STATUS_RX_ACTIVE            (1<<13)
#define MCI_STATUS_TX_FIFO_HALF_EMPTY   (1<<14)
#define MCI_STATUS_RX_FIFO_HALF_FULL    (1<<15)
#define MCI_STATUS_TX_FIFO_FULL         (1<<16)
#define MCI_STATUS_RX_FIFO_FULL         (1<<17)
#define MCI_STATUS_TX_FIFO_EMPTY        (1<<18)
#define MCI_STATUS_RX_FIFO_EMPTY        (1<<19)
#define MCI_STATUS_TX_DATA_AVAIL        (1<<20)
#define MCI_STATUS_RX_DATA_AVAIL        (1<<21)

#define MCI_STATUS_ERRORS \
    (MCI_STATUS_DATA_CRC_FAIL|MCI_STATUS_DATA_TIME_OUT|MCI_STATUS_RX_OVERRUN| \
        MCI_STATUS_TX_UNDERRUN | MCI_STATUS_DATA_END|MCI_STATUS_START_BIT_ERR| \
            MCI_STATUS_DATA_BLOCK_END)
      
/*#define MCI_STATUS_ERRORS \
    (MCI_STATUS_CMD_CRC_FAIL|MCI_STATUS_CMD_TIME_OUT| \
      MCI_STATUS_DATA_CRC_FAIL|MCI_STATUS_DATA_TIME_OUT|MCI_STATUS_RX_OVERRUN| \
        MCI_STATUS_TX_UNDERRUN | MCI_STATUS_DATA_END|MCI_STATUS_START_BIT_ERR| \
            MCI_STATUS_DATA_BLOCK_END)*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const HAL_MCI *iHAL;

    T_uezGPIOPortPin iCardDetect;
    T_uezGPIOPortPin iWriteProtect;

    T_mciReceptionComplete iReceptionCompleteCallback;
    void *iReceptionCompleteCallbackWorkspace;

    T_mciTransmissionComplete iTransmissionCompleteCallback;
    void *iTransmissionCompleteCallbackWorkspace;

    T_mciError iErrorCallback;
    void *iErrorCallbackWorkspace;
    
    TUInt32 iRCA;       // Relative Card Address

    HAL_GPDMA **iGPDMA;
    TBool iIRQActive;

    // Position in buffer array to receive data
    TUInt8 iBufferPosIn;

    // Position in buffer to take data out of
    TUInt8 iBufferPosOut;

    // Number we are writing
    TUInt8 iNumWriting;

    // Status of current transfer
    TUInt8 iTransferStatus;

    TUInt32 iLastMCIError;
} T_LPC17xx_40xx_MCI_Workspace;

#define TRANSFER_STATUS_READING             0x01
#define TRANSFER_STATUS_WRITING             0x02
#define TRANSFER_STATUS_BUFFER_OVERRUN      0x04
#define TRANSFER_STATUS_MCI_ERROR           0x08
#define TRANSFER_STATUS_DONE                0x10
#define TRANSFER_STATUS_ANY_ERROR   \
    (TRANSFER_STATUS_BUFFER_OVERRUN|TRANSFER_STATUS_MCI_ERROR)

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/



/* Block transfer FIFO -- MUST be placed in AHB memory */
UEZ_ALIGN_VAR(32,static TUInt32 G_dmaBlocks[NUM_MAX_BLOCKS][128] MCI_MEMORY);
/* DMA link lists -- MUST be placed in AHB memory */
UEZ_ALIGN_VAR(32,static TUInt32 G_linkListRead[NUM_MAX_BLOCKS][4] MCI_MEMORY);
UEZ_ALIGN_VAR(32,static TUInt32 G_linkListWrite[NUM_MAX_BLOCKS][4] MCI_MEMORY);
T_LPC17xx_40xx_MCI_Workspace *G_mciWorkspace;

#define Copy_al2un memcpy // This is ok if our buffers above and application buffer are 32 aligned.

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC17xx_40xx_MCI_ProcessInterrupt(T_LPC17xx_40xx_MCI_Workspace *p);

/* Note: After a data write, data cannot be written to this register for three
 * MCLK clock periods plus two PCLK clock periods. Applies to these registers:
 * LPC_MCI->POWER LPC_MCI->CLOCK LPC_MCI->ARGUMENT LPC_MCI->DATACTRL
*/
UEZ_FUNC_OPT(UEZ_OPT_LEVEL_NONE,static void writeDelay(void))
{
	volatile uint8_t i;
	for ( i = 0; i < 0x10; i++ ) {	/* delay 3MCLK + 2PCLK  */
	}
}

/*---------------------------------------------------------------------------*
 * Interrupt:  LPC17xx_40xx_MCI_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      A MCI Interrupt has come in.  Process the one workspace attached
 *      to this interrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(LPC17xx_40xx_MCI_Interrupt)
{
    IRQ_START();
    LPC17xx_40xx_MCI_ProcessInterrupt(G_mciWorkspace);
    IRQ_END();
}



/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_ProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Process an interrupt for this workspace
 * Inputs:
 *      void *aWorkspace          -- MCI Workspace
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_MCI_ProcessInterrupt(
    T_LPC17xx_40xx_MCI_Workspace *p)
{
    TUInt32 ms;
    TUInt8 xs;
    //TUInt32 n;
    //T_gpdmaRequest r;

    /* Clear MCI interrupt status */
    ms = MCI_STATUS & MCI_STATUS_ERRORS;
    MCI_CLEAR = ms;

    xs = p->iTransferStatus;

    if ((ms & MCI_STATUS_TX_UNDERRUN) == MCI_STATUS_TX_UNDERRUN) {
      p->iLastMCIError = ms;
      if (p->iErrorCallback) {
         p->iErrorCallback(p->iErrorCallbackWorkspace);
      }
    }
    if ((ms & MCI_STATUS_RX_OVERRUN) == MCI_STATUS_RX_OVERRUN) {
      p->iLastMCIError = ms;
      if (p->iErrorCallback) {
         p->iErrorCallback(p->iErrorCallbackWorkspace);
      }
    }
    
    if ((ms & MCI_STATUS_DATA_BLOCK_END) == MCI_STATUS_DATA_BLOCK_END) {
        /* A block transfer completed (DataBlockEnd) */
        if (xs & TRANSFER_STATUS_READING) {
            /* In card read operation */
            if ((ms & MCI_STATUS_DATA_END) == MCI_STATUS_DATA_END) {
                /* When last block is received (DataEnd), */
                /* Pop off remaining data in the MCIFIFO */
                (*(p->iGPDMA))->BurstRequest(p->iGPDMA,
                    GPDMA_PERIPHERAL_SD_MMC);
            }
            /* Track where the next position is at */
            p->iBufferPosIn++;

            if (p->iReceptionCompleteCallback)
                p->iReceptionCompleteCallback(
                    p->iReceptionCompleteCallbackWorkspace);
        } else {
            /* In card write operation */
            /* Next write buffer */
            p->iBufferPosOut++;
            /* Are we done yet? */
            if (p->iBufferPosOut >= p->iNumWriting) {
                xs |= TRANSFER_STATUS_DONE;
                // Notify that we are complete
                if (p->iTransmissionCompleteCallback)
                    p->iTransmissionCompleteCallback(
                        p->iTransmissionCompleteCallbackWorkspace);
            }
        }
    } else {
        if ((ms & MCI_STATUS_CMD_RESP_END) == MCI_STATUS_CMD_RESP_END) {
          
        } else {
			          
            /* An MCI error occured (not DataBlockEnd) */
            xs |= TRANSFER_STATUS_MCI_ERROR;

            // Record the error
            p->iLastMCIError = ms;
            // Keep these so that we don't have to wait on timeout even when there is an error.
            if (p->iTransferStatus & TRANSFER_STATUS_READING) {
                // Error during reading
                if (p->iReceptionCompleteCallback)
                    p->iReceptionCompleteCallback(
                        p->iReceptionCompleteCallbackWorkspace);
            } else {
                // Error in writing
                if (p->iTransmissionCompleteCallback)
                    p->iTransmissionCompleteCallback(
                        p->iTransmissionCompleteCallbackWorkspace);
            }
        }
    }

    p->iTransferStatus = xs;

}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this LCD Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_InitializeWorkspace(void *aWorkspace)
{
    TUInt32 n;
    TUInt32 dma_control;
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;
    p->iHAL = &LPC17xx_40xx_MCI_Interface;

    p->iCardDetect = GPIO_NONE;
    p->iWriteProtect = GPIO_NONE;
    p->iReceptionCompleteCallback = 0;
    p->iReceptionCompleteCallbackWorkspace = 0;
    p->iRCA = 0;
    p->iGPDMA = 0; // DMA channel prio is fixed. DMA0=highest prio, DMA7=lowest prio.
    p->iIRQActive = EFalse;

    // Setup the GPDMA channel for reads
    dma_control = /*GPDMA_INT_ENABLE | */GPDMA_DI | GPDMA_DBWIDTH_32
        | GPDMA_SBWIDTH_32 | GPDMA_SBSIZE_8 | GPDMA_DBSIZE_8
        | GPDMA_TRANSFER_SIZE(512 / 4);

    /* Create link list */
    // Each link in the link list to copy from MCI_FIFO into a block
    // of memory in G_dmaBlocks.  The links to each other allow the whole
    // list of G_dmaBlocks blocks allow the complete dma buffers to be called.
    for (n = 0; n < NUM_MAX_BLOCKS; n++) {
        // MCI_FIFO -> G_dmaBlocks[n]
        G_linkListRead[n][0] = (TUInt32)&MCI_FIFO;
        G_linkListRead[n][1] = (TUInt32)G_dmaBlocks[n];
        G_linkListRead[n][2] = ((n+1)>=NUM_MAX_BLOCKS)?0:(TUInt32)G_linkListRead[n + 1];
        G_linkListRead[n][3] = dma_control;
    }

    // Setup the GPDMA channel for writes
    dma_control = /*GPDMA_INT_ENABLE | */GPDMA_SI | GPDMA_DBWIDTH_32
        | GPDMA_SBWIDTH_32 | GPDMA_SBSIZE_8 | GPDMA_DBSIZE_8
        | GPDMA_TRANSFER_SIZE(512 / 4);

    /* Create link list */
    // Each link in the link list to copy from MCI_FIFO into a block
    // of memory in G_dmaBlocks.  The links to each other allow the whole
    // list of G_dmaBlocks blocks allow the complete dma buffers to be called.
    for (n = 0; n < NUM_MAX_BLOCKS; n++) {
        // G_dmaBlocks[n] -> MCI_FIFO
        G_linkListWrite[n][0] = (TUInt32)G_dmaBlocks[n];
        G_linkListWrite[n][1] = (TUInt32)&MCI_FIFO;
        G_linkListWrite[n][2] = ((n+1)>=NUM_MAX_BLOCKS)?0:(TUInt32)G_linkListWrite[n + 1];
        G_linkListWrite[n][3] = dma_control;
    }

    G_mciWorkspace = p;



    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_PowerOn
 *---------------------------------------------------------------------------*
 * Description:
 *      Have the MCI power on the target
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_PowerOn(void *aWorkspace)
{
    // Turn on the power (if not already on), wait 10 ms minimum,
    // and then enable the signals
    LPC_MCI->POWER = 0x01;
    writeDelay();
    UEZTaskDelay(10);
    LPC_MCI->POWER = 0x03;
    writeDelay();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_PowerOff
 *---------------------------------------------------------------------------*
 * Description:
 *      Have the MCI power off the target
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_PowerOff(void *aWorkspace)
{
    MCI_MASK0 = 0;
    MCI_COMMAND = 0;
    MCI_DATA_CTRL = 0;
    writeDelay();

    MCI_POWER = 0; /* Power-off */
    writeDelay();
    MCI_CLOCK = 0;
    writeDelay();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Reset the MCI Controller
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_Reset(void *aWorkspace)
{
    // Reset MCI Registers
    LPC_MCI->MASK0 = 0;
    LPC_MCI->COMMAND = 0;
    LPC_MCI->DATACTRL = 0;
    writeDelay();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_IsCardInserted
 *---------------------------------------------------------------------------*
 * Description:
 *      Check Card detect line of socket
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 * Outputs:
 *      TBool              -- ETrue if card present
 *---------------------------------------------------------------------------*/
TBool LPC17xx_40xx_MCI_IsCardInserted(void * aWorkspace)
{
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;    
    PARAM_NOT_USED(p);
#if 0
	if(p->iSdMmcPins.iCardDetect != GPIO_NONE) {
		TBool read = UEZGPIORead(p->iSdMmcPins.iCardDetect);
		UEZBSPDelayUS(20);
		if (read == UEZGPIORead(p->iSdMmcPins.iCardDetect)){
		  if(read == ETrue) {
			return ETrue; // card present both reads
		  }
		}
		// If we read 2 different values or card disconnected then card was removed at least momentarily.
		return EFalse;
	} else {
		return ETrue;
	}
#else // not implemented
    return ETrue;
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_SendCommand
 *---------------------------------------------------------------------------*
 * Description:
 *      Send the MCI Controller a standard command.  Commands are of the
 *      type MCI_CMDx or MCI_ACMDx.  Some commands require an argument.
 *      Depending on the command, a type of response (none, short, or long)
 *      may be returned in the passed aResponse.
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 *      TUInt16 aCommand        -- Command number, MCI_ACMD for ACMD's.
 *      TUInt32 aArgument       -- Argument for command
 *      T_uezMCIResponseType aResponseType -- Response type to get back
 *      TUInt32 *aResponse      -- Place to store the response.
 * Outputs:
 *      TBool                   -- ETrue if successful, else EFalse.
 *---------------------------------------------------------------------------*/
TBool LPC17xx_40xx_MCI_SendCommand(
    void *aWorkspace,
    TUInt16 aCommand,
    TUInt32 aArgument,
    T_uezMCIResponseType aResponseType,
    TUInt32 *aResponse)
{
    TUInt32 status, command;
    TUInt32 startTimeout;
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    /* Send a MCI_CMD55 prior to the specified command if it is MCI_ACMD class */
    if (aCommand & 0x80) {
        /* If MCI_CMD55 is fails, exit with error */
        if (!LPC17xx_40xx_MCI_SendCommand(aWorkspace, MCI_CMD55, (TUInt32)p->iRCA << 16UL,
            UEZ_MCI_RESPONSE_TYPE_SHORT, aResponse) || !(aResponse[0] & 0x00000020))
            return EFalse;
    }
    aCommand &= 0x3F; /* Mask out MCI_ACMD flag */

    do { /* Wait while CmdActive bit is set */
        MCI_COMMAND = 0; /* Cancel to transmit command */
        MCI_CLEAR = 0x0C5; /* Clear status flags */
        for (status = 0; status < 10; status++)
            MCI_STATUS; /* Skip lock out time of command reg. */
    } while (MCI_STATUS & 0x00800);

    MCI_ARGUMENT = aArgument; /* Set the argument into argument register */
    writeDelay();
    command = 0x400 | aCommand; /* Enable bit + aCommand */
    if (aResponseType == 1)
        command |= 0x040; /* Set Response bit to reveice short resp */
    if (aResponseType > 1)
        command |= 0x0C0; /* Set Response and LongResp bit to receive long resp */
    MCI_COMMAND = command; /* Initiate command transaction */

    startTimeout = UEZTickCounterGet();
    for (;;) { /* Wait for end of the cmd/resp transaction */
        if (UEZTickCounterGetDelta(startTimeout) > 100)
            return EFalse;
        status = MCI_STATUS; /* Get the transaction status */
        if (aResponseType == UEZ_MCI_RESPONSE_TYPE_NONE) {
            /* No response */
            if (status & 0x080) {
                /* CmdSent */
                /* No response, so stop here */
                return ETrue;
            }
        } else {
            /* Short or long response */
            if (status & MCI_STATUS_CMD_RESP_END) {
                break;
            }
            if (status & MCI_STATUS_CMD_CRC_FAIL) {
                if (aCommand == 1 || aCommand == 12 || aCommand == 41) {
                    /* Ignore resp CRC error on MCI_CMD1/12/41 */
                    break;
                }
                return EFalse;
            }
            if (status & 0x004) {
                /* CmdTimeOut */
                return EFalse;
            }
        }
    }

    /* Got a short or long response */
    /* Read the response words */
    aResponse[0] = MCI_RESP0;
    if (aResponseType == UEZ_MCI_RESPONSE_TYPE_RESPONSE_LONG) {
        aResponse[1] = MCI_RESP1;
        aResponse[2] = MCI_RESP2;
        aResponse[3] = MCI_RESP3;
    }

    return ETrue; /* Return with success */
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_ReadyReception
 *---------------------------------------------------------------------------*
 * Description:
 *      Prepare the MCI Controller for receiving blocks of data of the
 *      given size (usually 512 bytes).  When each block is received, call
 *      the given callback routine.
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 *      TUInt32 aNumBlocks      -- Number of blocks to receive
 *      TUInt32 aBlockSize      -- Block size.  Should be multiple of 4 and
 *                                  a power of 2.  Currently only 512 supported.
 *      T_mciReceptionComplete aReceptionCallback - Callback routine when
 *                                  received block.
 *      void *aReceptionCallbackWorkspace -- Workspace for callback.
 *      TUInt32 aNumBlocksReady -- Number of blocks readied equal or or less
 *                                  than aNumBlocks.  Can be zero.
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_ReadyReception(
    void *aWorkspace,
    TUInt32 aNumBlocks,
    TUInt32 aBlockSize,
    T_mciReceptionComplete aReceptionCallback,
    void *aReceptionCallbackWorkspace,
    TUInt32 *aNumBlocksReady) // TODO: Comment this everywhere
{
    T_gpdmaRequest r;
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    // Limit the number of blocks allowed to the length of our buffer
    if (aNumBlocks > NUM_MAX_BLOCKS)
        aNumBlocks = NUM_MAX_BLOCKS;
    *aNumBlocksReady = aNumBlocks;

    p->iReceptionCompleteCallback = aReceptionCallback;
    p->iReceptionCompleteCallbackWorkspace = aReceptionCallbackWorkspace;

    /* --------- Setting up block indexes ---------- */
    /* Block FIFO R/W index */
    p->iBufferPosOut = 0;
    p->iBufferPosIn = 0;
    p->iTransferStatus = TRANSFER_STATUS_READING;

    /* --------- Setting up DMA request ---------- */
    // Setup the first request
    r.iControl = /*GPDMA_INT_ENABLE | */GPDMA_DI | GPDMA_DBWIDTH_32
        | GPDMA_SBWIDTH_32 | GPDMA_SBSIZE_8 | GPDMA_DBSIZE_8;
    r.iSourcePeripheral = GPDMA_PERIPHERAL_SD_MMC;
    r.iSourceAddr = G_linkListRead[0][0];
    r.iDestinationPeripheral = GPDMA_PERIPHERAL_MEMORY;
    r.iDestinationAddr = G_linkListRead[0][1];
    r.iLinkedList = (void *)G_linkListRead[0][2];
    r.iNumTransferItems = GPDMA_TRANSFER_SIZE(aBlockSize / 4);
    //r.iFlowControl = GPDMA_FLOW_CONTROL_SOURCE_PERIPHERAL; // Will cause read failure!
    r.iFlowControl = GPDMA_FLOW_CONTROL_DMA; // Receiver should always be flow controller!

    (*p->iGPDMA)->PrepareRequest(p->iGPDMA, &r, 0, 0);

    (*p->iGPDMA)->Start(p->iGPDMA);

    /* --------- Setting up MCI ---------- */
    // Set data length
    MCI_DATA_LEN = 512 * aNumBlocks;
    // Data timer: 0.2sec
//    MCI_DATA_TMR = (TUInt32)(SDCARD_MCI_RATE_FOR_RW_STATE * 0.2);
    MCI_DATA_TMR = (TUInt32)(SDCARD_MCI_RATE_FOR_RW_STATE * 2.0); // TODO do we need to increase this timeout?
    // Clear status flags for errors
    MCI_CLEAR = MCI_STATUS_ERRORS;
    MCI_MASK0 = MCI_STATUS_ERRORS;
//    for (n = 0; aBlockSize > 1; aBlockSize >>= 1, n += 0x10)
//        ;
    /* Start to receive data blocks */
    MCI_DATA_CTRL = (9<<4) /*n*/ | 0xB;
    writeDelay();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_ReadyTransmission
 *---------------------------------------------------------------------------*
 * Description:
 *      Prepare for a number of write block transactions.
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 *      TUInt32 aNumBlocks      -- Number of total blocks to write
 *      TUInt32 aBlockSize      -- Size of each block (only 512 supported)
 *      T_mciReceptionComplete aTranmissionCallback -- Routine to call
 *                                  when complete from interrupt.
 *      void *aTransmissionCallbackWorkspace -- Workspace for callback.
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_ReadyTransmission(
    void *aWorkspace,
    TUInt32 aNumBlocks,
    TUInt32 aBlockSize,
    TUInt32 *aNumWriting)
{
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    // This implementation only supports 512 byte blocks!
    if (aBlockSize != 512)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Determine how many blocks we are going to write (based on the
    // number requested to send and how much room we got).
    *aNumWriting = aNumBlocks;
    if (*aNumWriting >= NUM_MAX_BLOCKS)
        *aNumWriting = NUM_MAX_BLOCKS-1;
    p->iNumWriting = *aNumWriting;

    // Setup indexes
    p->iBufferPosOut = 0;
    p->iBufferPosIn = 0;
    p->iTransferStatus = TRANSFER_STATUS_WRITING;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_StartTransmission
 *---------------------------------------------------------------------------*
 * Description:
 *      Start writing blocks as they are written
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 *      TUInt32 aNumBlocks      -- Number of total blocks to write
 *      TUInt32 aBlockSize      -- Size of each block (only 512 supported)
 *      T_mciReceptionComplete aTranmissionCallback -- Routine to call
 *                                  when complete from interrupt.
 *      void *aTransmissionCallbackWorkspace -- Workspace for callback.
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_StartTransmission(
    void *aWorkspace,
    T_mciTransmissionComplete aTransmissionCallback,
    void *aTransmissionCallbackWorkspace)
{
    T_gpdmaRequest r;
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    // Set the callback routine (if any)
    p->iTransmissionCompleteCallback = aTransmissionCallback;
    p->iTransmissionCompleteCallbackWorkspace = aTransmissionCallbackWorkspace;

    // Load first LLI
    r.iSourcePeripheral = GPDMA_PERIPHERAL_MEMORY;
    r.iSourceAddr = G_linkListWrite[0][0];
    r.iDestinationPeripheral = GPDMA_PERIPHERAL_SD_MMC;
    r.iDestinationAddr = G_linkListWrite[0][1];
    r.iLinkedList = (void *)G_linkListWrite[0][2];
    r.iNumTransferItems = GPDMA_TRANSFER_SIZE(512 / 4);
    r.iFlowControl = GPDMA_FLOW_CONTROL_DESTINATION_PERIPHERAL; // Receiver should always be flow controller!
    r.iControl = /*GPDMA_INT_ENABLE | */GPDMA_SI | GPDMA_DBWIDTH_32
        | GPDMA_SBWIDTH_32 | GPDMA_SBSIZE_8 | GPDMA_DBSIZE_8;

    /* Transfer status: Memory --> MCI*/
    (*p->iGPDMA)->PrepareRequest(p->iGPDMA, &r, 0, 0);
    (*p->iGPDMA)->Start(p->iGPDMA);

    // Set total data length
    MCI_DATA_LEN = 512 * (p->iNumWriting+1);
    // Data timer: 0.2sec
    MCI_DATA_TMR = (TUInt32)(SDCARD_MCI_RATE_FOR_RW_STATE * 3.0); // TODO verify the new timeout, but we need to support multiple brands of cards
    // Clear status flags
    MCI_CLEAR = 0x51A;
    // DataBlockEnd StartBitErr DataEnd RxOverrun DataTimeOut DataCrcFail
    MCI_MASK0 = 0x51A;

    // Start to transmit data blocks (enable for DMA)
    MCI_DATA_CTRL = (9<<4) | 0x9;
    writeDelay();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_StopTransfer
 *---------------------------------------------------------------------------*
 * Description:
 *      Halt any transfers possibly in progress regardless of state.
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_StopTransfer(void *aWorkspace)
{
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    /* Disable MCI interrupt */
    MCI_MASK0 = 0;

    /* Stop MCI data transfer */
    MCI_DATA_CTRL = 0;
    writeDelay();

    /* Disable DMA ch-0 */
    (*p->iGPDMA)->Stop(p->iGPDMA);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read a block waiting to be received.  Currently only supports
 *      512 byte blocks.
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 *      void *aBuffer           -- Buffer to receive 512 byte block
 *      TUInt32 aReadSize       -- Size of block to read
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_Read(void *aWorkspace, void *aBuffer, TUInt32 aReadSize)
{
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    // This implementation only supports 512 byte blocks!
    if (aReadSize != 512)
        return UEZ_ERROR_INVALID_PARAMETER;

    // If an error has occurred, report this
    if (p->iTransferStatus & TRANSFER_STATUS_ANY_ERROR) {
        return UEZ_ERROR_READ_WRITE_ERROR;
    }

    // If there is no data (yet), report this
    if (p->iBufferPosOut == p->iBufferPosIn)
        return UEZ_ERROR_OUT_OF_DATA;
    
    if (( p->iLastMCIError & MCI_STATUS_TX_UNDERRUN) == MCI_STATUS_TX_UNDERRUN) {
        //SEGGER_RTT_WriteString(0, "RXUNDER!\n");
        //return UEZ_ERROR_READ_WRITE_ERROR; // Note that if we report error here it is for the last read not the current read!
    }

    // Copy over a block of data
    Copy_al2un(aBuffer, G_dmaBlocks[p->iBufferPosOut], aReadSize);

    /* Next buffer block */
    p->iBufferPosOut++;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write a block of data to the MCI controller.
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 *      void *aBuffer           -- Data to write to controller
 *      TUInt32 aNumBytes       -- Number of bytes to write (usually 512)
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_Write(
    void *aWorkspace,
    const TUInt8 *aBuffer,
    TUInt32 aNumBytes)
{
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    // This implementation only supports 512 byte blocks!
    if (aNumBytes != 512)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Is there room?
    if (p->iBufferPosIn >= p->iNumWriting)
        return UEZ_ERROR_OUT_OF_SPACE;

    // If an error has occurred, report this
    if (p->iTransferStatus & TRANSFER_STATUS_ANY_ERROR) {
        return UEZ_ERROR_READ_WRITE_ERROR;
    }
    
    if (( p->iLastMCIError & MCI_STATUS_TX_UNDERRUN) == MCI_STATUS_TX_UNDERRUN) {
        //SEGGER_RTT_WriteString(0, "TXUNDER!\n");
        //return UEZ_ERROR_READ_WRITE_ERROR; // Note that this is the last error, so we need to handle it somewhere else
    }

    // There is a block, write it
    Copy_al2un(G_dmaBlocks[p->iBufferPosIn], aBuffer, 512);

    /* Next DMA buffer */
    p->iBufferPosIn++;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_SetRelativeCardAddress
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the active RCA (Relative Card Address).
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 *      TUInt32 aRCA            -- Relative Card Address (RCA)
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_SetRelativeCardAddress(void *aWorkspace, TUInt32 aRCA)
{
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;
    p->iRCA = aRCA;
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_SetClockRate
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the clocking rate of the MCI bus clock in Hz.  If power save
 *      mode is set, the clock is only run when communications is active,
 *      otherwise it is run continuously.
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 *      TUInt32 aHz             -- Clock rate in Hz.
 *      TBool aPowerSave        -- ETrue to save power by running the clock
 *                                  only when active, else EFalse to run
 *                                  clock continuously.
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_SetClockRate(
    void *aWorkspace,
    TUInt32 aHz,
    TBool aPowerSave)
{
    TUInt32 clock;
    TUInt32 div;
    PARAM_NOT_USED(aWorkspace);

    // Power save means the clock ONLY runs when needed, otherwise the SD CLK
    // runs continously.
    if (aHz == 0) {
        LPC_MCI->CLOCK &= ~0x100;
        writeDelay();
    } else {
        clock = PCLK_FREQUENCY/2;
        div = 0;
        while ((clock / (div+1)) > aHz)
            div++;

          LPC_MCI->CLOCK = (LPC_MCI->CLOCK & 0x00000C00) | (1<<8)
              | (div)
              | (aPowerSave ? (1<<9) : 0);
          writeDelay();
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_SetDataBus
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the type of MCI bus active.  MCI device will initialize to
 *      1 bit bus.  Used to go into 4 bit parallel mode.
 * Inputs:
 *      void *aWorkspace        -- MCI Workspace
 *      T_uezMCIBus aBus        -- Type of bus (1-bit or 4-bit)
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_SetDataBus(void *aWorkspace, T_uezMCIBus aBus)
{
    switch (aBus) {
        case UEZ_MCI_BUS_4BIT_WIDE:
            LPC_MCI->CLOCK |= 0x800;
            writeDelay();
            break;
        case UEZ_MCI_BUS_1BIT_WIDE:
            LPC_MCI->CLOCK &= ~0x800;
            writeDelay();
            break;
        default:
            return UEZ_ERROR_INVALID_PARAMETER;
    }
    return UEZ_ERROR_NONE;
}

TBool LPC17xx_40xx_MCI_IsWriteAvailable(void *aWorkspace)
{
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    // Determine if there is room available.  If there is room, then ETrue
    if (p->iBufferPosIn < p->iNumWriting)
        return ETrue;

    return EFalse;
}

TBool LPC17xx_40xx_MCI_IsWriteEmpty(void *aWorkspace)
{
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    // Have all the blocks been written?  It not, then EFalse.
    if (p->iBufferPosOut < p->iNumWriting)
        return EFalse;

    return ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC17xx_40xx_MCI_SetErrorCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Start error callback
 * Inputs:
 *      void *aWorkspace          -- MCI Workspace
 *      T_mciError aErrorCallback -- Routine to call when error encountered.
 *      void *aErrorCallbackWorkspace -- Workspace for callback.
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC17xx_40xx_MCI_SetErrorCallback(
    void *aWorkspace,
    T_mciError aErrorCallback,
    void *aErrorCallbackWorkspace)
{
    T_LPC17xx_40xx_MCI_Workspace *p = (T_LPC17xx_40xx_MCI_Workspace *)aWorkspace;

    // Set the callback routine (if any)
    p->iErrorCallback = aErrorCallback;
    p->iErrorCallbackWorkspace = aErrorCallbackWorkspace;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const HAL_MCI LPC17xx_40xx_MCI_Interface = { {
    "LPC17xx_40xx:MCI",
    0x0204,
    LPC17xx_40xx_MCI_InitializeWorkspace,
    sizeof(T_LPC17xx_40xx_MCI_Workspace), },

// v2.04
    LPC17xx_40xx_MCI_PowerOn,
    LPC17xx_40xx_MCI_PowerOff,
    LPC17xx_40xx_MCI_Reset,
    LPC17xx_40xx_MCI_SendCommand,
    LPC17xx_40xx_MCI_ReadyReception,
    LPC17xx_40xx_MCI_StopTransfer,
    LPC17xx_40xx_MCI_Read,
    LPC17xx_40xx_MCI_SetRelativeCardAddress,
    LPC17xx_40xx_MCI_SetClockRate,
    LPC17xx_40xx_MCI_SetDataBus,
    LPC17xx_40xx_MCI_ReadyTransmission,
    LPC17xx_40xx_MCI_StartTransmission,
    LPC17xx_40xx_MCI_Write,
    LPC17xx_40xx_MCI_IsWriteAvailable,
    LPC17xx_40xx_MCI_IsWriteEmpty,

    // uEZ 2.11.1
    LPC17xx_40xx_MCI_SetErrorCallback,
    // uEZ 2.14
    LPC17xx_40xx_MCI_IsCardInserted
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC17xx_40xx_MCI_Require(const T_LPC17xx_40xx_MCI_Pins *aPins, const char *aGPDMA)
{
    T_halWorkspace *p_mci;
    T_LPC17xx_40xx_MCI_Workspace *p;

    static const T_LPC17xx_40xx_IOCON_ConfigList pinsDAT0[] = {
            {GPIO_P0_22, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
#if (UEZ_PROCESSOR == NXP_LPC1788)
            {GPIO_P1_6,  IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
#elif (UEZ_PROCESSOR == NXP_LPC4088)
            {GPIO_P1_6,  IOCON_W(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_DIGITAL, IOCON_FILTER_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
#endif
    };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsDAT1[] = {
#if (UEZ_PROCESSOR == NXP_LPC1788)
            {GPIO_P1_7,  IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
#elif (UEZ_PROCESSOR == NXP_LPC4088)
            {GPIO_P1_7,  IOCON_W(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_DIGITAL, IOCON_FILTER_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
#endif
            {GPIO_P2_11, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
    };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsDAT2[] = {
            {GPIO_P1_11, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
            {GPIO_P2_12, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
    };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsDAT3[] = {
            {GPIO_P1_12, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
            {GPIO_P2_13, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
    };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsCLK[] = {
            {GPIO_P0_19, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
            {GPIO_P1_2,  IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
    };
    static const T_LPC17xx_40xx_IOCON_ConfigList pinsCMD[] = {
            {GPIO_P0_20, IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
            {GPIO_P1_3,  IOCON_D(2, IOCON_NO_PULL, IOCON_HYS_ENABLE, IOCON_INVERT_OFF, IOCON_SLEW_FAST, IOCON_PUSH_PULL)},
    };

    HAL_DEVICE_REQUIRE_ONCE();

    LPC17xx_40xx_GPIO0_Require();
    LPC17xx_40xx_GPIO1_Require();
    LPC17xx_40xx_GPIO2_Require();

    // Register MCI Controller
    HALInterfaceRegister("MCI",
            (T_halInterface *)&LPC17xx_40xx_MCI_Interface, 0, &p_mci);
    p = (T_LPC17xx_40xx_MCI_Workspace *)p_mci;

    // Turn on the MCI Controller
    LPC17xx_40xxPowerOn(1<<28);

    // Lock GPIO pins, if any
    p->iCardDetect = aPins->iCardDetect;
    p->iWriteProtect = aPins->iWriteProtect;
    if (p->iCardDetect != GPIO_NONE)
        UEZGPIOLock(p->iCardDetect);
    if (p->iWriteProtect != GPIO_NONE)
        UEZGPIOLock(p->iWriteProtect);

    LPC17xx_40xx_IOCON_ConfigPin(aPins->iDAT0, pinsDAT0, ARRAY_COUNT(pinsDAT0));
    LPC17xx_40xx_IOCON_ConfigPin(aPins->iDAT1, pinsDAT1, ARRAY_COUNT(pinsDAT1));
    LPC17xx_40xx_IOCON_ConfigPin(aPins->iDAT2, pinsDAT2, ARRAY_COUNT(pinsDAT2));
    LPC17xx_40xx_IOCON_ConfigPin(aPins->iDAT3, pinsDAT3, ARRAY_COUNT(pinsDAT3));
    LPC17xx_40xx_IOCON_ConfigPin(aPins->iCLK, pinsCLK, ARRAY_COUNT(pinsCLK));
    LPC17xx_40xx_IOCON_ConfigPin(aPins->iCMD, pinsCMD, ARRAY_COUNT(pinsCMD));

    // Reset MCI Registers
    LPC_MCI->MASK0 = 0;
    LPC_MCI->COMMAND = 0;
    LPC_MCI->DATACTRL = 0;
    writeDelay();

    // Setup interrupt for the MCI
    InterruptRegister(MCI_IRQn, LPC17xx_40xx_MCI_Interrupt,
        INTERRUPT_PRIORITY_HIGHEST, "MCI");
    InterruptEnable(MCI_IRQn);



    // Find the GPDMA HAL driver
    HALInterfaceFind(aGPDMA, (T_halWorkspace **)&p->iGPDMA);

    if ((((TUInt32)&G_dmaBlocks) & 0xFFF00000) != 0x20000000) {
        // The G_dmaBlocks and G_linkList must be in AHB0 or AHB1 memory
        // at about 0x20000000 - 0x2007FFFF.  If it didn't get put there,
        // the code was not compiled correctly!
        UEZFailureMsg("MCI memory space error!");
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_MCI.c
 *-------------------------------------------------------------------------*/
