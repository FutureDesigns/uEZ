/*********************************************************************
*                SEGGER MICROCONTROLLER GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 2003-2012     SEGGER Microcontroller GmbH & Co KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.16 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

#include "GUI.h"
#include <stddef.h>

#ifndef _WINDOWS
  #include "GUIDRV_Lin.h"
  #include "I2C.h"
  #include "I2C_PCA9532.h"
  #include "LPC177x_8x.h"
  #include "system_LPC177x_8x.h"
  #include "HWConf.h"
  #include "LCDConf.h"
#endif

/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/

//
// Display framebuffer size
// These values can be reduced if using a smaller display and might have
// to be adjusted if using virtual screens. The values will be checked
// during emWin init.
//
#define FB_XSIZE  800
#define FB_YSIZE  480

//
// Physical display size
//
#ifndef _WINDOWS
  #define XSIZE_PHYS  _LcdParams.PPL
  #define YSIZE_PHYS  _LcdParams.LPP
#else
  #define XSIZE_PHYS  FB_XSIZE
  #define YSIZE_PHYS  FB_YSIZE
#endif

//
// Virtual display size
//
#define VXSIZE_PHYS (XSIZE_PHYS)
#define VYSIZE_PHYS (YSIZE_PHYS)

//
// Color conversion
//
#define COLOR_CONVERSION  GUICC_M565

//
// Pixel width in bytes
//
#define PIXEL_WIDTH  2

//
// Display driver
//
#define DISPLAY_DRIVER_TRULY  &GUIDRV_Lin_OSX_16_API
#define DISPLAY_DRIVER_OTHER  &GUIDRV_Lin_16_API

//
// Video RAM address
//
#define VRAM_ADDR_PHYS  (U32)&_aVRAM[0]

//
// Touch controller settings for 3.2" Truly display
//
#define TOUCH_TRULY_240_320_AD_LEFT    3686
#define TOUCH_TRULY_240_320_AD_RIGHT   205
#define TOUCH_TRULY_240_320_AD_TOP     3842
#define TOUCH_TRULY_240_320_AD_BOTTOM  267

//
// Touch controller settings for 4.3" display board
//
#define TOUCH_BOARD_480_272_AD_LEFT    500
#define TOUCH_BOARD_480_272_AD_RIGHT   3650
#define TOUCH_BOARD_480_272_AD_TOP     3300
#define TOUCH_BOARD_480_272_AD_BOTTOM  610

//
// Touch controller settings for 7" display board
//
#define TOUCH_BOARD_800_480_AD_LEFT    750
#define TOUCH_BOARD_800_480_AD_RIGHT   3200
#define TOUCH_BOARD_800_480_AD_TOP     3320
#define TOUCH_BOARD_800_480_AD_BOTTOM  870

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef   VXSIZE_PHYS
  #define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef   VYSIZE_PHYS
  #define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef   VRAM_ADDR_PHYS
  #define VRAM_ADDR_PHYS  0
#endif
#ifndef   VRAM_ADDR_VIRT
  #define VRAM_ADDR_VIRT  0
#endif

#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif
#ifndef   COLOR_CONVERSION
  #error Color conversion not defined!
#endif

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

//
// LCD types
//
enum {
  DISPLAY_TRULY_240_320,
  DISPLAY_BOARD_480_272,
  DISPLAY_BOARD_800_480
};

//
// LCD configuration for 3.2" Truly display
//
#define HBP_TRULY_3_2   28       // Horizontal back porch in clocks
#define HFP_TRULY_3_2   10       // Horizontal front porch in clocks
#define HSW_TRULY_3_2   2        // HSYNC pulse width in clocks
#define PPL_TRULY_3_2   240      // Pixels per line
#define VBP_TRULY_3_2   2        // Vertical back porch in clocks
#define VFP_TRULY_3_2   1        // Vertical front porch in clocks
#define VSW_TRULY_3_2   2        // VSYNC pulse width in clocks
#define LPP_TRULY_3_2   320      // Lines per panel
#define IOE_TRULY_3_2   0        // Invert output enable, 1 = invert
#define IPC_TRULY_3_2   1        // Invert panel clock, 1 = invert
#define IHS_TRULY_3_2   1        // Invert HSYNC, 1 = invert
#define IVS_TRULY_3_2   1        // Invert VSYNC, 1 = invert
#define ACB_TRULY_3_2   1        // AC bias frequency in clocks (not used)
#define BPP_TRULY_3_2   16       // Bits per pixel b110 = 16 bpp 5:6:5 mode
#define CLK_TRULY_3_2   8200000  // Optimal clock rate (Hz) between 1-8.22 MHz according to SSD1289 datasheet
#define LCD_TRULY_3_2   0        // Panel type; 0: LCD TFT panel
#define DUAL_TRULY_3_2  0        // No dual panel

//
// LCDC
//
#define LCDC_BPP_16_565    6     // Bits per pixel b110 = 16 bpp 5:6:5 mode
#define LCDC_TFT           1     // TFT value for LCDC

#define BGR                1     // Swap red and blue

#define BCD_BIT            26    // Bypass pixel clock divider
#define LCD_BACKLIGHT_BIT  18    // P1.18
#define LCD_REG_BIT        19    // P0.19

#define LCD_DATA()         (LPC_GPIO0->SET = (1 << LCD_REG_BIT))
#define LCD_CMD()          (LPC_GPIO0->CLR = (1 << LCD_REG_BIT))

//
// I2C
//
#define I2C0_BASE_ADDR  0x4001C000

//
// PCA9532 (I2C)
//
#define PCA9532_I2C_ADDR         0x64  // Address of the PCA9532 on the display PCB
#define PCA9532_DISP_3V3_EN_BIT  0
#define PCA9532_DISP_5V_EN_BIT   1
#define PCA9532_DISP_EN_BIT      4
#define PCA9532_DISP_BL_BIT      8

//
// EEPROM (I2C)
//
#define EEPROM_I2C_ADDR         0x56  // Address of the EEPROM that holds the display configurations
#define EEPROM_MAGIC_NUMBER     0xEA01CDAE
#define EEPROM_LCD_CONFIG_OFFS  64
#define EEPROM_MAX_SEQ_VER      1

//
// SSP
//
#define SSPSR_TNF      (1 << 1)  // Transmit FIFO not full
#define SSPSR_RNE      (1 << 2)  // Receive FIFO not empty
#define SSPSR_BSY      (1 << 4)  // Busy
#define SSP_CLOCK      3000000
#define SSP_FIFO_SIZE  8

#define SSP_MODE_LCD   0
#define SSP_MODE_TS    1

//
// Touch screen
//
#define TOUCH_CS_BIT                   20
#define TS_CS_SET()                    LPC_GPIO0->CLR = (1 << TOUCH_CS_BIT)
#define TS_CS_CLR()                    LPC_GPIO0->SET = (1 << TOUCH_CS_BIT)

#define ADS_START                      (1 << 7)
#define ADS_A2A1A0_d_y                 (1 << 4)  // Differential
#define ADS_A2A1A0_d_z1                (3 << 4)  // Differential
#define ADS_A2A1A0_d_z2                (4 << 4)  // Differential
#define ADS_A2A1A0_d_x                 (5 << 4)  // Differential
#define ADS_A2A1A0_vaux                (6 << 4)  // Non-differential
#define ADS_12_BIT                     (0 << 3)
#define ADS_SER                        (1 << 2)  // Non-differential
#define ADS_DFR                        (0 << 2)  // Differential
#define ADS_PD10_ADC_ON                (1 << 0)  // ADC on
#define ADS_PD10_REF_ON                (2 << 0)  // vREF on + penirq
#define ADS_PD10_ALL_ON                (3 << 0)  // ADC + vREF on

#define READ_12BIT_DFR(D, ADC, VREF)  (ADS_START                    | \
                                       D                            | \
                                       ADS_12_BIT                   | \
                                       ADS_DFR                      | \
                                       (ADC  ? ADS_PD10_ADC_ON : 0) | \
                                       (VREF ? ADS_PD10_REF_ON : 0))


#define READ_12BIT_SER(x)              (ADS_START  | /* Single-ended samples need to power   */ \
                                        x          | /* up reference voltage first; therefor */ \
                                        ADS_12_BIT | /* we leave both VREF and ADC powered.  */ \
                                        ADS_SER)

#define REF_ON                         (READ_12BIT_DFR(ADS_A2A1A0_d_x,  1, 1))
#define READ_Y(VREF)                   (READ_12BIT_DFR(ADS_A2A1A0_d_y,  1, VREF))
#define READ_Z1(VREF)                  (READ_12BIT_DFR(ADS_A2A1A0_d_z1, 1, VREF))
#define READ_Z2(VREF)                  (READ_12BIT_DFR(ADS_A2A1A0_d_z2, 1, VREF))
#define READ_X(VREF)                   (READ_12BIT_DFR(ADS_A2A1A0_d_x,  1, VREF))
#define PWRDOWN                        (READ_12BIT_DFR(ADS_A2A1A0_d_y,  0, 0))

#define TS_DEBOUNCE_MAX                10
#define TS_DEBOUNCE_TOL                3

#ifndef   ABS
  #define ABS(x)                       (((int32_t)(x)) < 0 ? (-x) : (x))
#endif

/*********************************************************************
*
*       Defines, sfrs
*
**********************************************************************
*/

#define MATRIX_ARB  (*(volatile U32*)(0x400FC188))

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

typedef struct {
  U8  HBP;   // Horizontal back porch in clocks
  U8  HFP;   // Horizontal front porch in clocks
  U8  HSW;   // HSYNC pulse width in clocks
  U16 PPL;   // Pixels per line
  U8  VBP;   // Vertical back porch in clocks
  U8  VFP;   // Vertical front porch in clocks
  U8  VSW;   // VSYNC pulse width in clocks
  U16 LPP;   // Lines per panel
  U8  IOE;   // Invert output enable, 1 = invert
  U8  IPC;   // Invert panel clock, 1 = invert
  U8  IHS;   // Invert HSYNC, 1 = invert
  U8  IVS;   // Invert VSYNC, 1 = invert
  U8  ACB;   // AC bias frequency in clocks (not used)
  U8  BPP;   // Maximum bits per pixel the display supports
  U32 Clk;   // Optimal clock rate (Hz)
  U8  LCD;   // LCD panel type
  U8  Dual;  // Dual panel, 1 = dual panel display
} LCD_PARAM;

typedef struct {
  U16 LcdParams;
  U16 LcdInit;
  U16 PowerDown;
  U16 Touch;
  U16 End;
} EEPROM_CONFIG_OFFS;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

//
// LCD
//
static EEPROM_CONFIG_OFFS _EepromConfigOffs;
static LCD_PARAM          _LcdParams;
static U8                 _acBuffer[256];  // Buffer to read EEPROM initstring
static U8                 _Display;

//
// Video RAM
//
#ifdef __ICCARM__
  #pragma data_alignment=8  // 8 byte align frame buffer
  #pragma location="VRAM"
  static __no_init U32 _aVRAM[FB_XSIZE * FB_YSIZE / (4 / PIXEL_WIDTH)];
#endif
#ifdef __CC_ARM
  __align(8) static U32 _aVRAM[FB_XSIZE * FB_YSIZE / (4 / PIXEL_WIDTH)] __attribute__ ((section ("VRAM"), zero_init));
#endif
#ifdef __GNUC__
  U32 static _aVRAM[FB_XSIZE * FB_YSIZE / (4 / PIXEL_WIDTH)]  __attribute__ ((section(".VRAM"))) = { 0 };
#endif

#if GUI_SUPPORT_TOUCH  // Used when touch screen support is enabled

//
// Touch screen results
//
static int _TouchX;
static int _TouchY;
static U8  _PenIsDown;
static U8  _IsInited   = 0;

#endif

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _DelayMs
*
* Function description
*   Starts a timer and waits for the given delay in ms.
*/
#ifndef _WINDOWS
static void _DelayMs(U32 ms) {
  LPC_TIM0->TCR = 0x02;  // Reset timer
  LPC_TIM0->PR  = 0x00;  // Set prescaler to zero
  LPC_TIM0->MR0 = ms * (SystemCoreClock / (LPC_SC->PCLKSEL & 0x1F) / 1000 - 1);
  LPC_TIM0->IR  = 0xFF;  // Reset all interrrupts
  LPC_TIM0->MCR = 0x04;  // Stop timer on match
  LPC_TIM0->TCR = 0x01;  // Start timer
  //
  // Wait until delay time has elapsed
  //
  while (LPC_TIM0->TCR & 1);
}
#endif

/*********************************************************************
*
*       _SetDisplayOrigin()
*/
#ifndef _WINDOWS
static void _SetDisplayOrigin(int x, int y) {
  (void)x;
  //
  // Set start address for display data and enable LCD controller
  //
  LPC_LCD->UPBASE = VRAM_ADDR_PHYS + (y * YSIZE_PHYS * PIXEL_WIDTH);  // Needs to be set, before LCDC is enabled
}
#endif

/*********************************************************************
*
*       _FindClockDivisor
*
* Function description
*   Find closest clock divider to get the desired clock rate.
*/
#ifndef _WINDOWS
static U32 _FindClockDivisor(U32 TargetClock) {
  U32 Divider;
  U32 r;

  Divider = 1;
  while (((SystemCoreClock / Divider) > TargetClock) && (Divider <= 0x3F)) {
    Divider++;
  }
  if (Divider <= 1) {
    r = (1 << BCD_BIT);  // Skip divider logic if clock divider is 1
  } else {
    //
    // Use found divider
    //
    Divider -= 2;
    r = 0
        | (((Divider >> 0) & 0x1F)
        | (((Divider >> 5) & 0x1F) << 27))
        ;
  }
  return r;
}
#endif

/*********************************************************************
*
*       _EEPROM_Read
*
* Function description
*   Reads from EEPROM connected via I2C.
*
* Return value:
*      0: O.K.
*   != 0: Error
*/
#ifndef _WINDOWS
static U8 _EEPROM_Read(U16 Offs, U8 * pData, U16 NumBytes) {
  U8 acOffs[2];

  acOffs[0] = (Offs >> 8);
  acOffs[1] = (Offs & 0xFF);
  return I2C_WriteRead(I2C0_BASE_ADDR, EEPROM_I2C_ADDR, acOffs, 2, pData, NumBytes);
}
#endif

/*********************************************************************
*
*       Local functions for SSP
*
**********************************************************************
*/

/*********************************************************************
*
*       _GetSspDividers
*
* Function description
*   Find best SSP dividers for the desired clock rate.
*/
#ifndef _WINDOWS
static void _GetSspDividers(U32 TargetClock, U8* Scr, U8* Cpsr) {
  U16 tScr  = 1;
  U8  tCpsr = 2;
  U32 Clock = PeripheralClock;

  while (Clock > TargetClock) {
    Clock = PeripheralClock / ((tScr + 1) * tCpsr);
    if (Clock > TargetClock) {
      tScr++;
      if (tScr > 0xFF) {
        tScr   = 0;
        tCpsr += 2;
      }
    }
  }
  *Scr  = (U8)tScr;
  *Cpsr = tCpsr;
}
#endif

/*********************************************************************
*
*       _InitSSP
*
* Function description
*   Initialize the SSP interface. Used to configure Truly LCD display
*   controller IC on Embedded Artists QVGA Base Board.
*/
#ifndef _WINDOWS
static void _InitSSP(U8 Mode) {
  U8 Cpsr;
  U8 Scr;
  U8 i;
  volatile U32 v;

  //
  // Init ports
  //
#ifdef LPC4088
  LPC_SC->PCONP |=  (1UL << 20);  // Power the SSP2 port
  LPC_IOCON->P5_2 = 2;  // SSP2_SCK
  LPC_IOCON->P5_3 = 2;  // SSP2_SSEL
  LPC_IOCON->P5_1 = 2;  // SSP2_MISO
  LPC_IOCON->P5_0 = 2;  // SSP2_MOSI
#else
  LPC_IOCON->P0_15 = 2;  // SSP0_SCK
  LPC_IOCON->P0_16 = 2;  // SSP0_SSEL
  LPC_IOCON->P0_17 = 2;  // SSP0_MISO
  LPC_IOCON->P0_18 = 2;  // SSP0_MOSI
#endif
  //
  // Init SSP parameters
  //
  _GetSspDividers(SSP_CLOCK, &Scr, &Cpsr);
  v               = 0
                    | (7   << 0)  // DSS : 8-bit transfer
                    | (0   << 4)  // FRF : Frame Format -> 0 = SPI
                    | (Scr << 8)  // SCR : Serial Clock Rate
                    ;
  if        (Mode == SSP_MODE_LCD) {
    v            |= 0
                    | (0   << 6)  // CPOL: Clock out polarity
                    | (0   << 7)  // CPHA: Clock out phase
                    ;
  } else if (Mode == SSP_MODE_TS) {
    v            |= 0
                    | (1   << 6)  // CPOL: Clock out polarity
                    | (1   << 7)  // CPHA: Clock out phase
                    ;
  }
  LPC_SSP->CR0   = v;
  LPC_SSP->CPSR  = Cpsr;         // CPSR: Clock prescale register, master mode, minimum divisor is 0x02
  for (i = 0; i < SSP_FIFO_SIZE; i++) {
    v = LPC_SSP->DR;             // Clear the RxFIFO
  }
  LPC_SSP->CR1   = 0
                    | (1 << 1)    // SSE : Enable SSP
                    | (0 << 2)    // MS  : Master mode
                    ;
}
#endif

/*********************************************************************
*
*       _SSP_Send
*
* Function description
*   Sends data via the SSP0 interface. Used to configure Truly LCD
*   display controller IC on Embedded Artists QVGA Base Board.
*/
#ifndef _WINDOWS
static void _SSP_Send(U8* pData, U32 NumBytes) {
  volatile U8  Dummy;

  do {
    //
    // Wait for not busy and TX FIFO not full
    //
    while ((LPC_SSP->SR & (SSPSR_TNF|SSPSR_BSY)) != SSPSR_TNF);
    //
    // Send next byte
    //
    LPC_SSP->DR = *pData;
    pData++;
    //
    // Whenever a byte is written, MISO FIFO counter is incremented.
    // Therefore we have to clear the byte from FIFO as otherwise we would
    // get old data on next read.
    //
    while ((LPC_SSP->SR & (SSPSR_BSY|SSPSR_RNE)) != SSPSR_RNE);
    Dummy = LPC_SSP->DR;
  }
  while (--NumBytes);
}
#endif

/*********************************************************************
*
*       Local functions for display selection and LCDC init
*
**********************************************************************
*/

/*********************************************************************
*
*       _InitLCDPorts
*
* Function description
*   Initializes the port pins as needed for the LCD.
*/
#ifndef _WINDOWS
static void _InitLCDPorts(void) {
  LPC_IOCON->P0_4   = 7;  // LCD_VD_0
  LPC_IOCON->P0_5   = 7;  // LCD_VD_1
  LPC_IOCON->P0_6   = 7;  // LCD_VD_8
  LPC_IOCON->P0_7   = 7;  // LCD_VD_9
  LPC_IOCON->P0_8   = 7;  // LCD_VD_16
  LPC_IOCON->P0_9   = 7;  // LCD_VD_17

  LPC_IOCON->P1_20  = 7;  // LCD_VD_10
  LPC_IOCON->P1_21  = 7;  // LCD_VD_11
  LPC_IOCON->P1_22  = 7;  // LCD_VD_12
  LPC_IOCON->P1_23  = 7;  // LCD_VD_13
  LPC_IOCON->P1_24  = 7;  // LCD_VD_14
  LPC_IOCON->P1_25  = 7;  // LCD_VD_15
  LPC_IOCON->P1_26  = 7;  // LCD_VD_20
  LPC_IOCON->P1_27  = 7;  // LCD_VD_21
  LPC_IOCON->P1_28  = 7;  // LCD_VD_22
  LPC_IOCON->P1_29  = 7;  // LCD_VD_23

  LPC_IOCON->P2_0   = 7;  // LCD_PWR
  LPC_IOCON->P2_1   = 7;  // LCD_LE
  LPC_IOCON->P2_2   = 7;  // LCD_DCLK
  LPC_IOCON->P2_3   = 7;  // LCD_FP
  LPC_IOCON->P2_4   = 7;  // LCD_ENAB_M
  LPC_IOCON->P2_5   = 7;  // LCD_LP
  LPC_IOCON->P2_6   = 7;  // LCD_VD_4
  LPC_IOCON->P2_7   = 7;  // LCD_VD_5
  LPC_IOCON->P2_8   = 7;  // LCD_VD_6
  LPC_IOCON->P2_9   = 7;  // LCD_VD_7

  LPC_IOCON->P2_11  = 7;  // LCD_CLKIN
  // LPC_IOCON->P2_12  = 7;  // LCD_VD_18
  LPC_IOCON->P2_12  = 5;  // LCD_VD_3(LCD_VD_18 is not there in 5:6:5 mode and signal is at LCD_VD_3 ) 
  LPC_IOCON->P2_13  = 7;  // LCD_VD_19

  LPC_IOCON->P4_28  = 7;  // LCD_VD_2
  LPC_IOCON->P4_29  = 7;  // LCD_VD_3

  LPC_GPIO1->DIR   |= (1 << LCD_BACKLIGHT_BIT);
  LPC_GPIO1->CLR   |= (1 << LCD_BACKLIGHT_BIT);  // Initially set backlight to off
}
#endif

/*********************************************************************
*
*       _InitLCDC
*
* Function description
*   Inits the LCD controller, backlight and sets the frame buffer.
*/
#ifndef _WINDOWS
static void _InitLCDC(void) {
  U32 i;

  //
  // Translate and check params from EEPROM values to device values
  //
  if (_LcdParams.BPP == 16) {
    _LcdParams.BPP = LCDC_BPP_16_565;
  } else {
    while (1);  // Error, display mode not yet supported
  }
  if (_LcdParams.LCD == 0) {
    _LcdParams.LCD = LCDC_TFT;
  } else {
    while (1);  // Error, display type not yet supported
  }
  //
  // Init LCDC
  //
  LPC_SC->PCONP |=  (1UL << 0);  // Power the LCDC
  LPC_LCD->CTRL &= ~(1UL << 0);  // Disable the LCDC
  LPC_LCD->TIMH  = 0             // Configure horizontal axis
                   | ((((U32)_LcdParams.PPL / 16) - 1) <<  2)
                   |  (((U32)_LcdParams.HSW - 1)       <<  8)
                   |  (((U32)_LcdParams.HFP - 1)       << 16)
                   |  (((U32)_LcdParams.HBP - 1)       << 24)
                   ;
  LPC_LCD->TIMV  = 0             // Configure vertical axis
                   | (((U32)_LcdParams.LPP - 1) <<  0)
                   | (((U32)_LcdParams.VSW - 1) << 10)
                   | (((U32)_LcdParams.VFP)     << 16)
                   | (((U32)_LcdParams.VBP)     << 24)
                   ;
  LPC_LCD->POL   = 0             // Configure clock and signal polarity
                   | (_FindClockDivisor(_LcdParams.Clk) <<  0)
                   | (((U32)_LcdParams.ACB - 1)         <<  6)
                   | (((U32)_LcdParams.IVS)             << 11)
                   | (((U32)_LcdParams.IHS)             << 12)
                   | (((U32)_LcdParams.IPC)             << 13)
                   | (((U32)_LcdParams.PPL - 1)         << 16)
                   ;
  LPC_LCD->CTRL  = 0             // Configure operating mode and panel parameters
                   | ((U32)_LcdParams.BPP << 1)
                   | ((U32)BGR            << 8)
                   | ((U32)_LcdParams.LCD << 5)
                   ;
  for (i = 0; i < GUI_COUNTOF(LPC_LCD->PAL); i++) {
    LPC_LCD->PAL[i] = 0;  // Clear the color palette with black
  }
  LPC_SC->LCD_CFG = 0x0;  // No panel clock prescaler
  //
  // Enable LCDC
  //
  LPC_LCD->UPBASE  = VRAM_ADDR_PHYS;
  LPC_LCD->CTRL   |= (1 <<  0);                 // Enable LCD signals
  LPC_LCD->CTRL   |= (1 << 11);                 // Enable LCD power
  LPC_GPIO1->SET  |= (1 << LCD_BACKLIGHT_BIT);  // Set backlight to on
}
#endif

/*********************************************************************
*
*       Local functions for Truly LCD
*
**********************************************************************
*/

/*********************************************************************
*
*       _WriteLcdReg
*
* Function description
*   Write to display register. Used to configure Truly LCD display
*   controller IC on Embedded Artists QVGA Base Board.
*/
#ifndef _WINDOWS
static void _WriteLcdReg(U16 Addr, U16 Data) {
  U8 Buffer[2];

  LCD_CMD();
  Buffer[0] = 0;
  Buffer[1] = Addr & 0xFF;
  _SSP_Send(Buffer, 2);

  LCD_DATA();
  Buffer[0] = Data >> 8;
  Buffer[1] = Data & 0xFF;
  _SSP_Send(Buffer, 2);

  LCD_CMD();
  Buffer[0] = 0;
  Buffer[1] = 0x22;
  _SSP_Send(Buffer, 2);
}
#endif

/*********************************************************************
*
*       _InitTrulyLCD
*
* Function description
*   Initialize the LCD display. Used to configure Truly LCD display
*   controller IC on Embedded Artists QVGA Base Board.
*/
#ifndef _WINDOWS
static void _InitTrulyLCD(void) {
  //
  // Power LCD
  //
  LPC_GPIO2->DIR |= 1;  // Set to output
  LPC_GPIO2->SET  = 1;  // Output LCD power
  //
  // Set display CMD/DATA register select pin to output high
  //
  LPC_GPIO0->DIR |= (1 << LCD_REG_BIT);
  //
  // Display init sequence
  //
  _WriteLcdReg(0x00,0x0001);
  _DelayMs(15 * 2);
  _WriteLcdReg(0x03,0x6E3E);  // AAAC
  _WriteLcdReg(0x0C,0x0007);  // 0002
  _WriteLcdReg(0x0D,0x000E);  // 000A
  _WriteLcdReg(0x0E,0x2C00);  // 2C00
  _WriteLcdReg(0x1E,0x00AE);  // 00B8
  _DelayMs(15 * 2);

  _WriteLcdReg(0x07,0x0021);
  _DelayMs(50 * 2);
  _WriteLcdReg(0x07,0x0023);
  _DelayMs(50 * 2);
  _WriteLcdReg(0x07,0x0033);
  _DelayMs(50 * 2);

  _WriteLcdReg(0x01,0x2B3F);
  _WriteLcdReg(0x02,0x0600);
  _WriteLcdReg(0x10,0x0000);
  _DelayMs(15 * 2);
  _WriteLcdReg(0x11,0xC5B0);  // 60B0: RGB I/R
  _DelayMs(20 * 2);
  _WriteLcdReg(0x15,0x00D0);

  _WriteLcdReg(0x05,0x0000);
  _WriteLcdReg(0x06,0x0000);
  _WriteLcdReg(0x16,0xEF1C);
  _WriteLcdReg(0x17,0x0003);
  _WriteLcdReg(0x07,0x0233);
  _WriteLcdReg(0x0B,0x5310);
  _WriteLcdReg(0x0F,0x0000);
  _WriteLcdReg(0x25,0xE000);
  _DelayMs(20 * 2);

  _WriteLcdReg(0x41,0x0000);
  _WriteLcdReg(0x42,0x0000);
  _WriteLcdReg(0x48,0x0000);
  _WriteLcdReg(0x49,0x013F);
  _WriteLcdReg(0x44,0xEF00);
  _WriteLcdReg(0x45,0x0000);
  _WriteLcdReg(0x46,0x013F);
  _WriteLcdReg(0x4A,0x0000);
  _WriteLcdReg(0x4B,0x0000);
  _DelayMs(20 * 2);

  _WriteLcdReg(0x30,0x0707);
  _WriteLcdReg(0x31,0x0600);  // 0704
  _WriteLcdReg(0x32,0x0005);  // 0204
  _WriteLcdReg(0x33,0x0402);  // 0201
  _WriteLcdReg(0x34,0x0203);
  _WriteLcdReg(0x35,0x0204);
  _WriteLcdReg(0x36,0x0204);
  _WriteLcdReg(0x37,0x0401);  // 0502
  _WriteLcdReg(0x3A,0x0302);
  _WriteLcdReg(0x3B,0x0500);
  _DelayMs(20 * 2);
  _WriteLcdReg(0x22,0x0000);
  _DelayMs(20 * 2);
}
#endif

/*********************************************************************
*
*       Local functions for LCDs other than Truly
*
**********************************************************************
*/

/*********************************************************************
*
*       _EatWhite
*/
static void _EatWhite(char ** ps) {
  while ((**ps == ' ') && (**ps != '\0')) {
    (*ps)++;
  }
}

/*********************************************************************
*
*       _ParseDec
*
* Return value:
*   Number of digits parsed
*/
static int _ParseDec(char ** ps, int Len, int * pNumDigits) {
  U32 Data;
  int NumDigits;

  Data      = 0;
  NumDigits = 0;

  _EatWhite(ps);
  while (Len--) {
    int v;
    char c;

    c = **ps;
    v =  ((c >= '0') && (c <= '9')) ? c - '0' : -1;
    if (v >= 0) {
      Data = (Data * 10) + v;
      (*ps)++;
      NumDigits++;
    } else {
      if (pNumDigits != NULL) {
        *pNumDigits = NumDigits;
      }
      break;
    }
  }
  return Data;
}

/*********************************************************************
*
*       _ExecutePCA9532Cmd
*
* Function description
*   Executes a command on the multiplexer.
*/
#ifndef _WINDOWS
static void _ExecutePCA9532Cmd(char * s, int Len) {
  U32  v;
  char Cmd;
  U8   PwmDuty;

  Cmd = *s++;
  //
  // Parse all parameters
  //
  switch (Cmd) {
  case 'c':
  case 'd':
  case '3':
  case '5':
    v = _ParseDec(&s, Len - 1, NULL);
    break;
  default:
    return;
  }
  //
  // Exec cmd
  //
  switch (Cmd) {
  case 'c':  // Enable/disable backlight
    v = 100 - v;
    if (v > 100) {
      v = 100;
    }
    PwmDuty = (255 * v) / 100;
    I2C_PCA9532_SetPwm(I2C0_BASE_ADDR, PCA9532_I2C_ADDR, PCA9532_DISP_BL_BIT, 0, PwmDuty, 0);
    break;
  case 'd':  // Enable/disable display
    if (v) {
      v = 0;
    } else {
      v = 1;
    }
    I2C_PCA9532_SetPinSelect(I2C0_BASE_ADDR, PCA9532_I2C_ADDR, PCA9532_DISP_EN_BIT, v);
    break;
  case '3':  // Enable/disable display 3v3
    I2C_PCA9532_SetPinSelect(I2C0_BASE_ADDR, PCA9532_I2C_ADDR, PCA9532_DISP_3V3_EN_BIT, v);
    break;
  case '5':  // Enable/disable display 5v
    I2C_PCA9532_SetPinSelect(I2C0_BASE_ADDR, PCA9532_I2C_ADDR, PCA9532_DISP_5V_EN_BIT, v);
    break;
  }
}
#endif

/*********************************************************************
*
*       _ExecInitSequence
*
* Function description
*   Executes the LCD init sequence found in EEPROM.
*/
#ifndef _WINDOWS
static void _ExecInitSequence(char * s) {
  char * pCmd;
  int    ParamLen;  // Len of command parameters

  //
  // Init PCA9532 multiplexer
  //
  I2C_PCA9532_Init(I2C0_BASE_ADDR, PeripheralClock, 100000);
  //
  // Process init string
  //
  while (*s != '\0') {
    _EatWhite(&s);
    //
    // Calc len of next command
    //
    pCmd = s;
    while ((*s != ',') && (*s != '\0')) {
      s++;  // Skip chars until at end of command
    }
    ParamLen = (s - pCmd) - 1;
    if (*s == ',') {
      s++;  // Skip separator if any
    }
    //
    // Execute command
    //
    switch (*pCmd++) {
    case 'c':  // Exec PCA9532 action
      _ExecutePCA9532Cmd(pCmd, ParamLen);
      break;
    case 'd':  // Delay for a given number of ms
      _DelayMs(_ParseDec(&pCmd, ParamLen, NULL));
      break;
    case 'o':  // Open LCD, for us this means init the LCDC
      _InitLCDC();
      break;
    case 'v':  // Check if init string format is supported
      if (_ParseDec(&pCmd, ParamLen, NULL) > EEPROM_MAX_SEQ_VER) {
        while (1);  // Init string format not supported
      }
      break;
    }
  }
}
#endif

/*********************************************************************
*
*       _InitOtherLCD
*
* Function description
*   Init displays other than Truly display.
*/
#ifndef _WINDOWS
static void _InitOtherLCD(void) {
  //
  // Read and execute init sequence from EEPROM
  //
  _EEPROM_Read(_EepromConfigOffs.LcdInit, _acBuffer, _EepromConfigOffs.PowerDown - _EepromConfigOffs.LcdInit);  // Read LCD init sequence string
  _ExecInitSequence((char*)_acBuffer);
}
#endif

/*********************************************************************
*
*       Local functions for touch
*
**********************************************************************
*/

#if GUI_SUPPORT_TOUCH  // Used when touch screen support is enabled

/*********************************************************************
*
*       _SSP_Recv
*
* Function description
*   Sends data via the SSP0 interface. Used to configure Truly LCD
*   display controller IC on Embedded Artists QVGA Base Board.
*/
#ifndef _WINDOWS
static void _SSP_Recv(U8* pData, U32 NumBytes) {
  do {
    LPC_SSP->DR = 0xFF;                                            // Using peer-to-peer communication, SSPDR has to be written to receive data
    while ((LPC_SSP->SR & (SSPSR_BSY | SSPSR_RNE)) != SSPSR_RNE);  // Wait for not busy and receive buffer not empty
    *pData++ = LPC_SSP->DR;
  } while (--NumBytes);
}
#endif

/*********************************************************************
*
*       _SSP_SendCmd
*
* Function description
*   Sends a cmd via the SSP0 interface and returns the answer.
*   Used to configure Truly LCD display controller IC on
*   Embedded Artists QVGA Base Board.
*/
#ifndef _WINDOWS
static U16 _SSP_SendCmd(U8 Cmd) {
  U8 v[2];

  v[0] = Cmd;

  TS_CS_SET();
  _SSP_Send(v, 1);
  _SSP_Recv(v, 2);
  TS_CS_CLR();

  return ((v[0] << 8) | v[1]);
}
#endif

/*********************************************************************
*
*       _CheckUpdateTouch
*
* Function description
*   Reads touch values from ADC on Truly LCD display
*   controller IC on Embedded Artists QVGA Base Board.
*   Checks if a touch event has occurred. If we found an event the
*   static variables for x and y will be filled with new values that
*   can be processed via GUI touch routines.
*
* Return value:
*      0  No touch event, x, y have not been updated
*   != 0  Touch event occurred, x, y have been updated and return value is read z value
*/
#ifndef _WINDOWS
static int _CheckUpdateTouch(void) {
  int x;
  int y;
  int z;
  int z1;
  int z2;
  int LastX;
  int LastY;
  int LastZ1;
  int LastZ2;
  int i;

  LastY   = _SSP_SendCmd(READ_Y(0));
  LastY >>= 3;

  LastX   = _SSP_SendCmd(READ_X(0));
  LastX >>= 3;

  LastZ1   = _SSP_SendCmd(READ_Z1(0));
  LastZ1 >>= 3;

  LastZ2   = _SSP_SendCmd(READ_Z2(0));
  LastZ2 >>= 3;

  //
  // Check for touch event
  //
  z = (LastX * LastZ2 - LastZ1) / LastZ1;
  if ((z > 40000) || (z == 0)) {
    return 0;  // No update
  }
  //
  // Get debounced y value
  //
  for (i = 0; i < TS_DEBOUNCE_MAX; i++) {
    y   = _SSP_SendCmd(READ_Y(0));
    y >>= 3;
    if (ABS(LastY - y) <= TS_DEBOUNCE_TOL) {
      break;
    }
    LastY = y;
  }
  //
  // Get debounced x value
  //
  for (i = 0; i < TS_DEBOUNCE_MAX; i++) {
    x   = _SSP_SendCmd(READ_X(0));
    x >>= 3;
    if (ABS(LastX - x) <= TS_DEBOUNCE_TOL) {
      break;
    }
    LastX = x;
  }
  //
  // Get debounced z1 value
  //
  for (i = 0; i < TS_DEBOUNCE_MAX; i++) {
    z1   = _SSP_SendCmd(READ_Z1(0));
    z1 >>= 3;
    if (ABS(LastZ1 - z1) <= TS_DEBOUNCE_TOL) {
      break;
    }
    LastZ1 = z1;
  }
  //
  // Get debounced z2 value
  //
  for (i = 0; i < TS_DEBOUNCE_MAX; i++) {
    z2   = _SSP_SendCmd(READ_Z1(0));
    z2 >>= 3;
    if (ABS(LastZ2 - z2) <= TS_DEBOUNCE_TOL) {
      break;
    }
    LastZ2 = z2;
  }
  //
  // Update static variables if we have a valid touch
  //
  z = ((x * z2) - z1) / z1;
  if (z > 40000) {
    return 0;   // No update
  }
  _TouchX = x;
  _TouchY = y;
  return z;
}
#endif

/*********************************************************************
*
*       _InitTouch
*
* Function description
*   Initializes the touch screen.
*/
#ifndef _WINDOWS
static void _InitTouch(void) {
  U32 TouchOrientation;
  U8  Config[3];
  U8  TouchHasSwapXY;
  int TouchADLeft;
  int TouchADRight;
  int TouchADTop;
  int TouchADBottom;

  //
  // Init ports and SSP interface as needed for touch
  //
  LPC_GPIO0->DIR |= (1 << TOUCH_CS_BIT);  // P0.20 as output (touch controller CS)
  _InitSSP(SSP_MODE_TS);
  Config[0] = REF_ON;
  Config[1] = (READ_12BIT_SER(ADS_A2A1A0_vaux) | ADS_PD10_ALL_ON);
  Config[2] = PWRDOWN;
  TS_CS_SET();
  _SSP_Send(Config, 3);
  TS_CS_CLR();
  //
  // Set touch orientation and calibrate touch
  //
  if (_Display == DISPLAY_TRULY_240_320) {
    //
    // 3.2" Truly display
    // Swap XY for touch as touch does not have the same orientation as the display
    //
    if (LCD_GetSwapXYEx(0)) {
      TouchHasSwapXY = 0;
    } else {
      TouchHasSwapXY = 1;
    }
    //
    // Calibrate touch
    //
    TouchOrientation = (GUI_MIRROR_X * LCD_GetMirrorYEx(0)) |  // XY swapped
                       (GUI_MIRROR_Y * LCD_GetMirrorXEx(0)) |  // XY swapped
                       (GUI_SWAP_XY  * TouchHasSwapXY) ;
    GUI_TOUCH_SetOrientation(TouchOrientation);
    if (TouchHasSwapXY) {
      GUI_TOUCH_Calibrate(GUI_COORD_X, 0, YSIZE_PHYS, TOUCH_TRULY_240_320_AD_LEFT, TOUCH_TRULY_240_320_AD_RIGHT);
      GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, XSIZE_PHYS, TOUCH_TRULY_240_320_AD_TOP , TOUCH_TRULY_240_320_AD_BOTTOM);
    } else {
      GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, XSIZE_PHYS, TOUCH_TRULY_240_320_AD_LEFT, TOUCH_TRULY_240_320_AD_RIGHT);
      GUI_TOUCH_Calibrate(GUI_COORD_X, 0, YSIZE_PHYS, TOUCH_TRULY_240_320_AD_TOP , TOUCH_TRULY_240_320_AD_BOTTOM);
    }
  } else {
    //
    // LCD board other than truly
    //
    if        (_Display == DISPLAY_BOARD_480_272) {
      TouchADLeft   = TOUCH_BOARD_480_272_AD_LEFT;
      TouchADRight  = TOUCH_BOARD_480_272_AD_RIGHT;
      TouchADTop    = TOUCH_BOARD_480_272_AD_TOP;
      TouchADBottom = TOUCH_BOARD_480_272_AD_BOTTOM;
    } else if (_Display == DISPLAY_BOARD_800_480) {
      TouchADLeft   = TOUCH_BOARD_800_480_AD_LEFT;
      TouchADRight  = TOUCH_BOARD_800_480_AD_RIGHT;
      TouchADTop    = TOUCH_BOARD_800_480_AD_TOP;
      TouchADBottom = TOUCH_BOARD_800_480_AD_BOTTOM;
    } else {
      while (1);  // Touch not supported
    }
    //
    // Calibrate touch
    //
    TouchOrientation = (GUI_MIRROR_X * LCD_GetMirrorXEx(0)) |
                       (GUI_MIRROR_Y * LCD_GetMirrorYEx(0)) |
                       (GUI_SWAP_XY  * LCD_GetSwapXYEx (0)) ;
    GUI_TOUCH_SetOrientation(TouchOrientation);
    GUI_TOUCH_Calibrate(GUI_COORD_X, 0, XSIZE_PHYS, TouchADLeft, TouchADRight);
    GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, YSIZE_PHYS, TouchADTop , TouchADBottom);
  }
  _IsInited = 1;
}
#endif

#endif  // GUI_SUPPORT_TOUCH

/*********************************************************************
*
*       Local functions for display selection and general init
*
**********************************************************************
*/

/*********************************************************************
*
*       _InitLCD
*
* Function description
*   Initializes the port pins as needed for the LCD and selects
*   an available LCD.
*/
#ifndef _WINDOWS
static void _InitLCD(void) {
  _InitLCDPorts();
  //
  // Setup BUS priorities
  //
  MATRIX_ARB = 0            // Set AHB Matrix priorities [0..3] with 3 being highest priority
               | (1 <<  0)  // PRI_ICODE : I-Code bus priority. Should be lower than PRI_DCODE for proper operation.
               | (2 <<  2)  // PRI_DCODE : D-Code bus priority.
               | (0 <<  4)  // PRI_SYS   : System bus priority.
               | (0 <<  6)  // PRI_GPDMA : General Purpose DMA controller priority.
               | (0 <<  8)  // PRI_ETH   : Ethernet: DMA priority.
               | (3 << 10)  // PRI_LCD   : LCD DMA priority.
               | (0 << 12)  // PRI_USB   : USB DMA priority.
               ;
  //
  // Check which display is connected and initialize it
  //
  if (_Display == DISPLAY_TRULY_240_320) {
    //
    // No configuration EEPROM found, display is Truly (if any connected).
    // LCDC is enabled after display init.
    //
    _InitSSP(SSP_MODE_LCD);
    _InitTrulyLCD();
    _InitLCDC();
  } else {
    //
    // EEPROM with settings was found. Either 4.3" or 7" display is connected.
    // LCDC is enabled upon request of the stored initialization sequence in EEPROM.
    //
    _InitOtherLCD();
  }
}
#endif

/*********************************************************************
*
*       _InitController
*
* Function description
*   Initializes the LCD controller and touch screen
*/
#ifndef _WINDOWS
static void _InitController(unsigned LayerIndex) {
  _InitLCD();
  //
  // Set display size and video-RAM address
  //
  LCD_SetSizeEx (XSIZE_PHYS, YSIZE_PHYS, LayerIndex);
  LCD_SetVSizeEx(VXSIZE_PHYS, VYSIZE_PHYS, LayerIndex);
  LCD_SetVRAMAddrEx(LayerIndex, (void*)VRAM_ADDR_PHYS);
  //
  // Init touch
  //
#if GUI_SUPPORT_TOUCH  // Used when touch screen support is enabled
  _InitTouch();
#endif
}
#endif

/*********************************************************************
*
*       _UpdateDisplayConfig
*
* Function description
*   Checks for attached display and updates the LCDC parameters.
*/
#ifndef _WINDOWS
static void _UpdateDisplayConfig(void) {
  U32 v;

  //
  // Init I2C for access to configuration EEPROM
  //
  LPC_SC->PCONP |= (1UL << 7);                       // Power I2C
  LPC_IOCON->P0_27 = (LPC_IOCON->P0_27 & ~7UL) | 1;  // I2C0_SDA
  LPC_IOCON->P0_28 = (LPC_IOCON->P0_27 & ~7UL) | 1;  // I2C0_SCL
  I2C_Init(I2C0_BASE_ADDR, PeripheralClock, 100000);
  //
  // Check if we have an EEPROM that contains display settings and
  // save the decision for later usage.
  //
  _EEPROM_Read(0, (U8*)&v, 4);
  if (v == EEPROM_MAGIC_NUMBER) {
    //
    // Read LCD settings from EEPROM
    //
    _EEPROM_Read(64, (U8*)&_EepromConfigOffs, sizeof(EEPROM_CONFIG_OFFS));           // Offset of LCD params is stored at offset 64 in the EEPROM
    _EEPROM_Read(_EepromConfigOffs.LcdParams, (U8*)&_LcdParams, sizeof(LCD_PARAM));  // Read LCD params
    if        (_LcdParams.PPL == 480) {
      _Display = DISPLAY_BOARD_480_272;
    } else if (_LcdParams.PPL == 800) {
      _Display = DISPLAY_BOARD_800_480;
    } else {
      while (1);  // Error, display type not supported
    }
  } else {
    _Display = DISPLAY_TRULY_240_320;
    //
    // Setup display settings for Truly display manually as there is no EEPROM on this display module
    //
    _LcdParams.HBP  = HBP_TRULY_3_2;
    _LcdParams.HFP  = HFP_TRULY_3_2;
    _LcdParams.HSW  = HSW_TRULY_3_2;
    _LcdParams.PPL  = PPL_TRULY_3_2;
    _LcdParams.VBP  = VBP_TRULY_3_2;
    _LcdParams.VFP  = VFP_TRULY_3_2;
    _LcdParams.VSW  = VSW_TRULY_3_2;
    _LcdParams.LPP  = LPP_TRULY_3_2;
    _LcdParams.IOE  = IOE_TRULY_3_2;
    _LcdParams.IPC  = IPC_TRULY_3_2;
    _LcdParams.IHS  = IHS_TRULY_3_2;
    _LcdParams.IVS  = IVS_TRULY_3_2;
    _LcdParams.ACB  = ACB_TRULY_3_2;
    _LcdParams.BPP  = BPP_TRULY_3_2;
    _LcdParams.Clk  = CLK_TRULY_3_2;
    _LcdParams.LCD  = LCD_TRULY_3_2;
    _LcdParams.Dual = DUAL_TRULY_3_2;
  }
}
#endif

/*********************************************************************
*
*       Global functions for display init
*
**********************************************************************
*/

/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*/
void LCD_X_Config(void) {
  const GUI_DEVICE_API * pDriver;

  #ifndef _WINDOWS
    _UpdateDisplayConfig();
  #endif
  //
  // Check framebuffer size
  //
  #ifndef _WINDOWS
    if ((FB_XSIZE * FB_YSIZE) < (VXSIZE_PHYS * VYSIZE_PHYS)) {
      while (1);  // Error, framebuffer too small
    }
  #endif
  //
  // Set display driver and color conversion for 1st layer
  //
  #ifndef _WINDOWS
  if (_Display == DISPLAY_TRULY_240_320) {
    pDriver = DISPLAY_DRIVER_TRULY;
  } else {
    pDriver = DISPLAY_DRIVER_OTHER;
  }
  #else
  pDriver = GUIDRV_WIN32;
  #endif
  GUI_DEVICE_CreateAndLink(pDriver, COLOR_CONVERSION, 0, 0);
  //
  // Display driver configuration, required for Lin-driver
  //
  LCD_SetPosEx(0, 0, 0);
  if (LCD_GetSwapXYEx(0)) {
    LCD_SetSizeEx  (0, YSIZE_PHYS , XSIZE_PHYS);
    LCD_SetVSizeEx (0, VYSIZE_PHYS, VXSIZE_PHYS);
  } else {
    LCD_SetSizeEx  (0, XSIZE_PHYS , YSIZE_PHYS);
    LCD_SetVSizeEx (0, VXSIZE_PHYS, VYSIZE_PHYS);
  }
  LCD_SetVRAMAddrEx(0, (void*)VRAM_ADDR_VIRT);
  //
  // Set user palette data (only required if no fixed palette is used)
  //
  #if defined(PALETTE)
    LCD_SetLUTEx(0, PALETTE);
  #endif
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  #ifndef _WINDOWS
  LCD_X_SETORG_INFO * pSetOrg;
  #endif
  int r;

  (void) LayerIndex;

  switch (Cmd) {
  //
  // Required
  //
  case LCD_X_INITCONTROLLER:
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    // ...
    #ifndef _WINDOWS
      _InitController(0);
    #endif
    return 0;
  case LCD_X_SETORG:
    //
    // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
    //
    #ifndef _WINDOWS
      pSetOrg = (LCD_X_SETORG_INFO *)pData;
      _SetDisplayOrigin(pSetOrg->xPos, pSetOrg->yPos);
    #endif
    return 0;
  default:
    r = -1;
  }
  return r;
}

/*********************************************************************
*
*       Global functions for GUI touch
*
**********************************************************************
*/

#if GUI_SUPPORT_TOUCH  // Used when touch screen support is enabled

/*********************************************************************
*
*       GUI_TOUCH_X_ActivateX()
*
* Function decription:
*   Called from GUI, if touch support is enabled.
*   Switches on voltage on X-axis,
*   prepares measurement for Y-axis.
*   Voltage on Y-axis is switched off.
*/
void GUI_TOUCH_X_ActivateX(void) {
}

/*********************************************************************
*
*       GUI_TOUCH_X_ActivateY()
*
* Function decription:
*   Called from GUI, if touch support is enabled.
*   Switches on voltage on Y-axis,
*   prepares measurement for X-axis.
*   Voltage on X-axis is switched off.
*/
void GUI_TOUCH_X_ActivateY(void) {
}

/*********************************************************************
*
*       GUI_TOUCH_X_MeasureX()
*
* Function decription:
*   Called from GUI, if touch support is enabled.
*   Measures voltage of X-axis.
*/
int  GUI_TOUCH_X_MeasureX(void) {
  return _TouchX;
}

/*********************************************************************
*
*       GUI_TOUCH_X_MeasureY()
*
* Function decription:
*   Called from GUI, if touch support is enabled.
*   Measures voltage of Y-axis.
*/
int  GUI_TOUCH_X_MeasureY(void) {
  return _TouchY;
}

#endif  // GUI_SUPPORT_TOUCH

/*********************************************************************
*
*       ExecTouch
*
* Function description
*   Check for new touch event. Static x, y coordinates will be updated
*   by the _CheckUpdateTouch() routine. If no touch event has occurred
*   we do nothing. Is called by SysTickHandler().
*/
#ifndef _WINDOWS
void ExecTouch(void) {
  GUI_PID_STATE State;
  int           HasUpdate;

  if (_IsInited == 0) {
    return;
  }
  HasUpdate = _CheckUpdateTouch();
  _SSP_SendCmd(PWRDOWN);
  if (HasUpdate) {
    _PenIsDown = 1;
    GUI_TOUCH_Exec();
  } else if (_PenIsDown) {
    //
    // No further touch event after at least one touch event means we have
    // lift the pen from the touch screen which means a click.
    //
    _PenIsDown = 0;
    GUI_PID_GetState(&State);
    State.Pressed = 0;
    GUI_PID_StoreState(&State);
  }
}
#endif

/*************************** End of file ****************************/
