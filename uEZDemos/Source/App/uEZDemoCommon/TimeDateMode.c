/*-------------------------------------------------------------------------*
 * File:  TimeDateMode
 *-------------------------------------------------------------------------*
 * Description:
 *      Example program that tests the LCD, Queue, Semaphore, and
 *      touchscreen features.
 *
 * Implementation:
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
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZLCD.h>
#include <DEVICE/ADCBank.h>
#include <Device/Temperature.h>
#include <Device/RTC.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
#include <uEZTimeDate.h>
#include "AppDemo.h"
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/Graphics/SWIM/lpc_droidsansr76.h>
#include <Source/Library/Graphics/SWIM/droidsansr32.h>
#include <UEZLCD.h>
#include <UEZKeypad.h>

/*---------------------------------------------------------------------------*
 * Constants and Macros:
 *---------------------------------------------------------------------------*/
#if UEZ_DEFAULT_LCD_RES_QVGA
    #define TDM_BUTTON_HEIGHT     32
    #define TDM_BUTTON_WIDTH      64
    #define TDM_BOX_WIDTH         90
    #define EXIT_BUTTON_HEIGHT    32
    #define EXIT_BUTTON_WIDTH     64
    #define TIME_DATE_FONT        font_winfreesys14x16
#endif

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
    #define TDM_BUTTON_HEIGHT     64
    #define TDM_BUTTON_WIDTH      128
    #define TDM_BOX_WIDTH         180
    #define EXIT_BUTTON_HEIGHT    64
    #define EXIT_BUTTON_WIDTH     128
    #define TIME_DATE_FONT        font_lpc_droidsansr76
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
    #define TDM_BUTTON_HEIGHT     32
    #define TDM_BUTTON_WIDTH      64
    #define TDM_BOX_WIDTH         90
    #define EXIT_BUTTON_HEIGHT    32
    #define EXIT_BUTTON_WIDTH     64
    #define TIME_DATE_FONT        font_lpc_droidsansr32 // font_lpc_droidsansr76
#endif

#define MAX_CHOICES     10

#define TDM_BOX_INNER_PADDING   8
#define TDM_BOX_OUTER_PADDING   4
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
    T_region iRField1;
    T_region iRField2;
    T_region iRField3;
    T_region iRField4;
    T_region iRField5;
    T_region iRField6;
    T_region iRMiddle;
    T_region iRSlash1;
    T_region iRSlash2;
    T_region iRSlash4;
    T_region iRSlash5;
    TUInt32 iField;
    TUInt16 iFontHeight;
    TUInt16 iBigDigitWidth;
    TUInt16 iBigDigitHeight;
    TUInt16 iBigWidthSlash;
    TUInt32 iMode; // 0 = Time, 1 = Date
    TUInt32 iShowButtons;
} T_timeDateWorkspace;
#define G_win (G_ws->iWin)

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void TDMExit(const T_choice *aChoice);
static void SelectField1(const T_choice *aChoice);
static void SelectField2(const T_choice *aChoice);
static void SelectField3(const T_choice *aChoice);
static void SelectField4(const T_choice *aChoice);
static void SelectField5(const T_choice *aChoice);
static void SelectField6(const T_choice *aChoice);
static void DateUp(const T_choice *aChoice);
static void DateDown(const T_choice *aChoice);
static void TimeUp(const T_choice *aChoice);
static void TimeDown(const T_choice *aChoice);
static void PushUp(const T_choice *aChoice);
static void PushDown(const T_choice *aChoice);
static void TDMUpdate(T_timeDateWorkspace *G_ws);

static TUInt8 G_numdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

void TDMScreen(T_timeDateWorkspace *G_ws)
{
    T_pixelColor *pixels;
    T_region r;
    T_region rbuttons;
    T_region rbutton;
    T_region rbottom;
    T_region rsub;
    T_region rtemp;
    TUInt32 width;
    TUInt32 height;
    T_choice *p = G_ws->iChoices;

    SUIHidePage0();

    UEZLCDGetFrame(G_ws->iLCD, 0, (void **)&pixels);
    swim_window_open(
        &G_win,
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
    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    G_ws->iFontHeight = swim_get_font_height(&G_win);
    swim_set_title(&G_win, "uEZ(tm) Time & Date Demonstration", BLUE);
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_fill_color(&G_win, BLACK);

    r.iLeft = 0;
    r.iTop = 0;
    r.iRight = G_win.xvsize;
    r.iBottom = G_win.yvsize;

    // Come off the edges
    RegionShrink(&r, SCREEN_EDGE_MIN_PADDING-2);

    RegionSplitFromBottom(&r, &rbottom, 4+G_ws->iFontHeight+2+EXIT_BUTTON_HEIGHT, 2);
    rtemp = rbottom;

    // Up and Down arrow icons
    if (G_ws->iShowButtons) {
        RegionCenterLeftRight(&rtemp, &rsub, TDM_BUTTON_WIDTH*2+2);

        // Up arrow icon
        RegionSplitFromLeft(&rsub, &rbutton, TDM_BUTTON_WIDTH, 2);
        p->iLeft = rbutton.iLeft;
        p->iTop = rbutton.iTop;
        p->iRight = rbutton.iRight;
        p->iBottom = rbutton.iBottom;
        p->iText = "";
        p->iAction = PushUp;
        p->iIcon = G_upArrowIcon;
        p->iData = (void *)G_ws;
        p->iDraw = 0; // standard icon
        p->iFlags = SUI_FLAG_REPEAT;
        p++;

        // Down arrow icon
        RegionSplitFromLeft(&rsub, &rbutton, TDM_BUTTON_WIDTH, 0);
        p->iLeft = rbutton.iLeft;
        p->iTop = rbutton.iTop;
        p->iRight = rbutton.iRight;
        p->iBottom = rbutton.iBottom;
        p->iText = "";
        p->iAction = PushDown;
        p->iIcon = G_downArrowIcon;
        p->iData = (void *)G_ws;
        p->iDraw = 0; // standard icon
        p->iFlags = SUI_FLAG_REPEAT;
        p++;
    }

    // Add the back button
    RegionShrink(&rbottom, 1);
    p->iLeft = rbottom.iLeft;
    p->iRight = rbottom.iLeft+EXIT_BUTTON_WIDTH-1;
    p->iTop = rbottom.iTop;
    p->iBottom = rbottom.iTop+EXIT_BUTTON_HEIGHT-1;
    p->iText = "Exit";
    p->iAction = TDMExit;
    p->iIcon = G_exitIcon;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // Use default
    p->iFlags = 0;
    p++;

    swim_set_font_transparency(&G_win, 0);
    swim_set_fill_transparent(&G_win, 0);
    swim_set_font_transparency(&G_win, 1);
    swim_set_font(&G_win, &TIME_DATE_FONT);
    G_ws->iBigDigitWidth = 2+swim_get_font_char_width(&G_win, '1');
    G_ws->iBigWidthSlash = 1+swim_get_font_char_width(&G_win, '/');
    G_ws->iBigDigitHeight = swim_get_font_height(&G_win);
    width = G_ws->iBigDigitWidth*6+/*2+TDM_BUTTON_WIDTH*/+G_ws->iBigWidthSlash*2+2*2+2*2;
    height = (G_ws->iBigDigitHeight+4)*2;
//    if ((TDM_BUTTON_HEIGHT*2+2)>height)
//        height = (TDM_BUTTON_HEIGHT*2+2);
    height += 2*2;
    width += 2*2; // padding
//    height += TDM_BUTTON_HEIGHT+2;

    RegionCenterTopBottom(&r, &G_ws->iRMiddle, height);
#if 0
    RegionSplitFromBottom(&G_ws->iRMiddle, &rtemp, TDM_BUTTON_HEIGHT, 2);
#endif
    rbuttons = G_ws->iRMiddle;
    RegionCenterLeftRight(&rbuttons, &G_ws->iRMiddle, width);
    rbuttons = G_ws->iRMiddle;

    // Now create the fields
    RegionSplitFromTop(&rbuttons, &rsub, G_ws->iBigDigitHeight+4, 2);

    RegionSplitFromLeft(&rsub, &G_ws->iRField1, G_ws->iBigDigitWidth*2, 4);
    RegionSplitFromLeft(&rsub, &G_ws->iRSlash1, G_ws->iBigWidthSlash, 0);
    rbutton = G_ws->iRField1;
    p->iLeft = rbutton.iLeft;
    p->iTop = rbutton.iTop;
    p->iRight = rbutton.iRight;
    p->iBottom = rbutton.iBottom;
    p->iText = "";
    p->iAction = SelectField1;
    p->iIcon = 0;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // standard icon
    p->iFlags = 0;
    p++;
    RegionSplitFromLeft(&rsub, &G_ws->iRField2, G_ws->iBigDigitWidth*2, 4);
    RegionSplitFromLeft(&rsub, &G_ws->iRSlash2, G_ws->iBigWidthSlash, 0);
    rbutton = G_ws->iRField2;
    p->iLeft = rbutton.iLeft;
    p->iTop = rbutton.iTop;
    p->iRight = rbutton.iRight;
    p->iBottom = rbutton.iBottom;
    p->iText = "";
    p->iAction = SelectField2;
    p->iIcon = 0;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // standard icon
    p->iFlags = 0;
    p++;
    RegionSplitFromLeft(&rsub, &G_ws->iRField3, G_ws->iBigDigitWidth*2, 0);
    rbutton = G_ws->iRField3;
    p->iLeft = rbutton.iLeft;
    p->iTop = rbutton.iTop;
    p->iRight = rbutton.iRight;
    p->iBottom = rbutton.iBottom;
    p->iText = "";
    p->iAction = SelectField3;
    p->iIcon = 0;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // standard icon
    p->iFlags = 0;
    p++;

    // Now create the fields
    RegionSplitFromTop(&rbuttons, &rsub, G_ws->iBigDigitHeight+4, 0);
    RegionSplitFromLeft(&rsub, &G_ws->iRField4, G_ws->iBigDigitWidth*2, 4);
    RegionSplitFromLeft(&rsub, &G_ws->iRSlash4, G_ws->iBigWidthSlash, 0);
    rbutton = G_ws->iRField4;
    p->iLeft = rbutton.iLeft;
    p->iTop = rbutton.iTop;
    p->iRight = rbutton.iRight;
    p->iBottom = rbutton.iBottom;
    p->iText = "";
    p->iAction = SelectField4;
    p->iIcon = 0;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // standard icon
    p->iFlags = 0;
    p++;
    RegionSplitFromLeft(&rsub, &G_ws->iRField5, G_ws->iBigDigitWidth*2, 4);
    RegionSplitFromLeft(&rsub, &G_ws->iRSlash5, G_ws->iBigWidthSlash, 0);
    rbutton = G_ws->iRField5;
    p->iLeft = rbutton.iLeft;
    p->iTop = rbutton.iTop;
    p->iRight = rbutton.iRight;
    p->iBottom = rbutton.iBottom;
    p->iText = "";
    p->iAction = SelectField5;
    p->iIcon = 0;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // standard icon
    p->iFlags = 0;
    p++;
    RegionSplitFromLeft(&rsub, &G_ws->iRField6, G_ws->iBigDigitWidth*2, 0);
    rbutton = G_ws->iRField6;
    p->iLeft = rbutton.iLeft;
    p->iTop = rbutton.iTop;
    p->iRight = rbutton.iRight;
    p->iBottom = rbutton.iBottom;
    p->iText = "";
    p->iAction = SelectField6;
    p->iIcon = 0;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // standard icon
    p->iFlags = 0;
    p++;

    // Mark the end of the choices
    p->iText = 0;

    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
    ChoicesDraw(&G_win, G_ws->iChoices);

    TDMUpdate(G_ws);

    SUIShowPage0();
}

static void TDMExit(const T_choice *aChoice)
{
    ((T_timeDateWorkspace *)(aChoice->iData))->iExit = ETrue;
}

static void SelectField1(const T_choice *aChoice)
{
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;
    G_ws->iField = 1;
    G_ws->iMode = 1;
    G_ws->iShowButtons = ETrue;
}

static void SelectField2(const T_choice *aChoice)
{
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;
    G_ws->iField = 2;
    G_ws->iMode = 1;
    G_ws->iShowButtons = ETrue;
}

static void SelectField3(const T_choice *aChoice)
{
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;
    G_ws->iField = 3;
    G_ws->iMode = 1;
    G_ws->iShowButtons = ETrue;
}

static void SelectField4(const T_choice *aChoice)
{
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;
    G_ws->iField = 4;
    G_ws->iMode = 0;
    G_ws->iShowButtons = ETrue;
}

static void SelectField5(const T_choice *aChoice)
{
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;
    G_ws->iField = 5;
    G_ws->iMode = 0;
    G_ws->iShowButtons = ETrue;
}

static void SelectField6(const T_choice *aChoice)
{
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;
    G_ws->iField = 6;
    G_ws->iMode = 0;
    G_ws->iShowButtons = ETrue;
}

static void DateUp(const T_choice *aChoice)
{
    T_uezTimeDate td;
    TUInt32 month, day, year;
    TUInt32 numdays;
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;

    if (UEZTimeDateGet(&td) == UEZ_ERROR_NONE) {
        // Get Current Hour, Minute, Second
        month = td.iDate.iMonth;
        day = td.iDate.iDay;
        year = td.iDate.iYear;
        switch(G_ws->iField) {
            case 1:
                if (++month >= 13)
                   month = 1;
                td.iDate.iMonth = month;
                break;
            case 2:
                numdays = G_numdays[month-1];
                if (((year%4) == 0) && (month==2))
                    numdays = 29;
                if (++day > numdays)
                  day = 1;
                td.iDate.iDay = day;
                break;
            case 3:
                year++;
                td.iDate.iYear = year;
                break;
        }
        UEZTimeDateSet(&td);
    }
}

static void DateDown(const T_choice *aChoice)
{
    T_uezTimeDate td;
    TUInt32 month, day, year;
    TUInt32 numdays;
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;

    if (UEZTimeDateGet(&td) == UEZ_ERROR_NONE) {
        // Get Current Hour, Minute, Second
        month = td.iDate.iMonth;
        day = td.iDate.iDay;
        year = td.iDate.iYear;
        switch(G_ws->iField) {
            case 1:
                if (--month == 0)
                   month = 12;
                td.iDate.iMonth = month;
                break;
            case 2:
                day--;
                numdays = G_numdays[month-1];
                if ((year%4) == 0)
                    numdays = 29;
                if (day == 0)
                    day = numdays;
                td.iDate.iDay = day;
                break;
            case 3:
                if (year > 2000) {
                    td.iDate.iYear = year-1;
                }
                break;
        }
        UEZTimeDateSet(&td);
    }
}

static void PushUp(const T_choice *aChoice)
{
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;
    if (G_ws->iMode)
        DateUp(aChoice);
    else
        TimeUp(aChoice);
}

static void PushDown(const T_choice *aChoice)
{
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;
    if (G_ws->iMode)
        DateDown(aChoice);
    else
        TimeDown(aChoice);
}

static void TimeUp(const T_choice *aChoice)
{
    T_uezTimeDate td;
    TUInt32 hour, minute, second;
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;

    if (UEZTimeDateGet(&td) == UEZ_ERROR_NONE) {
        // Get Current Hour, Minute, Second
        hour = td.iTime.iHour;
        minute = td.iTime.iMinute;
        second = td.iTime.iSecond;
        switch(G_ws->iField) {
            case 4:
                if (++hour == 24)
                    hour = 0;
                td.iTime.iHour = hour;
                break;
            case 5:
                if (++minute == 60)
                    minute = 0;
                td.iTime.iMinute = minute;
                break;
            case 6:
                if (++second == 60)
                    second = 0;
                td.iTime.iSecond = second;
                break;
        }
        UEZTimeDateSet(&td);
    }
}

static void TimeDown(const T_choice *aChoice)
{
    T_uezTimeDate td;
    TUInt32 hour, minute, second;
    T_timeDateWorkspace *G_ws = (T_timeDateWorkspace *)aChoice->iData;

    if (UEZTimeDateGet(&td) == UEZ_ERROR_NONE) {
        // Get Current Hour, Minute, Second
        hour = td.iTime.iHour;
        minute = td.iTime.iMinute;
        second = td.iTime.iSecond;
        switch(G_ws->iField) {
            case 4:
                if (--hour>=24)
                    hour = 23;
                td.iTime.iHour = hour;
                break;
            case 5:
                if (--minute>=60)
                    minute = 59;
                td.iTime.iMinute = minute;
                break;
            case 6:
                if (--second >= 60)
                    second = 59;
                td.iTime.iSecond = second;
                break;
        }
        UEZTimeDateSet(&td);
    }
}

static void IDrawTwoDigits(T_timeDateWorkspace *G_ws, T_region *r, char *aDigits, T_pixelColor aColor)
{
    INT_32 x, y;
    INT_32 nextX;

    swim_set_fill_color(&G_win, BLACK);
    swim_set_pen_color(&G_win, BLACK);
    swim_set_fill_transparent(&G_win, 0);
    swim_set_xy(&G_win, r->iLeft+1, r->iTop+1);
    swim_set_pen_color(&G_win, aColor);
    if (aDigits[0] != ' ')
        swim_put_char(&G_win, aDigits[0]);
    swim_get_xy(&G_win, &x, &y);
    nextX = r->iLeft+1+G_ws->iBigDigitWidth-1;
    if (x < nextX) {
        swim_set_pen_color(&G_win, BLACK);
        swim_put_box(&G_win, x, y, nextX, y+G_ws->iBigDigitHeight-1);
    }
    swim_set_xy(&G_win, r->iLeft+1+G_ws->iBigDigitWidth, r->iTop+1);
    swim_set_pen_color(&G_win, aColor);
    if (aDigits[1] != ' ')
        swim_put_char(&G_win, aDigits[1]);
    swim_get_xy(&G_win, &x, &y);
    nextX = r->iLeft+1+2*G_ws->iBigDigitWidth-3;
    if (x < nextX) {
        swim_set_pen_color(&G_win, BLACK);
        swim_put_box(&G_win, x, y, nextX, y+G_ws->iBigDigitHeight-1);
    }
}

static void TDMUpdate(T_timeDateWorkspace *G_ws)
{
    char number[4];
    T_uezTimeDate td;
T_uezError error;

    // Now do the RTC
    error = UEZTimeDateGet(&td);
    if (error) {
        memset(&td, 0, sizeof(td));
    }

    // Now draw the fields
    swim_set_font(&G_win, &TIME_DATE_FONT);
    swim_set_font_transparency(&G_win, 0);
    swim_set_pen_color(&G_win, YELLOW);

    // Date
    sprintf(number, "%2d", td.iDate.iMonth);
    IDrawTwoDigits(G_ws, &G_ws->iRField1, number, (G_ws->iField == 1)?GREEN:YELLOW);
    swim_set_pen_color(&G_win, YELLOW);
    swim_put_text_xy(&G_win, "/", G_ws->iRSlash1.iLeft+1, G_ws->iRSlash1.iTop+1);
    sprintf(number, "%2d", td.iDate.iDay);
    IDrawTwoDigits(G_ws, &G_ws->iRField2, number, (G_ws->iField == 2)?GREEN:YELLOW);
    swim_set_pen_color(&G_win, YELLOW);
    swim_put_text_xy(&G_win, "/", G_ws->iRSlash2.iLeft+1, G_ws->iRSlash2.iTop+1);
    sprintf(number, "%02d", td.iDate.iYear%100);
    IDrawTwoDigits(G_ws, &G_ws->iRField3, number, (G_ws->iField == 3)?GREEN:YELLOW);

    // Time
    sprintf(number, "%2d", td.iTime.iHour);
    IDrawTwoDigits(G_ws, &G_ws->iRField4, number, (G_ws->iField == 4)?GREEN:YELLOW);
    swim_set_pen_color(&G_win, YELLOW);
    swim_put_text_xy(&G_win, ":", G_ws->iRSlash4.iLeft+1, G_ws->iRSlash4.iTop+1);
    sprintf(number, "%02d", td.iTime.iMinute);
    IDrawTwoDigits(G_ws, &G_ws->iRField5, number, (G_ws->iField == 5)?GREEN:YELLOW);
    swim_set_pen_color(&G_win, YELLOW);
    swim_put_text_xy(&G_win, ":", G_ws->iRSlash5.iLeft+1, G_ws->iRSlash5.iTop+1);
    sprintf(number, "%02d", td.iTime.iSecond);
    IDrawTwoDigits(G_ws, &G_ws->iRField6, number, (G_ws->iField == 6)?GREEN:YELLOW);
}

void TimeDateMode(const T_choice *aChoice)
{
    T_uezDevice ts;
    static T_uezQueue queue = NULL;
    const T_choice *p_choice;
    static T_timeDateWorkspace *G_ws = NULL;
    TUInt32 timeLastTouch = UEZTickCounterGet();
    TBool lastShowButtons;
#if ENABLE_UEZ_BUTTON
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
    G_ws->iField = 0; // none selected
    G_ws->iShowButtons = EFalse;
    lastShowButtons = EFalse;

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
	  	UEZQueueAddToRegistry( queue, "TimeDate TS" );	
#else
	if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if UEZ_REGISTER
        UEZQueueSetName(queue, "TimeDate", "\0");
#endif

#endif
#if ENABLE_UEZ_BUTTON
        UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &G_ws->iLCD) == UEZ_ERROR_NONE)  {
                // Put the draw screen up
                TDMScreen(G_ws);

                // Sit here in a loop until we are done
                while (!G_ws->iExit) {
                    // Do choices and updates
                    p_choice = ChoicesUpdate(&G_ws->iWin, G_ws->iChoices, queue, 500);
                    if (G_ws->iShowButtons) {
                        if (p_choice) {
                            // Reset time since last touch
                            timeLastTouch = UEZTickCounterGet();
                        } else {
                            if (UEZTickCounterGetDelta(timeLastTouch) >= 3000) {
                                // Timed out, turn off buttons and redraw
                                G_ws->iShowButtons = EFalse;
                                G_ws->iField = 0;
                            }
                        }
                    }
                    if (lastShowButtons != G_ws->iShowButtons) {
                        TDMScreen(G_ws);
                        lastShowButtons = G_ws->iShowButtons;
                    }
                    TDMUpdate(G_ws);
                }
                UEZLCDClose(G_ws->iLCD);
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
	/* <<< WHIS >>> Potential memory leak in FreeRTOS version as G_ws is not
	free'd. */
}

/*-------------------------------------------------------------------------*
 * File:  TimeDateMode
 *-------------------------------------------------------------------------*/
