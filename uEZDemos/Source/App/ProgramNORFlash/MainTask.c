/*-------------------------------------------------------------------------*
 * File:  MainMenu.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Present the main menu
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm)+ Bootloader - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ+ Bootloader distribution.
 *
 * uEZ(tm)+ Bootloader is commercial software licensed under the
 * Software End-User License Agreement (EULA) delivered with this source
 * code package.  The Software and Documentation contain material that is
 * protected by United States Copyright Law and trade secret law, and by
 * international treaty provisions. All rights not granted to Licensee
 * herein are expressly reserved by FDI. Licensee may not remove any
 * proprietary notice of FDI from any copy of the Software or
 * Documentation.
 *
 * For more details, please review your EULA agreement included with this
 * software package.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uEZ.h>
#include <uEZFlash.h>
#include <uEZLCD.h>
#include <uEZPacked.h>
#include <uEZProcessor.h>
#include <uEZTS.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16_subset.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include "Calibration.h"
#include "NVSettings.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#ifndef BUTTON_HEIGHT
#define BUTTON_HEIGHT 100
#endif

#ifndef BUTTON_WIDTH
#define BUTTON_WIDTH  190
#endif

#define BUTTON1_X     5
#define BUTTON1_Y     (DISPLAY_HEIGHT-BUTTON_HEIGHT-5)
#define BUTTON2_X     (10+BUTTON_WIDTH)
#define BUTTON2_Y     (DISPLAY_HEIGHT-BUTTON_HEIGHT-5)
#define BUTTON3_X     (15+BUTTON_WIDTH*2)
#define BUTTON3_Y     (DISPLAY_HEIGHT-BUTTON_HEIGHT-5)

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_uezQueue G_tsQueue;
T_uezDevice G_ts;
SWIM_WINDOW_T G_win;
TBool G_screenOn = EFalse;

void print(const char *aString)
{
    swim_put_text(&G_win, aString);
    printf("%s", aString);
}

/*---------------------------------------------------------------------------*
 * Routine:  ScreenOn
 *---------------------------------------------------------------------------*/
/** Turn on the display if it is off (slowly bringing up the backlight).
 */
/*---------------------------------------------------------------------------*/
void ScreenOn(void)
{
    T_uezDevice lcd;
    int i;

    UEZLCDOpen("LCD", &lcd);
    if (!G_screenOn) {
        UEZLCDBacklight(lcd, 0);
        UEZLCDOn(lcd);
        for (i = 0; i < 255; i++) {
            UEZLCDBacklight(lcd, i);
            UEZTaskDelay(2);
        }
        G_screenOn = ETrue;
    }
    UEZLCDClose(lcd);
}

/*---------------------------------------------------------------------------*
 * Routine:  ScreenInit
 *---------------------------------------------------------------------------*/
/** Setup the screen with a title and display.  Initializes
 *  the variable G_win.
 */
/*---------------------------------------------------------------------------*/
void ScreenInit(void)
{
    T_uezDevice lcd;
    T_pixelColor *pixels;

    UEZLCDOpen("LCD", &lcd);
    UEZLCDGetFrame(lcd, 0, (void **)&pixels);
    UEZLCDBacklight(lcd, 0);
    UEZLCDOff(lcd);

    swim_window_open(&G_win, DISPLAY_WIDTH, DISPLAY_HEIGHT, pixels, 0, 0,
            DISPLAY_WIDTH - 1, DISPLAY_HEIGHT - 1, 1, WHITE, RGB(0, 0, 0),
            WHITE);

    swim_set_font(&G_win, &APP_DEMO_DEFAULT_FONT);

    print(PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner
    
    ScreenOn();
}

void PNFClearArea(TUInt32 baseAddr, TUInt32 aSize)
{
    TUInt32 *p = (TUInt32 *)baseAddr;

    // Fill region with 0xDEADBEEF markers
    aSize /= 4;
    while (aSize--) {
        *(p++) = 0xDEADBEEF;
    }
}

TUInt32 PNFGetSize(TUInt32 baseAddr, TUInt32 aSize)
{
    TUInt32 *p = (TUInt32 *)baseAddr;

    // Fill region with 0xDEADBEEF markers
    aSize /= 4;
    while (aSize) {
        if (p[aSize-1] != 0xDEADBEEF)
            break;
        aSize--;
    }

    if (aSize == 0)
        aSize = 1;
    return aSize*4;
}

/*---------------------------------------------------------------------------*
 * Routine:  MainMenu
 *---------------------------------------------------------------------------*/
/** Main routine for the bootloader.  Present the screen and decide how
 *  to go forward.
 */
/*---------------------------------------------------------------------------*/
void MainMenu(void)
{
    T_uezDevice lcd;
    char line[100];
    TUInt32 size;
    T_uezDevice flash;

    NVSettingsLoad();

    // Open the LCD and get the pixel buffer
    UEZLCDOpen("LCD", &lcd);

    // Setup the LCD, setup the window, and clear the screen
    ScreenInit();

    //Calibrate();

    //UEZQueueCreate(1, sizeof(T_uezInputEvent), &G_tsQueue);
    //UEZTSOpen("Touchscreen", &G_ts, &G_tsQueue);

    print("Clearing receive area ...\n");
    PNFClearArea(IMAGE_ADDR, IMAGE_SIZE);

    sprintf(line, "Waiting for image at 0x%08lX ... \n", IMAGE_ADDR);
    print(line);

    // Wait for an image to appear
    JLINK_MODE = 0;
    while (JLINK_MODE != 1) {
        UEZTaskDelay(10);
    }

    print("Image received.\n");

    print("Determing size ... ");
    size = PNFGetSize(IMAGE_ADDR, IMAGE_SIZE);
    sprintf(line, "%u bytes\n", size);
    print(line);

    print("Erasing ...");
    UEZFlashOpen("Flash0", &flash);
    UEZFlashBlockErase(flash, 0, size);
    print("done.\n");

    print("Writing ...");
    UEZFlashWrite(flash, 0, (void *)IMAGE_ADDR, size);
    print("done.\n");
    UEZFlashClose(flash);

    print("\n** NOR Flash Programming Complete. **\n");

    JLINK_MODE = 2;

    while (1) {
        UEZTaskDelay(10);
    }
}

/*-------------------------------------------------------------------------*
 * End of File:  MainMenu.c
 *-------------------------------------------------------------------------*/
