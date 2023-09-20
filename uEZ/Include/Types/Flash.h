/*-------------------------------------------------------------------------*
 * File:  Types/Flash.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Flash types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _Flash_TYPES_H_
#define _Flash_TYPES_H_

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

/**
 *  @file   /Include/Types/Flash.h
 *  @brief  uEZ Flash Types
 *
 *  The uEZ Flash Types
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iNumEraseBlocks;        /** count, base 1 */ 
    TUInt32 iSizeEraseBlock;        /** in bytes */ 
} T_FlashChipRegion;

#define FLASH_CHIP_MAX_TRACKED_REGIONS          4

typedef struct {
    TUInt32 iNumBytesLow;
    TUInt32 iNumBytesHigh;		/** if bigger than 4 Gig */ 
    TUInt8 iBitWidth;   		/**  8, 16, or 32 */

    TUInt32 iNumRegions;
    T_FlashChipRegion iRegions[FLASH_CHIP_MAX_TRACKED_REGIONS];
} T_FlashChipInfo;

typedef struct {
    TUInt32 iOffset;
    TUInt32 iSize;
} T_FlashBlockInfo;

#ifdef __cplusplus
}
#endif

#endif // _Flash_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/Flash.h
 *-------------------------------------------------------------------------*/
