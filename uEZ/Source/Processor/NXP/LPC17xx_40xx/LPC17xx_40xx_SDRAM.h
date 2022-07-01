/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_SDRAM.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC17xx_40xx_SDRAM_H_
#define LPC17xx_40xx_SDRAM_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
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
#include <Types/SDRAM.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
// Calculate number of SDRAM cycles (rounded up)
#define SDRAM_CYCLES(x) \
    (1+((unsigned int)(((double)x)*(1.0/1000000000.0)* \
            ((double)SDRAM_CLOCK_FREQUENCY))))
#define SDRAM_CLOCKS(x) (x)

#define LPC17xx_40xx_SDRAM_CLKOUT0       0
#define LPC17xx_40xx_SDRAM_CLKOUT1       1

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        // Base address for LPC17xx_40xx.  Can be one of the following (and changes
        // which bank of control pins to use).
        //     0xA0000000 - Dynamic Memory Bank 0
        //     0xB0000000 - Dynamic Memory Bank 1
        //     0xC0000000 - Dynamic Memory Bank 2
        //     0xD0000000 - Dynamic Memory Bank 3
        TUInt32 iBaseAddress;

        // Total size of memory in bytes
        TUInt32 iSize;

        // Number of CAS cycles, usually 2 or 3
        TUInt8 iCAS;

        // Number of RAS cycles, usually 2 or 3
        TUInt8 iRAS;

        // 0 for CLKOUT0 (P2.18) or 1 for CLKOUT1 (P2.19)
        TUInt8 iClockOut;

        // SDRAM clock frequency (Hz)
        TUInt32 iClockFrequency;

        // SDRAM Refresh period in ms (example: 64 ms)
        TUInt32 iRefreshPeriod;

        // Number cycles per period (example: 8192 cycles)
        TUInt32 iRefreshCycles;

        // Timings for this SDRAM
        TUInt32 iDynamicRP;     // tRP min
        TUInt32 iDynamicRAS;    // tRAS min
        TUInt32 iDynamicSREX;   // tXSR min
        TUInt32 iDynamicAPR;    // tAPR/tRCD
        TUInt32 iDynamicDAL;    // tDAL clocks
        TUInt32 iDynamicWR;     // tWR
        TUInt32 iDynamicRC;     // tRC
        TUInt32 iDynamicRFC;    // tRFC
        TUInt32 iDynamicXSR;    // tXSR
        TUInt32 iDynamicRRD;    // tRRD
        TUInt32 iDynamicMRD;    // tMRD clocks
        // Example:
        //    sdramConfig->DynamicRP = SDRAM_CYCLES(20); // tRP min = 20 ns
        //    sdramConfig->DynamicRAS = SDRAM_CYCLES(42); // tRAS min = 42 ns
        //    sdramConfig->DynamicSREX = SDRAM_CYCLES(70); // use tXSR min = 70 ns
        //    sdramConfig->DynamicAPR = SDRAM_CYCLES(18); // no tAPR, using tRCD = 18 ns
        //    sdramConfig->DynamicDAL = SDRAM_CLOCKS(4); // for CL 2, tDAL = 4 tCK
        //    sdramConfig->DynamicWR = (SDRAM_CYCLES(6) + SDRAM_CLOCKS(1)); // tWR = tCK + 6ns
        //    sdramConfig->DynamicRC = SDRAM_CYCLES(60); // tRC min = 60 ns
        //    sdramConfig->DynamicRFC = SDRAM_CYCLES(60); // tRFC min = 60 ns
        //    sdramConfig->DynamicXSR = SDRAM_CYCLES(70); // tXSR min = 70 ns
        //    sdramConfig->DynamicRRD = SDRAM_CYCLES(12); // tRRD = 12 ns
        //    sdramConfig->DynamicMRD = SDRAM_CLOCKS(2); // tMRD = 2 tCK
} T_LPC17xx_40xx_SDRAM_Configuration;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC17xx_40xx_SDRAM_Init_32BitBus(const T_LPC17xx_40xx_SDRAM_Configuration *aConfig);

#ifdef __cplusplus
}
#endif

#endif // LPC17xx_40xx_SDRAM_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_SDRAM.h
 *-------------------------------------------------------------------------*/
