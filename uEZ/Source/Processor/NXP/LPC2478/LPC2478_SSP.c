/*-------------------------------------------------------------------------*
 * File:  SSP.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the LPC2478 SSP Bus Interface.
 * Implementation:
 *      Two SSPs are created, SSP0 and SSP1.
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
#include "Config.h"
#include <uEZTypes.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <HAL/GPIO.h>
#include <Source/Processor/NXP/LPC2478/LPC2478_SSP.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    TVUInt32 iCR0;  // 0x00
        #define SSP_FRF_SPI (0<<4)      // Frame format: SPI
        #define SSP_FRF_TI  (1<<4)      // Frame format: TI
        #define SSP_FRF_MW  (2<<4)      // Frame format: Microwire
        #define SSP_CPOL    (1<<6)      // Clock Polarity
        #define SSP_CPHA    (1<<7)      // Clock Phase
    TVUInt32 iCR1;  // 0x04
        #define SSP_LBM     (1<<0)      // Loop back mode
        #define SSP_SSE     (1<<1)      // SSP Enable
        #define SSP_MS      (1<<2)      // Master/Slave Mode (master=0)
        #define SSP_SOD     (1<<3)      // Slave Output Disable
    TVUInt32 iDR;   // 0x08
    TVUInt32 iSR;   // 0x0C -- read only
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
    TVUInt32 iRIS;  // 0x18
        #define SSP_RORRIS  (1<<0)      // Raw Receive overrun interrupt
        #define SSP_RTRIS   (1<<1)      // Raw Receive timeout
        #define SSP_RXRIS   (1<<2)      // Raw Rx FIFO is at least half full
        #define SSP_TXRIS   (1<<3)      // Raw Tx FIFO is at least half empty
    TVUInt32 iMIS;  // 0x1C
        #define SSP_RORMIS  (1<<0)      // Mask (1=on) Receive overrun interrupt
        #define SSP_RTMIS   (1<<1)      // Mask (1=on) Receive timeout
        #define SSP_RXMIS   (1<<2)      // Mask (1=on) Rx FIFO is at least half full
        #define SSP_TXMIS   (1<<3)      // Mask (1=on) Tx FIFO is at least half empty
    TVUInt32 iCR;   // 0x20
        #define SSP_RORIC   (1<<0)      // Clears SSP_RORIM interrupt
        #define SSP_RTIC    (1<<1)      // Clears SSP_RTRIS interrupt
    TVUInt32 iDMACR;// 0x24
        #define SSP_RXDMAE  (1<<0)      // When 1, DMA enabled for Rx FIFO
        #define SSP_TXDMAE  (1<<1)      // When 1, DMA enabled for Tx FIFO
} T_LPC2478_SSP_Registers;

typedef struct {
    const HAL_SPI *iHAL;
    T_LPC2478_SSP_Registers *iReg;
} T_LPC2478_SSP_Workspace;

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
 * Routine:  ISSPEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      SSP is going to end -- chip select disabled for device
 * Outputs:
 *      SPI_Request *aRequest      -- Configuration of SSP device
 *---------------------------------------------------------------------------*/
static void ISSPEnd(SPI_Request *aRequest)
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
 * Routine:  ISSPConfig
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the SSP for the particular SSP request.
 * Outputs:
 *      T_LPC2478_SSP_Workspace *aW -- Particular SSP workspace
 *      SPI_Request *aRequest      -- Configuration of SSP device
 *---------------------------------------------------------------------------*/
static void ISSPConfig(T_LPC2478_SSP_Workspace *aW, SPI_Request *aRequest)
{
    T_LPC2478_SSP_Registers *p = aW->iReg;
    TUInt32 scr;

    // Disable for the moment
    p->iCR = 0;

    // Set the CS as an output io pin (after making sure it is in a proper unselected state)
    if (aRequest->iCSPolarity)  {
        (*(aRequest->iCSGPIOPort))->Clear(
                aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    } else {
        (*(aRequest->iCSGPIOPort))->Set(
                aRequest->iCSGPIOPort,
                aRequest->iCSGPIOBit);
    }
    (*(aRequest->iCSGPIOPort))->SetOutputMode(
            aRequest->iCSGPIOPort,
            aRequest->iCSGPIOBit);

    // Calculate the scr divider (rounding up to the next even number)
//    scr = (((((Fpclk/1000)+(aRequest->iRate-1))/(aRequest->iRate)))+1)&0xFFFE;
    scr = (((Fpclk/1000)+(aRequest->iRate-1))/(aRequest->iRate))-1;
    // Minimum of 2 divide factor
    if (scr < 2)
        scr = 2;

    // Make sure the SSP clock is PCLK/2
    p->iCPSR = 2;

    p->iCR0 =
        // Number of bits per transaction
        (aRequest->iBitsPerTransfer-1) |
        // SPI interface only
        SSP_FRF_SPI |
        // Clock out polarity
        (aRequest->iClockOutPolarity?SSP_CPOL:0) |
        (aRequest->iClockOutPhase?SSP_CPHA:0) |
        (scr << 8);

    // Enable the SSP
    p->iCR1 = SSP_SSE;

    // Clear out the SSP buffer
    // First wait for any undone transfers
    while (p->iSR & SSP_BSY)
        {}

    // Now read any waiting data
    while (p->iSR & SSP_RNE)
        p->iDR;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_SSP_TransferPolled
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a SSP transfer (both read and write) out the SSP port using
 *      the given request.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 *      SPI_Request *aRequest       -- Request to perform
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_SSP_TransferPolled(void *aWorkspace, SPI_Request *aRequest)
{
    TUInt32 i;
    T_LPC2478_SSP_Workspace *aW = (T_LPC2478_SSP_Workspace *)aWorkspace;
    T_LPC2478_SSP_Registers *p = aW->iReg;
    TUInt8 *p_dataMOSI8;
    TUInt8 *p_dataMISO8;
    TUInt16 *p_dataMOSI16;
    TUInt16 *p_dataMISO16;
    TVUInt32 dummy;

    // Setup SSP for this configuration
    ISSPConfig(aW, aRequest);

    // Start SSP
    ISSPStart(aRequest);

    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer <= 8)  {
        // Doing bytes
        p_dataMOSI8 = (TUInt8 *)(aRequest->iDataMOSI);
        p_dataMISO8 = (TUInt8 *)(aRequest->iDataMISO);


        for (i=0; i<aRequest->iNumTransfers; i++)  {
            // Wait for SSP to be no longer busy and transmit to be empty
            while ((p->iSR & SSP_BSY) || (!(p->iSR & SSP_TFE)))  {
                // TBD: Polling here instead of interrupt
            }

            if (p_dataMOSI8) {
                // Send the SSP data (this also triggers a read in)
                if ((i+4)<=aRequest->iNumTransfers) {
                    // Do 4 at one time
                    p->iDR = (TUInt32) *(p_dataMOSI8++);
                    p->iDR = (TUInt32) *(p_dataMOSI8++);
                    p->iDR = (TUInt32) *(p_dataMOSI8++);
                    p->iDR = (TUInt32) *(p_dataMOSI8++);
                } else {
                    p->iDR = (TUInt32) *(p_dataMOSI8++);
                }
            } else {
                // Send the SSP data (this also triggers a read in)
                if ((i+4)<=aRequest->iNumTransfers) {
                    // Do 4 at one time
                    p->iDR = (TUInt32) 0xFF;
                    p->iDR = (TUInt32) 0xFF;
                    p->iDR = (TUInt32) 0xFF;
                    p->iDR = (TUInt32) 0xFF;
                } else {
                    p->iDR = (TUInt32) 0xFF;
                }
            }

            // Wait until SSP done  (transmit empty)
            while ((p->iSR & SSP_BSY) || ((p->iSR & SSP_TFE)==0))  {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            if (p_dataMISO8) {
                // Store the SSP received over the data sent
                if ((i+4)<=aRequest->iNumTransfers) {
                    // Do 4 at one time
                    *(p_dataMISO8++) = p->iDR;
                    *(p_dataMISO8++) = p->iDR;
                    *(p_dataMISO8++) = p->iDR;
                    *(p_dataMISO8++) = p->iDR;
                    i+=3;
                } else {
                    *(p_dataMISO8++) = p->iDR;
                }
            } else {
                // Store the SSP received over the data sent
                if ((i+4)<=aRequest->iNumTransfers) {
                    // Do 4 at one time
                    dummy = p->iDR;
                    dummy = p->iDR;
                    dummy = p->iDR;
                    dummy = p->iDR;
                    i+=3;
                } else {
                    dummy = p->iDR;
                }
            }
        }
    } else {
        // Doing words
        p_dataMOSI16 = (TUInt16 *)(aRequest->iDataMOSI);
        p_dataMISO16 = (TUInt16 *)(aRequest->iDataMISO);

        for (i=0; i<aRequest->iNumTransfers; i++)  {
            // Wait for SSP to be no longer busy and transmit to be empty
            while ((p->iSR & SSP_BSY) || (!(p->iSR & SSP_TFE)))  {
                // TBD: Polling here instead of interrupt
            }

            // Send the SSP data (this also triggers a read in)
            if (p_dataMOSI16)
                p->iDR = (TUInt32) *(p_dataMOSI16++);
            else
                p->iDR = 0xFFFF;

            // Wait until SSP done
            while ((p->iSR & SSP_BSY) || ((p->iSR & SSP_TFE)==0))  {
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
 * Routine:  LPC2478_SSP_SSP0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 SSP0 workspace.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_SSP_SSP0_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_SSP_Workspace *p = (T_LPC2478_SSP_Workspace *)aWorkspace;
    p->iReg = (T_LPC2478_SSP_Registers *)SSP0_BASE;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_SSP_SSP1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LPC2478 SSP1 workspace.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC2478_SSP_SSP1_InitializeWorkspace(void *aWorkspace)
{
    T_LPC2478_SSP_Workspace *p = (T_LPC2478_SSP_Workspace *)aWorkspace;
    p->iReg = (T_LPC2478_SSP_Registers *)SSP1_BASE;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_SSP_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Start a SSP transfer by putting the chip select in the active state
 *      after configuring the SSP speed and other settings.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 *      SPI_Request *aRequest       -- SSP request being processed
 *---------------------------------------------------------------------------*/
void LPC2478_SSP_Start(void *aWorkspace, SPI_Request *aRequest)
{
    // Setup SSP for this configuration
    ISSPConfig(aWorkspace, aRequest);
    ISSPStart(aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_SSP_TransferInOut
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
static TUInt32 LPC2478_SSP_TransferInOut(
        void *aWorkspace,
        SPI_Request *aRequest,
        TUInt32 aSend)
{
    TUInt32 recv;
    T_LPC2478_SSP_Workspace *aW = (T_LPC2478_SSP_Workspace *)aWorkspace;
    T_LPC2478_SSP_Registers *p = aW->iReg;

    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer <= 8)  {
        // Send the SSP data (this also triggers a read in)
        p->iDR = aSend;

        // Wait until SSP done
        while (((p->iSR & SSP_TFE)==0) || (p->iSR & SSP_BSY))  {
            // TBD: For polling this is fine, but we may need to yield at some point?
        }

        // Store the SSP received over the data sent
        recv = p->iDR;
    } else {
        // Send the SSP data (this also triggers a read in)
        p->iDR = (TUInt32) *((TUInt16 *)aSend);

        // Wait until SSP done
        while ((p->iSR & SSP_RNE)==0)  {
            // TBD: For polling this is fine, but we may need to yield at some point?
        }

        // Store the SSP received over the data sent
        recv = p->iDR;
    }

    return recv;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_SSP_Stop
 *---------------------------------------------------------------------------*
 * Description:
 *      End a SSP transfer by putting the chip select in the done state.
 * Inputs:
 *      void *aW                    -- Particular SSP workspace
 *      SPI_Request *aRequest       -- SSP request being processed
 *---------------------------------------------------------------------------*/
static void LPC2478_SSP_Stop(void *aWorkspace, SPI_Request *aRequest)
{
    ISSPEnd(aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC2478_SSP_SSP1_InitializeWorkspace
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
static T_uezError LPC2478_SSP_TransferInOutBytes(
        void *aWorkspace,
        SPI_Request *aRequest)
{
    T_LPC2478_SSP_Workspace *aW = (T_LPC2478_SSP_Workspace *)aWorkspace;
    T_LPC2478_SSP_Registers *p = aW->iReg;
    TUInt32 sent;
    TUInt32 count;
    TUInt8 *p_dataMOSI = (TUInt8 *)aRequest->iDataMOSI;
    TUInt8 *p_dataMISO = (TUInt8 *)aRequest->iDataMISO;
    TUInt8 dummy[8];

    // This routine only works with transfer of 8 bits or lower
    if (aRequest->iBitsPerTransfer > 8)
        return UEZ_ERROR_INVALID_PARAMETER;

    sent=0;
    count = aRequest->iNumTransfers;

    // Send data while there is data to send
    while (sent < count) {
        if (!aRequest->iDataMISO)
            p_dataMISO = dummy;

        // Always send in groups of 8 bytes if there are 8 bytes to send
        if ((sent+8) <= count) {
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
            while (((p->iSR & SSP_TFE)==0) || (p->iSR & SSP_BSY))  {
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
            while (((p->iSR & SSP_TFE)==0) || (p->iSR & SSP_BSY))  {
                // TBD: For polling this is fine, but we may need to yield at some point?
            }

            // Receive 1
            *(p_dataMISO++) = p->iDR;

            sent++;
        }
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC2478_SSP0_Require(
        T_uezGPIOPortPin aPinSCK0,
        T_uezGPIOPortPin aPinMISO0,
        T_uezGPIOPortPin aPinMOSI0,
        T_uezGPIOPortPin aPinSSEL0)
{
    static const T_LPC2478_PINSEL_ConfigList sck0[] = {
            {GPIO_P0_15, 2},
            {GPIO_P1_20, 3},
            {GPIO_P2_22, 3},
    };
    static const T_LPC2478_PINSEL_ConfigList ssel0[] = {
            {GPIO_P0_16, 2},
            {GPIO_P1_21, 3},
            {GPIO_P2_23, 3},
    };
    static const T_LPC2478_PINSEL_ConfigList miso0[] = {
            {GPIO_P0_17, 2},
            {GPIO_P1_23, 3},
            {GPIO_P2_26, 3},
    };
    static const T_LPC2478_PINSEL_ConfigList mosi0[] = {
            {GPIO_P0_18, 2},
            {GPIO_P1_24, 3},
            {GPIO_P2_27, 3},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register SSP0
    HALInterfaceRegister("SSP0",
            (T_halInterface *)&SSP_LPC2478_Port0_Interface, 0, 0);
    LPC2478_PINSEL_ConfigPinOrNone(aPinSCK0, sck0, ARRAY_COUNT(sck0));
    LPC2478_PINSEL_ConfigPinOrNone(aPinSSEL0, ssel0, ARRAY_COUNT(ssel0));
    LPC2478_PINSEL_ConfigPinOrNone(aPinMISO0, miso0, ARRAY_COUNT(miso0));
    LPC2478_PINSEL_ConfigPinOrNone(aPinMOSI0, mosi0, ARRAY_COUNT(mosi0));
}

void LPC2478_SSP1_Require(
        T_uezGPIOPortPin aPinSCK1,
        T_uezGPIOPortPin aPinMISO1,
        T_uezGPIOPortPin aPinMOSI1,
        T_uezGPIOPortPin aPinSSEL1)
{
    static const T_LPC2478_PINSEL_ConfigList sck1[] = {
            {GPIO_P0_7,  2},
            {GPIO_P1_31, 2},
            {GPIO_P4_20, 3},
    };
    static const T_LPC2478_PINSEL_ConfigList ssel1[] = {
            {GPIO_P0_6,  2},
            {GPIO_P0_14, 3},
            {GPIO_P4_21, 3},
    };
    static const T_LPC2478_PINSEL_ConfigList miso1[] = {
            {GPIO_P0_8,  2},
            {GPIO_P0_12, 2},
            {GPIO_P4_22, 3},
    };
    static const T_LPC2478_PINSEL_ConfigList mosi1[] = {
            {GPIO_P0_9,  2},
            {GPIO_P0_13, 2},
            {GPIO_P4_23, 3},
    };

    HAL_DEVICE_REQUIRE_ONCE();
    // Register SSP1
    HALInterfaceRegister("SSP1",
            (T_halInterface *)&SSP_LPC2478_Port1_Interface, 0, 0);
    LPC2478_PINSEL_ConfigPinOrNone(aPinSCK1, sck1, ARRAY_COUNT(sck1));
    LPC2478_PINSEL_ConfigPinOrNone(aPinSSEL1, ssel1, ARRAY_COUNT(ssel1));
    LPC2478_PINSEL_ConfigPinOrNone(aPinMISO1, miso1, ARRAY_COUNT(miso1));
    LPC2478_PINSEL_ConfigPinOrNone(aPinMOSI1, mosi1, ARRAY_COUNT(mosi1));
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_SPI SSP_LPC2478_Port0_Interface = {
    "NXP:LPC2478:SSP0",
    0x0100,
    LPC2478_SSP_SSP0_InitializeWorkspace,
    sizeof(T_LPC2478_SSP_Workspace),

    LPC2478_SSP_TransferPolled,

    LPC2478_SSP_Start,
    LPC2478_SSP_TransferInOut,
    LPC2478_SSP_Stop,
    LPC2478_SSP_TransferInOutBytes,
};

const HAL_SPI SSP_LPC2478_Port1_Interface = {
    "NXP:LPC2478:SSP1",
    0x0100,
    LPC2478_SSP_SSP1_InitializeWorkspace,
    sizeof(T_LPC2478_SSP_Workspace),

    LPC2478_SSP_TransferPolled,

    LPC2478_SSP_Start,
    LPC2478_SSP_TransferInOut,
    LPC2478_SSP_Stop,
    LPC2478_SSP_TransferInOutBytes,
};

/*-------------------------------------------------------------------------*
 * End of File:  SSP.c
 *-------------------------------------------------------------------------*/

