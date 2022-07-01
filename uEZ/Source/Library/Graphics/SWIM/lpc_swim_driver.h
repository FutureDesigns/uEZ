/*-------------------------------------------------------------------------*
 * File:  lpc_swim_internal.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC_SWIM_INTERNAL_H_
#define LPC_SWIM_INTERNAL_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // LPC_SWIM_INTERNAL_H_
/*-------------------------------------------------------------------------*
 * End of File:  lpc_swim_internal.h
 *-------------------------------------------------------------------------*/
