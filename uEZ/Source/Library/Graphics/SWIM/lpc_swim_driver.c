/*-------------------------------------------------------------------------*
 * File:  lpc_swim_driver.c
 *-------------------------------------------------------------------------*
 * Description: Screen Driver for drawing all SWIM based items.
 *  Assumes COLOR_T is defined as the proper pixel resolution type.
 *  This version has a runtime configuration of the screen orientation.
 *-------------------------------------------------------------------------*/

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

/**
 *  @addtogroup lpc_swim_driver
 *  @{
 *  @brief     uEZ SWIM Screen Driver
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *  The uEZ to LPC SWIM graphics "driver" routines.  The goal of this module
 *  is to provide low level access of the frame buffer in the most simplest
 *  of coordinate systesm (i.e., 0, 0 is top left).  It's goal is to do
 *  simple translates to make the screen appear flipped or rotated as
 *  needed.
 *
 *  The routines take SWIM_WINDOW_T structure for tracking
 *  state of color and screen configuration.  The routines do not look at
 *  the structure to determine pixel locations (that is done by the higher
 *  level routines).  All coordinates are specified based on where they
 *  should be on the screen with an upper left coordinate of (0, 0) and
 *  a lower right coordinate of (X max-1, Y max-1).  Rasters are defined
 *  as horizontal strips of pixels.  Transformations such as flipping the
 *  X or the Y are performed within this driver.
 *
 * @par Example code:
 * Example to do basic drawing of a blue pixel in the upper left in a window
 * with nothing.
 *
 * @par
 * @code
 * #include <lpc_swim.h>
 * #include <lpc_swim_driver.h>
 *
 * SWIM_WINDOW_T win;
 *
 * swim_driver_set_orientation(0, 0);
 * swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 * swim_driver_put_pixel(&win, 0, 0, BLUE);
 *
 * @endcode
 */

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <string.h>
#include "lpc_swim.h"
#include "lpc_swim_driver.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
// Track if we are flipping the X direction
static BOOL_8 G_flipX;
// Track if we are flipping the Y direction
static BOOL_8 G_flipY;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_put_pixel
 *---------------------------------------------------------------------------*/
/**
 *  Draw a single pixel on the screen.  This routine is used for swim
 *  routines that draw lines and special shapes such as triangles and
 *  diamonds.
 *
 *  @param [in]    win          Structure with screen configuration.
 *  @param [in]    x            X position on screen to draw pixel
 *  @param [in]    y            Y position on screen to draw pixel
 *  @param [in]    color        Color of pixel to draw
 *
 *  @return        void
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *
 *  swim_driver_set_orientation(0, 0);
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *
 *  // Draw a single blue pixel at (0, 0)
 *  swim_driver_put_pixel(&win, 0, 0, BLUE);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_put_pixel(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    COLOR_T color)
{
    if (G_flipX)
        x = win->xpsize - 1 - x;
    if (G_flipY)
        y = win->ypsize - 1 - y;
    (win->fb + y * win->xpsize)[x] = color;
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_copy_raster
 *---------------------------------------------------------------------------*/
/**
 *  Copy a single raster/strip from one place on the screen to another place.
 *  This routine is usually used for scrolling and moving large sections of
 *  the screen.
 *
 *  @param [in]    win          Structure with screen configuration.
 *  @param [in]    xSource      X position to copy from
 *  @param [in]    ySource      Y position to copy from
 *  @param [in]    xDestination X position to copy to
 *  @param [in]    yDestination Y position to copy to
 *  @param [in]    numPixels    Number of pixels to copy in this raster.
 *
 *  @return        void
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *
 *  swim_driver_set_orientation(0, 0);
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *
 *  // Copy pixels from (0, 0) -> (99, 0) to (10, 20) -> (109, 20)
 *  swim_driver_copy_raster(&win, 0, 0, 10, 20, 100);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_copy_raster(
    SWIM_WINDOW_T *win,
    UNS_32 xSource,
    UNS_32 ySource,
    UNS_32 xDestination,
    UNS_32 yDestination,
    UNS_32 numPixels)
{
    COLOR_T *pDestination;
    COLOR_T *pSource;

    if (G_flipX) {
        xSource = win->xpsize - 1 - (xSource + numPixels - 1);
        xDestination = win->xpsize - 1 - (xDestination + numPixels - 1);
    }
    if (G_flipY) {
        ySource = win->ypsize - 1 - ySource;
        yDestination = win->ypsize - 1 - yDestination;
    }
    pSource = win->fb + ySource * win->xpsize + xSource;
    pDestination = win->fb + yDestination * win->xpsize + xDestination;
    memcpy(pDestination, pSource, numPixels * sizeof(COLOR_T));
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_fill_raster
 *---------------------------------------------------------------------------*/
/**
 *  Fill a single horizontal raster of pixels with a given color.  This
 *  routine is used by many SWIM routines that fill areas of the screen.
 *
 *  @param [in]    win          Structure with screen configuration.
 *  @param [in]    x            X position of start of raster
 *  @param [in]    y            Y position to start of raster
 *  @param [in]    color        Color to fill this raster
 *  @param [in]    numPixels    Number of pixels to fill to the right
 *                              of start pixel (including start).
 *
 *  @return        void
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *
 *  swim_driver_set_orientation(0, 0);
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *
 *  // Fill pixels from (0, 0) -> (99, 0) with blue color
 *  swim_driver_copy_raster(&win, 0, 0, BLUE, 100);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_fill_raster(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    COLOR_T color,
    UNS_32 numPixels)
{
    COLOR_T *p;
    if (G_flipX)
        x = win->xpsize - 1 - (x + numPixels - 1);
    if (G_flipY)
        y = win->ypsize - 1 - y;

    p = win->fb + y * win->xpsize + x;
    while (numPixels--) {
        *(p++) = color;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_put_transparent_bit_pattern
 *---------------------------------------------------------------------------*/
/**
 *  Draw a short raster of pixels based on the given pattern.  If the pattern
 *  has a bit of 1, then draw the pixel.  If the pattern has a bit of 0,
 *  skip the pixel.  Go up to the number of bits provided (max of 32).
 *
 *  @param [in]    win          Structure with screen configuration.
 *  @param [in]    x            X position of start of raster
 *  @param [in]    y            Y position to start of raster
 *  @param [in]    pattern      32-bit pattern starting with highest bit
 *  @param [in]    numBits      Number of pixels in pattern
 *
 *  @return        void
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *
 *  swim_driver_set_orientation(0, 0);
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *
 *  // Draw 4-pixel pattern 1101 from (0, 0) -> (3, 0) with
 *  // blue blue skip blue
 *  swim_set_pen_color(&win, BLUE);
 *  swim_driver_put_bit_transparent_pattern(&win, 0, 0, 0xD0000000, BLUE, 4);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_put_transparent_bit_pattern(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    UNS_32 pattern,
    UNS_8 numBits)
{
    COLOR_T *p;
    COLOR_T penColor = win->pen;

    if (G_flipY)
        y = win->ypsize - 1 - y;
    p = win->fb + y * win->xpsize;

    if (G_flipX) {
        // Start on the opposite end
        p += win->xpsize - 1 - x;
        while (numBits--) {
            if (pattern & 0x80000000) {
                *p = penColor;
            }
            p--;
            pattern <<= 1;
        }
    } else {
        // Start at the left most edge
        p += x;
        while (numBits--) {
            if (pattern & 0x80000000) {
                *p = penColor;
            }
            p++;
            pattern <<= 1;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_put_opaque_bit_pattern
 *---------------------------------------------------------------------------*/
/**
 *  Draw a short raster of pixels based on the given pattern.  If the pattern
 *  has a bit of 1, then draw the pen color.  If the pattern has a bit of 0,
 *  draw the fill color.  Go up to the number of bits provided (max of 32).
 *
 *  @param [in]    win          Structure with screen configuration.
 *  @param [in]    x            X position of start of raster
 *  @param [in]    y            Y position to start of raster
 *  @param [in]    pattern      32-bit pattern starting with highest bit
 *  @param [in]    numBits      Number of pixels in pattern
 *
 *  @return        void
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *
 *  swim_driver_set_orientation(0, 0);
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *
 *  // Draw 4-pixel pattern 1101 from (0, 0) -> (3, 0) with
 *  // blue blue white blue
 *  swim_set_pen_color(&win, BLUE);
 *  swim_set_fill_color(&win, WHITE);
 *  swim_driver_put_bit_transparent_pattern(&win, 0, 0, 0xD0000000, 4);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_put_opaque_bit_pattern(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    UNS_32 pattern,
    UNS_8 numBits)
{
    COLOR_T *p;
    COLOR_T penColor = win->pen;
    COLOR_T backgroundColor = win->bkg;

    if (G_flipY)
        y = win->ypsize - 1 - y;
    p = win->fb + y * win->xpsize;

    if (G_flipX) {
        // Start on the opposite end
        p += win->xpsize - 1 - x;
        while (numBits--) {
            if (pattern & 0x80000000) {
                *p = penColor;
            } else {
                *p = backgroundColor;
            }
            p--;
            pattern <<= 1;
        }
    } else {
        // Start at the left most edge
        p += x;
        while (numBits--) {
            if (pattern & 0x80000000) {
                *p = penColor;
            } else {
                *p = backgroundColor;
            }
            p++;
            pattern <<= 1;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_put_raster
 *---------------------------------------------------------------------------*/
/**
 *  Copy a horizontal raster of pixels from the given memory location.
 *
 *  @param [in]    *win          Structure with screen configuration.
 *  @param [in]    x            X position of start of raster
 *  @param [in]    y            Y position to start of raster
 *  @param [in]    *pixels       Pixels to draw horizontally.
 *  @param [in]    numPixels     Number of pixels
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *  const COLOR_T pixels[4] = { BLUE, BLUE, WHITE, BLUE };
 *
 *  swim_driver_set_orientation(0, 0);
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *
 *  // Draw 4-pixel pattern 1101 from (0, 0) -> (3, 0) from memory
 *  swim_driver_put_raster(&win, 0, 0, pixels, 4);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_put_raster(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    const COLOR_T *pixels,
    UNS_32 numPixels)
{
    COLOR_T *p;

    if (G_flipY)
        y = win->ypsize - 1 - y;
    p = win->fb + y * win->xpsize;
    if (G_flipX) {
        // Start on the opposite end
        p += win->xpsize - 1 - x;
        while (numPixels--) {
            *p = *(pixels++);
            p--;
        }
    } else {
        // Copy the pixels directly
        memcpy(p, pixels, numPixels * sizeof(COLOR_T));
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_get_raster
 *---------------------------------------------------------------------------*/
/**
 *  Copy a horizontal raster of pixels from the screen to the given
 *  memory location.
 *
 *  @param [in]    *win          Structure with screen configuration.
 *  @param [in]    x            X position of start of raster
 *  @param [in]    y            Y position to start of raster
 *  @param [in]    pixels       Place to store retrieved pixels
 *  @param [in]    numPixels     Number of pixels to get
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *  const COLOR_T pixels[4];
 *
 *  swim_driver_set_orientation(0, 0);
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *
 *  // Get pixels from (0, 0) -> (3, 0) and into memory
 *  swim_driver_get_raster(&win, 0, 0, pixels, 4);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_get_raster(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    COLOR_T *pixels,
    UNS_32 numPixels)
{
    COLOR_T *p;

    if (G_flipY)
        y = win->ypsize - 1 - y;
    p = win->fb + y * win->xpsize;
    if (G_flipX) {
        // Start on the opposite end
        p += win->xpsize - 1 - x;
        while (numPixels--) {
            *(pixels++) = *p;
            p--;
        }
    } else {
        p += x;
        // Copy the pixels directly
        memcpy(pixels, p, numPixels * sizeof(COLOR_T));
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_put_raster_indexed_color
 *---------------------------------------------------------------------------*/
/**
 *  Copy a horizontal raster of 256-color indexed color pixels from the given
 *  memory location.
 *
 *  @param [in]    *win          Structure with screen configuration.
 *  @param [in]    x            X position of start of raster
 *  @param [in]    y            Y position to start of raster
 *  @param [in]    *pixels       Pixels to draw horizontally.
 *  @param [in]    *palette      Array of color values
 *  @param [in]    numPixels     Number of pixels
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *  const COLOR_T palette[3] = { BLACK, BLUE, WHITE };
 *  const TUInt8 pixels[4] = { 1, 1, 2, 0 };
 *
 *  swim_driver_set_orientation(0, 0);
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *
 *  // Draw 4-pixel pattern 1101 from (0, 0) -> (3, 0) from memory
 *  // as blue blue white black
 *  swim_driver_put_raster_index_color(&win, 0, 0, pixels, 4, palette);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_put_raster_indexed_color(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    const TUInt8 *pixels,
    COLOR_T *palette,
    UNS_32 numPixels)
{
    COLOR_T *p;

    if (G_flipY)
        y = win->ypsize - 1 - y;
    p = win->fb + y * win->xpsize;
    if (G_flipX) {
        // Start on the opposite end
        p += win->xpsize - 1 - x;
        while (numPixels--) {
            *p = palette[*(pixels++)];
            p--;
        }
    } else {
        p += x;
        while (numPixels--) {
            *(p++) = palette[*(pixels++)];
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_set_orientation
 *---------------------------------------------------------------------------*/
/**
 *  Declare which way the orientation should be for the screen (and all
 *  SWIM windows).
 *
 *  @param [in]    flipX        0 if regular X orientation, non-zero for reverse
 *  @param [in]    flipY        0 if regular Y orientation, non-zero for reverse
 *
 *  @return        void
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *
 *  // Flip screen both X and Y (turning it 180 degress)
 *  swim_driver_set_orientation(1, 1);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_set_orientation(BOOL_8 flipX, BOOL_8 flipY)
{
    G_flipX = flipX;
    G_flipY = flipY;
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_get_orientation
 *---------------------------------------------------------------------------*/
/**
 *  Recall which way the screen is oriented.
 *
 *  @param [in]    *flipX       0 if regular X orientation, non-zero for reverse
 *  @param [in]    *flipY       0 if regular Y orientation, non-zero for reverse
 *
 *  @return        void
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *  BOOL_8 flip_x, flip_y;
 *
 *  // Flip screen both X and Y (turning it 180 degress)
 *  swim_driver_get_orientation(&flip_x, &flip_y);
 *  if (flip_x)
 *      printf("Screen is flipped along the X\n");
 *  else
 *      printf("Screen is NOT flipped along the X\n");
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_get_orientation(BOOL_8 *flipX, BOOL_8 *flipY)
{
    if (flipX)
        *flipX = G_flipX;
    if (flipY)
        *flipY = G_flipY;
}

/*---------------------------------------------------------------------------*
 * Routine:  swim_driver_set_page
 *---------------------------------------------------------------------------*/
/**
 *  Changes which frame page to draw to.  A page is a complete screen
 *  that is at the end of the start frame buffer.
 *
 *  @param [in]    win          Structure with screen configuration.
 *  @param [in]    page         Page index to change to.
 *
 *  @return        void
 *
 *  @par Example Code:
 *  @code
 *  #include <lpc_swim.h>
 *  #include <lpc_swim_driver.h>
 *
 *  SWIM_WINDOW_T win;
 *
 *  // Change to the background (unseen) page 1.
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *  swim_driver_set_page(&win, 1);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void swim_driver_set_page(SWIM_WINDOW_T *win, UNS_16 page)
{
    // Move to the next page
    win->fb = win->fb0 + (win->xpsize * win->ypsize) * page;
    win->page = page;
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  lpc_swim_driver.c
 *-------------------------------------------------------------------------*/
