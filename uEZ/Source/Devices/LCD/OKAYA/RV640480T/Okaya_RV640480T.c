/*-------------------------------------------------------------------------*
 * File:  Okaya_RV640480T.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the Okaya RV640480T-5X7WP-A5.
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
#include <HAL/GPIO.h>
#include <uEZDeviceTable.h>
#include "Okaya_RV640480T.h"
#include <uEZGPIO.h>
#include <uEZTimer.h>
#include <uEZPlatformAPI.h>
#include "uEZPlatform.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RESOLUTION_X        640
#define RESOLUTION_Y        480
#define LCD_CLOCK_SPEED     25175000 // Hz

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_LCD *iHAL;
    TUInt32 iBaseAddress;
    TUInt32 aNumOpen;
    TUInt32 iBacklightLevel;
    HAL_LCDController **iLCDController;
    DEVICE_Backlight **iBacklight;
    const T_uezLCDConfiguration *iConfiguration;
    T_uezSemaphore iVSyncSem;
    T_uezDevice itimer;
    T_uezTimerCallback icallback;
    volatile TBool itimerDone;
} T_RV640480TWorkspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LCDControllerSettings LCD_RV640480T_settings;

static T_LCDControllerSettings LCD_RV640480T_params16bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_565,

    10,         /* Horizontal back porch */
    120,          /* Horizontal front porch */
    30,         /* HSYNC pulse width */
    640,        /* Pixels per line */

    7,          /* Vertical back porch */
    35,          /* Vertical front porch */
    3,         /* VSYNC pulse width */
    480,        /* Lines per panel */

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

    LCD_CLOCK_SPEED,    // 25.175 MHz Typical value from datasheet
};

static T_LCDControllerSettings LCD_RV640480T_paramsI15bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_I555,

    10,         /* Horizontal back porch */
    120,          /* Horizontal front porch */
    30,         /* HSYNC pulse width */
    640,        /* Pixels per line */

    7,          /* Vertical back porch */
    35,          /* Vertical front porch */
    3,         /* VSYNC pulse width */
    480,        /* Lines per panel */

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
    LCD_CLOCK_SPEED,   // 25.175 MHz Typical value from datasheet
};

static const T_LCDControllerSettings LCD_RV640480T_params8bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_8,

    10,         /* Horizontal back porch */
    120,          /* Horizontal front porch */
    30,         /* HSYNC pulse width */
    640,        /* Pixels per line */

    7,          /* Vertical back porch */
    35,          /* Vertical front porch */
    3,         /* VSYNC pulse width */
    480,        /* Lines per panel */

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

    LCD_CLOCK_SPEED,    // 25.175 MHz Typical value from datasheet
};

static T_uezLCDConfiguration LCD_RV640480T_configuration_16Bit = {
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

static T_uezLCDConfiguration LCD_RV640480T_configuration_I15Bit = {
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

static T_uezLCDConfiguration LCD_RV640480T_configuration_8Bit = {
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
extern const DEVICE_LCD LCD_OKAYA_RV640480T_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_InitializeWorkspace_16Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for RV640480T LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV640480T_InitializeWorkspace_16Bit(void *aW)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 0; // 0%
    p->iConfiguration = &LCD_RV640480T_configuration_16Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_InitializeWorkspace_I15Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for RV640480T LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV640480T_InitializeWorkspace_I15Bit(void *aW)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 0; // 0%
    p->iConfiguration = &LCD_RV640480T_configuration_I15Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_InitializeWorkspace_8Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for RV640480T LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV640480T_InitializeWorkspace_8Bit(void *aW)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 0; // 0%
    p->iConfiguration = &LCD_RV640480T_configuration_8Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_SetBacklightLevel
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
T_uezError LCD_RV640480T_SetBacklightLevel(void *aW, TUInt32 aLevel)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
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
 * Routine:  LCD_RV640480T_MSTimerStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup MS timer in one function to avoid cluttering up the open function
 * Inputs:
*      T_uezTimerCallback *aCallbackWorkspace  -- Workspace 
*---------------------------------------------------------------------------*/
static T_uezError LCD_RV640480T_MSTimerStart(void *aW, float milliseconds){
  T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
  T_uezError error = UEZ_ERROR_NONE;
  p->itimerDone = EFalse; // set to true when timer finishes
  error = UEZTimerSetupOneShot(p->itimer,
                               1,
                               ((int32_t)milliseconds*(PROCESSOR_OSCILLATOR_FREQUENCY/1000)),
                               &p->icallback);
  if(error == UEZ_ERROR_NONE) {
    error = UEZTimerSetTimerMode(p->itimer, TIMER_MODE_CLOCK);
    error = UEZTimerReset(p->itimer);
    UEZTimerEnable(p->itimer);
  }  
  return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_TimerCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Timer callback to set pins immediately when the target time is reached.
 * Needed for LCDs with sub-ms and ms range accuracy for bring up sequencing.
 * Inputs:
 *      T_uezTimerCallback *aCallbackWorkspace  -- Workspace 
 *---------------------------------------------------------------------------*/
static void LCD_RV640480T_TimerCallback(T_uezTimerCallback *aCallbackWorkspace){
  T_RV640480TWorkspace *p = aCallbackWorkspace->iData;
  UEZTimerClose(p->itimer);    
  p->itimerDone = ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV640480T_On(void *aW)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
    (*p->iLCDController)->On(p->iLCDController);
    if (p->iBacklight){
      if (UEZTimerOpen("Timer0", &p->itimer) == UEZ_ERROR_NONE) {          
         LCD_RV640480T_MSTimerStart(p, 167.0); // no minimum specified but clear 10 frames just in case.
         while (p->itimerDone == EFalse){;} // wait for timer to finish, there will be a small task delay of a few hundred uS
         (*p->iBacklight)->On(p->iBacklight); // turn backlight on 
         LCD_RV640480T_SetBacklightLevel(p, p->iBacklightLevel);     // Turn back on to the remembered level
      }
    }
    return UEZ_ERROR_NONE;    
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns off the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV640480T_Off(void *aW)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;    
    
    if (p->iBacklight){
      (*p->iBacklight)->Off(p->iBacklight);     // Turn off, but don't remember the level
      if (UEZTimerOpen("Timer0", &p->itimer) == UEZ_ERROR_NONE) {
        LCD_RV640480T_MSTimerStart(p, 1.0); // no delay specified so make it 1ms in case a new data-sheet specifies timing
        while (p->itimerDone == EFalse){;} // wait for timer to finish, there will be a small task delay of a few hundred uS
      }   
    }    
    (*p->iLCDController)->Off(p->iLCDController);
    return UEZ_ERROR_NONE;    
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_Open
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
T_uezError LCD_RV640480T_Open(void *aW)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
    HAL_LCDController **plcdc;
    T_uezError error = UEZ_ERROR_NONE;
	TUInt32 i;
        
    p->icallback.iTimer = p->itimer; // Setup callback information for timer
    p->icallback.iMatchRegister = 1;
    p->icallback.iTriggerSem = 0;
    p->icallback.iCallback = LCD_RV640480T_TimerCallback;
    p->icallback.iData = p;
        
    p->aNumOpen++;
    if (p->aNumOpen == 1) {
        plcdc = p->iLCDController;

        switch (p->iConfiguration->iColorDepth) {
            case UEZLCD_COLOR_DEPTH_8_BIT:
                LCD_RV640480T_settings = LCD_RV640480T_params8bit;
                break;
            default:
            case UEZLCD_COLOR_DEPTH_16_BIT:
                LCD_RV640480T_settings = LCD_RV640480T_params16bit;
                break;
            case UEZLCD_COLOR_DEPTH_I15_BIT:
                LCD_RV640480T_settings = LCD_RV640480T_paramsI15bit;
                break;
        }
        LCD_RV640480T_settings.iBaseAddress = p->iBaseAddress;
        //pre-configure LCD controller first so that there is no delay for turn on  
        error = (*plcdc)->Configure(plcdc, &LCD_RV640480T_settings);        
        
        for (i=0; i<640*480*2; i+=2) {// black screen to clear any leftovers in memory
                *((TUInt16 *)(p->iBaseAddress+i)) = 0x0000; 
        }
        
        if (!error) {
        LCD_RV640480T_On(p); // start DOTCLK/HSYNC/VSYNC immediately
        }  
    }
  return error;    
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      End access to the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV640480T_Close(void *aW)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD to use a particular LCD controller.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully configured,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV640480T_Configure(
            void *aW,
            HAL_LCDController **aLCDController,
            TUInt32 aBaseAddress,
            DEVICE_Backlight **aBacklight)
{
	T_uezError error;
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
    p->iLCDController = aLCDController;
    p->iBaseAddress = aBaseAddress;
    error = UEZ_ERROR_NONE;

    p->iBacklight = aBacklight;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the LCD device.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RV640480T_GetInfo(void *aW, T_uezLCDConfiguration *aConfiguration)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;

    // Use the setting based on color depth
    *aConfiguration = *p->iConfiguration;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_GetFrame
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
T_uezError LCD_RV640480T_GetFrame(
            void *aW,
            TUInt32 aFrame,
            void **aFrameBuffer)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
    *aFrameBuffer = (void *)(p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_ShowFrame
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
T_uezError LCD_RV640480T_ShowFrame(void *aW, TUInt32 aFrame)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;
    (*p->iLCDController)->SetBaseAddr(
            p->iLCDController,
            p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2,
            EFalse);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_GetBacklightLevel
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
static T_uezError LCD_RV640480T_GetBacklightLevel(
        void *aW, 
        TUInt32 *aLevel,
        TUInt32 *aNumLevels)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aW;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (aNumLevels)
        *aNumLevels = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    *aLevel = p->iBacklightLevel;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_SetPaletteColor
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
T_uezError LCD_RV640480T_SetPaletteColor(
                    void *aWorkspace,
                    TUInt32 aColorIndex,
                    TUInt16 aRed,
                    TUInt16 aGreen,
                    TUInt16 aBlue)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aWorkspace;

    return (*p->iLCDController)->SetPaletteColor(
                p->iLCDController,
                aColorIndex,
                aRed,
                aGreen,
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_VerticalSyncCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      This routine is called from within an interrupt when the vertical
 *      sync occurs.  This routines releases any waiting callers on
 *      the semaphore.
 * Inputs:
 *      void *aCallbackWorkspace -- LCD workspace
 *---------------------------------------------------------------------------*/
static void LCD_RV640480T_VerticalSyncCallback(void *aCallbackWorkspace)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aCallbackWorkspace;

    _isr_UEZSemaphoreRelease(p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RV640480T_WaitForVerticalSync
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
static T_uezError LCD_RV640480T_WaitForVerticalSync(
    void *aWorkspace,
    TUInt32 aTimeout)
{
    T_RV640480TWorkspace *p = (T_RV640480TWorkspace *)aWorkspace;
    HAL_LCDController **p_hal = p->iLCDController;

    UEZSemaphoreGrab(p->iVSyncSem, 0);
    (*p_hal)->EnableVerticalSync(p_hal, LCD_RV640480T_VerticalSyncCallback, p);
    return UEZSemaphoreGrab(p->iVSyncSem, aTimeout);
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LCD LCD_OKAYA_RV640480T_Interface_16Bit = {
	{
	    // Common interface
	    "LCD:OKAYA_RV640480T:16Bit",
	    0x0100,
	    LCD_RV640480T_InitializeWorkspace_16Bit,
	    sizeof(T_RV640480TWorkspace),
	},
	
    // Functions
    LCD_RV640480T_Open,
    LCD_RV640480T_Close,
    LCD_RV640480T_Configure,
    LCD_RV640480T_GetInfo,
    LCD_RV640480T_GetFrame,
    LCD_RV640480T_ShowFrame,
    LCD_RV640480T_On,
    LCD_RV640480T_Off,
    LCD_RV640480T_SetBacklightLevel,
    LCD_RV640480T_GetBacklightLevel,
    LCD_RV640480T_SetPaletteColor,
    // v2.04
    LCD_RV640480T_WaitForVerticalSync,
};

const DEVICE_LCD LCD_OKAYA_RV640480T_Interface_I15Bit = {
	{
	    // Common interface
	    "LCD:OKAYA_RV640480T:I15Bit",
	    0x0100,
	    LCD_RV640480T_InitializeWorkspace_I15Bit,
	    sizeof(T_RV640480TWorkspace),
	},
	
    // Functions
    LCD_RV640480T_Open,
    LCD_RV640480T_Close,
    LCD_RV640480T_Configure,
    LCD_RV640480T_GetInfo,
    LCD_RV640480T_GetFrame,
    LCD_RV640480T_ShowFrame,
    LCD_RV640480T_On,
    LCD_RV640480T_Off,
    LCD_RV640480T_SetBacklightLevel,
    LCD_RV640480T_GetBacklightLevel,
    LCD_RV640480T_SetPaletteColor,
    // v2.04
    LCD_RV640480T_WaitForVerticalSync,
};

const DEVICE_LCD LCD_OKAYA_RV640480T_Interface_8Bit = {
	{
	    // Common interface
	    "LCD:OKAYA_RV640480T:8Bit",
	    0x0100,
	    LCD_RV640480T_InitializeWorkspace_8Bit,
	    sizeof(T_RV640480TWorkspace),
	},
	
    // Functions
    LCD_RV640480T_Open,
    LCD_RV640480T_Close,
    LCD_RV640480T_Configure,
    LCD_RV640480T_GetInfo,
    LCD_RV640480T_GetFrame,
    LCD_RV640480T_ShowFrame,
    LCD_RV640480T_On,
    LCD_RV640480T_Off,
    LCD_RV640480T_SetBacklightLevel,
    LCD_RV640480T_GetBacklightLevel,
    LCD_RV640480T_SetPaletteColor,
    // v2.04
    LCD_RV640480T_WaitForVerticalSync,
};

// List of interfaces
const T_uezDeviceInterface *LCD_OKAYA_RV640480T_InterfaceArray[] = {
    0,      // 1 bit
    0,      // 2 bit
    0,      // 4 bit
    (T_uezDeviceInterface *)&LCD_OKAYA_RV640480T_Interface_8Bit,      // 8 bit
    (T_uezDeviceInterface *)&LCD_OKAYA_RV640480T_Interface_16Bit,      // 16 bit
    (T_uezDeviceInterface *)&LCD_OKAYA_RV640480T_Interface_I15Bit,      // I15 bit
    0,      // 24 bit
};

/*-------------------------------------------------------------------------*
 * End of File:  Okaya_RV640480T.c
 *-------------------------------------------------------------------------*/
