/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_EMC_Static.c
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
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_EMC_Static.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_ExternalBus.h>
#include <uEZBSP.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    __IO uint32_t StaticConfig;              /*!< Offset: 0x200 (R/W)  Static Memory Configuration Register */
    __IO uint32_t StaticWaitWen;             /*!< Offset: 0x204 (R/W)  Static Memory Write Enable Delay Register */
    __IO uint32_t StaticWaitOen;             /*!< Offset: 0x208 (R/W)  Static Memory Output Enable Delay Register */
    __IO uint32_t StaticWaitRd;              /*!< Offset: 0x20C (R/W)  Static Memory Read Delay Register */
    __IO uint32_t StaticWaitPage;            /*!< Offset: 0x210 (R/W)  Static Memory Page Mode Read Delay Register */
    __IO uint32_t StaticWaitWr;              /*!< Offset: 0x214 (R/W)  Static Memory Write Delay Register */
    __IO uint32_t StaticWaitTurn;            /*!< Offset: 0x218 (R/W)  Static Memory Turn Round Delay Register */
} T_LPC17xx_40xx_EMC_Static_Regs;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC17xx_40xx_EMC_Static_Init(const T_LPC17xx_40xx_EMC_Static_Configuration *aConfig)
{
    T_LPC17xx_40xx_EMC_Static_Regs *emc;
    TUInt8 numAddrBits;
    TUInt32 remaining;

    if (aConfig->iBaseAddress == 0x80000000) {
        emc = (T_LPC17xx_40xx_EMC_Static_Regs *)&LPC_EMC->StaticConfig0;
        /* CSN[0] @ P4.30 */
        LPC_IOCON->P4_30 = (LPC_IOCON->P4_30 & ~7) | 1;
    } else if (aConfig->iBaseAddress == 0x90000000) {
        emc = (T_LPC17xx_40xx_EMC_Static_Regs *)&LPC_EMC->StaticConfig1;
        /* CSN[1] @ P4.31 */
        LPC_IOCON->P4_31 = (LPC_IOCON->P4_31 & ~7) | 1;
    } else if (aConfig->iBaseAddress == 0x98000000) {
        emc = (T_LPC17xx_40xx_EMC_Static_Regs *)&LPC_EMC->StaticConfig2;
        /* CSN[2] @ P2.14 */
        LPC_IOCON->P2_14 = (LPC_IOCON->P2_14 & ~7) | 1;
    } else {
        emc = (T_LPC17xx_40xx_EMC_Static_Regs *)&LPC_EMC->StaticConfig3;
        /* CSN[3] @ P2.15 */
        LPC_IOCON->P2_15 = (LPC_IOCON->P2_15 & ~7) | 1;
    }
    // Configure control pins (we really only care for bus lane 0)
    /* OEN @ P4.24 */
    LPC_IOCON->P4_24 = (LPC_IOCON->P4_24 & ~7) | 1;
    /* WEN @ P4.25 */
    LPC_IOCON->P4_25 = (LPC_IOCON->P4_25 & ~7) | 1;

    // Setup the address bits
    numAddrBits = aConfig->iBitWidth+1;
    remaining = aConfig->iSize / (1<<numAddrBits);
    while (remaining) {
        numAddrBits++;
        remaining >>= 1;
    }
    LPC17xx_40xx_ExternalAddressBus_ConfigureIOPins(numAddrBits);

    // Configurate static memory
    emc->StaticConfig =
            ((aConfig->iBitWidth) << 0) | // 1=16-bit
            (1 << 3) | // 0=page mode disabled, 1 = page mode enabled
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
    if ((aConfig->iStaticWaitRd == 0) || (aConfig->iStaticWaitRd > 0x1F))
        UEZBSP_FatalError(3);
    emc->StaticWaitRd = ((aConfig->iStaticWaitRd-1) & 0x1F);

    // Page Mode Read Delay register
    // Delay for asynchronous page mode sequential access
    // use tRC = address valid to address valid
    if ((aConfig->iStaticWaitPage == 0) || (aConfig->iStaticWaitPage > 0x1F))
        UEZBSP_FatalError(3);
    emc->StaticWaitPage = ((aConfig->iStaticWaitPage-1) & 0x1F);

    // Delay from Chip select to write enable time
    // Use tELWL = tCS = Chip Enable Low to Write Enable Low
    if ((aConfig->iStaticWaitWen == 0) || (aConfig->iStaticWaitWen > 0xF))
        UEZBSP_FatalError(3);
    emc->StaticWaitWen = ((aConfig->iStaticWaitWen-1) & 0xF);

    // Write Delay Register
    // Delay from the Chip Select to the Write Access
    // use tWLRH = tWP = Write Enable Low to Write Enable High
    if ((aConfig->iStaticWaitWr <= 1) || (aConfig->iStaticWaitWr > 0x1F))
        UEZBSP_FatalError(3);
    emc->StaticWaitWr = ((aConfig->iStaticWaitWr-2) & 0x1F);

    // Turn Round Delay register
    // Number of bus turnaround cycles, cycles between read and write access
    // use tWLRH = tWP = Write Enable Low to Write Enable High
    if ((aConfig->iStaticWaitTurn == 0) || (aConfig->iStaticWaitTurn > 0xF))
        UEZBSP_FatalError(3);
    emc->StaticWaitTurn = aConfig->iStaticWaitTurn;

    // Make sure address are byte wise on the address bus
    LPC_SC->SCS |= (1<<0);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_EMC_Static.c
 *-------------------------------------------------------------------------*/
