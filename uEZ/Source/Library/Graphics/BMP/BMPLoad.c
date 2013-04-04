/*-------------------------------------------------------------------------*
 * File:  BMPLoad.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
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
#include <uEZFile.h>
#include "BMPLoad.h"
#include "BMPTypes.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError BMPLoadScreen(const char *aFilename, T_pixelColor *aPixels)
{
    T_uezFile file = 0;
    T_uezError error;
    T_BMP_Header header;
    T_BMP_DIBHeader dib;
    TUInt32 num;
    char rgb[3];
    TInt32 x, y;
    T_pixelColor *p;
    TUInt32 pad;

    do {
        error = UEZFileOpen(aFilename, FILE_FLAG_READ_ONLY, &file);
        if (error)
            break;

        error = UEZFileRead(file, &header, sizeof(header), &num);
        if (error)
            break;

        error = UEZFileRead(file, &dib, sizeof(dib), &num);
        if (error)
            break;

        // We only support a 24-bit graphic that matches the size of the
        // screen with no compression!  (Yes, very limited)
        if ((header.iMagicNumber[0] != 'B') || (header.iMagicNumber[1] != 'M')) {
            error = UEZ_ERROR_INVALID;
            break;
        }

        // Make sure this is correctly setup
        if (dib.iDIBHeaderSize != sizeof(dib)) {
            error = UEZ_ERROR_INVALID;
            break;
        }

        // Make sure the pixels are right after the header and dib
        if (header.iOffsetToPixels == (sizeof(dib) + sizeof(header))) {
            error = UEZ_ERROR_INVALID;
            break;
        }

        if ((dib.iBMPHeight != DISPLAY_HEIGHT) || (dib.iBMPWidth
                != DISPLAY_WIDTH)) {
            error = UEZ_ERROR_MISMATCH;
            break;
        }

        // Are we doing a 24 bit graphics?
        if ((dib.iNumberColorPlanes != 1) || (dib.iBitsPerPlane != 24)
                || (dib.iPaletteNumColors != 0)) {
            error = UEZ_ERROR_MISMATCH;
            break;
        }

        // Only non-compressed
        if (dib.iPixelArrayCompression != 0) {
            error = UEZ_ERROR_NOT_SUPPORTED;
            break;
        }

        // do the drawing from this point on
        for (y=DISPLAY_HEIGHT-1; y>=0; y--) {
            pad = 0;
            p = &aPixels[y*DISPLAY_WIDTH];
            for (x=0; x<DISPLAY_WIDTH; x++, p++) {
                error = UEZFileRead(file, rgb, 3, &num);
                if (error)
                    break;
                *p = RGB(rgb[2], rgb[1], rgb[0]);
            }
            pad = DISPLAY_WIDTH*3;

            while (pad & 3) {
                error = UEZFileRead(file, rgb, 1, &num);
                if (error)
                    break;
                pad++;
            }
        }
    } while (0);

    if (file)
        UEZFileClose(file);
    return error;
}

/*-------------------------------------------------------------------------*
 * End of File:  BMPLoad.c
 *-------------------------------------------------------------------------*/
