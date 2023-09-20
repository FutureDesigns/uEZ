/*-------------------------------------------------------------------------*
 * File:  Tianma_TM043NBH02.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the Tianma_TM043NBH02.
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
#include <Device/LCD.h>
#include <HAL/LCDController.h>
#include <HAL/GPIO.h>
#include "uEZPlatform.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RESOLUTION_X        480
#define RESOLUTION_Y        272

#define LCD_CLOCK_RATE      9000000


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
} T_TM043NBH02Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LCDControllerSettings LCD_TM043NBH02_settings;

static const T_LCDControllerSettings LCD_TM043NBH02_params16bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_565,

    2,         /* Horizontal back porch */
    2,         /* Horizontal front porch */
    41,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    2,          /* Vertical back porch */
    2,          /* Vertical front porch */
    10,         /* VSYNC pulse width */
    272,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    ETrue,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address
    LCD_CLOCK_RATE,
};

static const T_LCDControllerSettings LCD_TM043NBH02_paramsI15bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_I555,

    2,         /* Horizontal back porch */
    2,         /* Horizontal front porch */
    41,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    2,          /* Vertical back porch */
    2,          /* Vertical front porch */
    10,         /* VSYNC pulse width */
    272,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    ETrue,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address
    LCD_CLOCK_RATE,
};

static const T_LCDControllerSettings LCD_TM043NBH02_params8bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_8,

    2,         /* Horizontal back porch */
    2,         /* Horizontal front porch */
    41,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    2,          /* Vertical back porch */
    2,          /* Vertical front porch */
    10,         /* VSYNC pulse width */
    272,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    ETrue,      /* Invert panel clock */
    ETrue,      /* Invert HSYNC */
    ETrue,      /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address
    LCD_CLOCK_RATE,
};

static T_uezLCDConfiguration LCD_TM043NBH02_configuration_16Bit = {
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

static T_uezLCDConfiguration LCD_TM043NBH02_configuration_I15Bit = {
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

static T_uezLCDConfiguration LCD_TM043NBH02_configuration_8Bit = {
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
extern const DEVICE_LCD LCD_Tianma_TM043NBH02_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_InitializeWorkspace_16Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for TM043NBH02 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_TM043NBH02_InitializeWorkspace_16Bit(void *aW)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_TM043NBH02_configuration_16Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_InitializeWorkspace_I15Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for TM043NBH02 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_TM043NBH02_InitializeWorkspace_I15Bit(void *aW)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_TM043NBH02_configuration_I15Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_InitializeWorkspace_8Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for TM043NBH02 LCD.
 * Inputs:
 *      void *aW                    -- Particular  workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_TM043NBH02_InitializeWorkspace_8Bit(void *aW)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_TM043NBH02_configuration_8Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_Open
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
static T_uezError LCD_TM043NBH02_Open(void *aW)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    HAL_LCDController **plcdc;
    T_uezError error = UEZ_ERROR_NONE;

    p->aNumOpen++;

    if (p->aNumOpen == 1) {
        plcdc = p->iLCDController;
        if (!error) {
            switch (p->iConfiguration->iColorDepth) {
                case UEZLCD_COLOR_DEPTH_8_BIT:
                    LCD_TM043NBH02_settings = LCD_TM043NBH02_params8bit;
                    break;
                default:
                case UEZLCD_COLOR_DEPTH_16_BIT:
                    LCD_TM043NBH02_settings = LCD_TM043NBH02_params16bit;
                    break;
                case UEZLCD_COLOR_DEPTH_I15_BIT:
                    LCD_TM043NBH02_settings = LCD_TM043NBH02_paramsI15bit;
                    break;
            }
            LCD_TM043NBH02_settings.iBaseAddress = p->iBaseAddress;
            error = (*plcdc)->Configure(plcdc, &LCD_TM043NBH02_settings);

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
 * Routine:  LCD_TM043NBH02_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      End access to the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_TM043NBH02_Close(void *aW)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD to use a particular LCD controller.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully configured,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_TM043NBH02_Configure(
            void *aW,
            HAL_LCDController **aLCDController,
            TUInt32 aBaseAddress,
            DEVICE_Backlight **aBacklight)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    T_uezError error = UEZ_ERROR_NONE;

    p->iLCDController = aLCDController;
    p->iBaseAddress = aBaseAddress;
    p->iBacklight = aBacklight;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the LCD device.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_TM043NBH02_GetInfo(void *aW, T_uezLCDConfiguration *aConfiguration)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;

    // Use the setting based on color depth
    *aConfiguration = *p->iConfiguration;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_GetFrame
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
static T_uezError LCD_TM043NBH02_GetFrame(
            void *aW,
            TUInt32 aFrame,
            void **aFrameBuffer)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    *aFrameBuffer = (void *)(p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_ShowFrame
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
static T_uezError LCD_TM043NBH02_ShowFrame(void *aW, TUInt32 aFrame)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    (*p->iLCDController)->SetBaseAddr(
            p->iLCDController,
            p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2,
            EFalse);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_TM043NBH02_On(void *aW)
{
    // Turn back on to the remembered level
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    (*p->iLCDController)->On(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->On(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns off the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_TM043NBH02_Off(void *aW)
{
    // Turn off, but don't remember the level
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
    (*p->iLCDController)->Off(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->Off(p->iBacklight);


    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_SetBacklightLevel
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
static T_uezError LCD_TM043NBH02_SetBacklightLevel(void *aW, TUInt32 aLevel)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;
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
 * Routine:  LCD_TM043NBH02_GetBacklightLevel
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
static T_uezError LCD_TM043NBH02_GetBacklightLevel(
        void *aW, 
        TUInt32 *aLevel,
        TUInt32 *aNumLevels)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aW;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (aNumLevels)
        *aNumLevels = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    *aLevel = p->iBacklightLevel;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_SetPaletteColor
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
static T_uezError LCD_TM043NBH02_SetPaletteColor(
                    void *aWorkspace,
                    TUInt32 aColorIndex,
                    TUInt16 aRed,
                    TUInt16 aGreen,
                    TUInt16 aBlue)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aWorkspace;

    return (*p->iLCDController)->SetPaletteColor(
                p->iLCDController,
                aColorIndex,
                aRed,
                aGreen,
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_VerticalSyncCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      This routine is called from within an interrupt when the vertical
 *      sync occurs.  This routines releases any waiting callers on
 *      the semaphore.
 * Inputs:
 *      void *aCallbackWorkspace -- LCD workspace
 *---------------------------------------------------------------------------*/
static void LCD_TM043NBH02_VerticalSyncCallback(void *aCallbackWorkspace)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aCallbackWorkspace;

    _isr_UEZSemaphoreRelease(p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_TM043NBH02_WaitForVerticalSync
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
static T_uezError LCD_TM043NBH02_WaitForVerticalSync(
    void *aWorkspace,
    TUInt32 aTimeout)
{
    T_TM043NBH02Workspace *p = (T_TM043NBH02Workspace *)aWorkspace;
    HAL_LCDController **p_hal = p->iLCDController;

    UEZSemaphoreGrab(p->iVSyncSem, 0);
    (*p_hal)->EnableVerticalSync(p_hal, LCD_TM043NBH02_VerticalSyncCallback, p);
    return UEZSemaphoreGrab(p->iVSyncSem, aTimeout);
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LCD LCD_Tianma_TM043NBH02_Interface_16Bit = {
	{
	    // Common interface
	    "LCD:Tianma_TM043NBH02:16Bit",
	    0x0100,
	    LCD_TM043NBH02_InitializeWorkspace_16Bit,
	    sizeof(T_TM043NBH02Workspace),
	},
	
    // Functions
    LCD_TM043NBH02_Open,
    LCD_TM043NBH02_Close,
    LCD_TM043NBH02_Configure,
    LCD_TM043NBH02_GetInfo,
    LCD_TM043NBH02_GetFrame,
    LCD_TM043NBH02_ShowFrame,
    LCD_TM043NBH02_On,
    LCD_TM043NBH02_Off,
    LCD_TM043NBH02_SetBacklightLevel,
    LCD_TM043NBH02_GetBacklightLevel,
    LCD_TM043NBH02_SetPaletteColor,
    // v2.04
    LCD_TM043NBH02_WaitForVerticalSync,
};

const DEVICE_LCD LCD_Tianma_TM043NBH02_Interface_I15Bit = {
	{
	    // Common interface
	    "LCD:Tianma_TM043NBH02:I15Bit",
	    0x0100,
	    LCD_TM043NBH02_InitializeWorkspace_I15Bit,
	    sizeof(T_TM043NBH02Workspace),
	},
	
    // Functions
    LCD_TM043NBH02_Open,
    LCD_TM043NBH02_Close,
    LCD_TM043NBH02_Configure,
    LCD_TM043NBH02_GetInfo,
    LCD_TM043NBH02_GetFrame,
    LCD_TM043NBH02_ShowFrame,
    LCD_TM043NBH02_On,
    LCD_TM043NBH02_Off,
    LCD_TM043NBH02_SetBacklightLevel,
    LCD_TM043NBH02_GetBacklightLevel,
    LCD_TM043NBH02_SetPaletteColor,
    // v2.04
    LCD_TM043NBH02_WaitForVerticalSync,
};

const DEVICE_LCD LCD_Tianma_TM043NBH02_Interface_8Bit = {
	{
	    // Common interface
	    "LCD:Tianma_TM043NBH02:8Bit",
	    0x0100,
	    LCD_TM043NBH02_InitializeWorkspace_8Bit,
	    sizeof(T_TM043NBH02Workspace),
	},
	
    // Functions
    LCD_TM043NBH02_Open,
    LCD_TM043NBH02_Close,
    LCD_TM043NBH02_Configure,
    LCD_TM043NBH02_GetInfo,
    LCD_TM043NBH02_GetFrame,
    LCD_TM043NBH02_ShowFrame,
    LCD_TM043NBH02_On,
    LCD_TM043NBH02_Off,
    LCD_TM043NBH02_SetBacklightLevel,
    LCD_TM043NBH02_GetBacklightLevel,
    LCD_TM043NBH02_SetPaletteColor,
    // v2.04
    LCD_TM043NBH02_WaitForVerticalSync,
};

// List of interfaces
const T_uezDeviceInterface *LCD_Tianma_TM043NBH02_InterfaceArray[] = {
    0,      // 1 bit
    0,      // 2 bit
    0,      // 4 bit
    (T_uezDeviceInterface *)&LCD_Tianma_TM043NBH02_Interface_8Bit,      // 8 bit
    (T_uezDeviceInterface *)&LCD_Tianma_TM043NBH02_Interface_16Bit,      // 16 bit
    (T_uezDeviceInterface *)&LCD_Tianma_TM043NBH02_Interface_I15Bit,      // I15 bit
    0,      // 24 bit
};

/*-------------------------------------------------------------------------*
 * End of File:  Tianma_TM043NBH02.c
 *-------------------------------------------------------------------------*/
