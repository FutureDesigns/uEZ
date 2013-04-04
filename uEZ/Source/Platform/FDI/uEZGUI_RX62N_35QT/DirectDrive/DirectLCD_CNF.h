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

#define uEZGUI_RX_35_DD

// Controls the presentation of data Top to Bottom on panel. Define this to invert
// Define this if need High to Low memory Lines (default is Low to High)
//#define V_LINES_INVERT

// Controls the presentation of data Left to Right on panel. Define this to invert
// Define this if need High to Low memory Pixels (default is Low to High)
//#define H_DOT_INVERT

/* Include files are used to support multiple panels and platforms. */
//#include "DirectLCD_CNF(RX62N_Proto).h"
#include "DirectLCD_CNF(uEZGUI_RX_35).h"

//#include "DirectLCD_CNF(Kyocera_QV).h"
//#include "DirectLCD_CNF(Kyocera_VG).h"
//#include "DirectLCD_CNF(Sharp_V3).h"
//#include "DirectLCD_CNF(Sharp_T3).h"
//#include "DirectLCD_CNF(Sharp_V7).h"
//#include "DirectLCD_CNF(Hitachi_TX).h"
//#include "DirectLCD_CNF(Toshiba_LTA057A347F).h"
#include "DirectLCD_CNF(Tianma_TM035KBH02).h"

// Controls the placement of data on the panel...ONLY FOR H8SX. Define this to rotate
// All panel definitions are same as data sheet, but grapics API interpretation of data is rotated.
//  For 320x240 QVGA panel, the raster data would be treated as 240x320 by graphics API.
// Allow user image to be rotated in transfer to the panel. 
//#define PANEL_ROTATE

// Number of RAM frames to allocate...the locations are accessed via LCDSetRaster Routines.
// GAPI Demo Code uses 3 frames...displayFrameBuf,backFrameBuf,workFrameBuf
#define LCD_FRAMES 3
// Specify the maximum number of regions that the screen can be split into
#define MAX_FRAME_REGIONS 1
// Specify size of RAM frame...each dimension must be greater than or equal to panel dimension
#define FRAME_HEIGHT V_LINES_DISPLAY
#define FRAME_WIDTH H_DOT_DISPLAY

// Desired system behavior. Dot clock frequency must be integer multiple of BCLK and PCLK
#ifndef DOT_CLOCK_FREQUENCY_DATA
    #define DOT_CLOCK_FREQUENCY_DATA 16000000L //24000000 //16000000L
#endif
#ifndef DOT_CLOCK_FREQUENCY_BLANK
    #define DOT_CLOCK_FREQUENCY_BLANK 16000000L //24000000 //16000000L  
#endif
//#define DOT_CLOCK_FREQUENCY_DATA 16000000L
//#define DOT_CLOCK_FREQUENCY_BLANK 12000000L

#define DESIRED_FRAME_RATE 80   
#define MINIMUM_MCU_ACCESS_PCT 30

#define configDIRECT_DRIVE_INTERRUPT_PRIORITY 6	 // Renesas 1668

#endif
