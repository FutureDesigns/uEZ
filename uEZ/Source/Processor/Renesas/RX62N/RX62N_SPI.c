/*-------------------------------------------------------------------------*
 * File:  RX62N_SPI.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the RX62N SPI Bus Interface.
 * Implementation:
 *      The RSPI0 and RSPI1 interface of the RX62N are mapped to standard
 *      SPI request handlers.
 *      Master only.
 *      Currently, only polled.
 *      This current implementation does NOT use the SSL lines for CS
 *      or slave selection.  It only uses a GPIO line defined in the
 *      configuration.
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
#include <uEZ.h>
#include <uEZTypes.h>
#include <uEZProcessor.h>
#include <HAL/SPI.h>
#include <HAL/GPIO.h>
#include "RX62N_UtilityFuncs.h"
#include "RX62N_SPI.h"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
#if (COMPILER_TYPE==RenesasRX)
    typedef volatile struct st_rspi __evenaccess T_RX62N_SPI_Registers;
#endif

typedef struct {
    const HAL_SPI *iHAL;
    T_RX62N_SPI_Registers *iReg;
    TUInt8 iMSTPBit;
} T_RX62N_SPI_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static const TUInt8 G_bitMapping[32+1] = {
    0xFF,           // 0 bits
    0xFF,           // 1 bit
    0xFF,           // 2 bit
    0xFF,           // 3 bit
    0xFF,           // 4 bit
    0xFF,           // 5 bit
    0xFF,           // 6 bit
    0xFF,           // 7 bit
    7,              // 8 bits
    8,              // 9 bits
    9,              // 10 bits
    10,             // 11 bits
    11,             // 12 bits
    12,             // 13 bits
    13,             // 14 bits
    14,             // 15 bits
    15,             // 16 bits
    0xFF,           // 17 bits
    0xFF,           // 18 bits
    0xFF,           // 19 bits
    0,              // 20 bits
    0xFF,           // 21 bits
    0xFF,           // 22 bits
    0xFF,           // 23 bits
    1,              // 24 bits
    0xFF,           // 25 bits
    0xFF,           // 26 bits
    0xFF,           // 27 bits
    0xFF,           // 28 bits
    0xFF,           // 29 bits
    0xFF,           // 30 bits
    0xFF,           // 31 bits
    2,              // 32 bits
};

static const SPI_Request G_defaultSPIRequest = {
    0, // iDataMOSI
    0, // iDataMISO
    0, // iNumTransfers
    8, // iBitsPerTransfer (8 bits default)
    1000, // iRate (1000 kHz or 1 MHz default)
    EFalse, // iClockOutPolarity
    EFalse, // iClockOutPhase
    0, // iCSGPIOPort
    0, // iCSGPIOBit
    EFalse, // iCSPolarity
};

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
 *      T_RX62N_SPI_Workspace *aW -- Particular SPI workspace
 *      SPI_Request *aRequest      -- Configuration of SPI device
 *---------------------------------------------------------------------------*/
static void ISPIConfig(T_RX62N_SPI_Workspace *aW, SPI_Request *aRequest)
{
    T_RX62N_SPI_Registers *p = aW->iReg;

    // Ensure the SPI is on
    SYSTEM.MSTPCRB.LONG &= ~(1<<aW->iMSTPBit);

    // Disable/reset RSPI for the moment
    p->SPCR.BIT.SPE = 0;

    if (aRequest->iCSGPIOPort) {
        // Set the CS as an output io pin
        (*(aRequest->iCSGPIOPort))->SetOutputMode(
                aRequest->iCSGPIOPort, 
                aRequest->iCSGPIOBit);
    }
    
    // Not loopback and using CMOS output
    p->SPPCR.BYTE = 0x00;

    // Calculate the bit rate divider (assuming the BRDV[1:0] values are always 0)
    // TBD: At some point we should calculate BRDV[1:0] for the commands
    //      and use this value.  For now this limits the range of speeds
    //      to higher speeds.  (e.g. 32 MHz / 256 = 125 kHz as slowest rate)
    if ((2*aRequest->iRate) >= (BCLK_FREQUENCY/1000))
        p->SPBR.BYTE = 0; // maximum speed
    else
        p->SPBR.BYTE = ((BCLK_FREQUENCY/1000)-1)/(2*aRequest->iRate);

    // RSPI Data Control Register (SPDCR)
    //  00-- ----   Reserved
    //  --0- ----   SPLW = Access SPDR in long words
    //  ---0 ----   SPRDTD = SPDR values are read from the receive buffer
    //  ---- 00--   SLSEL[1:0] = SSL[3:0] are outputs (not used)
    //  ---- --00   SPFC[1:0] = These bits specify the number of frames that
    //              can be stored in SPDR.  For, this configuration:
    //              1 transfer per access, 1 buffered
    p->SPDCR.BYTE = 0x00;

    // RSPI Clock Delay Register (SPCKD)
    //  0000 0---   Reserved
    //  ---- -abc   SCKDL[2:0] = 1 RSPCK + abc (0+1=1 in this case)
    p->SPCKD.BYTE = 0x00;

    // RSPI Slave Select Negation Delay Register (SSLND)
    //  0000 0---   Reserved
    //  ---- -abc   SLNDL[2:0] = 1 RSPCK + abc (0+1=1 in this case)
    p->SSLND.BYTE = 0x00;

    // RSPI Next-Access Delay Register (SPND)
    //  0000 0---   Reserved
    //  ---- -abc   SPNDL[2:0] = 1 RSPCK + abc + 2 PCLK
    //              (0+1=1 RSPCK + 2 PCLK in this case)
    p->SPND.BYTE = 0x00;

    // SPCR2: RSPI Control Register 2
    //  0000 ----   Reserved
    //  ---- 0---   PTE = Disables the self-diagnosis function of the parity
    //                  circuit
    //  ---- -0--   SPIIE = Disables the generation of idle interrupt requests
    //  ---- --0-   SPOE = Selects even parity for use in transmission and
    //                  reception (not used)
    //  ---- ---0   SPPE = Does not add the parity bit to transmit data and
    //                  does not check the parity bit of receive data
    p->SPCR2.BYTE = 0x00;

    // SPCMD0: RSPI Command Register 0
    //  0--- ---- ---- ---- SCKDEN = An RSPCK delay of 1 RSPCK
    //  -0-- ---- ---- ---- SLNDEN = An SSL negation delay of 1 RSPCK
    //  --0- ---- ---- ---- SPNDEN = A next-access delay of 1 RSPCK + 2 PCLK
    //  ---0 ---- ---- ---- LSBF = MSB first
    //  ---- xxxx ---- ---- SPB[3:0] = Number of bits per transfer
    //                          0:      20 bits,
    //                          1:      24 bits,
    //                          2-3:    32 bits
    //                          4-7:    8 bits,
    //                          8-15:   9 to 16 bits,
    //  ---- ---- 0--- ---- SSLKP = Negates all SSL signals upon completion of
    //                          transfer (not used)
    //  ---- ---- -000 ---- SSLA[2:0]:
    //                          0:      SSL0
    //                          1:      SSL1
    //                          2:      SSL2
    //                          3:      SSL3
    //                          4-7:    Prohibited
    //  ---- ---- ---- 00-- BRDV[1:0]: These bits select the base bit rate
    //  ---- ---- ---- --x- CPOL = RSPCK Polarity Setting
    //                          0:      RSPCK=0 when idle
    //                          1:      RSPCK=1 when idle
    //  ---- ---- ---- ---x CPHA = RSPCK Phase Setting
    //                          0:      Data sampling on odd edge, data
    //                                  variation on even edge
    //                          1:      Data variation on odd edge, data
    //                                  sampling on even edge
    if (aRequest->iBitsPerTransfer<32)
        p->SPCMD0.WORD =
            (G_bitMapping[aRequest->iBitsPerTransfer]<<8) |
            (aRequest->iClockOutPhase?(1<<0):0) |
            (aRequest->iClockOutPolarity?(1<<1):0);
    else
        p->SPCMD0.WORD = 0x0700; // 8 bit default

    // SPCR: RSPI Control Register
    //  0--- ----   SPRIE = Disables the generation of RSPI receive interrupt
    //                  requests
    //  -1-- ----   SPE = Enables the RSPI function
    //  --0- ----   SPTIE = Disables the generation of RSPI transmit interrupt
    //                  requests
    //  ---0 ----   SPEIE = Disables the generation of RSPI error interrupt
    //                  requests
    //  ---- 1---   MSTR = Master mode
    //  ---- -0--   MODFEN = Disables the detection of mode fault error
    //  ---- --0-   TXMD = Full-duplex synchronous serial communications
    //  ---- ---1   SPMS = SPI operation (3-wire method)
    p->SPCR.BYTE = 0x69; // 0x49 ;

    // SSLP: RSPI Slave Select Polarity Register (SSLP)
    //  0000 ----   Reserved
    //  ---- 0---   SSL3P = SSL3 signal is 0-active (not used)
    //  ---- -0--   SSL2P = SSL2 signal is 0-active (not used)
    //  ---- --0-   SSL1P = SSL1 signal is 0-active (not used)
    //  ---- ---0   SSL0P = SSL0 signal is 0-active (not used)
    p->SSLP.BYTE = 0x00;

    // RSPI Sequence Control Register (SPSCR)
    //  0000 0---   Reserved
    //  ---- -000   SPSLN[2:0]
    //                  000: 1 0 -> 0 -> ...
    //                  001: 1 0 -> 1 -> 0 -> ...
    //                  ...
    //                  111: 1 0 -> 1 -> 2 -> 3 -> 4 -> 5 -> 6 -> 7 -> 0 -> ...
    p->SPSCR.BYTE = 0x00;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_SPI_TransferPolled
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
T_uezError RX62N_SPI_TransferPolled(void *aWorkspace, SPI_Request *aRequest)
{
    TUInt32 i;
    T_RX62N_SPI_Workspace *aW = (T_RX62N_SPI_Workspace *)aWorkspace;
    T_RX62N_SPI_Registers *p = aW->iReg;
    TUInt8 *p_dataMOSI8;
    TUInt8 *p_dataMISO8;
    TUInt16 *p_dataMOSI16;
    TUInt16 *p_dataMISO16;
    TUInt32 *p_dataMOSI32;
    TUInt32 *p_dataMISO32;
    volatile TUInt32 dummy;
    TUInt32 send;

    // Setup SPI for this configuration
    ISPIConfig(aW, aRequest);

    // Start SPI
    ISPIStart(aRequest);

    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer <= 8)  {
        // Doing 8-bit bytes
        p_dataMOSI8 = (TUInt8 *)(aRequest->iDataMOSI);
        p_dataMISO8 = (TUInt8 *)(aRequest->iDataMISO);

        if (p_dataMOSI8) {
            if (p_dataMISO8) {
                for (i=0; i<aRequest->iNumTransfers; i++)  {
                    // Send the SPI data (this also triggers a read in)
                    send = (TUInt32)*(p_dataMOSI8++);
                    p->SPDR.WORD.L = (TUInt16) (send >> 16);
                    p->SPDR.WORD.H = (TUInt16) (send & 0xFFFF);

                    // Wait until SPI done 
                    while (p->SPSR.BIT.IDLNF);

                    // Store the SPI received over the data sent
                    *(p_dataMISO8++) = p->SPDR.LONG;
                }
            } else {
                for (i=0; i<aRequest->iNumTransfers; i++)  {
                    // Send the SPI data (this also triggers a read in)
                    send = (TUInt32)*(p_dataMOSI8++);
                    p->SPDR.WORD.L = (TUInt16) (send >> 16);
                    p->SPDR.WORD.H = (TUInt16) (send & 0xFFFF);

                    // Wait until SPI done 
                    while (p->SPSR.BIT.IDLNF);

                    // Store the SPI received over the data sent
                    dummy = p->SPDR.LONG;
                }
            }
        } else {
            if (p_dataMISO8) {
                for (i=0; i<aRequest->iNumTransfers; i++)  {
                    // Send the SPI data (this also triggers a read in)
                    p->SPDR.WORD.L = 0xFFFF;
                    p->SPDR.WORD.H = 0xFFFF;

                    // Wait until SPI done 
                    while (p->SPSR.BIT.IDLNF);

                    // Store the SPI received over the data sent
                    *(p_dataMISO8++) = p->SPDR.LONG;
                }
            } else {
                for (i=0; i<aRequest->iNumTransfers; i++)  {
                    // Send the SPI data (this also triggers a read in)
                    p->SPDR.WORD.L = 0xFFFF;
                    p->SPDR.WORD.H = 0xFFFF;

                    // Wait until SPI done 
                    while (p->SPSR.BIT.IDLNF);

                    // Store the SPI received over the data sent
                    dummy = p->SPDR.LONG;
                }
            }
        }
    } else if (aRequest->iBitsPerTransfer <= 16) {
        // Doing 16-bit words
        p_dataMOSI16 = (TUInt16 *)(aRequest->iDataMOSI);
        p_dataMISO16 = (TUInt16 *)(aRequest->iDataMISO);

        for (i=0; i<aRequest->iNumTransfers; i++)  {
            // Send the SPI data (this also triggers a read in)
            if (p_dataMOSI16) {
                send = (TUInt32)*(p_dataMOSI16++);
                p->SPDR.WORD.L = (TUInt16) (send >> 16);
                p->SPDR.WORD.H = (TUInt16) (send & 0xFFFF);
            } else {
                p->SPDR.WORD.L = 0xFFFF;
                p->SPDR.WORD.H = 0xFFFF;
            }
            // Wait until SPI done 
            while (p->SPSR.BIT.IDLNF);

            // Store the SPI received over the data sent
            if (p_dataMISO16)
                *(p_dataMISO16++) = p->SPDR.LONG;
            else
                dummy = p->SPDR.LONG;
        }
    } else {
        // Doing 32-bit words
        p_dataMOSI32 = (TUInt32 *)(aRequest->iDataMOSI);
        p_dataMISO32 = (TUInt32 *)(aRequest->iDataMISO);

        for (i=0; i<aRequest->iNumTransfers; i++)  {
            // Send the SPI data (this also triggers a read in)
            if (p_dataMOSI32) {
                send = (TUInt32)*(p_dataMOSI32++);
                p->SPDR.WORD.L = (TUInt16) (send >> 16);
                p->SPDR.WORD.H = (TUInt16) (send & 0xFFFF);
            } else {
                p->SPDR.WORD.L = 0xFFFF;
                p->SPDR.WORD.H = 0xFFFF;
            }

            // Wait until SPI done
            while (p->SPSR.BIT.IDLNF);

            // Store the SPI received over the data sent
            if (p_dataMISO32)
                *(p_dataMISO32++) = p->SPDR.LONG;
            else
                dummy = p->SPDR.LONG;
        }
    }

    // End SPI
    ISPIEnd(aRequest);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_SPI_TransferInOutBytes
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
T_uezError RX62N_SPI_TransferInOutBytes(void *aWorkspace, SPI_Request *aRequest)
{
    TUInt32 i;
    T_RX62N_SPI_Workspace *aW = (T_RX62N_SPI_Workspace *)aWorkspace;
    T_RX62N_SPI_Registers *p = aW->iReg;
    TUInt8 *p_dataMOSI8;
    TUInt8 *p_dataMISO8;
    TUInt16 *p_dataMOSI16;
    TUInt16 *p_dataMISO16;
    TUInt32 *p_dataMOSI32;
    TUInt32 *p_dataMISO32;
    volatile TUInt32 dummy;
    TUInt32 send;

    ISPIConfig(aWorkspace, aRequest);
    
    // Are we transferring byte sized or word sized?
    if (aRequest->iBitsPerTransfer <= 8)  {
        // Doing 8-bit bytes
        p_dataMOSI8 = (TUInt8 *)(aRequest->iDataMOSI);
        p_dataMISO8 = (TUInt8 *)(aRequest->iDataMISO);

        if (p_dataMOSI8) {
            if (p_dataMISO8) {
                for (i=0; i<aRequest->iNumTransfers; i++)  {
                    // Send the SPI data (this also triggers a read in)
                    send = (TUInt32)*(p_dataMOSI8++);
                    p->SPDR.WORD.L = (TUInt16) (send >> 16);
                    p->SPDR.WORD.H = (TUInt16) (send & 0xFFFF);

                    // Wait until SPI done 
                    while (p->SPSR.BIT.IDLNF);

                    // Store the SPI received over the data sent
                    *(p_dataMISO8++) = p->SPDR.LONG;
                }
            } else {
                for (i=0; i<aRequest->iNumTransfers; i++)  {
                    // Send the SPI data (this also triggers a read in)
                    send = (TUInt32)*(p_dataMOSI8++);
                    p->SPDR.WORD.L = (TUInt16) (send >> 16);
                    p->SPDR.WORD.H = (TUInt16) (send & 0xFFFF);

                    // Wait until SPI done 
                    while (p->SPSR.BIT.IDLNF);

                    // Store the SPI received over the data sent
                    dummy = p->SPDR.LONG;
                }
            }
        } else {
            if (p_dataMISO8) {
                for (i=0; i<aRequest->iNumTransfers; i++)  {
                    // Send the SPI data (this also triggers a read in)
                    p->SPDR.WORD.L = 0xFFFF;
                    p->SPDR.WORD.H = 0xFFFF;

                    // Wait until SPI done 
                    while (p->SPSR.BIT.IDLNF);

                    // Store the SPI received over the data sent
                    *(p_dataMISO8++) = p->SPDR.LONG;
                }
            } else {
                for (i=0; i<aRequest->iNumTransfers; i++)  {
                    // Send the SPI data (this also triggers a read in)
                    p->SPDR.WORD.L = 0xFFFF;
                    p->SPDR.WORD.H = 0xFFFF;

                    // Wait until SPI done 
                    while (p->SPSR.BIT.IDLNF);

                    // Store the SPI received over the data sent
                    dummy = p->SPDR.LONG;
                }
            }
        }
    } else if (aRequest->iBitsPerTransfer <= 16) {
        // Doing 16-bit words
        p_dataMOSI16 = (TUInt16 *)(aRequest->iDataMOSI);
        p_dataMISO16 = (TUInt16 *)(aRequest->iDataMISO);

        for (i=0; i<aRequest->iNumTransfers; i++)  {
            // Send the SPI data (this also triggers a read in)
            if (p_dataMOSI16) {
                send = (TUInt32)*(p_dataMOSI16++);
                p->SPDR.WORD.L = (TUInt16) (send >> 16);
                p->SPDR.WORD.H = (TUInt16) (send & 0xFFFF);
            } else {
                p->SPDR.WORD.L = 0xFFFF;
                p->SPDR.WORD.H = 0xFFFF;
            }
            // Wait until SPI done 
            while (p->SPSR.BIT.IDLNF);

            // Store the SPI received over the data sent
            if (p_dataMISO16)
                *(p_dataMISO16++) = p->SPDR.LONG;
            else
                dummy = p->SPDR.LONG;
        }
    } else {
        // Doing 32-bit words
        p_dataMOSI32 = (TUInt32 *)(aRequest->iDataMOSI);
        p_dataMISO32 = (TUInt32 *)(aRequest->iDataMISO);

        for (i=0; i<aRequest->iNumTransfers; i++)  {
            // Send the SPI data (this also triggers a read in)
            if (p_dataMOSI32) {
                send = (TUInt32)*(p_dataMOSI32++);
                p->SPDR.WORD.L = (TUInt16) (send >> 16);
                p->SPDR.WORD.H = (TUInt16) (send & 0xFFFF);
            } else {
                p->SPDR.WORD.L = 0xFFFF;
                p->SPDR.WORD.H = 0xFFFF;
            }

            // Wait until SPI done
            while (p->SPSR.BIT.IDLNF);

            // Store the SPI received over the data sent
            if (p_dataMISO32)
                *(p_dataMISO32++) = p->SPDR.LONG;
            else
                dummy = p->SPDR.LONG;
        }
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_SPI_RSPI0_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N SPI0 workspace.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_SPI_RSPI0_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_SPI_Workspace *p = (T_RX62N_SPI_Workspace *)aWorkspace;

    p->iReg = &RSPI0;
    p->iMSTPBit = 17;
    ISPIConfig(aWorkspace, (SPI_Request *)&G_defaultSPIRequest);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_SPI_RSPI1_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RX62N SPI1 workspace.
 * Inputs:
 *      void *aWorkspace            -- Particular SPI workspace
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_SPI_RSPI1_InitializeWorkspace(void *aWorkspace)
{
    T_RX62N_SPI_Workspace *p = (T_RX62N_SPI_Workspace *)aWorkspace;

    p->iReg = &RSPI1;
    p->iMSTPBit = 16;
    ISPIConfig(aWorkspace, (SPI_Request *)&G_defaultSPIRequest);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_SPI_Start
 *---------------------------------------------------------------------------*
 * Description:
 *      Start a SPI transfer by configuring for the speed and settings
 *      and then activating the chip select.
 * Inputs:
 *      void *aWorkspace            -- Particular SPI workspace
 *      SPI_Request *aRequest       -- Request settings to process
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
void RX62N_SPI_Start(void *aWorkspace, SPI_Request *aRequest)
{
    // Setup SPI for this configuration
    ISPIConfig(aWorkspace, aRequest);
    ISPIStart(aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_SPI_TransferInOut
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a SPI transfer (that has already been started) and transfer
 *      a request full of data.
 * Inputs:
 *      void *aWorkspace            -- Particular SPI workspace
 *      SPI_Request *aRequest       -- Request settings to process
 *      TUInt32 aSend               -- Data on the send side
 * Outputs:
 *      TUInt32                     -- Returned data
 *---------------------------------------------------------------------------*/
TUInt32 RX62N_SPI_TransferInOut(
        void *aWorkspace, 
        SPI_Request *aRequest, 
        TUInt32 aSend)
{
    TUInt32 recv;
    T_RX62N_SPI_Workspace *aW = (T_RX62N_SPI_Workspace *)aWorkspace;
    T_RX62N_SPI_Registers *p = aW->iReg;

    // Doing one transfer
    // Send the SPI data (this also triggers a read in)
    p->SPDR.WORD.L = (TUInt16) (aSend >> 16);
    p->SPDR.WORD.H = (TUInt16) (aSend & 0xFFFF);

    // Wait until SPI done
    while (p->SPSR.BIT.IDLNF);

    recv = p->SPDR.LONG;

    return recv;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_SPI_Stop
 *---------------------------------------------------------------------------*
 * Description:
 *      The SPI transfer(s) are complete.  End the SPI by flipping
 *      the chip select.
 * Inputs:
 *      void *aWorkspace            -- Particular SPI workspace
 *      SPI_Request *aRequest       -- Request settings to process
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
void RX62N_SPI_Stop(
        void *aWorkspace,
        SPI_Request *aRequest)
{
    ISPIEnd(aRequest);
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const HAL_SPI SPI_RX62N_RSPI0_Interface = {
    "RX62N:SPI0",
    0x0100,
    RX62N_SPI_RSPI0_InitializeWorkspace,
    sizeof(T_RX62N_SPI_Workspace),

    RX62N_SPI_TransferPolled,

    RX62N_SPI_Start,
    RX62N_SPI_TransferInOut,
    RX62N_SPI_Stop,
    RX62N_SPI_TransferInOutBytes,
};

const HAL_SPI SPI_RX62N_RSPI1_Interface = {
    "RX62N:SPI1",
    0x0100,
    RX62N_SPI_RSPI1_InitializeWorkspace,
    sizeof(T_RX62N_SPI_Workspace),

    RX62N_SPI_TransferPolled,

    RX62N_SPI_Start,
    RX62N_SPI_TransferInOut,
    RX62N_SPI_Stop,
    RX62N_SPI_TransferInOutBytes,
};

void RX62N_RSPI0_A_Require(void)
{
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_PC5,  	// RSPCKA-A
			GPIO_PC6,	// MOSIA-A
			GPIO_PC7,	// MISOA-A
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("RSPI0", (T_halInterface *)&SPI_RX62N_RSPI0_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFGSPI.BYTE = 0x0E;	// Select & enable RSPI0-A pins
	PORTC.ICR.BIT.B7 = 1; 		// Turn on PC7 (MISOA-A) input buffer 
}

void RX62N_RSPI0_B_Require(void)
{
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_PA5,  	// RSPICKA-B
			GPIO_PA6,	// MOSIA-B
			GPIO_PA7	// MISOA-B
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("RSPI0", (T_halInterface *)&SPI_RX62N_RSPI0_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFGSPI.BYTE = 0x0F;	// Select & enable RSPI0-B pins
	PORTA.ICR.BIT.B7 = 1; 		// Turn on PA7 (MISOA-A) input buffer 
}

void RX62N_RSPI1_A_Require(void)
{
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_P27,  	// RSPCKB-A
			GPIO_P26,	// MOSIB-A
			GPIO_P30	// MISOB-A
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("RSPI1", (T_halInterface *)&SPI_RX62N_RSPI1_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));

	IOPORT.PFHSPI.BYTE = 0x0E;		// Select & enable RSPI1-A pins
	PORT3.ICR.BIT.B0 = 1; 			// Turn on P30 (MISOB-A) input buffer 
}

void RX62N_RSPI1_B_Require(void)
{
	static const T_uezGPIOPortPin pins[] = {
           	GPIO_PE5,  	// RSPCKB-B
			GPIO_PE6,	// MOSIB-B
			GPIO_PE7	// MISOB-B
    };
    HAL_DEVICE_REQUIRE_ONCE();
    HALInterfaceRegister("RSPI1", (T_halInterface *)&SPI_RX62N_RSPI1_Interface, 0, 0);
	RX62N_PinsLock(pins, ARRAY_COUNT(pins));
	
	IOPORT.PFHSPI.BYTE = 0x0F;		// Select & enable RSPI1-B pins
	PORTE.ICR.BIT.B7 = 1; 			// Turn on PE7 (MISOB-B) input buffer 
}

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_SPI.c
 *-------------------------------------------------------------------------*/

