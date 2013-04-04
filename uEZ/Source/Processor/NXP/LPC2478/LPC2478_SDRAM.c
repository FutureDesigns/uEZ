/*-------------------------------------------------------------------------*
 * File:  LPC2478_SDRAM.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZBSP.h>
#include <stdint.h>
#include "LPC2478_ExternalBus.h"
#include "LPC2478_SDRAM.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
TUInt32 G_SDRAMAddress = 0;
TUInt32 G_SDRAMSize = 0;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

void LPC2478_SDRAM_Init_32BitBus(const T_LPC2478_SDRAM_Configuration * aConfig)
{
    TUInt32 dynconfig;
    volatile unsigned *dc;
    volatile TUInt32 dmy = 0;
    volatile unsigned int i;

    G_SDRAMSize = aConfig->iSize;
    G_SDRAMAddress = aConfig->iBaseAddress;

    // Reset EMC
    SCS     &= ~0x00000002;

    // Turn on EMC PCLK (if not already on)
    PCONP   |= 0x00000800;
    EMCControl = 0x00000001;

    // Setup 32-bit data bus
    LPC2478_ExternalDataBus_ConfigureIOPins(32);

    // Setup A0 .. A14 for SDRAM
    // TODO: Note: A11 and A12 are included regardless if they are used
    LPC2478_ExternalAddressBus_ConfigureIOPins(15);

    // Configure control pins (we really only care for bus lane 0)
    /* OEN @ P4.24 */
    PINSEL9 &= ~PINSEL_MASK(24);
    PINSEL9 |= PINSEL_MUX(24, 1);
    /* WEN @ P4.25 */
    PINSEL9 &= ~PINSEL_MASK(25);
    PINSEL9 |= PINSEL_MUX(25, 1);

    if (aConfig->iSize == (8 * 1024 * 1024)) {
        dynconfig = 0x00005300;
    } else if (aConfig->iSize == (16 * 1024 * 1024)) {
        dynconfig = 0x00005500;
    } else if (aConfig->iSize == (32 * 1024 * 1024)) {
        dynconfig = 0x00005480;
    } else {
        // "Unsupported UEZBSP_SDRAM_SIZE!"
        UEZFailureMsg("SDRAM Init");
    }

    if (aConfig->iBaseAddress == 0xA0000000) {
        /* CS0n @ P4.30 */
        PINSEL9 &= ~PINSEL_MASK(30);
        PINSEL9 |= PINSEL_MUX(30, 1);

        /* DYCSN[0] @ P2.20 */
        PINSEL5 &= ~PINSEL_MASK(20);
        PINSEL5 |= PINSEL_MUX(20, 1);

        /* CKEOUT0 @ P2.24 */
        PINSEL5 &= ~PINSEL_MASK(24);
        PINSEL5 |= PINSEL_MUX(24, 1);

        dc = &EMCDynamicConfig0;
        EMCDynamicConfig0 = dynconfig;
    } else if (aConfig->iBaseAddress == 0xB0000000) {
        /* CS1n @ P4.31 */
        PINSEL9 &= ~PINSEL_MASK(31);
        PINSEL9 |= PINSEL_MUX(31, 1);

        /* DYCSN[1] @ P2.21 */
        PINSEL5 &= ~PINSEL_MASK(21);
        PINSEL5 |= PINSEL_MUX(21, 1);

        /* CKEOUT1 @ P2.25 */
        PINSEL5 &= ~PINSEL_MASK(25);
        PINSEL5 |= PINSEL_MUX(25, 1);

        dc = &EMCDynamicConfig1;
        EMCDynamicConfig1 = dynconfig;
    } else if (aConfig->iBaseAddress == 0xC0000000) {
        /* CS2n @ P2.14 */
        PINSEL4 &= ~PINSEL_MASK(14);
        PINSEL4 |= PINSEL_MUX(14, 1);

        /* DYCSN[1] @ P2.22 */
        PINSEL5 &= ~PINSEL_MASK(22);
        PINSEL5 |= PINSEL_MUX(22, 1);

        /* CKEOUT2 @ P2.26 */
        PINSEL5 &= ~PINSEL_MASK(26);
        PINSEL5 |= PINSEL_MUX(26, 1);

        dc = &EMCDynamicConfig2;
        EMCDynamicConfig2 = dynconfig;
    } else if (aConfig->iBaseAddress == 0xD0000000) {
        /* CS3n @ P2.15 */
        PINSEL4 &= ~PINSEL_MASK(15);
        PINSEL4 |= PINSEL_MUX(15, 1);

        /* DYCSN[3] @ P2.23 */
        PINSEL5 &= ~PINSEL_MASK(23);
        PINSEL5 |= PINSEL_MUX(23, 1);

        /* CKEOUT2 @ P2.27 */
        PINSEL5 &= ~PINSEL_MASK(27);
        PINSEL5 |= PINSEL_MUX(27, 1);

        dc = &EMCDynamicConfig3;
        EMCDynamicConfig3 = dynconfig;
    }

    /* CASN @ P2.16 */
    PINSEL5 &= ~PINSEL_MASK(16);
    PINSEL5 |= PINSEL_MUX(16, 1);

    /* RASN @ P2.17 */
    PINSEL5 &= ~PINSEL_MASK(17);
    PINSEL5 |= PINSEL_MUX(17, 1);

    if (aConfig->iClockOut == 0) {
        /* CLKOUT0 @ P2.18 */
        PINSEL5 &= ~PINSEL_MASK(18);
        PINSEL5 |= PINSEL_MUX(18, 1);
    } else {
        /* CLKOUT1 @ P2.19 */
        PINSEL5 &= ~PINSEL_MASK(19);
        PINSEL5 |= PINSEL_MUX(19, 1);
    }

    // Setup for 32-bit bus
    /* DQM[0] @ P2.28 */
    PINSEL5 &= ~PINSEL_MASK(28);
    PINSEL5 |= PINSEL_MUX(28, 1);
    /* DQM[1] @ P2.29 */
    PINSEL5 &= ~PINSEL_MASK(29);
    PINSEL5 |= PINSEL_MUX(29, 1);
    /* DQM[2] @ P2.30 */
    PINSEL5 &= ~PINSEL_MASK(30);
    PINSEL5 |= PINSEL_MUX(30, 1);
    /* DQM[3] @ P2.31 */
    PINSEL5 &= ~PINSEL_MASK(31);
    PINSEL5 |= PINSEL_MUX(31, 1);

    // RAS=2, CAS=2
    EMCDynamicReadConfig = 1;
    EMCDynamicRasCas0 = 0x00000202;
    EMCDynamicReadConfig = 0x00000001; /* Command delayed strategy, using EMCCLKDELAY */

    // -6 timings
    EMCDynamictRP = aConfig->iDynamicRP - 1;
    EMCDynamictRAS = aConfig->iDynamicRAS - 1;
    EMCDynamictSREX = aConfig->iDynamicSREX - 1;
    EMCDynamictAPR = aConfig->iDynamicAPR - 1;
    EMCDynamictDAL = aConfig->iDynamicDAL;
    EMCDynamictWR = aConfig->iDynamicWR - 1;
    EMCDynamictRC = aConfig->iDynamicRC - 1;
    EMCDynamictRFC = aConfig->iDynamicRFC - 1;
    EMCDynamictXSR = aConfig->iDynamicXSR - 1;
    EMCDynamictRRD = aConfig->iDynamicRRD - 1;
    EMCDynamictMRD = aConfig->iDynamicMRD - 1;

    //Send command: NOP
    EMCDynamicControl = 0x00000183;

    //wait 200mS
    UEZBSPDelayMS(200);

    //Send command: PRECHARGE-ALL, shortest possible refresh period
    EMCDynamicControl = 0x00000100;
    EMCDynamicRefresh = 0x00000001;

    //wait at least 128 ABH clock cycles
    for (i = 0; i < 0x100; i++)
        NOP();

    //Set correct refresh period
    EMCDynamicRefresh
            = (unsigned int)((((aConfig->iClockFrequency / 1000)
                    * (aConfig->iRefreshPeriod)) / aConfig->iRefreshCycles)
                    / 16) + 1;

    //wait at least 128 ABH clock cycles
    for (i = 0; i < 0x40; i++)
        NOP();

    //Send command: MODE
    EMCDynamicControl = 0x00000080;

    //Set mode register in SDRAM
    if (aConfig->iSize == (32 * 1024 * 1024)) {
        dmy = *((volatile unsigned int*)(aConfig->iBaseAddress | (0x22 << 11)));
    } else {
        dmy = *((volatile unsigned int*)(aConfig->iBaseAddress | (0x22 << 10)));
    }

    //wait 128 ABH clock cycles
    for (i = 0; i < 0x40; i++)
        NOP();

    //Send command: NORMAL
    EMCDynamicControl = 0x00000000;

    //Enable buffer
    *dc |= 0x00080000;

    //initial system delay
    UEZBSPDelayMS(1);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC2478_SDRAM.c
 *-------------------------------------------------------------------------*/
