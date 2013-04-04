/*-------------------------------------------------------------------------*
 * File:  lpc_swim_internal.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC_SWIM_INTERNAL_H_
#define LPC_SWIM_INTERNAL_H_

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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void swim_driver_put_pixel(SWIM_WINDOW_T *win, UNS_32 x, UNS_32 y, COLOR_T color);
void swim_driver_put_opaque_bit_pattern(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    UNS_32 pattern,
    UNS_8 numBits);
void swim_driver_put_transparent_bit_pattern(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    UNS_32 pattern,
    UNS_8 numBits);
void swim_driver_fill_raster(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    COLOR_T color,
    UNS_32 numPixels);
void swim_driver_copy_raster(
    SWIM_WINDOW_T *win,
    UNS_32 xSource,
    UNS_32 ySource,
    UNS_32 xDestination,
    UNS_32 yDestination,
    UNS_32 numPixels);
void swim_driver_put_pixel(SWIM_WINDOW_T *win, UNS_32 x, UNS_32 y, COLOR_T color);
void swim_driver_put_raster(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    const COLOR_T *pixels,
    UNS_32 numPixels);
void swim_driver_get_raster(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    COLOR_T *pixels,
    UNS_32 numPixels);
void swim_driver_put_raster_indexed_color(
    SWIM_WINDOW_T *win,
    UNS_32 x,
    UNS_32 y,
    const TUInt8 *pixels,
    COLOR_T *palette,
    UNS_32 numPixels);
void swim_driver_set_page(SWIM_WINDOW_T *win, UNS_16 page);

void swim_driver_set_orientation(BOOL_8 flipX, BOOL_8 flipY);

void swim_driver_get_orientation(BOOL_8 *flipX, BOOL_8 *flipY);


#endif // LPC_SWIM_INTERNAL_H_
/*-------------------------------------------------------------------------*
 * End of File:  lpc_swim_internal.h
 *-------------------------------------------------------------------------*/
