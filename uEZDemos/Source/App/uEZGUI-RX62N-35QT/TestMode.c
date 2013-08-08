/*-------------------------------------------------------------------------*
 * File:  TestMode.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Present calibration screen and do it.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * This software and associated documentation files (the "Software")
 * are copyright 2008 Future Designs, Inc. All Rights Reserved.
 *
 * A copyright license is hereby granted for redistribution and use of
 * the Software in source and binary forms, with or without modification,
 * provided that the following conditions are met:
 * 	-   Redistributions of source code must retain the above copyright
 *      notice, this copyright license and the following disclaimer.
 * 	-   Redistributions in binary form must reproduce the above copyright
 *      notice, this copyright license and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * 	-   Neither the name of Future Designs, Inc. nor the names of its
 *      subsidiaries may be used to endorse or promote products
 *      derived from the Software without specific prior written permission.
 *
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * 	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * 	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * 	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * 	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * 	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * 	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * 	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * 	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * 	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * 	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-------------------------------------------------------------------------*/

#include <uEZ.h>
#include <uEZTS.h>
#include "AppDemo.h"
#if UEZ_ENABLE_LCD
#include "NVSettings.h"
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include <Source/App/UEZDemoCommon/Calibration.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>

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
    static const T_uezTSReading G_expectedReadings[5] = {
        { 0,    0x1997,                     0x0818,                     1 }, // 0
        { 0,    0x05E9,                     0x0843,                     1 }, // 1
        { 0,    0x19AA,                     0x17CD,                     1 }, // 2
        { 0,    0x05D4,                     0x17EE,                     1 }, // 3
        { 0,    0x0FD4,                     0x0FFE,                     1 }, // 4
    };

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

		G_nonvolatileSettings.iNeedRecalibrate = EFalse;
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
        swim_set_pen_color(aWin, (y<<11));
	    for (x=0; x<256; x++, i++)  {
            swim_put_pixel(aWin,
                    x+20,
                    y+40);
        }
    }
    for (y=1; y<32; y++)  {
        swim_set_pen_color(aWin, (y<<5));
	    for (x=0; x<256; x++, i++)  {
            swim_put_pixel(aWin,
                    x+20,
                    y+40+32);
        }
    }
    for (y=1; y<32; y++)  {
        swim_set_pen_color(aWin, (y<<0));
	    for (x=0; x<256; x++, i++)  {
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
    //G_mmTestMode = ETrue;
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
