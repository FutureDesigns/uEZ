/*-------------------------------------------------------------------------*
 * File:  BrightnessControlMode
 *-------------------------------------------------------------------------*
 * Description:
 *      Example program that tests the LCD, Queue, Semaphore, and
 *      touchscreen features.
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
#include <Device/ADCBank.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
#include <uEZTimeDate.h>
#include <uEZLCD.h>
#include "uEZDemoCommon.h"
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/ScreenSaver/BouncingLogoSS.h>
#include <Types/InputEvent.h>
#if UEZ_ENABLE_LIGHT_SENSOR
#include "uEZRTOS.h"
#include <Source/Devices/Light Sensor/ROHM/BH1721FVC/Light_Sensor_BH1721FVC.h>
#endif
#include <uEZLCD.h>
#include <uEZKeypad.h>
/*---------------------------------------------------------------------------*
 * Constants and Macros:
 *---------------------------------------------------------------------------*/
#if UEZ_DEFAULT_LCD_RES_QVGA  
    #define BCM_KNOB_WIDTH    32
    #define BCM_KNOB_HEIGHT   32
    #define BCM_COLORS_HEIGHT 150
    #define BCM_COLORS_WIDTH  (64*3)
#endif

#if UEZ_DEFAULT_LCD_RES_VGA  
    #define BCM_KNOB_WIDTH    64
    #define BCM_KNOB_HEIGHT   64
    #define BCM_COLORS_HEIGHT (96*3)
    #define BCM_COLORS_WIDTH  400
#endif

#if UEZ_DEFAULT_LCD_RES_WVGA
    #define BCM_KNOB_WIDTH    64
    #define BCM_KNOB_HEIGHT   64
    #define BCM_COLORS_HEIGHT (96*3)
    #define BCM_COLORS_WIDTH  550
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
    #define BCM_KNOB_WIDTH    32
    #define BCM_KNOB_HEIGHT   32
    #define BCM_COLORS_HEIGHT (64*3)
    #define BCM_COLORS_WIDTH  BCM_COLORS_HEIGHT
#endif

#define BCM_GROOVE_WIDTH  4
#define BCM_SLIDE_PANEL_OUTER_PADDING   20
#define BCM_SLIDE_PANEL_INNER_PADDING   20

#define MAX_CHOICES     3

#define SCREEN_EDGE_MIN_PADDING         10

#if UEZ_DEFAULT_LCD_RES_QVGA
    #define UEZ_ICON_HEIGHT 64
    #define UEZ_ICON_WIDTH 111
#endif

#if UEZ_DEFAULT_LCD_RES_VGA
    #define UEZ_ICON_HEIGHT    118
    #define UEZ_ICON_WIDTH   220
#endif

#if UEZ_DEFAULT_LCD_RES_WVGA
    #define UEZ_ICON_HEIGHT    118
    #define UEZ_ICON_WIDTH   220
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
    #define UEZ_ICON_HEIGHT 64
    #define UEZ_ICON_WIDTH 111
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    SWIM_WINDOW_T iWin;
    T_uezDevice iLCD;
    T_choice iChoices[MAX_CHOICES+1];
    TBool iExit;
    T_region iRColors;
    T_region iRSlidePanel;
    T_region iRGroove;
    TUInt32 iFontHeight;
    TBool iNeedUpdate;
    TUInt32 iKnobY;
    TUInt32 iLevel;
    TUInt32 iNumLevels;
} T_brightnessControlWorkspace;

#define G_win (G_ws->iWin)

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_region G_ssOptionBox;
TBool G_ScreenSaverOn = EFalse;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void BCMExit(const T_choice *aChoice);

static void BCMExit(const T_choice *aChoice)
{
    ((T_brightnessControlWorkspace *)(aChoice->iData))->iExit = ETrue;
}

static void DrawScreenSaverBox(T_brightnessControlWorkspace *G_ws)
{
    if(G_ScreenSaverOn == EFalse) {
        swim_set_fill_color(&G_win, BLACK);
    } else {
        swim_set_fill_color(&G_win, YELLOW);
    }
    swim_set_pen_color(&G_win, YELLOW);
    swim_put_box(
        &G_win,
        G_ssOptionBox.iLeft+1,
        G_ssOptionBox.iTop+1,
        G_ssOptionBox.iRight-1,
        G_ssOptionBox.iBottom-1);
}

static void BCMScreenSaverToggle(const T_choice *aChoice)
{
    T_uezLCDScreenSaverInfo ssInfo;
    T_uezDevice lcd;
    
    UEZLCDOpen("LCD", &lcd);
    
    if(G_ScreenSaverOn == ETrue) {
        G_ScreenSaverOn = EFalse;
        UEZLCDScreensaverStop();
    } else {
        G_ScreenSaverOn = ETrue;

        ssInfo.iMSTillDim = 5000;
        ssInfo.iMSTillAnimation = 10000;
        ssInfo.iMSTillSleep = 30000;
        ssInfo.iMSAnimationRefresh = 1000;
        ssInfo.iCallback_AnimationStart = &BouncingLogoSS_Start;
        ssInfo.iCallback_AnimationUpdate = &BouncingLogoSS_Update;
        ssInfo.iCallback_AnimationSleep = &BouncingLogoSS_Sleep;
        ssInfo.iCallback_AnimationEnd = &BouncingLogoSS_End;
        //Must be call before starting
        BouncingLogoSS_Setup((TUInt8*)&G_uEZLogo,
                UEZ_ICON_WIDTH,
                UEZ_ICON_HEIGHT,
                DISPLAY_WIDTH,
                DISPLAY_HEIGHT);
        UEZLCDScreensaverStart(lcd, &ssInfo);
    }
    DrawScreenSaverBox(((T_brightnessControlWorkspace *)(aChoice->iData)));
}

static void IPatternOfColors(T_brightnessControlWorkspace *G_ws)
{
    int x, y;
    int v;
    TUInt16 i=0;
    T_pixelColor c;

    for (y=1; y<=(BCM_COLORS_HEIGHT/3); y++)  {
        v = (y*256)/(BCM_COLORS_HEIGHT/3);
        c = RGB(v, 0, 0);
        for (x=0; x<BCM_COLORS_WIDTH; x++, i++)  {
            swim_set_pen_color(&G_win, c);
            swim_put_pixel(
                &G_win,
                G_ws->iRColors.iLeft+x,
                G_ws->iRColors.iTop+y+0);
        }
    }
    for (y=1; y<=(BCM_COLORS_HEIGHT/3); y++)  {
        v = (y*256)/(BCM_COLORS_HEIGHT/3);
        c = RGB(0, v, 0);
        for (x=0; x<BCM_COLORS_WIDTH; x++, i++)  {
            swim_set_pen_color(&G_win, c);
            swim_put_pixel(
                &G_win,
                G_ws->iRColors.iLeft+x,
                G_ws->iRColors.iTop+y+(BCM_COLORS_HEIGHT/3));
        }
    }
    for (y=1; y<=(BCM_COLORS_HEIGHT/3); y++)  {
        v = (y*256)/(BCM_COLORS_HEIGHT/3);
        c = RGB(0, 0, v);
        for (x=0; x<BCM_COLORS_WIDTH; x++, i++)  {
            swim_set_pen_color(&G_win, c);
            swim_put_pixel(
                &G_win,
                G_ws->iRColors.iLeft+x,
                G_ws->iRColors.iTop+y+((2*BCM_COLORS_HEIGHT)/3));
        }
    }
}


static void BCMScreen(T_brightnessControlWorkspace *G_ws)
{
    T_pixelColor *pixels;
    T_region r;
    T_region rbottom;
    T_region rtemp;
    TUInt32 width;
    T_choice *p = G_ws->iChoices;
    TUInt32 padding;

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
    swim_set_title(&G_win, "uEZ(tm) Brightness Control", BLUE);
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_fill_color(&G_win, BLACK);

    r.iLeft = 0;
    r.iTop = 0;
    r.iRight = G_win.xvsize;
    r.iBottom = G_win.yvsize;

    // Come off the edges
    RegionShrink(&r, SCREEN_EDGE_MIN_PADDING-2);

    // Add the back button
    RegionSplitFromBottom(&r, &rbottom, 4+G_ws->iFontHeight+2+EXIT_BUTTON_HEIGHT, 5);
    //RegionCenterLeftRight(&rbottom, &rtemp, EXIT_BUTTON_WIDTH);
    //rbottom = rtemp;
    
    rtemp = rbottom;
    RegionSplitFromLeft(&rtemp, &rbottom, EXIT_BUTTON_WIDTH, 5);
    
    RegionShrink(&rbottom, 1);
    p->iLeft = rbottom.iLeft;
    p->iRight = rbottom.iLeft+EXIT_BUTTON_WIDTH-1;
    p->iTop = rbottom.iTop;
    p->iBottom = rbottom.iTop+EXIT_BUTTON_HEIGHT-1;
    p->iText = "Exit";
    p->iAction = BCMExit;
    p->iIcon = G_exitIcon;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // Use default
    p++;

    rbottom = rtemp;
    RegionCenterLeftRight(&rbottom, &rtemp, 100);
    RegionSplitFromBottom(&rtemp, &G_ssOptionBox, (rtemp.iBottom-rtemp.iTop)/2+6, 5); // location of screen saver option box
    //RegionSplitFromLeft(&rtemp, &G_ssOptionBox, (rtemp.iBottom-rtemp.iTop)/2 - swim_get_font_height(&G_win)/2, 5);
    G_ssOptionBox.iRight = G_ssOptionBox.iLeft+swim_get_font_height(&G_win)+16; // size of screen saver option box
    G_ssOptionBox.iBottom = G_ssOptionBox.iTop+swim_get_font_height(&G_win)+16;
    
    p->iLeft = G_ssOptionBox.iLeft;
    p->iRight = G_ssOptionBox.iRight;
    p->iTop = G_ssOptionBox.iTop;
    p->iBottom = G_ssOptionBox.iBottom;
    p->iText = "";
    p->iAction = BCMScreenSaverToggle;
    p->iIcon = 0;
    p->iData = (void *)G_ws;
    p->iDraw = 0; // Use default
    p++;
    
    // Determine the spacing between slider and RGB
    // Determine width used
    width = BCM_COLORS_WIDTH+BCM_SLIDE_PANEL_INNER_PADDING*2+BCM_SLIDE_PANEL_OUTER_PADDING*2+BCM_GROOVE_WIDTH;
    // Now subtract that from the full width available.
    width = (1+r.iRight-r.iLeft)-width;
    // Split it 3 ways to divide it evenly
    padding = width/3;

    // Now use this padding to take a chunk out of the left
    RegionSplitFromLeft(&r, &rtemp, 0, padding);
    // Now determine the region for the slider
    RegionSplitFromLeft(
        &r, 
        &G_ws->iRSlidePanel, 
        BCM_SLIDE_PANEL_INNER_PADDING*2+BCM_SLIDE_PANEL_OUTER_PADDING*2+BCM_GROOVE_WIDTH, 
        padding);
    RegionCenterLeftRight(&G_ws->iRSlidePanel, &G_ws->iRGroove, BCM_GROOVE_WIDTH);
    RegionShrinkTopBottom(&G_ws->iRGroove, BCM_SLIDE_PANEL_INNER_PADDING);

    // Now place the colors on the right
    RegionSplitFromLeft(
        &r, 
        &rtemp,
        BCM_COLORS_WIDTH,
        0);
    RegionCenterTopBottom(
        &rtemp,
        &G_ws->iRColors,
        BCM_COLORS_HEIGHT);

    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);

    swim_set_font_transparency(&G_win, 1);
    swim_set_fill_transparent(&G_win, 0);
    swim_set_pen_color(&G_win, YELLOW);
    r = G_ws->iRSlidePanel;
    swim_put_box(
        &G_win,
        r.iLeft,
        r.iTop,
        r.iRight,
        r.iBottom);
    r = G_ws->iRGroove;
    swim_put_box(
        &G_win,
        r.iLeft,
        r.iTop,
        r.iRight,
        r.iBottom);
    
    DrawScreenSaverBox(G_ws);
    
    swim_put_text_xy(&G_win, "Screen Saver On/Off", 
                     G_ssOptionBox.iLeft+swim_get_font_height(&G_win)+22,//x
                     G_ssOptionBox.iTop+8);//y
    
    // Now do the RTC
    // Now draw the fields
    IPatternOfColors(G_ws);
    
    swim_set_pen_color(&G_win, YELLOW);
    ChoicesDraw(&G_win, G_ws->iChoices);

    SUIShowPage0();
}

static void BCMUpdate(T_brightnessControlWorkspace *G_ws)
{
    T_region r;
    TUInt32 y;

    swim_set_font_transparency(&G_win, 1);
    r = G_ws->iRGroove;
    UEZLCDGetBacklightLevel(G_ws->iLCD, &G_ws->iLevel, &G_ws->iNumLevels);
    y = (G_ws->iLevel * (1+r.iBottom-r.iTop))/G_ws->iNumLevels;
    y = r.iBottom - y;
    if (y != G_ws->iKnobY) {
        if (G_ws->iKnobY) {
            // Erase the old one
            swim_set_fill_transparent(&G_win, 0);
            swim_set_pen_color(&G_win, BLACK);
            swim_set_fill_color(&G_win, BLACK);
            swim_put_box(
                &G_win,
                r.iLeft-15,
                G_ws->iKnobY-5,
                r.iRight+15,
                G_ws->iKnobY+5);
        }
        swim_set_fill_transparent(&G_win, 1);
        swim_set_pen_color(&G_win, YELLOW);
        swim_put_box(
            &G_win,
            r.iLeft,
            r.iTop,
            r.iRight,
            r.iBottom);
        G_ws->iKnobY = y;
        swim_set_fill_transparent(&G_win, 0);
        swim_set_pen_color(&G_win, YELLOW);
        swim_set_fill_color(&G_win, BLACK);
        swim_put_box(
            &G_win,
            r.iLeft-15,
            G_ws->iKnobY-5,
            r.iRight+15,
            G_ws->iKnobY+5);
    }
}

void BrightnessControlMode(const T_choice *aChoice)
{
    T_uezDevice ts;
    static T_uezQueue queue = NULL;
    static T_brightnessControlWorkspace *G_ws = NULL;
    INT_32 winX, winY;
    T_uezInputEvent inputEvent;
#if ENABLE_UEZ_BUTTON
    T_uezDevice keypadDevice;
#endif
#if UEZ_ENABLE_LIGHT_SENSOR
    TUInt32 levelCurrent = 1, levelPrevious = 0;
    T_uezDevice ls;
    DEVICE_LightSensor **p;
    T_uezError error;
    char levelText[30];
    TBool lightSensorActive = EFalse;
    
    if( UEZDeviceTableFind("Light Sensor", &ls) == UEZ_ERROR_NONE){
        if(UEZDeviceTableGetWorkspace(ls, (T_uezDeviceWorkspace **)&p) == UEZ_ERROR_NONE){
          if((*p)->Open((void *)p, "I2C1") == UEZ_ERROR_NONE){
            lightSensorActive = ETrue;
          }
        }
    }
#endif
#ifdef NO_DYNAMIC_MEMORY_ALLOC	
	if (NULL == G_ws)
	{
		G_ws = UEZMemAlloc(sizeof(*G_ws));
	}
#else
	G_ws = UEZMemAlloc(sizeof(*G_ws));
#endif

#if UEZ_ENABLE_LIGHT_SENSOR
    UEZTaskSuspend(G_lightSensorTask);
#endif    
    if (!G_ws)
        return;
    memset(G_ws, 0, sizeof(*G_ws));
    G_ws->iExit = EFalse;
    G_ws->iNeedUpdate = ETrue;

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
	  	UEZQueueAddToRegistry( queue, "Brightness TS" );	  
#else
	if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if UEZ_REGISTER
        UEZQueueSetName(queue, "Brightness", "\0");
#endif
#endif
#if ENABLE_UEZ_BUTTON
        UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif

        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
 
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &G_ws->iLCD) == UEZ_ERROR_NONE)  {
                UEZLCDGetBacklightLevel(G_ws->iLCD, &G_ws->iLevel, &G_ws->iNumLevels);

                // Put the screen up
                BCMScreen(G_ws);

                // Sit here in a loop until we are done
                while (!G_ws->iExit) {
                    // Do choices and updates
                    if (UEZQueueReceive(queue, &inputEvent, 500)==UEZ_ERROR_NONE) {
                        winX = inputEvent.iEvent.iXY.iX;
                        winY = inputEvent.iEvent.iXY.iY;
                        swim_get_virtual_xy(&G_win, &winX, &winY);
                        if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD)  {
                            // Are we in the panel?
                            if ((winY >= G_ws->iRSlidePanel.iTop) && (winY <= G_ws->iRSlidePanel.iBottom) && 
                                    (winX >= G_ws->iRSlidePanel.iLeft) && (winX <= G_ws->iRSlidePanel.iRight)) {
                                // Inside the panel and touching the screen, let's map this to an intensity
                                // of 0-255
								if(winY > G_ws->iRGroove.iBottom)
									G_ws->iLevel = 0;
								else
								{
	                                G_ws->iLevel = 
	                                    (G_ws->iRGroove.iBottom - winY)*(G_ws->iNumLevels)/
	                                    (1+G_ws->iRGroove.iBottom-G_ws->iRGroove.iTop);
								}
                                UEZLCDSetBacklightLevel(G_ws->iLCD, G_ws->iLevel);
                                G_ws->iNeedUpdate = ETrue;
                            }
                        }
                        ChoicesUpdateByReading(&G_win, G_ws->iChoices, &inputEvent);
#if UEZ_ENABLE_LIGHT_SENSOR
                        if (lightSensorActive){
                            levelCurrent = (*p)->GetLevel((void *)p);
                            if(levelCurrent == 0xFFFFFFFF){ //ligh sensor no longer resonding
                                lightSensorActive = EFalse;
                                break;
                            }
                            if( levelCurrent != levelPrevious){
                                swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);
                                swim_set_fill_color(&G_win, BLACK);
                                swim_set_pen_color(&G_win, BLACK);
                                sprintf(levelText, "Ambient Light Level: %04d lux\0", levelCurrent);
                                //erase old text
                                swim_put_box(&G_win,
                                             (UEZ_LCD_DISPLAY_WIDTH/2) - (swim_get_text_line_width(&G_win, levelText) /2),//x1
                                             BCM_SLIDE_PANEL_OUTER_PADDING,//y1
                                             (UEZ_LCD_DISPLAY_WIDTH/2) + (swim_get_text_line_width(&G_win, levelText) /2),//x2
                                             BCM_SLIDE_PANEL_OUTER_PADDING + swim_get_font_height(&G_win));//y2
                                //put new text
                                swim_set_pen_color(&G_win, YELLOW);
                                swim_put_text_xy(&G_win, levelText, 
                                                 (UEZ_LCD_DISPLAY_WIDTH/2) - (swim_get_text_line_width(&G_win, levelText) /2),//x
                                                 (BCM_SLIDE_PANEL_OUTER_PADDING));//y
                                levelPrevious = levelCurrent;
                            } 
                        }
#endif
                    }
                    if (G_ws->iNeedUpdate) {
                        BCMUpdate(G_ws);
                        G_ws->iNeedUpdate = EFalse;
                    }
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
#if UEZ_ENABLE_LIGHT_SENSOR
        (*p)->Close((void *)p);
        UEZTaskResume(G_lightSensorTask);
#endif
    }
    UEZMemFree(G_ws);
    G_ws = 0;
}

/*-------------------------------------------------------------------------*
 * File:  BrightnessControlMode
 *-------------------------------------------------------------------------*/
