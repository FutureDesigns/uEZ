#ifndef _DIRECT_DRIVE_H_
#define _DIRECT_DRIVE_H_

#include "Config_Build.h"

//---------------------------------------------------------------------------
// LCD Timing and Size:
//---------------------------------------------------------------------------

#define LCD_FLIP_Y 1
#define LCD_FLIP_X 0

//#define DOT_INVERT
#define H_DOT_PULSE 41				// Horizontal pulse width (dot clocks inactive)
									//...specify value GREATER than min AND LESS than max (min < value < max)
#define H_DOT_BACK_PORCH 2			// Horizontal back porch (dot clocks Hsync active till display data)
#define H_DOT_DISPLAY 480L			// Horizontal pixels (active data)
#define H_DOT_FRONT_PORCH 40 		// Horizontal front porch (post data till Hsynch inactive)
									// problems with flickering may be addressed by increasing the front porch number
#define V_LINES_PULSE 10L			// Vertical pulse width (inactive pulse width) "VSYNC"

#define V_LINES_BACK_PORCH 2		// Vertical back porch (Lines Vsynch Active till display lines)

#define V_LINES_DISPLAY 272L		// Vertical lines (active data lines)
#define V_LINES_FRONT_PORCH 2 		// Minimum Vertical Front Porch (post display till Vsych inactive)
									// ...this may be extended by driver to meet requested behavior

#define DESIRED_FRAME_RATE  60 // 60 Hz

// Desired system behavior. Dot clock frequency must be integer multiple of BCLK and PCLK
#define DOT_CLOCK_FREQUENCY 12000000L

#define H_DOTS_PER_LINE   (H_DOT_PULSE+H_DOT_BACK_PORCH+H_DOT_DISPLAY+H_DOT_FRONT_PORCH)
#define V_LINES_PER_FRAME   (V_LINES_PULSE+V_LINES_BACK_PORCH+V_LINES_DISPLAY+V_LINES_FRONT_PORCH)

//---------------------------------------------------------------------------
// Frame Memory
//---------------------------------------------------------------------------
// Specify size of RAM frame...each dimension must be greater than or equal
// to panel dimension
#define DD_FRAME_HEIGHT    V_LINES_DISPLAY
#define DD_FRAME_WIDTH     H_DOT_DISPLAY

// Number of RAM frames to allocate
#define DD_NUM_FRAMES 4
typedef unsigned short pixel;
typedef struct { pixel raster[DD_FRAME_HEIGHT * DD_FRAME_WIDTH]; } DD_frame;
extern DD_frame DD_Frames[DD_NUM_FRAMES];

// Specify the maximum number of regions that the screen can be split into
// (Internally, each frame can be split among multiple regions of memory)
#define MAX_REGIONS_PER_FRAME 1

//---------------------------------------------------------------------------
// Globals:
//---------------------------------------------------------------------------
// frame counter since power up
extern unsigned short DD_FrameCount;
// flag indicating if we're running ExDMA on this line
extern unsigned short DD_IsBusActive;

//---------------------------------------------------------------------------
// Function prototypes:
//---------------------------------------------------------------------------
TUInt16 * LCDSetActiveRaster(TUInt32 frame);
short LCDSetLineSource(short Region, short LineCount, pixel *pSource, short LineStep);

//unsigned short LCDGetActiveFrame(void);
//short LCDSetFrameRate(short rate);
//short LCDGetFrameRate(void);
//short LCDSetRasterOffset(short x, short y);

//unsigned short LCDFlipDisplay(void);


int LCD_Init(void);
void LCD_Frames_Init(void);
void LCD_DISP_ON(void);
//void LCD_Frame_Clear(unsigned char frame, BMP_ColorTable_t const *const ct);
void app_lcd(void);

#endif // _DIRECT_DRIVE_H_

