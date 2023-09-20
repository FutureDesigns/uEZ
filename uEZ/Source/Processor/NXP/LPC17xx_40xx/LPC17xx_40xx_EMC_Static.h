/*-------------------------------------------------------------------------*
 * File:  LPC17xx_40xx_EMC_Static.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC17xx_40xx_EMC_STATIC_H_
#define LPC17xx_40xx_EMC_STATIC_H_

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

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
        EMC_STATIC_MEMORY_WIDTH_8_BITS=0,
        EMC_STATIC_MEMORY_WIDTH_16_BITS=1,
        EMC_STATIC_MEMORY_WIDTH_32_BITS=2,
} T_LPC17xx_40xx_EMC_Static_MemoryWidth;

typedef struct {
        // Base address for this static memory area.  For the LPC17xx_40xx,
        // the following base addresses are allowed:
        //      0x80000000 - Static Memory Bank 0
        //      0x90000000 - Static Memory Bank 1
        //      0x98000000 - Static Memory Bank 2
        //      0x9C000000 - Static Memory Bank 3
        TUInt32 iBaseAddress;

        // Size is needed to configure the pins
        TUInt32 iSize;

        // Bit width
        T_LPC17xx_40xx_EMC_Static_MemoryWidth iBitWidth;
        TBool iCSIsActiveHigh;

        // In EMC cycles (use Macro EMC_STATIC_CYCLES):
        TUInt32 iStaticWaitOen;
        TUInt32 iStaticWaitRd;
        TUInt32 iStaticWaitPage;
        TUInt32 iStaticWaitWen;
        TUInt32 iStaticWaitWr;
        TUInt32 iStaticWaitTurn;
} T_LPC17xx_40xx_EMC_Static_Configuration;

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
// Convert a number of nano-seconds into a number of flash memory cycles
// (rounded up)
#define EMC_STATIC_CYCLES(x) \
    (1+((uint32_t)(((double)x)*(1.0/1000000000.0)*((double)EMC_CLOCK_FREQUENCY))))
#define EMC_STATIC_CLOCKS(x) (x)

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC17xx_40xx_EMC_Static_Init(const T_LPC17xx_40xx_EMC_Static_Configuration *aConfig);

#ifdef __cplusplus
}
#endif

#endif // LPC17xx_40xx_EMC_STATIC_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC17xx_40xx_EMC_Static.h
 *-------------------------------------------------------------------------*/
