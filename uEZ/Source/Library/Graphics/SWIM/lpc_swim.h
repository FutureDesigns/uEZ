/**********************************************************************
 * $Id:: lpc_swim.h 745 2008-05-13 19:59:29Z pdurgesh                  $
 *
 * Project: Simple Windowing Interface Manager (SWIM)
 *
 * Description:
 *     This package provides a simple windows manager that provides the
 *     following functions:
 *         Windows initialization and validity checks
 *             Must be in physical display space
 *         Color support for background, primary pen, and fill
 *         Simple graphics primatives (pixels, lines, boxes)
 *         Window deallocation
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

#ifndef LPC_SWIM_H
#define LPC_SWIM_H

#include <Config.h>
#include <uEZLCD.h>

#include "lpc_types.h"
#include "lpc_fonts.h"
#include "lpc_colors.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * Window information structure
 **********************************************************************/

/* Structure is used to store information about a specific window */
typedef struct
{
  INT_32 xpsize;   /* Physical (absolute) horizontal screen size */
  INT_32 ypsize;   /* Physical (absolute) vertical screen size */
  INT_32 xpmin;    /* Physical left edge of window */
  INT_32 ypmin;    /* Physical top edge of window */
  INT_32 xpmax;    /* Physical right edge of window */
  INT_32 ypmax;    /* Physical bottom edge of window */
  INT_32 bdsize;   /* Size of window frame in pixels */
  INT_32 xvsize;   /* Virtual horizontal window size */
  INT_32 yvsize;   /* Virtual vertical window size */
  INT_32 xpvmin;   /* Physical left edge of draw window */
  INT_32 ypvmin;   /* Physical top edge of draw window */
  INT_32 xpvmax;   /* Physical right edge of draw window */
  INT_32 ypvmax;   /* Physical bottom edge of draw window */
  INT_32 xvpos;    /* Next virtual 'x' position of output */
  INT_32 yvpos;    /* Next virtual 'y' position of output */
  COLOR_T pen;     /* Pen/text color */
  COLOR_T bkg;     /* Window/text background color */
  COLOR_T fill;    /* Fill/border color */
  BOOL_32 tfill;   /* Transparent fill */
  const FONT_T *font;    /* Selected font structure */
  BOOL_32 tfont;    /* Transparent font background flag when true */
  COLOR_T *fb;     /* Frame buffer address for the physical display */
  INT_32 winused;  /* Window used flag */
  UNS_16 page;       /* Current page */
  COLOR_T *fb0;     /* Frame buffer address for page 0 */
} SWIM_WINDOW_T;

typedef enum {
  SWIM_ALPHA_50,
} T_swimAlpha;


/***********************************************************************
 * Graphics primative drawing functions
 **********************************************************************/

/* Puts a pixel at (X, Y) in the pen color */
void swim_put_pixel(SWIM_WINDOW_T *win,
                    INT_32 x1,
                    INT_32 y1);

/* Draw a line in the virtual window */
void swim_put_line(SWIM_WINDOW_T *win,
                   INT_32 x1,
                   INT_32 y1,
                   INT_32 x2,
                   INT_32 y2);

/* Draw a diamond in the virtual window */
void swim_put_diamond(SWIM_WINDOW_T *win,
                      INT_32 x,
                      INT_32 y,
                      INT_32 rx,
                      INT_32 ry);

enum TRIANGLE_ORIENTATION {up, right, down, left};
typedef enum TRIANGLE_ORIENTATION T_triangle_orientation;

/* Draw a triangle in the virtual window */
void swim_put_triangle(SWIM_WINDOW_T *win,
                      INT_32 x,
                      INT_32 y,
                      INT_32 rx,
                      INT_32 ry,
                      T_triangle_orientation orientation);

/* Fills the draw area of the display with the selected color */
void swim_clear_screen(SWIM_WINDOW_T *win,
                       COLOR_T colr);

/* Place a box with corners (X1, Y1) and (X2, Y2). Use pen color
   for edges and fill color for center */
void swim_put_box(SWIM_WINDOW_T *win,
                  INT_32 x1,
                  INT_32 y1,
                  INT_32 x2,
                  INT_32 y2);

/***********************************************************************
 * Initialization, selection, and de-initialization functions
 **********************************************************************/

/* Initialize a window */
BOOL_32 swim_window_open(SWIM_WINDOW_T *win,
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
                         COLOR_T fcolor);

/* Initialize a window without clearing it */
BOOL_32 swim_window_open_noclear(SWIM_WINDOW_T *win,
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
                                 COLOR_T fcolor);

/* Destroy a window */
void swim_window_close(SWIM_WINDOW_T *win);

/* Set the pen color */
void swim_set_pen_color(SWIM_WINDOW_T *win,
                        COLOR_T pen_color);

/* Get the pen color */
COLOR_T swim_get_pen_color(SWIM_WINDOW_T *win);

/* Set fill color (used for boxes and circles) */
void swim_set_fill_color(SWIM_WINDOW_T *win,
                         COLOR_T fill_color);

/* Get fill color (used for boxes and circles) */
COLOR_T swim_get_fill_color(SWIM_WINDOW_T *win);

/* Choose transparent or not */
void swim_set_fill_transparent(SWIM_WINDOW_T *win, BOOL_32 transparent);

/* Return transparency setting */
BOOL_32 swim_get_fill_transparent(SWIM_WINDOW_T *win);

/* Set background color */
void swim_set_bkg_color(SWIM_WINDOW_T *win,
                        COLOR_T bkg_color);

/* Get background color */
COLOR_T swim_get_bkg_color(SWIM_WINDOW_T *win);

void swim_put_shaded_rectangle(SWIM_WINDOW_T *win,
                  INT_32 x1,
                  INT_32 y1,
                  INT_32 x2,
                  INT_32 y2,
                  T_swimAlpha alpha);

/***********************************************************************
 * Information functions
 **********************************************************************/

/* Get the virtual window horizontal size */
INT_32 swim_get_horizontal_size(SWIM_WINDOW_T *win);

/* Get the virtual window vertical size */
INT_32 swim_get_vertical_size(SWIM_WINDOW_T *win);

/* Convert physical to virtual points */
void swim_get_virtual_xy(SWIM_WINDOW_T *win, INT_32 *x, INT_32 *y);

/* Convert virtual to physical points */
void swim_get_physical_xy(SWIM_WINDOW_T *win, INT_32 *x, INT_32 *y);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#include "lpc_swim_font.h"

#include "lpc_swim_driver.h"


#endif /* LPC_SWIM_H */
