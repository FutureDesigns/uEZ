/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_SDRAM.c
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
#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZMemory.h>
#include <uEZBSP.h>
#include <stdint.h>
#include "LPC17xx_40xx_ExternalBus.h"
#include "LPC17xx_40xx_SDRAM.h"
#include "LPC17xx_40xx_UtilityFuncs.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
/* Note that we don't access these variables here, but we can't set them
 * here in SDRAM until after SDRAM is on. So force them into internal RAM.*/
UEZ_PUT_SECTION(".IRAM", TUInt32 G_SDRAMAddress = 0);
UEZ_PUT_SECTION(".IRAM", TUInt32 G_SDRAMSize = 0);

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*****************************************************************************
 ** Function name:       ILPC17xx_40xx_SDRAM_QuickTest
 **
 ** Descriptions:        sdram test
 **
 ** parameters:          None
 **
 ** Returned value:      1 if test passed, otherwise 0
 **
 *****************************************************************************/
static uint32_t ILPC17xx_40xx_SDRAM_QuickTest(TUInt32 aBaseAddress, TUInt32 aSize)
{
    volatile uint32_t *wr_ptr;
    volatile uint16_t *short_wr_ptr;
    uint32_t data;
    uint32_t i, j;

    wr_ptr = (uint32_t *)aBaseAddress;
    short_wr_ptr = (uint16_t *)wr_ptr;
    /* Clear content before 16 bit access test */
    //  for (i = 0; i < G_SDRAMSize/4; i++)
    //  {
    //  *wr_ptr++ = 0;
    //  }

    /* 16 bit write */
    for (i = 0; i < aSize / 0x40000; i++) {
        for (j = 0; j < 0x100; j++) {
            *short_wr_ptr++ = (i + j);
            *short_wr_ptr++ = (i + j) + 1;
        }
    }

    /* Verifying */
    wr_ptr = (uint32_t *)aBaseAddress;
    for (i = 0; i < aSize / 0x40000; i++) {
        for (j = 0; j < 0x100; j++) {
            data = *wr_ptr;
            if (data != (((((i + j) + 1) & 0xFFFF) << 16) | ((i + j) & 0xFFFF))) {
                return 0x0;
            }
            wr_ptr++;
        }
    }
    return 0x1;
}

/*****************************************************************************
 ** Function name:       ILPC17xx_40xx_SDRAM_FindCmdDelay
 **
 ** Descriptions:        find CMDDLY
 **
 ** parameters:          None
 **
 ** Returned value:      1 if test passed, otherwise 0
 **
 *****************************************************************************/
static uint32_t ILPC17xx_40xx_SDRAM_FindCmdDelay(TUInt32 aBaseAddress, TUInt32 aSize)
{
    uint32_t cmddly, cmddlystart, cmddlyend, dwtemp;
    uint32_t ppass = 0x0, pass = 0x0;

    cmddly = 0x0;
    cmddlystart = cmddlyend = 0xFF;

    while (cmddly < 32) {
        dwtemp = LPC_SC->EMCDLYCTL & ~0x1F;
        LPC_SC->EMCDLYCTL = dwtemp | cmddly;

        if (ILPC17xx_40xx_SDRAM_QuickTest(aBaseAddress, aSize) == 0x1) {
            /* Test passed */
            if (cmddlystart == 0xFF) {
                cmddlystart = cmddly;
            }
            ppass = 0x1;
        } else {
            /* Test failed */
            if (ppass == 1) {
                cmddlyend = cmddly;
                pass = 0x1;
                ppass = 0x0;
            }
        }

        /* Try next value */
        cmddly++;
    }

    /* If the test passed, the we can use the average of the min */
    /* and max values to get an optimal DQSIN delay */
    if (pass == 0x1) {
        cmddly = (cmddlystart + cmddlyend) / 2;
    } else if (ppass == 0x1) {
        cmddly = (cmddlystart + 0x1F) / 2;
    } else {
        /* A working value couldn't be found, just pick something */
        /* safe so the system doesn't become unstable */
        cmddly = 0x10;
    }

    dwtemp = LPC_SC->EMCDLYCTL & ~0x1F;
    LPC_SC->EMCDLYCTL = dwtemp | cmddly;

    return (pass | ppass);
}

/*****************************************************************************
 ** Function name:       ILPC17xx_40xx_SDRAM_FindFBClockDelay
 **
 ** Descriptions:        find FBCLKDLY
 **
 ** parameters:          None
 **
 ** Returned value:      1 if test passed, otherwise 0
 **
 *****************************************************************************/
static uint32_t ILPC17xx_40xx_SDRAM_FindFBClockDelay(
        TUInt32 aBaseAddress,
        TUInt32 aSize)
{
    uint32_t fbclkdly, fbclkdlystart, fbclkdlyend, dwtemp;
    uint32_t ppass = 0x0, pass = 0x0;

    fbclkdly = 0x0;
    fbclkdlystart = fbclkdlyend = 0xFF;

    while (fbclkdly < 32) {
        dwtemp = LPC_SC->EMCDLYCTL & ~0x1F00;
        LPC_SC->EMCDLYCTL = dwtemp | (fbclkdly << 8);

        if (ILPC17xx_40xx_SDRAM_QuickTest(aBaseAddress, aSize) == 0x1) {
            /* Test passed */
            if (fbclkdlystart == 0xFF) {
                fbclkdlystart = fbclkdly;
            }
            ppass = 0x1;
        } else {
            /* Test failed */
            if (ppass == 1) {
                fbclkdlyend = fbclkdly;
                pass = 0x1;
                ppass = 0x0;
            }
        }

        /* Try next value */
        fbclkdly++;
    }

    /* If the test passed, the we can use the average of the min and max values to get an optimal DQSIN delay */
    if (pass == 0x1) {
        fbclkdly = (fbclkdlystart + fbclkdlyend) / 2;
    } else if (ppass == 0x1) {
        fbclkdly = (fbclkdlystart + 0x1F) / 2;
    } else {
        /* A working value couldn't be found, just pick something safe so the system doesn't become unstable */
        fbclkdly = 0x10;
    }

    dwtemp = LPC_SC->EMCDLYCTL & ~0x1F00;
    LPC_SC->EMCDLYCTL = dwtemp | (fbclkdly << 8);

    return (pass | ppass);
}

/*****************************************************************************
 ** Function name:       ILPC17xx_40xx_SDRAM_Calibration
 **
 ** Descriptions:        Calibration
 **
 ** parameters:          None
 **
 ** Returned value:      current ring osc count
 **
 *****************************************************************************/
static uint32_t ILPC17xx_40xx_SDRAM_Calibration(void)
{
    uint32_t dwtemp, i;
    uint32_t cnt = 0;

    for (i = 0; i < 10; i++) {
        dwtemp = LPC_SC->EMCCAL & ~0x4000;
        LPC_SC->EMCCAL = dwtemp | 0x4000;

        dwtemp = LPC_SC->EMCCAL;
        while ((dwtemp & 0x8000) == 0x0000) {
            dwtemp = LPC_SC->EMCCAL;
        }
        cnt += (dwtemp & 0xFF);
    }
    return (cnt / 10);
}

/*****************************************************************************
 ** Function name:       ILPC17xx_40xx_SDRAM_AdjustTiming
 **
 ** Descriptions:        Adjust timing
 **
 ** parameters:          None
 **
 ** Returned value:      None
 **
 *****************************************************************************/
static void ILPC17xx_40xx_SDRAM_AdjustTiming(volatile uint32_t ringosccount[2])
{
    uint32_t dwtemp, cmddly, fbclkdly;

    /* Current value */
    ringosccount[1] = ILPC17xx_40xx_SDRAM_Calibration();

    dwtemp = LPC_SC->EMCDLYCTL;
    cmddly = ((dwtemp & 0x1F) * ringosccount[0] / ringosccount[1]) & 0x1F;
    fbclkdly = ((dwtemp & 0x1F00) * ringosccount[0] / ringosccount[1]) & 0x1F00;
    LPC_SC->EMCDLYCTL = (dwtemp & ~0x1F1F) | fbclkdly | cmddly;
}

void LPC17xx_40xx_SDRAM_Init_32BitBus(const T_LPC17xx_40xx_SDRAM_Configuration * aConfig)
{
    TUInt32 dynconfig;
    __IO uint32_t *dc;
    volatile TUInt32 dmy = 0;
    volatile uint32_t ringosccount[2] = { 0, 0 };
    volatile uint32_t i;

    // Do this to get rid of warning in compiler that dmy is set but not used
    VARIABLE_NOT_USED(dmy);

    G_SDRAMSize = aConfig->iSize;
    G_SDRAMAddress = aConfig->iBaseAddress;

    // EMC Reset Disable = Enabled, Both EMC resets are asserted
    // when any type of reset occurs.
    // [Ref: LPC178x_7x_UM_0.01 page 30, Table 14]
    LPC_SC->SCS &= ~(1 << 1);

    // Turn on EMC PCLK (if not already on)
    // [Ref: LPC178x_7x_UM_0.01 page 58, Table 36]
    LPC_SC->PCONP |= (1 << 11); // Don't use LPC17xx_40xxPowerOn here as this is before the service init.

    // Setup slow/default delays
    LPC_SC->EMCDLYCTL = 0x00001010;

    // Reset the EMC and put configuration
    // back to power up reset (little-endian mode, 1:1 clock)
    // [Ref: LPC178x_7x_UM_0.01 page 156]
    LPC_EMC->Control = 0x00000001;
    LPC_EMC->Config = 0x00000000;

    // Setup 32-bit data bus
    LPC17xx_40xx_ExternalDataBus_ConfigureIOPins(32);

    // Setup A0 .. A14 for SDRAM
    // TODO: Note: A11 and A12 are included regardless if they are used
    LPC17xx_40xx_ExternalAddressBus_ConfigureIOPins(15);

    // Configure control pins (we really only care for bus lane 0)
    /* OEN @ P4.24 */
    LPC_IOCON->P4_24 = (LPC_IOCON->P4_24 & ~7) | 1;
    /* WEN @ P4.25 */
    LPC_IOCON->P4_25 = (LPC_IOCON->P4_25 & ~7) | 1;

    //      LPC_IOCON->P4_26 |= 1; /* BLSN[0] @ P4.26 */
    //      LPC_IOCON->P4_27 |= 1; /* BLSN[1] @ P4.27 */
    //      LPC_IOCON->P4_28 |= 1; /* BLSN[2] @ P4.28 */
    //      LPC_IOCON->P4_29 |= 1; /* BLSN[3] @ P4.29 */
    dynconfig = 0;
     // 4 banks, then either 11/12 row length, and 8/9 column length
    if (aConfig->iSize == (8 * 1024 * 1024)) {
        dynconfig = 0x00005300; // 1 001 10 64Mbits  (2M x 32)
    } else if (aConfig->iSize == (16 * 1024 * 1024)) {
        dynconfig = 0x00005500; // 1 010 10 128Mbits (4M x 32)
    } else if (aConfig->iSize == (32 * 1024 * 1024)) {
        dynconfig = 0x00005480; // 1 010 01 128Mbits (8M x 16)
    } else {
        // "Unsupported UEZBSP_SDRAM_SIZE!"
        UEZFailureMsg("SDRAM Init");
    }

    if (aConfig->iBaseAddress == 0xA0000000) {
        /* CSN[0] @ P4.30 */
        LPC_IOCON->P4_30 = (LPC_IOCON->P4_30 & ~7) | 1;

        /* DYCSN[0] @ P2.20 */
        LPC_IOCON->P2_20 = (LPC_IOCON->P2_20 & ~7) | 1;

        /* CKE[0] @ P2.24 */
        LPC_IOCON->P2_24 = (LPC_IOCON->P2_24 & ~7) | 1;

        dc = &LPC_EMC->DynamicConfig0;
        LPC_EMC->DynamicConfig0 = dynconfig;
    } else if (aConfig->iBaseAddress == 0xB0000000) {
        /* CSN[1] @ P4.31 */
        LPC_IOCON->P4_31 = (LPC_IOCON->P4_31 & ~7) | 1;

        /* DYCSN[1] @ P2.21 */
        LPC_IOCON->P2_21 = (LPC_IOCON->P2_21 & ~7) | 1;

        /* CKE[1] @ P2.25 */
        LPC_IOCON->P2_25 = (LPC_IOCON->P2_25 & ~7) | 1;

        dc = &LPC_EMC->DynamicConfig1;
        LPC_EMC->DynamicConfig1 = dynconfig;
    } else if (aConfig->iBaseAddress == 0xC0000000) {
        /* CSN[2] @ P2.14 */
        LPC_IOCON->P2_14 = (LPC_IOCON->P2_14 & ~7) | 1;

        /* DYCSN[2] @ P2.22 */
        LPC_IOCON->P2_22 = (LPC_IOCON->P2_22 & ~7) | 1;

        /* CKE[2] @ P2.26 */
        LPC_IOCON->P2_26 = (LPC_IOCON->P2_26 & ~7) | 1;

        dc = &LPC_EMC->DynamicConfig2;
        LPC_EMC->DynamicConfig2 = dynconfig;
    } else /*if (aConfig->iBaseAddress == 0xD0000000) */ {
        /* CSN[3] @ P2.15 */
        LPC_IOCON->P2_15 = (LPC_IOCON->P2_15 & ~7) | 1;

        /* DYCSN[3] @ P2.23 */
        LPC_IOCON->P2_23 = (LPC_IOCON->P2_23 & ~7) | 1;

        /* CKE[3] @ P2.27 */
        LPC_IOCON->P2_27 = (LPC_IOCON->P2_27 & ~7) | 1;

        dc = &LPC_EMC->DynamicConfig3;
        LPC_EMC->DynamicConfig3 = dynconfig;
    }

    /* CASN @ P2.16 */
    LPC_IOCON->P2_16 = (LPC_IOCON->P2_16 & ~7) | 1;

    /* RASN @ P2.17 */
    LPC_IOCON->P2_17 = (LPC_IOCON->P2_17 & ~7) | 1;

    if (aConfig->iClockOut == 0) {
        /* CLK[0] @ P2.18 */
        LPC_IOCON->P2_18 = (LPC_IOCON->P2_18 & ~7) | 1;
    } else {
        /* CLK[1] @ P2.19 */
        LPC_IOCON->P2_19 = (LPC_IOCON->P2_19 & ~7) | 1;
    }

    // Setup for 32-bit bus
    /* DQM[0] @ P2.28 */
    LPC_IOCON->P2_28 = (LPC_IOCON->P2_28 & ~7) | 1;
    /* DQM[1] @ P2.29 */
    LPC_IOCON->P2_29 = (LPC_IOCON->P2_29 & ~7) | 1;
    /* DQM[2] @ P2.30 */
    LPC_IOCON->P2_30 = (LPC_IOCON->P2_30 & ~7) | 1;
    /* DQM[3] @ P2.31 */
    LPC_IOCON->P2_31 = (LPC_IOCON->P2_31 & ~7) | 1;

    // RAS=2, CAS=2
    LPC_EMC->DynamicRasCas0 = 0x00000202;
    LPC_EMC->DynamicReadConfig = 0x00000001; /* Command delayed strategy, using EMCCLKDELAY */

    // -6 timings
    LPC_EMC->DynamicRP = aConfig->iDynamicRP - 1;
    LPC_EMC->DynamicRAS = aConfig->iDynamicRAS - 1;
    LPC_EMC->DynamicSREX = aConfig->iDynamicSREX - 1;
    LPC_EMC->DynamicAPR = aConfig->iDynamicAPR - 1;
    LPC_EMC->DynamicDAL = aConfig->iDynamicDAL;
    LPC_EMC->DynamicWR = aConfig->iDynamicWR - 1;
    LPC_EMC->DynamicRC = aConfig->iDynamicRC - 1;
    LPC_EMC->DynamicRFC = aConfig->iDynamicRFC - 1;
    LPC_EMC->DynamicXSR = aConfig->iDynamicXSR - 1;
    LPC_EMC->DynamicRRD = aConfig->iDynamicRRD - 1;
    LPC_EMC->DynamicMRD = aConfig->iDynamicMRD - 1;

    //Send command: NOP
    LPC_EMC->DynamicControl = 0x00000183;

    //wait 200mS
    UEZBSPDelayMS(200);

    //Send command: PRECHARGE-ALL, shortest possible refresh period
    LPC_EMC->DynamicControl = 0x00000100;
    LPC_EMC->DynamicRefresh = 0x00000001;

    //wait at least 128 ABH clock cycles
    for (i = 0; i < 0x100; i++)
        NOP();

    //Set correct refresh period // NOTE: TODO This number may be smaller than needed.
    LPC_EMC->DynamicRefresh
            = (uint32_t)((((aConfig->iClockFrequency / 1000)
                    * (aConfig->iRefreshPeriod)) / aConfig->iRefreshCycles)
                    / 16) + 1;

    //wait at least 128 ABH clock cycles
    for (i = 0; i < 0x40; i++)
        NOP();

    //Send command: MODE
    LPC_EMC->DynamicControl = 0x00000080;

    //Set mode register in SDRAM
    if (aConfig->iSize == (32 * 1024 * 1024)) {
        dmy = *((volatile uint32_t*)(aConfig->iBaseAddress | (0x22 << 11)));
    } else {
        dmy = *((volatile uint32_t*)(aConfig->iBaseAddress | (0x22 << 10)));
    }

    //wait 128 ABH clock cycles
    for (i = 0; i < 0x40; i++)
        NOP();

    //Send command: NORMAL
    LPC_EMC->DynamicControl = 0x00000000;

    //Enable buffer
    *dc |= 0x00080000;

    //initial system delay
    UEZBSPDelayMS(1);

    ringosccount[0] = ILPC17xx_40xx_SDRAM_Calibration();

    if (ILPC17xx_40xx_SDRAM_FindCmdDelay(aConfig->iBaseAddress, aConfig->iSize)
            == 0x0) {
        UEZFailureMsg("SDRAM Init"); /* fatal error */
    }

    if (ILPC17xx_40xx_SDRAM_FindFBClockDelay(aConfig->iBaseAddress, aConfig->iSize)
            == 0x0) {
        UEZFailureMsg("SDRAM Init"); /* fatal error */
    }

    ILPC17xx_40xx_SDRAM_AdjustTiming(ringosccount);

}

/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_SDRAM.c
 *-------------------------------------------------------------------------*/
