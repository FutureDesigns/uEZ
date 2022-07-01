/*-------------------------------------------------------------------------*
* File:  TS_NoiseDetect.c
*-------------------------------------------------------------------------*
* Description:
*     Detect and count various touchscreen events.
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
#include <uEZTS.h>
#include <uEZLCD.h>
#include "uEZDemoCommon.h"
#include "AppMenu.h"
#include "Audio.h"
#include <uEZLCD.h>
#include <stdio.h>
#include <stdbool.h>

#define INDENT              75
#define INDENTNUM           350

SWIM_WINDOW_T noiseDetectWin;
TUInt16 noiseDetectFontHeight;
TUInt32 countPenUp = 0;
TUInt32 countPenDown = 0;
TUInt32 countMove = 0;
TUInt32 countHold = 0;
TUInt32 countNone = 0;
T_uezTSReading currentReading;
COLOR_T g_backgroundColor = RGB(0, 0, 0); // inital background color is black

char bufferA[16];
char bufferB[16];
char bufferC[16];
char bufferD[16];
char bufferE[16];
char bufferF[16];
char bufferG[16];

bool flagA = false, flagB = false, flagC = false, flagD = false,  flagE = false, flagF = false, flagG = false;
bool changeScreenFlag = false;

// Prototypes

void TS_clearNumber(TUInt32 row);
void TS_NoiseDetectScreen(void);
void TS_NoiseDetectUpdateScreen(void);
void TS_NoiseDetectProcedure(T_pixelColor *aPixels, T_uezDevice ts);

void TS_clearNumber(TUInt32 row) {
  swim_set_pen_color(&noiseDetectWin, g_backgroundColor); // make sure to use background color to "erase" text
  swim_put_box(&noiseDetectWin, 
               (INDENTNUM-5),     (row*noiseDetectFontHeight),
               (INDENTNUM+100),   ((row+1)*noiseDetectFontHeight));  
  swim_set_pen_color(&noiseDetectWin,WHITE);  
}

/*---------------------------------------------------------------------------*
* Routine:  TS_NoiseDetectUpdateScreen
*---------------------------------------------------------------------------*
* Description:
*      Update numbers
* Inputs:
*---------------------------------------------------------------------------*/
void TS_NoiseDetectUpdateScreen(void) {  
  if(flagA) {
    flagA = false;
    TS_clearNumber(3);
    sprintf(bufferA, "%u", countPenDown); 
    swim_put_text_xy(&noiseDetectWin, bufferA, INDENTNUM, (3*noiseDetectFontHeight)); // pen down
  }
  
  if(flagB) {
    flagB = false;
    TS_clearNumber(5);  
    sprintf(bufferB, "%u", countPenUp);
    swim_put_text_xy(&noiseDetectWin, bufferB, INDENTNUM, (5*noiseDetectFontHeight)); // pen up    
  }
  
  if(flagC) { // not supported yet
    flagC = false;
    TS_clearNumber(7);
    sprintf(bufferC, "%u", countMove);
    swim_put_text_xy(&noiseDetectWin, bufferC, INDENTNUM, (7*noiseDetectFontHeight)); // moves
  }
  
  if(flagD) { // not supported yet
    flagD = false;
    TS_clearNumber(9);
    sprintf(bufferD, "%u", countHold);
    swim_put_text_xy(&noiseDetectWin, bufferD, INDENTNUM, (9*noiseDetectFontHeight)); // hold
  }
  
  if(flagE) { // not seen on some touch drivers?
    flagE = false;
    TS_clearNumber(11);
    sprintf(bufferE, "%u", countNone);
    swim_put_text_xy(&noiseDetectWin, bufferE, INDENTNUM, (11*noiseDetectFontHeight)); // none
  }
  
  if(flagF) {
    flagF = false;
    TS_clearNumber(13);
    sprintf(bufferF, "%ux%u", currentReading.iX, currentReading.iY);
    swim_put_text_xy(&noiseDetectWin, bufferF, INDENTNUM, (13*noiseDetectFontHeight)); //coordinates
  }
  
  if(flagG) { // optional
    flagG = false;
    TS_clearNumber(15);
    sprintf(bufferG, "%u", currentReading.iPressure);
    swim_put_text_xy(&noiseDetectWin, bufferG, INDENTNUM, (15*noiseDetectFontHeight)); // Pressure
  }
}

/*---------------------------------------------------------------------------*
* Routine:  TS_NoiseDetectScreen
*---------------------------------------------------------------------------*
* Description:
*      Draw the screen used for noise detect.
* Inputs:
*---------------------------------------------------------------------------*/
void TS_NoiseDetectScreen(void) {
  T_uezDevice lcd;
  T_pixelColor *pixels;
  
  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
    if(!changeScreenFlag) { // speed up on background switch?
      UEZLCDOn(lcd);
      SUIHidePage0();
    }
    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
    swim_window_open(
                     &noiseDetectWin,
                     DISPLAY_WIDTH,
                     DISPLAY_HEIGHT,
                     pixels,
                     0,
                     0,
                     DISPLAY_WIDTH-1,
                     DISPLAY_HEIGHT-1,
                     2,
                     YELLOW,
                     g_backgroundColor,
                     RED);
    swim_set_font(&noiseDetectWin, &APP_DEMO_DEFAULT_FONT);
    swim_set_title(&noiseDetectWin, "Touch Screen Noise Detection Test", BLUE);
    noiseDetectFontHeight =  swim_get_font_height(&noiseDetectWin);
     
    
    swim_put_text_horizontal_centered(&noiseDetectWin,
                                      "Do not touch screen while running!",
                                      DISPLAY_WIDTH, (noiseDetectFontHeight));
    
    swim_put_text_xy(&noiseDetectWin, "Number of pen down detected so far: ",
                     INDENT, (3*noiseDetectFontHeight));    
    swim_put_text_xy(&noiseDetectWin, "Number of pen up detected so far: ",
                     INDENT, (5*noiseDetectFontHeight));
    swim_put_text_xy(&noiseDetectWin, "Number of moves detected so far: (not supported)",
                     INDENT, (7*noiseDetectFontHeight));  
    swim_put_text_xy(&noiseDetectWin, "Number of holds detected so far: (not supported)",
                     INDENT, (9*noiseDetectFontHeight));
    swim_put_text_xy(&noiseDetectWin, "Number of no touch detected so far: (not supported)",
                     INDENT, (11*noiseDetectFontHeight));
    swim_put_text_xy(&noiseDetectWin, "Current touch coordinates: ",
                     INDENT, (13*noiseDetectFontHeight));
    swim_put_text_xy(&noiseDetectWin, "Current pressure if supported: ",
                     INDENT, (15*noiseDetectFontHeight));
    
    swim_put_text_xy(&noiseDetectWin, "0", INDENTNUM, (3*noiseDetectFontHeight)); // pen down
    swim_put_text_xy(&noiseDetectWin, "0", INDENTNUM, (5*noiseDetectFontHeight)); // pen up
    swim_put_text_xy(&noiseDetectWin, "0", INDENTNUM, (7*noiseDetectFontHeight)); // moves
    swim_put_text_xy(&noiseDetectWin, "0", INDENTNUM, (9*noiseDetectFontHeight)); // hold
    swim_put_text_xy(&noiseDetectWin, "0", INDENTNUM, (11*noiseDetectFontHeight)); // none
    swim_put_text_xy(&noiseDetectWin, "XXXxXXX", INDENTNUM, (13*noiseDetectFontHeight)); //coordinates
    swim_put_text_xy(&noiseDetectWin, "0", INDENTNUM, (15*noiseDetectFontHeight)); // none
    
    swim_set_fill_color(&noiseDetectWin,g_backgroundColor);
    
    if(!changeScreenFlag) {
      SUIShowPage0();
    }
  }
}

/*---------------------------------------------------------------------------*
* Routine:  TS_NoiseDetectProcedure
*---------------------------------------------------------------------------*
* Description:
*      Start the Noise detect routine
* Inputs:
*      TUInt16 *aPixels            -- Pointer to pixel memory
*      T_uezDevice ts              -- Pointer to touchscreen device
*---------------------------------------------------------------------------*/
void TS_NoiseDetectProcedure(T_pixelColor *aPixels, T_uezDevice ts) {    
  //T_uezError error;
  TS_NoiseDetectScreen(); // display screen
    
  while (1) {
    if (UEZTSGetReading(ts, &currentReading)==UEZ_ERROR_NONE)  {
      if (!(currentReading.iFlags & TSFLAG_PEN_DOWN))  { // Pen up flag
        countPenUp++;
        flagB = true;
      } else if (currentReading.iFlags & TSFLAG_PEN_DOWN)  { // Pen down flag
        countPenDown++;
        flagA = true;
        flagF = true;
        flagG = true;
        //ButtonClick(); // disable audio so it doesn't blow out the speaker and drive everyone crazy.
      } else { // not recoginized flag
        countNone++;
        flagE = true;
      }           
    } else { // did not get TS reading
      countNone++;
      flagE = true;
    }
    if(countPenDown == 1) {// change background color to signify touch, but only once      
      if(!changeScreenFlag){
        changeScreenFlag = true;
        g_backgroundColor = RGB(0x80, 0, 0); // dark red
        TS_NoiseDetectScreen(); // redraw screen with new color    
      }   
    }
    TS_NoiseDetectUpdateScreen();
    UEZTaskDelay(1);
  }
}

/*---------------------------------------------------------------------------*
* Task:  TS_NoiseDetect
*---------------------------------------------------------------------------*
* Description:
*      Setup for noise detect
*---------------------------------------------------------------------------*/
void TS_NoiseDetect(void) {
  T_uezDevice ts;
  T_uezDevice lcd;
  T_pixelColor *pixels;
  static T_uezQueue queue = 0;
  
  // Setup queue to receive touchscreen events
#ifdef NO_DYNAMIC_MEMORY_ALLOC	
  if (NULL == queue) {
    if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) != UEZ_ERROR_NONE) {		
      queue = NULL;
    }
  }
  
  if (NULL != queue) {
    /* Register the queue so that the IAR Stateviewer Plugin knows about it. */
    UEZQueueAddToRegistry( queue, "TS Noise Detect" );
#else
    if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
#if UEZ_REGISTER
      UEZQueueSetName(queue, "TS_NoiseDetect", "\0");
#endif
#endif
      // Open up the touchscreen and pass in the queue to receive events
      if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
        // Open the LCD and get the pixel buffer
        if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
          UEZLCDBacklight(lcd, 256);
          UEZLCDGetFrame(lcd, 0, (void **)&pixels);                            
          TS_NoiseDetectProcedure(pixels, ts); // run noise detect
        }
        UEZTSClose(ts, queue);
      }
#ifndef NO_DYNAMIC_MEMORY_ALLOC	
      UEZQueueDelete(queue);
#endif
    }
  }
  /*-------------------------------------------------------------------------*
  * File:  TS_NoiseDetect.c
  *-------------------------------------------------------------------------*/
  