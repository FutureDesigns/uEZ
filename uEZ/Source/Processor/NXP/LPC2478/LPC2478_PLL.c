/*-------------------------------------------------------------------------*
 * File:  LPC2478_PLL.c
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
#include "LPC2478_PLL.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
TUInt32 G_OscillatorFrequency;
TUInt32 G_ProcessorFrequency;
TUInt32 G_PeripheralFrequency;
TUInt32 G_EMCFrequency;
TUInt32 G_USBFrequency;

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
#ifndef PLLCFG_VAL
    #define PLLCFG_VAL ((11 << PLLCFG_MSEL_BIT) | (0 << PLLCFG_NSEL_BIT))
#endif
#ifndef CCLKCFG_VAL
    #define CCLKCFG_VAL 3 // divide Fcco by 4 for 72 MHz
#endif

#ifndef USBCLKCFG_VAL
    #define USBCLKCFG_VAL 5 // divide Fcco by 6 for 48 MHz
#endif

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC2478_PLL_SetFrequencies(const T_LPC2478_PLL_Frequencies *aFreq)
{
    TUInt32 timeout;
    
    if (aFreq->iPLL0Frequency != 72000000)
        UEZFailureMsg("72MHz CPU only supported currently!");
    if (aFreq->iOscillatorClockHz != 12000000)
        UEZFailureMsg("12MHz XTAL only supported currently!");

    // Hard coded values
    G_OscillatorFrequency = 12000000;
    G_ProcessorFrequency = 72000000;
    G_PeripheralFrequency = 72000000;
    G_EMCFrequency = 72000000;
    G_USBFrequency = 48000000;

reprogram:
    // Is the PLL connected?
    if (PLLSTAT & (1<<25)) {
        // Disconnect the PLL
        PLLCON = PLLCON_PLLE;
        PLLFEED = 0xAA;
        PLLFEED = 0x55;
    }

    // Disable PLL
    PLLCON = 0;
    PLLFEED = 0xAA;
    PLLFEED = 0x55;

    // Enable the main oscillator
    SCS |= (1<<5);

    // Wait for main oscilaltor to be enabled
    while ((SCS & (1<<6))==0)
        {}

    // Select the main oscillator as the PLL clock source
    CLKSRCSEL = 1;

    // Set PLLCFG
    PLLCFG = PLLCFG_VAL;
    PLLFEED = 0xAA;
    PLLFEED = 0x55;

    // Enable PLL
    PLLCON = 1;
    PLLFEED = 0xAA;
    PLLFEED = 0x55;

    CCLKCFG = CCLKCFG_VAL;

    USBCLKCFG = USBCLKCFG_VAL;

    // Wait for PLL to lock (via PLOCK bit)
    timeout = 0;
    while (!(PLLSTAT & (1<<26)))
        { 
            timeout++;
            if (timeout > 10000)
                goto reprogram;
        }

    // PLL Locked, connect PLL as clock source
    PLLCON = (1<<0)|(1<<1);
    PLLFEED = 0xAA;
    PLLFEED = 0x55;

    // Wait for PLL to connect (PLLC bit)
    while ((PLLSTAT & (1<<25)) == 0)
        {}

    // Internal flash settings can be faster
    MAMTIM = 4; // 4 cycles per flash access, so less than 20 MHz
    MAMCR = 2; // turn on flash acceleration fully
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC2478_PLL.c
 *-------------------------------------------------------------------------*/
