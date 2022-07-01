/***********************************************************************
 * $Id:: lpc_swim.c 745 2008-05-13 19:59:29Z pdurgesh                  $
 *
 * Project: Simple Windowing Interface Manager (SWIM)
 *
 * Description:
 *     See the swim.h header file for a description of this package.
 *
 ***********************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#include "lpc_swim.h"
#include "lpc_fonts.h"
#include "lpc_swim_driver.h"

/***********************************************************************
 * Private functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: swim_put_line_raw
 *
 * Purpose: Draw a line on the physical display
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win : Window identifier
 *     x1  : Physical X position of X line start
 *     y1  : Physical Y position of Y line start
 *     x2  : Physical X position of X line end
 *     y2  : Physical Y position of Y line end
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_put_line_raw(
    SWIM_WINDOW_T *win,
    INT_32 x1,
    INT_32 y1,
    INT_32 x2,
    INT_32 y2)
{
#define iabs(v) (((v)>=0)?(v):(-(v)))

    int32_t deltax, deltay;
    int32_t x, y;
    int32_t den;
    int32_t xinc1, xinc2, yinc1, yinc2;
    int32_t numpixels;
    int32_t curpixel;
    int32_t numadd;
    int32_t num;

    // The difference between the x's
    deltax = iabs(x2 - x1);
    // The difference between the y's
    deltay = iabs(y2 - y1);
    // Start x off at the first pixel
    x = x1;
    // Start y off at the first pixel
    y = y1;

    if (x2 >= x1) {
        // The x-values are increasing
        xinc1 = 1;
        xinc2 = 1;
    } else {
        // The x-values are decreasing
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1) {
        // The y-values are increasing
        yinc1 = 1;
        yinc2 = 1;
    } else {
        // The y-values are decreasing
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay) {
        // There is at least one x-value for every y-value
        // Don't change the x when numerator >= denominator
        xinc1 = 0;
        // Don't change the y for every iteration
        yinc2 = 0;
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        // There are more x-values than y-values
        numpixels = deltax;
    } else {
        // There is at least one y-value for every x-value
        // Don't change the x for every iteration
        xinc2 = 0;
        // Don't change the y when numerator >= denominator
        yinc1 = 0;
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        // There are more y-values than x-values
        numpixels = deltay;
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        if (((x >= win->xpmin) && (x <= win->xpmax))
            && ((y >= win->ypmin) && (y <= win->ypmax)))
            swim_driver_put_pixel(win, x, y, win->pen);

        // Increase the numerator by the top of the fraction
        num += numadd;
        // Check if numerator >= denominator
        if (num >= den) {
            // Calculate the new numerator value
            num -= den;
            // Change the x as appropriate
            x += xinc1;
            // Change the y as appropriate
            y += yinc1;
        }
        // Change the x as appropriate
        x += xinc2;
        // Change the y as appropriate
        y += yinc2;
    }
}

/***********************************************************************
 *
 * Function: swim_window_open_p
 *
 * Purpose: Initializes a window and the default values for the window
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win          : Preallocated windows structure to fill
 *     xsize        : Physical horizontal dimension of the display
 *     ysize        : Physical vertical dimension of the display
 *     fbaddr       : Address of the display's frame buffer
 *     xwin_min     : Physical window left coordinate
 *     ywin_min     : Physical window top coordinate
 *     xwin_max     : Physical window right coordinate
 *     ywin_max     : Physical window bottom coordinate
 *     border_width : Width of the window border in pixels
 *     pcolor       : Pen color
 *     bkcolor      : Background color
 *     fcolor       : Fill color
 *     clear        : Clear window flag
 *
 * Outputs: None
 *
 * Returns:
 *  TRUE if the window was initialized correctly, otherwise FALSE
 *
 * Notes:
 *     This function must be called prior to any other window function
 *
 **********************************************************************/
BOOL_32 swim_window_open_p(
    SWIM_WINDOW_T *win,
    INT_32 xsize,
    INT_32 ysize,
    COLOR_T *fbaddr,
    INT_32 xwin_min,
    INT_32 ywin_min,
    INT_32 xwin_max,
    INT_32 ywin_max,
    INT_32 border_width,
    COLOR_T pcolor,
    COLOR_T bkcolor,
    COLOR_T fcolor,
    BOOL_32 clear)
{
    INT_32 i;
    BOOL_32 init = FALSE;

    /* Before continuing, check to see that the window size is
     in the physical dimensions of the display */
    if ((xwin_min >= 0) && (ywin_min >= 0) && (xwin_max < xsize)
        && (ywin_max < ysize)) {
        init = TRUE;
    } else {
        /* Window size is out of the physical display size, so it
         should be invalidated */
        win->winused = 0x0;
    }

    if (init == TRUE) {
        /* Save physical display dimensions */
        win->xpsize = xsize;
        win->ypsize = ysize;

        /* Save frame buffer address */
        win->fb0 = win->fb = fbaddr;
        win->page = 0;

        /* Save physical window dimensions and default colors */
        win->xpmin = xwin_min;
        win->ypmin = ywin_min;
        win->xpmax = xwin_max;
        win->ypmax = ywin_max;
        win->pen = pcolor;
        win->bkg = bkcolor;
        win->fill = fcolor;

        /* Compute physical window dimensions of draw area only */
        win->xpvmin = xwin_min + border_width;
        win->ypvmin = ywin_min + border_width;
        win->xpvmax = xwin_max - border_width;
        win->ypvmax = ywin_max - border_width;

        /* Compute virtual window size of draw area */
        win->xvsize = xwin_max - xwin_min - 2 * border_width;
        win->yvsize = ywin_max - ywin_min - 2 * border_width;

        /* Fill in any unused border padding between draw area and border
         will fill color */
        for (i = 0; i < border_width; i++) {
            swim_put_line_raw(win, (xwin_min + i), (ywin_min + i),
                (xwin_max - i), (ywin_min + i));
            swim_put_line_raw(win, (xwin_max - i), (ywin_min + i),
                (xwin_max - i), (ywin_max - i));
            swim_put_line_raw(win, (xwin_max - i), (ywin_max - i),
                (xwin_min + i), (ywin_max - i));
            swim_put_line_raw(win, (xwin_min + i), (ywin_max - i),
                (xwin_min + i), (ywin_min + i));
        }

        /* Clear draw area with background color */
        if (clear == TRUE) {
            swim_clear_screen(win, bkcolor);
        }

        /* Use no default font and make background transparent */
        win->font = 0;

        /* Set starting text position in upper left of window */
        win->xvpos = win->xpvmin;
        win->yvpos = win->ypvmin;

        /* Set fill to NOT be transparent */
        win->tfill = 0;
    }

    return init;
}

/***********************************************************************
 * Public functions
 **********************************************************************/

/***********************************************************************
 *
 * Function: swim_put_pixel
 *
 * Purpose: Puts a pixel at the virtual X, Y coordinate in the window
 *
 * Processing:
 *     Convert the virtual pixel position to a physical position.
 *     If the pixel is inside the window draw area, update the pixel
 *     on the display.
 *
 * Parameters:
 *     win : Window identifier
 *     x1  : Virtual X position of pixel
 *     y1  : Virtual Y position of pixel
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     The pixel will not be displayed if the pixel exceeds the window
 *     virtual size. Pixel positions below 0 should not be used with
 *     this function.
 *
 **********************************************************************/
void swim_put_pixel(SWIM_WINDOW_T *win, INT_32 x1, INT_32 y1)
{
    INT_16 realx, realy;

    /* Convert virtual coordinate to physical coordinate taking into
     consideration the border size of the window */
    realx = win->xpvmin + x1;
    realy = win->ypvmin + y1;

    /* Only put the pixel in the window if it fits in the window */
    if ((realx <= win->xpvmax) && (realy <= win->ypvmax)) {
        swim_driver_put_pixel(win, realx, realy, win->pen);
    }
}

/***********************************************************************
 *
 * Function: swim_put_line
 *
 * Purpose: Draw a line in the virtual window with clipping.
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win : Window identifier
 *     x1  : Virtual X position of X line start
 *     y1  : Virtual Y position of Y line start
 *     x2  : Virtual X position of X line end
 *     y2  : Virtual Y position of Y line end
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     This function supports clipping.
 *
 **********************************************************************/
void swim_put_line(
    SWIM_WINDOW_T *win,
    INT_32 x1,
    INT_32 y1,
    INT_32 x2,
    INT_32 y2)
{
#define iabs(v) (((v)>=0)?(v):(-(v)))

    int32_t deltax, deltay;
    int32_t x, y;
    int32_t den;
    int32_t xinc1, xinc2, yinc1, yinc2;
    int32_t numpixels;
    int32_t curpixel;
    int32_t numadd;
    int32_t num;

    x1 = x1 + win->xpvmin;
    x2 = x2 + win->xpvmin;
    y1 = y1 + win->ypvmin;
    y2 = y2 + win->ypvmin;

    deltax = iabs(x2 - x1);        // The difference between the x's
    deltay = iabs(y2 - y1);        // The difference between the y's
    x = x1;                       // Start x off at the first pixel
    y = y1;                       // Start y off at the first pixel

    if (x2 >= x1)                 // The x-values are increasing
        {
        xinc1 = 1;
        xinc2 = 1;
    } else                          // The x-values are decreasing
    {
        xinc1 = -1;
        xinc2 = -1;
    }

    if (y2 >= y1)                 // The y-values are increasing
        {
        yinc1 = 1;
        yinc2 = 1;
    } else                          // The y-values are decreasing
    {
        yinc1 = -1;
        yinc2 = -1;
    }

    if (deltax >= deltay)     // There is at least one x-value for every y-value
        {
        xinc1 = 0;           // Don't change the x when numerator >= denominator
        yinc2 = 0;                  // Don't change the y for every iteration
        den = deltax;
        num = deltax / 2;
        numadd = deltay;
        numpixels = deltax;         // There are more x-values than y-values
    } else                    // There is at least one y-value for every x-value
    {
        xinc2 = 0;                  // Don't change the x for every iteration
        yinc1 = 0;           // Don't change the y when numerator >= denominator
        den = deltay;
        num = deltay / 2;
        numadd = deltax;
        numpixels = deltay;         // There are more y-values than x-values
    }

    for (curpixel = 0; curpixel <= numpixels; curpixel++) {
        if (((x >= win->xpmin) && (x <= win->xpmax))
            && ((y >= win->ypmin) && (y <= win->ypmax)))
            swim_driver_put_pixel(win, x, y, win->pen);

        num += numadd;      // Increase the numerator by the top of the fraction
        if (num >= den)             // Check if numerator >= denominator
            {
            num -= den;               // Calculate the new numerator value
            x += xinc1;               // Change the x as appropriate
            y += yinc1;               // Change the y as appropriate
        }
        x += xinc2;                 // Change the x as appropriate
        y += yinc2;                 // Change the y as appropriate
    }
}

/***********************************************************************
 *
 * Function: swim_put_diamond
 *
 * Purpose:
 * Purpose: Draw a diamond in the virtual window
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win : Window identifier
 *     x   : Virtual X position of the diamond
 *     y   : Virtual Y position of the diamond
 *     rx  : Radius for horizontal
 *     ry  : Radius for vertical
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     This function supports clipping.
 *
 **********************************************************************/
void swim_put_diamond(
    SWIM_WINDOW_T *win,
    INT_32 x,
    INT_32 y,
    INT_32 rx,
    INT_32 ry)
{
    INT_32 xinc, idy, xleft, xright, pixel, ydsp;

    /* For center line, limit to size of the window */
    xleft = x - rx;
    if (xleft < win->xpvmin) {
        xleft = win->xpvmin;
    }
    xright = x + rx;
    if (xright > win->xpvmax) {
        xright = win->xpvmax;
    }

    /* Draw the left and right pixels of the center line in the pen
     color */
    swim_driver_put_pixel(win, xleft, y + win->ypmin, win->pen);
    swim_driver_put_pixel(win, xright, y + win->ypmin, win->pen);

    /* Draw the center line first in the fill color */
    if ((y >= win->ypvmin) && (y <= win->ypvmax)) {
        for (pixel = (xleft + 1); pixel <= (xright - 1); pixel++) {
            swim_driver_put_pixel(win, pixel, y + win->ypmin, win->fill);
        }
    }

    /* Draw the top and bottom halves of the diamond */
    for (idy = 1; idy <= ry; idy++) {
        /* Compute left and right endpoints for the horizontal line */
        xinc = ((ry - idy) * rx);
        xleft = x - xinc / ry;
        xright = x + xinc / ry;

        /* Clip left and right edges if needed */
        if (xleft < win->xpvmin) {
            xleft = win->xpvmin;
        }
        if (xright > win->xpvmax) {
            xright = win->xpvmax;
        }

        /* Convert virtual coordinates to physical coordinates */
        xleft = xleft + win->xpmin;
        xright = xright + win->xpmin;

        /* Only render top line if it is visible */
        if ((y - idy) >= win->ypvmin) {
            /* Convert to physical coordinate */
            ydsp = ((y - idy) + win->ypmin);

            /* Draw the left pixel of the line in the pen color */
            swim_driver_put_pixel(win, xleft, ydsp, win->pen);

            /* Draw top line in fill color */
            swim_driver_fill_raster(win, xleft + 1, ydsp, (xright - xleft - 1),
                win->fill);

            /* Draw the right pixel of the line in the pen color */
            swim_driver_put_pixel(win, xright, ydsp, win->pen);
        }

        /* Only render bottom line if it is visible */
        if ((y + idy) <= win->ypvmax) {
            /* Convert to physical offset */
            ydsp = ((y + idy) + win->ypmin) * win->xpsize;

            /* Draw the left pixel of the line in the pen color */
            swim_driver_put_pixel(win, xleft, ydsp, win->pen);

            /* Draw top line in fill color */
            swim_driver_fill_raster(win, xleft + 1, ydsp, (xright - xleft - 1),
                win->fill);

            /* Draw the right pixel of the line in the pen color */
            swim_driver_put_pixel(win, xright, ydsp, win->pen);
        }
    }
}

/***********************************************************************
 *
 * Function: swim_put_triangle
 *
 * Purpose:
 * Purpose: Draw a triangle in the virtual window
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win         : Window identifier
 *     x           : Virtual X position of the diamond
 *     y           : Virtual Y position of the diamond
 *     rx          : Radius for horizontal
 *     ry          : Radius for vertical
 *     orientation : Orientation: up, right, down or left
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes:
 *     This function supports clipping.
 *
 **********************************************************************/
void swim_put_triangle(
    SWIM_WINDOW_T *win,
    INT_32 ix,
    INT_32 iy,
    INT_32 irx,
    INT_32 iry,
    T_triangle_orientation orientation)
{
    INT_32 xinc, idy, xleft, xright, ytop, ybot, pixel, ydsp, x, y, rx, ry;

    x = ix;
    y = iy;
    switch (orientation) {
        case up:
            rx = irx;
            ry = (iry << 1);
            xleft = x - rx;
            xright = x + rx;
            ytop = y - ry;
            ybot = y;
            break;
        case right:
            rx = (irx << 1);
            ry = iry;
            xleft = x;
            xright = x + rx;
            ytop = y - ry;
            ybot = y + ry;
            break;
        case down:
            rx = irx;
            ry = (iry << 1);
            xleft = x - rx;
            xright = x + rx;
            ytop = y;
            ybot = y + ry;
            break;
        case left:
            rx = (irx << 1);
            ry = iry;
            xleft = x - rx;
            xright = x;
            ytop = y - ry;
            ybot = y + ry;
            break;
        default:
            return;
    }

    /* For center line, limit to size of the window */
    if (xleft < win->xpvmin) {
        xleft = win->xpvmin;
    }
    if (xright > win->xpvmax) {
        xright = win->xpvmax;
    }

    /* Draw the center lines in the pen color */
    switch (orientation) {
        case up:
            if ((y >= win->ypvmin) && (y <= win->ypvmax)) {
                swim_driver_fill_raster(win, xleft, y, win->pen, 1 + xright - xleft);
            }
            break;
        case right:
            if ((x >= win->xpvmin) && (x <= win->xpvmax)) {
                /* Draw the left and right pixels of the center line in the pen
                 color */
                swim_driver_put_pixel(win, xleft, y, win->pen);
                swim_driver_put_pixel(win, xright, y, win->pen);

                /* Draw the center line first in the fill color */
                if ((y >= win->ypvmin) && (y <= win->ypvmax)) {
                    swim_driver_fill_raster(win, xleft+1, y + win->ypmin, win->fill,
                        xright - xleft - 1);
                }

                /* Draw the vertical line */
                for (pixel = (ytop); pixel < (ybot); pixel++) {
                    swim_driver_put_pixel(win, x, pixel + win->ypmin, win->pen);
                }
            }
            break;
        case down:
            if ((y >= win->ypvmin) && (y <= win->ypvmax)) {
                swim_driver_fill_raster(win, xleft, y + win->ypmin, win->pen,
                    1 + xright - xleft);
            }
            break;
        case left:
            if ((x >= win->xpvmin) && (x <= win->xpvmax)) {
                /* Draw the left and right pixels of the center line in the pen
                 color */
                swim_driver_put_pixel(win, xleft, y, win->pen);
                swim_driver_put_pixel(win, xright, y, win->pen);

                /* Draw the center line first in the fill color */
                if ((y >= win->ypvmin) && (y <= win->ypvmax)) {
                    swim_driver_fill_raster(win, xleft + 1, y + win->ypmin, win->fill,
                        xright - xleft - 1);
                }

                /* Draw the vertical line */
                for (pixel = (ytop); pixel < (ybot); pixel++) {
                    swim_driver_put_pixel(win, x, pixel + win->ypmin, win->pen);
                }
            }
            break;
        default:
            break;
    }

    /* Draw the triangle */
    for (idy = 1; idy <= ry; idy++) {
        /* Compute left and right endpoints for the horizontal line */
        xinc = ((ry - idy) * rx);
        switch (orientation) {
            case up:
                xleft = x - xinc / ry;
                xright = x + xinc / ry;
                break;
            case right:
                xleft = x;
                xright = x + xinc / ry;
                break;
            case down:
                xleft = x - xinc / ry;
                xright = x + xinc / ry;
                break;
            case left:
                xleft = x - xinc / ry;
                xright = x;
                break;
            default:
                break;
        }

        /* Clip left and right edges if needed */
        if (xleft < win->xpvmin) {
            xleft = win->xpvmin;
        }
        if (xright > win->xpvmax) {
            xright = win->xpvmax;
        }

        /* Convert virtual coordinates to physical coordinates */
        xleft = xleft + win->xpmin;
        xright = xright + win->xpmin;

        /* Only render top line if it is visible */
        if ((orientation != down) && ((y - idy) >= win->ypvmin))
        //if ( (orientation == up) && ((y - idy) >= win->ypvmin) )
            {
            /* Convert to physical coordinate */
            ydsp = ((y - idy) + win->ypmin);

            /* Draw the left pixel of the line in the pen color */
            swim_driver_put_pixel(win, xleft, ydsp, win->pen);

            /* Draw top line in fill color */
            swim_driver_fill_raster(win, xleft + 1, ydsp, xright - xleft - 1,
                win->fill);

            /* Draw the right pixel of the line in the pen color */
            swim_driver_put_pixel(win, xright, ydsp, win->pen);
        }

        /* Only render bottom line if it is visible */
        if ((orientation != up) && ((y + idy) <= win->ypvmax))
        //if ( (orientation == down) && ((y + idy) <= win->ypvmax) )
            {
            /* Convert to physical offset */
            ydsp = ((y + idy) + win->ypmin) * win->xpsize;

            /* Draw the left pixel of the line in the pen color */
            swim_driver_put_pixel(win, xleft, ydsp, win->pen);

            /* Draw top line in fill color */
            swim_driver_fill_raster(win, xleft + 1, ydsp, xright - xleft - 1,
                win->fill);

            /* Draw the right pixel of the line in the pen color */
            swim_driver_put_pixel(win, xright, ydsp, win->pen);
        }
    }
}

/***********************************************************************
 *
 * Function: swim_clear_screen
 *
 * Purpose:
 *     Fills the draw area of the display with the selected color
 *
 * Processing:
 *     Loop through all virtual window (draw area) locations and
 *     updates them with the passed color value.
 *
 * Parameters:
 *     win  : Window identifier
 *     colr : Color to place in the window
 *
 * Outputs:  None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_clear_screen(SWIM_WINDOW_T *win, COLOR_T color)
{
    INT_32 y;

    for (y = win->ypvmin; y <= win->ypvmax; y++) {
        swim_driver_fill_raster(win, win->xpvmin, y, color,
            1 + win->xpvmax - win->xpvmin);
    }
}

/***********************************************************************
 *
 * Function: swim_put_box
 *
 * Purpose: Place a box with corners (X1, Y1) and (X2, Y2)
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win : Window identifier
 *     x1  : Virtual left position of box
 *     y1  : Virtual upper position of box
 *     x2  : Virtual right position of box
 *     y2  : Virtual lower position of box
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_put_box(
    SWIM_WINDOW_T *win,
    INT_32 x1,
    INT_32 y1,
    INT_32 x2,
    INT_32 y2)
{
    INT_32 xinc, yinc;

    if (x1 > x2) {
        xinc = x1;
        x1 = x2;
        x2 = xinc;
    }

    /* Swap y1 and y2 if y1 is larger than y2 */
    if (y1 > y2) {
        yinc = y1;
        y1 = y2;
        y2 = yinc;
    }

    /* Convert virtual coordinates to physical coordinates */
    x1 = x1 + win->xpvmin;
    x2 = x2 + win->xpvmin;
    y1 = y1 + win->ypvmin;
    y2 = y2 + win->ypvmin;

    /* Clip boxes to window sizes */
    if (x1 < win->xpvmin) {
        x1 = win->xpvmin;
    }
    if (y1 < win->ypvmin) {
        y1 = win->ypvmin;
    }
    if (x2 > win->xpvmax) {
        x2 = win->xpvmax;
    }
    if (y2 > win->ypvmax) {
        y2 = win->ypvmax;
    }

    /* Get X and Y differences */
    xinc = x2 - x1;
    yinc = y2 - y1;

    /* Make outer edge of box in pen color */
    swim_put_line_raw(win, x1, y1, x2, y1);
    swim_put_line_raw(win, x2, y1, x2, y2);
    swim_put_line_raw(win, x2, y2, x1, y2);
    swim_put_line_raw(win, x1, y2, x1, y1);

    /* Increment X, Y values so they won't overwrite the edge */
    x1++;
    y1++;

    /* Draw inside if NOT transparent */
    if ((!win->tfill) && (x1 < x2)) {
        /* Draw the box inside with the fill color */
        while (y1 < y2) {
            swim_driver_fill_raster(win, x1, y1, win->fill, x2 - x1);
            y1++;
        }
    }
}

/***********************************************************************
 *
 * Function: swim_put_box
 *
 * Purpose: Place a box with corners (X1, Y1) and (X2, Y2)
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win : Window identifier
 *     x1  : Virtual left position of box
 *     y1  : Virtual upper position of box
 *     x2  : Virtual right position of box
 *     y2  : Virtual lower position of box
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
#if 0 // Needs more work
void swim_put_shaded_rectangle(
    SWIM_WINDOW_T *win,
    INT_32 x1,
    INT_32 y1,
    INT_32 x2,
    INT_32 y2,
    T_swimAlpha alpha)
{
    INT_32 xinc, yinc;
    INT_32 ysave;

    if (x1 > x2) {
        xinc = x1;
        x1 = x2;
        x2 = xinc;
    }

    /* Swap y1 and y2 if y1 is larger than y2 */
    if (y1 > y2) {
        yinc = y1;
        y1 = y2;
        y2 = yinc;
    }

    /* Convert virtual coordinates to physical coordinates */
    x1 = x1 + win->xpvmin;
    x2 = x2 + win->xpvmin;
    y1 = y1 + win->ypvmin;
    y2 = y2 + win->ypvmin;

    /* Clip boxes to window sizes */
    if (x1 < win->xpvmin) {
        x1 = win->xpvmin;
    }
    if (y1 < win->ypvmin) {
        y1 = win->ypvmin;
    }
    if (x2 > win->xpvmax) {
        x2 = win->xpvmax;
    }
    if (y2 > win->ypvmax) {
        y2 = win->ypvmax;
    }

    /* Get X and Y differences */
    xinc = x2 - x1;
    yinc = y2 - y1;

    /* Make outer edge of box in pen color */
//  swim_put_line_raw(win, x1, y1, x2, y1);
//  swim_put_line_raw(win, x2, y1, x2, y2);
//  swim_put_line_raw(win, x2, y2, x1, y2);
//  swim_put_line_raw(win, x1, y2, x1, y1);
    /* Increment X, Y values so they won't overwrite the edge */
//  x1++;
//  y1++;
    /* Draw inside if NOT transparent */
    if (!win->tfill) {
        /* Draw the box inside with the fill color */
        ysave = y1;
        while (x1 <= x2) {
            y1 = ysave;
            while (y1 <= y2) {
                swim_driver_fill_alpha_raster(win, x1, y1, win->fill, alpha);
                y1++;
            }

            x1++;
        }
    }
}
#endif

/***********************************************************************
 *
 * Function: swim_window_open
 *
 * Purpose: Initializes a window and the default values for the window
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win          : Preallocated windows structure to fill
 *     xsize        : Physical horizontal dimension of the display
 *     ysize        : Physical vertical dimension of the display
 *     fbaddr       : Address of the display's frame buffer
 *     xwin_min     : Physical window left coordinate
 *     ywin_min     : Physical window top coordinate
 *     xwin_max     : Physical window right coordinate
 *     ywin_max     : Physical window bottom coordinate
 *     border_width : Width of the window border in pixels
 *     pcolor       : Pen color
 *     bkcolor      : Background color
 *     fcolor       : Fill color
 *
 * Outputs: None
 *
 * Returns:
 *  TRUE if the window was initialized correctly, otherwise FALSE
 *
 * Notes:
 *     This function must be called prior to any other window function
 *
 **********************************************************************/
BOOL_32 swim_window_open(
    SWIM_WINDOW_T *win,
    INT_32 xsize,
    INT_32 ysize,
    COLOR_T *fbaddr,
    INT_32 xwin_min,
    INT_32 ywin_min,
    INT_32 xwin_max,
    INT_32 ywin_max,
    INT_32 border_width,
    COLOR_T pcolor,
    COLOR_T bkcolor,
    COLOR_T fcolor)
{
    BOOL_32 init;

    init = swim_window_open_p(win, xsize, ysize, fbaddr, xwin_min, ywin_min,
        xwin_max, ywin_max, border_width, pcolor, bkcolor, fcolor, TRUE);

    /* Default font background is not transparent */
    win->tfont = 1;

    return init;
}

/***********************************************************************
 *
 * Function: swim_window_open_noclear
 *
 * Purpose: Initializes a window and the default values for the window
 *
 * Processing:
 *     See function.
 *
 * Parameters:
 *     win          : Preallocated windows structure to fill
 *     xsize        : Physical horizontal dimension of the display
 *     ysize        : Physical vertical dimension of the display
 *     fbaddr       : Address of the display's frame buffer
 *     xwin_min     : Physical window left coordinate
 *     ywin_min     : Physical window top coordinate
 *     xwin_max     : Physical window right coordinate
 *     ywin_max     : Physical window bottom coordinate
 *     border_width : Width of the window border in pixels
 *     pcolor       : Pen color
 *     bkcolor      : Background color
 *     fcolor       : Fill color
 *
 * Outputs: None
 *
 * Returns:
 *  TRUE if the window was initialized correctly, otherwise FALSE
 *
 * Notes:
 *     This function must be called prior to any other window function
 *
 **********************************************************************/
BOOL_32 swim_window_open_noclear(
    SWIM_WINDOW_T *win,
    INT_32 xsize,
    INT_32 ysize,
    COLOR_T *fbaddr,
    INT_32 xwin_min,
    INT_32 ywin_min,
    INT_32 xwin_max,
    INT_32 ywin_max,
    INT_32 border_width,
    COLOR_T pcolor,
    COLOR_T bkcolor,
    COLOR_T fcolor)
{
    BOOL_32 init;

    init = swim_window_open_p(win, xsize, ysize, fbaddr, xwin_min, ywin_min,
        xwin_max, ywin_max, border_width, pcolor, bkcolor, fcolor, FALSE);

    /* Default font background is transparent */
    win->tfont = 1;

    return init;
}

/***********************************************************************
 *
 * Function: swim_window_close
 *
 * Purpose: Reallocates a window for use
 *
 * Processing:
 *     For the passed window ID, clear the window used flag.
 *
 * Parameters:
 *     win : Window identifier
 *
 * Outputs:  None
 *
 * Returns:  Nothing
 *
 * Notes:
 *     This is a defunct function and is not needed.
 *
 **********************************************************************/
void swim_window_close(SWIM_WINDOW_T *win)
{
    win->winused = 0x0;
}

/***********************************************************************
 *
 * Function: swim_set_pen_color
 *
 * Purpose: Sets the pen color
 *
 * Processing:
 *     For the passed window ID, update to the passed pen color.
 *
 * Parameters:
 *     win       : Window identifier
 *     pen_color : New pen color
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_set_pen_color(SWIM_WINDOW_T *win, COLOR_T pen_color)
{
    win->pen = pen_color;
}

/***********************************************************************
 *
 * Function: swim_get_pen_color
 *
 * Purpose: Gets the pen color
 *
 * Processing:
 *     For the passed window ID, returns the pen color.
 *
 * Parameters:
 *     win       : Window identifier
 *
 * Outputs: None
 *
 * Returns: 
 *     pen_color : New pen color
 *
 * Notes: None
 *
 **********************************************************************/
COLOR_T swim_get_pen_color(SWIM_WINDOW_T *win)
{
    return win->pen;
}

/***********************************************************************
 *
 * Function: swim_set_fill_color
 *
 * Purpose: Sets the fill color
 *
 * Processing:
 *     For the passed window ID, update to the passed fill color.
 *
 * Parameters:
 *     win        : Window identifier
 *     fill_color : New fill color
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_set_fill_color(SWIM_WINDOW_T *win, COLOR_T fill_color)
{
    win->fill = fill_color;
}

/***********************************************************************
 *
 * Function: swim_get_fill_color
 *
 * Purpose: Gets the fill color
 *
 * Processing:
 *     For the passed window ID, returns the passed fill color.
 *
 * Parameters:
 *     win        : Window identifier
 *
 * Outputs: None
 *
 * Returns:
 *     fill_color : New fill color
 *
 * Notes: None
 *
 **********************************************************************/
COLOR_T swim_get_fill_color(SWIM_WINDOW_T *win)
{
    return win->fill;
}

void swim_set_fill_transparent(SWIM_WINDOW_T *win, BOOL_32 transparent)
{
    win->tfill = transparent;
}

BOOL_32 swim_get_fill_transparent(SWIM_WINDOW_T *win)
{
    return win->tfill;
}

/***********************************************************************
 *
 * Function: swim_set_bkg_color
 *
 * Purpose: Sets the color used for backgrounds
 *
 * Processing:
 *     For the passed window ID, update to the passed background color.
 *
 * Parameters:
 *     win        : Window identifier
 *     tbkg_color : New background color
 *
 * Outputs: None
 *
 * Returns: Nothing
 *
 * Notes: None
 *
 **********************************************************************/
void swim_set_bkg_color(SWIM_WINDOW_T *win, COLOR_T bkg_color)
{
    win->bkg = bkg_color;
}

/***********************************************************************
 *
 * Function: swim_get_bkg_color
 *
 * Purpose: Gets the color used for backgrounds
 *
 * Processing:
 *     For the passed window ID, returns the passed background color.
 *
 * Parameters:
 *     win        : Window identifier
 *
 * Outputs: None
 *
 * Returns:
 *     tbkg_color : New background color
 *
 * Notes: None
 *
 **********************************************************************/
COLOR_T swim_get_bkg_color(SWIM_WINDOW_T *win)
{
    return win->bkg;
}

/***********************************************************************
 *
 * Function: swim_get_horizontal_size
 *
 * Purpose: Get the virtual window horizontal size
 *
 * Processing:
 *     For the passed window ID, return the x size of the window.
 *
 * Parameters:
 *     win        : Window identifier
 *
 * Outputs: None
 *
 * Returns: The virtual window horizontal size
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 swim_get_horizontal_size(SWIM_WINDOW_T *win)
{
    return win->xvsize;
}

/***********************************************************************
 *
 * Function: swim_get_vertical_size
 *
 * Purpose: Get the virtual window vertical size
 *
 * Processing:
 *     For the passed window ID, return the x size of the window.
 *
 * Parameters:
 *     win        : Window identifier
 *
 * Outputs: None
 *
 * Returns: The virtual window horizontal size
 *
 * Notes: None
 *
 **********************************************************************/
INT_32 swim_get_vertical_size(SWIM_WINDOW_T *win)
{
    return win->yvsize;
}

void swim_get_virtual_xy(SWIM_WINDOW_T *win, INT_32 *x, INT_32 *y)
{
    *y -= win->ypvmin;
    *x -= win->xpvmin;
}

void swim_get_physical_xy(SWIM_WINDOW_T *win, INT_32 *x, INT_32 *y)
{
    *y += win->ypvmin;
    *x += win->xpvmin;
}

