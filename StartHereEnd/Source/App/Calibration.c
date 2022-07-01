/*-------------------------------------------------------------------------*
 * File:  Calibration.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Present calibration screen and do it.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
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
#include <uEZTS.h>
#include <uEZLCD.h>
#include "NVSettings.h"
#include "Calibration.h"
#include <UEZLCD.h>

#ifndef TP_INSET
    #define TP_INSET        50
#endif

typedef enum {
    FIRST_TARGET,
    FIRST_TARGET_RETRY
} CalibrationState;

CalibrationState G_calibrateState;

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
 * Routine:  CalibrateScreen
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the screen used for calibration.
 * Inputs:
 *      TUInt16 *aPixels            -- Pointer to pixel memory
 *      TUInt16 aX, aY              -- Coordinate of target to draw
 *---------------------------------------------------------------------------*/
void CalibrateScreen()
{
  T_uezDevice lcd;
  T_pixelColor *pixels;
  SWIM_WINDOW_T win;
  TUInt16 fontHeight;

  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
    UEZLCDOn(lcd);
    UEZLCDBacklight(lcd, 256);

    SUIHidePage0();

    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
    swim_window_open(
                     &win,
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
    swim_set_font(&win, &APP_DEMO_DEFAULT_FONT);
    swim_set_title(&win, "Touchscreen Calibration", BLUE);
    fontHeight =  swim_get_font_height(&win);

    switch(G_calibrateState)
    {
    case FIRST_TARGET_RETRY:
      swim_put_text_horizontal_centered(&win,
                       "Calibration error, please try again.",
                       DISPLAY_WIDTH,
                       (fontHeight));
      swim_put_text_horizontal_centered(&win,
                       "Accurately press the center of the",
                       DISPLAY_WIDTH,
                       (DISPLAY_HEIGHT-(4*fontHeight)));
      swim_put_text_horizontal_centered(&win,
                       "red and white target shown.",
                       DISPLAY_WIDTH,
                       (DISPLAY_HEIGHT-(3*fontHeight)));
      break;
    case FIRST_TARGET:
    default:
      swim_put_text_horizontal_centered(&win,
                       "Before using this device, the touch",
                       DISPLAY_WIDTH,
                       (fontHeight));
            swim_put_text_horizontal_centered(&win,
                       "screen must be properly calibrated.",
                       DISPLAY_WIDTH,
                       (2*fontHeight));

            swim_put_text_horizontal_centered(&win,
                       "Accurately press the center of the",
                       DISPLAY_WIDTH,
                       (DISPLAY_HEIGHT-(4*fontHeight)));

            swim_put_text_horizontal_centered(&win,
                       "red and white target shown.",
                       DISPLAY_WIDTH,
                       (DISPLAY_HEIGHT-(3*fontHeight)));

      }
  SUIShowPage0();
  }
}

/*---------------------------------------------------------------------------*
 * Routine:  CalibrateProcedure
 *---------------------------------------------------------------------------*
 * Description:
 *      Step the user through the calibration process.  A target is shown
 *      at each of 3 locations and waits for the user to touch the
 *      target.
 * Inputs:
 *      TUInt16 *aPixels            -- Pointer to pixel memory
 *      TUInt16 aX, aY              -- Coordinate of target to draw
 *---------------------------------------------------------------------------*/
void CalibrateProcedure(
                T_pixelColor *aPixels,
                T_uezDevice ts,
                TBool aForceCalibrate)
{
    static const T_uezTSReading G_expectedReadings[5] = {
            { 0,    0+TP_INSET,                 TP_INSET,                   1 }, // 0
            { 0,    DISPLAY_WIDTH-TP_INSET,     TP_INSET,                   1 }, // 1
            { 0,    0+TP_INSET,                 DISPLAY_HEIGHT-TP_INSET,    1 }, // 2
#if 0
            #if UEZ_DEFAULT_LCD_RES_QVGA
        { 0, 50, 50, 1 },
        { 0, 250, 50, 1 },
        { 0, 50, 150, 1 }
    #endif

    #if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
        { 0, 100, 100, 1 },
        { 0, 500, 100, 1 },
        { 0, 100, 350, 1 }
    #endif

    #if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
        { 0, 100, 60, 1 },
        { 0, 380, 60, 1 },
        { 0, 100, 212, 1 }
    #endif
#endif
        };
    static const T_uezTSReading validateExpected[2] = {
            { 0,    DISPLAY_WIDTH-TP_INSET,     DISPLAY_HEIGHT-TP_INSET,    1 },
            { 0,    (DISPLAY_WIDTH/2), (DISPLAY_HEIGHT/2), 1},
    };

    T_uezTSReading reading;
    T_uezTSReading sum;
    T_uezTSReading validate[2];

    TUInt32 count;
    TInt32 i;
    TBool needCalib = ETrue;
    T_uezError error;
    TBool invalid = ETrue;
    TBool sawNoTouch;
    G_calibrateState = FIRST_TARGET;

    while (1) {
        // Put the touch screen TInt32o calibration mode
        UEZTSCalibrationStart(ts);

        if (!aForceCalibrate) {
                // Should have the number of poTInt32s equal to '3' if successful
                if (G_nonvolatileSettings.iNum == 3) {
                    // Feed in the data poTInt32s
                    for (i=0; i<3; i++) {
                        UEZTSCalibrationAddReading(
                            ts,
                            (const T_uezTSReading *)&G_nonvolatileSettings.iReadings[i],
                            &G_expectedReadings[i]);
                    }
                    needCalib = EFalse;
                    invalid = EFalse;
                }
        }

        if (needCalib) {
            // Clear the screen
            CalibrateScreen();



            // Show 3 different targets
            for (i=0; i<3; i++)  {
                // Make sure we see the screen is not being touched
                // when we start.  This ensures if the unit is being held at power
                // up that we don't go TInt32o calibration and get a bogus
                // starting reading.
                sawNoTouch = EFalse;

                // Show target to touch
                IDrawTarget(aPixels, G_expectedReadings[i].iX, G_expectedReadings[i].iY);

                // Wait for the pen to touch
                for (;;) {
                    sum.iX = 0;
                    sum.iY = 0;
                    for (count=0; count<4; ) {
                        if (UEZTSGetReading(ts, &reading)==UEZ_ERROR_NONE)  {
#if APP_MENU_ALLOW_TEST_MODE
                            if (G_mmTestMode == ETrue) {
                                // Use junk data to finish out the calibration mode
                                reading.iX = 0;
                                reading.iY = 0;
                                count++;
                            } else
#endif
			if (reading.iFlags & TSFLAG_PEN_DOWN)  {
                                // Only accept reading if we originally
                                // were not touching the screen.
                                if (sawNoTouch) {
                                    sum.iX += reading.iX;
                                    sum.iY += reading.iY;
                                    count++;
                                }
                            } else {
                                sawNoTouch = ETrue;
                            }
                        }
                        UEZTaskDelay(50);
                    }
                    reading.iX = sum.iX/count;
                    reading.iY = sum.iY/count;
                    UEZTSCalibrationAddReading(ts, &reading, &G_expectedReadings[i]);
                    G_nonvolatileSettings.iReadings[i] = reading;
                    G_nonvolatileSettings.iNum = i+1;
                    //ButtonClick();
                    break;
                }

                // Erase target to signal the user touched the target
                IEraseTarget(aPixels, G_expectedReadings[i].iX, G_expectedReadings[i].iY);
                if(i == 0)
                {
                  CalibrateScreen();
                }

                // Wait for the pen to lift
#if APP_MENU_ALLOW_TEST_MODE
                for (;G_mmTestMode == EFalse;) {
#else
                for (;;) {
#endif
                    if (UEZTSGetReading(ts, &reading)==UEZ_ERROR_NONE)  {
                        if (!(reading.iFlags & TSFLAG_PEN_DOWN))  {
                            break;
                        }
                    }
                    UEZTaskDelay(50);
                }
            }
        }

        // Calibration is complete.  Put the new calibration TInt32o effect.
        error = UEZTSCalibrationEnd(ts);
#if APP_MENU_ALLOW_TEST_MODE
        if (G_mmTestMode == EFalse) {
#endif
            //Check that the last two poTInt32s read in the expected range.
            if (needCalib) {
                for(i = 0; i < 2; i++){
                    IDrawTarget(aPixels, validateExpected[i].iX, validateExpected[i].iY);
                    for(;;){
                        if(UEZTSGetReading(ts, &validate[i]) == UEZ_ERROR_NONE){
                            if ((validate[i].iFlags & TSFLAG_PEN_DOWN)){
                                //ButtonClick();
                                break;
                            }
                        }
                    }
                    IEraseTarget(aPixels, validateExpected[i].iX, validateExpected[i].iY);

                    for (;;) {
                        if (UEZTSGetReading(ts, &reading)==UEZ_ERROR_NONE)  {
                            if (!(reading.iFlags & TSFLAG_PEN_DOWN))  {
                                break;
                            }
                        }
                        UEZTaskDelay(50);
                    }
                    UEZTaskDelay(300);
                }

                //check that the last two poTInt32s match the expected readings
                if((validate[0].iX > (validateExpected[0].iX - (TP_INSET/2))) && (validate[0].iX < (validateExpected[0].iX + (TP_INSET/2)))){
                    if((validate[0].iY > (validateExpected[0].iY - (TP_INSET/2))) && (validate[0].iY < (validateExpected[0].iY + (TP_INSET/2)))){
                        if((validate[1].iX > (validateExpected[1].iX - (TP_INSET/2))) && (validate[1].iX < (validateExpected[1].iX + (TP_INSET/2)))){
                            if((validate[1].iY > (validateExpected[1].iY - (TP_INSET/2))) && (validate[1].iY < (validateExpected[1].iY + (TP_INSET/2)))){
                                invalid = EFalse;
                            }
                        }
                    }
                }
            }
#if APP_MENU_ALLOW_TEST_MODE
        }else{
            invalid = EFalse;
            }
#endif

#if APP_MENU_ALLOW_TEST_MODE
        // Stop if we are going TInt32o test mode (and don't save any settings)
        if (G_mmTestMode)
            break;
#endif

        // Redo the calibration on any error
        if (error || invalid) {
            needCalib = ETrue;
            aForceCalibrate = ETrue;
            G_calibrateState = FIRST_TARGET_RETRY;
            continue;
        }

        G_nonvolatileSettings.iNeedRecalibrate = EFalse;
        //NVSettingsSave();
        break;
    }
}

/*---------------------------------------------------------------------------*
 * Task:  Calibrate
 *---------------------------------------------------------------------------*
 * Description:
 *      Goes through the calibration procedure
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 *---------------------------------------------------------------------------*/
TUInt32 Calibrate(TBool aForceCalibrate)
{
    T_uezDevice ts;
    T_uezDevice lcd;
    T_pixelColor *pixels;
    static T_uezQueue queue = 0;

    // Setup queue to receive touchscreen events
#ifdef NO_DYNAMIC_MEMORY_ALLOC	
	if (NULL == queue)
	{
	  	if (UEZQueueCreate(1, sizeof(T_uezTSReading), &queue) != UEZ_ERROR_NONE)
		{
		  	queue = NULL;
		}
	}
	
    if (NULL != queue) {
		/* Register the queue so that the IAR Stateviewer Plugin knows about it. */
	  	UEZQueueAddToRegistry( queue, "Calibrate TS" );
#else
	if (UEZQueueCreate(1, sizeof(T_uezTSReading), &queue) == UEZ_ERROR_NONE) {
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                UEZLCDGetFrame(lcd, 0, (void **)&pixels);

                // Before we use the touchscreen, go through calibration
                if (G_nonvolatileSettings.iNeedRecalibrate)
                    aForceCalibrate = ETrue;
                CalibrateProcedure(pixels, ts, aForceCalibrate);
            }
            UEZTSClose(ts, queue);
        }
#ifndef NO_DYNAMIC_MEMORY_ALLOC	
        UEZQueueDelete(queue);
#endif
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Task:  CalibrateTestIfTouchscreenHeld
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if the touchscreen is being held for 250 ms
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 *---------------------------------------------------------------------------*/
TBool CalibrateTestIfTouchscreenHeld(void)
{
    T_uezDevice ts;
    T_uezTSReading reading;
    TBool sawPress = ETrue;
    TUInt32 count;

    // Open up the touchscreen and pass in the queue to receive events
    if (UEZTSOpen("Touchscreen", &ts, 0)==UEZ_ERROR_NONE)  {
        for (count=0; count<5; count++) {
            UEZTSGetReading(ts, &reading);
            if (!(reading.iFlags & TSFLAG_PEN_DOWN))  {
                sawPress = EFalse;
                break;
            }
            UEZTaskDelay(50);
        }
        UEZTSClose(ts, 0);
    }
    return sawPress;
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
void CalibrateMode(const T_choice *p_choice)
{
    Calibrate(ETrue);
}

/*-------------------------------------------------------------------------*
 * File:  Calibration.c
 *-------------------------------------------------------------------------*/
