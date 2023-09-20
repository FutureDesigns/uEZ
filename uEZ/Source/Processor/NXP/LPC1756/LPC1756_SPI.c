/*-------------------------------------------------------------------------*
 * File:  LPC1756_SPI.c
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
 /*
 *  @{
 *  @brief     uEZ LPC1756 SPI Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    HAL implementation of the LPC1756 SPI Bus Interface.
 * Implementation:
 *      A single SPI0 is created, but the code can be easily changed for
 *      other processors to use multiple SPI busses.
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZ.h>
 * TODO
 * @endcode
 */
#include <Config.h>
#include <uEZTypes.h>
#include <HAL/GPIO.h>
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC1756/LPC1756_SPI.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iCR;
    TVUInt32 iSR;
        #define SPI_ABRT (1<<3)     /** Slave abort */
        #define SPI_MODF (1<<4)     /** Mode fault */
        #define SPI_ROVR (1<<5)     /** Read overrun */
        #define SPI_WCOL (1<<6)     /** Write collision */
        #define SPI_SPIF (1<<7)     /** SPI transfer complete flag */
    TVUInt32 iDR;
    TVUInt32 iCCR; /** 0x0C */
    TVUInt32 reserved1; /** 0x10 */
    TVUInt32 reserved2; /** 0x14 */
    TVUInt32 reserved3; /** 0x18 */
    TVUInt32 iINT; /** 0x1C */
} T_LPC1756_SPI_Registers;

typedef struct {
    const HAL_SPI *iHAL;
    T_LPC1756_SPI_Registers *iReg;
} T_LPC1756_SPI_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  ISPIStart
 *---------------------------------------------------------------------------*/
/**
 *  SPI is going to start -- chip select enabled for device
 *
 *  @param [in]    *aRequest  	Configuration of SPI device
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
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
 *---------------------------------------------------------------------------*/
/**
 *  SPI is going to end -- chip select disabled for device
 *
 *  @param [in]   *aRequest 	Configuration of SPI device
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
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
 *---------------------------------------------------------------------------*/
/**
 *  Configure the SPI for the particular SPI request.
 *
 *  @param [in]    *aW			Particular SPI workspace
 *
 *  @param [in]    *aRequest   	Configuration of SPI device
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  
 *  TODO
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
static void ISPIConfig(T_LPC1756_SPI_Workspace *aW, SPI_Request *aRequest)
{
    T_LPC1756_SPI_Registers *p = aW->iReg;
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
 * Routine:  LPC1756_SPI_TransferPolled
 *---------------------------------------------------------------------------*/
/**
 *  Do a SPI transfer (both read and write) out the SPI port using
 *      the given request.
 *
 *  @param [in]    *aWorkspace     	Particular SPI workspace
 *
 *  @param [in]    *aRequest       	Request to perform
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
T_uezError LPC1756_SPI_TransferPolled(void *aWorkspace, SPI_Request *aRequest)
{
    TUInt32 i;
    T_LPC1756_SPI_Workspace *aW = (T_LPC1756_SPI_Workspace *)aWorkspace;
    T_LPC1756_SPI_Registers *p = aW->iReg;
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
 * Routine:  LPC1756_SPI_SPI0_InitializeWorkspace
 *---------------------------------------------------------------------------*/
/**
 *  Setup the LPC1756 SPI0 workspace.
 *
 *  @param [in]    *aWorkspace  	Particular SPI workspace
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
T_uezError LPC1756_SPI_SPI0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC1756_SPI_Workspace *p = (T_LPC1756_SPI_Workspace *)aWorkspace;
    p->iReg = (T_LPC1756_SPI_Registers *)SPI_BASE;

    return UEZ_ERROR_NONE;
}

void LPC1756_SPI_Start(void *aWorkspace, SPI_Request *aRequest)
{
    // Setup SPI for this configuration
    ISPIConfig(aWorkspace, aRequest);
    ISPIStart(aRequest);
}

TUInt32 LPC1756_SPI_TransferInOut(
        void *aWorkspace, 
        SPI_Request *aRequest, 
        TUInt32 aSend)
{
    TUInt32 recv;
    T_LPC1756_SPI_Workspace *aW = (T_LPC1756_SPI_Workspace *)aWorkspace;
    T_LPC1756_SPI_Registers *p = aW->iReg;

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

void LPC1756_SPI_Stop(void *aWorkspace, SPI_Request *aRequest)
{
    ISPIEnd(aRequest);
}

T_uezError LPC1756_SPI_TransferInOutBytes(void *aWorkspace, SPI_Request *aRequest)
{
    UEZFailureMsg("SPI TODO");
}
T_uezError LPC1756_SPI_TransferNoBlock(
    void *aWorkspace,
    SPI_Request *aRequest,
    T_spiCompleteCallback aCallback,
    void *aCallbackWorkspace)
{
    UEZFailureMsg("SPI TODO");
}
TBool LPC1756_SPI_IsBusy(void *aWorkspace)
{
    UEZFailureMsg("SPI TODO");
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_SPI SPI_LPC1756_Port0_Interface = {
    {
    "LPC1756 SPI0",
    0x0100,
    LPC1756_SPI_SPI0_InitializeWorkspace,
    sizeof(T_LPC1756_SPI_Workspace),
    },

    LPC1756_SPI_TransferPolled,

    LPC1756_SPI_Start,
    LPC1756_SPI_TransferInOut,
    LPC1756_SPI_Stop,

    LPC1756_SPI_TransferInOutBytes,

    // uEZ v2.02
    LPC1756_SPI_TransferNoBlock,
    LPC1756_SPI_IsBusy,

};

void LPC1756_SPI0_Require(
        T_uezGPIOPortPin aPinSCK0,
        T_uezGPIOPortPin aPinMISO0,
        T_uezGPIOPortPin aPinMOSI0,
        T_uezGPIOPortPin aPinSSEL0)
{
    static const T_LPC1756_IOCON_ConfigList sck0[] = {
            {GPIO_P0_15, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1756_IOCON_ConfigList ssel0[] = {
            {GPIO_P0_16, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1756_IOCON_ConfigList miso0[] = {
            {GPIO_P0_17, IOCON_D_DEFAULT(3)},
    };
    static const T_LPC1756_IOCON_ConfigList mosi0[] = {
            {GPIO_P0_18, IOCON_D_DEFAULT(3)},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register SPI0
    HALInterfaceRegister("SPI0",
            (T_halInterface *)&SPI_LPC1756_Port0_Interface, 0, 0);
    LPC1756_IOCON_ConfigPinOrNone(aPinSCK0, sck0, ARRAY_COUNT(sck0));
    LPC1756_IOCON_ConfigPinOrNone(aPinSSEL0, ssel0, ARRAY_COUNT(ssel0));
    LPC1756_IOCON_ConfigPinOrNone(aPinMISO0, miso0, ARRAY_COUNT(miso0));
    LPC1756_IOCON_ConfigPinOrNone(aPinMOSI0, mosi0, ARRAY_COUNT(mosi0));
    LPC1756PowerOn(1<<21);

    // Setup interrupt, but do not enable
    InterruptRegister(SPI_IRQn, ISPI0IRQ, INTERRUPT_PRIORITY_HIGH, "SPI0");
    InterruptDisable(SPI_IRQn);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_SPI.c
 *-------------------------------------------------------------------------*/

