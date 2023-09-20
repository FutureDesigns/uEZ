/*-------------------------------------------------------------------------*
 * File:  Innolux_AT070TN84.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the Innolux_AT070TN84.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Device/LCD.h>
#include <HAL/LCDController.h>
#include <HAL/GPIO.h>
#include <uEZSPI.h>
#include "uEZPlatform.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RESOLUTION_X        800
#define RESOLUTION_Y        480

#ifndef LCD_Innolux_AT070TN84_FLIP_SCREEN_X
#define LCD_Innolux_AT070TN84_FLIP_SCREEN_X       0
#endif

#ifndef LCD_Innolux_AT070TN84_FLIP_SCREEN_Y
#define LCD_Innolux_AT070TN84_FLIP_SCREEN_Y       0
#endif

#ifndef Innolux_AT070TN84_DOTCLOCK_HZ
    #define     Innolux_AT070TN84_DOTCLOCK_HZ       40000000
#endif

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
    T_uezDevice iSPI;
    HAL_GPIOPort **iCSGPIOPort;
    TUInt32 iCSGPIOBit;
    HAL_GPIOPort **iResetGPIOPort;
    TUInt32 iResetGPIOBit;
    T_uezSemaphore iVSyncSem;
} T_AT070TN84Workspace;

typedef struct {
    TUInt8 iReg;
        #define REG_END         0xFF
    TUInt16 iData;
} T_lcdSPICmd;


/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LCDControllerSettings LCD_AT070TN84_settings;

static const T_LCDControllerSettings LCD_AT070TN84_params16bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_565,

    88,         /* Horizontal back porch */
    40,         /* Horizontal front porch */
    48,          /* HSYNC pulse width */
    800,        /* Pixels per line */

    32,          /* Vertical back porch */
    13,          /* Vertical front porch */
    3,         /* VSYNC pulse width */
    480,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    EFalse,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address
    Innolux_AT070TN84_DOTCLOCK_HZ,
};

static const T_LCDControllerSettings LCD_AT070TN84_paramsI15bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_I555,

    210,         /* Horizontal back porch */
    40,         /* Horizontal front porch */
    1,          /* HSYNC pulse width */
    800,        /* Pixels per line */

    22,          /* Vertical back porch */
    132,          /* Vertical front porch */
    1,         /* VSYNC pulse width */
    480,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    EFalse,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address
    Innolux_AT070TN84_DOTCLOCK_HZ,
};

static const T_LCDControllerSettings LCD_AT070TN84_params8bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_8,

    88,         /* Horizontal back porch */
    40,         /* Horizontal front porch */
    48,          /* HSYNC pulse width */
    800,        /* Pixels per line */

    32,          /* Vertical back porch */
    13,          /* Vertical front porch */
    3,         /* VSYNC pulse width */
    480,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    EFalse,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address
    Innolux_AT070TN84_DOTCLOCK_HZ,
};

static T_uezLCDConfiguration LCD_AT070TN84_configuration_16Bit = {
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

static T_uezLCDConfiguration LCD_AT070TN84_configuration_I15Bit = {
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

static T_uezLCDConfiguration LCD_AT070TN84_configuration_8Bit = {
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
#if 0
static const T_lcdSPICmd G_lcdStartup[] = {
#if LCD_Innolux_AT070TN84_FLIP_SCREEN_X
    #if LCD_Innolux_AT070TN84_FLIP_SCREEN_Y
        { 0x01, 0x610F },
    #else
        { 0x01, 0x630F },
    #endif
#else
    #if LCD_Innolux_AT070TN84_FLIP_SCREEN_Y
        { 0x01, 0x210F },
    #else
        { 0x01, 0x230F },
    #endif
#endif
    { 0x02, 0x0C02 },
    { 0x03, 0x040E },
    { 0x0B, 0xD000 },
    { 0x0C, 0x0005 },
    { 0x0D, 0x000F },
    { 0x0E, 0x2B00 },
    { 0x16, 0xEF8E },
    { 0x17, 0x0003 },
    { 0x1E, 0x0000 },
    { 0x30, 0x0000 },
    { 0x31, 0x0107 },
    { 0x32, 0x0000 },
    { 0x33, 0x0201 },
    { 0x34, 0x0607 },
    { 0x35, 0x0005 },
    { 0x36, 0x0707 },
    { 0x37, 0x0203 },
    { 0x3A, 0x0F0F },
    { 0x3B, 0x0F02 },
    { 0x10, 0x02CC },
    { 0x26, 0x2800 },
    { 0x15, 0x0090 },
    { 0x2C, 0x3BBD },
    { REG_END, 0 }
};
#endif
#if 0
static const T_lcdSPICmd G_lcdExitSleep[] = {
    { 0x28, 0x0006 },
    { 0x2D, 0x3F44 },
    { 0x29, 0xFFFE },
    { REG_END, 0 }
};
#endif
#if 0
static const T_lcdSPICmd G_lcdEnterSleep[] = {
    { 0x28, 0x0006 },
    { 0x29, 0x8000 },
    { 0x2E, 0xB544 },
    { 0x2D, 0x3F46 },
    { REG_END, 0 }
};
#endif
/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_LCD LCD_Innolux_AT070TN84_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_InitializeWorkspace_16Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for AT070TN84 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_AT070TN84_InitializeWorkspace_16Bit(void *aW)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_AT070TN84_configuration_16Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_InitializeWorkspace_I15Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for AT070TN84 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_AT070TN84_InitializeWorkspace_I15Bit(void *aW)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_AT070TN84_configuration_I15Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_InitializeWorkspace_8Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for AT070TN84 LCD.
 * Inputs:
 *      void *aW                    -- Particular  workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_AT070TN84_InitializeWorkspace_8Bit(void *aW)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_AT070TN84_configuration_8Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}
/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_Open
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
static T_uezError LCD_AT070TN84_Open(void *aW)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    HAL_LCDController **plcdc;
    T_uezError error = UEZ_ERROR_NONE;

    p->aNumOpen++;

    if (p->aNumOpen == 1) {
        plcdc = p->iLCDController;
        if (!error) {
            switch (p->iConfiguration->iColorDepth) {
                case UEZLCD_COLOR_DEPTH_8_BIT:
                    LCD_AT070TN84_settings = LCD_AT070TN84_params8bit;
                    break;
                default:
                case UEZLCD_COLOR_DEPTH_16_BIT:
                    LCD_AT070TN84_settings = LCD_AT070TN84_params16bit;
                    break;
                case UEZLCD_COLOR_DEPTH_I15_BIT:
                    LCD_AT070TN84_settings = LCD_AT070TN84_paramsI15bit;
                    break;
            }
            LCD_AT070TN84_settings.iBaseAddress = p->iBaseAddress;
            error = (*plcdc)->Configure(plcdc, &LCD_AT070TN84_settings);

            if (!error) {
//                for (i=0; i<480*272*2; i+=2) {
//                    *((TUInt16 *)(p->iBaseAddress+i)) = 0x0000; // black
//                }
                (*p->iLCDController)->On(p->iLCDController);

            }
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      End access to the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_AT070TN84_Close(void *aW)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD to use a particular LCD controller.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully configured,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_AT070TN84_Configure(
            void *aW,
            HAL_LCDController **aLCDController,
            TUInt32 aBaseAddress,
            DEVICE_Backlight **aBacklight)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    T_uezError error = UEZ_ERROR_NONE;

    p->iLCDController = aLCDController;
    p->iBaseAddress = aBaseAddress;
    p->iBacklight = aBacklight;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the LCD device.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_AT070TN84_GetInfo(void *aW, T_uezLCDConfiguration *aConfiguration)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;

    // Use the setting based on color depth
    *aConfiguration = *p->iConfiguration;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_GetFrame
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
static T_uezError LCD_AT070TN84_GetFrame(
            void *aW,
            TUInt32 aFrame,
            void **aFrameBuffer)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    *aFrameBuffer = (void *)(p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_ShowFrame
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
static T_uezError LCD_AT070TN84_ShowFrame(void *aW, TUInt32 aFrame)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    (*p->iLCDController)->SetBaseAddr(
            p->iLCDController,
            p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2,
            EFalse);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_AT070TN84_On(void *aW)
{
    // Turn back on to the remembered level
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    (*p->iLCDController)->On(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->On(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns off the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_AT070TN84_Off(void *aW)
{
    // Turn off, but don't remember the level
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
    (*p->iLCDController)->Off(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->Off(p->iBacklight);


    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_SetBacklightLevel
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
static T_uezError LCD_AT070TN84_SetBacklightLevel(void *aW, TUInt32 aLevel)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;
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
 * Routine:  LCD_AT070TN84_GetBacklightLevel
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
static T_uezError LCD_AT070TN84_GetBacklightLevel(
        void *aW, 
        TUInt32 *aLevel,
        TUInt32 *aNumLevels)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aW;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (aNumLevels)
        *aNumLevels = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    *aLevel = p->iBacklightLevel;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_SetPaletteColor
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
static T_uezError LCD_AT070TN84_SetPaletteColor(
                    void *aWorkspace,
                    TUInt32 aColorIndex,
                    TUInt16 aRed,
                    TUInt16 aGreen,
                    TUInt16 aBlue)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aWorkspace;

    return (*p->iLCDController)->SetPaletteColor(
                p->iLCDController,
                aColorIndex,
                aRed,
                aGreen,
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_VerticalSyncCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      This routine is called from within an interrupt when the vertical
 *      sync occurs.  This routines releases any waiting callers on
 *      the semaphore.
 * Inputs:
 *      void *aCallbackWorkspace -- LCD workspace
 *---------------------------------------------------------------------------*/
static void LCD_AT070TN84_VerticalSyncCallback(void *aCallbackWorkspace)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aCallbackWorkspace;

    _isr_UEZSemaphoreRelease(p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_AT070TN84_WaitForVerticalSync
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
static T_uezError LCD_AT070TN84_WaitForVerticalSync(
    void *aWorkspace,
    TUInt32 aTimeout)
{
    T_AT070TN84Workspace *p = (T_AT070TN84Workspace *)aWorkspace;
    HAL_LCDController **p_hal = p->iLCDController;

    UEZSemaphoreGrab(p->iVSyncSem, 0);
    (*p_hal)->EnableVerticalSync(p_hal, LCD_AT070TN84_VerticalSyncCallback, p);
    return UEZSemaphoreGrab(p->iVSyncSem, aTimeout);
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LCD LCD_Innolux_AT070TN84_Interface_16Bit = {
	{
	    // Common interface
	    "LCD:Innolux_AT070TN84:16Bit",
	    0x0100,
	    LCD_AT070TN84_InitializeWorkspace_16Bit,
	    sizeof(T_AT070TN84Workspace),
	},
	
    // Functions
    LCD_AT070TN84_Open,
    LCD_AT070TN84_Close,
    LCD_AT070TN84_Configure,
    LCD_AT070TN84_GetInfo,
    LCD_AT070TN84_GetFrame,
    LCD_AT070TN84_ShowFrame,
    LCD_AT070TN84_On,
    LCD_AT070TN84_Off,
    LCD_AT070TN84_SetBacklightLevel,
    LCD_AT070TN84_GetBacklightLevel,
    LCD_AT070TN84_SetPaletteColor,

    // v2.04
    LCD_AT070TN84_WaitForVerticalSync,
};

const DEVICE_LCD LCD_Innolux_AT070TN84_Interface_I15Bit = {
	{
	    // Common interface
	    "LCD:Innolux_AT070TN84:I15Bit",
	    0x0100,
	    LCD_AT070TN84_InitializeWorkspace_I15Bit,
	    sizeof(T_AT070TN84Workspace),
	},
	
    // Functions
    LCD_AT070TN84_Open,
    LCD_AT070TN84_Close,
    LCD_AT070TN84_Configure,
    LCD_AT070TN84_GetInfo,
    LCD_AT070TN84_GetFrame,
    LCD_AT070TN84_ShowFrame,
    LCD_AT070TN84_On,
    LCD_AT070TN84_Off,
    LCD_AT070TN84_SetBacklightLevel,
    LCD_AT070TN84_GetBacklightLevel,
    LCD_AT070TN84_SetPaletteColor,

    // v2.04
    LCD_AT070TN84_WaitForVerticalSync,
};

const DEVICE_LCD LCD_Innolux_AT070TN84_Interface_8Bit = {
	{
	    // Common interface
	    "LCD:Innolux_AT070TN84:8Bit",
	    0x0100,
	    LCD_AT070TN84_InitializeWorkspace_8Bit,
	    sizeof(T_AT070TN84Workspace),
	},
	
    // Functions
    LCD_AT070TN84_Open,
    LCD_AT070TN84_Close,
    LCD_AT070TN84_Configure,
    LCD_AT070TN84_GetInfo,
    LCD_AT070TN84_GetFrame,
    LCD_AT070TN84_ShowFrame,
    LCD_AT070TN84_On,
    LCD_AT070TN84_Off,
    LCD_AT070TN84_SetBacklightLevel,
    LCD_AT070TN84_GetBacklightLevel,
    LCD_AT070TN84_SetPaletteColor,

    // v2.04
    LCD_AT070TN84_WaitForVerticalSync,
};

// List of interfaces
const T_uezDeviceInterface *LCD_Innolux_AT070TN84_InterfaceArray[] = {
    0,      // 1 bit
    0,      // 2 bit
    0,      // 4 bit
    (T_uezDeviceInterface *)&LCD_Innolux_AT070TN84_Interface_8Bit,      // 8 bit
    (T_uezDeviceInterface *)&LCD_Innolux_AT070TN84_Interface_16Bit,      // 16 bit
    (T_uezDeviceInterface *)&LCD_Innolux_AT070TN84_Interface_I15Bit,      // I15 bit
    0,      // 24 bit
};

/*-------------------------------------------------------------------------*
 * End of File:  Innolux_AT070TN84.c
 *-------------------------------------------------------------------------*/
