/***********************************************************************
 * $Id:: lpc_colors.h 745 2008-05-13 19:59:29Z pdurgesh                $
 *
 * Project: Color definitions
 *
 * Description:
 *     This package contains functions for color mapping, color
 *     conversion, and common defines.
 *
 *     The palette table function can be configured for 555 or 565
 *     color.
 *
 * Notes:
 *     Color entries are stored in BGR format, with blue mapped to the
 *     most significant bits of a color type.
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

#ifndef LPC_COLOR_TYPES_H
#define LPC_COLOR_TYPES_H

#include "lpc_types.h"
#include "Types/LCD.h"

#if defined (__cplusplus)
extern "C"
{
#endif

/***********************************************************************
 * Default configuration values
 **********************************************************************/

/* Pick the appropriate define for 8- or 16-bit color modes. Only ONE
   of these lines should be uncommented */
#ifndef COLORS_DEF
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_16_BIT)
#define COLORS_DEF 16      /* 16-bit 565 color mode */
#endif
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_I15_BIT)
#define COLORS_DEF 15      /* 16-bit I555 color mode */
#endif
//#define COLORS_DEF 15      /* 15-bit 555 color mode */
//#define COLORS_DEF 12      /* 12-bit 444 color mode */
#if (UEZ_LCD_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_8_BIT)
#define COLORS_DEF 8      /* 8-bit color mode */
#endif
#endif

/* If COLORS_8 is used, then uncomment the following line to use 565
   output mode in the palette table, otherwise 555 mode will be used */
/*#define COLORS_8_565_MODE */
#define RGB16(r, g, b)      \
                    ( (((r>>3)&0x1F)<<11)| \
                      (((g>>2)&0x3F)<<5)| \
                      (((b>>3)&0x1F)<<0) )

#if COLORS_DEF == 16
/* Black color, 565 mode */
#define BLACK         RGB16(0, 0, 0)
/* Light gray color, 565 mode */
#define LIGHTGRAY     RGB16(192,192,192)
/* Dark gray color, 565 mode */
#define DARKGRAY      RGB16(64, 64, 64)
/* White color, 565 mode */
#define WHITE         RGB16(255, 255, 255)
/* Red color, 565 mode */
#define RED           RGB16(255, 0, 0)
/* Green color, 565 mode */
#define GREEN         RGB16(0, 255, 0)
/* Blue color, 565 mode */
#define BLUE          RGB16(0, 0, 255)
/* Magenta color, 565 mode */
#define MAGENTA       (RED | BLUE)
/* Cyan color, 565 mode */
#define CYAN          (GREEN | BLUE)
/* Yellow color, 565 mode */
#define YELLOW        (RED | GREEN)
/* Light red color, 565 mode */
#define LIGHTRED      RGB16(255, 64, 64)
/* Light green color, 565 mode */
#define LIGHTGREEN    RGB16(64, 255, 64)
/* Light blue color, 565 mode */
#define LIGHTBLUE     RGB16(64, 64, 255)
/* Light magenta color, 565 mode */
#define LIGHTMAGENTA  (LIGHTRED | LIGHTBLUE)
/* Light cyan color, 565 mode */
#define LIGHTCYAN     (LIGHTGREEN | LIGHTBLUE)
/* Light yellow color, 565 mode */
#define LIGHTYELLOW   (LIGHTRED | LIGHTGREEN)

/* Red color mask, 565 mode */
#define REDMASK       0xF800
/* Red shift value, 565 mode */
#define REDSHIFT      11
/* Green color mask, 565 mode */
#define GREENMASK     0x07E0
/* Green shift value, 565 mode */
#define GREENSHIFT    5
/* Blue color mask, 565 mode */
#define BLUEMASK      0x001F
/* Blue shift value, 565 mode */
#define BLUESHIFT     0

/* Number of colors in 565 mode */
#define NUM_COLORS    65536
/* Number of red colors in 565 mode */
#define RED_COLORS    0x20
/* Number of green colors in 565 mode */
#define GREEN_COLORS  0x40
/* Number of blue colors in 565 mode */
#define BLUE_COLORS   0x20

/* Color type is a 16-bit value */
typedef UNS_16 COLOR_T;
#endif

#if COLORS_DEF == 15
/* Black color, 555 mode */
#define BLACK         0x0000
/* Llight gray color, 555 mode */
#define LIGHTGRAY     0x3DEF
/* Drak gray color, 555 mode */
#define DARKGRAY      0x1CE7
/* White color, 555 mode */
#define WHITE         0x7fff
/* Red color, 555 mode */
#define RED           0x7C00
/* Green color, 555 mode */
#define GREEN         0x03E0
/* Blue color, 555 mode */
#define BLUE          0x001F
/* Magenta color, 555 mode */
#define MAGENTA       (RED | BLUE)
/* Cyan color, 555 mode */
#define CYAN          (GREEN | BLUE)
/* Yellow color, 555 mode */
#define YELLOW        (RED | GREEN)
/* Light red color, 555 mode */
#define LIGHTRED      0x3C00
/* Light green color, 555 mode */
#define LIGHTGREEN    0x01E0
/* Light blue color, 555 mode */
#define LIGHTBLUE     0x000F
/* Light magenta color, 555 mode */
#define LIGHTMAGENTA  (LIGHTRED | LIGHTBLUE)
/* Light cyan color, 555 mode */
#define LIGHTCYAN     (LIGHTGREEN | LIGHTBLUE)
/* Light yellow color, 555 mode */
#define LIGHTYELLOW   (LIGHTRED | LIGHTGREEN)

/* Red color mask, 555 mode */
#define REDMASK       0x7C00
/* Red shift value, 555 mode */
#define REDSHIFT      10
/* Green color mask, 555 mode */
#define GREENMASK     0x03E0
/* Green shift value, 555 mode */
#define GREENSHIFT    5
/* Blue color mask, 555 mode */
#define BLUEMASK      0x001F
/* Blue shift value, 555 mode */
#define BLUESHIFT     0

/* Number of colors in 555 mode */
#define NUM_COLORS    32768
/* Number of red colors in 555 mode */
#define RED_COLORS    0x20
/* Number of green colors in 555 mode */
#define GREEN_COLORS  0x20
/* Number of blue colors in 555 mode */
#define BLUE_COLORS   0x20

/* Color type is a 16-bit value */
typedef UNS_16 COLOR_T;
#endif

#if COLORS_DEF == 12
/* Black color, 444 mode */
#define BLACK         0x0000
/* Llight gray color, 444 mode */
#define LIGHTGRAY     0x3DEF
/* Drak gray color, 444 mode */
#define DARKGRAY      0x1CE7
/* White color, 444 mode */
#define WHITE         0x7fff
/* Red color, 444 mode */
#define RED           0x3C00
/* Green color, 444 mode */
#define GREEN         0x01E0
/* Blue color, 444 mode */
#define BLUE          0x000F
/* Magenta color, 444 mode */
#define MAGENTA       (RED | BLUE)
/* Cyan color, 444 mode */
#define CYAN          (GREEN | BLUE)
/* Yellow color, 444 mode */
#define YELLOW        (RED | GREEN)
/* Light red color, 444 mode */
#define LIGHTRED      0x3C00
/* Light green color, 444 mode */
#define LIGHTGREEN    0x01E0
/* Light blue color, 444 mode */
#define LIGHTBLUE     0x000F
/* Light magenta color, 444 mode */
#define LIGHTMAGENTA  (LIGHTRED | LIGHTBLUE)
/* Light cyan color, 444 mode */
#define LIGHTCYAN     (LIGHTGREEN | LIGHTBLUE)
/* Light yellow color, 444 mode */
#define LIGHTYELLOW   (LIGHTRED | LIGHTGREEN)

/* Red color mask, 444 mode */
#define REDMASK       0x3C00
/* Red shift value, 444 mode */
#define REDSHIFT      10
/* Green color mask, 444 mode */
#define GREENMASK     0x01E0
/* Green shift value, 444 mode */
#define GREENSHIFT    5
/* Blue color mask, 444 mode */
#define BLUEMASK      0x000F
/* Blue shift value, 444 mode */
#define BLUESHIFT     0

/* Number of colors in 444 mode */
#define NUM_COLORS    4096
/* Number of red colors in 444 mode */
#define RED_COLORS    0x10
/* Number of green colors in 444 mode */
#define GREEN_COLORS  0x10
/* Number of blue colors in 444 mode */
#define BLUE_COLORS   0x10

/* Color type is a 16-bit value */
typedef UNS_16 COLOR_T;
#endif

#if COLORS_DEF == 8
/* Black color, 323 mode */
#define BLACK         0x00
/* Light gray color, 323 mode */
#define LIGHTGRAY     0x6E
/* Dark gray color, 323 mode */
#define DARKGRAY      0x25
/* White color, 323 mode */
#define WHITE         0xFF
/* Red color, 323 mode */
#define RED           0xE0
/* Green color, 323 mode */
#define GREEN         0x1C
/* Blue color, 323 mode */
#define BLUE          0x03
/* Magenta color, 323 mode */
#define MAGENTA       (RED | BLUE)
/* Cyan color, 323 mode */
#define CYAN          (GREEN | BLUE)
/* Yellow color, 323 mode */
#define YELLOW        (RED | GREEN)
/* Light red color, 323 mode */
#define LIGHTRED      0x60
/* Light green color, 323 mode */
#define LIGHTGREEN    0x0C
/* Light blue color, 323 mode */
#define LIGHTBLUE     0x01
/* Light magenta color, 323 mode */
#define LIGHTMAGENTA  (LIGHTRED | LIGHTBLUE)
/* Light cyan color, 323 mode */
#define LIGHTCYAN     (LIGHTGREEN | LIGHTBLUE)
/* Light yellow color, 323 mode */
#define LIGHTYELLOW   (LIGHTRED | LIGHTGREEN)

/* Red color mask, 323 mode */
#define REDMASK       0xE0
/* Red shift value, 323 mode */
#define REDSHIFT      5
/* Green color mask, 323 mode */
#define GREENMASK     0x1C
/* Green shift value, 323 mode */
#define GREENSHIFT    2
/* Blue color mask, 323 mode */
#define BLUEMASK      0x3
/* Blue shift value, 323 mode */
#define BLUESHIFT     0

/* Number of colors in 332 mode */
#define NUM_COLORS    256
/* Number of red colors in 332 mode */
#define RED_COLORS    0x08
/* Number of green colors in 332 mode */
#define GREEN_COLORS  0x08
/* Number of blue colors in 332 mode */
#define BLUE_COLORS   0x08

/* Color type is a 8-bit value */
typedef UNS_8 COLOR_T;
#endif

/***********************************************************************
 * Color configuration functions
 **********************************************************************/

/* Generate a palette table (only in 8-bit mode). If compiled in 16-bit
   color mode, this will be a NULL function. */
void lpc_colors_set_palette(UNS_16 *palette_table);

#if defined (__cplusplus)
}
#endif /*__cplusplus */

#endif /* LPC_COLOR_TYPES_H */
