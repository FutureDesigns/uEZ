/*-------------------------------------------------------------------------*
 * File:  FunctionalTest.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test procedure for ARM Carrier board with ARM7DIMM
 *
 * Implementation:
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * This software and associated documentation files (the "Software")
 * are copyright 2008 Future Designs, Inc. All Rights Reserved.
 *
 * A copyright license is hereby granted for redistribution and use of
 * the Software in source and binary forms, with or without modification,
 * provided that the following conditions are met:
 * 	-   Redistributions of source code must retain the above copyright
 *      notice, this copyright license and the following disclaimer.
 * 	-   Redistributions in binary form must reproduce the above copyright
 *      notice, this copyright license and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * 	-   Neither the name of Future Designs, Inc. nor the names of its
 *      subsidiaries may be used to endorse or promote products
 *      derived from the Software without specific prior written permission.
 *
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * 	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * 	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * 	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * 	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * 	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * 	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * 	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * 	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * 	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * 	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#if UEZ_ENABLE_LCD
#include <string.h>
#include <uEZTS.h>
#include <Device/Stream.h>
#include <uEZStream.h>
#include <Types/Serial.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Source/Library/Graphics/SWIM/lpc_swim_font.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include "Images.h"
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Constants and Macros:
 *---------------------------------------------------------------------------*/
#if UEZ_DEFAULT_LCD_RES_QVGA  
    #define TT_BUTTON_HEIGHT    32
    #define TT_BUTTON_WIDTH     64
    #define TT_ROW_HEIGHT       52
    #define TT_ROW_HEIGHT2      15
    #define TT_ROWS_TOP         9
    #define TT_COLUMN_1         13
    #define TT_COLUMN_2         113
    #define TT_COLUMN_3         213
#endif

#if UEZ_DEFAULT_LCD_RES_VGA  
    #define TT_BUTTON_HEIGHT    64
    #define TT_BUTTON_WIDTH     128
    #define TT_ROW_HEIGHT       104
    #define TT_ROW_HEIGHT2      30
    #define TT_ROWS_TOP         18
    #define TT_COLUMN_1         26
    #define TT_COLUMN_2         226
    #define TT_COLUMN_3         426
#endif

#define TT_ROW_1       (TT_ROWS_TOP+(TT_ROW_HEIGHT*0))
#define TT_ROW_2       (TT_ROWS_TOP+(TT_ROW_HEIGHT*1))
#define TT_ROW_3       (TT_ROWS_TOP+(TT_ROW_HEIGHT*2))
#define TT_ROW_4       (TT_ROWS_TOP+(TT_ROW_HEIGHT*3))
#define TT_ROW2_1       (TT_ROWS_TOP+(TT_ROW_HEIGHT2*0))
#define TT_ROW2_2       (TT_ROWS_TOP+(TT_ROW_HEIGHT2*1))
#define TT_ROW2_3       (TT_ROWS_TOP+(TT_ROW_HEIGHT2*2))+10
#define TT_ROW2_4       (TT_ROWS_TOP+(TT_ROW_HEIGHT2*3))+10



/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

#define OPT_BUTTON_SKIP             0x0001
#define OPT_BUTTON_YES              0x0002
#define OPT_BUTTON_NO               0x0004
#define OPT_BUTTON_CANCEL           0x0008
#define OPT_BUTTON_OK               0x0010
#define OPT_INIT                    0x8000


typedef struct {
    SWIM_WINDOW_T iWin;
    TUInt32 iTestNum;
    TBool iNeedInit;
    TUInt16 iButtons;
    TUInt16 iResult;
    TUInt8 iData[100];
    TBool iGoNext;
    TBool iExit;
    TUInt16 iLine;
    TBool iAllPass;
    TBool iPauseComplete;
} T_testData;

typedef struct {
    void (*iAddButtons)(T_testData *aData, TUInt16 aButtonTypes);
    void (*iRemoveButtons)(T_testData *aData, TUInt16 aButtonTypes);
    void (*iTextLine)(T_testData *aData, TUInt16 aLine, const char *aText);
    void (*iShowResult)(T_testData *aData, TUInt16 aLine, TUInt16 aResult, TUInt32 aDelay);
    void (*iSetTestResult)(T_testData *aData, TUInt16 aResult);
    void (*iNextTest)(T_testData *aData);
} T_testAPI;

typedef struct {
    const char *iTitle;
    void (*iMonitorFunction)(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
    TUInt16 iLinesNeeded;
    TBool iPauseOnFail;
} T_testState;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void TT_Exit2(const T_choice *aChoice);

static const T_choice G_ttChoices[] = {
    { TT_COLUMN_1, TT_ROW_4, TT_COLUMN_1+TT_BUTTON_WIDTH-1, TT_ROW_4+TT_BUTTON_HEIGHT-1, "Exit", TT_Exit2, G_exitIcon },
    { 0, 0, 0, 0, 0 },
};


/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

static SWIM_WINDOW_T G_ttWin;
static TBool G_ttExit;


typedef struct {
    TUInt16 iBit;
    TUInt16 iX1, iY1, iX2, iY2;
   const char *iText;
} T_button;

#define BUTTONS_LEFT_EDGE           10
#define BUTTONS_RIGHT_EDGE          305
#define BUTTONS_TOP                 170
#define BUTTONS_BOTTOM              210
#define BUTTONS_NUM                 5
#define BUTTONS_PADDING             10
#define BUTTON_WIDTH                ((BUTTONS_RIGHT_EDGE-BUTTONS_LEFT_EDGE-((BUTTONS_NUM-1)*BUTTONS_PADDING))/BUTTONS_NUM)
#define BUTTON_X_SPACING            (BUTTON_WIDTH+BUTTONS_PADDING)

#define BUTTON_LEFT(n)              ((BUTTON_X_SPACING*(n))+BUTTONS_LEFT_EDGE)
#define BUTTON_TOP(n)               (BUTTONS_TOP)
#define BUTTON_RIGHT(n)             (BUTTON_LEFT(n) + BUTTON_WIDTH - 1)
#define BUTTON_BOTTOM(n)            (BUTTONS_BOTTOM)

#define BUTTON_COORDS(n)            BUTTON_LEFT(n), BUTTON_TOP(n), BUTTON_RIGHT(n), BUTTON_BOTTOM(n)


void TT_Exit2(const T_choice *aChoice)
{
    G_ttExit = ETrue;
}


void FCTL_Screen(T_uezDevice lcd)
{
    T_pixelColor *pixels;

    SUIHidePage0();

    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
    swim_window_open(
        &G_ttWin,
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
    swim_set_font(&G_ttWin, &APP_DEMO_DEFAULT_FONT);
    swim_set_title(&G_ttWin, "Functional Test Loopback", BLUE);
    swim_set_pen_color(&G_ttWin, YELLOW);
    swim_set_fill_color(&G_ttWin, BLACK);

    swim_put_text_xy(&G_ttWin, "In this mode the ARM-TS-KIT serves as an RS232", TT_COLUMN_1, TT_ROW2_3);
    swim_put_text_xy(&G_ttWin, "and CAN loopback for functional test", TT_COLUMN_1, TT_ROW2_4);

    ChoicesDraw(&G_ttWin, G_ttChoices);

    SUIShowPage0();
}

/*---------------------------------------------------------------------------*
 * Task:  FunctionalTestLoop
 *---------------------------------------------------------------------------*
 * Description:
 *      This enters loopback mode on the CAN and RS232 ports; This mode allows
 *      an ARM TS KIT to serve as counterparty for loopback of CAN and RS232
 *      traffic during functional test of ARM TS KITs and other devices
 *
 * Inputs:
 *---------------------------------------------------------------------------*/
void FunctionalTestLoop(const T_choice *aChoice)
{
//    unsigned int CANbyteA;
//    unsigned int CANbyteB;
//    unsigned int CANTXbyteA;
//    unsigned int CANTXbyteB;


    typedef struct {
        T_uezDevice iDevice;
        DEVICE_STREAM **iStream;
        char iReceived[5];
        TUInt32 iCount;
        TUInt32 iStart;
    } T_serialTest;

    T_serialTest testData;

    T_serialTest *p = &testData ;

    TUInt32 num;

    T_uezDevice lcd;
    T_uezDevice ts;
    T_uezError error;
    T_uezQueue queue;

    G_ttExit = EFalse;


    if (UEZQueueCreate(1, sizeof(T_uezInputEvent), &queue) == UEZ_ERROR_NONE) {
        // Open up the touchscreen and pass in the queue to receive events
        if (UEZTSOpen("Touchscreen", &ts, &queue)==UEZ_ERROR_NONE)  {
            // Open the LCD and get the pixel buffer
            if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {

                error = UEZDeviceTableFind("Console", &p->iDevice);

                if (!error)
                    error = UEZDeviceTableGetWorkspace(p->iDevice, (T_uezDeviceWorkspace **)&p->iStream);
                if (error) {
                    return;
                }
                p->iCount = 0;


                // Flush the input buffer
                while ((*p->iStream)->Read(p->iStream, (TUInt8 *)p->iReceived, 5, &num, 500) != UEZ_ERROR_TIMEOUT) {
                }
                    // Put the draw screen up
                    FCTL_Screen(lcd);

                    // Sit here in a loop until we are done
                    while (!G_ttExit) {
                        ChoicesUpdate(&G_ttWin, G_ttChoices, queue, 500);
/*
                        if (CANReceive8(&CANbyteA, &CANbyteB)) // check for a couple of CAN bytes
                        {
                            CANTXbyteA = (CANbyteA + 0x01010101) ;
                            CANTXbyteB = (CANbyteB + 0x01010101) ;
                            CANSend8(CANTXbyteA, CANTXbyteB) ;
                        }
*/
                        // read a byte from the serial stream
                        num = 0;
                        (*p->iStream)->Read(p->iStream, (TUInt8 *)(p->iReceived + p->iCount), 1, &num, 100);
                        //p->iCount += num;
                        if (num == 1) {
                            if (memcmp(p->iReceived, "|", 1) == 0) {  // check to see if it's an or bar
                            // Send out "J.Ha" character string to the serial
                            error = (*p->iStream)->Write(p->iStream, "J.Ha", 4, &num, 400);  // reply
                                if (error) {
                                   return;
                                }
                            }
                        }
                  }
                  UEZLCDClose(lcd);
            }
            UEZTSClose(ts, queue);
        }
        UEZQueueDelete(queue);
      }
}

#endif // UEZ_ENABLE_LCD
