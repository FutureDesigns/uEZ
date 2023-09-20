/*-------------------------------------------------------------------------*
* File:  SdReliabilityTest.c
*-------------------------------------------------------------------------*
* Description:
*     Run basic testing on SD cards, reboot periodically.
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
#include <uEZProcessor.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_Watchdog.h> 

#define INDENT              75
#define INDENTNUM           350

// GUI variables
SWIM_WINDOW_T SdTestWin;
TUInt16 SdTestFontHeight;
COLOR_T g_backgroundColorSDTest = RGB(0, 0, 0); // inital background color is black
char bufferSDTestA[16];
char bufferSDTestB[16];
char bufferSDTestC[16];
char bufferSDTestD[16];
char bufferSDTestE[16];
char bufferSDTestF[16];
char bufferSDTestG[16];

// Test Variables
TUInt32 cardCapacity = 0;
TUInt32 countFile = 0;
TUInt32 fileSizeKB = 0;

bool flagSDTestA = false, flagSDTestB = false, flagSDTestC = false, flagSDTestD = false,  flagSDTestE = false, flagSDTestF = false, flagSDTestG = false;
bool changeScreenflagSDTestSDTest = false;

// Prototypes
void clearRow(TUInt32 row);
void SdReliabilityTestScreen(void);
void SdReliabilityTestUpdateScreen(void);
void SdReliabilityTestProcedure(T_pixelColor *aPixels, T_uezDevice ts);

void clearRow(TUInt32 row) {
  swim_set_pen_color(&SdTestWin, g_backgroundColorSDTest); // make sure to use background color to "erase" text
  swim_put_box(&SdTestWin, 
               (INDENTNUM-5),     (row*SdTestFontHeight),
               (INDENTNUM+100),   ((row+1)*SdTestFontHeight));  
  swim_set_pen_color(&SdTestWin,WHITE);  
}

/*---------------------------------------------------------------------------*
* Routine:  SdReliabilityTestUpdateScreen
*---------------------------------------------------------------------------*
* Description:
*      Update numbers
* Inputs:
*---------------------------------------------------------------------------*/
void SdReliabilityTestUpdateScreen(void) {  
  if(flagSDTestA) { // Card capacity
    flagSDTestA = false;
    clearRow(3);
    sprintf(bufferSDTestA, "%u", cardCapacity); 
    swim_put_text_xy(&SdTestWin, bufferSDTestA, INDENTNUM, (3*SdTestFontHeight)); // pen down
  }
  
  if(flagSDTestB) { // File Counter
    flagSDTestB = false;
    clearRow(5);  
    sprintf(bufferSDTestB, "%u", countFile);
    swim_put_text_xy(&SdTestWin, bufferSDTestB, INDENTNUM, (5*SdTestFontHeight)); // pen up    
  }
  
  if(flagSDTestC) { // update file size
    flagSDTestC = false;
    clearRow(7);
    sprintf(bufferSDTestC, "%u KB", fileSizeKB);
    swim_put_text_xy(&SdTestWin, bufferSDTestC, INDENTNUM, (7*SdTestFontHeight)); // moves
  }
  
  if(flagSDTestD) { // show only if init failure
    flagSDTestD = false;
    clearRow(9);
    sprintf(bufferSDTestD, "Initialize Failure!");
    swim_put_text_xy(&SdTestWin, bufferSDTestD, INDENTNUM, (9*SdTestFontHeight));    
  }
  
  if(flagSDTestE) { // show only on read failure
    flagSDTestE = false;
    clearRow(11);
    sprintf(bufferSDTestE, "File Read Failure!");
    swim_put_text_xy(&SdTestWin, bufferSDTestE, INDENTNUM, (11*SdTestFontHeight));
  }
  
  if(flagSDTestF) { // show only on file write failure
    flagSDTestF = false;
    clearRow(13);
    sprintf(bufferSDTestF, "File Write Failure!");
    swim_put_text_xy(&SdTestWin, bufferSDTestF, INDENTNUM, (13*SdTestFontHeight)); 
  }
  
  if(flagSDTestG) { // show on file delete failure
    flagSDTestG = false;
    clearRow(15);
    sprintf(bufferSDTestG, "File Delete Failure!");
    swim_put_text_xy(&SdTestWin, bufferSDTestG, INDENTNUM, (15*SdTestFontHeight)); 
  }
}

/*---------------------------------------------------------------------------*
* Routine:  SdReliabilityTestScreen
*---------------------------------------------------------------------------*
* Description:
*      Draw the screen used for sd test.
* Inputs:
*---------------------------------------------------------------------------*/
void SdReliabilityTestScreen(void) {
  T_uezDevice lcd;
  T_pixelColor *pixels;
  
  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
    if(!changeScreenflagSDTestSDTest) { // speed up on background switch?
      UEZLCDOn(lcd);
      SUIHidePage0();
    }
    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
    swim_window_open(
                     &SdTestWin,
                     DISPLAY_WIDTH,
                     DISPLAY_HEIGHT,
                     pixels,
                     0,
                     0,
                     DISPLAY_WIDTH-1,
                     DISPLAY_HEIGHT-1,
                     2,
                     YELLOW,
                     g_backgroundColorSDTest,
                     RED);
    swim_set_font(&SdTestWin, &APP_DEMO_DEFAULT_FONT);
    swim_set_title(&SdTestWin, "SD Card Reliability Test", BLUE);
    SdTestFontHeight =  swim_get_font_height(&SdTestWin);
    swim_put_text_horizontal_centered(&SdTestWin,
                                      "Testing SD card until failure!",
                                      DISPLAY_WIDTH, (SdTestFontHeight));
    
    swim_put_text_xy(&SdTestWin, "Card Capacity: ",
                     INDENT, (3*SdTestFontHeight));    
    swim_put_text_xy(&SdTestWin, "File Counter: ",
                     INDENT, (5*SdTestFontHeight));
    swim_put_text_xy(&SdTestWin, "Size of current file: ",
                     INDENT, (7*SdTestFontHeight));  
    /*swim_put_text_xy(&SdTestWin, "Initialize Failure!",
    INDENT, (9*SdTestFontHeight));
    swim_put_text_xy(&SdTestWin, "File Read Failure!",
    INDENT, (11*SdTestFontHeight));
    swim_put_text_xy(&SdTestWin, "File Write Failure!",
    INDENT, (13*SdTestFontHeight));
    swim_put_text_xy(&SdTestWin, "File Delete Failure!",
    INDENT, (15*SdTestFontHeight));*/
    
    swim_put_text_xy(&SdTestWin, " ", INDENTNUM, (3*SdTestFontHeight)); // 
    swim_put_text_xy(&SdTestWin, " ", INDENTNUM, (5*SdTestFontHeight)); // 
    swim_put_text_xy(&SdTestWin, " ", INDENTNUM, (7*SdTestFontHeight)); // 
    //swim_put_text_xy(&SdTestWin, " ", INDENTNUM, (9*SdTestFontHeight)); // 
    //swim_put_text_xy(&SdTestWin, " ", INDENTNUM, (11*SdTestFontHeight)); // 
    //swim_put_text_xy(&SdTestWin, " ", INDENTNUM, (13*SdTestFontHeight)); //
    //swim_put_text_xy(&SdTestWin, " ", INDENTNUM, (15*SdTestFontHeight)); // 
    swim_set_fill_color(&SdTestWin,g_backgroundColorSDTest);
    
    if(!changeScreenflagSDTestSDTest) {
      SUIShowPage0();
    }
  }
}

/*---------------------------------------------------------------------------*
* Routine:  SdReliabilityTestProcedure
*---------------------------------------------------------------------------*
* Description:
*      Start the SD card reliability test
* Inputs:
*      TUInt16 *aPixels            -- Pointer to pixel memory
*      T_uezDevice ts              -- Pointer to touchscreen device
*---------------------------------------------------------------------------*/
void SdReliabilityTestProcedure(T_pixelColor *aPixels, T_uezDevice ts) {    
  //T_uezError error;
  //T_uezError errorSD;
  T_uezFile file;
  TUInt8 writebuf[1024];
  TUInt8 readbuf[1024];
  TUInt32 bytesread = 0;
  TUInt32 byteswritten = 0;
  SdReliabilityTestScreen(); // display screen
  
/** Note that today this test only writes and rewrites a small 1KB file.
  * However this test is enough to find some troublesome SD cards.
  * In this spot you can easily modify the code to write repeating patterns
  * in larger files for more extensive testing.
  */  
  
  while (1) {
    // init buffers, so far just doing simple test
    for (TUInt16 i = 0; i < 1024; i++){
      writebuf[i] = i;
      readbuf[i] = 0;
    }
    
    // open file on SD card
    if (UEZFileOpen("1:TESTFILE.TXT", FILE_FLAG_WRITE, &file) == UEZ_ERROR_NONE) {
      // write file
      if (UEZFileWrite(file, writebuf, sizeof(writebuf), &byteswritten) == UEZ_ERROR_NONE) {
        UEZFileClose(file);
        if (UEZFileOpen("1:TESTFILE.TXT", FILE_FLAG_READ_ONLY, &file) == UEZ_ERROR_NONE) {
          if (UEZFileRead(file, readbuf, sizeof(readbuf), &bytesread) == UEZ_ERROR_NONE) {
            for (TUInt16 i = 0; i < 1024; i++){
              if(writebuf[i] != readbuf[i]){
                // error verify                            
                flagSDTestE = 1;                    
                flagSDTestF = 1;
              } else { // pass
              }                  
            }
            UEZFileClose(file);
            if (UEZFileDelete("1:TESTFILE.TXT") == UEZ_ERROR_NONE) {               
            }else { // failed to delete
              flagSDTestG = 1;
            }
          } else {// failed to read 
            flagSDTestE = 1;
          }
        } else { // failed to open          
          flagSDTestD = 1;
        }        
      } else {// failed to write        
        flagSDTestF = 1;
      }            
    } else { // open failure
      flagSDTestD = 1;
    }
    
    //cardCapacity = 0;
    countFile++; flagSDTestB = 1;
    //fileSizeKB = 0;
    
    if((flagSDTestD == 1) | (flagSDTestE == 1) | (flagSDTestF == 1) | (flagSDTestG == 1)) {// change background color to signify any failure
      if(!changeScreenflagSDTestSDTest){
        changeScreenflagSDTestSDTest = true;
        g_backgroundColorSDTest = RGB(0x80, 0, 0); // dark red
        SdReliabilityTestScreen(); // redraw screen with new color
        SdReliabilityTestUpdateScreen();
        while(1){
          ; // stay on red scren so you can see failure
        }
      }
    }
    SdReliabilityTestUpdateScreen();
    UEZTaskDelay(1000);
    
    if(countFile == 10){ // run 10 times then reboot after testing all files on read/write/delete       
	  // Reboot option 1: Software reset, not used
      //portENABLE_INTERRUPTS();
      //NVIC_SystemReset();
      
      // Reboot option 2: Get bit by the angry dog
      #include <uEZWatchdog.h>
      UEZPlatform_Watchdog_Require();
      T_uezDevice WD;
      UEZWatchdogOpen("Watchdog", &WD);
      UEZWatchdogSetMinTime(WD,0xFF);
      UEZWatchdogSetMaxTime(WD,0xFFFF);
      UEZWatchdogStart(WD);
      UEZWatchdogFeed(WD);
      UEZWatchdogTrip(WD);
    }    
  }
}

/*---------------------------------------------------------------------------*
* Task:  SdReliabilityTest
*---------------------------------------------------------------------------*
* Description:
*      Setup for sd reliability test
*---------------------------------------------------------------------------*/
void SdReliabilityTest(void) {
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
      UEZQueueSetName(queue, "SdReliabilityTest", "\0");
#endif
#endif
      // Open up the touchscreen and pass in the queue to receive events
      if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
        // Open the LCD and get the pixel bufferSDTest
        if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
          UEZLCDBacklight(lcd, 256);
          UEZLCDGetFrame(lcd, 0, (void **)&pixels);                            
          SdReliabilityTestProcedure(pixels, ts); // run noise detect
        }
        UEZTSClose(ts, queue);
      }
#ifndef NO_DYNAMIC_MEMORY_ALLOC	
      UEZQueueDelete(queue);
#endif
    }
  }
  /*-------------------------------------------------------------------------*
  * File:  SdReliabilityTest.c
  *-------------------------------------------------------------------------*/
  