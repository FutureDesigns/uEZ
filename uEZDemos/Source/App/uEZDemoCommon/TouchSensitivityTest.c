#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <Device/ADCBank.h>
#include <uEZDeviceTable.h>
#include <uEZProcessor.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>

#define TEST_BOX_OUTSIDE_SIZE	11	//for 10x10 pixel box
#define TEST_BOX_INSIDE_SIZE	3	//fro 3x3 pixel box

//select only one
#define TEST_CENTER				1
#define TEST_TOP_LEFT			1
#define TEST_TOP_RIGHT			1
#define TEST_BOTTOM_LEFT		1
#define TEST_BOTTOM_RIGHT		1

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    SWIM_WINDOW_T iWin;
    T_uezDevice iLCD;
    TUInt16 iTopOutside;
    TUInt16 iLeftOutside;
    TUInt16 iTopInside;
    TUInt16 iLeftInside;
} T_FWTSTestWorkspace;

#define G_win (G_ws->iWin)

T_uezError DrawTestBox(T_FWTSTestWorkspace *G_ws)
{
	swim_set_pen_color(&G_win, YELLOW);
	swim_set_fill_color(&G_win, BLACK);
	swim_set_fill_transparent(&G_win, 0);

	//outside box
	swim_put_box(&G_win,
			G_ws->iLeftOutside,
			G_ws->iTopOutside,
			G_ws->iLeftOutside + TEST_BOX_OUTSIDE_SIZE -1,
			G_ws->iTopOutside + TEST_BOX_OUTSIDE_SIZE-1) ;

	swim_set_pen_color(&G_win, BLUE);
	swim_set_fill_color(&G_win, BLUE);

	//Draw inside box
	swim_put_box(&G_win,
				G_ws->iLeftInside,
				G_ws->iTopInside,
				G_ws->iLeftInside + TEST_BOX_INSIDE_SIZE-1,
				G_ws->iTopInside + TEST_BOX_INSIDE_SIZE -1);

	return UEZ_ERROR_NONE;
}

T_uezError FWTSTestScreen(T_FWTSTestWorkspace *G_ws)
{
	T_pixelColor *pixels;
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

	swim_set_pen_color(&G_win, YELLOW);
	swim_set_fill_color(&G_win, BLACK);
	swim_set_fill_transparent(&G_win, 1);

	//Make the screen Black
	swim_put_box(&G_win,
				 0,
				 0,
				 UEZ_LCD_DISPLAY_WIDTH,
				 UEZ_LCD_DISPLAY_HEIGHT);

	SUIShowPage0();
	return UEZ_ERROR_NONE;
}

void FWTSTest(void)
{
	T_uezDevice ts;
	static T_uezQueue queue = NULL;
	static T_FWTSTestWorkspace *G_ws = NULL;
	//T_uezTSReading reading;
	//INT_32 winX, winY;
	//T_uezError error;

	G_ws = UEZMemAlloc(sizeof(*G_ws));

	if (!G_ws)
		return;
	memset(G_ws, 0, sizeof(*G_ws));

	if (UEZQueueCreate(1, sizeof(T_uezTSReading), &queue) == UEZ_ERROR_NONE) {
		// Open up the touchscreen and pass in the queue to receive events
			if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
				// Open the LCD and get the pixel buffer
				if (UEZLCDOpen("LCD", &G_ws->iLCD) == UEZ_ERROR_NONE)  {

					FWTSTestScreen(G_ws);

					#if TEST_CENTER
						//Center Box
						G_ws->iTopOutside = (UEZ_LCD_DISPLAY_HEIGHT / 2) - (TEST_BOX_OUTSIDE_SIZE / 2);
						G_ws->iLeftOutside = (UEZ_LCD_DISPLAY_WIDTH / 2) - (TEST_BOX_OUTSIDE_SIZE / 2);
						G_ws->iTopInside = (UEZ_LCD_DISPLAY_HEIGHT / 2) - (TEST_BOX_INSIDE_SIZE / 2);
						G_ws->iLeftInside = (UEZ_LCD_DISPLAY_WIDTH / 2) - (TEST_BOX_INSIDE_SIZE / 2);
						DrawTestBox(G_ws);
					#endif

					#if TEST_TOP_LEFT
						//Center Box
						G_ws->iTopOutside = 10;//come down 10 pixels for all screens
						G_ws->iLeftOutside = 10;//come over 10 pixels for all screens
						G_ws->iTopInside = 10 + ((TEST_BOX_OUTSIDE_SIZE / 2)+ (TEST_BOX_INSIDE_SIZE / 2))-1;
						G_ws->iLeftInside = 10 + ((TEST_BOX_OUTSIDE_SIZE / 2)+ (TEST_BOX_INSIDE_SIZE / 2))-2;
						DrawTestBox(G_ws);
					#endif

					#if TEST_TOP_RIGHT
						//Center Box
						G_ws->iTopOutside = 10;//come down 10 pixels for all screens;
						G_ws->iLeftOutside = UEZ_LCD_DISPLAY_WIDTH - 10 - TEST_BOX_OUTSIDE_SIZE;
						G_ws->iTopInside = 10 + ((TEST_BOX_OUTSIDE_SIZE / 2)+ (TEST_BOX_INSIDE_SIZE / 2))-1;
						G_ws->iLeftInside = UEZ_LCD_DISPLAY_WIDTH - 10 -((TEST_BOX_OUTSIDE_SIZE / 2)+ (TEST_BOX_INSIDE_SIZE / 2))-2;
						DrawTestBox(G_ws);
					#endif

					#if TEST_BOTTOM_LEFT
						//Center Box
						G_ws->iTopOutside = UEZ_LCD_DISPLAY_HEIGHT - 10 - TEST_BOX_OUTSIDE_SIZE ;
						G_ws->iLeftOutside = 10;
						G_ws->iTopInside = UEZ_LCD_DISPLAY_HEIGHT - 10 - (TEST_BOX_OUTSIDE_SIZE/2) -TEST_BOX_INSIDE_SIZE +1;
						G_ws->iLeftInside = 10 + ((TEST_BOX_OUTSIDE_SIZE / 2)+ (TEST_BOX_INSIDE_SIZE / 2))-2;
						DrawTestBox(G_ws);
					#endif

					#if TEST_BOTTOM_RIGHT
						//Center Box
						G_ws->iTopOutside = UEZ_LCD_DISPLAY_HEIGHT - 10 - TEST_BOX_OUTSIDE_SIZE;
						G_ws->iLeftOutside = UEZ_LCD_DISPLAY_WIDTH - 10 - TEST_BOX_OUTSIDE_SIZE;
						G_ws->iTopInside = UEZ_LCD_DISPLAY_HEIGHT - 10 - (TEST_BOX_OUTSIDE_SIZE/2) -TEST_BOX_INSIDE_SIZE +1;
						G_ws->iLeftInside = UEZ_LCD_DISPLAY_WIDTH - 10 -((TEST_BOX_OUTSIDE_SIZE / 2)+ (TEST_BOX_INSIDE_SIZE / 2))-1;
						DrawTestBox(G_ws);
					#endif

					while(1)
						UEZTaskDelay(10);
				}
			}
	}

}