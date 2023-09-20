/*-------------------------------------------------------------------------*
 * File:  Calibration.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Present calibration screen and do it.
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
#include <uEZLCD.h>
#include <uEZTS.h>
#include "AppDemo.h"
#if APP_MENU_ALLOW_TEST_MODE
#include "NVSettings.h"
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include <Calibration.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <uEZLCD.h>

static SWIM_WINDOW_T G_tmWin;
volatile TBool G_mmTestModeRunning = EFalse;
volatile TBool G_mmTestModeTouchscreenCalibrationValid;
volatile TBool G_mmTestModeTouchscreenCalibrationBusy = EFalse;
T_uezQueue G_mmTestModeQueue;
T_pixelColor G_mmTestModeColor;

/*---------------------------------------------------------------------------*
 * Routine:  IDrawTarget
 *---------------------------------------------------------------------------*
 * Description:
 *      Draws a white square within a red square called a 'target'.
 * Inputs:
 *      TUInt16 *aPixels            -- Pointer to pixel memory
 *      TUInt16 aX, aY              -- Coordinate of target to draw
 *---------------------------------------------------------------------------*/
static void IDrawTarget(T_pixelColor *aPixels, TUInt16 aX, TUInt16 aY)
{
    #if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
        SUIFillRect(aPixels, aX-8, aY-8, aX+8, aY+8, RED);
        SUIFillRect(aPixels, aX-6, aY-6, aX+6, aY+6, BLACK);
        SUIFillRect(aPixels, aX-4, aY-4, aX+4, aY+4, WHITE);
        SUIFillRect(aPixels, aX-2, aY-2, aX+2, aY+2, BLACK);
    #else
        SUIFillRect(aPixels, aX-4, aY-4, aX+4, aY+4, RED);
        SUIFillRect(aPixels, aX-3, aY-3, aX+3, aY+3, BLACK);
        SUIFillRect(aPixels, aX-2, aY-2, aX+2, aY+2, WHITE);
        SUIFillRect(aPixels, aX-1, aY-1, aX+1, aY+1, BLACK);
    #endif

}

/*---------------------------------------------------------------------------*
 * Routine:  IEraseTarget
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase a previously drawn target
 * Inputs:
 *      TUInt16 *aPixels            -- Pointer to pixel memory
 *      TUInt16 aX, aY              -- Coordinate of target to draw
 *---------------------------------------------------------------------------*/
static void IEraseTarget(T_pixelColor *aPixels, TUInt16 aX, TUInt16 aY)
{
    #if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
        SUIFillRect(aPixels, aX-8, aY-8, aX+8, aY+8, BLACK);
    #else
        SUIFillRect(aPixels, aX-4, aY-4, aX+4, aY+4, BLACK);
    #endif
}

/*---------------------------------------------------------------------------*
 * Routine:  TestModeScreen
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the screen used for test mode.
 *---------------------------------------------------------------------------*/
void TestModeScreen(void)
{
    T_uezDevice lcd;
    T_pixelColor *pixels;

    if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
        UEZLCDOn(lcd);
        UEZLCDBacklight(lcd, 256);

        SUIHidePage0();

        UEZLCDGetFrame(lcd, 0, (void **)&pixels);
        swim_window_open(
            &G_tmWin,
            DISPLAY_WIDTH,
            DISPLAY_HEIGHT,
            pixels,
            0,
            0,
            DISPLAY_WIDTH-1,
            DISPLAY_HEIGHT-1,
            2,
            YELLOW,
            RGB(0, 0, 0),
            RED);
        swim_set_font(&G_tmWin, &APP_DEMO_DEFAULT_FONT);
        swim_set_title(&G_tmWin, PROJECT_NAME " - Test Mode", BLUE);
        SUIShowPage0();
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  TestModeTouchscreenProcedure
 *---------------------------------------------------------------------------*
 * Description:
 *      Step the user through the calibration process.  A target is shown
 *      at each of 3 locations and waits for the user to touch the
 *      target.
 * Inputs:
 *      TUInt16 *aPixels            -- Pointer to pixel memory
 *      TUInt16 aX, aY              -- Coordinate of target to draw
 *---------------------------------------------------------------------------*/
void TestModeTouchscreenProcedure(
                T_pixelColor *aPixels,
                T_uezDevice ts)
{
    static const T_uezTSReading G_coord[5] = {
        { 0,    0+TP_INSET,                 TP_INSET,                   1 }, // 0
        { 0,    DISPLAY_WIDTH-TP_INSET,     TP_INSET,                   1 }, // 1
        { 0,    0+TP_INSET,                 DISPLAY_HEIGHT-TP_INSET,    1 }, // 2
        { 0,    DISPLAY_WIDTH-TP_INSET,     DISPLAY_HEIGHT-TP_INSET,    1 }, // 3
        { 0,    DISPLAY_WIDTH/2,            DISPLAY_HEIGHT/2,           1 }, // 4
    };

    // List of expected results (based on a single screen)
#if ((UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T3DG01) || (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T1DG28))
    static const T_uezTSReading G_expectedReadings[5] = {
        { 0,    0x13f6,                     0x58ec,                     1 }, // 0
        { 0,    0x6aa3,                     0x58f0,                     1 }, // 1
        { 0,    0x12d8,                     0x22cd,                     1 }, // 2
        { 0,    0x6616,                     0x1f62,                     1 }, // 3
        { 0,    0x3f47,                     0x3de0,                     1 }, // 4
    };
#elif (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
    static const T_uezTSReading G_expectedReadings[5] = {
        { 0,    0x6D64,                     0x62F8,                     1 }, // 0
        { 0,    0x0F3A,                     0x63D7,                     1 }, // 1
        { 0,    0x6BFD,                     0x1B4B,                     1 }, // 2
        { 0,    0x0F6D,                     0x1880,                     1 }, // 3
        { 0,    0x3CC8,                     0x3C98,                     1 }, // 4
    };
#elif ((UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SEIKO_70WVW2T))
    static const T_uezTSReading G_expectedReadings[5] = {
        { 0,    0x6F1C,                     0x620D,                     1 }, // 0
        { 0,    0x118B,                     0x628A,                     1 }, // 1
        { 0,    0x6F01,                     0x1BF4,                     1 }, // 2
        { 0,    0x11D7,                     0x1BFB,                     1 }, // 3
        { 0,    0x4036,                     0x3F62,                     1 }, // 4
    };
#elif (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_TIANMA_TM070RBHG04)
        static const T_uezTSReading G_expectedReadings[5] = {
        { 0,    0x0C04,                     0x204C,                     1 }, // 0
        { 0,    0x6F2A,                     0x2134,                     1 }, // 1
        { 0,    0x0C58,                     0x6012,                     1 }, // 2
        { 0,    0x6F2C,                     0x5DBA,                     1 }, // 3
        { 0,    0x3C6A,                     0x4098,                     1 }, // 4
    };
#elif (UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_MICROTIPS_UMSH_8596MD_20T)
            static const T_uezTSReading G_expectedReadings[5] = {
        { 0, 50, 50, 1 },
        { 0, 760, 50, 1 },
        { 0, 50, 460, 1 },
        { 0, 750, 460, 1 },
        { 0, 480, 240, 1 },
    };
#elif (UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_NEWHAVEN_NHD43480272MF | UEZ_DEFAULT_LCD_CONFIG == LCD_CONFIG_NEWHAVEN_NHD50800480TF)
       static const T_uezTSReading G_expectedReadings[5] = {
       { 0,    0+TP_INSET,                 TP_INSET,                   1 }, // 0
       { 0,    DISPLAY_WIDTH-TP_INSET,     TP_INSET,                   1 }, // 1
       { 0,    0+TP_INSET,                 DISPLAY_HEIGHT-TP_INSET,    1 }, // 2
       { 0,    DISPLAY_WIDTH-TP_INSET,     DISPLAY_HEIGHT-TP_INSET,    1 }, // 3
       { 0,    DISPLAY_WIDTH/2,            DISPLAY_HEIGHT/2,           1 }, // 4
       };
#endif

    // The raw X range is 0x560A (left) to 0x2908 (right)
    //      100% = 0x2D02
    //       20% = 0x0900
    //       10% = 0x0480
    // The raw Y range is 0x5A7A (top) to 0x24A1 (bottom)
    //      100% = 0x35D9
    //       20% = 0x0AC5
    //       10% = 0x0562
    #define READING_X_TOLERANCE         0x0900
    #define READING_Y_TOLERANCE         0x0AC5

    T_uezTSReading readings[5];
    T_uezTSReading reading;
    T_uezTSReading sum;
    TUInt32 count;
    int i;
    T_uezError error;
    TBool sawNoTouch;
    TUInt32 timeoutCount=0;
    TUInt32 timeoutStart;

    G_mmTestModeTouchscreenCalibrationValid = EFalse;
    while (1) {
        // Put the touch screen into calibration mode
        UEZTSCalibrationStart(ts);

        // Clear the screen
        //CalibrateScreen();

        // Show 5 different targets
        timeoutStart = UEZTickCounterGet();
        for (i=0; i<5; i++)  {
            // Make sure we see the screen is not being touched
            // when we start.  This ensures if the unit is being held at power
            // up that we don't go into calibration and get a bogus
            // starting reading.
            sawNoTouch = EFalse;

            // Show target to touch
            IDrawTarget(aPixels, G_coord[i].iX, G_coord[i].iY);

            // Wait for the pen to touch (while not in test mode)
            for (;;) {
                sum.iX = 0;
                sum.iY = 0;

                for (count=0; count<4; ) {
                    if (UEZTSGetReading(ts, &reading)==UEZ_ERROR_NONE)  {
                        if (reading.iFlags & TSFLAG_PEN_DOWN)  {
                            // Only accept reading if we originally
                            // were not touching the screen.
                            if (sawNoTouch) {
                                sum.iX += reading.iX;
                                sum.iY += reading.iY;
                                count++;
                                timeoutStart = UEZTickCounterGet();
                                timeoutCount = 0;
                            }
                        } else {
                            sawNoTouch = ETrue;
                            // Reset the timeout
                            timeoutStart = UEZTickCounterGet();
                            timeoutCount = 0;
                        }
                    }
                    if (!sawNoTouch) {
                        timeoutCount = UEZTickCounterGet() - timeoutStart;
                        if (timeoutCount > 10000)
                            break;
                    }
                    UEZTaskDelay(50);
                }
                reading.iX = sum.iX/count;
                reading.iY = sum.iY/count;

                // Remember this reading
                readings[i] = reading;

                if (i < 3) {
                    // If one of the first 3 points, store the data
                    UEZTSCalibrationAddReading(ts, &reading, &G_coord[i]);
                    G_nonvolatileSettings.iReadings[i] = reading;
                    G_nonvolatileSettings.iNum = i+1;
                }
                ButtonClick();
                break;
            }

            // Erase target to signal the user touched the target
            IEraseTarget(aPixels, G_coord[i].iX, G_coord[i].iY);

            if (timeoutCount < 10000) {
                // Wait for the pen to lift
                timeoutCount = 0;
                for (;;) {
                    if (UEZTSGetReading(ts, &reading)==UEZ_ERROR_NONE)  {
                        if (!(reading.iFlags & TSFLAG_PEN_DOWN))  {
                            break;
                        }
                    }
                    UEZTaskDelay(50);
                    timeoutCount += 50;
                    if (timeoutCount >= 10000)
                        break;
                }
            }
        }

        // Calibration is complete.  Put the new calibration into effect.
        error = UEZTSCalibrationEnd(ts);
        if (timeoutCount >= 10000)
            error = UEZ_ERROR_TIMEOUT;


        // Determine if this is valid or not
        if (error) {
            G_mmTestModeTouchscreenCalibrationValid = EFalse;
        } else {
            // Check the validity of the readings
            G_mmTestModeTouchscreenCalibrationValid = ETrue;
            for (i=0; i<5; i++) {
                if ((readings[i].iX < G_expectedReadings[i].iX-READING_X_TOLERANCE) ||
                        (readings[i].iX > G_expectedReadings[i].iX+READING_X_TOLERANCE)) {
                    // Outside along X
                    G_mmTestModeTouchscreenCalibrationValid = EFalse;
                    break;
                }
                if ((readings[i].iY < G_expectedReadings[i].iY-READING_Y_TOLERANCE) ||
                        (readings[i].iY > G_expectedReadings[i].iY+READING_Y_TOLERANCE)) {
                    // Outside along Y
                    G_mmTestModeTouchscreenCalibrationValid = EFalse;
                    break;
                }
            }
        }

        NVSettingsSave();
        break;
    }
}

static void IClearScreen(T_uezDevice aLCD)
{
    T_pixelColor *pixels;

    UEZLCDGetFrame(aLCD, 0, (void **)&pixels);
    swim_window_open(
        &G_tmWin,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        pixels,
        0,
        0,
        DISPLAY_WIDTH-1,
        DISPLAY_HEIGHT-1,
        2,
        YELLOW,
        RGB(0, 0, 0),
        RED);
    swim_set_font(&G_tmWin, &APP_DEMO_DEFAULT_FONT);
    swim_set_title(&G_tmWin, "uEZ GUI Test Mode", BLUE);
}

static void IClearScreen0(T_uezDevice aLCD)
{
    T_pixelColor *pixels;

    UEZLCDGetFrame(aLCD, 0, (void **)&pixels);
    swim_window_open(
        &G_tmWin,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        pixels,
        0,
        0,
        DISPLAY_WIDTH-1,
        DISPLAY_HEIGHT-1,
        0,
        YELLOW,
        G_mmTestModeColor,
        RED);
    swim_set_font(&G_tmWin, &APP_DEMO_DEFAULT_FONT);
}

static void ITestPatternColors(SWIM_WINDOW_T *aWin)
{
    int x, y;
    TUInt16 i=0;

    for (y=1; y<32; y++)  {
        for (x=0; x<256; x++, i++)  {
#if (UEZ_LCD_DEFAULT_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_16_BIT)
            swim_set_pen_color(aWin, (y<<11));
#elif  (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
            swim_set_pen_color(aWin, (y<<10)+((x&128)?0x8000:0));
#elif (UEZ_LCD_DEFAULT_COLOR_DEPTH==UEZLCD_COLOR_DEPTH_I15_BIT)
            swim_set_pen_color(aWin, (y<<10));
#else
            swim_set_pen_color(aWin, (y<<11));
#endif
            swim_put_pixel(aWin,
                    x+20,
                    y+40);
        }
    }
    for (y=1; y<32; y++)  {
        for (x=0; x<256; x++, i++)  {
#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T3DG01)
            swim_set_pen_color(aWin, (y<<6));
#elif  (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
            swim_set_pen_color(aWin, (y<<5)+((x&128)?0x8000:0));
#else
            swim_set_pen_color(aWin, (y<<5));
#endif
            swim_put_pixel(aWin,
                    x+20,
                    y+40+32);
        }
    }
    for (y=1; y<32; y++)  {
        for (x=0; x<256; x++, i++)  {
#if (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_SHARP_LQ043T3DG01)
            swim_set_pen_color(aWin, (y<<0));
#elif  (UEZ_DEFAULT_LCD_CONFIG==LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1)
            swim_set_pen_color(aWin, (y<<0)+((x&128)?0x8000:0));
#else
            swim_set_pen_color(aWin, (y<<0));
#endif
            swim_put_pixel(aWin,
                    x+20,
                    y+40+64);
        }
    }
}


void TestModeLCD(T_uezDevice aLCD)
{
    IClearScreen(aLCD);
    ITestPatternColors(&G_tmWin);
}

void TestModeFillColor(T_uezDevice aLCD)
{
    IClearScreen0(aLCD);
}

void TestModeAlignmentBorder(T_uezDevice aLCD)
{
    T_pixelColor *pixels;

    UEZLCDGetFrame(aLCD, 0, (void **)&pixels);
    swim_window_open(
        &G_tmWin,
        DISPLAY_WIDTH,
        DISPLAY_HEIGHT,
        pixels,
        0,
        0,
        DISPLAY_WIDTH-1,
        DISPLAY_HEIGHT-1,
        2,
        WHITE,
        BLACK,
        RED);
    swim_set_font(&G_tmWin, &APP_DEMO_DEFAULT_FONT);
}

void TestModeTouchscreen(T_uezDevice aLCD, T_uezDevice aTS, T_uezQueue aQueue)
{
    T_pixelColor *pixels;
    T_uezInputEvent inputEvent;

    // We are busy doing calibration
    G_mmTestModeTouchscreenCalibrationBusy = ETrue;

    // Flush the queue by reading all the events up til this point
    while(UEZQueueReceive(aQueue, &inputEvent, 0) == UEZ_ERROR_NONE)
        {}

    UEZLCDGetFrame(aLCD, 0, (void **)&pixels);
    G_mmTestModeColor = RGB(0, 0, 0);
    IClearScreen0(aLCD);
    TestModeTouchscreenProcedure(pixels, aTS);
    IClearScreen(aLCD);

    // Flush the queue by reading all the events up til this point
    while(UEZQueueReceive(aQueue, &inputEvent, 0) == UEZ_ERROR_NONE)
        {}

    // Done doing calibration
    G_mmTestModeTouchscreenCalibrationBusy = EFalse;
}

/*---------------------------------------------------------------------------*
 * Task:  CalibrateMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the device in forced calibration mode since the user chose
 *      this choice.
 * Inputs:
 *      const T_choice *p_choice    -- Choice activated for this mode
 *---------------------------------------------------------------------------*/
void TestMode(void)
{
    T_uezDevice ts;
    T_uezDevice lcd;
    T_pixelColor *pixels;
    T_uezQueue queue;
    TUInt32 cmd;

    if (UEZQueueCreate(1, sizeof(TUInt32), &G_mmTestModeQueue) == UEZ_ERROR_NONE) {
        // Setup queue to receive touchscreen events
        if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
            // Open up the touchscreen and pass in the queue to receive events
            if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
                // Open the LCD and get the pixel buffer
                if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
                    TestModeScreen();
                    G_mmTestModeRunning = ETrue;
                    while (1) {
                        UEZQueueReceive(G_mmTestModeQueue, &cmd, UEZ_TIMEOUT_INFINITE);
                        if (cmd == 0)  {
                            break;
                        } else {
                            switch (cmd) {
                                case TEST_MODE_TOUCHSCREEN:
                                    // Draw targets for test pattern
                                    TestModeTouchscreen(lcd, ts, queue);
                                    break;
                                case TEST_MODE_LCD:
                                    // Draw test pattern
                                    TestModeLCD(lcd);
                                    break;
                                case TEST_MODE_PING:
                                    // Ping does nothing
                                    break;
                                case TEST_MODE_FILL_COLOR:
                                    TestModeFillColor(lcd);
                                    break;
                                case TEST_MODE_ALIGNMENT_BORDER:
                                    TestModeAlignmentBorder(lcd);
                                    break;
                                default:
                                    break;
                            }
                        }
                    }
                }
                UEZTSClose(ts, queue);
            }
            UEZQueueDelete(queue);
        }
        G_mmTestModeRunning = EFalse;
        UEZQueueDelete(G_mmTestModeQueue);
    }
}

TBool TestModeSendCmd(TUInt32 aCmd)
{
    // Go into test mode (if not already running)
    G_mmTestMode = ETrue;
    while (!G_mmTestModeRunning) {
        UEZTaskDelay(10);
    }

    // Check if we are running and send command
    if (G_mmTestModeRunning) {
        if (UEZQueueSend(G_mmTestModeQueue, &aCmd, UEZ_TIMEOUT_INFINITE) != UEZ_ERROR_NONE)
            return EFalse;
        return ETrue;
    }
    return EFalse;
}

void TestModeGetWindow(SWIM_WINDOW_T *aWin)
{
    // Return a copy
    *aWin = G_tmWin;
}

#endif

/*-------------------------------------------------------------------------*
 * File:  Calibration.c
 *-------------------------------------------------------------------------*/
