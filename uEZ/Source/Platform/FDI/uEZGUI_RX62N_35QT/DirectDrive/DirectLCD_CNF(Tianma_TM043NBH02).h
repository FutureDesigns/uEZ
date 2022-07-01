/******************************************************************************
* DISCLAIMER
* Please refer to http://www.renesas.com/disclaimer
******************************************************************************/
/***********************************************************************/
/*                                                                     */
/*  FILE        :DirectLCD_CNF(Tianma_TM047NBH01).H                    */
/*  DATE        :Fri, Jan 12, 2008                                     */
/*  DESCRIPTION :LCD Direct Drive Configuration Header                 */
/*                                                                     */
/***********************************************************************/
/************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
01.22.2008 RTA-JMB Parameterized Direct Drive Interface
************************************************************************/

// use the defintions below to configure the connected LCD panel to the MCU 

// USER DEFINES to Control Panel
// Controls the presentation of data Left to Right on panel. Define this to invert
// Define this if need High to Low memory Pixels (default is Low to High)
// #define H_DOT_INVERT

// DOT CLOCK Inversion Control (only works with TPU generated DOTCLK modes (ExDMA generated must invert in hardware if necessary)
// Define this symbol if dot inversion necessary
// #define DOT_INVERT

// Vertical pulse width (inactive pulse width)
#define V_LINES_PULSE 10L
// Vertical back porch (Lines Vsynch Active till display lines)
#define V_LINES_BACK_PORCH 2
// Vertical lines (active data lines)
#define V_LINES_DISPLAY 272L
// Minimum Vertical Front Porch (post display till Vsych inactive)...this may be extended by driver to meet requested behavior
#define V_LINES_FRONT_PORCH 2

// Horizontal pulse width (dot clocks inactive)...specify value GREATER than min AND LESS than max (min < value < max)
#define H_DOT_PULSE 41
// Horizontal back porch (dot clocks Hsync active till display data)
#define H_DOT_BACK_PORCH 2
// Horizonal pixels (active data)
#define H_DOT_DISPLAY 480L
// Horizonatl front porch (post data till Hsynch inactive)
#define H_DOT_FRONT_PORCH 80 // max time

// Desired system behavior. Dot clock frequency must be integer multiple of BCLK and PCLK
#ifndef DOT_CLOCK_FREQUENCY_DATA
    #define DOT_CLOCK_FREQUENCY_DATA 12000000L
#endif
#ifndef DOT_CLOCK_FREQUENCY_BLANK
    #define DOT_CLOCK_FREQUENCY_BLANK 12000000L
#endif

/* Initial Touch Screen Calibration data */
#define TC_LL_X         0x3CA6      /* filtered (14 bit) A/D reading at lower left of screen */
#define TC_LL_Y         0x0607
#define TC_LR_X         0x02D2      /* filtered (14 bit) A/D reading at lower right of screen */
#define TC_LR_Y         0x0595
#define TC_UL_X         0x3C9B      /* filtered (14 bit) A/D reading at upper left of screen */
#define TC_UL_Y         0x3983
#define TC_UR_X         0x02D1      /* filtered (14 bit) A/D reading at upper right of screen */
#define TC_UR_Y         0x3A57

//#define V_LINES_INVERT
