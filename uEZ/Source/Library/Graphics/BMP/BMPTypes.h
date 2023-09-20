/*-------------------------------------------------------------------------*
 * File:  BMPTypes.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef BMPTYPES_H_
#define BMPTYPES_H_

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
#include <uEZ.h>
#include <uEZPacked.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
PACK_STRUCT_BEGIN
typedef struct {
    // "BM" field
    PACK_STRUCT_FIELD(TUInt8 iMagicNumber[2]);

    // Total size of the BMP
    PACK_STRUCT_FIELD(TUInt32 iSize);

    // Unused fields
    PACK_STRUCT_FIELD(TUInt16 iUnused1);
    PACK_STRUCT_FIELD(TUInt16 iUnused2);

    // Offset to start of pixel data (skipping other data)
    PACK_STRUCT_FIELD(TUInt32 iOffsetToPixels);
} PACK_STRUCT_STRUCT T_BMP_Header;
PACK_STRUCT_END

PACK_STRUCT_BEGIN
typedef struct {
    // Number of bytes in DIB header
    PACK_STRUCT_FIELD(TUInt32 iDIBHeaderSize);

    // Bitmap width
    PACK_STRUCT_FIELD(TUInt32 iBMPWidth);

    // Bitmap height
    PACK_STRUCT_FIELD(TUInt32 iBMPHeight);

    // Number of color planes
    PACK_STRUCT_FIELD(TUInt16 iNumberColorPlanes);

    // Number of bits per plane
    PACK_STRUCT_FIELD(TUInt16 iBitsPerPlane);

    // Type of pixel array compression used (0=none)
    PACK_STRUCT_FIELD(TUInt32 iPixelArrayCompression);

    // Size of the raw data in the pixel array (including padding)
    PACK_STRUCT_FIELD(TUInt32 iSizePixelArray);

    // Horizontal resolution (pixels/meter)
    PACK_STRUCT_FIELD(TUInt32 aHorizontalResolution);

    // Vertical resolution (pixels/meter)
    PACK_STRUCT_FIELD(TUInt32 aVerticalResolution);

    // Number of colors in color palette (if any)
    PACK_STRUCT_FIELD(TUInt32 iPaletteNumColors);

    // Number of important colors (0=all)
    PACK_STRUCT_FIELD(TUInt32 iPaletteNumImportant);

    // Pixel array comes next -- Each line must be padded to 4 byte widths
    // RGB Pixel arrays are in triples in BGR format.
} PACK_STRUCT_STRUCT T_BMP_DIBHeader;
PACK_STRUCT_END

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
 
#ifdef __cplusplus
}
#endif

#endif // BMPTYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  BMPTypes.h
 *-------------------------------------------------------------------------*/
