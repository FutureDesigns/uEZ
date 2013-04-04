/******************************************************************************
* DISCLAIMER
* Please refer to http://www.renesas.com/disclaimer
******************************************************************************/
/***********************************************************************/
/*                                                                     */
/*  FILE        :DirectLCD_CNF(Tianma_TM035KBH02).H                                */
/*  DATE        :Mon, Aug 29, 2011                                     */
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
//#define H_DOT_INVERT

// DOT CLOCK Inversion Control (only works with TPU generated DOTCLK modes (ExDMA generated must invert in hardware if necessary)
// Define this symbol if dot inversion necessary
#define DOT_INVERT

// Vertical pulse width (inactive pulse width)
#define V_LINES_PULSE 1//3
// Vertical back porch (Lines Vsynch Active till display lines)
#define V_LINES_BACK_PORCH 12//15
// Vertical lines (active data lines)  
#define V_LINES_DISPLAY 240L
// Minimum Vertical Front Porch (post display till Vsych inactive)...this may be extended by driver to meet requested behavior
#define V_LINES_FRONT_PORCH 7//23

#if ((V_DOT_PULSE + V_LINES_BACK_PORCH + V_LINES_DISPLAY + V_LINES_FRONT_PORCH) > 340)
    #error "Frame error is too long (over 340)"
#endif
#if ((V_DOT_PULSE + V_LINES_BACK_PORCH + V_LINES_DISPLAY + V_LINES_FRONT_PORCH) < 251)
    #error "Frame error is too short (over 250)"
#endif
#if ((V_LINES_PULSE) < 1)
    #error "Vertical pulse must be 1 or greater"
#endif
#if ((V_LINES_PULSE) > 34)
    #error "Vertical pulse must not be greater than 34"
#endif
#if (V_LINES_DISPLAY != 240)
    #error "Vertical display must be 240!"
#endif
#if ((V_LINES_BACK_PORCH+V_LINES_PULSE+V_LINES_FRONT_PORCH) < 20)
    #error "Vertical blank period must be 20 or more"
#endif
#if ((V_LINES_BACK_PORCH+V_LINES_PULSE+V_LINES_FRONT_PORCH) > 70)
    #error "Vertical blank period less than 70"
#endif
#if ((V_LINES_BACK_PORCH+V_LINES_PULSE) != 7)
    // This seems wrong in the spec
    //#error "Vertical data starting position must be 7"
#endif

// Horizontal pulse width (dot clocks inactive)...specify value GREATER than min AND LESS than max (min < value < max)
#define H_DOT_PULSE 3 //53 // 54
// Horizontal back porch (dot clocks Hsync active till display data)
#define H_DOT_BACK_PORCH 67 // 68 // 32
// Horizonal pixels (active data)
#define H_DOT_DISPLAY 320L
// Horizonatl front porch (post data till Hsynch inactive)
#define H_DOT_FRONT_PORCH 1//40 // 20

#if ((H_DOT_PULSE+H_DOT_BACK_PORCH+H_DOT_DISPLAY+H_DOT_FRONT_PORCH) > 450)
    //#error "Too many dots in horizontal period!"
#endif
#if ((H_DOT_PULSE+H_DOT_BACK_PORCH+H_DOT_DISPLAY+H_DOT_FRONT_PORCH) < 360)
    #error "Too few dots in horizontal period!"
#endif
#if ((H_DOT_PULSE) < 2)
    #error "Horizontal dot pulse / sync must be at least 2!"
#endif
#if ((H_DOT_PULSE) > 96)
    #error "Horizontal dot pulse / sync must be less than 96!"
#endif
#if ((H_DOT_DISPLAY) != 320)
    #error "H_DOT_DISPLAY must be 320"
#endif
#if ((H_DOT_PULSE+H_DOT_BACK_PORCH)<11)
    #error "H_DOT_PULSE+H_DOT_BACK_PORCH must be 11 or greater"
#endif

// Desired system behavior. Dot clock frequency must be integer multiple of BCLK and PCLK
#ifndef DOT_CLOCK_FREQUENCY_DATA
    #define DOT_CLOCK_FREQUENCY_DATA 12000000L //16000000L
#endif
#ifndef DOT_CLOCK_FREQUENCY_BLANK
    #define DOT_CLOCK_FREQUENCY_BLANK 12000000L // 8000000L
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
