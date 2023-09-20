/*-------------------------------------------------------------------------*
 * File:  YourAppMode.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Simple display to show that your application can go here
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
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZLCD.h>
#include <Device/ADCBank.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
//#include <uEZTimeDate.h>
#include "uEZDemoCommon.h"
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
//#include <Source/Library/Graphics/SWIM/lpc_droidsansr76.h>
#include <uEZLCD.h>
#include <uEZKeypad.h>

/*---------------------------------------------------------------------------*
 * Constants and Macros:
 *---------------------------------------------------------------------------*/
#if UEZ_DEFAULT_LCD_RES_QVGA
    #define YAM_BUTTON_HEIGHT    32
    #define YAM_BUTTON_WIDTH     64
    #define YAM_IWIN_WIDTH    223
    #define YAM_IWIN_HEIGHT   187
    #define SCREEN_EDGE_MIN_PADDING   6
    #define YAM_FONT                  APP_DEMO_DEFAULT_FONT
#endif

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
    #define YAM_BUTTON_HEIGHT    64
    #define YAM_BUTTON_WIDTH     128
    #define YAM_IWIN_WIDTH    (223*2)
    #define YAM_IWIN_HEIGHT   (187*2)
    #define SCREEN_EDGE_MIN_PADDING   10
    #define YAM_FONT                  APP_DEMO_DEFAULT_FONT
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
    #define YAM_BUTTON_HEIGHT    32
    #define YAM_BUTTON_WIDTH     64
    #define YAM_IWIN_WIDTH    223
    #define YAM_IWIN_HEIGHT   187
    #define SCREEN_EDGE_MIN_PADDING   10
    #define YAM_FONT                  APP_DEMO_DEFAULT_FONT
#endif

#define MAX_CHOICES     10


#define YOUR_APP_HERE_MESSAGE   "Your Application Goes Here!"

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    T_uezDevice iLCD;
    TBool iExit;
    SWIM_WINDOW_T iWin;
    T_choice iChoices[MAX_CHOICES];
    T_region iRMiddle;
    T_region iRWinImage;
    TUInt16 iFontHeight;
    TUInt16 iBigDigitWidth;
    TUInt16 iBigDigitHeight;
} T_YAMWorkspace;
#define G_win (G_ws->iWin)

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_YAMWorkspace *G_ws = NULL;
	
/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void YAMExit(const T_choice *aChoice);

static void YAMScreen()
{
    T_pixelColor *pixels;
    T_region r;
    T_region rtemp;
    T_region rbuttons;
    T_region rbottom;
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
    swim_set_font(&G_win, &YAM_FONT);
    G_ws->iFontHeight = swim_get_font_height(&G_win);
    swim_set_title(&G_win, "uEZ(tm) Your App Here Demonstration", BLUE);
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_fill_color(&G_win, BLACK);

    r.iLeft = 0;
    r.iTop = 0;
    r.iRight = G_win.xvsize;
    r.iBottom = G_win.yvsize;

    // Come off the edges
    RegionShrink(&r, SCREEN_EDGE_MIN_PADDING-2);
    rtemp = r;
    RegionSplitFromBottom(&rtemp, &rbottom, 4+G_ws->iFontHeight+2+EXIT_BUTTON_HEIGHT, 2);
    // Add the back button
    RegionShrink(&rbottom, 1);
    p->iLeft = rbottom.iLeft;
    p->iRight = rbottom.iLeft+EXIT_BUTTON_WIDTH-1;
    p->iTop = rbottom.iTop;
    p->iBottom = rbottom.iTop+EXIT_BUTTON_HEIGHT-1;
    p->iText = "Exit";
    p->iAction = YAMExit;
    p->iIcon = G_exitIcon;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // Use default
    p++;

    swim_set_font_transparency(&G_win, 0);
    swim_set_fill_transparent(&G_win, 0);
    swim_set_font_transparency(&G_win, 1);
    swim_set_font(&G_win, &YAM_FONT);
    G_ws->iBigDigitWidth = swim_get_font_char_width(&G_win, '1');
    G_ws->iBigDigitHeight = swim_get_font_height(&G_win);
    width = G_ws->iBigDigitWidth*6+4;
    width = swim_get_text_line_width(&G_win, YOUR_APP_HERE_MESSAGE);
    height = G_ws->iBigDigitHeight+4;
    height += 2*2;
    width += 2*2; // padding

    RegionSplitFromBottom(&r, &G_ws->iRMiddle, height, 0);
#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_QVGA)
    // Remove exit button from left
    RegionSplitFromLeft(&G_ws->iRMiddle, &rbuttons, EXIT_BUTTON_WIDTH, 2);
#endif
    rbuttons = G_ws->iRMiddle;
    RegionCenterLeftRight(&rbuttons, &G_ws->iRMiddle, width);
    rbuttons = G_ws->iRMiddle;
    RegionShrink(&rbuttons, 2);

    // Mark the end of the choices
    p->iText = 0;

    r.iLeft = 0;
    r.iTop = 0;
    r.iRight = G_win.xvsize;
    r.iBottom = G_win.yvsize;
#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_QVGA)
    RegionShrink(&r, 4);
    RegionSplitFromLeft(&r, &rtemp, EXIT_BUTTON_WIDTH, 2);
#endif
    RegionCenterTopBottom(&r, &G_ws->iRWinImage, YAM_IWIN_HEIGHT);
    r = G_ws->iRWinImage,
    RegionCenterLeftRight(&r, &G_ws->iRWinImage, YAM_IWIN_WIDTH);

    SUISetDrawWindow(&G_win);
    SUIDrawIcon(G_yourAppWindow, G_ws->iRWinImage.iLeft, G_ws->iRWinImage.iTop);

    swim_set_font(&G_win, &YAM_FONT);
    ChoicesDraw(&G_win, G_ws->iChoices);

    SUIShowPage0();
}

static void YAMExit(const T_choice *aChoice)
{
    G_ws->iExit = ETrue;
}

static void YAMUpdate()
{
    T_region r;

    // Now do the RTC
    // Now draw the fields
    swim_set_font(&G_win, &YAM_FONT);
    swim_set_font_transparency(&G_win, 1);
    swim_set_pen_color(&G_win, YELLOW);
    r = G_ws->iRMiddle;
    swim_put_text_xy(&G_win, YOUR_APP_HERE_MESSAGE, r.iLeft, r.iTop);
	
}

TUInt32 G_crash = 0;

void YourAppMode(const T_choice *aChoice)
{
    T_uezDevice ts;
    static T_uezQueue queue = NULL;
#if UEZ_ENABLE_BUTTON_BOARD
    T_uezDevice keypadDevice;
#endif	
	G_crash = 1;

#ifdef NO_DYNAMIC_MEMORY_ALLOC	
	if (NULL == G_ws)
	{
		G_ws = UEZMemAlloc(sizeof(*G_ws));
	}
#else
	G_ws = UEZMemAlloc(sizeof(*G_ws));
#endif

	G_crash = 2;
    if (!G_ws)
        return;
    memset(G_ws, 0, sizeof(*G_ws));
    G_ws->iExit = EFalse;
//G_ws->iExit = ETrue;

	G_crash = 3;

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
	  	UEZQueueAddToRegistry( queue, "YourApp TS" );	
#else
	if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if UEZ_REGISTER
        UEZQueueSetName(queue, "YourApp", "\0");
#endif

#endif
#if UEZ_ENABLE_BUTTON_BOARD
        UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
	G_crash = 4;

		// Open up the touchscreen and pass in the queue to receive events
		if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
			// Open the LCD and get the pixel buffer
			
			G_crash = 5;
			if (UEZLCDOpen("LCD", &G_ws->iLCD) == UEZ_ERROR_NONE)  {
				// Put the draw screen up
				
				G_crash = 6;
				
				YAMScreen();

				// Sit here in a loop until we are done
				while (!G_ws->iExit) {
					G_crash = 0x0A;
					// Do choices and updates
					ChoicesUpdate(&G_ws->iWin, G_ws->iChoices, queue, 500);
					G_crash = 0x0C;
					YAMUpdate();
				}
				G_crash = 7;
				UEZLCDClose(G_ws->iLCD);
			}
			G_crash = 8;
			UEZTSClose(ts, queue);
		}
		G_crash = 9;
#if UEZ_ENABLE_BUTTON_BOARD
        UEZKeypadClose(keypadDevice, &queue);
#endif
#ifndef NO_DYNAMIC_MEMORY_ALLOC	
	    UEZQueueDelete(queue);
#endif
    }
 	G_crash = 0x0B;
#ifndef NO_DYNAMIC_MEMORY_ALLOC	
	UEZMemFree(G_ws);
#endif
}

/*-------------------------------------------------------------------------*
 * File:  YourAppMode.c
 *-------------------------------------------------------------------------*/
