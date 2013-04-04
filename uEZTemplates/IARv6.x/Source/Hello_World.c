/*-------------------------------------------------------------------------*
 * File:  
 *-------------------------------------------------------------------------*
 * Description:
 *      
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
#include <uEZ.h>
#include <uEZLCD.h>
#if UEZ_ENABLE_LCD
#include "Config_App.h"
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static TBool G_HelloWorldExit;
static SWIM_WINDOW_T G_HelloWorldWin;

/*---------------------------------------------------------------------------*
 * Routine:  
 *---------------------------------------------------------------------------*
 * Description:
 *     
 * Inputs:
 *      
 *---------------------------------------------------------------------------*/
void HelloWorld(void)
{
    T_uezDevice lcd;
    T_uezDevice ts;
    T_pixelColor *pixels;
    static T_uezQueue queue;
    
    G_HelloWorldExit = EFalse;
    
    //Create the touch screen
    if (UEZQueueCreate(1, sizeof(T_uezTSReading), &queue) == UEZ_ERROR_NONE) {
        //Open the touch screen for use
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
                UEZLCDGetFrame(lcd, 0, (void **)&pixels);
                
                //Open the swim window and setup the size
                swim_window_open(&G_HelloWorldWin,
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
                //Set the font for swim to use
                swim_set_font(&G_HelloWorldWin, &APP_DEMO_DEFAULT_FONT);
                //Set the pen color
                swim_set_pen_color(&G_HelloWorldWin, YELLOW);
                //Set the fille color
                swim_set_fill_color(&G_HelloWorldWin, BLACK);
                
                //blank the screen
                swim_put_box(&G_HelloWorldWin,
                             0,
                             0,
                             DISPLAY_WIDTH-1,
                             DISPLAY_HEIGHT-1);
                
                swim_put_text_xy(&G_HelloWorldWin,
                                 "Hello World!",
                                 ((DISPLAY_WIDTH / 2) - ( swim_get_text_line_width(&G_HelloWorldWin, "Hello World!") /2 )) ,
                                 (( DISPLAY_HEIGHT / 2) - (swim_get_font_height(&G_HelloWorldWin) / 2)));
                
                while (!G_HelloWorldExit) {            
                  
                }
            }
        }
    }    
}
#endif
/*-------------------------------------------------------------------------*
 * End of File:  Hello_World.c
 *-------------------------------------------------------------------------*/