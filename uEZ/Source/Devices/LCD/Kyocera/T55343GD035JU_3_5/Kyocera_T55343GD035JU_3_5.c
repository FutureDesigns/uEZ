/*-------------------------------------------------------------------------*
 * File:  Kyocera_T55343GD035JU_3_5.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HAL implementation of the Kyocera T55343GD035JU_3_5.
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
#include <UEZGPIO.h>
#include <uEZSPI.h>
#include "Kyocera_T55343GD035JU_3_5.h"
#include <uEZDeviceTable.h>
#include <string.h>
#include <uEZGPIO.h>
#include <uEZTimer.h>
#include <uEZPlatformAPI.h>
#include "uEZPlatform.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RESOLUTION_X        320
#define RESOLUTION_Y        240
#define LCD_CLOCK_RATE      7500000

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
    char iSPIBus[5];
    T_uezDevice iSPI;
    T_uezGPIOPortPin iCSGPIOPin;
    T_uezSemaphore iVSyncSem;
    T_uezDevice itimer;
    T_uezTimerCallback icallback;
    volatile TBool itimerDone;
} T_T55343GD035JU_3_5Workspace;

typedef struct {
    TUInt8 iReg;
        #define REG_END         0xFF
    TUInt16 iData;
} T_lcdSPICmd;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_LCDControllerSettings LCD_T55343GD035JU_3_5_settings;

static const T_LCDControllerSettings LCD_T55343GD035JU_3_5_params16bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_565,

    11,         /* Horizontal back porch */
    0,          /* Horizontal front porch */
    96,         /* HSYNC pulse width */
    320,        /* Pixels per line */

    8,          /* Vertical back porch */
    0,          /* Vertical front porch */
    15,         /* VSYNC pulse width */
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

    LCD_CLOCK_RATE,    // 6.3 MHz is optimal
};

static const T_LCDControllerSettings LCD_T55343GD035JU_3_5_paramsI15bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_16_I555,

    70,         /* Horizontal back porch */
    1,          /* Horizontal front porch */ // TIMH-HFP must be 0
    96,         /* HSYNC pulse width */
    320,        /* Pixels per line */

    0,          /* Vertical back porch */
    0,          /* Vertical front porch */
    13,         /* VSYNC pulse width */
    240,        /* Lines per panel */

    0,          // Line end delay disabled

    EFalse,     /* Do not invert output enable */
    EFalse,     /* Invert panel clock */
    ETrue,     /* Invert HSYNC */
    ETrue,     /* Invert VSYNC */

    0,          /* AC bias frequency (not used) */

    EFalse,     // Not dual panel
    EFalse,     // Little endian (NOT big endian)
    EFalse,     //    Left to Right pixels (NOT right to left)
    EFalse,     // Top to bottom (NOT bottom to top)
    LCD_COLOR_ORDER_BGR,    // BGR order please

    0, // Default Base address

    LCD_CLOCK_RATE,    // 6.3 MHz is optimal
};

static const T_LCDControllerSettings LCD_T55343GD035JU_3_5_params8bit = {
    LCD_ADVANCED_TFT,
    LCD_COLOR_RES_8,

    11,         /* Horizontal back porch */
    0,          /* Horizontal front porch */
    96,         /* HSYNC pulse width */
    320,        /* Pixels per line */

    8,          /* Vertical back porch */
    0,          /* Vertical front porch */
    15,         /* VSYNC pulse width */
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

    LCD_CLOCK_RATE,    // 6.3 MHz is optimal
};

static T_uezLCDConfiguration LCD_T55343GD035JU_3_5_configuration_16Bit = {
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

static T_uezLCDConfiguration LCD_T55343GD035JU_3_5_configuration_I15Bit = {
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

static T_uezLCDConfiguration LCD_T55343GD035JU_3_5_configuration_8Bit = {
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
    // Panel Driving Control
    { 0x01, 0x6300 },
    { 0x02, 0x0200 },
    { 0x03, 0x6064 }, // power control
    { 0x04, 0x0447 },
    { 0x05, 0xB084 },
    { 0x0A, 0x4008 },
    { 0x0B, 0xD400 },
    { 0x0D, 0x423D }, // power control
    { 0x0E, 0x3140 }, // power control
    { 0x0F, 0x0000 },
    { 0x16, 0x9F80 },
    { 0x17, 0x2212 },
    { 0x1E, 0x00DB }, // power control
    { 0x30, 0x0000 }, // Gamma 1-10
    { 0x31, 0x0607 },
    { 0x32, 0x0006 },
    { 0x33, 0x0307 },
    { 0x34, 0x0107 },
    { 0x35, 0x0001 },
    { 0x36, 0x0707 },
    { 0x37, 0x0703 },
    { 0x3A, 0x0C00 },
    { 0x3B, 0x0006 },
    { REG_END, 0 }
};

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_LCD LCD_Kyocera_T55343GD035JU_3_5_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_InitializeWorkspace_16Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for T55343GD035JU_3_5 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_T55343GD035JU_3_5_InitializeWorkspace_16Bit(void *aW)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 0; // 0%
    p->iConfiguration = &LCD_T55343GD035JU_3_5_configuration_16Bit;
    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_InitializeWorkspace_I15Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for T55343GD035JU_3_5 LCD.
 * Inputs:
 *      void *aW                    -- Particular workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_T55343GD035JU_3_5_InitializeWorkspace_I15Bit(void *aW)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 0; // 0%
    p->iConfiguration = &LCD_T55343GD035JU_3_5_configuration_I15Bit;
    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_InitializeWorkspace_8Bit
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for T55343GD035JU_3_5 LCD.
 * Inputs:
 *      void *aW                    -- Particular  workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LCD_T55343GD035JU_3_5_InitializeWorkspace_8Bit(void *aW)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
    p->iBaseAddress = LCD_DISPLAY_BASE_ADDRESS;
    p->aNumOpen = 0;
    p->iBacklightLevel = 0; // 0%
    p->iConfiguration = &LCD_T55343GD035JU_3_5_configuration_8Bit;
    return UEZSemaphoreCreateBinary(&p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_SetBacklightLevel
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
static T_uezError LCD_T55343GD035JU_3_5_SetBacklightLevel(void *aW, TUInt32 aLevel)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
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

static T_uezError SPIWriteCmd(
        T_T55343GD035JU_3_5Workspace *p,
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

    cmd[0] = 0x74|LCD_SPI_ID0|LCD_SPI_WRITE|LCD_SPI_CMD;
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
        data[0] = 0x74|LCD_SPI_ID0|LCD_SPI_WRITE|LCD_SPI_DATA;
        data[1] = aCmd->iData>>8;
        data[2] = (aCmd->iData & 0xFF);
        r.iDataMOSI = data;
        r.iDataMISO = data;
        error = UEZSPITransferPolled(p->iSPI, &r);
    }
    return error;
}

static T_uezError ISPIWriteCommands(
        T_T55343GD035JU_3_5Workspace *p,
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
 * Routine:  ISPIConfigure
 *---------------------------------------------------------------------------*
 * Description:
 *      program lcd with SPI commands
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If programming is successful, returns
 *                                  UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError ISPIConfigure(T_T55343GD035JU_3_5Workspace *p)
{
    T_uezError error = UEZ_ERROR_NONE;
    if(strcmp(p->iSPIBus, "\0") == 0){
        //See LCD_T55343GD035JU_3_5_Create to fix.
        UEZFailureMsg("SPI Bus not Configured");
    }
    error = UEZSPIOpen(p->iSPIBus, &p->iSPI);
    if (!error) {
            if (!error) {
                UEZTaskDelay(1);
                error = ISPIWriteCommands(p, G_lcdStartup);
            }
        UEZSPIClose(p->iSPI);
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_MSTimerStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup MS timer in one function to avoid cluttering up the open function
 * Inputs:
*      T_uezTimerCallback *aCallbackWorkspace  -- Workspace
*---------------------------------------------------------------------------*/
//static T_uezError LCD_T55343GD035JU_3_5_MSTimerStart(void *aW, float milliseconds){
//  T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
//  T_uezError error = UEZ_ERROR_NONE;
//  p->itimerDone = EFalse; // set to true when itimer finishes
//  error = UEZTimerSetupOneShot(p->itimer,
//                               1,
//                               ((int32_t)milliseconds*(PROCESSOR_OSCILLATOR_FREQUENCY/1000)),
//                               &p->icallback);
//  if(error == UEZ_ERROR_NONE) {
//    error = UEZTimerSetTimerMode(p->itimer, TIMER_MODE_CLOCK);
//    error = UEZTimerReset(p->itimer);
//    UEZTimerEnable(p->itimer);
//  }
//  return error;
//}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_TimerCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Timer icallback to set pins immediately when the target time is reached.
 * Needed for LCDs with sub-ms and ms range accuracy for bring up sequencing.
 * Inputs:
 *      T_uezTimerCallback *aCallbackWorkspace  -- Workspace
 *---------------------------------------------------------------------------*/
//static void LCD_T55343GD035JU_3_5_TimerCallback(T_uezTimerCallback *aCallbackWorkspace){
//  T_T55343GD035JU_3_5Workspace *p = aCallbackWorkspace->iData;
//  p->itimerDone = ETrue;
//  UEZTimerClose(p->itimer);
//}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns on the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_T55343GD035JU_3_5_On(void *aW) {
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
    T_uezError error = UEZ_ERROR_NONE;

    //if (UEZTimerOpen("Timer0", &p->itimer) == UEZ_ERROR_NONE) {
        //LCD_T55343GD035JU_3_5_MSTimerStart(p, 1.25); // 1.25ms itimer, minimum 1ms needed
        //while (p->itimerDone == EFalse){;} // wait for itimer to finish, there will be a small task delay of a few hundred uS
        UEZTaskDelay(1);

        (*p->iLCDController)->On(p->iLCDController); // max 100us delay, start DOTCLK/HSYNC/VSYNC immediately
        error = ISPIConfigure(p); // program registers, configure the LCD over the SPI port

        //LCD_T55343GD035JU_3_5_MSTimerStart(p, 6.25);// 6.25ms itimer, minimum 5ms
        //while (p->itimerDone == EFalse){;} // wait for timer before starting next stage
        UEZTaskDelay(5);

        //LCD_T55343GD035JU_3_5_MSTimerStart(p, 184.0); // minimum 166ms timer, 184ms = 11 frame skip
        // Backlight will turn on when timer callback is called, then timer will be closed
        //while (p->itimerDone == EFalse){;} // wait for timer before continuing
        UEZTaskDelay(184);
        if(p->iBacklight){
            (*p->iBacklight)->On(p->iBacklight); // turn backlight on
            LCD_T55343GD035JU_3_5_SetBacklightLevel(p, p->iBacklightLevel);
        }
      //}

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turns off the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_T55343GD035JU_3_5_Off(void *aW)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;

    if (p->iBacklight){ // Turn off backlight
      if(p->iBacklight){
          (*p->iBacklight)->Off(p->iBacklight);
      }
      //if (UEZTimerOpen("Timer0", &p->itimer) == UEZ_ERROR_NONE) {
      //  LCD_T55343GD035JU_3_5_MSTimerStart(p, 166.6); // minimum 166.6ms timer
      //  while (p->itimerDone == EFalse){;} // wait for timer to finish, there will be a small task delay of a few hundred uS
      //}
    }
    //(*p->iLCDController)->Off(p->iLCDController); // turn off LCD
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_Open
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
static T_uezError LCD_T55343GD035JU_3_5_Open(void *aW)
{
  T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
  HAL_LCDController **plcdc;
  T_uezError error = UEZ_ERROR_NONE;
  TUInt32 i;

  p->icallback.iTimer = p->itimer; // Setup callback information for itimer
    p->icallback.iData = p;
  p->icallback.iMatchRegister = 1;
  p->icallback.iTriggerSem = 0;
  //p->icallback.iCallback = LCD_T55343GD035JU_3_5_TimerCallback;

  p->aNumOpen++;
  if (p->aNumOpen == 1) {
    plcdc = p->iLCDController;
      switch (p->iConfiguration->iColorDepth) {
      case UEZLCD_COLOR_DEPTH_8_BIT:
        LCD_T55343GD035JU_3_5_settings = LCD_T55343GD035JU_3_5_params8bit;
        break;
      default:
      case UEZLCD_COLOR_DEPTH_16_BIT:
        LCD_T55343GD035JU_3_5_settings = LCD_T55343GD035JU_3_5_params16bit;
        break;
      case UEZLCD_COLOR_DEPTH_I15_BIT:
        LCD_T55343GD035JU_3_5_settings = LCD_T55343GD035JU_3_5_paramsI15bit;
        break;
      }
      LCD_T55343GD035JU_3_5_settings.iBaseAddress = p->iBaseAddress;
        //pre-configure LCD controller first so that there is no delay for turn on
        error = (*plcdc)->Configure(plcdc, &LCD_T55343GD035JU_3_5_settings);

        //for (i=0; i<480*272*2; i+=2) {// black screen to clear any leftovers in memory
        //    *((TUInt16 *)(p->iBaseAddress+i)) = 0x0000;
        //}

        if (!error) {
            error = LCD_T55343GD035JU_3_5_On(p);
        }
  }
  return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      End access to the LCD display.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_T55343GD035JU_3_5_Close(void *aW)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD to use a particular LCD controller.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If the device is successfully configured,
 *                                  returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
T_uezError LCD_T55343GD035JU_3_5_Configure(
            void *aW,
            HAL_LCDController **aLCDController,
            TUInt32 aBaseAddress,
            DEVICE_Backlight **aBacklight)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
    T_uezError error = UEZ_ERROR_NONE;

    p->iLCDController = aLCDController;
    p->iBaseAddress = aBaseAddress;
    p->iBacklight = aBacklight;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Get information about the LCD device.
 * Inputs:
 *      void *aW                -- Workspace
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_T55343GD035JU_3_5_GetInfo(void *aW, T_uezLCDConfiguration *aConfiguration)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;

    // Use the setting based on color depth
    *aConfiguration = *p->iConfiguration;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_GetFrame
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
static T_uezError LCD_T55343GD035JU_3_5_GetFrame(
            void *aW,
            TUInt32 aFrame,
            void **aFrameBuffer)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
    *aFrameBuffer = (void *)(p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_ShowFrame
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
static T_uezError LCD_T55343GD035JU_3_5_ShowFrame(void *aW, TUInt32 aFrame)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;
    (*p->iLCDController)->SetBaseAddr(
            p->iLCDController,
            p->iBaseAddress + aFrame * RESOLUTION_X * RESOLUTION_Y * 2,
            EFalse);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_GetBacklightLevel
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
static T_uezError LCD_T55343GD035JU_3_5_GetBacklightLevel(
        void *aW, 
        TUInt32 *aLevel,
        TUInt32 *aNumLevels)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aW;

    if (!p->iBacklight)
        return UEZ_ERROR_NOT_SUPPORTED;

    if (aNumLevels)
        *aNumLevels = p->iConfiguration->iNumBacklightLevels;

    // Remember this level and use it
    *aLevel = p->iBacklightLevel;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_SetPaletteColor
 *---------------------------------------------------------------------------*
 * Description:
 *      Change the color of a palette entry given the red, green, blue
 *      component of a particular color index.  The color components are
 *      expressed in full 16-bit values at a higher resolution than
 *      the hardware can usually perform.  The color is down shifted to what
 *      the hardware can handle.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aColorIndex         -- Index to palate entry
 *      TUInt16 aRed                -- Red value
 *      TUInt16 aGreen              -- Green value
 *      TUInt16 aBlue               -- Blue value
 * Outputs:
 *      T_uezError               -- If successful, returns UEZ_ERROR_NONE.
 *                                  If the index is invalid, returns
 *                                  UEZ_ERROR_OUT_OF_RANGE.
 *---------------------------------------------------------------------------*/
static T_uezError LCD_T55343GD035JU_3_5_SetPaletteColor(
                    void *aWorkspace,
                    TUInt32 aColorIndex,
                    TUInt16 aRed,
                    TUInt16 aGreen,
                    TUInt16 aBlue)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aWorkspace;

    return (*p->iLCDController)->SetPaletteColor(
                p->iLCDController,
                aColorIndex,
                aRed,
                aGreen,
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_VerticalSyncCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      This routine is called from within an interrupt when the vertical
 *      sync occurs.  This routines releases any waiting callers on
 *      the semaphore.
 * Inputs:
 *      void *aCallbackWorkspace -- LCD workspace
 *---------------------------------------------------------------------------*/
static void LCD_T55343GD035JU_3_5_VerticalSyncCallback(void *aCallbackWorkspace)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aCallbackWorkspace;

    _isr_UEZSemaphoreRelease(p->iVSyncSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_WaitForVerticalSync
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
static T_uezError LCD_T55343GD035JU_3_5_WaitForVerticalSync(
    void *aWorkspace,
    TUInt32 aTimeout)
{
    T_T55343GD035JU_3_5Workspace *p = (T_T55343GD035JU_3_5Workspace *)aWorkspace;
    HAL_LCDController **p_hal = p->iLCDController;

    UEZSemaphoreGrab(p->iVSyncSem, 0);
    (*p_hal)->EnableVerticalSync(p_hal, LCD_T55343GD035JU_3_5_VerticalSyncCallback, p);
    return UEZSemaphoreGrab(p->iVSyncSem, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  LCD_T55343GD035JU_3_5_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Routine to create workspace.
 * Inputs:
 *      void *aWorkspace -- LCD workspace
 *      const char* -- SPI or SSP bus the LCD on on (ex. "SSP0")
 *
 *---------------------------------------------------------------------------*/
void LCD_Kyocera_TM035NKH02_09_Create(const char *aName,
                            char* aSPIBus,
                            T_uezGPIOPortPin aSPICSPin)
{
    T_T55343GD035JU_3_5Workspace *p;

    T_uezDevice lcd;
    T_uezDeviceWorkspace *p_lcd;

    UEZDeviceTableFind(aName, &lcd);
    UEZDeviceTableGetWorkspace(lcd, &p_lcd);

    p = (T_T55343GD035JU_3_5Workspace *)p_lcd;

    strcpy(p->iSPIBus, aSPIBus);
    p->iCSGPIOPin = aSPICSPin;
}

/*---------------------------------------------------------------------------*
 * HAL Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LCD LCD_Kyocera_T55343GD035JU_3_5_Interface_16Bit = {
    {
        // Common interface
        "LCD:Kyocera_T55343GD035JU_3_5:16Bit",
        0x0100,
        LCD_T55343GD035JU_3_5_InitializeWorkspace_16Bit,
        sizeof(T_T55343GD035JU_3_5Workspace),
    },

    // Functions
    LCD_T55343GD035JU_3_5_Open,
    LCD_T55343GD035JU_3_5_Close,
    LCD_T55343GD035JU_3_5_Configure,
    LCD_T55343GD035JU_3_5_GetInfo,
    LCD_T55343GD035JU_3_5_GetFrame,
    LCD_T55343GD035JU_3_5_ShowFrame,
    LCD_T55343GD035JU_3_5_On,
    LCD_T55343GD035JU_3_5_Off,
    LCD_T55343GD035JU_3_5_SetBacklightLevel,
    LCD_T55343GD035JU_3_5_GetBacklightLevel,
    LCD_T55343GD035JU_3_5_SetPaletteColor,

    // v2.04
    LCD_T55343GD035JU_3_5_WaitForVerticalSync,
};

const DEVICE_LCD LCD_Kyocera_T55343GD035JU_3_5_Interface_I15Bit = {
    {
        // Common interface
        "LCD:Kyocera_T55343GD035JU_3_5:I15Bit",
        0x0100,
        LCD_T55343GD035JU_3_5_InitializeWorkspace_I15Bit,
        sizeof(T_T55343GD035JU_3_5Workspace),
    },

    // Functions
    LCD_T55343GD035JU_3_5_Open,
    LCD_T55343GD035JU_3_5_Close,
    LCD_T55343GD035JU_3_5_Configure,
    LCD_T55343GD035JU_3_5_GetInfo,
    LCD_T55343GD035JU_3_5_GetFrame,
    LCD_T55343GD035JU_3_5_ShowFrame,
    LCD_T55343GD035JU_3_5_On,
    LCD_T55343GD035JU_3_5_Off,
    LCD_T55343GD035JU_3_5_SetBacklightLevel,
    LCD_T55343GD035JU_3_5_GetBacklightLevel,
    LCD_T55343GD035JU_3_5_SetPaletteColor,

    // v2.04
    LCD_T55343GD035JU_3_5_WaitForVerticalSync,
};

const DEVICE_LCD LCD_Kyocera_T55343GD035JU_3_5_Interface_8Bit = {
    {
        // Common interface
        "LCD:Kyocera_T55343GD035JU_3_5:8Bit",
        0x0100,
        LCD_T55343GD035JU_3_5_InitializeWorkspace_8Bit,
        sizeof(T_T55343GD035JU_3_5Workspace),
    },

    // Functions
    LCD_T55343GD035JU_3_5_Open,
    LCD_T55343GD035JU_3_5_Close,
    LCD_T55343GD035JU_3_5_Configure,
    LCD_T55343GD035JU_3_5_GetInfo,
    LCD_T55343GD035JU_3_5_GetFrame,
    LCD_T55343GD035JU_3_5_ShowFrame,
    LCD_T55343GD035JU_3_5_On,
    LCD_T55343GD035JU_3_5_Off,
    LCD_T55343GD035JU_3_5_SetBacklightLevel,
    LCD_T55343GD035JU_3_5_GetBacklightLevel,
    LCD_T55343GD035JU_3_5_SetPaletteColor,

    // v2.04
    LCD_T55343GD035JU_3_5_WaitForVerticalSync,
};

// List of interfaces
const T_uezDeviceInterface *Kyocera_T55343GD035JU_3_5_InterfaceArray[] = {
    0,      // 1 bit
    0,      // 2 bit
    0,      // 4 bit
    (T_uezDeviceInterface *)&LCD_Kyocera_T55343GD035JU_3_5_Interface_8Bit,      // 8 bit
    (T_uezDeviceInterface *)&LCD_Kyocera_T55343GD035JU_3_5_Interface_16Bit,      // 16 bit
    (T_uezDeviceInterface *)&LCD_Kyocera_T55343GD035JU_3_5_Interface_I15Bit,      // I15 bit
    0,      // 24 bit
};

/*-------------------------------------------------------------------------*
 * End of File:  Kyocera_T55343GD035JU_3_5.c
 *-------------------------------------------------------------------------*/
