/*-------------------------------------------------------------------------*
 * File:  SPI.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC1768 SPI Bus Interface.
 * Implementation:
 *      A single SPI0 is created, but the code can be easily changed for 
 *      other processors to use multiple SPI busses.
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
#include <HAL/GPIO.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_SPI.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iCR;
    TVUInt32 iSR;
        #define SPI_ABRT (1<<3)     // Slave abort
        #define SPI_MODF (1<<4)     // Mode fault
        #define SPI_ROVR (1<<5)     // Read overrun
        #define SPI_WCOL (1<<6)     // Write collision
        #define SPI_SPIF (1<<7)     // SPI transfer complete flag
    TVUInt32 iDR;
    TVUInt32 iCCR; // 0x0C
    TVUInt32 reserved1; // 0x10
    TVUInt32 reserved2; // 0x14
    TVUInt32 reserved3; // 0x18
    TVUInt32 iINT; // 0x1C
} T_LPC1768_SPI_Registers;

typedef struct {
    const HAL_SPI *iHAL;
    T_LPC1768_SPI_Registers *iReg;
} T_LPC1768_SPI_Workspace;

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
    if (aRequest->iCSPolarity)  {
        (*aRequest->iCSGPIOPort)->Set(
                aRequest->iCSGPIOPort, 
                aRequest->iCSGPIOBit);
    } else {
        (*aRequest->iCSGPIOPort)->Clear(
                aRequest->iCSGPIOPort, 
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
    if (aRequest->iCSPolarity)  {
        (*(aRequest->iCSGPIOPort))->Clear(
                aRequest->iCSGPIOPort, 
                aRequest->iCSGPIOBit);
    } else {
        (*(aRequest->iCSGPIOPort))->Set(
                aRequest->iCSGPIOPort, 
                aRequest->iCSGPIOBit);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ISPIConfig
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the SPI for the particular SPI request.
 * Outputs:
 *      T_LPC1768_SPI_Workspace *aW -- Particular SPI workspace
 *      SPI_Request *aRequest      -- Configuration of SPI device
 *---------------------------------------------------------------------------*/
static void ISPIConfig(T_LPC1768_SPI_Workspace *aW, SPI_Request *aRequest)
{
    T_LPC1768_SPI_Registers *p = aW->iReg;
    TUInt32 ccr;

    // Ensure power is on
    SC->PCONP |= (1<<8);

    // Disable for the moment
    p->iCR = 0;

    // Set pins for use with SPI
    (*(aRequest->iCSGPIOPort))->Activate(
            aRequest->iCSGPIOPort, 
            aRequest->iCSGPIOBit);

    // Set the CS as an output io pin
    (*(aRequest->iCSGPIOPort))->SetOutputMode(
            aRequest->iCSGPIOPort, 
            aRequest->iCSGPIOBit);

    // Calculate the scr divider (rounding up to the next even number)
    ccr = (((((Fpclk/1000)+(aRequest->iRate-1))/(aRequest->iRate)))+1)&0xFFFE;
    // Minimum of 8 divide factor
    if (ccr < 8)
        ccr = 8;
    p->iCCR = ccr;

    // Enable and run
    p->iCR = 
        // clock out phase
        (aRequest->iClockOutPhase?(1<<3):0) |
        // SPI interface only
        (1<<5) | // master mode
        (0<<6) | // MSB first
        (0<<7) | // no interrupts
        // Clock out polarity
        (aRequest->iClockOutPolarity?(1<<4):0) |
        // Control numbe of bits
        (1<<2) |
        // Number of bits per transaction
        ((aRequest->iBitsPerTransfer&0x0F)<<8) ;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_SPI_TransferPolled
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a SPI transfer (both read and write) out the SPI port using
 *      the given request.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 *      SPI_Request *aRequest       -- Request to perform
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_SPI_TransferPolled(void *aWorkspace, SPI_Request *aRequest)
{
    TUInt32 i;
    T_LPC1768_SPI_Workspace *aW = (T_LPC1768_SPI_Workspace *)aWorkspace;
    T_LPC1768_SPI_Registers *p = aW->iReg;
    TUInt8 *p_dataMOSI8;
    TUInt8 *p_dataMISO8;
    TUInt16 *p_dataMOSI16;
    TUInt16 *p_dataMISO16;

    // Setup SPI for this configuration
    ISPIConfig(aW, aRequest);

    // Start SPI
    ISPIStart(aRequest);

    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer <= 8)  {
        // Doing bytes
        p_dataMOSI8 = (TUInt8 *)(aRequest->iDataMOSI);
        p_dataMISO8 = (TUInt8 *)(aRequest->iDataMISO);

        for (i=0; i<aRequest->iNumTransfers; i++)  {
            // Send the SPI data (this also triggers a read in)
            p->iDR = (TUInt32) *(p_dataMOSI8++);

            // Wait until SPI done 
            while ((p->iSR & SPI_SPIF)==0)  {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Store the SPI received over the data sent
            *(p_dataMISO8++) = p->iDR;
        }
    } else {
        // Doing words
        p_dataMOSI16 = (TUInt16 *)(aRequest->iDataMOSI);
        p_dataMISO16 = (TUInt16 *)(aRequest->iDataMISO);

        for (i=0; i<aRequest->iNumTransfers; i++)  {
            // Send the SPI data (this also triggers a read in)
            p->iDR = (TUInt32) *(p_dataMOSI16++);

            // Wait until SPI done 
            while ((p->iSR & SPI_SPIF)==0)  {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Store the SPI received over the data sent
            *(p_dataMISO16++) = p->iDR;
        }
    }
    // End SPI
    ISPIEnd(aRequest);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC1768_SPI_SPI0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC1768 SPI0 workspace.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC1768_SPI_SPI0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1768_SPI_Workspace *p = (T_LPC1768_SPI_Workspace *)aWorkspace;
    p->iReg = (T_LPC1768_SPI_Registers *)SPI_BASE;

    return UEZ_ERROR_NONE;
}

void LPC1768_SPI_Start(void *aWorkspace, SPI_Request *aRequest)
{
    // Setup SPI for this configuration
    ISPIConfig(aWorkspace, aRequest);
    ISPIStart(aRequest);
}

TUInt32 LPC1768_SPI_TransferInOut(
        void *aWorkspace, 
        SPI_Request *aRequest, 
        TUInt32 aSend)
{
    TUInt32 recv;
    T_LPC1768_SPI_Workspace *aW = (T_LPC1768_SPI_Workspace *)aWorkspace;
    T_LPC1768_SPI_Registers *p = aW->iReg;

    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer <= 8)  {
        // Send the SPI data (this also triggers a read in)
        p->iDR = aSend;

        // Wait until SPI done 
        while ((p->iSR & SPI_SPIF)==0)  {
            // TBD: For polling this is fine, but we may need to yield at some point?
        }

        // Store the SPI received over the data sent
        recv = p->iDR;
    } else {
        // Send the SPI data (this also triggers a read in)
        p->iDR = (TUInt32) *((TUInt16 *)aSend);

        // Wait until SPI done 
        while ((p->iSR & SPI_SPIF)==0)  {
            // TBD: For polling this is fine, but we may need to yield at some point?
        }

        // Store the SPI received over the data sent
        recv = p->iDR;
    }

    return recv;
}

void LPC1768_SPI_Stop(void *aWorkspace, SPI_Request *aRequest)
{
    ISPIEnd(aRequest);
}

T_uezError LPC1768_SPI_TransferInOutBytes(void *aWorkspace, SPI_Request *aRequest)
{
    UEZFailureMsg("SPI TODO");
}
T_uezError LPC1768_SPI_TransferNoBlock(
    void *aWorkspace,
    SPI_Request *aRequest,
    T_spiCompleteCallback aCallback,
    void *aCallbackWorkspace)
{
    UEZFailureMsg("SPI TODO");
}
TBool LPC1768_SPI_IsBusy(void *aWorkspace)
{
    UEZFailureMsg("SPI TODO");
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_SPI SPI_LPC1768_Port0_Interface = {
    {
    "LPC1768 SPI0",
    0x0100,
    LPC1768_SPI_SPI0_InitializeWorkspace,
    sizeof(T_LPC1768_SPI_Workspace),
    },

    LPC1768_SPI_TransferPolled,

    LPC1768_SPI_Start,
    LPC1768_SPI_TransferInOut,
    LPC1768_SPI_Stop,

    LPC1768_SPI_TransferInOutBytes,

    // uEZ v2.02
    LPC1768_SPI_TransferNoBlock,
    LPC1768_SPI_IsBusy,
};

/*-------------------------------------------------------------------------*
 * End of File:  SPI.c
 *-------------------------------------------------------------------------*/

