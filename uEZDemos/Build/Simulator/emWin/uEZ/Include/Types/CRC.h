/*-------------------------------------------------------------------------*
 * File:  CRC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef TYPES_CRC_H_
#define TYPES_CRC_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
/**
 *  @file   /Include/Types/CRC.h
 *  @brief  uEZ CRC Types
 *
 *  The uEZ CRC Types
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    UEZ_CRC_1 = 0x1,
    UEZ_CRC_4_ITU = 0x15,
    UEZ_CRC_5_USB = 0x05,
    UEZ_CRC_6_ITU = 0x03,
    UEZ_CRC_7 = 0x09,
    UEZ_CRC_8_CCITT = 0x07,
    UEZ_CRC_8_MAX = 0x31,
    UEZ_CRC_8_WCDMA = 0x9B,
    UEZ_CRC_15_CAN = 0x4599,
    UEZ_CRC_16_IBM = 0x8005, // hardware support in 1788
    UEZ_CRC_16_CCITT = 0x1021, // hardware support in 1788
    UEZ_CRC_16_DNP = 0x3D65,
    UEZ_CRC_16_DECT = 0x0589,
    UEZ_CRC_16_ARINC = 0xA02B,
    UEZ_CRC_32_ANSI = 0x04C11DB7, // hardware support in 1788
    UEZ_CRC_32 = 0x04C11DB7, // hardware support in 1788
    UEZ_CRC_32_CASTAGNOLI = 0x1EDC6F41,
    UEZ_CRC_32_KOOPMAN = 0x741B8CD7,          
    UEZ_CRC_UNKNOWN
} T_uezCRCComputationType;

typedef enum {
    UEZ_CRC_MODIFIER_INPUT_REVERSE_BIT_ORDER = 0,
    UEZ_CRC_MODIFIER_INPUT_ONES_COMPLIMENT = 1,
    UEZ_CRC_MODIFIER_CHECKSUM_REVERSE_BIT_ORDER = 2,
    UEZ_CRC_MODIFIER_CHECKSUM_ONES_COMPLIMENT = 3
} T_uezCRCModifier;

typedef enum {
    UEZ_CRC_DATA_SIZE_UINT8 = 0,
    UEZ_CRC_DATA_SIZE_UINT16 = 1,
    UEZ_CRC_DATA_SIZE_UINT32 = 2,
} T_uezCRCDataElementSize;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif // TYPES_CRC_H_
/*-------------------------------------------------------------------------*
 * End of File:  CRC.h
 *-------------------------------------------------------------------------*/
