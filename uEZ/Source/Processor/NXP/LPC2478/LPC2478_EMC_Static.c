/*-------------------------------------------------------------------------*
 * File:  LPC2478_EMC_Static.c
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
#include "LPC2478_EMC_Static.h"
#include "LPC2478_ExternalBus.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    volatile TUInt32 StaticConfig;              /*!< Offset: 0x200 (R/W)  Static Memory Configuration Register */
    volatile TUInt32 StaticWaitWen;             /*!< Offset: 0x204 (R/W)  Static Memory Write Enable Delay Register */
    volatile TUInt32 StaticWaitOen;             /*!< Offset: 0x208 (R/W)  Static Memory Output Enable Delay Register */
    volatile TUInt32 StaticWaitRd;              /*!< Offset: 0x20C (R/W)  Static Memory Read Delay Register */
    volatile TUInt32 StaticWaitPage;            /*!< Offset: 0x210 (R/W)  Static Memory Page Mode Read Delay Register */
    volatile TUInt32 StaticWaitWr;              /*!< Offset: 0x214 (R/W)  Static Memory Write Delay Register */
    volatile TUInt32 StaticWaitTurn;            /*!< Offset: 0x218 (R/W)  Static Memory Turn Round Delay Register */
} T_LPC2478_EMC_Static_Regs;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC2478_EMC_Static_Init(const T_LPC2478_EMC_Static_Configuration *aConfig)
{
    T_LPC2478_EMC_Static_Regs *emc;
    TUInt8 numAddrBits;
    TUInt32 remaining;

    if (aConfig->iBaseAddress == 0x80000000) {
        emc = (T_LPC2478_EMC_Static_Regs *)&EMCStaticConfig0;
        /* CS0n @ P4.30 */
        PINSEL9 &= ~PINSEL_MASK(30);
        PINSEL9 |= PINSEL_MUX(30, 1);
    } else if (aConfig->iBaseAddress == 0x81000000) {
        emc = (T_LPC2478_EMC_Static_Regs *)&EMCStaticConfig1;
        /* CS1n @ P4.31 */
        PINSEL9 &= ~PINSEL_MASK(31);
        PINSEL9 |= PINSEL_MUX(31, 1);
    } else if (aConfig->iBaseAddress == 0x82000000) {
        emc = (T_LPC2478_EMC_Static_Regs *)&EMCStaticConfig2;
        /* CS2n @ P2.14 */
        PINSEL4 &= ~PINSEL_MASK(14);
        PINSEL4 |= PINSEL_MUX(14, 1);
    } else { // 0x83000000
        emc = (T_LPC2478_EMC_Static_Regs *)&EMCStaticConfig3;
        /* CS3n @ P2.15 */
        PINSEL4 &= ~PINSEL_MASK(15);
        PINSEL4 |= PINSEL_MUX(15, 1);
    }
    // Configure control pins (we really only care for bus lane 0)
    /* OEN @ P4.24 */
    PINSEL9 &= ~PINSEL_MASK(24);
    PINSEL9 |= PINSEL_MUX(24, 1);
    /* WEN @ P4.25 */
    PINSEL9 &= ~PINSEL_MASK(25);
    PINSEL9 |= PINSEL_MUX(25, 1);

    // Setup the address bits
    numAddrBits = aConfig->iBitWidth+1;
    remaining = aConfig->iSize / (1<<numAddrBits);
    while (remaining) {
        numAddrBits++;
        remaining >>= 1;
    }
    LPC2478_ExternalAddressBus_ConfigureIOPins(numAddrBits);

    // Configurate static memory
    emc->StaticConfig =
            ((aConfig->iBitWidth) << 0) | // 1=16-bit
            (0 << 3) | // 0=page mode disabled (TBD: for now)
            (((aConfig->iCSIsActiveHigh)?1:0) << 6) | // 0=Acive LOW chip select (CS0n -> En)
            (1 << 7) | // 0=For Reads BLSn[3:0] HIGH, For writes BLSn[3:0] LOW (not used)
            (0 << 8) | // 0=Extended Wait disabled (POR reset value)
            (0 << 19) | // 0=Buffer disabled (POR reset value, TBD)
            (0 << 20); // 0=Write not protected

    // Delay from chip select or address change, whichever is later to
    // the output enable
    // Use tAVQV = tACC = Address valid to Output Valid
    emc->StaticWaitOen = aConfig->iStaticWaitOen;

    // Delay from the chip select to the read access
    // Use tELQV = tEn = Address valid to Next Address Valid
    emc->StaticWaitRd = aConfig->iStaticWaitRd;

    // Page Mode Read Delay register
    // Delay for asynchronous page mode sequential access
    // use tRC = address valid to address valid
    emc->StaticWaitPage = aConfig->iStaticWaitPage;

    // Delay from Chip select to write enable time
    // Use tELWL = tCS = Chip Enable Low to Write Enable Low
    emc->StaticWaitWen = aConfig->iStaticWaitWen;

    // Write Delay Register
    // Delay from the Chip Select to the Write Access
    // use tWLRH = tWP = Write Enable Low to Write Enable High
    emc->StaticWaitWr = aConfig->iStaticWaitWr;

    // Turn Round Delay register
    // Number of bus turnaround cycles, cycles between read and write access
    // use tWLRH = tWP = Write Enable Low to Write Enable High
    emc->StaticWaitTurn = aConfig->iStaticWaitTurn;
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC2478_EMC_Static.c
 *-------------------------------------------------------------------------*/
