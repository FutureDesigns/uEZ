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
#include "uEZPlatformAPI.h"

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

// Implement in platform to allow for LED activity and to see which step we are on.
void MemoryTest_StepA(void)
{
  UEZPlatform_MemTest_StepA();
}
void MemoryTest_StepA_Pass(void)
{
  UEZPlatform_MemTest_StepA_Pass();
}
void MemoryTest_StepB(void)
{
  UEZPlatform_MemTest_StepB();
}
void MemoryTest_StepB_Pass(void)
{
  UEZPlatform_MemTest_StepB_Pass();
}
void MemoryTest_StepC(void)
{
  UEZPlatform_MemTest_StepC();
}
void MemoryTest_StepC_Pass(void)
{
  UEZPlatform_MemTest_StepC_Pass();
}
void MemoryTest_StepD(void)
{
  UEZPlatform_MemTest_StepD();
}
void MemoryTest_StepD_Pass(void)
{
  UEZPlatform_MemTest_StepD_Pass();
}
void MemoryTest_StepE(void)
{
  UEZPlatform_MemTest_StepE();
}
void MemoryTest_StepE_Pass(void)
{
  UEZPlatform_MemTest_StepE_Pass();
}
void MemoryTest_StepF(void)
{
  UEZPlatform_MemTest_StepF();
}
void MemoryTest_StepF_Pass(void)
{
  UEZPlatform_MemTest_StepF_Pass();
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
    MemoryTest_StepA();
    // Now confirm
    for (count = 0; count < aMemorySize; count++) {
        if (ram8[count] != 0x00)
            MemoryTestFail();
    }
    MemoryTest_StepA_Pass();

    // Fill with 0xFF's
    for (count = 0; count < aMemorySize; count++) {
        ram8[count] = 0xFF;
    }
    MemoryTest_StepB();
    // Now confirm
    for (count = 0; count < aMemorySize; count++) {
        if (ram8[count] != 0xFF)
            MemoryTestFail();
    }
    MemoryTest_StepB_Pass();

    // Fill with 0xAA
    for (count = 0; count < aMemorySize; count++) {
        ram8[count] = 0xAA;
    }
    MemoryTest_StepC();
    // Now confirm
    for (count = 0; count < aMemorySize; count++) {
        if (ram8[count] != 0xAA)
            MemoryTestFail();
    }
    MemoryTest_StepC_Pass();

    // Fill with 0x55
    for (count = 0; count < aMemorySize; count++) {
        ram8[count] = 0x55;
    }
    MemoryTest_StepD();
    // Now confirm
    for (count = 0; count < aMemorySize; count++) {
        if (ram8[count] != 0x55)
            MemoryTestFail();
    }
    MemoryTest_StepD_Pass();

    // Now use a semi-random pattern using 32-bit words
    for (count = 0, prime = 0; count < aMemorySize; count += 4, prime += 37) {
        ram32[count / 4] = prime;
    }
    MemoryTest_StepE();
    // Now confirm
    for (count = 0, prime = 0; count < aMemorySize; count += 4, prime += 37) {
        if (ram32[count / 4] != prime)
            MemoryTestFail();
    }
    MemoryTest_StepE_Pass();

    // Now change to a different prime number and run the opposite direction
    for (count = 0, prime = 0xFEDCBA98; count < aMemorySize; count += 4, prime
            -= 319993) {
        ram32[count / 4] = prime;
    }
    MemoryTest_StepF();
    // Now confirm
    for (count = 0, prime = 0xFEDCBA98; count < aMemorySize; count += 4, prime
            -= 319993) {
        if (ram32[count / 4] != prime)
            MemoryTestFail();
    }
    MemoryTest_StepF_Pass();
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  MemoryTest.c
 *-------------------------------------------------------------------------*/
