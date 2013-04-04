/*-------------------------------------------------------------------------*
 * File:  LPC1788_EMC_Static.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC1788_EMC_STATIC_H_
#define LPC1788_EMC_STATIC_H_

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
} T_LPC1788_EMC_Static_MemoryWidth;

typedef struct {
        // Base address for this static memory area.  For the LPC1788,
        // the following base addresses are allowed:
        //      0x80000000 - Static Memory Bank 0
        //      0x90000000 - Static Memory Bank 1
        //      0x98000000 - Static Memory Bank 2
        //      0x9C000000 - Static Memory Bank 3
        TUInt32 iBaseAddress;

        // Size is needed to configure the pins
        TUInt32 iSize;

        // Bit width
        T_LPC1788_EMC_Static_MemoryWidth iBitWidth;
        TBool iCSIsActiveHigh;

        // In EMC cycles (use Macro EMC_STATIC_CYCLES):
        TUInt32 iStaticWaitOen;
        TUInt32 iStaticWaitRd;
        TUInt32 iStaticWaitPage;
        TUInt32 iStaticWaitWen;
        TUInt32 iStaticWaitWr;
        TUInt32 iStaticWaitTurn;
} T_LPC1788_EMC_Static_Configuration;

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
// Convert a number of nano-seconds into a number of flash memory cycles
// (rounded up)
#define EMC_STATIC_CYCLES(x) \
    (1+((unsigned int)(((double)x)*(1.0/1000000000.0)*((double)EMC_CLOCK_FREQUENCY))))
#define EMC_STATIC_CLOCKS(x) (x)

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC1788_EMC_Static_Init(const T_LPC1788_EMC_Static_Configuration *aConfig);

#endif // LPC1788_EMC_STATIC_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1788_EMC_Static.h
 *-------------------------------------------------------------------------*/
