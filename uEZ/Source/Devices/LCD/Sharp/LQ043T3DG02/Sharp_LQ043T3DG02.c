/*-------------------------------------------------------------------------*
 * File:  Sharp_LQ043T3DG02.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the Sharp LQ043T3DG02.
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
#include <uEZSPI.h>
#include <string.h>
#include "Sharp_LQ043T3DG02.h"
#include <uEZGPIO.h>
#include <uEZDeviceTable.h>
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
    char iSPIBus[5];
    T_uezDevice iSPI;
    T_uezGPIOPortPin iCSGPIOPin;
    T_uezGPIOPortPin iResetGPIOPin;
	T_uezSemaphore iVSyncSem;
} T_LQ043T3DG02Workspace;

typedef struct {
    TUInt8 iReg;
        #define REG_END         0xFF
    TUInt16 iData;
} T_lcdSPICmd;


/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LCDControllerSettings LCD_LQ043T3DG02_settings;

static const T_LCDControllerSettings LCD_LQ043T3DG02_params16bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_565,

    16,         /* Horizontal back porch */
    10,         /* Horizontal front porch */
    2,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    3,          /* Vertical back porch */
    2,          /* Vertical front porch */
    2,         /* VSYNC pulse width */
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

static const T_LCDControllerSettings LCD_LQ043T3DG02_paramsI15bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_I555,

#if 1
    16,         /* Horizontal back porch */
    10,         /* Horizontal front porch */
    2,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    3,          /* Vertical back porch */
    2,          /* Vertical front porch */
    10,          /* VSYNC pulse width */
    272,        /* Lines per panel */
#else
    16,         /* Horizontal back porch */
    10,         /* Horizontal front porch */
    2,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    3,          /* Vertical back porch */
    2,          /* Vertical front porch */
    2,          /* VSYNC pulse width */
    272,        /* Lines per panel */
#endif

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
//    2000000,
};

static const T_LCDControllerSettings LCD_LQ043T3DG02_params8bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_8,

    16,         /* Horizontal back porch */
    10,         /* Horizontal front porch */
    2,          /* HSYNC pulse width */
    480,        /* Pixels per line */

    3,          /* Vertical back porch */
    2,          /* Vertical front porch */
    2,         /* VSYNC pulse width */
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

static T_uezLCDConfiguration LCD_LQ043T3DG02_configuration_16Bit = {
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

static T_uezLCDConfiguration LCD_LQ043T3DG02_configuration_I15Bit = {
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

static T_uezLCDConfiguration LCD_LQ043T3DG02_configuration_8Bit = {
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

static const T_lcdSPICmd G_lcdStartup[] = {
    { 0x01, 0x230F },
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

static const T_lcdSPICmd G_lcdExitSleep[] = {
    { 0x28, 0x0006 },
    { 0x2D, 0x3F44 },
    { 0x29, 0xFFFE },
    { REG_END, 0 }
};

static const T_lcdSPICmd G_lcdEnterSleep[] = {
    { 0x28, 0x0006 },
    { 0x29, 0x8000 },
    { 0x2E, 0xB544 },
    { 0x2D, 0x3F46 },
    { REG_END, 0 }
};

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_LCD LCD_SHARP_LQ043T3DG02_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_InitializeWorkspace_16Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for LQ043T3DG02 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ043T3DG02_InitializeWorkspace_16Bit(void *aW)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_LQ043T3DG02_configuration_16Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_InitializeWorkspace_I15Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for LQ043T3DG02 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ043T3DG02_InitializeWorkspace_I15Bit(void *aW)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_LQ043T3DG02_configuration_I15Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_InitializeWorkspace_8Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for LQ043T3DG02 LCD.
 * Inputs:
 *      void *aW                    -- Particular  workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ043T3DG02_InitializeWorkspace_8Bit(void *aW)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 256; // 100%
    p->iConfiguration = &LCD_LQ043T3DG02_configuration_8Bit;

    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

static T_uezError SPIWriteCmd(
        T_LQ043T3DG02Workspace *p,
        const T_lcdSPICmd *aCmd)
{
    SPI_Request r;
    TUInt8 cmd[3];
    TUInt8 data[3];
    T_uezError error = UEZ_ERROR_NONE;

#define LCD_SPI_WRITE       (0<<0)
#define LCD_SPI_READ        (1<<0)
#define LCD_SPI_CMD         (0<<1)
#define LCD_SPI_DATA        (1<<1)
#define LCD_SPI_ID0         (0<<2)
#define LCD_SPI_ID1         (1<<2)

    cmd[0] = 0x70|LCD_SPI_ID0|LCD_SPI_WRITE|LCD_SPI_CMD;
    cmd[1] = 0x00;
    cmd[2] = aCmd->iReg;
    r.iNumTransfers = 3;
    r.iBitsPerTransfer = 8;
    r.iRate = 1000; // spec says 12 MHz, but we'll start slow at 1 MHz
    r.iClockOutPolarity = ETrue; // rising edge
    r.iClockOutPhase = ETrue;
    r.iCSGPIOPort = GPIO_TO_HAL_PORT(p->iCSGPIOPin);
    r.iCSGPIOBit = GPIO_TO_PIN_BIT(p->iCSGPIOPin);
    r.iCSPolarity = EFalse;
    r.iDataMISO = cmd;
    r.iDataMOSI = cmd;
    error = UEZSPITransferPolled(p->iSPI, &r);

    if (!error) {
        data[0] = 0x70|LCD_SPI_ID0|LCD_SPI_WRITE|LCD_SPI_DATA;
        data[1] = aCmd->iData>>8;
        data[2] = aCmd->iData & 0xFF;
        r.iDataMOSI = data;
        r.iDataMISO = data;
        error = UEZSPITransferPolled(p->iSPI, &r);
    }

    return error;
}

static T_uezError ISPIWriteCommands(
        T_LQ043T3DG02Workspace *p,
        const T_lcdSPICmd *aCmd)
{
    T_uezError error = UEZ_ERROR_NONE;
    while (aCmd->iReg != REG_END) {
        error = SPIWriteCmd(p, aCmd);
        if (error)
            break;
        aCmd++;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_Open
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
static T_uezError ISPIConfigure(T_LQ043T3DG02Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    error = UEZSPIOpen(p->iSPIBus, &p->iSPI);
    if (!error) {
        if (!error) {
            UEZTaskDelay(1);
            error = ISPIWriteCommands(p, G_lcdEnterSleep);
            if (!error) {
                UEZTaskDelay(1);
                error = ISPIWriteCommands(p, G_lcdStartup);
                if (!error) {
                    UEZTaskDelay(1);
//                    (*p->iLCDController)->On(p->iLCDController);
                    error = ISPIWriteCommands(p, G_lcdExitSleep);
//                    UEZTaskDelay(1000);
                }
            }
        }
        UEZSPIClose(p->iSPI);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_Open
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
static T_uezError LCD_LQ043T3DG02_Open(void *aW)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    HAL_LCDController **plcdc;
    T_uezError error = UEZ_ERROR_NONE;

    p->aNumOpen++;

    if (p->aNumOpen == 1) {
        memset((void *)p->iBaseAddress, 0, 480*272*2);
        plcdc = p->iLCDController;

        if (!error) {
            switch (p->iConfiguration->iColorDepth) {
                case UEZLCD_COLOR_DEPTH_8_BIT:
                    LCD_LQ043T3DG02_settings = LCD_LQ043T3DG02_params8bit;
                    break;
                default:
                case UEZLCD_COLOR_DEPTH_16_BIT:
                    LCD_LQ043T3DG02_settings = LCD_LQ043T3DG02_params16bit;
                    break;
                case UEZLCD_COLOR_DEPTH_I15_BIT:
                    LCD_LQ043T3DG02_settings = LCD_LQ043T3DG02_paramsI15bit;
                    break;
            }
            LCD_LQ043T3DG02_settings.iBaseAddress = p->iBaseAddress;
            error = (*plcdc)->Configure(plcdc, &LCD_LQ043T3DG02_settings);

            if (!error) {
//                for (i=0; i<480*272*2; i+=2) {
//                    *((TUInt16 *)(p->iBaseAddress+i)) = 0x0000; // black
//                }
#if 0
                for (i=0; i<480*272*2; i+=4) {
                    *((TUInt16 *)(p->iBaseAddress+i)) = 0x001F; // blue
                    *((TUInt16 *)(p->iBaseAddress+i)) = (0x1F<<10); // red (no intensity bit)
                }
#endif
                (*p->iLCDController)->On(p->iLCDController);

                // Toggle the rest pin
                UEZGPIOClear(p->iResetGPIOPin);
                UEZTaskDelay(1);
                UEZGPIOSet(p->iResetGPIOPin);
                UEZTaskDelay(1);

                // Configure the LCD over the SPI port
                error = ISPIConfigure(p);
            }
        }
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      End access to the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ043T3DG02_Close(void *aW)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD to use a particular LCD controller.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully configured,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_LQ043T3DG02_Configure(
            void *aW,
            HAL_LCDController **aLCDController,
            TUInt32 aBaseAddress,
            DEVICE_Backlight **aBacklight)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    T_uezError error = UEZ_ERROR_NONE;

    p->iLCDController = aLCDController;
    p->iBaseAddress = aBaseAddress;
    p->iBacklight = aBacklight;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the LCD device.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ043T3DG02_GetInfo(void *aW, T_uezLCDConfiguration *aConfiguration)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;

    // Use the setting based on color depth
    *aConfiguration = *p->iConfiguration;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_GetFrame
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
static T_uezError LCD_LQ043T3DG02_GetFrame(
            void *aW,
            TUInt32 aFrame,
            void **aFrameBuffer)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    *aFrameBuffer = (void *)(p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_ShowFrame
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
static T_uezError LCD_LQ043T3DG02_ShowFrame(void *aW, TUInt32 aFrame)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    (*p->iLCDController)->SetBaseAddr(
            p->iLCDController,
            p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2,
            EFalse);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ043T3DG02_On(void *aW)
{
    // Turn back on to the remembered level
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    (*p->iLCDController)->On(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->On(p->iBacklight);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns off the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_LQ043T3DG02_Off(void *aW)
{
    // Turn off, but don't remember the level
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
    (*p->iLCDController)->Off(p->iLCDController);

    if (p->iBacklight)
        (*p->iBacklight)->Off(p->iBacklight);


    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_SetBacklightLevel
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
static T_uezError LCD_LQ043T3DG02_SetBacklightLevel(void *aW, TUInt32 aLevel)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;
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
 * Routine:  LCD_LQ043T3DG02_GetBacklightLevel
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
static T_uezError LCD_LQ043T3DG02_GetBacklightLevel(
        void *aW, 
        TUInt32 *aLevel,
        TUInt32 *aNumLevels)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aW;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (aNumLevels)
        *aNumLevels = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    *aLevel = p->iBacklightLevel;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_SetPaletteColor
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
static T_uezError LCD_LQ043T3DG02_SetPaletteColor(
                    void *aWorkspace,
                    TUInt32 aColorIndex,
                    TUInt16 aRed,
                    TUInt16 aGreen,
                    TUInt16 aBlue)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aWorkspace;

    return (*p->iLCDController)->SetPaletteColor(
                p->iLCDController,
                aColorIndex,
                aRed,
                aGreen,
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_VerticalSyncCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      This routine is called from within an interrupt when the vertical
 *      sync occurs.  This routines releases any waiting callers on
 *      the semaphore.
 * Inputs:
 *      void *aCallbackWorkspace -- LCD workspace
 *---------------------------------------------------------------------------*/
static void LCD_LQ043T3DG02_VerticalSyncCallback(void *aCallbackWorkspace)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aCallbackWorkspace;

    _isr_UEZSemaphoreRelease(p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_LQ043T3DG02_WaitForVerticalSync
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
static T_uezError LCD_LQ043T3DG02_WaitForVerticalSync(
    void *aWorkspace,
    TUInt32 aTimeout)
{
    T_LQ043T3DG02Workspace *p = (T_LQ043T3DG02Workspace *)aWorkspace;
    HAL_LCDController **p_hal = p->iLCDController;

    UEZSemaphoreGrab(p->iVSyncSem, 0);
    (*p_hal)->EnableVerticalSync(p_hal, LCD_LQ043T3DG02_VerticalSyncCallback, p);
    return UEZSemaphoreGrab(p->iVSyncSem, aTimeout);
}

void LCD_LQ043T3DG02_Create(char* aName,
                            char* aSPIBus,
                            T_uezGPIOPortPin aSPICSPin,
                            T_uezGPIOPortPin aResetPin)
{
    T_LQ043T3DG02Workspace *p;

    T_uezDevice lcd;
    T_uezDeviceWorkspace *p_lcd;

    UEZDeviceTableFind(aName, &lcd);
    UEZDeviceTableGetWorkspace(lcd, &p_lcd);

    p = (T_LQ043T3DG02Workspace *)p_lcd;

    strcpy(p->iSPIBus, aSPIBus);
    p->iCSGPIOPin = aSPICSPin;
    p->iResetGPIOPin = aResetPin;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LCD LCD_SHARP_LQ043T3DG02_Interface_16Bit = {
	{
	    // Common interface
	    "LCD:SHARP_LQ043T3DG02:16Bit",
	    0x0100,
	    LCD_LQ043T3DG02_InitializeWorkspace_16Bit,
	    sizeof(T_LQ043T3DG02Workspace),
	},
	
    // Functions
    LCD_LQ043T3DG02_Open,
    LCD_LQ043T3DG02_Close,
    LCD_LQ043T3DG02_Configure,
    LCD_LQ043T3DG02_GetInfo,
    LCD_LQ043T3DG02_GetFrame,
    LCD_LQ043T3DG02_ShowFrame,
    LCD_LQ043T3DG02_On,
    LCD_LQ043T3DG02_Off,
    LCD_LQ043T3DG02_SetBacklightLevel,
    LCD_LQ043T3DG02_GetBacklightLevel,
    LCD_LQ043T3DG02_SetPaletteColor,
    // v2.04
    LCD_LQ043T3DG02_WaitForVerticalSync,
};

const DEVICE_LCD LCD_SHARP_LQ043T3DG02_Interface_I15Bit = {
	{
	    // Common interface
	    "LCD:SHARP_LQ043T3DG02:I15Bit",
	    0x0100,
	    LCD_LQ043T3DG02_InitializeWorkspace_I15Bit,
	    sizeof(T_LQ043T3DG02Workspace),
	},
	
    // Functions
    LCD_LQ043T3DG02_Open,
    LCD_LQ043T3DG02_Close,
    LCD_LQ043T3DG02_Configure,
    LCD_LQ043T3DG02_GetInfo,
    LCD_LQ043T3DG02_GetFrame,
    LCD_LQ043T3DG02_ShowFrame,
    LCD_LQ043T3DG02_On,
    LCD_LQ043T3DG02_Off,
    LCD_LQ043T3DG02_SetBacklightLevel,
    LCD_LQ043T3DG02_GetBacklightLevel,
    LCD_LQ043T3DG02_SetPaletteColor,
    // v2.04
    LCD_LQ043T3DG02_WaitForVerticalSync,
};

const DEVICE_LCD LCD_SHARP_LQ043T3DG02_Interface_8Bit = {
	{
	    // Common interface
	    "LCD:SHARP_LQ043T3DG02:8Bit",
	    0x0100,
	    LCD_LQ043T3DG02_InitializeWorkspace_8Bit,
	    sizeof(T_LQ043T3DG02Workspace),
	},
	
    // Functions
    LCD_LQ043T3DG02_Open,
    LCD_LQ043T3DG02_Close,
    LCD_LQ043T3DG02_Configure,
    LCD_LQ043T3DG02_GetInfo,
    LCD_LQ043T3DG02_GetFrame,
    LCD_LQ043T3DG02_ShowFrame,
    LCD_LQ043T3DG02_On,
    LCD_LQ043T3DG02_Off,
    LCD_LQ043T3DG02_SetBacklightLevel,
    LCD_LQ043T3DG02_GetBacklightLevel,
    LCD_LQ043T3DG02_SetPaletteColor,
    // v2.04
    LCD_LQ043T3DG02_WaitForVerticalSync,
};

// List of interfaces
const T_uezDeviceInterface *LCD_SHARP_LQ043T3DG02_InterfaceArray[] = {
    0,      // 1 bit
    0,      // 2 bit
    0,      // 4 bit
    (T_uezDeviceInterface *)&LCD_SHARP_LQ043T3DG02_Interface_8Bit,      // 8 bit
    (T_uezDeviceInterface *)&LCD_SHARP_LQ043T3DG02_Interface_16Bit,      // 16 bit
    (T_uezDeviceInterface *)&LCD_SHARP_LQ043T3DG02_Interface_I15Bit,      // I15 bit
    0,      // 24 bit
};

/*-------------------------------------------------------------------------*
 * End of File:  Sharp_LQ043T3DG02.c
 *-------------------------------------------------------------------------*/
