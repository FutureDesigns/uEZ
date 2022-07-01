/*-------------------------------------------------------------------------*
 * File:  Sharp_LQ070Y3LW01.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the Sharp LQ070Y3LW01.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZGPIO.h>
#include <uEZProcessor.h>
#include <HAL/LCDController.h>
//#include <HAL/GPIO.h>
#include <Device/LCD.h>
#include "Sharp_LQ070Y3LW01.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RESOLUTION_X        800
#define RESOLUTION_Y        480

#ifndef LCD_SHARP_LQ070Y3LW01_FLIP_SCREEN_X
#define LCD_SHARP_LQ070Y3LW01_FLIP_SCREEN_X       0
#endif

#ifndef LCD_SHARP_LQ070Y3LW01_FLIP_SCREEN_Y
#define LCD_SHARP_LQ070Y3LW01_FLIP_SCREEN_Y       0
#endif

// cannot reach 33.3Mhz on LPC1788 with default Crystal
// 28-36MHz is within spec
#ifndef SHARP_LQ070Y3LW01_DOTCLOCK_HZ
    #define     SHARP_LQ070Y3LW01_DOTCLOCK_HZ       30000000
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_LCD *iHAL;
    TUInt32 iBaseAddress;
    int aNumOpen;
    TUInt32 iBacklightLevel;
    HAL_LCDController **iLCDController;
    DEVICE_Backlight **iBacklight;
    const T_uezLCDConfiguration *iConfiguration;
    T_uezSemaphore iVSyncSem;
    // control pins for LCD, LVDS and back light system
    T_uezGPIOPortPin iLVDSShutPin;
    T_uezGPIOPortPin iBackLightEnablePin;
    T_uezGPIOPortPin iLCDPWMPin;
    T_uezGPIOPortPin iLCDPowerEnablePin;    
} T_LQ070Y3LW01Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LCDControllerSettings LCD_LQ070Y3LW01_settings;

static const T_LCDControllerSettings LCD_LQ070Y3LW01_params16bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_565,

    0,         /* Horizontal back porch */
    0,         /* Horizontal front porch */
    256,          /* HSYNC pulse width */
    800,        /* Pixels per line */

    0,          /* Vertical back porch */
    0,          /* Vertical front porch */
    45,         /* VSYNC pulse width */
    480,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    EFalse,      /* Invert panel clock */
    EFalse,      /* Invert HSYNC */
    EFalse,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     // Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0xA0000000, // Default Base address
    SHARP_LQ070Y3LW01_DOTCLOCK_HZ,
};

static const T_LCDControllerSettings LCD_LQ070Y3LW01_paramsI15bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_I555,

    0,         /* Horizontal back porch */
    0,         /* Horizontal front porch */
    230,          /* HSYNC pulse width */
    800,        /* Pixels per line */

    0,          /* Vertical back porch */
    0,          /* Vertical front porch */
    41,         /* VSYNC pulse width */
    480,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* invert output enable */
    ETrue,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     // Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0xA0000000, // Default Base address
    SHARP_LQ070Y3LW01_DOTCLOCK_HZ,
//    2000000,
};

static const T_LCDControllerSettings LCD_LQ070Y3LW01_params8bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_8,

    0,         /* Horizontal back porch */
    0,         /* Horizontal front porch */
    256,          /* HSYNC pulse width */
    800,        /* Pixels per line */

    0,          /* Vertical back porch */
    0,          /* Vertical front porch */
    45,         /* VSYNC pulse width */
    480,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    EFalse,      /* Invert panel clock */
    EFalse,      /* Invert HSYNC */
    EFalse,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     // Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0xA0000000, // Default Base address
    SHARP_LQ070Y3LW01_DOTCLOCK_HZ,
};

static T_uezLCDConfiguration LCD_LQ070Y3LW01_configuration_16Bit = {
    RESOLUTION_X,
    RESOLUTION_Y,
    UEZLCD_COLOR_DEPTH_16_BIT,
    UEZLCD_ORDER_RGB,
    0,
    1,
    RESOLUTION_X*2,
    RESOLUTION_X*2,
    256
};

static T_uezLCDConfiguration LCD_LQ070Y3LW01_configuration_I15Bit = {
    RESOLUTION_X,
    RESOLUTION_Y,
    UEZLCD_COLOR_DEPTH_I15_BIT,
    UEZLCD_ORDER_RGB,
    0,
    1,
    RESOLUTION_X*2,
    RESOLUTION_X*2,
    256
};

static T_uezLCDConfiguration LCD_LQ070Y3LW01_configuration_8Bit = {
    RESOLUTION_X,
    RESOLUTION_Y,
    UEZLCD_COLOR_DEPTH_8_BIT,
    UEZLCD_ORDER_RGB,
    0,
    1,
    RESOLUTION_X*1,
    RESOLUTION_X*1,
    256
};

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_LCD LCD_SHARP_LQ070Y3LW01_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_InitializeWorkspace_16Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for LQ070Y3LW01 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ070Y3LW01_InitializeWorkspace_16Bit(void *aW) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    p->iBaseAddress = 0xA0000000;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_LQ070Y3LW01_configuration_16Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_InitializeWorkspace_I15Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for LQ070Y3LW01 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ070Y3LW01_InitializeWorkspace_I15Bit(void *aW) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    p->iBaseAddress = 0xA0000000;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_LQ070Y3LW01_configuration_I15Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_InitializeWorkspace_8Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for LQ070Y3LW01 LCD.
 * Inputs:
 *      void *aW                    -- Particular  workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ070Y3LW01_InitializeWorkspace_8Bit(void *aW) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    p->iBaseAddress = 0xA0000000;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_LQ070Y3LW01_configuration_8Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Start the LCD screen.  If this is the first open, initialize the
 *      screen.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is opened, returns
 *                                  UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_Open(void *aW) {
  T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
  HAL_LCDController **plcdc;
  T_uezError error = UEZ_ERROR_NONE;
  p->aNumOpen++;
  plcdc = p->iLCDController;
  if (p->aNumOpen == 1) {
    
        if(p->iLVDSShutPin != GPIO_NONE){
        // LVDS Shutdown Pin must be pulled high to turn on.
        UEZGPIOSetMux(p->iLVDSShutPin, 0);
        UEZGPIOSet(p->iLVDSShutPin);
        UEZGPIOOutput(p->iLVDSShutPin);      
        }
  
        // LCD Power set low
        UEZGPIOSetMux(p->iLCDPowerEnablePin, 0);
        UEZGPIOClear(p->iLCDPowerEnablePin);
        UEZGPIOOutput(p->iLCDPowerEnablePin);
        
        // Backlight PWM set high
        if(p->iLCDPWMPin != GPIO_NONE){
        UEZGPIOSetMux(p->iLCDPWMPin, 0);
        UEZGPIOSet(p->iLCDPWMPin);
        UEZGPIOOutput(p->iLCDPWMPin);
        }
        
        // Backlight Power enable set low
        if(p->iBackLightEnablePin != GPIO_NONE){
        UEZGPIOSetMux(p->iBackLightEnablePin, 0);
        UEZGPIOClear(p->iBackLightEnablePin);
        UEZGPIOOutput(p->iBackLightEnablePin);
        }
  
    if (!error) {
      switch (p->iConfiguration->iColorDepth) {
      case UEZLCD_COLOR_DEPTH_8_BIT:
        LCD_LQ070Y3LW01_settings = LCD_LQ070Y3LW01_params8bit;
        break;
      default:
      case UEZLCD_COLOR_DEPTH_16_BIT:
        LCD_LQ070Y3LW01_settings = LCD_LQ070Y3LW01_params16bit;
        break;
      case UEZLCD_COLOR_DEPTH_I15_BIT:
        LCD_LQ070Y3LW01_settings = LCD_LQ070Y3LW01_paramsI15bit;
        break;
      }
      LCD_LQ070Y3LW01_settings.iBaseAddress = p->iBaseAddress;
      error = (*plcdc)->Configure(plcdc, &LCD_LQ070Y3LW01_settings); // start DOTCLK immediately
      (*p->iLCDController)->On(p->iLCDController); // start HSYNC and VSYNC (same as UEZLCDOn(lcd))
      UEZTaskDelay(200); // delay before turning backlight on, min 10 frame
      (*p->iBacklight)->On(p->iBacklight); // turn backlight on      
    }
  }
  return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      End access to the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_Close(void *aW) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD to use a particular LCD controller.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully configured,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ070Y3LW01_Configure(
            void *aW,
            HAL_LCDController **aLCDController,
            TUInt32 aBaseAddress,
            DEVICE_Backlight **aBacklight) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    T_uezError error = UEZ_ERROR_NONE;

    p->iLCDController = aLCDController;
    p->iBaseAddress = aBaseAddress;
    p->iBacklight = aBacklight;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the LCD device.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_GetInfo(void *aW, T_uezLCDConfiguration *aConfiguration) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;

    // Use the setting based on color depth
    *aConfiguration = *p->iConfiguration;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_GetFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Returns a pointer to the frame memory in the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 *      TUInt32 aFrame          -- Index to frame (0 based)
 *      void **aFrameBuffer     -- Pointer to base address
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_GetFrame(
            void *aW,
            TUInt32 aFrame,
            void **aFrameBuffer) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    *aFrameBuffer = (void *)(p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_ShowFrame
 *---------------------------------------------------------------------------*
 * Description:
 *      Makes the passed frame the actively viewed frame on the LCD
 *      (if not already).
 * Inputs:
 *      void *aW                -- Workspace
 *      TUInt32 aFrame          -- Index to frame (0 based)
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_ShowFrame(void *aW, TUInt32 aFrame) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    (*p->iLCDController)->SetBaseAddr(
            p->iLCDController,
            p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2,
            EFalse);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_On(void *aW) {
    // Turn back on to the remembered level
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    (*p->iLCDController)->On(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->On(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns off the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_Off(void *aW) {
    // Turn off, but don't remember the level
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    (*p->iLCDController)->Off(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->Off(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_SetBacklightLevel
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on or off the backlight.  A value of 0 is off and values of 1
 *      or higher is higher levels of brightness (dependent on the LCD
 *      display).  If a display is on/off only, this value will be 1 or 0
 *      respectively.
 * Inputs:
 *      void *aW                -- Workspace
 *      TUInt32 aLevel          -- Level of backlight
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *                                  If the backlight intensity level is
 *                                  invalid, returns UEZ_ERROR_OUT_OF_RANGE.
 *                                  Returns UEZ_ERROR_NOT_SUPPORTED if
 *                                  no backlight for LCD.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_SetBacklightLevel(void *aW, TUInt32 aLevel) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;
    TUInt16 level;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    // Limit the backlight level
    if (aLevel > p->iConfiguration->iNumBacklightLevels)
        aLevel = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    p->iBacklightLevel = aLevel;

    // Scale backlight to be 0 - 0xFFFF
    level = (aLevel * 0xFFFF) / p->iConfiguration->iNumBacklightLevels;

    return (*p->iBacklight)->SetRatio(p->iBacklight, level);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_GetBacklightLevel
 *---------------------------------------------------------------------------*
 * Description:
 *      Returns the current backlight level as well as the number of
 *      maximum light levels available.
 * Inputs:
 *      void *aW                -- Workspace
 *      TUInt32 *aLevel          -- Level of backlight returned
 *      TUInt32 *aNumLevels      -- Number of levels maximum
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *                                  If the backlight intensity level is
 *                                  invalid, returns UEZ_ERROR_OUT_OF_RANGE.
 *                                  Returns UEZ_ERROR_NOT_SUPPORTED if
 *                                  no backlight for LCD.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_GetBacklightLevel(
        void *aW, 
        TUInt32 *aLevel,
        TUInt32 *aNumLevels) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aW;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (aNumLevels)
        *aNumLevels = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    *aLevel = p->iBacklightLevel;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_SetPaletteColor
 *---------------------------------------------------------------------------*
 * Description:
 *      Change the color of a palette entry given the red, green, blue
 *      component of a particular color index.  The color components are
 *      expressed in full 16-bit values at a higher resolution than
 *      the hardware can usually perform.  The color is down shifted to what
 *      the hardware can handle.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aColorIndex         -- Index to palette entry
 *      TUInt16 aRed                -- Red value
 *      TUInt16 aGreen              -- Green value
 *      TUInt16 aBlue               -- Blue value
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *                                  If the index is invalid, returns
 *                                  UEZ_ERROR_OUT_OF_RANGE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_SetPaletteColor(
                    void *aWorkspace,
                    TUInt32 aColorIndex,
                    TUInt16 aRed,
                    TUInt16 aGreen,
                    TUInt16 aBlue) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aWorkspace;

    return (*p->iLCDController)->SetPaletteColor(
                p->iLCDController,
                aColorIndex,
                aRed,
                aGreen,
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_VerticalSyncCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      This routine is called from within an interrupt when the vertical
 *      sync occurs.  This routines releases any waiting callers on
 *      the semaphore.
 * Inputs:
 *      void *aCallbackWorkspace -- LCD workspace
 *---------------------------------------------------------------------------*/
static void LCD_LQ070Y3LW01_VerticalSyncCallback(void *aCallbackWorkspace) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aCallbackWorkspace;

    _isr_UEZSemaphoreRelease(p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ070Y3LW01_WaitForVerticalSync
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for a vertical sync be enabling the vertical sync callback
 *      and waiting on a semaphore.
 * Inputs:
 *      void *aWorkspace -- LCD workspace
 *      TUInt32 aTimeout -- Timeout in ms or UEZ_TIMEOUT_INFINITE
 * Outputs:
 *      T_uezError -- UEZ_ERROR_TIMEOUT or UEZ_ERROR_OK.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ070Y3LW01_WaitForVerticalSync(
    void *aWorkspace,
    TUInt32 aTimeout) {
    T_LQ070Y3LW01Workspace *p = (T_LQ070Y3LW01Workspace *)aWorkspace;
    HAL_LCDController **p_hal = p->iLCDController;

    UEZSemaphoreGrab(p->iVSyncSem, 0);
    (*p_hal)->EnableVerticalSync(p_hal, LCD_LQ070Y3LW01_VerticalSyncCallback, p);
    return UEZSemaphoreGrab(p->iVSyncSem, aTimeout);
}

void LCD_LQ070Y3LW01_Create(char* aName,
                              T_uezGPIOPortPin aLVDSShutPin,
                              T_uezGPIOPortPin aLCDPowerEnablePin,
                              T_uezGPIOPortPin aLCDPWMPin,
                              T_uezGPIOPortPin aBackLightEnablePin) {
    T_LQ070Y3LW01Workspace *p;
    T_uezDevice lcd;
    T_uezDeviceWorkspace *p_lcd;
    UEZDeviceTableFind(aName, &lcd);
    UEZDeviceTableGetWorkspace(lcd, &p_lcd);
    p = (T_LQ070Y3LW01Workspace*)p_lcd;
   
    p->iLVDSShutPin = aLVDSShutPin;
    p->iLCDPowerEnablePin = aLCDPowerEnablePin;
    p->iLCDPWMPin = aLCDPWMPin;
    p->iBackLightEnablePin = aBackLightEnablePin;    
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LCD LCD_SHARP_LQ070Y3LW01_Interface_16Bit = {
	{
	    // Common interface
	    "LCD:SHARP_LQ070Y3LW01:16Bit",
	    0x0100,
	    LCD_LQ070Y3LW01_InitializeWorkspace_16Bit,
	    sizeof(T_LQ070Y3LW01Workspace),
	},
	
    // Functions
    LCD_LQ070Y3LW01_Open,
    LCD_LQ070Y3LW01_Close,
    LCD_LQ070Y3LW01_Configure,
    LCD_LQ070Y3LW01_GetInfo,
    LCD_LQ070Y3LW01_GetFrame,
    LCD_LQ070Y3LW01_ShowFrame,
    LCD_LQ070Y3LW01_On,
    LCD_LQ070Y3LW01_Off,
    LCD_LQ070Y3LW01_SetBacklightLevel,
    LCD_LQ070Y3LW01_GetBacklightLevel,
    LCD_LQ070Y3LW01_SetPaletteColor,
    // v2.04
    LCD_LQ070Y3LW01_WaitForVerticalSync,
};

const DEVICE_LCD LCD_SHARP_LQ070Y3LW01_Interface_I15Bit = {
	{
	    // Common interface
	    "LCD:SHARP_LQ070Y3LW01:I15Bit",
	    0x0100,
	    LCD_LQ070Y3LW01_InitializeWorkspace_I15Bit,
	    sizeof(T_LQ070Y3LW01Workspace),
	},
	
    // Functions
    LCD_LQ070Y3LW01_Open,
    LCD_LQ070Y3LW01_Close,
    LCD_LQ070Y3LW01_Configure,
    LCD_LQ070Y3LW01_GetInfo,
    LCD_LQ070Y3LW01_GetFrame,
    LCD_LQ070Y3LW01_ShowFrame,
    LCD_LQ070Y3LW01_On,
    LCD_LQ070Y3LW01_Off,
    LCD_LQ070Y3LW01_SetBacklightLevel,
    LCD_LQ070Y3LW01_GetBacklightLevel,
    LCD_LQ070Y3LW01_SetPaletteColor,
    // v2.04
    LCD_LQ070Y3LW01_WaitForVerticalSync,
};

const DEVICE_LCD LCD_SHARP_LQ070Y3LW01_Interface_8Bit = {
	{
	    // Common interface
	    "LCD:SHARP_LQ070Y3LW01:8Bit",
	    0x0100,
	    LCD_LQ070Y3LW01_InitializeWorkspace_8Bit,
	    sizeof(T_LQ070Y3LW01Workspace),
	},
	
    // Functions
    LCD_LQ070Y3LW01_Open,
    LCD_LQ070Y3LW01_Close,
    LCD_LQ070Y3LW01_Configure,
    LCD_LQ070Y3LW01_GetInfo,
    LCD_LQ070Y3LW01_GetFrame,
    LCD_LQ070Y3LW01_ShowFrame,
    LCD_LQ070Y3LW01_On,
    LCD_LQ070Y3LW01_Off,
    LCD_LQ070Y3LW01_SetBacklightLevel,
    LCD_LQ070Y3LW01_GetBacklightLevel,
    LCD_LQ070Y3LW01_SetPaletteColor,
    // v2.04
    LCD_LQ070Y3LW01_WaitForVerticalSync,
};

// List of interfaces
const T_uezDeviceInterface *LCD_SHARP_LQ070Y3LW01_InterfaceArray[] = {
    0,      // 1 bit
    0,      // 2 bit
    0,      // 4 bit
    (T_uezDeviceInterface *)&LCD_SHARP_LQ070Y3LW01_Interface_8Bit,      // 8 bit
    (T_uezDeviceInterface *)&LCD_SHARP_LQ070Y3LW01_Interface_16Bit,      // 16 bit
    (T_uezDeviceInterface *)&LCD_SHARP_LQ070Y3LW01_Interface_I15Bit,      // I15 bit
    0,      // 24 bit
};

/*-------------------------------------------------------------------------*
 * End of File:  Sharp_LQ070Y3LW01.c
 *-------------------------------------------------------------------------*/
