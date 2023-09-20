/*-------------------------------------------------------------------------*
 * File:  BMPLoad.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
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
