
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

#include <Device/Accelerometer.h>
#include <uEZDeviceTable.h>
#include <stdio.h>

#define ACCEL_DEMO_DEBUG_PRINTF 0

#ifndef ACCEL_DEMO_G_TO_PIXELS
#define ACCEL_DEMO_G_TO_PIXELS    65536
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

 
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
#if UEZ_ENABLE_BUTTON_BOARD
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
#if UEZ_ENABLE_BUTTON_BOARD
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
#if UEZ_ENABLE_BUTTON_BOARD
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
