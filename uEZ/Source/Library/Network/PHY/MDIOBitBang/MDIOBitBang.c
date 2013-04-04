/*-------------------------------------------------------------------------*
 * File:  MDIOBitBang.c
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
#include <uEZBSP.h>
#include "MDIOBitBang.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define MDIO22_WRITE        0x0001
#define MDIO22_READ         0x0002

#define MDIO45_ADDR         0x8000
#define MDIO45_WRITE        0x8001
#define MDIO45_READ         0x8003

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void MDIOBitBangSendBit(const T_mdioBitBangSettings *p, TUInt8 aBit)
{
    // Set the data line
    if (aBit)
        UEZGPIOSet(p->iPinMDIO);
    else
        UEZGPIOClear(p->iPinMDIO);

    // Hold data line
    UEZBSPDelayUS(p->iDelayUSPerHalfBit);

    // Clock out
    UEZGPIOSet(p->iPinMDC);
    UEZBSPDelayUS(p->iDelayUSPerHalfBit);
    UEZGPIOClear(p->iPinMDC);
}

TUInt8 MDIOBitBangReceiveBit(const T_mdioBitBangSettings *p)
{
    // Delay first
    UEZBSPDelayUS(p->iDelayUSPerHalfBit);

    // Clock in
    UEZGPIOSet(p->iPinMDC);
    UEZBSPDelayUS(p->iDelayUSPerHalfBit);
    UEZGPIOClear(p->iPinMDC);

    // What's the final result?  Is it high?
    if (UEZGPIORead(p->iPinMDIO))
        return 1;

    // Must be low
    return 0;
}

TUInt32 MDIOBitBangReceiveBits(const T_mdioBitBangSettings *p, TUInt8 aNumBits)
{
    TUInt32 v = 0;

    while (aNumBits--) {
        v <<= 1;
        v |= MDIOBitBangReceiveBit(p);
    }

    return v;
}

void MDIOBitBangSendBits(
    const T_mdioBitBangSettings *p,
    TUInt32 aBits,
    TUInt8 aNumBits)
{
    TUInt32 mask = 1UL << (aNumBits - 1);

    while (aNumBits--) {
        MDIOBitBangSendBit(p, (aBits & mask) ? 1 : 0);
        mask >>= 1;
    }
}

void MDIOBitBangSendCmd(
    const T_mdioBitBangSettings *p,
    TUInt16 aCommand,
    TUInt16 aPHY,
    TUInt16 aReg)
{
    // Output on the MDIO line
    UEZGPIOOutput(p->iPinMDIO);
    // Output on the MDC line
    UEZGPIOOutput(p->iPinMDC);

    // Send 32-bit preamble of 1's
    MDIOBitBangSendBits(p, 0xFFFFFFFF, 32);

    // now send a start bits (01 for Clause 22, 00 for Clause 45)
    MDIOBitBangSendBit(p, 0);
    MDIOBitBangSendBit(p, (p->iIsClause45) ? 0 : 1);

    // Send two lowest bits of command
    MDIOBitBangSendBit(p, (aCommand >> 1) & 1);
    MDIOBitBangSendBit(p, (aCommand >> 0) & 1);

    // Send Phy and register bits (10-bits total)
    MDIOBitBangSendBits(p, aPHY, 5);
    MDIOBitBangSendBits(p, aReg, 5);
}

void MDIOBitBangSetAddress(
    const T_mdioBitBangSettings *p,
    TUInt16 aPHY,
    TUInt32 aAddr)
{
    MDIOBitBangSendCmd(p, MDIO45_ADDR, aPHY, (aAddr >> 16));

    // Send the turnaround bits (10)
    MDIOBitBangSendBit(p, 1);
    MDIOBitBangSendBit(p, 0);

    // Send the value
    MDIOBitBangSendBits(p, aAddr, 16);

    // Back to input
    UEZGPIOInput(p->iPinMDIO);

    // Clock out one last bit
    MDIOBitBangReceiveBit(p);
}

TUInt16 MDIOBitBangRead(
    const T_mdioBitBangSettings *p,
    TUInt16 aPHY,
    TUInt32 aReg)
{
    TUInt8 b;
    TUInt16 v;

    if (p->iIsClause45) {
        MDIOBitBangSetAddress(p, aPHY, aReg);
        MDIOBitBangSendCmd(p, MDIO45_READ, aPHY, aReg);
    } else {
        MDIOBitBangSendCmd(p, MDIO22_READ, aPHY, aReg);
    }

    // Input on the MDIO line
    UEZGPIOInput(p->iPinMDIO);

    // Check the turn around bit (should be driving zero)
    b = MDIOBitBangReceiveBit(p);
    if (b != 0) {
        // Didn't get a low, flush by reading 32 bits
        MDIOBitBangReceiveBits(p, 32);

        // Return a 0xFFFF bogus result
        v = 0xFFFF;
    } else {
        // Let's get the value
        v = MDIOBitBangReceiveBits(p, 16);
    }

    // Clock out one last bit
    MDIOBitBangReceiveBit(p);

    return v;
}

void MDIOBitBangWrite(
    const T_mdioBitBangSettings *p,
    TUInt16 aPHY,
    TUInt32 aReg,
    TUInt16 aValue)
{
    if (p->iIsClause45) {
        MDIOBitBangSetAddress(p, aPHY, aReg);
        MDIOBitBangSendCmd(p, MDIO45_WRITE, aPHY, aReg);
    } else {
        MDIOBitBangSendCmd(p, MDIO22_WRITE, aPHY, aReg);
    }

    // Send the turnaround bits (10)
    MDIOBitBangSendBit(p, 1);
    MDIOBitBangSendBit(p, 0);

    // Send the value
    MDIOBitBangSendBits(p, aValue, 16);

    // Back to input
    UEZGPIOInput(p->iPinMDIO);

    // Clock out one last bit
    MDIOBitBangReceiveBit(p);
}

/*-------------------------------------------------------------------------*
 * End of File:  MDIOBitBang.c
 *-------------------------------------------------------------------------*/
