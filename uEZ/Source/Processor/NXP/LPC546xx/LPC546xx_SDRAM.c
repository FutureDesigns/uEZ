/*-------------------------------------------------------------------------*
 * File:  LPC546xx_SDRAM.c
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
#include "LPC546xx_ExternalBus.h"
#include "LPC546xx_SDRAM.h"

//TODO: Remove
#include "iar/Include/CMSIS/LPC54608.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define EMC_SDRAM_WAIT_CYCLES  (2000U)

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
 ** Function name:       ILPC546xx_SDRAM_QuickTest
 **
 ** Descriptions:        sdram test
 **
 ** parameters:          None
 **
 ** Returned value:      1 if test passed, otherwise 0
 **
 *****************************************************************************/
static uint32_t ILPC546xx_SDRAM_QuickTest(TUInt32 aBaseAddress, TUInt32 aSize)
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

void LPC546xx_SDRAM_Init_16BitBus(const T_LPC546xx_SDRAM_Configuration * aConfig)
{
    TUInt32 count = 0;
    volatile TUInt32 dmy;
    TUInt32 cycles;
    
    // Do this to get rid of warning in compiler that dmy is set but not used
    VARIABLE_NOT_USED(dmy);

    LPC546xxPowerOn(kCLOCK_Iocon);
    LPC546xxPowerOn(kCLOCK_Emc);

    LPC546xx_ExternalAddressBus_ConfigureIOPins();
    LPC546xx_ExternalDataBus_ConfigureIOPins();
    LPC546xx_ExternalControl_ConfigureIOPins();

    /* Reset the EMC. */
    SYSCON->PRESETCTRL[2] |= SYSCON_PRESETCTRL_EMC_RESET_MASK;
    SYSCON->PRESETCTRL[2] &= ~ SYSCON_PRESETCTRL_EMC_RESET_MASK;

    /* Set the EMC sytem configure. */
    if(aConfig->iClockFrequency < 100000000){
        SYSCON->EMCCLKDIV = SYSCON_EMCCLKDIV_DIV(0);//Div by 1
    } else {
        SYSCON->EMCCLKDIV = SYSCON_EMCCLKDIV_DIV(1);//Div by 2
    }

    SYSCON->EMCSYSCTRL = SYSCON_EMCSYSCTRL_EMCFBCLKINSEL(0);

    EMC->CONFIG = 0x00; //Little Endian
    EMC->CONTROL = EMC_CONTROL_E_MASK;

    EMC->DYNAMIC[0].DYNAMICCONFIG = EMC_DYNAMIC_DYNAMICCONFIG_MD(0) |
                    ((0x09u) << 7 /* 128Mbits (8M*16, 4banks, 12 rows, 9 columns)*/);

    EMC->DYNAMIC[0].DYNAMICRASCAS = (aConfig->iCAS << 8) | (aConfig->iRAS);

    /* Configure the Dynamic Memory controller timing/latency for all chips. */
    EMC->DYNAMICREADCONFIG = EMC_DYNAMICREADCONFIG_RD(1);
    EMC->DYNAMICRP = aConfig->iDynamicRP - 1;
    EMC->DYNAMICRAS = aConfig->iDynamicRAS - 1;
    EMC->DYNAMICSREX = aConfig->iDynamicSREX - 1;
    EMC->DYNAMICAPR = aConfig->iDynamicAPR - 1;
    EMC->DYNAMICDAL = aConfig->iDynamicDAL;
    EMC->DYNAMICWR = aConfig->iDynamicWR;
    EMC->DYNAMICRC = aConfig->iDynamicRC - 1;
    EMC->DYNAMICRFC = aConfig->iDynamicRFC - 1;
    EMC->DYNAMICXSR = aConfig->iDynamicXSR - 1;
    EMC->DYNAMICRRD = aConfig->iDynamicRRD - 1;
    EMC->DYNAMICMRD = aConfig->iDynamicMRD - 1;

    /* Initialize the SDRAM.*/
    for (count = 0; count < EMC_SDRAM_WAIT_CYCLES;  count ++){
    }
    /* Step 2. issue nop command. */
    EMC->DYNAMICCONTROL  = 0x00000183;
    for (count = 0; count < EMC_SDRAM_WAIT_CYCLES;  count ++){
    }
    /* Step 3. issue precharge all command. */
    EMC->DYNAMICCONTROL  = 0x00000103;

    /* Step 4. issue two auto-refresh command. */
    EMC->DYNAMICREFRESH = 2;
    for (count = 0; count < EMC_SDRAM_WAIT_CYCLES/2; count ++){
    }

    cycles = (aConfig->iClockFrequency / 1000000) * ((aConfig->iRefreshPeriod*1000000)/aConfig->iRefreshCycles);
    cycles = ((cycles + 1000 - 1)) / 1000;
    cycles /= 16;
    EMC->DYNAMICREFRESH = cycles;

    /* Step 5. issue a mode command and set the mode value. */
    EMC->DYNAMICCONTROL  = 0x00000083;

    //Set mode register in SDRAM
    if (aConfig->iSize == (32 * 1024 * 1024)) { //compare this step
        dmy = *((volatile unsigned int*)(aConfig->iBaseAddress | (0x22 << 11)));
        dmy = dmy;
    } else {
        dmy = *((volatile unsigned int*)(aConfig->iBaseAddress | (0x23 << 12)));
        dmy = dmy;
    }

//    //wait 128 ABH clock cycles
//    for (count = 0; count < 0x40; count++){
//        NOP();
//    }

    /* Step 6. issue normal operation command. */
    EMC->DYNAMICCONTROL  = 0x00000000; /* Issue NORMAL command */

    EMC->DYNAMIC[0].DYNAMICCONFIG |= EMC_DYNAMIC_DYNAMICCONFIG_B_MASK;
    ILPC546xx_SDRAM_QuickTest(aConfig->iBaseAddress, aConfig->iSize);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC546xx_SDRAM.c
 *-------------------------------------------------------------------------*/
