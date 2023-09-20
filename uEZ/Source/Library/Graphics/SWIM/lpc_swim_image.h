/***********************************************************************
 * $Id:: lpc_swim_image.h 745 2008-05-13 19:59:29Z pdurgesh            $
 *
 * Project: Image management for SWIM
 *
 * Description:
 *     This package provides the following image capabilities with SWIM:
 *        Display of raw image data (stored left to right, top to
 *            bottom)
 *        Stored raw images MUST be stored in the same color format as
 *           color_type
 *        Image scaling, rotation, and clipping
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

#ifndef LPC_SWIM_IMAGE_H
#define LPC_SWIM_IMAGE_H

#include "lpc_types.h"
#include "lpc_swim.h"
#include "lpc_colors.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * Package types
 **********************************************************************/

/* Image rotation tags */
typedef enum {NOROTATION, RIGHT, INVERT, LEFT} SWIM_ROTATION_T;

/***********************************************************************
 * Image drawing functions
 **********************************************************************/

/* Puts a raw image into a window */
void swim_put_image(SWIM_WINDOW_T *win,
                    const COLOR_T *image,
                    INT_32 xsize,
                    INT_32 ysize);

/* Puts a raw image into a window inverted */
void swim_put_invert_image(SWIM_WINDOW_T *win,
                           const COLOR_T *image,
                           INT_32 xsize,
                           INT_32 ysize);

/* Puts a raw image into a window rotated left */
void swim_put_left_image(SWIM_WINDOW_T *win,
                         const COLOR_T *image,
                         INT_32 xsize,
                         INT_32 ysize);

/* Puts a raw image into a window rotated right */
void swim_put_right_image(SWIM_WINDOW_T *win,
                          const COLOR_T *image,
                          INT_32 xsize,
                          INT_32 ysize);

/* Puts and scales a raw image into a window */
void swim_put_scale_image(SWIM_WINDOW_T *win,
                          const COLOR_T *image,
                          INT_32 xsize,
                          INT_32 ysize);

/* Puts and scales a raw image into a window inverted */
void swim_put_scale_invert_image(SWIM_WINDOW_T *win,
                                 const COLOR_T *image,
                                 INT_32 xsize,
                                 INT_32 ysize);

/* Puts and scales a raw image into a window rotated left */
void swim_put_scale_left_image(SWIM_WINDOW_T *win,
                               const COLOR_T *image,
                               INT_32 xsize,
                               INT_32 ysize);

/* Puts and scales a raw image into a window rotated right */
void swim_put_scale_right_image(SWIM_WINDOW_T *win,
                                const COLOR_T *image,
                                INT_32 xsize,
                                INT_32 ysize);

/* One API for all the functions */
void swim_put_win_image(SWIM_WINDOW_T *win,
                        const COLOR_T *image,
                        INT_32 xsize,
                        INT_32 ysize,
                        INT_32 scale,
                        SWIM_ROTATION_T rtype);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_SWIM_IMAGE_H */
