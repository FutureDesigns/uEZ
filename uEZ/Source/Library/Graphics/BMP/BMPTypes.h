/*-------------------------------------------------------------------------*
 * File:  BMPTypes.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef BMPTYPES_H_
#define BMPTYPES_H_

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
#include <uEZ.h>
#include <uEZPacked.h>

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

#endif // BMPTYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  BMPTypes.h
 *-------------------------------------------------------------------------*/
