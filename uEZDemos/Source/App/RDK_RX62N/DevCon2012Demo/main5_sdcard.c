/*-------------------------------------------------------------------------*
 * File:  main.c
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
#include <uEZFile.h>
#include <uEZGPIO.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZTemperature.h>
#include <uEZTimeDate.h>
#include <uEZAccelerometer.h>
#include <uEZToneGenerator.h>
#include <uEZStream.h>
#include "NVSettings.h"
#include <stdio.h>

#include <Source/Library/Graphics/Glyph/glyph_api.h>
#include <Source/Library/Web/BasicWeb/BasicWeb.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>

#include <Source/Library/Web/HTTPServer/HTTPServer.h>

#define CENTER_X(len, wid)  DISPLAY_WIDTH/2 - len*wid/2

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals
 *---------------------------------------------------------------------------*/
extern T_uezDevice G_network;
extern T_uezNetworkStatus G_networkStatus;
T_glyphHandle G_glyphLCD;

char G_ipAddress[40]; // string of the IP Address

/*---------------------------------------------------------------------------*
 * Task:  Heartbeat
 *---------------------------------------------------------------------------*
 * Description:
 *      Blink heartbeat LED
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams)
{
	UEZGPIOOutput(GPIO_PD0);
	
    // Blink
    for (;;) {
        UEZGPIOSet(GPIO_PD0);
        UEZTaskDelay(250);
        UEZGPIOClear(GPIO_PD0);
        UEZTaskDelay(250);
    }

    return 0;
}

static void IDrawTitle(void)
{
    GlyphNormalScreen(G_glyphLCD);
    GlyphClearScreen(G_glyphLCD);
    GlyphSetFont(G_glyphLCD, GLYPH_FONT_5_BY_7);
    GlyphSetXY(G_glyphLCD, CENTER_X(20, 5u), DISPLAY_HEIGHT - 24);
    GlyphString(G_glyphLCD, "Future Designs, Inc.", 20);
    GlyphSetXY(G_glyphLCD, CENTER_X(12, 5u), DISPLAY_HEIGHT - 16);
    GlyphString(G_glyphLCD, "RX Solutions", 12);
    GlyphSetXY(G_glyphLCD, CENTER_X(15, 5u), DISPLAY_HEIGHT - 8);
    GlyphString(G_glyphLCD, "www.teamfdi.com", 15);
}

int CmdPing(void *aWorkspace, int argc, char *argv[])
{
    if (argc == 1) {
        // No Parameters (other than command) cause PONG to pass
        FDICmdSendString(aWorkspace, "PASS: OK\n");
    } else {
        // Parameters cause PONG to fail
        FDICmdSendString(aWorkspace, "FAIL: Incorrect Parameters\n");
    }
    return 0;
}

int CmdSpeaker(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 freq;
    T_uezDevice speaker;

    if (argc == 2) {
        UEZToneGeneratorOpen("Speaker", &speaker);
        freq = FDICmdUValue(argv[1]);
        if (freq) {
            UEZToneGeneratorPlayToneContinuous(speaker, TONE_GENERATOR_HZ(freq));
        } else {
            UEZToneGeneratorPlayToneContinuous(speaker, TONE_GENERATOR_OFF);
        }
        FDICmdPrintf(aWorkspace, "PASS: %d Hz\n", freq);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int CmdLED(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 on;

    if (argc == 2) {
        on = FDICmdUValue(argv[1]);
        UEZGPIOOutput(GPIO_PE3);
        if (on) {
            UEZGPIOClear(GPIO_PE3);
            FDICmdPrintf(aWorkspace, "PASS: On\n");
        } else {
            UEZGPIOSet(GPIO_PE3);
            FDICmdPrintf(aWorkspace, "PASS: Off\n");
        }
    } else {
        FDICmdPrintf(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int CmdLCD(void *aWorkspace, int argc, char *argv[])
{
    if (argc == 2) {
        GlyphClearScreen(G_glyphLCD);
        GlyphSetXY(G_glyphLCD, 0, 0);
        GlyphString(G_glyphLCD, (uint8_t *)argv[1], strlen(argv[1]));
        FDICmdPrintf(aWorkspace, "PASS: OK\n");
    } else {
        FDICmdPrintf(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

int CmdDir(void *aWorkspace, int argc, char *argv[])
{
    T_uezFileEntry entry;

    if (UEZFileFindStart("0:/HTTPROOT", &entry) == UEZ_ERROR_NONE) {
        do {
            FDICmdPrintf(aWorkspace, "%c %-12.12s %02d/%02d/%04d\r\n",
                (entry.iFileAttributes & UEZ_FILE_ENTRY_ATTR_DIRECTORY)?'D':'F',
                entry.iFilename,
                entry.iModifiedDate.iDay,
                entry.iModifiedDate.iMonth,
                entry.iModifiedDate.iYear
                );
        } while (UEZFileFindNext(&entry) == UEZ_ERROR_NONE);
    } else {
        FDICmdPrintf(aWorkspace, "No files found.\n");
    }
    UEZFileFindStop(&entry);

    return 0;
}

static const T_consoleCmdEntry G_UEZGUICommands[] = {
        { "PING", CmdPing },
        { "SPEAKER", CmdSpeaker },
        { "DIR", CmdDir },
        { "LED", CmdLED },
        { "LCD", CmdLCD },
};

T_uezError TestCmdsInit(void)
{
    T_uezDevice stream;
    T_uezError error;
    void *G_UEZGUIWorkspace;

    // Open the console serial port if available
    error = UEZStreamOpen("Console", &stream);
    if (error)
        return error;

    // Start FDI Cmd Console
    G_UEZGUIWorkspace = 0;
    error = FDICmdStart(stream, &G_UEZGUIWorkspace, 2048, G_UEZGUICommands);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

void DataLogXYZ(double x, double y, double z)
{
    T_uezTimeDate td;
    char line[300];
    T_uezFile file;
    TUInt32 numWritten;

    UEZTimeDateGet(&td);

    printf("%d/%d/%4d %d:%02d:%02d ",
        td.iDate.iMonth,
        td.iDate.iDay,
        td.iDate.iYear,
        td.iTime.iHour,
        td.iTime.iMinute,
        td.iTime.iSecond);

    printf("Accel: (%0.2f, %0.2f, %0.2f)\n", x, y, z);

    sprintf(line, "%d/%d/%4d %d:%02d:%02d (%0.2f, %0.2f, %0.2f)\r\n",
        td.iDate.iMonth,
        td.iDate.iDay,
        td.iDate.iYear,
        td.iTime.iHour,
        td.iTime.iMinute,
        td.iTime.iSecond,
        x,
        y,
        z);

    if (UEZFileOpen("0:/ACCELLOG.TXT", FILE_FLAG_APPEND, &file) == UEZ_ERROR_NONE) {
        UEZFileWrite(file, line, strlen(line), &numWritten);
        UEZFileClose(file);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  AccelerometerMonitor
 *---------------------------------------------------------------------------*
 * Description:
 *      Track the task
 *---------------------------------------------------------------------------*/
TUInt32 AccelerometerMonitor(T_uezTask aMyTask, void *aParams)
{
    T_uezDevice Accel;
    double x, y, z;
    int i;
    AccelerometerReading ar;

    if (UEZAccelerometerOpen("Accel0", &Accel) == UEZ_ERROR_NONE) {
        while (1) {
            // The device opened properly
            x = y = z = 0;
            for (i=0; i<8; i++) {
                if (UEZAccelerometerReadXYZ(Accel, &ar, 10) != UEZ_ERROR_NONE) {
                    UEZFailureMsg("Accelerometer failed to read!");
                    break;
                }
                x += ar.iX;
                y += ar.iY;
                z += ar.iZ;
            }
            // Convert from signed 15.16 fixed format to doubles over 8 samples
            // The value is in g's.  For example, the x, y, z is
            // (0.0, 0.0, -1.0) when at rest on a table.
            x /= 65536.0 * 8;
            y /= 65536.0 * 8;
            z /= 65536.0 * 8;
            DataLogXYZ(x, y, z);

            // Sleep before next reading
            UEZTaskDelay(1000);
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  SetupTasks
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup tasks that run besides main or the main menu.
 *---------------------------------------------------------------------------*/
void SetupTasks(void)
{
    // Start up the command line
    TestCmdsInit();

    // Start the Accelerometer monitoring task
    UEZTaskCreate(AccelerometerMonitor, "AccelMon", UEZ_TASK_STACK_BYTES(1024),
        (void *)0, UEZ_PRIORITY_NORMAL, 0);
}

/*---------------------------------------------------------------------------*
 * Task:  main
 *---------------------------------------------------------------------------*
 * Description:
 *      In the uEZ system, main() is a task.  Do not exit this task
 *      unless you want to the board to reset.  This function should
 *      setup the system and then run the main loop of the program.
 * Outputs:
 *      int                     -- Output error code
 *---------------------------------------------------------------------------*/
int MainTask(void)
{	
	printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner
	
	// Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);
	
    NVSettingsInit();
    // Load the settings from non-volatile memory
    if (NVSettingsLoad() == UEZ_ERROR_CHECKSUM_BAD) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }

    // Delete old log file
    UEZFileDelete("0:/ACCELLOG.TXT");
    	
	// Setup any additional misc. tasks (such as the heartbeat task)
    SetupTasks();

    GlyphOpen(&G_glyphLCD, 0);
	IDrawTitle();

    // Play tones	
	PlayAudio(523, 100);
    PlayAudio(659, 100);
    PlayAudio(783, 100);
    PlayAudio(1046, 100);
    PlayAudio(783, 100);
    PlayAudio(659, 100);
    PlayAudio(523, 100);	
	
	while(1) {
		UEZTaskDelay(500);
	}

    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  uEZPlatformStartup
 *---------------------------------------------------------------------------*
 * Description:
 *      When uEZ starts, a special Startup task is created and called.
 *      This task brings up the all the hardware, reports any problems,
 *      starts the main task, and then exits.
 *---------------------------------------------------------------------------*/
TUInt32 uEZPlatformStartup(T_uezTask aMyTask, void *aParameters)
{
    extern T_uezTask G_mainTask;
    const T_uezTimeDate resetTD = {
        {   0, 0, 0}, // 12:00 midnight
        {   1, 1, 2012}, // Jan 1, 2012
    };

    // Setup console immediately
    UEZPlatform_Console_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
	UEZPlatform_SPI0_Require();
	UEZPlatform_S12AD_Require();
	UEZPlatform_DataFlash_Require();
	UEZPlatform_Speaker_Require();
	UEZPlatform_Temp_Require();
	UEZPlatform_Accel_Require();
	UEZPlatform_RTC_Require();
	
	if (UEZTimeDateIsValid(&resetTD) == UEZ_ERROR_INVALID)
	    printf("RTC reset!\n");

    // SDCard needed?
    UEZPlatform_SDCard_Drive_Require(0);

    // Network needed?
    UEZPlatform_WiredNetwork0_Require();

    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
            UEZ_PRIORITY_NORMAL, &G_mainTask);

    // Done with this task, fall out
    return 0;
}

