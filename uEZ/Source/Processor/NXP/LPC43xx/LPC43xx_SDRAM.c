/*-------------------------------------------------------------------------*
 * File:  LPC43xx_SDRAM.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZBSP.h>
#include <stdint.h>
#include "LPC43xx_ExternalBus.h"
#include "LPC43xx_SDRAM.h"

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

/*****************************************************************************
 ** Function name:       ILPC43xx_SDRAM_QuickTest
 **
 ** Descriptions:        sdram test
 **
 ** parameters:          None
 **
 ** Returned value:      1 if test passed, otherwise 0
 **
 *****************************************************************************/
static uint32_t ILPC43xx_SDRAM_QuickTest(TUInt32 aBaseAddress, TUInt32 aSize)
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

typedef void (*emcdivby2) (volatile uint32_t*creg6, volatile uint32_t*emcdiv, uint32_t cfg);

const uint16_t  emcdivby2_opc[] =
{
    0x6803, /*      LDR  R3,[R0,#0]      ; Load CREG6          */ 0xF443,
    0x3380, /*      ORR  R3,R3,#0x10000  ; Set Divided by 2    */
    0x6003, /*      STR  R3,[R0,#0]      ; Store CREG6         */
    0x600A, /*      STR  R2,[R1,#0]      ; EMCDIV_CFG = cfg    */
    0x684B, /* loop LDR  R3,[R1,#4]      ; Load EMCDIV_STAT    */
    0x07DB, /*      LSLS R3,R3,#31       ; Check EMCDIV_STAT.0 */
    0xD0FC, /*      BEQ  loop            ; Jump if 0           */
    0x4770, /*      BX   LR              ; Exit                */ 0,
};


#define emcdivby2_szw   ((sizeof(emcdivby2_opc) + 3) / 4)
#define emcdivby2_ram   0x10000000

void LPC43xx_SDRAM_Init_32BitBus(const T_LPC43xx_SDRAM_Configuration * aConfig)
{
    TUInt32 dynconfig;
    __IO uint32_t *dc;
    volatile TUInt32 dmy = 0;
    //volatile uint32_t ringosccount[2] = { 0, 0 };
    volatile unsigned int i;
    //uint32_t    emcdivby2_buf[emcdivby2_szw];
    //uint32_t    div;
    //uint32_t    n;

    // Do this to get rid of warning in compiler that dmy is set but not used
    VARIABLE_NOT_USED(dmy);

    G_SDRAMSize = aConfig->iSize;
    G_SDRAMAddress = aConfig->iBaseAddress;

    //for system clock over 80MHz
    if(aConfig->iClockFrequency > 80000000){
        LPC_SCU->EMCDELAYCLK = 0x7777;   /* 3.5 ns EMC clock out delay   */
    } else {
        LPC_SCU->EMCDELAYCLK = 0x0;   /* 0 ns EMC clock out delay   */
    }

    // Reset the EMC and put configuration
    // back to power up reset (little-endian mode, 1:1 clock)
    LPC_EMC->CONTROL = 0x00000001;
    LPC_EMC->CONFIG = 0x00000000;
    
    // Setup 32-bit data bus
    LPC43xx_ExternalDataBus_ConfigureIOPins(32);

    // Setup A0 .. A14 for SDRAM
    LPC43xx_ExternalAddressBus_ConfigureIOPins(15);

    LPC_SCU->SFSP6_4 = EMC_PIN_SET | 3; /* P6_4:  CAS*/

    LPC_SCU->SFSP6_5 = EMC_PIN_SET | 3; /* P6_5:  RAS*/

    // Setup for 32-bit bus
    LPC_SCU->SFSP6_12 = EMC_PIN_SET | 3; /* P6_12: DQMOUT0*/

    LPC_SCU->SFSP6_10 = EMC_PIN_SET | 3; /* P6_10: DQMOUT1*/

    LPC_SCU->SFSPD_0 = EMC_PIN_SET | 2; /* PD_0:  DQMOUT2*/

    LPC_SCU->SFSPE_13 = EMC_PIN_SET | 3; /* PE_13: DQMOUT3 */

    // Configure control pins (we really only care for bus lane 0)
    LPC_SCU->SFSP1_3 = EMC_PIN_SET | 3; /* P1_3:  OE */

    LPC_SCU->SFSP1_6 = EMC_PIN_SET | 3; /* P1_6:  WE */

//    LPC_SCU->SFSP1_5 = EMC_PIN_SET | 3; /* P1_5:  CS0 */
//    LPC_SCU->SFSP6_3 = EMC_PIN_SET | 3; /* P6_3:  CS1 */
//    LPC_SCU->SFSPD_12 = EMC_PIN_SET | 2; /* PD_12:  CS2 */
//    LPC_SCU->SFSPD_11 = EMC_PIN_SET | 2; /* PD_11:  CS3 */
//
//    LPC_SCU->SFSP1_4 = EMC_PIN_SET | 3; /* P1_4:  BLS0 */
//    LPC_SCU->SFSP6_6 = EMC_PIN_SET | 1; /* P6_6:  BLS1 */
//    LPC_SCU->SFSPD_13 = EMC_PIN_SET | 2; /* PD_13:  BLS2 */
//    LPC_SCU->SFSPD_10 = EMC_PIN_SET | 2; /* PD_10:  BLS3 */

    dynconfig = 0;
    if (aConfig->iSize == (8 * 1024 * 1024)) {
        //1 1 001 10 64 Mb (2Mx32), 4 banks, row length = 11, column length = 8
        dynconfig = (1<<14) | (1<<12) | (1<<9) | (2<<7);//0x00005300;
    } else if (aConfig->iSize == (16 * 1024 * 1024)) {
        //1 1 010 10 128 Mb (4Mx32), 4 banks, row length = 12, column length = 8
        dynconfig = (1<<14) | (1<<12) | (2<<9) | (2<<7);//0x00005500;
    } else if (aConfig->iSize == (32 * 1024 * 1024)) {
        //1 0 010 01 256 Mb (8Mx32), 4 banks, row length = 12, column length = 9
        dynconfig = (1<<14) | (1<<12) | (2<<9) | (1<<7);
    } else {
        // "Unsupported UEZBSP_SDRAM_SIZE!"
        UEZFailureMsg("SDRAM Init");
    }

    if (aConfig->iBaseAddress == 0x28000000) {
        LPC_SCU->SFSP6_9 = EMC_PIN_SET | 3; /* P6_9:  DYCS0 */

        LPC_SCU->SFSP6_11 = EMC_PIN_SET | 3; /* P6_11: CKEOUT0  */

        dc = &LPC_EMC->DYNAMICCONFIG0;
        LPC_EMC->DYNAMICCONFIG0 = dynconfig;
//        LPC_EMC->DYNAMICCONFIG1 = dynconfig;
//        LPC_EMC->DYNAMICCONFIG2 = dynconfig;
//        LPC_EMC->DYNAMICCONFIG3 = dynconfig;
    } else if (aConfig->iBaseAddress == 0xB0000000) {
//        /* CSN[1] @ P4.31 */
//        LPC_IOCON->P4_31 = (LPC_IOCON->P4_31 & ~7) | 1;
//
//        /* DYCSN[1] @ P2.21 */
//        LPC_IOCON->P2_21 = (LPC_IOCON->P2_21 & ~7) | 1;
//
//        /* CKE[1] @ P2.25 */
//        LPC_IOCON->P2_25 = (LPC_IOCON->P2_25 & ~7) | 1;
//
//        dc = &LPC_EMC->DynamicConfig1;
//        LPC_EMC->DynamicConfig1 = dynconfig;
    } else if (aConfig->iBaseAddress == 0xC0000000) {
//        /* CSN[2] @ P2.14 */
//        LPC_IOCON->P2_14 = (LPC_IOCON->P2_14 & ~7) | 1;
//
//        /* DYCSN[2] @ P2.22 */
//        LPC_IOCON->P2_22 = (LPC_IOCON->P2_22 & ~7) | 1;
//
//        /* CKE[2] @ P2.26 */
//        LPC_IOCON->P2_26 = (LPC_IOCON->P2_26 & ~7) | 1;
//
//        dc = &LPC_EMC->DynamicConfig2;
//        LPC_EMC->DynamicConfig2 = dynconfig;
    } else /*if (aConfig->iBaseAddress == 0xD0000000) */ {
//        /* CSN[3] @ P2.15 */
//        LPC_IOCON->P2_15 = (LPC_IOCON->P2_15 & ~7) | 1;
//
//        /* DYCSN[3] @ P2.23 */
//        LPC_IOCON->P2_23 = (LPC_IOCON->P2_23 & ~7) | 1;
//
//        /* CKE[3] @ P2.27 */
//        LPC_IOCON->P2_27 = (LPC_IOCON->P2_27 & ~7) | 1;
//
//        dc = &LPC_EMC->DynamicConfig3;
//        LPC_EMC->DynamicConfig3 = dynconfig;
    }

    LPC_EMC->DYNAMICRASCAS0 = 0x00000303;
    LPC_EMC->DYNAMICREADCONFIG = 0x00000001; /* Command delayed strategy, using EMCCLKDELAY */

    // -6 timings
    LPC_EMC->DYNAMICRP = aConfig->iDynamicRP - 1;
    LPC_EMC->DYNAMICRAS = aConfig->iDynamicRAS - 1;
    LPC_EMC->DYNAMICSREX = aConfig->iDynamicSREX - 1;
    LPC_EMC->DYNAMICAPR = 5;//aConfig->iDynamicAPR - 1;
    LPC_EMC->DYNAMICDAL = aConfig->iDynamicDAL;
    LPC_EMC->DYNAMICWR = 1;//aConfig->iDynamicWR - 1;
    LPC_EMC->DYNAMICRC = aConfig->iDynamicRC - 1;
    LPC_EMC->DYNAMICRFC = aConfig->iDynamicRFC - 1;
    LPC_EMC->DYNAMICXSR = aConfig->iDynamicXSR - 1;
    LPC_EMC->DYNAMICRRD = aConfig->iDynamicRRD - 1;
    LPC_EMC->DYNAMICMRD =2;// aConfig->iDynamicMRD - 1;

    //Send command: NOP
    LPC_EMC->DYNAMICCONTROL = 0x00000183;

    //wait 200mS
    UEZBSPDelayMS(200);

    //Send command: PRECHARGE-ALL, shortest possible refresh period
    LPC_EMC->DYNAMICCONTROL = 0x00000103;
    LPC_EMC->DYNAMICREFRESH = 0x00000001;

    //wait at least 128 ABH clock cycles
    for (i = 0; i < 0x100; i++)
        NOP();

    //Set correct refresh period
    LPC_EMC->DYNAMICREFRESH
            = (unsigned int)((((aConfig->iClockFrequency / 1000)
                    * (aConfig->iRefreshPeriod)) / aConfig->iRefreshCycles)
                    / 16) + 1;

    //wait at least 128 ABH clock cycles
    for (i = 0; i < 0x40; i++)
        NOP();

    //Send command: MODE
    LPC_EMC->DYNAMICCONTROL = 0x00000083;

    TUInt8 col_len;
    TUInt32 devBusWidth;

    devBusWidth = (dynconfig >> 7) & 0x03;

    if(devBusWidth == 2){
        col_len = 8;
    } else if(devBusWidth == 1){
        col_len = ((dynconfig >> (9 + 1)) & 0x03) + 8;
    } else{
        col_len = ((dynconfig >> (9 + 1)) & 0x03) + 9;
    }

    if(dynconfig & (1<< 14)){
        /*32bit bus */
        /*burst_length = 2;*/
        col_len+= 2;
    } else {
        /*burst_length = 4;*/
        col_len += 1;
    }

    if(!(dynconfig & (1<<12))){
        if(!(dynconfig & (0x7 << 9))){
            /* 2 banks => 1 bank select bit */
            col_len += 1;
        } else{
            /* 4 banks => 2 bank select bits */
            col_len += 2;
        }
    }

    devBusWidth = EMC_DYN_MODE_WBMODE_PROGRAMMED | //(0<<9)
    EMC_DYN_MODE_OPMODE_STANDARD | //(0<<7)
    EMC_DYN_MODE_CAS_3 | //(3<<4)
    EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL | //(0<<3)
    EMC_DYN_MODE_BURST_LEN_4; //(2)

    dmy = *((volatile unsigned int*)(aConfig->iBaseAddress | (devBusWidth << col_len)));
    dmy = dmy;

    //wait 128 ABH clock cycles
    for (i = 0; i < 0x40; i++)
        NOP();

    //Send command: NORMAL
    LPC_EMC->DYNAMICCONTROL = 0x00000000;

    //Enable buffer
    *dc |= (1<<19);
//    LPC_EMC->DYNAMICCONFIG1 |= (1<<19);
//    LPC_EMC->DYNAMICCONFIG2 |= (1<<19);
//    LPC_EMC->DYNAMICCONFIG3 |= (1<<19);

    //initial system delay
    UEZBSPDelayMS(1);

    ILPC43xx_SDRAM_QuickTest(aConfig->iBaseAddress, aConfig->iSize);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_SDRAM.c
 *-------------------------------------------------------------------------*/
