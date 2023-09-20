/*-------------------------------------------------------------------------*
 * File:  Drawing.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Utility routines to use for drawing graphics and showing pages
 *      of the LCD display.
 *
 * Implementation:
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>

#include <Types/LCD.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>

#include <uEZFile.h>
#include <string.h>
#include <uEZPlatform.h>
#include "SimpleUI.h"

// Move this here (instead of SimpleUI_Types.h) to prevent some of the circular includes.
#include <uEZTypes.h>

/*---------------------------------------------------------------------------*
 * Globals
 *---------------------------------------------------------------------------*/
extern T_SUISettings G_SUISettings;

/*---------------------------------------------------------------------------*
 * Routine:  IHidePage0
 *---------------------------------------------------------------------------*
 * Description:
 *      Copy the current viewed page to the hidden page 1 and then show
 *      page 1 (hiding page 0 as changes are about to be made).
 *---------------------------------------------------------------------------*/
void SUIHidePage0(void)
{
    const TUInt32 height = G_SUISettings.iWindow.ypsize;
    const TUInt32 width = G_SUISettings.iWindow.xpsize;
    TUInt32 frame_buffer_size = height * width * sizeof(T_pixelColor);
    // Make sure to add only the required frame size, and not double or quadrouple based on int type.
    const TUInt32 fb1 = ((TUInt32) G_SUISettings.iWindow.fb) + (TUInt32)frame_buffer_size;

    // Copy current page 0 of display to page 1
    SUICopyFast32((void*) fb1, G_SUISettings.iWindow.fb, frame_buffer_size);

    // Show page 1
    SUICallbackSetLCDBase((void *) fb1);
}

/*---------------------------------------------------------------------------*
 * Routine:  IShowPage0
 *---------------------------------------------------------------------------*
 * Description:
 *      Show page 0 (assumed page 1 or other was being shown)
 *---------------------------------------------------------------------------*/
void SUIShowPage0(void)
{
    const T_pixelColor *fb = G_SUISettings.iWindow.fb;
    // Show page 0
    SUICallbackSetLCDBase((void *)fb);
}

/*---------------------------------------------------------------------------*
 * Routine:  IShowPage0Fancy
 *---------------------------------------------------------------------------*
 * Description:
 *      Scroll from page 1 to page 0 bringing page 0 back into view.
 *      Scrolls in at 1 ms per screen raster.
 *---------------------------------------------------------------------------*/
void SUIShowPage0Fancy(void)
{
    TUInt32 start, end, diff;
    const TUInt32 height = G_SUISettings.iWindow.ypsize;
    const TUInt32 width = G_SUISettings.iWindow.xpsize;
    const T_pixelColor *fb = G_SUISettings.iWindow.fb;

    start = UEZTickCounterGet();
    while (1) {
        end = UEZTickCounterGet();
        diff = ((end - start) * height) / 240;
        if (diff > height)
            break;
        SUICallbackSetLCDBase((void *)(fb + ((height - 1 - diff) * width))); // TODO this goes negative from FB start!
        UEZTaskDelay(1);
    }
    SUIShowPage0();
}

/*---------------------------------------------------------------------------*
 * Routine:  IShowPage0FancyDown
 *---------------------------------------------------------------------------*
 * Description:
 *      Scroll from page 1 to page 2 bringing page 2 back into view, but
 *      then swap it all over to page 0
 *      Scrolls in at 1 ms per screen raster.
 *---------------------------------------------------------------------------*/
void SUIShowPage0FancyDown(void)
{
    TUInt32 start, end, diff;
    const TUInt32 height = G_SUISettings.iWindow.ypsize;
    const TUInt32 width = G_SUISettings.iWindow.xpsize;
    const TUInt32 frame_buffer_size = height * width;
    T_pixelColor * const fb = G_SUISettings.iWindow.fb;

    // Copy page 0 to page 2 first
    SUICopyFast32((void *)(fb + (2 * frame_buffer_size)), (void *)fb,
        sizeof(*fb) * frame_buffer_size);

    start = UEZTickCounterGet();
    while (1) {
        end = UEZTickCounterGet();
        diff = ((end - start) * height) / 240;
        if (diff >= height)
            break;
        SUICallbackSetLCDBase((void *)(fb + ((height - 1 + diff) * width)));
        UEZTaskDelay(1);
    }

    // Jump back to page 0
    SUIShowPage0();
}

/*---------------------------------------------------------------------------*
 * Routine:  SUIDrawPixel
 *---------------------------------------------------------------------------*
 * Description:
 *      Draws a single pixel on the screen.  Each pixel is a 16-bit
 *      value in RGB order.  Red is 5 bits, Green is 6 bits, Blue is 5 bits.
 *      Use the macro RGB16(r, g, b) to generate the proper value.
 * Inputs:
 *      TUInt16 *pixels         -- Pointer to pixel memory
 *      TUInt32 x, y            -- Coordinate of pixel
 *      TUInt16 color           -- Color of pixel
 *---------------------------------------------------------------------------*/
void SUIDrawPixel(TUInt32 x, TUInt32 y, T_pixelColor color)
{
    if (x >= (TUInt32) G_SUISettings.iWindow.xpsize)
        return;
    if (y >= (TUInt32) G_SUISettings.iWindow.ypsize)
        return;

    swim_driver_put_pixel(&G_SUISettings.iWindow, x, y, color);
}

/*---------------------------------------------------------------------------*
 * Routine:  SUIFillRect
 *---------------------------------------------------------------------------*
 * Description:
 *      Draws a filled rectangle in the pixel memory.
 * Inputs:
 *      TUInt16 *pixels         -- Pointer to pixel memory
 *      TUInt32 x, y            -- Coordinate of pixel
 *      TUInt16 color           -- Color of pixel
 *---------------------------------------------------------------------------*/
void SUIFillRect(
    T_pixelColor *pixels,
    int32_t x1,
    int32_t y1,
    int32_t x2,
    int32_t y2,
    T_pixelColor color)
{
    int32_t y;
    int32_t len = 1 + x2 - x1;

    for (y = y1; y <= y2; y++)
        swim_driver_fill_raster(&G_SUISettings.iWindow, x1, y, color, len);
}

/*---------------------------------------------------------------------------*
 * Routine:  SUIDrawIcon
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw an icon to the screen at the given x, y location.  Coordinates
 *      are physical coordinates (not window coordinates)
 * Inputs:
 *      const TUInt8 *p         -- Pointer to icon data in Targa format.
 *      TUInt16 aXOffset        -- X pixel offset
 *      TUInt16 aYOffset        -- Y pixel offset
 *---------------------------------------------------------------------------*/
void SUIDrawIcon(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset)
{
    // First, load the background into the buffer
    const TUInt8 *p_palette;
    TUInt32 x, y;
    TUInt32 xx, yy;
    const TUInt8 *p_pixels;
    TUInt32 width;
    TUInt32 height;
    TUInt32 numColors;
    T_pixelColor rgbArray[256];
    TUInt32 i;
    const TUInt8 *rgb;

    width = *((TUInt16 *)(p + 12));
    height = *((TUInt16 *)(p + 14));
    numColors = p[6];
    numColors *= 256;
    numColors += p[5];
    if (numColors > 256)
        return;

    p_palette = p + 18;
    p_pixels = p + 18 + numColors * 3;

    // Convert the palette to native pixels
    rgb = p_palette;
    for (i = 0; i < numColors; i++, rgb += 3)
        rgbArray[i] = SUICallbackRGBConvert(rgb[2], rgb[1], rgb[0]);

    for (y = 0; y < height; y++) {
        const TUInt8 *p_raster = &p_pixels[(height - 1 - y) * width];
        if (G_SUISettings.iDoubleSizeIcons) {
            for (x = 0; x < width; x++) {
                T_pixelColor color = rgbArray[*(p_raster++)];

                // Icons in VGA are drawn double sized.  Draw this
                // one by scaling 2:1 with larger pixels
                xx = x * 2 + aXOffset;
                yy = y * 2 + aYOffset;
                swim_driver_put_pixel(&G_SUISettings.iWindow, xx, yy, color);
                swim_driver_put_pixel(&G_SUISettings.iWindow, ++xx, yy, color);
                swim_driver_put_pixel(&G_SUISettings.iWindow, xx, ++yy, color);
                swim_driver_put_pixel(&G_SUISettings.iWindow, --xx, yy, color);
            }
        } else {
            swim_driver_put_raster_indexed_color(&G_SUISettings.iWindow,
                aXOffset, aYOffset+y, p_raster, rgbArray, width);
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  SUIDrawImage
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw an image to the screen at the given x, y location.  Coordinates
 *      are physical coordinates (not window coordinates).  Images
 *      are not resized like icons.
 * Inputs:
 *      const TUInt8 *p         -- Pointer to icon data in Targa format.
 *      TUInt16 aXOffset        -- X pixel offset
 *      TUInt16 aYOffset        -- Y pixel offset
 *---------------------------------------------------------------------------*/
void SUIDrawImage(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset)
{
    // First, load the background into the buffer
    const TUInt8 *p_palette;
    TUInt32 x, y;
    const TUInt8 *p_pixels;
    TUInt32 width;
    TUInt32 height;
    T_pixelColor rgbArray[256];
    TUInt32 i;
    const TUInt8 *rgb;

    width = *((TUInt16 *)(p + 12));
    height = *((TUInt16 *)(p + 14));

    p_palette = p + 18;
    p_pixels = p + 18 + 256 * 3;

    // Convert the palette to native pixels
    rgb = p_palette;
    for (i = 0; i < 256; i++, rgb += 3)
        rgbArray[i] = SUICallbackRGBConvert(rgb[2], rgb[1], rgb[0]);

    for (y = 0; y < height; y++) {
        const TUInt8 *p_raster = &p_pixels[(height - 1 - y) * width];
        for (x = 0; x < width; x++) {
            T_pixelColor color = rgbArray[*(p_raster++)];
            SUIDrawPixel(x + aXOffset, y + aYOffset, color);
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  SUIRenderRGBRaster
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw a number of pixels on the screen at the given location,
 *      the length of pixels, and the rgb data.  RGB data is stored
 *      in byte triples in R, G, B order.
 * Inputs:
 *      TUInt8 aPage                -- Page of graphics to draw to
 *      TUInt16 x                   -- x position from left
 *      TUInt16 y                   -- y position from top
 *      TUInt16 aNumPixels          -- Nubmer of pixels to draw
 *      TUInt8 *aRGB                -- RGB triples memory
 * Outputs:
 *      TUInt16                     -- raw pixel format of screen.
 *---------------------------------------------------------------------------*/
void SUIRenderRGBRaster(
    TUInt8 aPage,
    TUInt16 x,
    TUInt16 y,
    TUInt16 aNumPixels,
    TUInt8 *aRGB)
{
    // Copy the window pages into our own working space (so we can change
    // the page without affecting anyone else)
    SWIM_WINDOW_T win = G_SUISettings.iWindow;

    swim_driver_set_page(&win, aPage);

    // Render the pixels, one at a time
    while (aNumPixels--) {
        swim_driver_put_pixel(&win, x++, y,
            SUICallbackRGBConvert(aRGB[2], aRGB[1], aRGB[0]));
        aRGB += 3;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  SUILoadPicture
 *---------------------------------------------------------------------------*
 * Description:
 *      Load a picture from the given file and store at the given page.
 *      The file is assumed to be in Targa uncompressed format and
 *      is the same size as the page.
 * Inputs:
 *      char *aPicture              -- Name of file to use
 *      TUInt8 aPage                -- Page to load the file into
 *      TBool *aAbortFlag           -- Pointer to flag.  If 0, then ignored.
 *                                        Loads while flag is EFalse, else
 *                                        stops
 * Outputs:
 *      int32_t                         -- 1 if error, 2 if aborted, else 0
 *---------------------------------------------------------------------------*/
int32_t SUILoadPicture(
    char *aPicture,
    TUInt8 aPage,
    TBool *aAbortFlag,
    TUInt8 *aLoadAddr)
{
    T_uezFile file;
    TUInt32 num;
    int32_t y = 0;
    int32_t error = 0;
    TUInt32 loadLeft;
    TUInt32 blockSize;
    TUInt8 *p_raster;
    TUInt8 header[18];
    const TUInt32 height = G_SUISettings.iWindow.ypsize;
    const TUInt32 width = G_SUISettings.iWindow.xpsize;

    if (UEZFileOpen(aPicture, FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE) {
        if (UEZFileRead(file, header, 18, &num) != UEZ_ERROR_NONE) {
            // Problem!  Stop here!
            error = 1;
        }
        if (!error) {
            loadLeft = (3 * width * height);
            p_raster = aLoadAddr;
            while (loadLeft) {
                // Do we need to abort?
                if (aAbortFlag) {
                    if (*aAbortFlag != EFalse) {
                        error = 2;
                        break;
                    }
                }
                if (loadLeft > 3 * width)
                    blockSize = 3 * width;
                else
                    blockSize = loadLeft;
                if (UEZFileRead(file, p_raster, blockSize, &num)
                    != UEZ_ERROR_NONE) {
                    // Problem!  Stop here!
                    error = 1;
                    break;
                }
                SUIRenderRGBRaster(aPage, 0, (height - 1 - (y++)), width,
                    p_raster);
                if (y == height)
                    break;
                // No errors, keep going
                loadLeft -= blockSize;
            }
        }

        UEZFileClose(file);
    } else {
        error = 1;
    }

    return error;
}

/*-------------------------------------------------------------------------*
 * File:  Drawing.c
 *-------------------------------------------------------------------------*/
