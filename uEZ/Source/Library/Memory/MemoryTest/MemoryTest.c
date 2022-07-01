/*-------------------------------------------------------------------------*
 * File:  MemoryTest.c
 *-------------------------------------------------------------------------*/
/**
 *  @file   MemoryTest.c
 *  @brief  uEZ Memory Test
 *
 *  uEZ Memory Test
 */
 
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include "MemoryTest.h"

/*---------------------------------------------------------------------------*
 * Routine:  MemoryTestFail
 *---------------------------------------------------------------------------*/
/**
 *	Come here when the memory test fails and report the error.
 *      This routine can be overridden when linking to the library.
 */
void MemoryTestFail(void)
{
    UEZFailureMsg("Memory Test Fail!");
}

/*---------------------------------------------------------------------------*
 * Routine:  MemoryTest 
 *---------------------------------------------------------------------------*/
/**
 *	This is a very slow and comprehensive memory test.  Only use
 *      if debugging the hardware.
 *
 *	@param [in] 	aMemoryAddress	Memory Address
 *                          
 *	@param [in]		aMemorySize		Size of memory region to test
 */
void MemoryTest(TUInt32 aMemoryAddress, TUInt32 aMemorySize)
{
    TUInt8 *ram8 = (TUInt8 *)aMemoryAddress;
    TUInt32 *ram32 = (TUInt32 *)aMemoryAddress;
    TUInt32 count = 0;
    TUInt32 prime;

    // Clear with zeros
    for (count = 0; count < aMemorySize; count++) {
        ram8[count] = 0x00;
    }
    // Now confirm
    for (count = 0; count < aMemorySize; count++) {
        if (ram8[count] != 0x00)
            MemoryTestFail();
    }

    // Fill with 0xFF's
    for (count = 0; count < aMemorySize; count++) {
        ram8[count] = 0xFF;
    }
    // Now confirm
    for (count = 0; count < aMemorySize; count++) {
        if (ram8[count] != 0xFF)
            MemoryTestFail();
    }

    // Fill with 0xAA
    for (count = 0; count < aMemorySize; count++) {
        ram8[count] = 0xAA;
    }
    // Now confirm
    for (count = 0; count < aMemorySize; count++) {
        if (ram8[count] != 0xAA)
            MemoryTestFail();
    }

    // Fill with 0x55
    for (count = 0; count < aMemorySize; count++) {
        ram8[count] = 0x55;
    }
    // Now confirm
    for (count = 0; count < aMemorySize; count++) {
        if (ram8[count] != 0x55)
            MemoryTestFail();
    }

    // Now use a semi-random pattern using 32-bit words
    for (count = 0, prime = 0; count < aMemorySize; count += 4, prime += 37) {
        ram32[count / 4] = prime;
    }
    // Now confirm
    for (count = 0, prime = 0; count < aMemorySize; count += 4, prime += 37) {
        if (ram32[count / 4] != prime)
            MemoryTestFail();
    }

    // Now change to a different prime number and run the opposite direction
    for (count = 0, prime = 0xFEDCBA98; count < aMemorySize; count += 4, prime
            -= 319993) {
        ram32[count / 4] = prime;
    }
    // Now confirm
    for (count = 0, prime = 0xFEDCBA98; count < aMemorySize; count += 4, prime
            -= 319993) {
        if (ram32[count / 4] != prime)
            MemoryTestFail();
    }
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  MemoryTest.c
 *-------------------------------------------------------------------------*/
