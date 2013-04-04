/******************************************************************************
* DISCLAIMER
* Please refer to http://www.renesas.com/disclaimer
******************************************************************************/
/***********************************************************************/
/*                                                                     */
/*  FILE        :DirectLCD_CNF(platform).H                             */
/*  DATE        :Fri, Jan 12, 2008                                     */
/*  DESCRIPTION :LCD Direct Drive Configuration Header                 */
/*                                                                     */
/***********************************************************************/
/************************************************************************
Revision History
DD.MM.YYYY OSO-UID Description
01.22.2008 RTA-JMB Parameterized Direct Drive Interface
************************************************************************/

// The DK-TS-KIT with SOMDIMM-RX62N uses SDRAM

//#define SRAM_DD
//#define SRAM_NOMUX_DD
#define SDRAM_DD
//#define SDRAM_CLUSTER_DD


// use the defintions below to change the MCU ports used for interfacing to the panel 
/* Define which CS is used for the frame buffer */
#define FRAME_CS        3

/* Number of bus cycles to run frame RAM at (based on memory access speed and xtal frequency) */
#ifndef FRAME_BUS_CYCLES
#define FRAME_BUS_CYCLES 3
#endif
// CAS Latency if SDRAM usage
#ifndef SDRAM_ROW_PRECHARGE
#define SDRAM_ROW_PRECHARGE  2
#endif

#ifndef SDRAM_CAS_LATENCY
////#define CAS_LATENCY 3
#define SDRAM_CAS_LATENCY 2
#endif

// SDRAM Page Size (in WORDS!)
#define SDRAM_PAGE_SIZE 256

/* control vsync */
#define VSYNC_PORT                      2 // 2
#define VSYNC_PIN                       4 // 5

/* control hsync */
#define HSYNC_PORT                      3 // 2
#define HSYNC_PIN                       2 // 4

/* control dotclock */
#define DOTCLK_PORT                     5
#define DOTCLK_PIN                      6

/* turn on power to the LCD panel */          
#define USE_LCD_PANEL_POWER
#define LCD_PANEL_POWER_PORT            9
#define LCD_PANEL_POWER_PIN             2

/* turn on power to LCD backlight */
#define LCD_BACKLIGHT_PORT              9
#define LCD_BACKLIGHT_PIN               3

/* ExDMA Channel used for direct drive */
#define EXDMAC_DD                       EXDMAC0
//#define EXDMAC_DD_UNIT                  _EXDMAC     /* unit assoicated with ExDMA Channel */
#define EXDMAC_DD_CHANNEL               EXDMAC0I   /* IRQ for channel */
#define EXDMAC_DD_REQ_PORT              5 // 8
#define EXDMAC_DD_REQ_PIN               5 // 0

/* specify timer unit used for direct drive */
#define DD_TMR_MTU
//#define DD_TMR_TPU

/* Timer Channel Used for the Dot Clock Signal and period (period will not drive output) */
#define DOTCLK_CHANNEL  3
#define DOTCLK_TIOC     C
#define DOTPER_CHANNEL  3     /* must be same as DOTCLK_CHANNEL */
#define DOTPER_TIOC     D

/* Timer Channel used for Horizontal Period (period will not drive output) */
#define HPER_CHANNEL    0 // 4
#define HPER_TIOC       D // B

/* Timer Channel used for Horizontal Synch Signal (same channel as HPER_CHANNEL preferred) */
#define HSYNC_CHANNEL   0 // 4
#define HSYNC_TIOC      C // A

/* Timer Channel used for Vertical Synch Signal (same channel as HPER_CHANNEL preferred) */
#define VSYNC_CHANNEL   4 // 0 // 4
#define VSYNC_TIOC      A // C
#define VPER_CHANNEL   4 // 0 // 4
#define VPER_TIOC      B // D

/* Timer Channel used for Horizontal Data Enable Signal (ExDMA request) must be seperate channel because of PWM1 operation
   this channel will be syncd with HPER_CHANNEL to provide timebase */

#define HDEN_CHANNEL                    0
#define HDEN_TIOC                       A     /* must be A or C because of PWM1 operation */
#define HDEN2_CHANNEL                   0     /* must be same as HDEN_TPU_CHANNEL */
#define HDEN2_TIOC                      B     /* must be complementary pin to HDEN_TPU_PIN A->B, C->D */

