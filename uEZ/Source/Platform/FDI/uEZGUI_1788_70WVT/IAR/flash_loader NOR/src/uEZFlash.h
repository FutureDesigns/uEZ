/*-------------------------------------------------------------------------*
 * File:  Types/Flash.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Flash types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _Flash_TYPES_H_
#define _Flash_TYPES_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
#include <uEZTypes.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iNumEraseBlocks;        // count, base 1
    TUInt32 iSizeEraseBlock;        // in bytes
} T_FlashChipRegion;

#define FLASH_CHIP_MAX_TRACKED_REGIONS          4

typedef struct {
    TUInt32 iNumBytesLow;
    TUInt32 iNumBytesHigh; // if bigger than 4 Gig
    TUInt8 iBitWidth;   // 8, 16, or 32

    TUInt32 iNumRegions;
    T_FlashChipRegion iRegions[FLASH_CHIP_MAX_TRACKED_REGIONS];
} T_FlashChipInfo;

typedef struct {
    TUInt32 iOffset;
    TUInt32 iSize;
} T_FlashBlockInfo;

#endif // _Flash_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/Flash.h
 *-------------------------------------------------------------------------*/
