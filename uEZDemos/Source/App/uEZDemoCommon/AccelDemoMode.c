
/*-------------------------------------------------------------------------*
 * File:  AccelDemoMode.c
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

#include "uEZDemoCommon.h"
#include "UDCImages.h"
#include <uEZLCD.h>
#include <Device/Accelerometer.h>
#include <uEZDeviceTable.h>
#include <uEZLCD.h>
#include <uEZKeypad.h>
#include <stdio.h>

#define ACCEL_DEMO_DEBUG_PRINTF 0
#define MAX_PIXELS_PER_SECOND  (300*256)

#ifndef ACCEL_DEMO_G_TO_PIXELS
#define ACCEL_DEMO_G_TO_PIXELS    65536
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#if UEZ_DEFAULT_LCD_RES_QVGA
    #define AC_BUTTON_HEIGHT    32
    #define AC_BUTTON_WIDTH     64
    #define AC_RBUTTON_HEIGHT   64
    #define AC_RBUTTON_WIDTH    44
    #define AC_BOX_WIDTH        160
    #define AC_BOX_HEIGHT       160

    #define AC_BALL_RADIUS      5
#endif

#if UEZ_DEFAULT_LCD_RES_VGA
    #define AC_BUTTON_HEIGHT    64
    #define AC_BUTTON_WIDTH     128
    #define AC_RBUTTON_HEIGHT   128
    #define AC_RBUTTON_WIDTH    88
    #define AC_BOX_WIDTH        360
    #define AC_BOX_HEIGHT       360
    #define AC_BALL_RADIUS      10
#endif

#if UEZ_DEFAULT_LCD_RES_WVGA
    #define AC_BUTTON_HEIGHT    64
    #define AC_BUTTON_WIDTH     128
    #define AC_RBUTTON_HEIGHT   128
    #define AC_RBUTTON_WIDTH    88
    #define AC_BOX_WIDTH        360
    #define AC_BOX_HEIGHT       360
    #define AC_BALL_RADIUS      10
#endif

#if (UEZ_DEFAULT_LCD==LCD_RES_480x272)
    #define AC_BUTTON_HEIGHT    32
    #define AC_BUTTON_WIDTH     64
    #define AC_RBUTTON_HEIGHT   64
    #define AC_RBUTTON_WIDTH    44
    #define AC_BOX_WIDTH        200
    #define AC_BOX_HEIGHT       200
    #define AC_BALL_RADIUS      5
#endif

#define AC_BOX_LEFT   (G_ws->iBox.iLeft)
#define AC_BOX_RIGHT  (G_ws->iBox.iRight)
#define AC_BOX_TOP    (G_ws->iBox.iTop)
#define AC_BOX_BOTTOM (G_ws->iBox.iBottom)

#define MAX_CHOICES 2

#ifndef ACCEL_DEMO_ZEROES_AT_START
#define ACCEL_DEMO_ZEROES_AT_START  1
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    T_choice iChoices[MAX_CHOICES+1];
    T_region iBox;
#if ACCEL_DEMO_ALLOW_ROTATE
    TBool iNeedRotate;
    TBool iRotated;
#endif
    TBool iNeedErase;
#if ACCEL_DEMO_ZEROES_AT_START
    TInt32 iZeroX;
    TInt32 iZeroY;
    TBool iZeroSet;
#endif
    TInt32 iSpeedX;
    TInt32 iSpeedY;
} T_accelDemoWorkspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static SWIM_WINDOW_T G_win;
static TInt32 G_acBallX;
static TInt32 G_acBallY;
static DEVICE_Accelerometer **G_accel0;
static TBool G_acExit;
static T_accelDemoWorkspace G;
static T_accelDemoWorkspace *G_ws = &G;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static void AC_Exit(const T_choice *aChoice);

/*---------------------------------------------------------------------------*
 * Routine:  AC_Screen
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the accelerometer demo screen
 * Inputs:
 *      T_uezDevice lcd             -- Display to draw to
 *---------------------------------------------------------------------------*/
void AC_Screen(T_uezDevice lcd)
{
    T_pixelColor *pixels;
    T_region r;
    T_region rtemp;
    T_region rexit;
    T_region rhoriz;
    T_choice *p = G_ws->iChoices;
    TUInt32 fontHeight;

    SUIHidePage0();

    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
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
    fontHeight = swim_get_font_height(&G_win);

#if ACCEL_DEMO_ALLOW_ROTATE
    if (G_ws->iRotated) {
        r.iLeft = 0;
        r.iTop = 0;
        r.iRight = G_win.xvsize;
        r.iBottom = G_win.yvsize;

        // Take out space from top as if banner was still there
        RegionSplitFromTop(&r, &rtemp, 24, 0);
        SUIDrawIcon(G_rotatedBanner, 2, 2);
    } else
#endif // ACCEL_DEMO_ALLOW_ROTATE
    {
        swim_set_title(&G_win, "uEZ(tm) Accelerometer Demonstration", BLUE);

        r.iLeft = 0;
        r.iTop = 0;
        r.iRight = G_win.xvsize;
        r.iBottom = G_win.yvsize;
    }
    swim_set_pen_color(&G_win, YELLOW);
    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);

    // Setup the main box in the remaining middle
    RegionCenterTopBottom(&r, &rtemp, AC_BOX_HEIGHT);
    rhoriz = rtemp;
    RegionCenterLeftRight(&rtemp, &G_ws->iBox, AC_BOX_WIDTH);
    rtemp = rhoriz;
    rtemp.iRight = G_ws->iBox.iLeft-1-5;

#if ACCEL_DEMO_ALLOW_ROTATE
    if (!G_ws->iRotated) {
#endif
        RegionSplitFromRight(&rtemp, &rexit, AC_BUTTON_WIDTH, 0);
        rtemp = rexit;
        RegionSplitFromBottom(&rtemp, &rexit, AC_BUTTON_HEIGHT+2+fontHeight, 0);

        p->iLeft = rexit.iLeft;
        p->iRight = rexit.iRight;
        p->iTop = rexit.iTop;
        p->iBottom = rexit.iTop+AC_BUTTON_HEIGHT ;
        p->iText = "Exit";
        p->iAction = AC_Exit;
        p->iIcon = G_exitIcon;
        p->iData = 0;
        p->iDraw = 0; // Use default
        p++;
#if ACCEL_DEMO_ALLOW_ROTATE
    } else {
        RegionSplitFromRight(&rtemp, &rexit, AC_RBUTTON_WIDTH, 0);
        rtemp = rexit;
        RegionSplitFromBottom(&rtemp, &rexit, AC_RBUTTON_HEIGHT, 0);

        p->iLeft = rexit.iLeft;
        p->iRight = rexit.iRight;
        p->iTop = rexit.iTop;
        p->iBottom = rexit.iTop+AC_RBUTTON_HEIGHT ;
        p->iText = "";
        p->iAction = AC_Exit;
        p->iIcon = G_downExit;
        p->iData = 0;
        p->iDraw = 0; // Use default
        p++;
    }
#endif // ACCEL_DEMO_ALLOW_ROTATE

    // No more choices
    p->iText = 0;

    ChoicesDraw(&G_win, G_ws->iChoices);

    // Now draw the boundary area
    swim_set_fill_color(
        &G_win,
        BLACK);
    swim_put_box(
        &G_win,
        AC_BOX_LEFT,
        AC_BOX_TOP,
        AC_BOX_RIGHT,
        AC_BOX_BOTTOM);

    SUIShowPage0();

    G_ws->iNeedErase = EFalse;
}

/*---------------------------------------------------------------------------*
 * Routine:  AC_Exit
 *---------------------------------------------------------------------------*
 * Description:
 *      Exit button was pressed
 * Inputs:
 *      const T_choice *aChoice -- Pointer to choice pressed
 *---------------------------------------------------------------------------*/
static void AC_Exit(const T_choice *aChoice)
{
    G_acExit = ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  IDrawBall
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw the ball on the screen at its current location
 *---------------------------------------------------------------------------*/
static void IDrawBall(void)
{
    INT_32 imageX, imageY;
    imageX = (G_acBallX>>8)-AC_BALL_RADIUS;
    imageY = (G_acBallY>>8)-AC_BALL_RADIUS;
    swim_get_physical_xy(&G_win, &imageX, &imageY);
    SUISetDrawWindow(&G_win);
    SUIDrawIcon(
        G_ballIcon,
        imageX,
        imageY);
}

/*---------------------------------------------------------------------------*
 * Routine:  IEraseBall
 *---------------------------------------------------------------------------*
 * Description:
 *      Erase the ball on the screen at its current location
 *---------------------------------------------------------------------------*/
void IEraseBall(void)
{
    TUInt32 x, y;
    swim_set_fill_color(&G_win, BLACK);
    swim_set_pen_color(&G_win, BLACK);
    swim_set_fill_transparent(&G_win, 0);
    x = (G_acBallX>>8) - AC_BALL_RADIUS;
    y = (G_acBallY>>8) - AC_BALL_RADIUS;
    swim_put_box(&G_win,
        x,
        y,
        x+AC_BALL_RADIUS*2-1,
        y+AC_BALL_RADIUS*2-1);
    swim_set_fill_transparent(&G_win, 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  AC_UpdateBall
 *---------------------------------------------------------------------------*
 * Description:
 *    Update the position of the ball based on a reading from the
 *    accelerometer.
 * Inputs:
 *    TInt32 aDelta -- Delta in milliseconds since last update
 *    T_uezDevice aLCD -- Handle to LCD screen
 *---------------------------------------------------------------------------*/
static void AC_UpdateBall(TInt32 aDelta, T_uezDevice aLCD)
{
    TInt32 diffX, diffY;
    TInt32 x, y;
    AccelerometerReading reading;
    T_uezError error;
    TUInt32 time;
    static TUInt32 lastTime = 0;
    TInt32 n;

    error = (*G_accel0)->ReadXYZ(G_accel0, &reading, 100);

    time =  UEZTickCounterGet();
    aDelta = time-lastTime;
    lastTime = time;

#if ACCEL_DEMO_ALLOW_ROTATE
    if (reading.iX > (1<<15))
        G_ws->iNeedRotate = ETrue;
    else if (reading.iY > (1<<15))
        G_ws->iNeedRotate = EFalse;
#endif

    if (error == UEZ_ERROR_NONE) {
#if ACCEL_DEMO_ZEROES_AT_START
        if (!G_ws->iZeroSet) {
            G_ws->iZeroX = 0;
            G_ws->iZeroY = 0;
            for (n=0; n<16; n++) {
                UEZTaskDelay(10);
                (*G_accel0)->ReadXYZ(G_accel0, &reading, 100);
                G_ws->iZeroX += reading.iX;
                G_ws->iZeroY += reading.iY;
            }
            G_ws->iZeroX /= n;
            G_ws->iZeroY /= n;
            G_ws->iZeroSet = ETrue;
            aDelta = 0; // reset the time
            lastTime = UEZTickCounterGet();
        }
        reading.iX -= G_ws->iZeroX;
        reading.iY -= G_ws->iZeroY;
#endif
#if ACCEL_DEMO_DEBUG_PRINTF
    if (error) {
        printf("error=%d\r\n", error);
    } else {
        printf("zero=%6.2f %6.2f ", ((double)G_ws->iZeroX)/65536.0, ((double)G_ws->iZeroY)/65536.0);
        printf("delta=%d %6.2f %6.2f %6.2f\r\n", aDelta, ((double)reading.iX)/65536.0, ((double)reading.iY)/65536.0, ((double)reading.iZ)/65536.0);
    }
#endif
#if ACCEL_DEMO_SWAP_XY
        diffX = ((TInt32)(reading.iY*aDelta))/65536;
        diffY = ((TInt32)(reading.iX*aDelta))/65536;
#else
        diffX = ((TInt32)(reading.iX*aDelta))/65536;
        diffY = ((TInt32)(reading.iY*aDelta))/65536;
#endif
#if ACCEL_DEMO_FLIP_X
        diffX = -diffX;
#endif
#if ACCEL_DEMO_FLIP_Y
        diffY = -diffY;
#endif
        G_ws->iSpeedX += diffX*512;
        G_ws->iSpeedY += diffY*512;
        if (G_ws->iSpeedX >= MAX_PIXELS_PER_SECOND)
          G_ws->iSpeedX = MAX_PIXELS_PER_SECOND;
        if (G_ws->iSpeedX <= (-MAX_PIXELS_PER_SECOND))
          G_ws->iSpeedX = (-MAX_PIXELS_PER_SECOND);
        if (G_ws->iSpeedY >= (MAX_PIXELS_PER_SECOND))
          G_ws->iSpeedY = (MAX_PIXELS_PER_SECOND);
        if (G_ws->iSpeedY <= (-MAX_PIXELS_PER_SECOND))
          G_ws->iSpeedY = (-MAX_PIXELS_PER_SECOND);
        x = G_acBallX + (G_ws->iSpeedX*aDelta)/1000;
        y = G_acBallY + (G_ws->iSpeedY*aDelta)/1000;
        if (x > (((AC_BOX_RIGHT-AC_BALL_RADIUS)-1)<<8)) {
            x = ((AC_BOX_RIGHT-AC_BALL_RADIUS)-1)<<8;
            G_ws->iSpeedX = -(G_ws->iSpeedX/2);
        }
        if (x < (((AC_BOX_LEFT+AC_BALL_RADIUS)+1)<<8)) {
            x = ((AC_BOX_LEFT+AC_BALL_RADIUS)+1)<<8;
            G_ws->iSpeedX = -(G_ws->iSpeedX/2);
        }
        if (y > (((AC_BOX_BOTTOM-AC_BALL_RADIUS)-1)<<8)) {
            y = ((AC_BOX_BOTTOM-AC_BALL_RADIUS)-1)<<8;
            G_ws->iSpeedY = -(G_ws->iSpeedY/2);
        }
        if (y < (((AC_BOX_TOP+AC_BALL_RADIUS)+1)<<8)) {
            y = ((AC_BOX_TOP+AC_BALL_RADIUS)+1)<<8;
            G_ws->iSpeedY = -(G_ws->iSpeedY/2);
        }
        if ((x != G_acBallX) || (y != G_acBallY)) {
            // Removed temporarily until vertical sync is fixed on RX platforms -MK
            //UEZLCDWaitForVerticalSync(aLCD, 100);
            if (G_ws->iNeedErase)
                IEraseBall();
            G_acBallX = x;
            G_acBallY = y;
            IDrawBall();
            G_ws->iNeedErase = ETrue;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AccelDemoMode
 *---------------------------------------------------------------------------*
 * Description:
 *    Run the acceleration demo by moving a ball around on the screen.
 * Inputs:
 *    const T_choice *aChoice -- Choice pressed to get to this option.
 *                                (ignored)
 *---------------------------------------------------------------------------*/
void AccelDemoMode(const T_choice *aChoice)
{
    T_uezDevice lcd;
    T_uezDevice ts;
    const T_choice *p_choice;
    T_pixelColor *pixels;
    T_uezError error;
    static T_uezQueue queue = (TUInt32)NULL;
    T_uezDevice acdevice;
    TUInt32 time, start, delta;
#if ENABLE_UEZ_BUTTON
    T_uezDevice keypadDevice;
#endif

    PARAM_NOT_USED(aChoice);

    G_acExit = EFalse;
#if ACCEL_DEMO_ZEROES_AT_START
    G_ws->iZeroSet = EFalse;
#endif
    G_ws->iSpeedX = 0;
    G_ws->iSpeedY = 0;
    G_ws->iZeroX = 0;
    G_ws->iZeroY = 0;

    // Put ball in center
    G_acBallX = (AC_BOX_LEFT + AC_BOX_RIGHT)/2;
    G_acBallY = (AC_BOX_TOP + AC_BOX_BOTTOM)/2;

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
	  	UEZQueueAddToRegistry( queue, "AccelDemo TS" );
#else
	if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
	    UEZQueueSetName(queue, "AccelDemo", "\0");
#endif
#if ENABLE_UEZ_BUTTON
        UEZKeypadOpen("BBKeypad", &keypadDevice, &queue);
#endif
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                UEZLCDGetFrame(lcd, 0, (void **)&pixels);
                error = UEZDeviceTableFind("Accel0", &acdevice);
                if (!error)
                    error = UEZDeviceTableGetWorkspace(
                                acdevice,
                                (T_uezDeviceWorkspace **)&G_accel0);
                if (!error) {

                    // Put the draw screen up
                    AC_Screen(lcd);
                    start = UEZTickCounterGet();

                    // Sit here in a loop until we are done
                    while (!G_acExit) {
                        p_choice = ChoicesUpdate(&G_win, G_ws->iChoices, queue, 0);
                        time = UEZTickCounterGet();
                        delta = time-start;
                        start = time;
                        if ((!p_choice) && (delta))
                            AC_UpdateBall((TInt32)delta, lcd);

#if ACCEL_DEMO_ALLOW_ROTATE
                        if (G_ws->iNeedRotate) {
                            if (G_ws->iRotated == EFalse) {
                                G_ws->iRotated = ETrue;
                                AC_Screen(lcd);
                                start = UEZTickCounterGet();
                            }
                        } else {
                            if (G_ws->iRotated == ETrue) {
                                G_ws->iRotated = EFalse;
                                AC_Screen(lcd);
                                start = UEZTickCounterGet();
                            }
                        }
#endif
                        UEZTaskDelay(20);
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
 * File:  AccelDemoMode.c
 *-------------------------------------------------------------------------*/
