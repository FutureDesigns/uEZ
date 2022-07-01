/*-------------------------------------------------------------------------*
 * File:  SPI.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC546xx SPI Bus Interface.
 * Implementation:
 *      Two SPIs are created, SPI0 and SPI1 and SPI2.
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
#include "LPC546xx_SPI.h"
#include "Config.h"
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <HAL/GPIO.h>
#include <HAL/Interrupt.h>

//TODO: Remove
#include "iar/Include/CMSIS/LPC54608.h"
/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TUInt8 RESERVED_0[1024];
    TUInt32 iCFG;
    TUInt32 iDLY;
    TUInt32 iSTAT;
    TUInt32 iINTENSET;
    TUInt32 iINTENCLR;
    TUInt8 RESERVED_1[16];
    TUInt32 iDIV;
    TUInt32 iINTSTAT;
    TUInt8 RESERVED_2[2516];
    TUInt32 iFIFOCFG;
    TUInt32 iFIFOSTAT;
    TUInt32 iFIFOTRIG;
    TUInt8 RESERVED_3[4];
    TUInt32 iFIFOINTENSET;
    TUInt32 iFIFOINTENCLR;
    TUInt32 iFIFOINTSTAT;
    TUInt8 RESERVED_4[4];
    TUInt32 iFIFOWR;
    TUInt8 RESERVED_5[12];
    TUInt32 iFIFORD;
    TUInt8 RESERVED_6[12];
    TUInt32 iFIFORDNOPOP;
    TUInt8 RESERVED_7[440];
    TUInt32 iID;
} T_LPC546xx_SPI_Registers;

typedef struct {
    const HAL_SPI *iHAL;
    T_LPC546xx_SPI_Registers *iReg;
    T_spiCompleteCallback iCompleteCallback;
    void *iCompleteCallbackWorkspace;
    SPI_Request *iRequest;
    T_irqChannel iIRQChannel;
    TBool iIsBusy;
} T_LPC546xx_SPI_Workspace;

/*---------------------------------------------------------------------------*
 * Function Prototypes:
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE( ISPI6IRQ );
static void ISPIEnd(SPI_Request *aRequest);
static void ISPIProcessInterrupt(T_LPC546xx_SPI_Workspace *aW);
static void ISPIOutput(T_LPC546xx_SPI_Workspace *aW);

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LPC546xx_SPI_Workspace *G_SPI0Workspace;
static T_LPC546xx_SPI_Workspace *G_SPI6Workspace;

/*---------------------------------------------------------------------------*
 * Routine: ISPIxIRQ
 *---------------------------------------------------------------------------*
 * Description:
 *      Capture the SPIx interrupts and forward to ISPIProcessInterrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE( ISPI6IRQ )
{
    IRQ_START();
    ISPIProcessInterrupt(G_SPI0Workspace);
    IRQ_END();
}


/*---------------------------------------------------------------------------*
 * Routine: ISPIProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Generic routine for handling any of the SPI interrupts.
 *      NOTE: This routine is ALWAYS processed inside of a interrupt
 *              service routine.
 * Inputs:
 *      T_SerialPortEntry *p      -- Port's definition to process interrupt
 *---------------------------------------------------------------------------*/
static void ISPIProcessInterrupt(T_LPC546xx_SPI_Workspace *aW)
{
    T_LPC546xx_SPI_Registers *p = aW->iReg;
    SPI_Request *r = aW->iRequest;
    TUInt8 ris;
    TUInt32 data;

    while (1) {
        // Do we have any interrupts to process now?
        ris = p->iFIFOSTAT;
        if (!ris)
            break;

        // Is data waiting to be received?
        if (ris & ((1<<6))) {//RXNOTEMPTY
            // Pull the data out of the SPI peripheral
            if (r->iNumTransferredIn < r->iNumTransfers) {
                data = p->iFIFORD & 0xFF;
                if (r->iDataMISO)
                    ((TUInt8 *)r->iDataMISO)[r->iNumTransferredIn++] = data;
            }

            // If we have reached the end, we are done
            if (r->iNumTransferredIn >= r->iNumTransfers) {
                // No more processing.  Stop here.
                InterruptDisable(aW->iIRQChannel);

                // Clear any remaining interrupts
                p->iFIFOINTSTAT = 0;//TODO: Check

                // Mask out all interrupts
                p->iFIFOINTENCLR = 0;

                // End the chip select
                ISPIEnd(aW->iRequest);

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
                //p->iICR = SPI_RORIC;
            }
        } else if (ris & (!(1<<4))) { //TXEMPTY
            ISPIOutput(aW);
        } else {
            // Some other interrupt.  Just clear it.  Possibly an error.
            p->iFIFOINTENCLR = 0;
        }
    }

}

/*---------------------------------------------------------------------------*
 * Routine:  ISPIStart
 *---------------------------------------------------------------------------*
 * Description:
 *      SPI is going to start -- chip select enabled for device
 * Outputs:
 *      SPI_Request *aRequest      -- Configuration of SPI device
 *---------------------------------------------------------------------------*/
static void ISPIStart(SPI_Request *aRequest)
{
    if (aRequest->iCSPolarity) {
        (*aRequest->iCSGPIOPort)->Set(aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    } else {
        (*aRequest->iCSGPIOPort)->Clear(aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ISPIEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      SPI is going to end -- chip select disabled for device
 * Outputs:
 *      SPI_Request *aRequest      -- Configuration of SPI device
 *---------------------------------------------------------------------------*/
static void ISPIEnd(SPI_Request *aRequest)
{
    if (aRequest->iCSPolarity) {
        (*(aRequest->iCSGPIOPort))->Clear(aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    } else {
        (*(aRequest->iCSGPIOPort))->Set(aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ISPIConfig
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the SPI for the particular SPI request.
 * Outputs:
 *      T_LPC546xx_SPI_Workspace *aW -- Particular SPI workspace
 *      SPI_Request *aRequest      -- Configuration of SPI device
 *---------------------------------------------------------------------------*/
static void ISPIConfig(T_LPC546xx_SPI_Workspace *aW, SPI_Request *aRequest)
{
    T_LPC546xx_SPI_Registers *p = aW->iReg;
    TUInt32 scr;
    TUInt32 dmy;

    // Set the CS as an output io pin
    (*(aRequest->iCSGPIOPort))->SetOutputMode(aRequest->iCSGPIOPort,
            aRequest->iCSGPIOBit);

    // Make sure the SPI clock is main/2
    p->iDIV = 1;
    // Calculate the scr prescalar
    scr = ((Fpclk/1000)/(p->iDIV * aRequest->iRate));
    // If too fast, run it down one more divider (this happenes when not
    // exactly on the right divider)
    while (((Fpclk/1000)/(p->iDIV * scr)) > aRequest->iRate)
        scr++;
    // Final value of scr is one less
    scr--;

    p->iCFG = (1<<2) | (aRequest->iClockOutPolarity ? (1<<5) : 0) |
                       (aRequest->iClockOutPhase ? (1<<4) : 0);
    
    p->iFIFOWR |= (aRequest->iBitsPerTransfer - 1) << 24;

    // Enable the SPI
    p->iCFG |= 1;

    // Clear out the SPI buffer
    while (p->iFIFOSTAT & (1<<6))
        dmy = p->iFIFORD;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SPI_TransferPolled
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a SPI transfer (both read and write) out the SPI port using
 *      the given request. CS should be set as a GPIO output before calling
 *      this functions.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 *      SPI_Request *aRequest       -- Request to perform
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_SPI_TransferPolled(void *aWorkspace, SPI_Request *aRequest)
{
    TUInt32 i;
    T_LPC546xx_SPI_Workspace *aW = (T_LPC546xx_SPI_Workspace *)aWorkspace;
    T_LPC546xx_SPI_Registers *p = aW->iReg;
    TUInt8 *p_dataMOSI8;
    TUInt8 *p_dataMISO8;
    TUInt16 *p_dataMOSI16;
    TUInt16 *p_dataMISO16;
    TVUInt32 dummy;
    int isToggling = aRequest->iFlags & SPI_REQUEST_TOGGLING_CS;

    // No interrupt processing
    InterruptDisable(aW->iIRQChannel);

    // Setup SPI for this configuration
    ISPIConfig(aW, aRequest);

    // Start SPI
    ISPIStart(aRequest);

    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer == 1) {
//        // Optimized version for 1 byte
//        // Wait for SPI to be no longer busy and transmit to be empty
//        while ((p->iSR & SPI_BSY) || (!(p->iSR & SPI_TFE))) {
//            // TBD: Polling here instead of interrupt
//        }
//        p->iDR = (TUInt32)((aRequest->iDataMOSI)?*((TUInt8 *)aRequest->iDataMOSI):0xFF);
//        // Wait until SPI done  (transmit empty)
//        while ((p->iSR & SPI_BSY) || ((p->iSR & SPI_TFE) == 0)) {
//            // TBD: For polling this is fine, but we may need to yield at some point?
//        }
//        dummy = p->iDR;
//        if (aRequest->iDataMISO)
//            *((TUInt8 *)aRequest->iDataMISO) = dummy;
    } else if (aRequest->iBitsPerTransfer <= 8) {
        // Doing bytes
        p_dataMOSI8 = (TUInt8 *)(aRequest->iDataMOSI);
        p_dataMISO8 = (TUInt8 *)(aRequest->iDataMISO);

        for (i = 0; i < aRequest->iNumTransfers; i++) {
            // Wait for SPI to be no longer busy and transmit to be empty
            while ((p->iFIFOINTSTAT & !(1<<4)) != 0) {
                // TBD: Polling here instead of interrupt
            }

            if (isToggling) {
                ISPIStart(aRequest);
            }
            if (p_dataMOSI8) {
                // Send the SPI data (this also triggers a read in)
                if (((i + 4) <= aRequest->iNumTransfers) && (!isToggling)) {
                    // Do 4 at one time
                    p->iFIFOWR = (TUInt32)*(p_dataMOSI8++);
                    p->iFIFOWR = (TUInt32)*(p_dataMOSI8++);
                    p->iFIFOWR = (TUInt32)*(p_dataMOSI8++);
                    p->iFIFOWR = (TUInt32)*(p_dataMOSI8++);
                } else {
                    p->iFIFOWR = (TUInt32)*(p_dataMOSI8++);
                }
            } else {
                // Send the SPI data (this also triggers a read in)
                if (((i + 4) <= aRequest->iNumTransfers) && (!isToggling)) {
                    // Do 4 at one time
                    p->iFIFOWR = (TUInt32)0xFF;
                    p->iFIFOWR = (TUInt32)0xFF;
                    p->iFIFOWR = (TUInt32)0xFF;
                    p->iFIFOWR = (TUInt32)0xFF;
                } else {
                    p->iFIFOWR = (TUInt32)0xFF;
                }
            }

            // Wait until SPI done  (transmit empty)
            while ((p->iFIFOINTSTAT & (1<<4) != 0)) {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Store the SPI received over the data sent
            if (p_dataMISO8) {
                // Store the SPI received over the data sent
                if (((i + 4) <= aRequest->iNumTransfers) && (!isToggling)) {
                    // Do 4 at one time
                    *(p_dataMISO8++) = p->iFIFORD;
                    *(p_dataMISO8++) = p->iFIFORD;
                    *(p_dataMISO8++) = p->iFIFORD;
                    *(p_dataMISO8++) = p->iFIFORD;
                    i += 3;
                } else {
                    *(p_dataMISO8++) = p->iFIFORD;
                }
            } else {
                // Store the SPI received over the data sent
                if (((i + 4) <= aRequest->iNumTransfers) && (!isToggling)) {
                    // Do 4 at one time
                    dummy = p->iFIFORD;
                    dummy = p->iFIFORD;
                    dummy = p->iFIFORD;
                    dummy = p->iFIFORD;
                    i += 3;
                } else {
                    dummy = p->iFIFORD;
                }
            }
            if (isToggling) {
                ISPIEnd(aRequest);
            }
        }
    } else {
        // Doing words
        p_dataMOSI16 = (TUInt16 *)(aRequest->iDataMOSI);
        p_dataMISO16 = (TUInt16 *)(aRequest->iDataMISO);

        for (i = 0; i < aRequest->iNumTransfers; i++) {
            // Wait for SPI to be no longer busy and transmit to be empty
            while ((p->iFIFOINTSTAT & (1<<4) != 0)) {
                // TBD: Polling here instead of interrupt
            }

            // Send the SPI data (this also triggers a read in)
            if (p_dataMOSI16)
                p->iFIFOWR = (TUInt32)*(p_dataMOSI16++);
            else
                p->iFIFOWR = 0xFFFF;

            // Wait until SPI done
            while ((p->iFIFOINTSTAT & (1<<4) != 0)) {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Store the SPI received over the data sent
            if (p_dataMISO16)
                *(p_dataMISO16++) = p->iFIFORD;
            else
                dummy = p->iFIFORD;
        }
    }

    // Do this to get rid of warning in compiler that dummy is set but not used
    VARIABLE_NOT_USED(dummy);

    // End SPI
    ISPIEnd(aRequest);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SPI_SPI1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC546xx SPI1 workspace.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_SPI_SPI6_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_SPI_Workspace *p = (T_LPC546xx_SPI_Workspace *)aWorkspace;
    p->iReg = (T_LPC546xx_SPI_Registers *)SPI6;

    G_SPI6Workspace = p;
    p->iIsBusy = EFalse;
    p->iIRQChannel = FLEXCOMM6_IRQn;

    //Turn on peripheral clock


    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SPI_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Start a SPI transfer by putting the chip select in the active state
 *      after configuring the SPI speed and other settings.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 *      SPI_Request *aRequest       -- SPI request being processed
 *---------------------------------------------------------------------------*/
void LPC546xx_SPI_Start(void *aWorkspace, SPI_Request *aRequest)
{
    // Setup SPI for this configuration
    ISPIConfig(aWorkspace, aRequest);
    ISPIStart(aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SPI_TransferInOut
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a single SPI transfer to the SPI device.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 *      SPI_Request *aRequest       -- SPI request being processed
 *      TUInt32 aSend               -- Item to transfer
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static TUInt32 LPC546xx_SPI_TransferInOut(
        void *aWorkspace,
        SPI_Request *aRequest,
        TUInt32 aSend)
{
    TUInt32 recv;
    T_LPC546xx_SPI_Workspace *aW = (T_LPC546xx_SPI_Workspace *)aWorkspace;
    T_LPC546xx_SPI_Registers *p = aW->iReg;

    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer <= 8) {
        // Send the SPI data (this also triggers a read in)
        p->iFIFOWR = aSend;

        // Wait until SPI done
        while ((p->iFIFOINTSTAT & (1<<4) != 0)) {
            // TBD: For polling this is fine, but we may need to yield at some point?
        }

        // Store the SPI received over the data sent
        recv = p->iFIFORD;
    } else {
        // Send the SPI data (this also triggers a read in)
        p->iFIFOWR = (TUInt32)*((TUInt16 *)aSend);

        // Wait until SPI done
        while ((p->iFIFOINTSTAT & (1<<6))) {
            // TBD: For polling this is fine, but we may need to yield at some point?
        }

        // Store the SPI received over the data sent
        recv = p->iFIFORD;
    }

    return recv;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SPI_Stop
 *---------------------------------------------------------------------------*
 * Description:
 *      End a SPI transfer by putting the chip select in the done state.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 *      SPI_Request *aRequest       -- SPI request being processed
 *---------------------------------------------------------------------------*/
static void LPC546xx_SPI_Stop(void *aWorkspace, SPI_Request *aRequest)
{
    ISPIEnd(aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SPI_SPI1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Transfer a set of SPI bytes (in the request) to the given
 *      target.  Works similarly to TransferPolled except this routine
 *      does not affect the chip select state.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 *      SPI_Request *aRequest       -- SPI request being processed
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError LPC546xx_SPI_TransferInOutBytes(
        void *aWorkspace,
        SPI_Request *aRequest)
{
    T_LPC546xx_SPI_Workspace *aW = (T_LPC546xx_SPI_Workspace *)aWorkspace;
    T_LPC546xx_SPI_Registers *p = aW->iReg;
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
                p->iFIFOWR = 0xFF;
                p->iFIFOWR = 0xFF;
                p->iFIFOWR = 0xFF;
                p->iFIFOWR = 0xFF;
                p->iFIFOWR = 0xFF;
                p->iFIFOWR = 0xFF;
                p->iFIFOWR = 0xFF;
                p->iFIFOWR = 0xFF;
            } else {
                p->iFIFOWR = *(p_dataMOSI++);
                p->iFIFOWR = *(p_dataMOSI++);
                p->iFIFOWR = *(p_dataMOSI++);
                p->iFIFOWR = *(p_dataMOSI++);
                p->iFIFOWR = *(p_dataMOSI++);
                p->iFIFOWR = *(p_dataMOSI++);
                p->iFIFOWR = *(p_dataMOSI++);
                p->iFIFOWR = *(p_dataMOSI++);
            }

            // Wait until SPI done
            while ((p->iFIFOINTSTAT & (1<<4) != 0)) {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Receive 8
            *(p_dataMISO++) = p->iFIFORD;
            *(p_dataMISO++) = p->iFIFORD;
            *(p_dataMISO++) = p->iFIFORD;
            *(p_dataMISO++) = p->iFIFORD;
            *(p_dataMISO++) = p->iFIFORD;
            *(p_dataMISO++) = p->iFIFORD;
            *(p_dataMISO++) = p->iFIFORD;
            *(p_dataMISO++) = p->iFIFORD;

            sent += 8;
        } else {
            // There are now less than 8 bytes to send, send
            // one at a time.

            // Send 1
            if (!aRequest->iDataMOSI) {
                p->iFIFOWR = 0xFF;
            } else {
                p->iFIFOWR = *(p_dataMOSI++);
            }
            // Wait until SPI done
            while ((p->iFIFOINTSTAT & (1<<4) != 0)) {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Receive 1
            *(p_dataMISO++) = p->iFIFORD;

            sent++;
        }
    }

    return UEZ_ERROR_NONE;
}

static void ISPIOutput(T_LPC546xx_SPI_Workspace *aW)
{
    T_LPC546xx_SPI_Registers *p = aW->iReg;
    SPI_Request *r = aW->iRequest;
    TUInt32 num;
    TUInt8 *p_dataMOSI8;

    // Is there any room to send data?
    num = r->iNumTransfers - r->iNumTransferredOut;
    if (r->iDataMOSI) {
        // Output bytes from the array
        if (num >= 4) {
            p_dataMOSI8 = ((TUInt8 *)r->iDataMOSI) + r->iNumTransferredOut;
            p->iFIFOWR = p_dataMOSI8[0];
            p->iFIFOWR = p_dataMOSI8[1];
            p->iFIFOWR = p_dataMOSI8[2];
            p->iFIFOWR = p_dataMOSI8[3];
            r->iNumTransferredOut+=4;
        } else if (num) {
            // Output all we got left
            p_dataMOSI8 = ((TUInt8 *)r->iDataMOSI) + r->iNumTransferredOut;
            while (num--) {
                p->iFIFOWR = *(p_dataMOSI8++);
                r->iNumTransferredOut++;
            }
        } else {
            // No longer report interrupts for room to transfer,
            // we are out of bytes to transfer.

        }
    } else {
        // Output 0xFF's
        if (num >= 4) {
            // Output 4 in a bunch
            p->iFIFOWR = 0xFF;
            p->iFIFOWR = 0xFF;
            p->iFIFOWR = 0xFF;
            p->iFIFOWR = 0xFF;
            r->iNumTransferredOut += 4;
        } else if (num) {
            // Output the remaining
            while (num--) {
                p->iFIFOWR = 0xFF;
                r->iNumTransferredOut++;
            }
        } else {
            // There is no more to output, stop doing interrupts

        }
    }
}

static T_uezError LPC546xx_SPI_TransferNoBlock(
        void *aWorkspace,
        SPI_Request *aRequest,
        T_spiCompleteCallback aCallback,
        void *aCallbackWorkspace)
{
    T_LPC546xx_SPI_Workspace *aW = (T_LPC546xx_SPI_Workspace *)aWorkspace;
    T_LPC546xx_SPI_Registers *p = aW->iReg;

    // This routine only works with transfer of 8 bits or lower
    if (aRequest->iBitsPerTransfer > 8)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Disable any potential SPI interrupts
    InterruptDisable(aW->iIRQChannel);

    aRequest->iNumTransferredOut = 0;
    aRequest->iNumTransferredIn = 0;

    // Setup the callback and the parameters
    aW->iCompleteCallback = aCallback;
    aW->iCompleteCallbackWorkspace = aCallbackWorkspace;
    aW->iRequest = aRequest;

    // Setup SPI for this configuration
    ISPIConfig(aW, aRequest);

    // Start SPI
    ISPIStart(aRequest);

    // Note we are in the middle of a transaction
    aW->iIsBusy = ETrue;

    // Allow receive half full, receive idle, and transmit half empty interrupts
    p->iFIFOINTENSET = (1<<2) | (1<<3);

    // Prime the pump and output in an amount of data
    // On interrupting, we'll feed in the data
    ISPIOutput(aW);

    // Now allow processing of the interrupts (which probably just occurred)
    InterruptEnable(aW->iIRQChannel);

    return UEZ_ERROR_NONE;
}

static TBool LPC546xx_SPI_IsBusy(void *aWorkspace)
{
    T_LPC546xx_SPI_Workspace *aW = (T_LPC546xx_SPI_Workspace *)aWorkspace;

    return aW->iIsBusy;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_SPI SPI_LPC546xx_Port6_Interface = {
        {
        "NXP:LPC546xx:SPI6",
        0x0202,
        LPC546xx_SPI_SPI6_InitializeWorkspace,
        sizeof(T_LPC546xx_SPI_Workspace),
        },

        LPC546xx_SPI_TransferPolled,

        LPC546xx_SPI_Start,
        LPC546xx_SPI_TransferInOut,
        LPC546xx_SPI_Stop,
        LPC546xx_SPI_TransferInOutBytes,

        // uEZ v2.02
        LPC546xx_SPI_TransferNoBlock,
        LPC546xx_SPI_IsBusy,
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_SPI6_Require(
        T_uezGPIOPortPin aPinSCK6,
        T_uezGPIOPortPin aPinMISO6,
        T_uezGPIOPortPin aPinMOSI6,
        T_uezGPIOPortPin aPinSSEL6)
{

    HAL_DEVICE_REQUIRE_ONCE();

    static const T_LPC546xx_ICON_ConfigList miso6[] = {
            {GPIO_P4_3      , IOCON_D_DEFAULT(2)},
    };
    static const T_LPC546xx_ICON_ConfigList mosi6[] = {
            {GPIO_P4_2      , IOCON_D_DEFAULT(2)},
    };
    static const T_LPC546xx_ICON_ConfigList sck6[] = {
            {GPIO_P4_1      , IOCON_D_DEFAULT(2)},
    };
    static const T_LPC546xx_ICON_ConfigList ssel6[] = {
            {GPIO_P4_0      , IOCON_D_DEFAULT(2)},
    };

    // Register SPI1
    HALInterfaceRegister("SPI6",
            (T_halInterface *)&SPI_LPC546xx_Port6_Interface, 0, 0);

    LPC546xx_ICON_ConfigPinOrNone(aPinSCK6, sck6, ARRAY_COUNT(sck6));
    LPC546xx_ICON_ConfigPinOrNone(aPinSSEL6, ssel6, ARRAY_COUNT(ssel6));
    LPC546xx_ICON_ConfigPinOrNone(aPinMISO6, miso6, ARRAY_COUNT(miso6));
    LPC546xx_ICON_ConfigPinOrNone(aPinMOSI6, mosi6, ARRAY_COUNT(mosi6));

    //Setup and lock the Flexcomm for SPI mode
    LPC546xxPowerOn(kCLOCK_FlexComm6);
    SYSCON->FCLKSEL[6] = 0x03; //Main clock
    FLEXCOMM6->PSELID = 0x02; //Lock, SPI mode
     FLEXCOMM6->PSELID |= (1<<3);

    // Setup interrupt, but do not enable
    InterruptRegister(FLEXCOMM6_IRQn, ISPI6IRQ, INTERRUPT_PRIORITY_HIGH, "SPI6");
    InterruptDisable(FLEXCOMM6_IRQn);
}


/*-------------------------------------------------------------------------*
 * End of File:  SPI.c
 *-------------------------------------------------------------------------*/

