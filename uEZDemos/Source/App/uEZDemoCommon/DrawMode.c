/*-------------------------------------------------------------------------*
 * File:  DrawMode
 *-------------------------------------------------------------------------*
 * Description:
 *      Example program that allows the user to draw, save, and load
 *      an image.
 *
 * Implementation:
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
#include <uEZFile.h>
#include <string.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include "AppDemo.h"
#include <uEZLCD.h>
#include <uEZKeypad.h>

#include <Types/InputEvent.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

#if UEZ_DEFAULT_LCD_RES_QVGA
    #define DR_COLUMN_1         17
    #define DR_COLUMN_2         98

    #define DR_BUTTON_HEIGHT    32
    #define DR_BUTTON_WIDTH     64
    #define DR_ROW_HEIGHT       52
    #define DR_ROWS_TOP         9

    #define DR_IMAGE_WIDTH      200
    #define DR_IMAGE_HEIGHT     200
    #define DR_IMAGE_LEFT       103
#endif

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
    #define DR_COLUMN_1         34
    #define DR_COLUMN_2         196

    #define DR_BUTTON_HEIGHT    64
    #define DR_BUTTON_WIDTH     128
    #define DR_ROW_HEIGHT       104
    #define DR_ROWS_TOP         18

    #define DR_IMAGE_WIDTH      400
    #define DR_IMAGE_HEIGHT     400
    #define DR_IMAGE_LEFT       206
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
    #define DR_COLUMN_1         17+80
    #define DR_COLUMN_2         98+80

    #define DR_BUTTON_HEIGHT    32
    #define DR_BUTTON_WIDTH     64
    #define DR_ROW_HEIGHT       52
    #define DR_ROWS_TOP         9+16

    #define DR_IMAGE_WIDTH      200
    #define DR_IMAGE_HEIGHT     200
    #define DR_IMAGE_LEFT       103+80
#endif


#define DR_ROW_1            (DR_ROWS_TOP+(DR_ROW_HEIGHT*0))
#define DR_ROW_2            (DR_ROWS_TOP+(DR_ROW_HEIGHT*1))
#define DR_ROW_3            (DR_ROWS_TOP+(DR_ROW_HEIGHT*2))
#define DR_ROW_4            (DR_ROWS_TOP+(DR_ROW_HEIGHT*3))

#define DR_IMAGE_RIGHT      (DR_IMAGE_LEFT+DR_IMAGE_WIDTH+1)
#define DR_IMAGE_TOP        DR_ROWS_TOP
#define DR_IMAGE_BOTTOM     (DR_IMAGE_TOP+DR_IMAGE_HEIGHT+1)

#define NUM_COLOR_CHOICES  6

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static void DR_ChangeColor(const T_choice *aChoice);
static void DR_Load(const T_choice *aChoice);
static void DR_Save(const T_choice *aChoice);
static void DR_Exit(const T_choice *aChoice);
static void DR_DrawColor(void);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static SWIM_WINDOW_T G_drWin;
static const T_choice G_drChoices[] = {
    { DR_COLUMN_1, DR_ROW_1, DR_COLUMN_1+DR_BUTTON_WIDTH-1, DR_ROW_1+DR_BUTTON_HEIGHT-1, "Color", DR_ChangeColor, 0},
    { DR_COLUMN_1, DR_ROW_2, DR_COLUMN_1+DR_BUTTON_WIDTH-1, DR_ROW_2+DR_BUTTON_HEIGHT-1, "Load", DR_Load, G_loadIcon },
    { DR_COLUMN_1, DR_ROW_3, DR_COLUMN_1+DR_BUTTON_WIDTH-1, DR_ROW_3+DR_BUTTON_HEIGHT-1, "Save", DR_Save, G_saveIcon },
    { DR_COLUMN_1, DR_ROW_4, DR_COLUMN_1+DR_BUTTON_WIDTH-1, DR_ROW_4+DR_BUTTON_HEIGHT-1, "Exit", DR_Exit, G_exitIcon },
    { 0, 0, 0, 0, 0 },
};
const T_pixelColor G_drColorTable[NUM_COLOR_CHOICES] = {
    WHITE,
    RED,
    BLUE,
    GREEN,
    YELLOW,
    BLACK
};
static TBool G_drExit;
static TUInt8 G_drColorIndex=0;
static T_pixelColor G_drColor=WHITE;

/*---------------------------------------------------------------------------*
 * Routine:  DR_Screen
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the DrawMode screen.
 * Inputs:
 *      T_uezDevice lcd           -- LCD screen to use.
 *---------------------------------------------------------------------------*/
void DR_Screen(T_uezDevice lcd)
{
    T_pixelColor *pixels;

    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
    SUIHidePage0();
    swim_window_open(
        &G_drWin,
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
    swim_set_font(&G_drWin, &APP_DEMO_DEFAULT_FONT);
    swim_set_title(&G_drWin, "uEZ(tm) Draw Demonstration", BLUE);
    swim_set_pen_color(&G_drWin, YELLOW);

    ChoicesDraw(&G_drWin, G_drChoices);

    // Now draw the drawing area
    swim_set_fill_color(
        &G_drWin,
        BLACK);
    swim_put_box(
        &G_drWin,
        DR_IMAGE_LEFT,
        DR_IMAGE_TOP,
        DR_IMAGE_RIGHT,
        DR_IMAGE_BOTTOM);

    SUIShowPage0();
}

/*---------------------------------------------------------------------------*
 * Routine:  DR_ChangeColor
 *---------------------------------------------------------------------------*
 * Description:
 *      Change the currently selected color
 * Inputs:
 *      const T_choice *aChoice   -- Choice object selected for this action.
 *---------------------------------------------------------------------------*/
static void DR_ChangeColor(const T_choice *aChoice)
{
    G_drColorIndex++;
    if (G_drColorIndex >= NUM_COLOR_CHOICES)
        G_drColorIndex = 0;
    G_drColor = G_drColorTable[G_drColorIndex];
    DR_DrawColor();
}

/*---------------------------------------------------------------------------*
 * Routine:  DR_Load
 *---------------------------------------------------------------------------*
 * Description:
 *      Load an image from the flash drive and show.
 * Inputs:
 *      const T_choice *aChoice   -- Choice object selected for this action.
 *---------------------------------------------------------------------------*/
static void DR_Load(const T_choice *aChoice)
{
    T_uezError error;
    T_uezError errorUSB, errorSD;
    T_uezFile file;
    INT_32 winX, winY;
    TUInt32 num;
    TUInt16 x, y;
    void *load = UEZMemAlloc(DR_IMAGE_WIDTH * DR_IMAGE_HEIGHT * 2);

    // Draw line around choice while loading
    swim_set_fill_transparent(&G_drWin, 1);
    swim_set_pen_color(&G_drWin, YELLOW);
    swim_put_box(
        &G_drWin,
        aChoice->iLeft,
        aChoice->iTop,
        aChoice->iRight,
        aChoice->iBottom);

    // Try to load from the USB drive
    error = errorUSB = UEZFileOpen("0:IMAGE.RAW", FILE_FLAG_READ_ONLY, &file);

    // If an error, try to load from the SD Card
    if (error != UEZ_ERROR_NONE)
        error = errorSD = UEZFileOpen("1:IMAGE.RAW", FILE_FLAG_READ_ONLY, &file);
    else
        errorSD = UEZ_ERROR_UNKNOWN;

    // Continue if no errors
    if (error == UEZ_ERROR_NONE) {
        error = UEZFileRead(
                    file,
                    load,
                    DR_IMAGE_WIDTH*DR_IMAGE_HEIGHT*sizeof(T_pixelColor),
                    &num);
        UEZFileClose(file);
        if ((error != UEZ_ERROR_NONE) &&
                (num != DR_IMAGE_WIDTH*DR_IMAGE_HEIGHT*sizeof(T_pixelColor))) {
            BeepError();
        } else {
            T_pixelColor *raster = (T_pixelColor *)load;
            for (y=0; y<DR_IMAGE_HEIGHT; y++) {
                for (x=0; x<DR_IMAGE_WIDTH; x++) {
                    winX = x + DR_IMAGE_LEFT+1;
                    winY = y + DR_IMAGE_TOP+1;
                    swim_get_physical_xy(&G_drWin, &winX, &winY);
                    swim_driver_put_pixel(&G_drWin, winX, winY, raster[x]);
                }
                raster += DR_IMAGE_WIDTH;
            }
        }
    }
    if (error) {
        // Had an error, report it
        SUICopyFast32(FRAME(1), FRAME(0), FRAME_SIZE);
        BeepError();
        swim_set_fill_transparent(&G_drWin, 0);
        swim_set_fill_color(&G_drWin, BLACK);
        swim_set_pen_color(&G_drWin, YELLOW);
        swim_put_box(
            &G_drWin,
            DR_IMAGE_LEFT,
            DR_IMAGE_TOP,
            DR_IMAGE_RIGHT,
            DR_IMAGE_TOP+16);
        if (errorUSB == UEZ_ERROR_NOT_FOUND) {
            if (errorSD == UEZ_ERROR_NOT_READY) {
                swim_put_text_xy(
                    &G_drWin,
                    "Image file not found on USB Drive!",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            } else if (errorSD == UEZ_ERROR_NOT_FOUND) {
                swim_put_text_xy(
                    &G_drWin,
#if UEZ_DEFAULT_LCD_RES_QVGA
                    // Shorter string on QVGA screens
                    "Image file not found on USB or SD Card!",
#else
                    "Image file not found on USB Drive or SD Card!",
#endif
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            } else {
                swim_put_text_xy(
                    &G_drWin,
                    "Image file not found on USB Drive!",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            }
        } else if (errorUSB == UEZ_ERROR_NOT_READY) {
            if (errorSD == UEZ_ERROR_NOT_READY) {
                swim_put_text_xy(
                    &G_drWin,
                    "No USB Drive or SD Card found!",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            } else if (errorSD == UEZ_ERROR_NOT_FOUND) {
                swim_put_text_xy(
                    &G_drWin,
                    "Image file not found on SD Card!",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            } else {
                swim_put_text_xy(
                    &G_drWin,
                    "No USB Drive or SD Card found!",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            }
        } else {
            swim_put_text_xy(
                &G_drWin,
                "Read error!",
                DR_IMAGE_LEFT+2,
                DR_IMAGE_TOP+2);
        }
        swim_set_fill_transparent(&G_drWin, 1);
        UEZTaskDelay(2000);
        SUICopyFast32(FRAME(0), FRAME(1), FRAME_SIZE);
    }

    swim_set_fill_transparent(&G_drWin, 1);
    swim_set_pen_color(&G_drWin, BLACK);
    swim_put_box(
        &G_drWin,
        aChoice->iLeft,
        aChoice->iTop,
        aChoice->iRight,
        aChoice->iBottom);
    UEZMemFree(load);
}

/*---------------------------------------------------------------------------*
 * Routine:  DR_Save
 *---------------------------------------------------------------------------*
 * Description:
 *      Save the currently shown image to the flash drive.
 * Inputs:
 *      const T_choice *aChoice   -- Choice object selected for this action.
 *---------------------------------------------------------------------------*/
static void DR_Save(const T_choice *aChoice)
{
    T_uezError error;
    T_uezError errorUSB, errorSD;
    T_uezFile file;
    INT_32 winX, winY;
    T_pixelColor *raster;
    TUInt32 num;
    TUInt16 x, y;

    void *load = UEZMemAlloc(DR_IMAGE_WIDTH * DR_IMAGE_HEIGHT * 2);
    // Draw line around choice while saving
    swim_set_fill_transparent(&G_drWin, 1);
    swim_set_pen_color(&G_drWin, YELLOW);
    swim_put_box(
        &G_drWin,
        aChoice->iLeft,
        aChoice->iTop,
        aChoice->iRight,
        aChoice->iBottom);

    // Try to save to the USB drive
    error = errorUSB = UEZFileOpen("0:IMAGE.RAW", FILE_FLAG_WRITE, &file);

    // If an error, try to save to the SD Card
    if (error != UEZ_ERROR_NONE)
        error = errorSD = UEZFileOpen("1:IMAGE.RAW", FILE_FLAG_WRITE, &file);
    else
        errorSD = UEZ_ERROR_UNKNOWN;

    // Any errors?
    if (error == UEZ_ERROR_NONE) {
        raster = (T_pixelColor  *)load;
        for (y=0; y<DR_IMAGE_HEIGHT; y++) {
            for (x=0; x<DR_IMAGE_WIDTH; x++) {
                winX = x + DR_IMAGE_LEFT+1;
                winY = y + DR_IMAGE_TOP+1;
                swim_get_physical_xy(&G_drWin, &winX, &winY);
                swim_driver_get_raster(&G_drWin, winX, winY, (COLOR_T *)raster, 1); // get pixel color
                raster++;
            }
        }
        // Save image
        error = UEZFileWrite(
                    file,
                    load,
                    DR_IMAGE_WIDTH*DR_IMAGE_HEIGHT*sizeof(T_pixelColor),
                    &num);
        if ((error != UEZ_ERROR_NONE) &&
                (num != DR_IMAGE_WIDTH*DR_IMAGE_HEIGHT*sizeof(T_pixelColor))) {
            BeepError();
        }
        UEZFileClose(file);
    } else {
        // Had an error, report it
        SUICopyFast32(FRAME(1), FRAME(0), FRAME_SIZE);
        BeepError();
        swim_set_fill_transparent(&G_drWin, 0);
        swim_set_fill_color(&G_drWin, BLACK);
        swim_set_pen_color(&G_drWin, YELLOW);
        swim_put_box(
            &G_drWin,
            DR_IMAGE_LEFT,
            DR_IMAGE_TOP,
            DR_IMAGE_RIGHT,
            DR_IMAGE_TOP+16);
        if (errorUSB == UEZ_ERROR_NOT_FOUND) {
            if (errorSD == UEZ_ERROR_NOT_READY) {
                swim_put_text_xy(
                    &G_drWin,
                    "Could not write image to USB Drive",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            } else if (errorSD == UEZ_ERROR_NOT_FOUND) {
                swim_put_text_xy(
                    &G_drWin,
                    "Could not write to USB Drive or SD Card",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            } else {
                swim_put_text_xy(
                    &G_drWin,
                    "Could not write to USB Drive",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            }
        } else if (errorUSB == UEZ_ERROR_NOT_READY) {
            if (errorSD == UEZ_ERROR_NOT_READY) {
                swim_put_text_xy(
                    &G_drWin,
                    "No USB Drive or SDCard found!",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            } else if (errorSD == UEZ_ERROR_NOT_FOUND) {
                swim_put_text_xy(
                    &G_drWin,
                    "Could not write to SD Card!",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            } else {
                swim_put_text_xy(
                    &G_drWin,
                    "Could not write to SD Card!",
                    DR_IMAGE_LEFT+2,
                    DR_IMAGE_TOP+2);
            }
        } else {
            swim_put_text_xy(
                &G_drWin,
                "Write error!",
                DR_IMAGE_LEFT+2,
                DR_IMAGE_TOP+2);
        }
        swim_set_fill_transparent(&G_drWin, 1);
        UEZTaskDelay(2000);
        SUICopyFast32(FRAME(0), FRAME(1), FRAME_SIZE);
    }

    swim_set_fill_transparent(&G_drWin, 1);
    swim_set_pen_color(&G_drWin, BLACK);
    swim_put_box(
        &G_drWin,
        aChoice->iLeft,
        aChoice->iTop,
        aChoice->iRight,
        aChoice->iBottom);
    UEZMemFree(load);
}

/*---------------------------------------------------------------------------*
 * Routine:  DR_Exit
 *---------------------------------------------------------------------------*
 * Description:
 *      Exit the draw mode.  Do this by setting a global flag.
 * Inputs:
 *      const T_choice *aChoice   -- Choice object selected for this action.
 *---------------------------------------------------------------------------*/
static void DR_Exit(const T_choice *aChoice)
{
    G_drExit = 1;
}

/*---------------------------------------------------------------------------*
 * Routine:  DR_DrawColor
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the currently selected color in the upper left.
 *---------------------------------------------------------------------------*/
static void DR_DrawColor(void)
{
    const T_choice * const p_lastChoice = &G_drChoices[0];

    swim_set_fill_color(
        &G_drWin,
        G_drColor);
    swim_set_pen_color(&G_drWin, YELLOW);
    swim_put_box(&G_drWin,
        p_lastChoice->iLeft,
        p_lastChoice->iTop,
        p_lastChoice->iRight,
        p_lastChoice->iBottom);
}

static void DrawPointInBox(TInt32 winX, TInt32 winY)
{
    // Draw point only if in bounds
    if ((winX > DR_IMAGE_LEFT) && (winX < DR_IMAGE_RIGHT) &&
            (winY > DR_IMAGE_TOP) && (winY < DR_IMAGE_BOTTOM)) {
        swim_put_pixel(&G_drWin, winX, winY);
    }
}

static void DrawBlockPixel(TInt32 winX, TInt32 winY)
{
    DrawPointInBox(winX-1, winY-1);
    DrawPointInBox(winX-1, winY+0);
    DrawPointInBox(winX-1, winY+1);
    DrawPointInBox(winX+0, winY-1);
    DrawPointInBox(winX+0, winY+0);
    DrawPointInBox(winX+0, winY+1);
    DrawPointInBox(winX+1, winY-1);
    DrawPointInBox(winX+1, winY+0);
    DrawPointInBox(winX+1, winY+1);
}

static void DrawLineInBox(TInt32 startX, TInt32 startY, TInt32 winX, TInt32 winY)
{
    // Draw point only if in bounds
    if ((startX > DR_IMAGE_LEFT) && (startX < DR_IMAGE_RIGHT) &&
            (startY > DR_IMAGE_TOP) && (startY < DR_IMAGE_BOTTOM)) {
        if ((winX > DR_IMAGE_LEFT) && (winX < DR_IMAGE_RIGHT) &&
                (winY > DR_IMAGE_TOP) && (winY < DR_IMAGE_BOTTOM)) {
            swim_put_line(&G_drWin, startX, startY, winX, winY);
        }
    }
}

static void DrawBlockLine(TInt32 startX, TInt32 startY, TInt32 winX, TInt32 winY)
{
    DrawLineInBox(startX-1, startY-1, winX-1, winY-1);
    DrawLineInBox(startX-1, startY+0, winX-1, winY+0);
    DrawLineInBox(startX-1, startY+1, winX-1, winY+1);
    DrawLineInBox(startX+0, startY-1, winX+0, winY-1);
    DrawLineInBox(startX+0, startY+0, winX+0, winY+0);
    DrawLineInBox(startX+0, startY+1, winX+0, winY+1);
    DrawLineInBox(startX+1, startY-1, winX+1, winY-1);
    DrawLineInBox(startX+1, startY+0, winX+1, winY+0);
    DrawLineInBox(startX+1, startY+1, winX+1, winY+1);
}

/*---------------------------------------------------------------------------*
 * Routine:  DrawMode
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the processor in the draw mode 'application' or 'demo'.
 *      Buttons are shown to load, save, exit, and change the current color.
 *      The user can then draw in the designated area a small picture using
 *      the current color.
 * Inputs:
 *      const T_choice *aChoice   -- Choice object selected for this action.
 *---------------------------------------------------------------------------*/
void DrawMode(const T_choice *aChoice)
{
    T_uezDevice lcd;
    T_uezDevice ts;
    static T_uezQueue queue = (TUInt32)NULL;
    INT_32 winX, winY;
    T_pixelColor *pixels;
    T_uezInputEvent inputEvent;
    TBool isDrawing = EFalse;
    INT_32 lastWinX, lastWinY;
    G_drExit = EFalse;
#if ENABLE_UEZ_BUTTON
    T_uezDevice keypadDevice;
#endif
#ifdef NO_DYNAMIC_MEMORY_ALLOC	
	if (NULL == queue)
	{
	  	if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) != UEZ_ERROR_NONE)
		{
		  	queue = NULL;
		}
	}
	
    if (NULL != queue) {
		/* Register the queue so that the IAR Stateviewer Plugin knows about it. */
	  	UEZQueueAddToRegistry( queue, "Draw TS" );	
#else
	if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if UEZ_REGISTER
        UEZQueueSetName(queue, "Draw", "\0");
#endif

#endif
#if ENABLE_UEZ_BUTTON
        UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                UEZLCDGetFrame(lcd, 0, (void **)&pixels);

                // Put the draw screen up
                DR_Screen(lcd);
                DR_DrawColor();

                while (!G_drExit) {
                    // Wait forever until we receive a touchscreen event
                    // NOTE: UEZTSGetReading() can also be used, but it doesn't wait.
                    if (UEZQueueReceive(queue, &inputEvent, UEZ_TIMEOUT_INFINITE)==UEZ_ERROR_NONE) {
                        winX = inputEvent.iEvent.iXY.iX;
                        winY = inputEvent.iEvent.iXY.iY;
                        swim_get_virtual_xy(&G_drWin, &winX, &winY);

                        // Are we in the drawing area?
                        if ((winX > DR_IMAGE_LEFT) && (winX < DR_IMAGE_RIGHT) &&
                                (winY > DR_IMAGE_TOP) && (winY < DR_IMAGE_BOTTOM)) {
                            // Pen down or up?
                            if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD)  {
                                
                                UEZLCDScreensaverWake();
                                
                                if (G_drColor == BLACK) {
                                    // Draw a 3x3 block in the area
                                    swim_set_pen_color(&G_drWin, G_drColor);
                                    if (isDrawing) {
                                        DrawBlockLine(lastWinX, lastWinY, winX, winY);
                                    } else {
                                        DrawBlockPixel(winX, winY);
                                    }
                                } else {
                                    // Draw a dot
                                    swim_set_pen_color(&G_drWin, G_drColor);
                                    if (isDrawing) {
                                        swim_put_line(&G_drWin, lastWinX, lastWinY, winX, winY);
                                    } else {
                                        swim_put_pixel(&G_drWin, winX, winY);
                                    }
                                }
                                isDrawing = ETrue;
                                lastWinX = winX;
                                lastWinY = winY;
                            } else {
                                // No longer drawing
                                isDrawing = EFalse;
                            }
                        } else {
                            ChoicesUpdateByReading(&G_drWin, G_drChoices, &inputEvent);
                            if (inputEvent.iEvent.iXY.iAction == XY_ACTION_RELEASE)
                                isDrawing = EFalse;
                        }
                    }
                }
                UEZLCDClose(lcd);
            }
            UEZTSClose(ts, queue);
        }
#if ENABLE_UEZ_BUTTON
        UEZKeypadClose(keypadDevice, &queue);
#endif
#ifndef NO_DYNAMIC_MEMORY_ALLOC	
        UEZQueueDelete(queue);
#endif
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  DrawMode
 *-------------------------------------------------------------------------*/
