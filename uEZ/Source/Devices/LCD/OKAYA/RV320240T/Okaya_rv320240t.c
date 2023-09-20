/*-------------------------------------------------------------------------*
 * File:  lcdDriver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the OKAYA RV320240T-5X7WN-B5 LCD.
 *      This LCD can run in 2 different modes:  8-bit (palletized)
 *      or 16-bit color.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <HAL/LCDController.h>
#include <Device/Backlight.h>
#include "Okaya_RV320240T.h"
#include "uEZPlatform.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RESOLUTION_X        320
#define RESOLUTION_Y        240

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_LCD *iHAL;
    TUInt32 iBaseAddress;
    int32_t aNumOpen;
    TUInt32 iBacklightLevel;
    HAL_LCDController **iLCDController;
    DEVICE_Backlight **iBacklight;
    const T_uezLCDConfiguration *iConfiguration;
	T_uezSemaphore iVSyncSem;
} T_RV320240TWorkspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LCDControllerSettings LCD_RV320240T_settings;

static const T_LCDControllerSettings LCD_RV320240T_params16bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_565,

    36,         /* Horizontal back porch */
    12,         /* Horizontal front porch */
    30,         /* HSYNC pulse width */
    320,        /* Pixels per line */

    26,         /* Vertical back porch */
    18,         /* Vertical front porch */
    4,          /* VSYNC pulse width */
    240,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    ETrue,      /* Invert panel clock */
    EFalse,     /* Invert HSYNC */
    EFalse,     /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address

    8000000,    // 8.0 MHz is optimal (12 ns min per dot = 8.333 MHz)
};

static T_LCDControllerSettings LCD_RV320240T_paramsI15bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_I555,

    36,         /* Horizontal back porch */
    12,         /* Horizontal front porch */
    30,         /* HSYNC pulse width */
    320,        /* Pixels per line */

    26,         /* Vertical back porch */
    18,         /* Vertical front porch */
    4,          /* VSYNC pulse width */
    240,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    ETrue,      /* Invert panel clock */
    EFalse,     /* Invert HSYNC */
    EFalse,     /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address

    6300000,    // 6.4 MHz is optimal
};

static const T_LCDControllerSettings LCD_RV320240T_params8bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_8,

    36,         /* Horizontal back porch */
    12,         /* Horizontal front porch */
    30,         /* HSYNC pulse width */
    320,        /* Pixels per line */

    26,         /* Vertical back porch */
    18,         /* Vertical front porch */
    4,          /* VSYNC pulse width */
    240,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    EFalse,     /* Invert panel clock */
    EFalse,     /* Invert HSYNC */
    EFalse,     /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address

    6300000,    // 8.0 MHz is optimal (12 ns min per dot = 8.333 MHz)
};

static const T_uezLCDConfiguration LCD_RV320240T_configuration_16Bit = {
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

static T_uezLCDConfiguration LCD_RV320240T_configuration_I15Bit = {
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

static const T_uezLCDConfiguration LCD_RV320240T_configuration_8Bit = {
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
extern const DEVICE_LCD LCD_OKAYA_RV320240T_Interface_16Bit;
extern const DEVICE_LCD LCD_OKAYA_RV320240T_Interface_I15Bit;
extern const DEVICE_LCD LCD_OKAYA_RV320240T_Interface_8Bit;

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_InitializeWorkspace_16Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for RV320240T LCD (16 bit mode).
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV320240T_InitializeWorkspace_16Bit(void *aW)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    p->iBaseAddress = 0x80000000;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_RV320240T_configuration_16Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_InitializeWorkspace_I15Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for RV320240T LCD (16 bit mode).
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV320240T_InitializeWorkspace_I15Bit(void *aW)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    p->iBaseAddress = 0x80000000;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_RV320240T_configuration_I15Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_InitializeWorkspace_8Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for RV320240T LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV320240T_InitializeWorkspace_8Bit(void *aW)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    p->iBaseAddress = 0x80000000;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_RV320240T_configuration_8Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_Open
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
T_uezError LCD_RV320240T_Open(void *aW)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    HAL_LCDController **plcdc;
    p->aNumOpen++;
    if (p->aNumOpen == 1) {
        plcdc = p->iLCDController;

        // Choose a settings based on the configuration
        switch (p->iConfiguration->iColorDepth) {
            case UEZLCD_COLOR_DEPTH_8_BIT:
                LCD_RV320240T_settings = LCD_RV320240T_params8bit;
                break;
            default:
            case UEZLCD_COLOR_DEPTH_16_BIT:
                LCD_RV320240T_settings = LCD_RV320240T_params16bit;
                break;
            case UEZLCD_COLOR_DEPTH_I15_BIT:
                LCD_RV320240T_settings = LCD_RV320240T_paramsI15bit;
                break;
        }

        LCD_RV320240T_settings.iBaseAddress = p->iBaseAddress;
        return (*plcdc)->Configure(plcdc, &LCD_RV320240T_settings);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      End access to the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV320240T_Close(void *aW)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD to use a particular LCD controller.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully configured,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV320240T_Configure(
            void *aW,
            HAL_LCDController **aLCDController,
            TUInt32 aBaseAddress,
            DEVICE_Backlight **aBacklight)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    p->iLCDController = aLCDController;
    p->iBaseAddress = aBaseAddress;

    p->iBacklight = aBacklight;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the LCD device.
 * Inputs:
 *      void *aW                -- Workspace
 *      T_uezLCDConfiguration *aConfiguration -- Configuration settings
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV320240T_GetInfo(
                void *aW,
                T_uezLCDConfiguration *aConfiguration)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;

    // Use the setting based on color depth
    *aConfiguration = *p->iConfiguration;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_GetFrame
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
T_uezError LCD_RV320240T_GetFrame(
            void *aW,
            TUInt32 aFrame,
            void **aFrameBuffer)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    *aFrameBuffer = (void *)(p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_ShowFrame
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
T_uezError LCD_RV320240T_ShowFrame(void *aW, TUInt32 aFrame)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    (*p->iLCDController)->SetBaseAddr(
            p->iLCDController,
            p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2,
            EFalse);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV320240T_On(void *aW)
{
    // Turn back on to the remembered level
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    (*p->iLCDController)->On(p->iLCDController);
    (*p->iBacklight)->On(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns off the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV320240T_Off(void *aW)
{
    // Turn off, but don't remember the level
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;
    (*p->iLCDController)->Off(p->iLCDController);
    (*p->iBacklight)->Off(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_SetBacklightLevel
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
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV320240T_SetBacklightLevel(void *aW, TUInt32 aLevel)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;

    // Limit the backlight level
    if (aLevel > p->iConfiguration->iNumBacklightLevels)
        aLevel = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    p->iBacklightLevel = aLevel;

    return (*p->iBacklight)->SetRatio(p->iBacklight,
        (aLevel * 0xFFFF) / (p->iConfiguration->iNumBacklightLevels));
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_GetBacklightLevel
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
static T_uezError LCD_RV320240T_GetBacklightLevel(
        void *aW, 
        TUInt32 *aLevel,
        TUInt32 *aNumLevels)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aW;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (aNumLevels)
        *aNumLevels = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    *aLevel = p->iBacklightLevel;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_SetPaletteColor
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
T_uezError LCD_RV320240T_SetPaletteColor(
                    void *aWorkspace,
                    TUInt32 aColorIndex,
                    TUInt16 aRed,
                    TUInt16 aGreen,
                    TUInt16 aBlue)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aWorkspace;

    return (*p->iLCDController)->SetPaletteColor(
                p->iLCDController,
                aColorIndex,
                aRed,
                aGreen,
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_VerticalSyncCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      This routine is called from within an interrupt when the vertical
 *      sync occurs.  This routines releases any waiting callers on
 *      the semaphore.
 * Inputs:
 *      void *aCallbackWorkspace -- LCD workspace
 *---------------------------------------------------------------------------*/
static void LCD_RV320240T_VerticalSyncCallback(void *aCallbackWorkspace)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aCallbackWorkspace;

    _isr_UEZSemaphoreRelease(p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV320240T_WaitForVerticalSync
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
static T_uezError LCD_RV320240T_WaitForVerticalSync(
    void *aWorkspace,
    TUInt32 aTimeout)
{
    T_RV320240TWorkspace *p = (T_RV320240TWorkspace *)aWorkspace;
    HAL_LCDController **p_hal = p->iLCDController;

    UEZSemaphoreGrab(p->iVSyncSem, 0);
    (*p_hal)->EnableVerticalSync(p_hal, LCD_RV320240T_VerticalSyncCallback, p);
    return UEZSemaphoreGrab(p->iVSyncSem, aTimeout);
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LCD LCD_OKAYA_RV320240T_Interface_16Bit = {
	{
	    // Common interface
	    "LCD:OKAYA_RV320240T:16Bit",
	    0x0100,
	    LCD_RV320240T_InitializeWorkspace_16Bit,
	    sizeof(T_RV320240TWorkspace),
	},
	
    // Functions
    LCD_RV320240T_Open,
    LCD_RV320240T_Close,
    LCD_RV320240T_Configure,
    LCD_RV320240T_GetInfo,
    LCD_RV320240T_GetFrame,
    LCD_RV320240T_ShowFrame,
    LCD_RV320240T_On,
    LCD_RV320240T_Off,
    LCD_RV320240T_SetBacklightLevel,
    LCD_RV320240T_GetBacklightLevel,
    LCD_RV320240T_SetPaletteColor,
    // v2.04
    LCD_RV320240T_WaitForVerticalSync,
};

const DEVICE_LCD LCD_OKAYA_RV320240T_Interface_I15Bit = {
	{
	    // Common interface
	    "LCD:OKAYA_RV320240T:I15Bit",
	    0x0100,
	    LCD_RV320240T_InitializeWorkspace_I15Bit,
	    sizeof(T_RV320240TWorkspace),
	},
	
    // Functions
    LCD_RV320240T_Open,
    LCD_RV320240T_Close,
    LCD_RV320240T_Configure,
    LCD_RV320240T_GetInfo,
    LCD_RV320240T_GetFrame,
    LCD_RV320240T_ShowFrame,
    LCD_RV320240T_On,
    LCD_RV320240T_Off,
    LCD_RV320240T_SetBacklightLevel,
    LCD_RV320240T_GetBacklightLevel,
    LCD_RV320240T_SetPaletteColor,
    // v2.04
    LCD_RV320240T_WaitForVerticalSync,
};

const DEVICE_LCD LCD_OKAYA_RV320240T_Interface_8Bit = {
	{
	    // Common interface
	    "LCD:OKAYA_RV320240T:8Bit",
	    0x0100,
	    LCD_RV320240T_InitializeWorkspace_8Bit,
	    sizeof(T_RV320240TWorkspace),
	},
	
    // Functions
    LCD_RV320240T_Open,
    LCD_RV320240T_Close,
    LCD_RV320240T_Configure,
    LCD_RV320240T_GetInfo,
    LCD_RV320240T_GetFrame,
    LCD_RV320240T_ShowFrame,
    LCD_RV320240T_On,
    LCD_RV320240T_Off,
    LCD_RV320240T_SetBacklightLevel,
    LCD_RV320240T_GetBacklightLevel,
    LCD_RV320240T_SetPaletteColor,
    // v2.04
    LCD_RV320240T_WaitForVerticalSync,
};

// List of interfaces
const T_uezDeviceInterface *LCD_OKAYA_RV320240T_InterfaceArray[] = {
    0,      // 1 bit
    0,      // 2 bit
    0,      // 4 bit
    (T_uezDeviceInterface *)&LCD_OKAYA_RV320240T_Interface_8Bit,      // 8 bit
    (T_uezDeviceInterface *)&LCD_OKAYA_RV320240T_Interface_16Bit,      // 16 bit
    (T_uezDeviceInterface *)&LCD_OKAYA_RV320240T_Interface_I15Bit,      // I15 bit
    0,      // 24 bit
};

/*-------------------------------------------------------------------------*
 * End of File:  lcdDriver.c
 *-------------------------------------------------------------------------*/
