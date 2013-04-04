/******************************************************************************
* DISCLAIMER
* Please refer to http://www.renesas.com/disclaimer
******************************************************************************/
/***********************************************************************/
/*                                                                     */
/*  FILE        :DirectLCD_CNF.H                                       */
/*  DATE        :Fri, Jan 12, 2008                                     */
/*  DESCRIPTION :LCD Direct Drive Configuration Header                 */
/*                                                                     */
/***********************************************************************/
/************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
01.22.2008 RTA-JMB Parameterized Direct Drive Interface
************************************************************************/
#ifndef DIRECTLCD_CNF_H
#define DIRECTLCD_CNF_H

#include <Config.h>

// Based on the configuration, choose the display type (allowed on this platform)
#if ((UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TOSHIBA_QVGA_LCDC_REV2) || (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TOSHIBA_QVGA_LCDC_3_5))
#include "DirectLCD_CNF(Toshiba_LTA057A347F).h"
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TIANMA_TM047NBH01)
#include "DirectLCD_CNF(Tianma_TM047NBH01).h"
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SEIKO_43WQW1T)
#include "DirectLCD_CNF(Tianma_TM043NBH02).h"
#endif

#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TIANMA_QVGA_LCDC_3_5)
#include "DirectLCD_CNF(Tianma_TM035KBH02).h"
#endif

/* Include platform configuration */
#include "DirectLCD_CNF(CARRIER_RX62N).h"

// Controls the presentation of data Top to Bottom on panel. Define this to invert
// Define this if need High to Low memory Lines (default is Low to High)
////#define V_LINES_INVERT

// Controls the presentation of data Left to Right on panel. Define this to invert
// Define this if need High to Low memory Pixels (default is Low to High)
////#define H_DOT_INVERT

// Controls the placement of data on the panel...ONLY FOR H8SX. Define this to rotate
// All panel definitions are same as data sheet, but grapics API interpretation of data is rotated.
//  For 320x240 QVGA panel, the raster data would be treated as 240x320 by graphics API.
// Allow user image to be rotated in transfer to the panel. 
//#define PANEL_ROTATE

// Number of RAM frames to allocate...the locations are accessed via LCDSetRaster Routines.
// GAPI Demo Code uses 3 frames...displayFrameBuf,backFrameBuf,workFrameBuf
#ifndef LCD_FRAMES
    #define LCD_FRAMES 3
#endif

// Specify the maximum number of regions that the screen can be split into
#ifndef MAX_FRAME_REGIONS
    #define MAX_FRAME_REGIONS 1
#endif

// Specify size of RAM frame...each dimension must be greater than or equal to panel dimension
#ifndef FRAME_HEIGHT
    #define FRAME_HEIGHT    V_LINES_DISPLAY
#endif

#ifndef FRAME_WIDTH
    #define FRAME_WIDTH     H_DOT_DISPLAY
#endif

#ifndef DESIRED_FRAME_RATE
    #define DESIRED_FRAME_RATE 60
#endif

#ifndef MINIMUM_MCU_ACCESS_PCT
    #define MINIMUM_MCU_ACCESS_PCT 5
#endif

#define configDIRECT_DRIVE_INTERRUPT_PRIORITY 6

#endif
