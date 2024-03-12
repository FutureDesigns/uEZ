/*-------------------------------------------------------------------------*
 * File:  SSP.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC43xx SSP Bus Interface.
 * Implementation:
 *      Two SSPs are created, SSP0 and SSP1 and SSP2.
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
#include "Config.h"
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <HAL/GPIO.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_SSP.h>
#include "LPC43xx_UtilityFuncs.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iCR0; // 0x00
#define SSP_FRF_SPI (0<<4)      // Frame format: SPI
#define SSP_FRF_TI  (1<<4)      // Frame format: TI
#define SSP_FRF_MW  (2<<4)      // Frame format: Microwire
#define SSP_CPOL    (1<<6)      // Clock Polarity
#define SSP_CPHA    (1<<7)      // Clock Phase
    TVUInt32 iCR1; // 0x04
#define SSP_LBM     (1<<0)      // Loop back mode
#define SSP_SSE     (1<<1)      // SSP Enable
#define SSP_MS      (1<<2)      // Master/Slave Mode (master=0)
#define SSP_SOD     (1<<3)      // Slave Output Disable
    TVUInt32 iDR; // 0x08
    TVUInt32 iSR; // 0x0C -- read only
#define SSP_TFE     (1<<0)      // Transmit FIFO Empty
#define SSP_TNF     (1<<1)      // Transmit FIFO Not Full
#define SSP_RNE     (1<<2)      // Receive FIFO Not Empty
#define SSP_RFF     (1<<3)      // Receive FIFO Full
#define SSP_BSY     (1<<4)      // Busy
    TVUInt32 iCPSR; // 0x10
    TVUInt32 iIMSC; // 0x14
#define SSP_RORIM   (1<<0)      // Receive overrun interrupt
#define SSP_RTIM    (1<<1)      // Receive timeout
#define SSP_RXIM    (1<<2)      // Rx FIFO is at least half full
#define SSP_TXIM    (1<<3)      // Tx FIFO is at least half empty
    TVUInt32 iRIS; // 0x18
#define SSP_RORRIS  (1<<0)      // Raw Receive overrun interrupt
#define SSP_RTRIS   (1<<1)      // Raw Receive timeout
#define SSP_RXRIS   (1<<2)      // Raw Rx FIFO is at least half full
#define SSP_TXRIS   (1<<3)      // Raw Tx FIFO is at least half empty
    TVUInt32 iMIS; // 0x1C
#define SSP_RORMIS  (1<<0)      // Mask (1=on) Receive overrun interrupt
#define SSP_RTMIS   (1<<1)      // Mask (1=on) Receive timeout
#define SSP_RXMIS   (1<<2)      // Mask (1=on) Rx FIFO is at least half full
#define SSP_TXMIS   (1<<3)      // Mask (1=on) Tx FIFO is at least half empty
    TVUInt32 iICR; // 0x20
#define SSP_RORIC   (1<<0)      // Clears SSP_RORIM interrupt
#define SSP_RTIC    (1<<1)      // Clears SSP_RTRIS interrupt
    TVUInt32 iDMACR;// 0x24
#define SSP_RXDMAE  (1<<0)      // When 1, DMA enabled for Rx FIFO
#define SSP_TXDMAE  (1<<1)      // When 1, DMA enabled for Tx FIFO
} T_LPC43xx_SSP_Registers;

typedef struct {
    const HAL_SPI *iHAL;
    T_LPC43xx_SSP_Registers *iReg;
    T_spiCompleteCallback iCompleteCallback;
    void *iCompleteCallbackWorkspace;
    SPI_Request *iRequest;
    T_irqChannel iIRQChannel;
    TBool iIsBusy;
} T_LPC43xx_SSP_Workspace;

/*---------------------------------------------------------------------------*
 * Function Prototypes:
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE( ISSP0IRQ );
IRQ_ROUTINE( ISSP1IRQ );
IRQ_ROUTINE( ISSP2IRQ );
static void ISSPEnd(SPI_Request *aRequest);
static void ISSPProcessInterrupt(T_LPC43xx_SSP_Workspace *aW);
static void ISSPOutput(T_LPC43xx_SSP_Workspace *aW);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LPC43xx_SSP_Workspace *G_SSP0Workspace;
static T_LPC43xx_SSP_Workspace *G_SSP1Workspace;

/*---------------------------------------------------------------------------*
 * Routine: ISSPxIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the SSPx interrupts and forward to ISSPProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE( ISSP0IRQ )
{
    IRQ_START();
    ISSPProcessInterrupt(G_SSP0Workspace);
    IRQ_END();
}

IRQ_ROUTINE( ISSP1IRQ )
{
    IRQ_START();
    ISSPProcessInterrupt(G_SSP1Workspace);
    IRQ_END();
}


/*---------------------------------------------------------------------------*
 * Routine: ISSPProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Generic routine for handling any of the SSP interrupts.
 *      NOTE: This routine is ALWAYS processed inside of a interrupt
 *              service routine.
 * Inputs:
 *      T_SerialPortEntry *p      -- Port's definition to process interrupt
 *---------------------------------------------------------------------------*/
static void ISSPProcessInterrupt(T_LPC43xx_SSP_Workspace *aW)
{
    T_LPC43xx_SSP_Registers *p = aW->iReg;
    SPI_Request *r = aW->iRequest;
    TUInt8 ris;
    TUInt32 data;

    while (1) {
        // Do we have any interrupts to process now?
        ris = p->iMIS;
        if (!ris)
            break;

        // Is data waiting to be received?
        if (ris & (SSP_RTMIS | SSP_RXMIS)) {
            // Pull the data out of the SSP peripheral
            if (r->iNumTransferredIn < r->iNumTransfers) {
                data = p->iDR;
                if (r->iDataMISO)
                    ((TUInt8 *)r->iDataMISO)[r->iNumTransferredIn++] = data;
            }

            // If we have reached the end, we are done
            if (r->iNumTransferredIn >= r->iNumTransfers) {
                // No more processing.  Stop here.
                InterruptDisable(aW->iIRQChannel);

                // Clear any remaining interrupts
                p->iICR = SSP_RORIC|SSP_RTIC;

                // Mask out all interrupts
                p->iIMSC &= 0;

                // End the chip select
                ISSPEnd(aW->iRequest);

                // Note we are done
                aW->iIsBusy = EFalse;

                // Call the completion callback (if any)
                if (aW->iCompleteCallback) {
                    aW->iCompleteCallback(aW->iCompleteCallbackWorkspace,
                        aW->iRequest);
                }

                break;
            } else {
                // Not done yet.
                // Ensure if we had an interrupt on a stalled receive, clear it
                p->iICR = SSP_RORIC;
            }
        } else if (ris & (SSP_TXRIS)) {
            ISSPOutput(aW);
        } else {
            // Some other interrupt.  Just clear it.  Possibly an error.
            p->iICR = ris;
        }
    }

}

/*---------------------------------------------------------------------------*
 * Routine:  ISSPStart
 *---------------------------------------------------------------------------*
 * Description:
 *      SSP is going to start -- chip select enabled for device
 * Outputs:
 *      SPI_Request *aRequest      -- Configuration of SSP device
 *---------------------------------------------------------------------------*/
static void ISSPStart(SPI_Request *aRequest)
{
    if(aRequest->iCSGPIOPort == NULL){
        //GPIO not implemented, assume controlled by app.
        return;
    }
    if (aRequest->iCSPolarity) {
        (*aRequest->iCSGPIOPort)->Set(aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    } else {
        (*aRequest->iCSGPIOPort)->Clear(aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ISSPEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      SSP is going to end -- chip select disabled for device
 * Outputs:
 *      SPI_Request *aRequest      -- Configuration of SSP device
 *---------------------------------------------------------------------------*/
static void ISSPEnd(SPI_Request *aRequest)
{
    if(aRequest->iCSGPIOPort == NULL){
        //GPIO not implemented, assume controlled by app.
        return;
    }
    if (aRequest->iCSPolarity) {
        (*(aRequest->iCSGPIOPort))->Clear(aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    } else {
        (*(aRequest->iCSGPIOPort))->Set(aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ISSPConfig
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the SSP for the particular SSP request.
 * Outputs:
 *      T_LPC43xx_SSP_Workspace *aW -- Particular SSP workspace
 *      SPI_Request *aRequest      -- Configuration of SSP device
 *---------------------------------------------------------------------------*/
static void ISSPConfig(T_LPC43xx_SSP_Workspace *aW, SPI_Request *aRequest)
{
    T_LPC43xx_SSP_Registers *p = aW->iReg;
    TUInt32 scr;

    // Make sure the SSP clock is PCLK/2
    p->iCPSR = 2;
    // Calculate the scr prescalar
    scr = ((Fpclk/1000)/(p->iCPSR * aRequest->iRate));
    // If too fast, run it down one more divider (this happenes when not
    // exactly on the right divider)
    while (((Fpclk/1000)/(p->iCPSR * scr)) > aRequest->iRate)
        scr++;
    // Final value of scr is one less
    scr--;
    

    p->iCR0 =
    // Number of bits per transaction
            (aRequest->iBitsPerTransfer - 1) |
            // SPI interface only
                    SSP_FRF_SPI |
            // Clock out polarity
                    (aRequest->iClockOutPolarity ? SSP_CPOL : 0)
                    | (aRequest->iClockOutPhase ? SSP_CPHA : 0) | (scr << 8);

    // Enable the SSP
    p->iCR1 = SSP_SSE;

    // Clear out the SSP buffer
    // First wait for any undone transfers
    while (p->iSR & SSP_BSY) {
    }

    // Now read any waiting data
    while (p->iSR & SSP_RNE)
        p->iDR;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SSP_TransferPolled
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a SSP transfer (both read and write) out the SSP port using
 *      the given request. CS should be set as a GPIO output before calling
 *      this functions.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 *      SPI_Request *aRequest       -- Request to perform
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_SSP_TransferPolled(void *aWorkspace, SPI_Request *aRequest)
{
    TUInt32 i;
    T_LPC43xx_SSP_Workspace *aW = (T_LPC43xx_SSP_Workspace *)aWorkspace;
    T_LPC43xx_SSP_Registers *p = aW->iReg;
    TUInt8 *p_dataMOSI8;
    TUInt8 *p_dataMISO8;
    TUInt16 *p_dataMOSI16;
    TUInt16 *p_dataMISO16;
    TVUInt32 dummy = 0;
    int32_t isToggling = aRequest->iFlags & SPI_REQUEST_TOGGLING_CS;
    
    // Do this to get rid of warning in compiler that dummy is set but not used
    VARIABLE_NOT_USED(dummy);
    
    // No interrupt processing
    InterruptDisable(aW->iIRQChannel);

    // Setup SSP for this configuration
    ISSPConfig(aW, aRequest);

    // Start SSP
    ISSPStart(aRequest);

    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer == 1) {
        // Optimized version for 1 byte
        // Wait for SSP to be no longer busy and transmit to be empty
        while ((p->iSR & SSP_BSY) || (!(p->iSR & SSP_TFE))) {
            // TBD: Polling here instead of interrupt
        }
        p->iDR = (TUInt32)((aRequest->iDataMOSI)?*((TUInt8 *)aRequest->iDataMOSI):0xFF);
        // Wait until SSP done  (transmit empty)
        while ((p->iSR & SSP_BSY) || ((p->iSR & SSP_TFE) == 0)) {
            // TBD: For polling this is fine, but we may need to yield at some point?
        }
        dummy = p->iDR;
        if (aRequest->iDataMISO)
            *((TUInt8 *)aRequest->iDataMISO) = dummy;
    } else if (aRequest->iBitsPerTransfer <= 8) {
        // Doing bytes
        p_dataMOSI8 = (TUInt8 *)(aRequest->iDataMOSI);
        p_dataMISO8 = (TUInt8 *)(aRequest->iDataMISO);

        for (i = 0; i < aRequest->iNumTransfers; i++) {
            // Wait for SSP to be no longer busy and transmit to be empty
            while ((p->iSR & SSP_BSY) || (!(p->iSR & SSP_TFE))) {
                // TBD: Polling here instead of interrupt
            }

            if (isToggling) {
                ISSPStart(aRequest);
            }
            if (p_dataMOSI8) {
                // Send the SSP data (this also triggers a read in)
                if (((i + 4) <= aRequest->iNumTransfers) && (!isToggling)) {
                    // Do 4 at one time
                    p->iDR = (TUInt32)*(p_dataMOSI8++);
                    p->iDR = (TUInt32)*(p_dataMOSI8++);
                    p->iDR = (TUInt32)*(p_dataMOSI8++);
                    p->iDR = (TUInt32)*(p_dataMOSI8++);
                } else {
                    p->iDR = (TUInt32)*(p_dataMOSI8++);
                }
            } else {
                // Send the SSP data (this also triggers a read in)
                if (((i + 4) <= aRequest->iNumTransfers) && (!isToggling)) {
                    // Do 4 at one time
                    p->iDR = (TUInt32)0xFF;
                    p->iDR = (TUInt32)0xFF;
                    p->iDR = (TUInt32)0xFF;
                    p->iDR = (TUInt32)0xFF;
                } else {
                    p->iDR = (TUInt32)0xFF;
                }
            }

            // Wait until SSP done  (transmit empty)
            while ((p->iSR & SSP_BSY) || ((p->iSR & SSP_TFE) == 0)) {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Store the SSP received over the data sent
            if (p_dataMISO8) {
                // Store the SSP received over the data sent
                if (((i + 4) <= aRequest->iNumTransfers) && (!isToggling)) {
                    // Do 4 at one time
                    *(p_dataMISO8++) = p->iDR;
                    *(p_dataMISO8++) = p->iDR;
                    *(p_dataMISO8++) = p->iDR;
                    *(p_dataMISO8++) = p->iDR;
                    i += 3;
                } else {
                    *(p_dataMISO8++) = p->iDR;
                }
            } else {
                // Store the SSP received over the data sent
                if (((i + 4) <= aRequest->iNumTransfers) && (!isToggling)) {
                    // Do 4 at one time
                    dummy = p->iDR;
                    dummy = p->iDR;
                    dummy = p->iDR;
                    dummy = p->iDR;
                    i += 3;
                } else {
                    dummy = p->iDR;
                }
            }
            if (isToggling) {
                ISSPEnd(aRequest);
            }
        }
    } else {
        // Doing words
        p_dataMOSI16 = (TUInt16 *)(aRequest->iDataMOSI);
        p_dataMISO16 = (TUInt16 *)(aRequest->iDataMISO);

        for (i = 0; i < aRequest->iNumTransfers; i++) {
            // Wait for SSP to be no longer busy and transmit to be empty
            while ((p->iSR & SSP_BSY) || (!(p->iSR & SSP_TFE))) {
                // TBD: Polling here instead of interrupt
            }

            // Send the SSP data (this also triggers a read in)
            if (p_dataMOSI16)
                p->iDR = (TUInt32)*(p_dataMOSI16++);
            else
                p->iDR = 0xFFFF;

            // Wait until SSP done
            while ((p->iSR & SSP_BSY) || ((p->iSR & SSP_TFE) == 0)) {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Store the SSP received over the data sent
            if (p_dataMISO16)
                *(p_dataMISO16++) = p->iDR;
            else
                dummy = p->iDR;
        }
    }

    // End SSP
    ISSPEnd(aRequest);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SSP_SSP0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx SSP0 workspace.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_SSP_SSP0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_SSP_Workspace *p = (T_LPC43xx_SSP_Workspace *)aWorkspace;
    p->iReg = (T_LPC43xx_SSP_Registers *)LPC_SSP0_BASE;

    G_SSP0Workspace = p;
    p->iIsBusy = EFalse;

#ifdef CORE_M4
    p->iIRQChannel = SSP0_IRQn;
#endif
#ifdef CORE_M0
    p->iIRQChannel = M0_SSP0_OR_SSP1_IRQn;
#endif
#ifdef CORE_M0SUB
    p->iIRQChannel = M0S_SSP0_OR_SSP1_IRQn;
#endif

    //Turn on peripheral clock
    LPC_CGU->BASE_SPI_CLK = (9<<24) | (1<<11);
    LPC_CCU1->CLK_SPI_CFG = 0x03;
    LPC_CGU->BASE_SSP0_CLK = (9<<24) | (1<<11);
    LPC_CCU1->CLK_M4_SSP1_CFG = 0x03;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SSP_SSP1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC43xx SSP1 workspace.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_SSP_SSP1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_SSP_Workspace *p = (T_LPC43xx_SSP_Workspace *)aWorkspace;
    p->iReg = (T_LPC43xx_SSP_Registers *)LPC_SSP1_BASE;

    G_SSP1Workspace = p;
    p->iIsBusy = EFalse;

#ifdef CORE_M4
    p->iIRQChannel = SSP1_IRQn;
#endif
#ifdef CORE_M0
    p->iIRQChannel = M0_SSP0_OR_SSP1_IRQn;
#endif
#ifdef CORE_M0SUB
    p->iIRQChannel = M0S_SSP0_OR_SSP1_IRQn;
#endif

    //Turn on peripheral clock
    LPC_CGU->BASE_SPI_CLK = (9<<24) | (1<<11);
    LPC_CCU1->CLK_SPI_CFG = 0x03;
    LPC_CGU->BASE_SSP1_CLK = (9<<24) | (1<<11);
    LPC_CCU2->CLK_APB2_SSP1_CFG = 0x03;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SSP_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Start a SSP transfer by putting the chip select in the active state
 *      after configuring the SSP speed and other settings.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 *      SPI_Request *aRequest       -- SSP request being processed
 *---------------------------------------------------------------------------*/
void LPC43xx_SSP_Start(void *aWorkspace, SPI_Request *aRequest)
{
    // Setup SSP for this configuration
    ISSPConfig(aWorkspace, aRequest);
    ISSPStart(aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SSP_TransferInOut
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a single SSP transfer to the SSP device.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 *      SPI_Request *aRequest       -- SSP request being processed
 *      TUInt32 aSend               -- Item to transfer
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static TUInt32 LPC43xx_SSP_TransferInOut(
        void *aWorkspace,
        SPI_Request *aRequest,
        TUInt32 aSend)
{
    TUInt32 recv;
    T_LPC43xx_SSP_Workspace *aW = (T_LPC43xx_SSP_Workspace *)aWorkspace;
    T_LPC43xx_SSP_Registers *p = aW->iReg;

    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer <= 8) {
        // Send the SSP data (this also triggers a read in)
        p->iDR = aSend;

        // Wait until SSP done
        while (((p->iSR & SSP_TFE) == 0) || (p->iSR & SSP_BSY)) {
            // TBD: For polling this is fine, but we may need to yield at some point?
        }

        // Store the SSP received over the data sent
        recv = p->iDR;
    } else {
        // Send the SSP data (this also triggers a read in)
        p->iDR = (TUInt32)*((TUInt16 *)aSend);

        // Wait until SSP done
        while ((p->iSR & SSP_RNE) == 0) {
            // TBD: For polling this is fine, but we may need to yield at some point?
        }

        // Store the SSP received over the data sent
        recv = p->iDR;
    }

    return recv;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SSP_Stop
 *---------------------------------------------------------------------------*
 * Description:
 *      End a SSP transfer by putting the chip select in the done state.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 *      SPI_Request *aRequest       -- SSP request being processed
 *---------------------------------------------------------------------------*/
static void LPC43xx_SSP_Stop(void *aWorkspace, SPI_Request *aRequest)
{
    ISSPEnd(aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SSP_SSP1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Transfer a set of SSP bytes (in the request) to the given
 *      target.  Works similarly to TransferPolled except this routine
 *      does not affect the chip select state.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 *      SPI_Request *aRequest       -- SSP request being processed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError LPC43xx_SSP_TransferInOutBytes(
        void *aWorkspace,
        SPI_Request *aRequest)
{
    T_LPC43xx_SSP_Workspace *aW = (T_LPC43xx_SSP_Workspace *)aWorkspace;
    T_LPC43xx_SSP_Registers *p = aW->iReg;
    TUInt32 sent;
    TUInt32 count;
    TUInt8 *p_dataMOSI = (TUInt8 *)aRequest->iDataMOSI;
    TUInt8 *p_dataMISO = (TUInt8 *)aRequest->iDataMISO;
    TUInt8 dummy[8];

    // This routine only works with transfer of 8 bits or lower
    if (aRequest->iBitsPerTransfer > 8)
        return UEZ_ERROR_INVALID_PARAMETER;

    sent = 0;
    count = aRequest->iNumTransfers;

    // Send data while there is data to send
    while (sent < count) {
        if (!aRequest->iDataMISO)
            p_dataMISO = dummy;

        // Always send in groups of 8 bytes if there are 8 bytes to send
        if ((sent + 8) <= count) {
            // Send 8
            if (!aRequest->iDataMOSI) {
                p->iDR = 0xFF;
                p->iDR = 0xFF;
                p->iDR = 0xFF;
                p->iDR = 0xFF;
                p->iDR = 0xFF;
                p->iDR = 0xFF;
                p->iDR = 0xFF;
                p->iDR = 0xFF;
            } else {
                p->iDR = *(p_dataMOSI++);
                p->iDR = *(p_dataMOSI++);
                p->iDR = *(p_dataMOSI++);
                p->iDR = *(p_dataMOSI++);
                p->iDR = *(p_dataMOSI++);
                p->iDR = *(p_dataMOSI++);
                p->iDR = *(p_dataMOSI++);
                p->iDR = *(p_dataMOSI++);
            }

            // Wait until SSP done
            while (((p->iSR & SSP_TFE) == 0) || (p->iSR & SSP_BSY)) {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Receive 8
            *(p_dataMISO++) = p->iDR;
            *(p_dataMISO++) = p->iDR;
            *(p_dataMISO++) = p->iDR;
            *(p_dataMISO++) = p->iDR;
            *(p_dataMISO++) = p->iDR;
            *(p_dataMISO++) = p->iDR;
            *(p_dataMISO++) = p->iDR;
            *(p_dataMISO++) = p->iDR;

            sent += 8;
        } else {
            // There are now less than 8 bytes to send, send
            // one at a time.

            // Send 1
            if (!aRequest->iDataMOSI) {
                p->iDR = 0xFF;
            } else {
                p->iDR = *(p_dataMOSI++);
            }
            // Wait until SSP done
            while (((p->iSR & SSP_TFE) == 0) || (p->iSR & SSP_BSY)) {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Receive 1
            *(p_dataMISO++) = p->iDR;

            sent++;
        }
    }

    return UEZ_ERROR_NONE;
}

static void ISSPOutput(T_LPC43xx_SSP_Workspace *aW)
{
    T_LPC43xx_SSP_Registers *p = aW->iReg;
    SPI_Request *r = aW->iRequest;
    TUInt32 num;
    TUInt8 *p_dataMOSI8;

    // Is there any room to send data?
    num = r->iNumTransfers - r->iNumTransferredOut;
    if (r->iDataMOSI) {
        // Output bytes from the array
        if (num >= 4) {
            p_dataMOSI8 = ((TUInt8 *)r->iDataMOSI) + r->iNumTransferredOut;
            p->iDR = p_dataMOSI8[0];
            p->iDR = p_dataMOSI8[1];
            p->iDR = p_dataMOSI8[2];
            p->iDR = p_dataMOSI8[3];
            r->iNumTransferredOut+=4;
        } else if (num) {
            // Output all we got left
            p_dataMOSI8 = ((TUInt8 *)r->iDataMOSI) + r->iNumTransferredOut;
            while (num--) {
                p->iDR = *(p_dataMOSI8++);
                r->iNumTransferredOut++;
            }
        } else {
            // No longer report interrupts for room to transfer,
            // we are out of bytes to transfer.
            p->iIMSC &= ~(SSP_TXIM);
        }
    } else {
        // Output 0xFF's
        if (num >= 4) {
            // Output 4 in a bunch
            p->iDR = 0xFF;
            p->iDR = 0xFF;
            p->iDR = 0xFF;
            p->iDR = 0xFF;
            r->iNumTransferredOut += 4;
        } else if (num) {
            // Output the remaining
            while (num--) {
                p->iDR = 0xFF;
                r->iNumTransferredOut++;
            }
        } else {
            // There is no more to output, stop doing interrupts
            // from the TXIM
            p->iIMSC &= ~(SSP_TXIM);
        }
    }
}

static T_uezError LPC43xx_SSP_TransferNoBlock(
        void *aWorkspace,
        SPI_Request *aRequest,
        T_spiCompleteCallback aCallback,
        void *aCallbackWorkspace)
{
    T_LPC43xx_SSP_Workspace *aW = (T_LPC43xx_SSP_Workspace *)aWorkspace;
    T_LPC43xx_SSP_Registers *p = aW->iReg;

    // This routine only works with transfer of 8 bits or lower
    if (aRequest->iBitsPerTransfer > 8)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Disable any potential SSP interrupts
    InterruptDisable(aW->iIRQChannel);

    aRequest->iNumTransferredOut = 0;
    aRequest->iNumTransferredIn = 0;

    // Setup the callback and the parameters
    aW->iCompleteCallback = aCallback;
    aW->iCompleteCallbackWorkspace = aCallbackWorkspace;
    aW->iRequest = aRequest;

    // Setup SSP for this configuration
    ISSPConfig(aW, aRequest);

    // Start SSP
    ISSPStart(aRequest);

    // Note we are in the middle of a transaction
    aW->iIsBusy = ETrue;

    // Allow receive half full, receive idle, and transmit half empty interrupts
    p->iIMSC = SSP_RTIM|SSP_RXIM|SSP_TXIM;

    // Prime the pump and output in an amount of data
    // On interrupting, we'll feed in the data
    ISSPOutput(aW);

    // Now allow processing of the interrupts (which probably just occurred)
    InterruptEnable(aW->iIRQChannel);

    return UEZ_ERROR_NONE;
}

static TBool LPC43xx_SSP_IsBusy(void *aWorkspace)
{
    T_LPC43xx_SSP_Workspace *aW = (T_LPC43xx_SSP_Workspace *)aWorkspace;

    return aW->iIsBusy;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_SPI SSP_LPC43xx_Port0_Interface = {
        {
        "NXP:LPC43xx:SSP0",
        0x0202,
        LPC43xx_SSP_SSP0_InitializeWorkspace,
        sizeof(T_LPC43xx_SSP_Workspace),
        },

        LPC43xx_SSP_TransferPolled,

        LPC43xx_SSP_Start,
        LPC43xx_SSP_TransferInOut,
        LPC43xx_SSP_Stop,
        LPC43xx_SSP_TransferInOutBytes,

        // uEZ v2.02
        LPC43xx_SSP_TransferNoBlock,
        LPC43xx_SSP_IsBusy,
};

const HAL_SPI SSP_LPC43xx_Port1_Interface = {
        {
        "NXP:LPC43xx:SSP1",
        0x0202,
        LPC43xx_SSP_SSP1_InitializeWorkspace,
        sizeof(T_LPC43xx_SSP_Workspace),
        },

        LPC43xx_SSP_TransferPolled,

        LPC43xx_SSP_Start,
        LPC43xx_SSP_TransferInOut,
        LPC43xx_SSP_Stop,
        LPC43xx_SSP_TransferInOutBytes,

        // uEZ v2.02
        LPC43xx_SSP_TransferNoBlock,
        LPC43xx_SSP_IsBusy,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC43xx_SSP0_Require(
        T_uezGPIOPortPin aPinSCK0,
        T_uezGPIOPortPin aPinMISO0,
        T_uezGPIOPortPin aPinMOSI0,
        T_uezGPIOPortPin aPinSSEL0)
{

    HAL_DEVICE_REQUIRE_ONCE();

    static const T_LPC43xx_SCU_ConfigList miso0[] = {
            {GPIO_P0_8     ,SCU_NORMAL_DRIVE(5, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_P5_10    ,SCU_NORMAL_DRIVE(2, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_P4_13    ,SCU_NORMAL_DRIVE(7, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_P7_17    ,SCU_NORMAL_DRIVE(2, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
    };
    static const T_LPC43xx_SCU_ConfigList mosi0[] = {
            {GPIO_P0_9     ,SCU_NORMAL_DRIVE(5, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_P5_11    ,SCU_NORMAL_DRIVE(2, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_P4_14    ,SCU_NORMAL_DRIVE(7, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_P7_18    ,SCU_NORMAL_DRIVE(2, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
    };
    static const T_LPC43xx_SCU_ConfigList sck0[] = {
            {GPIO_PZ_2_P3_3 ,SCU_NORMAL_DRIVE(2, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_PZ_8_PF_0 ,SCU_NORMAL_DRIVE(0, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
    };
    static const T_LPC43xx_SCU_ConfigList ssel0[] = {
            {GPIO_P0_4     ,SCU_NORMAL_DRIVE(5, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_P0_6     ,SCU_NORMAL_DRIVE(2, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_P4_12    ,SCU_NORMAL_DRIVE(7, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
            {GPIO_P7_16    ,SCU_NORMAL_DRIVE(2, (1 << 7) , (1 << 6) , (1 << 5) , (1 << 4), (0 << 3))},
    };

    // Register SSP1
    HALInterfaceRegister("SSP0",
            (T_halInterface *)&SSP_LPC43xx_Port0_Interface, 0, 0);

    LPC43xx_SCU_ConfigPinOrNone(aPinSCK0, sck0, ARRAY_COUNT(sck0));
    LPC43xx_SCU_ConfigPinOrNone(aPinSSEL0, ssel0, ARRAY_COUNT(ssel0));
    LPC43xx_SCU_ConfigPinOrNone(aPinMISO0, miso0, ARRAY_COUNT(miso0));
    LPC43xx_SCU_ConfigPinOrNone(aPinMOSI0, mosi0, ARRAY_COUNT(mosi0));

    // Setup interrupt, but do not enable
#ifdef CORE_M4
    InterruptRegister(SSP0_IRQn, ISSP0IRQ, INTERRUPT_PRIORITY_HIGH, "SSP0");
    InterruptDisable(SSP0_IRQn);
#endif
#ifdef CORE_M0
    InterruptRegister(M0_SSP0_OR_SSP1_IRQn, ISSP0IRQ, INTERRUPT_PRIORITY_HIGH, "SSP");
    InterruptDisable(M0_SSP0_OR_SSP1_IRQn);
#endif
#ifdef CORE_M0SUB
    InterruptRegister(M0S_SSP0_OR_SSP1_IRQn, ISSP0IRQ, INTERRUPT_PRIORITY_HIGH, "SSP");
    InterruptDisable(M0S_SSP0_OR_SSP1_IRQn);
#endif
}

void LPC43xx_SSP1_Require(
        T_uezGPIOPortPin aPinSCK1,
        T_uezGPIOPortPin aPinMISO1,
        T_uezGPIOPortPin aPinMOSI1,
        T_uezGPIOPortPin aPinSSEL1)
{

    HAL_DEVICE_REQUIRE_ONCE();

    static const T_LPC43xx_SCU_ConfigList miso1[] = {
            {GPIO_P0_0     ,SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P0_10    ,SCU_NORMAL_DRIVE_DEFAULT(5)},
            {GPIO_P7_20    ,SCU_NORMAL_DRIVE_DEFAULT(2)},
    };
    static const T_LPC43xx_SCU_ConfigList mosi1[] = {
            {GPIO_P0_1     ,SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P0_11    ,SCU_NORMAL_DRIVE_DEFAULT(5)},
            {GPIO_P7_21    ,SCU_NORMAL_DRIVE_DEFAULT(2)},
    };
    static const T_LPC43xx_SCU_ConfigList sck1[] = {
            {GPIO_PZ_0_P1_19 ,SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_PZ_9_PF_4 ,SCU_NORMAL_DRIVE_DEFAULT(0)},
    };
    static const T_LPC43xx_SCU_ConfigList ssel1[] = {
            {GPIO_P1_8     ,SCU_NORMAL_DRIVE_DEFAULT(5)},
            {GPIO_P0_15    ,SCU_NORMAL_DRIVE_DEFAULT(1)},
            {GPIO_P7_19    ,SCU_NORMAL_DRIVE_DEFAULT(2)},
    };

    // Register SSP1
    HALInterfaceRegister("SSP1",
            (T_halInterface *)&SSP_LPC43xx_Port1_Interface, 0, 0);

    LPC43xx_SCU_ConfigPinOrNone(aPinSCK1, sck1, ARRAY_COUNT(sck1));
    LPC43xx_SCU_ConfigPinOrNone(aPinSSEL1, ssel1, ARRAY_COUNT(ssel1));
    LPC43xx_SCU_ConfigPinOrNone(aPinMISO1, miso1, ARRAY_COUNT(miso1));
    LPC43xx_SCU_ConfigPinOrNone(aPinMOSI1, mosi1, ARRAY_COUNT(mosi1));

    // Setup interrupt, but do not enable
#ifdef CORE_M4
    InterruptRegister(SSP1_IRQn, ISSP1IRQ, INTERRUPT_PRIORITY_HIGH, "SSP1");
    InterruptDisable(SSP1_IRQn);
#endif
#ifdef CORE_M0
    InterruptRegister(M0_SSP0_OR_SSP1_IRQn, ISSP1IRQ, INTERRUPT_PRIORITY_HIGH, "SSP");
    InterruptDisable(M0_SSP0_OR_SSP1_IRQn);
#endif
#ifdef CORE_M0SUB
    InterruptRegister(M0S_SSP0_OR_SSP1_IRQn, ISSP1IRQ, INTERRUPT_PRIORITY_HIGH, "SSP");
    InterruptDisable(M0S_SSP0_OR_SSP1_IRQn);
#endif
}

/*-------------------------------------------------------------------------*
 * End of File:  SSP.c
 *-------------------------------------------------------------------------*/

