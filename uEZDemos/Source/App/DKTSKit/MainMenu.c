/*-------------------------------------------------------------------------*
* File:  MainMenu.c
*-------------------------------------------------------------------------*
* Description:
*     Present the main menu
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

#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZTS.h>
#include <uEZLCD.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_DrawBitmap.h>
#include <Source/Library/Screensaver/BouncingLogoSS.h>
#include "AppDemo.h"
#include <uEZDemoCommon.h>
#include <Calibration.h>
#include <uEZDeviceTable.h>
#include "../GUICommon/FDI_emWin_Demo.h"
#include <UEZLCD.h>

/*---------------------------------------------------------------------------*
* Constants:
*---------------------------------------------------------------------------*/
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
* Globals:
*---------------------------------------------------------------------------*/
static SWIM_WINDOW_T G_mmWin;
static TUInt32 G_romChecksum;
static TBool G_romChecksumCalculated;

TUInt32 ROMChecksumCalculate()
{
    TUInt8 *p = (TUInt8 *)0;
    TUInt32 checksum = 0;
    TUInt32 count;

    for (count=32; count<0x7E000; count++, p) {
        checksum += *(p++);
    }

    return checksum;
}

TUInt32 CalcChecksumTask(T_uezTask aMyTask, void *aParams)
{
    PARAM_NOT_USED(aMyTask);
    PARAM_NOT_USED(aParams);
    G_romChecksum = ROMChecksumCalculate();
    G_romChecksumCalculated = ETrue;
    return 0;
}
#if APP_DEMO_APPS
static const T_appMenuEntry apps_menu_entries[] = {
    { "Accelerometer", AccelDemoMode, G_accelIcon, 0 },
    { "Time & Date", TimeDateMode, G_timeDateIcon, 0 },
    { "Temperature", TempMode, G_temperatureIcon, 0 },
#if APP_DEMO_EMWIN
{ "emWin Demo", emWin, G_segger, 0 },
#endif
{ 0 },
};

static const T_appMenu apps_submenu = {
    PROJECT_NAME " " VERSION_AS_TEXT " - Apps",
    apps_menu_entries,
    ETrue, // can exit
};
#endif

static const T_appMenuEntry settings_menu_entries[] = {
    { "Brightness", BrightnessControlMode, G_contrastIcon, 0 },
    { "Calibrate", CalibrateMode, G_calibrationIcon, 0 },
    { "Functional Test", FunctionalTest, G_funcTestIcon, 0 },
#if ((FDI_PLATFORM == CARRIER_R4) | (FDI_PLATFORM == CARRIER_R4))
{ "FCT Loopback", FunctionalTestLoop, G_funcTestLoopIcon, 0 },
#endif
{ 0 },
};
static const T_appMenu settings_submenu = {
    PROJECT_NAME " " VERSION_AS_TEXT " - Settings",
    settings_menu_entries,
    ETrue, // can exit
};

#if APP_DEMO_COM
static const T_appMenuEntry comm_menu_entries[] = {
    { "Console", ConsoleMode, G_consoleIcon, 0 },
    { 0 },
};

static const T_appMenu comm_submenu = {
    PROJECT_NAME " " VERSION_AS_TEXT " - Settings",
    comm_menu_entries,
    ETrue, // can exit
};
#endif

static const T_appMenuEntry mainmenu_entries[] = {
    { "Slideshow", MultiSlideshowMode, G_slideshowIcon, 0 },
#if APP_DEMO_APPS
{ "Apps", AppSubmenu, G_appFolderIcon, (void *)&apps_submenu },
#endif
#if APP_DEMO_COM
{ "Communications", AppSubmenu, G_consoleIcon, (void *)&comm_submenu },
#endif
{ "Settings", AppSubmenu, G_settingsIcon, (void *)&settings_submenu },
#if APP_DEMO_DRAW
{ "Draw", DrawMode, G_drawIcon, 0 },
#endif
#if APP_DEMO_EMWIN
{ "emWin Demo", emWin, G_segger, 0 },
#endif
#if APP_DEMO_YOUR_APP
{ "Your App Here!", YourAppMode, G_questionIcon, 0 },
#endif
{ 0 },
};
static const T_appMenu mainmenu = {
    PROJECT_NAME " " VERSION_AS_TEXT " - Main Menu",
    mainmenu_entries,
    EFalse, // cannot exit
};


/*---------------------------------------------------------------------------*
* Routine:  TitleScreen
*---------------------------------------------------------------------------*
* Description:
*      Draw the title screen at boot
*---------------------------------------------------------------------------*/
void TitleScreen(void)
{
    T_uezDevice lcd;
    T_pixelColor *pixels;
    TUInt16 i;
    char buffer[15];

    G_romChecksumCalculated = EFalse;
    UEZTaskCreate(
                  (T_uezTaskFunction)CalcChecksumTask,
                  "Chksum",
                  UEZ_TASK_STACK_BYTES(128),
                  (void *)0,
                  UEZ_PRIORITY_NORMAL,
                  0);

    if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
        UEZLCDGetFrame(lcd, 0, (void **)&pixels);
        UEZLCDBacklight(lcd, 0);
        UEZLCDOff(lcd);

        SUIHidePage0();

        swim_window_open(
                         &G_mmWin,
                         DISPLAY_WIDTH,
                         DISPLAY_HEIGHT,
                         pixels,
                         0,
                         0,
                         DISPLAY_WIDTH-1,
                         DISPLAY_HEIGHT-1,
                         2,
                         BLACK,
                         RGB(0, 0, 0),
                         RED);

        swim_set_font(&G_mmWin, &APP_DEMO_DEFAULT_FONT);
		
		SUIDrawBitmap(
			G_uEZLogo,
            (DISPLAY_WIDTH-UEZ_ICON_WIDTH)/2,
            (DISPLAY_HEIGHT-UEZ_ICON_HEIGHT)/2);

        swim_set_font_transparency(&G_mmWin, 1);
        /*swim_put_text_xy(
            &G_mmWin,
            VERSION_AS_TEXT,
            DISPLAY_WIDTH-60,
            DISPLAY_HEIGHT-15);*/

        SUIShowPage0();

        UEZLCDOn(lcd);
#if FAST_STARTUP
        UEZLCDBacklight(lcd, 255);
#else
        for (i=0; i<256; i++)  {
            UEZLCDBacklight(lcd, i);
            UEZTaskDelay(1);
        }
#endif
        // Create checksum string
#if FAST_STARTUP
        sprintf(buffer, "CS:????");
#else
        while (!G_romChecksumCalculated)
            UEZTaskDelay(10);
        sprintf(buffer, "CS:%08X", G_romChecksum);
        swim_set_font_transparency(&G_mmWin, 1);
        swim_put_text_xy(
                         &G_mmWin,
                         buffer,
                         5,
                         DISPLAY_HEIGHT-15);
        swim_set_font_transparency(&G_mmWin, 0);
#endif

    }
}

/*---------------------------------------------------------------------------*
* Routine:  MainMenu
*---------------------------------------------------------------------------*
* Description:
*      Present the main menu
*---------------------------------------------------------------------------*/

void MainMenu(void)
{
    T_uezDevice lcd;
    T_pixelColor *pixels;
    T_uezQueue queue;

    // Setup queue to receive touchscreen events
    if (UEZQueueCreate(1, sizeof(T_uezTSReading), &queue) == UEZ_ERROR_NONE) {
        // Open the LCD and get the pixel buffer
        if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
            UEZLCDGetFrame(lcd, 0, (void **)&pixels);
            UEZLCDOn(lcd);

#if (!FAST_STARTUP)
            // Clear the screen
            TitleScreen();

            PlayAudio(523, 100);
            PlayAudio(659, 100);
            PlayAudio(783, 100);
            PlayAudio(1046, 100);
            PlayAudio(783, 100);
            PlayAudio(659, 100);
            PlayAudio(523, 100);				


#if DKTS_BUTTON_SLIDE_SHOW_DEMO // show emwin on button 1, auto slideshow on buttons 2-4
#include <uEZButton.h>
#include <GUIDEMO.h>

            T_uezDevice ButtonBank;
            TUInt32 buttonsActive = 0x0F; // buttons 1-4
            TUInt32 ButtonsPressed;
            T_slideshowDefinition aDef;
            aDef.iDrive = 1;
            if (UEZButtonBankOpen("ButtonBank0", &ButtonBank) == UEZ_ERROR_NONE) {
                // the device opened properly
                if (UEZButtonBankSetActive(ButtonBank, buttonsActive) == UEZ_ERROR_NONE) {
                    // set active successful
                }
                if (UEZButtonBankRead(ButtonBank, &ButtonsPressed) == UEZ_ERROR_NONE) { // got buttons pressed
                    if (ButtonsPressed == 14) { // button 1 pressed
                        const T_choice aChoice = {0};
                        emWin(&aChoice);
                    }
                    else if (ButtonsPressed == 13) { // button 2 pressed
                        char str1[] = {'F','D','I',' ','O','v','e','r','v','i','e','w','\0'};
                        memcpy(aDef.iName,str1,strlen(str1)+1);
                        char str2[] = {'F','D','I','\0'};
                        memcpy(aDef.iDirectory,str2,strlen(str2)+1);
                        SingleSlideshowMode(&aDef);
                    }
                    else if (ButtonsPressed == 11) { // button 3 pressed
                        char str3[] = {'u','E','Z','G','U','I',' ','F','a','m','i','l','y','\0'};
                        memcpy(aDef.iName,str3,strlen(str3)+1);
                        char str4[] = {'U','E','Z','G','U','I','\0'};
                        memcpy(aDef.iDirectory,str4,strlen(str4)+1);
                        SingleSlideshowMode(&aDef);
                    }
                    else if (ButtonsPressed == 7) { // button 4 pressed
                        char str5[] = {'D','e','m','o','n','s','t','r','a','t','i',
                        'o','n',' ','P','i','c','t','u','r','e','s','\0'};
                        memcpy(aDef.iName,str5,strlen(str5)+1);
                        char str6[] = {'D','E','M','O','P','I','C','S','\0'};
                        memcpy(aDef.iDirectory,str6,strlen(str6)+1);
                        SingleSlideshowMode(&aDef);
                    }
                    else{ }
                }
                if (UEZButtonBankClose(ButtonBank) != UEZ_ERROR_NONE) {// error closing button bank
                }
            } else {// an error occurred opening Button Bank
            }
#else
            // Force calibration?
            Calibrate(CalibrateTestIfTouchscreenHeld());
#endif
            UEZTaskDelay(1000);
#else
            UEZLCDBacklight(lcd, 255);
#endif

            // Set the screen saver icon
            BouncingLogoSS_Setup(
                (TUInt8 *)G_uEZLogo,
                UEZ_ICON_WIDTH,
                UEZ_ICON_HEIGHT,
                DISPLAY_WIDTH,
                DISPLAY_HEIGHT);

            AppMenu(&mainmenu);
            UEZLCDClose(lcd);
        }
    }
}
/*-------------------------------------------------------------------------*
* End of File:  MainMenu.c
*-------------------------------------------------------------------------*/
