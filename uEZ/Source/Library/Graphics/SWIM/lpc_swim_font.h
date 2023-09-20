/***********************************************************************
 * $Id:: lpc_swim_font.h 745 2008-05-13 19:59:29Z pdurgesh             $
 *
 * Project: Font management for SWIM
 *
 * Description:
 *     This package provides the following font capabilities with SWIM:
 *         Font selection
 *         Text positioning
 *         newline and window scrolling
 *         Text display with multiple, selectable fonts
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

#ifndef LPC_SWIM_FONT_H
#define LPC_SWIM_FONT_H

#include "lpc_types.h"
#include "lpc_swim.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * Text drawing functions
 **********************************************************************/

/* Put a text message at an X, Y pixel coordinate in the window */
void swim_put_text_xy(SWIM_WINDOW_T *win,
                      const CHAR *text,
                      INT_32 x,
                      INT_32 y);

/* Put a horizontal centered text message at a Y pixel coordinate in the window */
void swim_put_text_horizontal_centered(SWIM_WINDOW_T *win,
                      const CHAR *text,
                      INT_32 displayWidth,
                      INT_32 y);

/* Sets the X, Y pixel coordinates for the next text operation */
void swim_set_xy(SWIM_WINDOW_T *win,
                 INT_32 x,
                 INT_32 y);

/* Returns the X, Y pixel coordinates for the next text operation */
void swim_get_xy(SWIM_WINDOW_T *win,
                 INT_32 *x,
                 INT_32 *y);

/* Puts a null-terminated string of text in a window */
void swim_put_text(SWIM_WINDOW_T *win,
                   const CHAR *text);

/* Puts a null-terminated string of text in a window, but will move an
   entire word to the next line if it will not fit on the present
   line */
void swim_put_ltext(SWIM_WINDOW_T *win,
                    const CHAR *text);

/* Puts a single character to the window */
void swim_put_char(SWIM_WINDOW_T *win,
                   const CHAR textchar);

/* Puts a newline in the window */
void swim_put_newline(SWIM_WINDOW_T *win);

/* Select the active font */
void swim_set_font(SWIM_WINDOW_T *win,
                   const FONT_T *font);

/* Returns the active font's height in pixels */
INT_16 swim_get_font_height(SWIM_WINDOW_T *win);
INT_16 swim_get_font_char_width(SWIM_WINDOW_T *win, const CHAR textchar);

/* Create a title bar */
void swim_set_title(SWIM_WINDOW_T *win,
                    const CHAR *title,
                    COLOR_T ttlbkcolor);

/* Enables and disables font backgrounds */
void swim_set_font_transparency(SWIM_WINDOW_T *win, BOOL_32 trans);

/* Returns status of font background transparency setting */
BOOL_32 swim_get_font_transparency(SWIM_WINDOW_T *win);

UNS_32 swim_get_text_line_width(SWIM_WINDOW_T *win, const CHAR *line);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_SWIM_FONT_H */
