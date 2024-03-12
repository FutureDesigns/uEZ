/*-------------------------------------------------------------------------*
 * File:  TempMode.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Example program that tests the LCD, Queue, Semaphore, and
 *      touchscreen features.
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
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZLCD.h>
#include <uEZMemory.h>
#include <Device/ADCBank.h>
#include <Device/Temperature.h>
#include <Device/RTC.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
//#include <uEZTimeDate.h>
#include "uEZDemoCommon.h"
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/Graphics/SWIM/lpc_droidsansr76.h>
#include <uEZTemperature.h>
#include <uEZLCD.h>
#include <uEZKeypad.h>

/*---------------------------------------------------------------------------*
 * Constants and Macros:
 *---------------------------------------------------------------------------*/
#if UEZ_DEFAULT_LCD_RES_QVGA
#define TM_BUTTON_HEIGHT    32
#define TM_BUTTON_WIDTH     64
#define TM_BOX_WIDTH     90
#define TM_ICON_HEIGHT      64
#define TM_ICON_WIDTH       64
#define TM_FONT             font_winfreesys14x16
#endif

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
#define TM_BUTTON_HEIGHT    128
#define TM_BUTTON_WIDTH     128
#define TM_BOX_WIDTH     180
#define TM_ICON_HEIGHT      128
#define TM_ICON_WIDTH       128
#define TM_FONT             font_lpc_droidsansr76
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
#define TM_BUTTON_HEIGHT    32
#define TM_BUTTON_WIDTH     64
#define TM_BOX_WIDTH     90
#define TM_ICON_HEIGHT      64
#define TM_ICON_WIDTH       64
#define TM_FONT             font_lpc_droidsansr76
#endif

#define MAX_CHOICES     10

#define TM_BOX_INNER_PADDING   8
#define TM_BOX_OUTER_PADDING   4
#define SCREEN_EDGE_MIN_PADDING   10

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    T_uezDevice iLCD;
    T_uezDevice iRTC;
    TBool iExit;
    SWIM_WINDOW_T iWin;
    T_choice iChoices[MAX_CHOICES];
    T_region iRMiddle;
    T_region iRIcon;
    TUInt32 iField;
    TUInt16 iFontHeight;
    TUInt16 iBigDigitWidth;
    TUInt16 iBigDigitHeight;
    TUInt32 iMode; // 0 = C, 1 = F
    T_uezDevice iTemperature;
} T_timeDateWorkspace;
#define G_win (G_ws->iWin)

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void TMExit(const T_choice *aChoice);

void TMToggleMode(const T_choice *aChoice)
{
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)(aChoice->iData);
    G_ws->iMode ^= 1;
}

void TMScreen(T_timeDateWorkspace *G_ws)
{
    T_pixelColor *pixels;
    T_region r;
    T_region rbuttons;
    T_region rbutton;
    T_region rbottom;
    T_region rtemp;
    TUInt32 width;
    TUInt32 height;
    T_choice *p = G_ws->iChoices;

    SUIHidePage0();

    UEZLCDGetFrame(G_ws->iLCD, 0, (void **)&pixels);
    swim_window_open(&G_win, DISPLAY_WIDTH, DISPLAY_HEIGHT, pixels, 0, 0,
        DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, 2, YELLOW, RGB(0, 0, 0), RED);
    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    G_ws->iFontHeight = swim_get_font_height(&G_win);
    swim_set_title(&G_win, "uEZ(tm) Temperature Demonstration", BLUE);
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_fill_color(&G_win, BLACK);

    r.iLeft = 0;
    r.iTop = 0;
    r.iRight = G_win.xvsize;
    r.iBottom = G_win.yvsize;

    // Come off the edges
    RegionShrink(&r, SCREEN_EDGE_MIN_PADDING - 2);

    RegionSplitFromBottom(&r, &rbottom,
        4 + G_ws->iFontHeight + 2 + EXIT_BUTTON_HEIGHT, 2);
    // Add the back button
    RegionShrink(&rbottom, 1);
    p->iLeft = rbottom.iLeft;
    p->iRight = rbottom.iLeft + EXIT_BUTTON_WIDTH - 1;
    p->iTop = rbottom.iTop;
    p->iBottom = rbottom.iTop + EXIT_BUTTON_HEIGHT - 1;
    p->iText = "Exit";
    p->iAction = TMExit;
    p->iIcon = G_exitIcon;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // Use default
    p++;

    // Now take a row off the top
//    RegionSplitFromTop(&r, &rtop, TM_BOX_INNER_PADDING*3+G_ws->iFontHeight*2, TM_BOX_INNER_PADDING);
//    rtop = r;

    swim_set_font_transparency(&G_win, 0);
    swim_set_fill_transparent(&G_win, 0);
    swim_set_font_transparency(&G_win, 1);
    swim_set_font(&G_win, &TM_FONT);
    G_ws->iBigDigitWidth = swim_get_font_char_width(&G_win, '1');
    G_ws->iBigDigitHeight = swim_get_font_height(&G_win);
    width = G_ws->iBigDigitWidth * 6 + 4;
    height = G_ws->iBigDigitHeight + 4;
    height += 2 * 2;
    width += 2 * 2; // padding
    width += TM_ICON_WIDTH + 2;
    if (height < TM_ICON_HEIGHT)
        height = TM_ICON_HEIGHT;

    RegionCenterTopBottom(&r, &G_ws->iRMiddle, height);
    rbuttons = G_ws->iRMiddle;
    RegionCenterLeftRight(&rbuttons, &G_ws->iRMiddle, width);
    rbuttons = G_ws->iRMiddle;

    RegionSplitFromRight(&rbuttons, &rtemp, TM_ICON_WIDTH, 2);
    RegionSplitFromTop(&rtemp, &rbutton, TM_ICON_HEIGHT, 0);
    p->iLeft = rbutton.iLeft;
    p->iTop = rbutton.iTop;
    p->iRight = rbutton.iRight;
    p->iBottom = rbutton.iBottom;
    p->iText = "";
    p->iAction = TMToggleMode;
    p->iIcon = G_temperatureIcon;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // standard icon
    p++;

    // Make F/C also toggle
    RegionSplitFromRight(&rbuttons, &rbutton, G_ws->iBigDigitWidth + 4, 0);
    p->iLeft = rbutton.iLeft;
    p->iTop = rbutton.iTop;
    p->iRight = rbutton.iRight;
    p->iBottom = rbutton.iBottom;
    p->iText = "";
    p->iAction = TMToggleMode;
    p->iIcon = 0; // no icon
    p->iData = (void *)G_ws;
    p->iDraw = 0; // standard icon
    p++;

    RegionShrink(&G_ws->iRMiddle, 2);

    // Mark the end of the choices
    p->iText = 0;

    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    ChoicesDraw(&G_win, G_ws->iChoices);

    SUIShowPage0();
}

static void TMExit(const T_choice *aChoice)
{
    ((T_timeDateWorkspace *)(aChoice->iData))->iExit = ETrue;
}

static void IDrawDigits(
    T_timeDateWorkspace *G_ws,
    T_region *r,
    char *aDigits,
    T_pixelColor aColor)
{
    INT_32 x, y;
    TUInt32 i;
    TInt32 left, right;
    TUInt32 width;

    left = r->iLeft;
    for (i = 0; i < 7; i++) {
        swim_set_fill_color(&G_win, BLACK);
        swim_set_pen_color(&G_win, BLACK);
        swim_set_fill_transparent(&G_win, 0);
        width = G_ws->iBigDigitWidth;
        if ((aDigits[i] == '.') || (aDigits[i] == '_'))
            width >>= 1; // half here
        right = left + width - 1;
        swim_set_xy(&G_win, left, r->iTop);
        swim_set_pen_color(&G_win, aColor);
        if ((aDigits[i] != ' ') && (aDigits[i] != '_'))
            swim_put_char(&G_win, aDigits[i]);

        swim_get_xy(&G_win, &x, &y);
        if (x < right) {
            swim_set_pen_color(&G_win, BLACK);
            swim_put_box(&G_win, x, y, right - 1,
                y + G_ws->iBigDigitHeight - 1);
        }
        left += width;
    }
}

static void TMUpdate(T_timeDateWorkspace *G_ws)
{
    TInt32 t;
    TInt32 i, f;
    char number[12];

    // Now do the temperature update in 15.16 degrees C
    if (UEZTemperatureRead(G_ws->iTemperature, &t) == UEZ_ERROR_NONE) {
        if (G_ws->iMode == 1) {
            // Convert to Fahrenheit
            t *= 9;
            t /= 5;
            t += (32 << 16);
        }
        // Output temperature
        i = t >> 16;
        f = ((((TUInt32)t) & 0xFFFF) * 10) >> 16;  // Convert to 1 digit decimal
        if (G_ws->iMode == 0) {
            sprintf(number, "%3d.%01d_C", i, f);
        } else {
            sprintf(number, "%3d.%01d_F", i, f);
        }
    } else {
        // Output Error!
        strcpy(number, "Err");
    }

    // Now draw the fields
    swim_set_font(&G_win, &TM_FONT);
    swim_set_font_transparency(&G_win, 0);
    swim_set_pen_color(&G_win, YELLOW);
//    r = G_ws->iRMiddle;
//    swim_put_box(&G_win, r.iLeft, r.iTop, r.iRight, r.iBottom);
    IDrawDigits(G_ws, &G_ws->iRMiddle, number, YELLOW);
}

void TempMode(const T_choice *aChoice)
{
    PARAM_NOT_USED(aChoice);
    T_uezDevice ts;
    static T_uezQueue queue = NULL;
    static T_timeDateWorkspace *G_ws = NULL;
#if UEZ_ENABLE_BUTTON_BOARD
    T_uezDevice keypadDevice;
#endif
#ifdef NO_DYNAMIC_MEMORY_ALLOC	
    if (NULL == G_ws)
    {
        G_ws = UEZMemAlloc(sizeof(*G_ws));
    }
#else
    G_ws = UEZMemAlloc(sizeof(*G_ws));
#endif
    if (!G_ws)
        return;
    memset(G_ws, 0, sizeof(*G_ws));
    G_ws->iExit = EFalse;
    G_ws->iField = 1;

    if (UEZTemperatureOpen("Temp0", &G_ws->iTemperature) == UEZ_ERROR_NONE) {
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
            UEZQueueAddToRegistry( queue, "TempMode TS" );
#else
        if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue)
            == UEZ_ERROR_NONE) {
#if UEZ_REGISTER
        UEZQueueSetName(queue, "TempMode", "\0");
#endif
#endif
#if UEZ_ENABLE_BUTTON_BOARD
            UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
            // Open up the touchscreen and pass in the queue to receive events
            if (UEZTSOpen("Touchscreen", &ts, &queue) == UEZ_ERROR_NONE) {
                // Open the LCD and get the pixel buffer
                if (UEZLCDOpen("LCD", &G_ws->iLCD) == UEZ_ERROR_NONE) {
                    // Put the draw screen up
                    TMScreen(G_ws);

                    // Sit here in a loop until we are done
                    while (!G_ws->iExit) {
                        // Do choices and updates
                        ChoicesUpdate(&G_ws->iWin, G_ws->iChoices, queue, 500);
                        TMUpdate(G_ws);
                    }
                    UEZLCDClose(G_ws->iLCD);
                }
                UEZTSClose(ts, queue);
            }
#if UEZ_ENABLE_BUTTON_BOARD
            UEZKeypadClose(keypadDevice, &queue);
#endif
#ifndef NO_DYNAMIC_MEMORY_ALLOC	
            UEZQueueDelete(queue);
#endif
        }
        UEZTemperatureClose(G_ws->iTemperature);
    }
    UEZMemFree(G_ws);
    G_ws = 0;
}

/*-------------------------------------------------------------------------*
 * File:  TimeDateMode
 *-------------------------------------------------------------------------*/
