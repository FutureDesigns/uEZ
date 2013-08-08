/*-------------------------------------------------------------------------*
 * File:  lcdDriver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the OKAYA RH320240T-3X5WP-A LCD.
 *      This LCD can run in 2 different modes:  8-bit (palletized)
 *      or 16-bit color.
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
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#include <uEZ.h>
#include <HAL/LCDController.h>
#include <Device/Backlight.h>
#include <Types/SPI.h>
#include <Device/SPIBus.h>
#include <UEZDeviceTable.h>
#include "Okaya_RH320240T.h"
#include <uEZGPIO.h>
#include <string.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RESOLUTION_X        320
#define RESOLUTION_Y        240

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

typedef struct {
	TUInt16 regAddress;
	TUInt16 dataByte;
} T_RH320240TConfigData ;

typedef struct {
    const DEVICE_LCD *iHAL;
    TUInt32 iBaseAddress;
    int aNumOpen;
    TUInt32 iBacklightLevel;
    HAL_LCDController **iLCDController;
    char iSPIBus[5];
    T_uezGPIOPortPin iCSGPIOPin;
    T_uezGPIOPortPin iSHUTGPIOPin;
    T_uezGPIOPortPin iENBGPIOPin;
    DEVICE_SPI_BUS **iSPI;
    DEVICE_Backlight **iBacklight;
    const T_uezLCDConfiguration *iConfiguration;
    SPI_Request r;
    TUInt8 iCmd[3];
	T_uezSemaphore iVSyncSem;
} T_RH320240TWorkspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
const T_RH320240TConfigData G_LCDConfig[] = {
    {0x0001, 0x7300},
    {0x0002, 0x0200},
    {0x0003, 0x6364},
    {0x0004, 0x04C7},
    {0x0005, 0xFC80},
    {0x000A, 0x4008},
    {0x000D, 0x3229},
    {0x000E, 0x3200},
    {0x001E, 0x00D2},
    };

static T_LCDControllerSettings LCD_RH320240T_settings;

static const T_LCDControllerSettings LCD_RH320240T_params16bit = {
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
    ETrue, //EFalse,     /* Invert panel clock */
    EFalse,     /* Invert HSYNC */
    EFalse,     /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0xA0000000, // Default Base address

    8000000,    // 8.0 MHz is optimal (12 ns min per dot = 8.333 MHz)
};

static T_LCDControllerSettings LCD_RH320240T_paramsI15bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_I555,

    68,         /* Horizontal back porch */
    20,         /* Horizontal front porch */
    2,         /* HSYNC pulse width */
    320,        /* Pixels per line */

    18,         /* Vertical back porch */
    4,         /* Vertical front porch */
    2,          /* VSYNC pulse width */
    240,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    ETrue, //EFalse,     /* Invert panel clock */
    EFalse,     /* Invert HSYNC */
    EFalse,     /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // RGB Color order

    0xA0000000, // Default Base address

    6500000,    // 6.5 MHz is datasheet typical value for 24 bit mode
};

static const T_LCDControllerSettings LCD_RH320240T_params8bit = {
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

    0xA0000000, // Default Base address

    6300000,    // 8.0 MHz is optimal (12 ns min per dot = 8.333 MHz)
};

static const T_uezLCDConfiguration LCD_RH320240T_configuration_16Bit = {
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

static T_uezLCDConfiguration LCD_RH320240T_configuration_I15Bit = {
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

static const T_uezLCDConfiguration LCD_RH320240T_configuration_8Bit = {
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
extern const DEVICE_LCD LCD_OKAYA_RH320240T_Interface_16Bit;
extern const DEVICE_LCD LCD_OKAYA_RH320240T_Interface_I15Bit;
extern const DEVICE_LCD LCD_OKAYA_RH320240T_Interface_8Bit;
void LCD_ConfigureSPI(T_RH320240TWorkspace *p);

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_InitializeWorkspace_16Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for RH320240T LCD (16 bit mode).
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_RH320240T_InitializeWorkspace_16Bit(void *aW)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
    p->iBaseAddress = 0x80000000;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_RH320240T_configuration_16Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_InitializeWorkspace_I15Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for RH320240T LCD (16 bit mode).
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_RH320240T_InitializeWorkspace_I15Bit(void *aW)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
    p->iBaseAddress = 0x80000000;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_RH320240T_configuration_I15Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_InitializeWorkspace_8Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for RH320240T LCD.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_RH320240T_InitializeWorkspace_8Bit(void *aW)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
    p->iBaseAddress = 0x80000000;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_RH320240T_configuration_8Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_Open
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
T_uezError LCD_RH320240T_Open(void *aW)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
    HAL_LCDController **plcdc;
    T_uezError error = UEZ_ERROR_NONE;

    p->aNumOpen++;
    if (p->aNumOpen == 1) {
        plcdc = p->iLCDController;

        // Choose a settings based on the configuration
        switch (p->iConfiguration->iColorDepth) {
            case UEZLCD_COLOR_DEPTH_8_BIT:
                LCD_RH320240T_settings = LCD_RH320240T_params8bit;
                break;
            default:
            case UEZLCD_COLOR_DEPTH_16_BIT:
                LCD_RH320240T_settings = LCD_RH320240T_params16bit;
                break;
            case UEZLCD_COLOR_DEPTH_I15_BIT:
                LCD_RH320240T_settings = LCD_RH320240T_paramsI15bit;
                break;
        }

        LCD_RH320240T_settings.iBaseAddress = p->iBaseAddress;

        // Turn on LCD controller and output power pin
        // Programs SPI first after power up
        error = (*plcdc)->Configure(plcdc, &LCD_RH320240T_settings);

// Testing
(*p->iLCDController)->On(p->iLCDController); // turns on data next

        UEZGPIOSetMux(p->iSHUTGPIOPin, 0);
        UEZGPIOSet(p->iSHUTGPIOPin);
        UEZGPIOOutput(p->iSHUTGPIOPin);

        // Drop SHUT down to be a low (active)
        UEZGPIOClear(p->iSHUTGPIOPin);

        // What does data enable pin do? high or low? It doesn't appear to be needed.
//      UEZGPIOSetMux(p->iENBGPIOPin, 0);
//      UEZGPIOClear(p->iENBGPIOPin);
//      UEZGPIOSet(p->iENBGPIOPin);
//      UEZGPIOOutput(p->iENBGPIOPin);

        // Configure SPI
        LCD_ConfigureSPI(p);

        return error;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      End access to the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RH320240T_Close(void *aW)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD to use a particular LCD controller.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully configured,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RH320240T_Configure(
            void *aW,
            HAL_LCDController **aLCDController,
            TUInt32 aBaseAddress,
            DEVICE_Backlight **aBacklight)
{
    T_uezDevice spi;
    T_uezDeviceWorkspace *p_spi;
//    T_uezDeviceWorkspace *p_gpio2;

    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
    p->iLCDController = aLCDController;
    p->iBaseAddress = aBaseAddress;

    p->iBacklight = aBacklight;

    UEZDeviceTableFind(p->iSPIBus, &spi);
    UEZDeviceTableGetWorkspace(spi, (T_uezDeviceWorkspace **)&p_spi);

    p->iSPI = (DEVICE_SPI_BUS **)p_spi;

    p->r.iDataMOSI = p->iCmd;
    p->r.iDataMISO = p->iCmd;
    p->r.iNumTransfers = 0;
    p->r.iBitsPerTransfer = 8;
    p->r.iRate = 9000;
    p->r.iClockOutPolarity = ETrue;//EFalse;
    p->r.iClockOutPhase = ETrue;
    p->r.iCSGPIOPort = GPIO_TO_HAL_PORT(p->iCSGPIOPin);
    p->r.iCSGPIOBit = GPIO_TO_PIN_BIT(p->iCSGPIOPin);
    p->r.iCSPolarity = EFalse; // LOW true
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the LCD device.
 * Inputs:
 *      void *aW                -- Workspace
 *      T_uezLCDConfiguration *aConfiguration -- Configuration settings
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RH320240T_GetInfo(
                void *aW,
                T_uezLCDConfiguration *aConfiguration)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;

    // Use the setting based on color depth
    *aConfiguration = *p->iConfiguration;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_GetFrame
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
T_uezError LCD_RH320240T_GetFrame(
            void *aW,
            TUInt32 aFrame,
            void **aFrameBuffer)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
    *aFrameBuffer = (void *)(p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_ShowFrame
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
T_uezError LCD_RH320240T_ShowFrame(void *aW, TUInt32 aFrame)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
    (*p->iLCDController)->SetBaseAddr(
            p->iLCDController,
            p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2,
            EFalse);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RH320240T_On(void *aW)
{
    // Turn back on to the remembered level
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
    (*p->iLCDController)->On(p->iLCDController);
    (*p->iBacklight)->On(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns off the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_RH320240T_Off(void *aW)
{
    // Turn off, but don't remember the level
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;
//TESTING    (*p->iLCDController)->Off(p->iLCDController);
    (*p->iBacklight)->Off(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_SetBacklightLevel
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
T_uezError LCD_RH320240T_SetBacklightLevel(void *aW, TUInt32 aLevel)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;

    // Limit the backlight level
    if (aLevel > p->iConfiguration->iNumBacklightLevels)
        aLevel = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    p->iBacklightLevel = aLevel;

    return (*p->iBacklight)->SetRatio(p->iBacklight,
        (aLevel * 0xFFFF) / (p->iConfiguration->iNumBacklightLevels));
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_GetBacklightLevel
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
static T_uezError LCD_RH320240T_GetBacklightLevel(
        void *aW,
        TUInt32 *aLevel,
        TUInt32 *aNumLevels)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aW;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (aNumLevels)
        *aNumLevels = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    *aLevel = p->iBacklightLevel;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_SetPaletteColor
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
T_uezError LCD_RH320240T_SetPaletteColor(
                    void *aWorkspace,
                    TUInt32 aColorIndex,
                    TUInt16 aRed,
                    TUInt16 aGreen,
                    TUInt16 aBlue)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aWorkspace;

    return (*p->iLCDController)->SetPaletteColor(
                p->iLCDController,
                aColorIndex,
                aRed,
                aGreen,
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_ConfigureSPI

 *---------------------------------------------------------------------------*
 * Description:
 *      Sends configuration data to the LCD via the SPI Bus
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      NONE
 *---------------------------------------------------------------------------*/
void LCD_ConfigureSPI(T_RH320240TWorkspace *p)
{
     TUInt8 i;

     for (i=0; i<8; i++)
     {
        p->iCmd[0] = 0x70; // command byte to send register
        p->iCmd[1] = (G_LCDConfig[i].regAddress>>8);
        p->iCmd[2] = (G_LCDConfig[i].regAddress &0xFF);
        p->r.iNumTransfers = 3;

        (*p->iSPI)->TransferPolled(p->iSPI, &p->r);

        p->iCmd[0] = 0x72; // command byte to send register
        p->iCmd[1] = (G_LCDConfig[i].dataByte>>8);
        p->iCmd[2] = (G_LCDConfig[i].dataByte &0xFF);
        p->r.iNumTransfers = 3;

        (*p->iSPI)->TransferPolled(p->iSPI, &p->r);
     }
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_VerticalSyncCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      This routine is called from within an interrupt when the vertical
 *      sync occurs.  This routines releases any waiting callers on
 *      the semaphore.
 * Inputs:
 *      void *aCallbackWorkspace -- LCD workspace
 *---------------------------------------------------------------------------*/
static void LCD_RH320240T_VerticalSyncCallback(void *aCallbackWorkspace)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aCallbackWorkspace;

    _isr_UEZSemaphoreRelease(p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_RH320240T_WaitForVerticalSync
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
static T_uezError LCD_RH320240T_WaitForVerticalSync(
    void *aWorkspace,
    TUInt32 aTimeout)
{
    T_RH320240TWorkspace *p = (T_RH320240TWorkspace *)aWorkspace;
    HAL_LCDController **p_hal = p->iLCDController;

    UEZSemaphoreGrab(p->iVSyncSem, 0);
    (*p_hal)->EnableVerticalSync(p_hal, LCD_RH320240T_VerticalSyncCallback, p);
    return UEZSemaphoreGrab(p->iVSyncSem, aTimeout);
}

void LCD_RH320240T_Create(char* aName, char* aSPIBus,
                          T_uezGPIOPortPin aSPICSPin,
                          T_uezGPIOPortPin aSHUTGPIOPin,
                          T_uezGPIOPortPin aENBGPIOPin)
{
    T_RH320240TWorkspace *p;

    T_uezDevice lcd;
    T_uezDeviceWorkspace *p_lcd;

    UEZDeviceTableFind(aName, &lcd);
    UEZDeviceTableGetWorkspace(lcd, &p_lcd);

    p = (T_RH320240TWorkspace *)p_lcd;

    strcpy(p->iSPIBus, aSPIBus);
    p->iCSGPIOPin = aSPICSPin;
    p->iSHUTGPIOPin = aSHUTGPIOPin;
    p->iENBGPIOPin = aENBGPIOPin;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LCD LCD_OKAYA_RH320240T_Interface_16Bit = {
	{
	    // Common interface
	    "LCD:OKAYA_RH320240T:16Bit",
	    0x0100,
	    LCD_RH320240T_InitializeWorkspace_16Bit,
	    sizeof(T_RH320240TWorkspace),
	},
	
    // Functions
    LCD_RH320240T_Open,
    LCD_RH320240T_Close,
    LCD_RH320240T_Configure,
    LCD_RH320240T_GetInfo,
    LCD_RH320240T_GetFrame,
    LCD_RH320240T_ShowFrame,
    LCD_RH320240T_On,
    LCD_RH320240T_Off,
    LCD_RH320240T_SetBacklightLevel,
    LCD_RH320240T_GetBacklightLevel,
    LCD_RH320240T_SetPaletteColor,
    // v2.04
    LCD_RH320240T_WaitForVerticalSync,
};

const DEVICE_LCD LCD_OKAYA_RH320240T_Interface_I15Bit = {
	{
	    // Common interface
	    "LCD:OKAYA_RH320240T:I15Bit",
	    0x0100,
	    LCD_RH320240T_InitializeWorkspace_I15Bit,
	    sizeof(T_RH320240TWorkspace),
	},
	
    // Functions
    LCD_RH320240T_Open,
    LCD_RH320240T_Close,
    LCD_RH320240T_Configure,
    LCD_RH320240T_GetInfo,
    LCD_RH320240T_GetFrame,
    LCD_RH320240T_ShowFrame,
    LCD_RH320240T_On,
    LCD_RH320240T_Off,
    LCD_RH320240T_SetBacklightLevel,
    LCD_RH320240T_GetBacklightLevel,
    LCD_RH320240T_SetPaletteColor,
    // v2.04
    LCD_RH320240T_WaitForVerticalSync,
};

const DEVICE_LCD LCD_OKAYA_RH320240T_Interface_8Bit = {
	{
	    // Common interface
	    "LCD:OKAYA_RH320240T:8Bit",
	    0x0100,
	    LCD_RH320240T_InitializeWorkspace_8Bit,
	    sizeof(T_RH320240TWorkspace),
	},
	
    // Functions
    LCD_RH320240T_Open,
    LCD_RH320240T_Close,
    LCD_RH320240T_Configure,
    LCD_RH320240T_GetInfo,
    LCD_RH320240T_GetFrame,
    LCD_RH320240T_ShowFrame,
    LCD_RH320240T_On,
    LCD_RH320240T_Off,
    LCD_RH320240T_SetBacklightLevel,
    LCD_RH320240T_GetBacklightLevel,
    LCD_RH320240T_SetPaletteColor,
    // v2.04
    LCD_RH320240T_WaitForVerticalSync,
};

// List of interfaces
const T_uezDeviceInterface *LCD_OKAYA_RH320240T_InterfaceArray[] = {
    0,      // 1 bit
    0,      // 2 bit
    0,      // 4 bit
    (T_uezDeviceInterface *)&LCD_OKAYA_RH320240T_Interface_8Bit,      // 8 bit
    (T_uezDeviceInterface *)&LCD_OKAYA_RH320240T_Interface_16Bit,      // 16 bit
    (T_uezDeviceInterface *)&LCD_OKAYA_RH320240T_Interface_I15Bit,      // I15 bit
    0,      // 24 bit
};

/*-------------------------------------------------------------------------*
 * End of File:  lcdDriver.c
 *-------------------------------------------------------------------------*/
