/*-------------------------------------------------------------------------*
 * File:  MainMenu.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Present the main menu
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
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZLCD.h>
#include <uEZTS.h>
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_DrawBitmap.h>
#include <Source/Library/ScreenSaver/BouncingLogoSS.h>
#include "AppDemo.h"
#include "AppTasks.h"
#include <HAL/GPDMA.h>
#include <Device/GPDMA.h>
#include <uEZDemoCommon.h>
#include <uEZPlatform.h>
#include <uEZLCD.h>
#include <uEZAudioMixer.h>
#include <uEZWatchdog.h>
#include <VideoPlayer.h> // bootup video

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

#define SPEAKER_RELIABILITY_TEST  0
#define ACCELEROMETER_TEST        0

#ifndef UEZ_AWS_IOT_CLIENT_DEMO
#define UEZ_AWS_IOT_CLIENT_DEMO   0
#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static SWIM_WINDOW_T G_mmWin;
TUInt32 G_romChecksum;
TBool G_romChecksumCalculated;

#if (TS_TEST_DEMO == 1)
extern void TS_Test_Demo(const T_choice *aChoice);
#endif
void ResetDevice(const T_choice *p_choice);

/*---------------------------------------------------------------------------*
 * Globals: Main Menu
 *---------------------------------------------------------------------------*/
#if APP_DEMO_APPS
static const T_appMenuEntry apps_menu_entries[] = {
    { "Accelerometer", AccelDemoMode, G_accelIcon, 0 },
    { "Time & Date", TimeDateMode, G_timeDateIcon, 0 },
    { "Temperature", TempMode, G_temperatureIcon, 0 },
#if (TS_TEST_DEMO == 1)
    { "TSTest", TS_Test_Demo, G_temperatureIcon, 0 },
#endif
#if (UEZ_AWS_IOT_CLIENT_DEMO == 1)
    { "MQTT Demo", emWin, G_MQTT_Logo, 0 },
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
#if UEZ_DEMO_FCT
    { "Functional Test", FunctionalTest, G_funcTestIcon, 0 },
#endif
#if UEZGUI_EXP_DK_FCT_TEST
    { "EXP-DK Test", FunctionalTest_EXP_DK, G_funcTestIcon, 0 },
    { "EXP-DK Loopback", FunctionalTest_EXP_DK_Loopback, G_funcTestIcon, 0 },
#endif
    { "Exit/Reset", ResetDevice, G_downExit, 0 },
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
#if APP_DEMO_SLIDESHOW
   { "Slideshow", MultiSlideshowMode, G_slideshowIcon, 0 },
#endif
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
#if APP_DEMO_VIDEO_PLAYER
   { "Video Player", VideoPlayerSelection, G_videoIcon, 0 },
#endif
   { 0 },
};

static const T_appMenu mainmenu = {
    PROJECT_NAME " " VERSION_AS_TEXT,
    mainmenu_entries,
    EFalse, // cannot exit
};

/*---------------------------------------------------------------------------*
 * Routine:  ROMChecksumCalculate
 *---------------------------------------------------------------------------*
 * Description:
 *      Calculate the checksum over the executable area excluding
 *      bootloader and vector table (which can change).
 * Outputs:
 *      TUInt32                   -- 32-bit additive checksum
 *---------------------------------------------------------------------------*/
TUInt32 ROMChecksumCalculate(void)
{
    // First Flash Bank
#if (UEZ_PROCESSOR != NXP_LPC4357)
    TUInt8 *p = (TUInt8 *)0x00000000;
#else
    TUInt8 *p = (TUInt8 *)0x1A000000;
#endif
    TUInt32 checksum = 0;
    TUInt32 count;

    checksum += *(p);
    for (count=0; count<0x80000; count++) {
        checksum += *(p++);
    }

#if (UEZ_PROCESSOR != NXP_LPC4357)
#else
    UEZTaskDelay(1);
    // Second Flash Bank
    p = (TUInt8 *)0x1B000000;
    checksum += *(p);
    for (count=0; count<0x80000; count++) {
        checksum += *(p++);
    }
#endif

    UEZTaskDelay(1);
#if (UEZ_PROCESSOR == NXP_LPC1788)
    p = (TUInt8 *)0x80000000; // NOR Flash
    checksum += *(p);
    for (count=0; count<0x800000; count++) { // first 1 MB only for now
        checksum += *(p++);
    }
#endif
#if (UEZ_PROCESSOR == NXP_LPC4088)
    p = (TUInt8 *)0x28000000; // QSPI
    checksum += *(p);
    for (count=0; count<0x800000; count++) { // first 1 MB only for now
        checksum += *(p++);
    }
#endif
#if (UEZ_PROCESSOR == NXP_LPC4357)
    p = (TUInt8 *)0x14000000; // QSPI
    checksum += *(p);
    for (count=0; count<0x800000; count++) { // first 1 MB only for now
        checksum += *(p++);
    }
#endif

    return checksum; // Don't use this for anything important! Does not check the full memory size of the application!
}

/*---------------------------------------------------------------------------*
 * Routine:  CalcChecksumTask
 *---------------------------------------------------------------------------*
 * Description:
 *      Calculate a checksum in a separate task
 * Inputs:
 *      T_uezTask aMyTask         -- This task
 *      void *aParams             -- Passed in parameters
 * Outputs:
 *      TUInt32                   -- Returned code
 *---------------------------------------------------------------------------*/
void CalcChecksumTask(T_uezTask aMyTask, void *aParams)
{
    PARAM_NOT_USED(aMyTask);
    PARAM_NOT_USED(aParams);
    G_romChecksum = ROMChecksumCalculate();
    G_romChecksumCalculated = ETrue;
}


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
    //char buffer[15];

    G_romChecksumCalculated = EFalse;
    UEZTaskCreate(
        (T_uezTaskFunction)CalcChecksumTask,
        "Chksum",
        UEZ_TASK_STACK_BYTES(512),
        (void *)0,
        UEZ_PRIORITY_NORMAL,
        0);

    if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
        UEZLCDGetFrame(lcd, 0, (void **)&pixels);
        
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

#if FAST_STARTUP
        UEZLCDBacklight(lcd, 255);
#else
        for (i=0; i<255; i++)  {
            UEZLCDBacklight(lcd, i);
            UEZTaskDelay(1);
        }
#endif
        // Create checksum string
#if FAST_STARTUP
        sprintf(buffer, "CS:????");
#else
        while (!G_romChecksumCalculated) {
            UEZTaskDelay(10);
        }
        /*sprintf(buffer, "CS:%08X", G_romChecksum); // TODO you can't actually see this.
        swim_set_font_transparency(&G_mmWin, 1);
        swim_put_text_xy(
            &G_mmWin,
            buffer,
            5,
            DISPLAY_HEIGHT-15);
        swim_set_font_transparency(&G_mmWin, 0);*/
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

    // Open the LCD and get the pixel buffer
    if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
        UEZLCDGetFrame(lcd, 0, (void **)&pixels);

        // Example of how to properly change volume to 50% in an application.
        //UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, 128);

#if (!FAST_STARTUP)

#if (APP_DEMO_ENABLE_STARTUP_VIDEO == 1)
        // This is from title screen function, but we don't call it anymore
        G_romChecksumCalculated = EFalse;
        UEZTaskCreate(
        (T_uezTaskFunction)CalcChecksumTask,
        "Chksum",
        UEZ_TASK_STACK_BYTES(512),
        (void *)0,
        UEZ_PRIORITY_NORMAL,
        0);

        TUInt16 i;
        // Setup the info for the startup video.
        T_VideoInfo videoInfo = { .iVideoPath = {"1:/BOOT/BOOT.BIN"},
                                  .iAudioPath = {"1:/BOOT/BOOT.WAV"},
#if(UEZ_PROCESSOR == NXP_LPC4357)
                                  .iFPS = 15,
                                  .iVideoWidth = 536,
                                  .iVideoHeight = 328,
#endif
#if(UEZ_PROCESSOR == NXP_LPC4088)
                                  .iFPS = 15,
                                  .iVideoWidth = 480,
                                  .iVideoHeight = 272,
#endif
#if(UEZ_PROCESSOR == NXP_LPC1788)
                                  .iFPS = 15,
                                  .iVideoWidth = 480,
                                  .iVideoHeight = 272,
#endif
                                  .iType = VIDEO_TYPE_BIN,
                                  .iTitle = {0},
                                  .iTextLine1 = {0},
                                  .iTextLine2 = {0}};
        T_choice videoChoice = {.iData = &videoInfo};
        VideoPlayer(&videoChoice); // play the startup video, will automatically turn the brightness up to 255.

        for (i=224; i>0; ) { // turn brightness down for fadeout
            UEZLCDBacklight(lcd, i);
            UEZTaskDelay(1);
            i = i - 32;
        }
        UEZLCDBacklight(lcd, i);
        // Clear the screen to black, we might be on second buffer so clear both.
        memset(FRAME(0), 0, DISPLAY_WIDTH*DISPLAY_HEIGHT*2);
        memset(FRAME(1), 0, DISPLAY_WIDTH*DISPLAY_HEIGHT*2);
        // Now bring the backlight back on for the main menu like we normally do.
        for (i=5; i<255; ){
            UEZLCDBacklight(lcd, i);
            UEZTaskDelay(1);
            i= i + 5;
        }
        while (!G_romChecksumCalculated) {
            UEZTaskDelay(10);
        }
#else
        // Clear the screen
        TitleScreen();
	
        PlayAudio(523, 100);
        PlayAudio(659, 100);
        PlayAudio(783, 100);
        PlayAudio(1046, 100);
        PlayAudio(783, 100);
        PlayAudio(659, 100);
        PlayAudio(523, 100);
#endif

        // Force calibration?
#if (SPEAKER_RELIABILITY_TEST == 0)
        Calibrate(CalibrateTestIfTouchscreenHeld());
#endif

        T_uezError error;

        //Storage_PrintInfo('0', EFalse);
        // Check SD card to force init, if SD card is present we return immediately.
        // If SD card is not present it will delay for about 1 second waiting on timeout.
        // We will delay below for 1 second timeout either way so that bootup will take the same amount of time.
        error = Storage_PrintInfo('1', ETrue);
        if (error == UEZ_ERROR_NONE) {
            UEZTaskDelay(1000); // delay if card found
        }        

#else
        UEZLCDBacklight(lcd, 255);
        Calibrate(FALSE);
#endif

#if (ACCELEROMETER_TEST == 1)
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
#include <Device/Accelerometer.h>
#include <uEZDeviceTable.h>
    (void)_SEGGER_RTT; // GCC complains if we don't use this.
    T_uezDevice acdevice;
    T_uezError error;
    char buffer[40] = {0}; 
    AccelerometerReading reading;
    AccelerometerReadingFloat readingF;
    DEVICE_Accelerometer **g_accel0;
    error = UEZDeviceTableFind("Accel0", &acdevice);
    if (!error) {
      error = UEZDeviceTableGetWorkspace(acdevice,
             (T_uezDeviceWorkspace **)&g_accel0);
      if(!error) {
         while (1) {
          error = (*g_accel0)->ReadXYZ(g_accel0, &reading, 100);
          error = (*g_accel0)->ReadXYZFloat(g_accel0, &readingF, 100);
    
          sprintf(buffer,"%+010d:%+010i:%+010d\n",reading.iX, reading.iY, reading.iZ); // Must print some special specifiers using standard buffer print
          SEGGER_RTT_WriteString(0, buffer); //int only output

          sprintf(buffer, "%010F:%010F:%010F\n",(float)readingF.iX,(float)readingF.iY,(float)readingF.iZ); // Must print float using standard buffer print
          SEGGER_RTT_WriteString(0, buffer); // float output

          UEZTaskDelay(50); //slow down for readability
        }
      }
    }
#endif

#if (UEZ_ENABLE_TOUCHSCREEN_NOISE_TEST == 1) // start touch screen noise detect test
        TS_NoiseDetect();
#endif
#if (UEZ_ENABLE_TOUCH_SENSITIVITY_TEST == 1) // start touch verification test
        //TODO
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

/*---------------------------------------------------------------------------*
 * Task:  ResetDevice
 *---------------------------------------------------------------------------*
 * Description:
 *      Trigger Reset. If we are using bootloader and ini is present, 
 *      we can return to menu.
 * Inputs:
 *      const T_choice *p_choice    -- Choice activated for this mode
 *---------------------------------------------------------------------------*/
void ResetDevice(const T_choice *p_choice)
{
    PARAM_NOT_USED(p_choice);
    UEZPlatform_Watchdog_Require();
    T_uezDevice watchdog;

    // Force a restart by tripping the watchdog
    UEZWatchdogOpen("Watchdog", &watchdog);
    if (!UEZIsResetFromWatchdog(watchdog)) {
        //G_clearWatchdogBit = 1;
        }
    UEZWatchdogSetMaxTime(watchdog, 100);
    UEZWatchdogStart(watchdog);
    UEZWatchdogTrip(watchdog);

    //UEZPlatform_System_Reset();
}

/*-------------------------------------------------------------------------*
 * End of File:  MainMenu.c
 *-------------------------------------------------------------------------*/
