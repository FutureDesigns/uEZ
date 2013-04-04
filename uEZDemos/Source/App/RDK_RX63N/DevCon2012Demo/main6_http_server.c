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
#include <stdio.h>
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
#include <Source/Library/Graphics/Glyph/glyph_api.h>
#include <Source/Library/Console/FDICmd/FDICmd.h>
#include <Source/Library/Web/HTTPServer/HTTPServer.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
/** GPIO for Heartbeat LED pin */
#define GPIO_HEARTBEAT              GPIO_PD0
/** GPIO for Status LED pin */
#define GPIO_STATUS_LED             GPIO_PE3

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
/** Macro for centering text on the glyph display based on a width per
 * character. */
#define CENTER_X(len, wid)  DISPLAY_WIDTH/2 - len*wid/2

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
/** Accelerometer record that holds a time and reading */
typedef struct {
    /** Time and date stamp */
    T_uezTimeDate td;

    /** XYZ accelerometer reading */
    double x;
    double y;
    double z;
} T_accelerometerRecord;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
/** Handle to glyph LCD */
T_glyphHandle G_glyphLCD;

/** Last reading from the accerometer task */
T_accelerometerRecord G_record;


/** Current IP address in ASCII format */
char G_ipAddress[40]; // string of the IP Address

/*---------------------------------------------------------------------------*
 * Task:  Heartbeat
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the DAC.
 *
 *  @param [in]     aMyTask     Handle to this task.  Not used.
 *  @param [in]     *aParams    Parameters.  Not used.
 *
 *  @return         TUInt32     Returns 0.
 */
/*---------------------------------------------------------------------------*/
TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams)
{
    UEZGPIOOutput(GPIO_HEARTBEAT);

    // Blink
    for (;;) {
        UEZGPIOSet(GPIO_HEARTBEAT);
        UEZTaskDelay(250);
        UEZGPIOClear(GPIO_HEARTBEAT);
        UEZTaskDelay(250);
    }

    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  IDrawTitle
 *---------------------------------------------------------------------------*/
/**
 *  Draw the title screen on the glyph display with centered text.
 */
/*---------------------------------------------------------------------------*/
static void IDrawTitle(void)
{
    GlyphNormalScreen(G_glyphLCD);
    GlyphClearScreen(G_glyphLCD);
    GlyphSetFont(G_glyphLCD, GLYPH_FONT_5_BY_7);
    GlyphSetXY(G_glyphLCD, CENTER_X(18, 5u), DISPLAY_HEIGHT - 48);
    GlyphString(G_glyphLCD, "uEZ(R) Data Logger", 18);
    GlyphSetXY(G_glyphLCD, CENTER_X(20, 5u), DISPLAY_HEIGHT - 24);
    GlyphString(G_glyphLCD, "Future Designs, Inc.", 20);
    GlyphSetXY(G_glyphLCD, CENTER_X(12, 5u), DISPLAY_HEIGHT - 16);
    GlyphString(G_glyphLCD, "RX Solutions", 12);
    GlyphSetXY(G_glyphLCD, CENTER_X(15, 5u), DISPLAY_HEIGHT - 8);
    GlyphString(G_glyphLCD, "www.teamfdi.com", 15);
}

/*---------------------------------------------------------------------------*
 * Console Demo Test Commands:
 *---------------------------------------------------------------------------*/
int CmdPing(void *aWorkspace, int argc, char *argv[]);
int CmdSpeaker(void *aWorkspace, int argc, char *argv[]);
int CmdLED(void *aWorkspace, int argc, char *argv[]);

/** List of UEZ GUI Commands */
static const T_consoleCmdEntry G_Commands[] = {
        { "PING", CmdPing },
        { "SPEAKER", CmdSpeaker },
        { "LED", CmdLED },
};

/*---------------------------------------------------------------------------*
 * Routine:  CmdPing
 *---------------------------------------------------------------------------*/
/**
 *  Console command "PING" that simply returns "PASS: OK" with no
 *  parameters, or failure message with any parameters.
 *
 *  @param [in]     *aWorkspace Workspace for this command console
 *  @param [in]     argc        Number of arguments to this command
 *  @param [in]     argv        List of arguments including command
 *
 *  @return         int         returns 0 if command handled, or 1 if not.
 */
/*---------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------*
 * Routine:  CmdSpeaker
 *---------------------------------------------------------------------------*/
/**
 *  Console command "SPEAKER" that uses the tone generator to play tones on
 *  the speaker.  SPEAKER <freq> sets the frequency to play continously.
 *  SPEAKER 0 turns off the tone played.
 *
 *  @param [in]     *aWorkspace Workspace for this command console
 *  @param [in]     argc        Number of arguments to this command
 *  @param [in]     argv        List of arguments including command
 *
 *  @return         int         returns 0 if command handled, or 1 if not.
 */
/*---------------------------------------------------------------------------*/
int CmdSpeaker(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 freq;
    T_uezDevice speaker;

    // Make sure the command is two parameters of the format "SPEAKER <freq>"
    if (argc == 2) {
        // Open the speaker
        UEZToneGeneratorOpen("Speaker", &speaker);

        // Get the frequency
        freq = FDICmdUValue(argv[1]);
        if (freq) {
            // Play a tone at that frequency
            UEZToneGeneratorPlayToneContinuous(speaker, TONE_GENERATOR_HZ(freq));
        } else {
            // Stop playing the tone
            UEZToneGeneratorPlayToneContinuous(speaker, TONE_GENERATOR_OFF);
        }

        // Return with a status message that shows what frequency is
        // being played.
        FDICmdPrintf(aWorkspace, "PASS: %d Hz\n", freq);
    } else {
        FDICmdSendString(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  CmdLED
 *---------------------------------------------------------------------------*/
/**
 *  Console command "LED" that uses the direct control of the GPIO pins
 *  to turn on/off the status LED.  A command of "LED 1" turns on the status
 *  led.  A command of "LED 0" turns off the status led.
 *
 *  @param [in]     *aWorkspace Workspace for this command console
 *  @param [in]     argc        Number of arguments to this command
 *  @param [in]     argv        List of arguments including command
 *
 *  @return         int         returns 0 if command handled, or 1 if not.
 */
/*---------------------------------------------------------------------------*/
int CmdLED(void *aWorkspace, int argc, char *argv[])
{
    TUInt32 on;

    // Command needs to be of the form "LED <on|off>"
    if (argc == 2) {
        // Parse the on|off field as a number
        on = FDICmdUValue(argv[1]);

        // Make sure the LED is in output mode
        UEZGPIOOutput(GPIO_STATUS_LED);
        if (on) {
            // If non-zero (1), then turn on the LED
            UEZGPIOClear(GPIO_STATUS_LED);
            FDICmdPrintf(aWorkspace, "PASS: On\n");
        } else {
            // If zero, then turn off the LED
            UEZGPIOSet(GPIO_STATUS_LED);
            FDICmdPrintf(aWorkspace, "PASS: Off\n");
        }
    } else {
        FDICmdPrintf(aWorkspace, "FAIL: Incorrect parameters\n");
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  TestCmdsInit
 *---------------------------------------------------------------------------*/
/**
 *  Open the "Console" stream device and start the console command interpreter.
 *
 *  @return         T_uezError      Reported error when creating task, or
 *                                  UEZ_ERROR_NONE if successful.
 */
/*---------------------------------------------------------------------------*/
T_uezError TestCmdsInit(void)
{
    T_uezDevice stream;
    T_uezError error;
    void *G_DemoWorkspace;

    // Open the console serial port if available
    error = UEZStreamOpen("Console", &stream);
    if (error)
        return error;

    // Start FDI Cmd Console
    G_DemoWorkspace = 0;
    error = FDICmdStart(stream, &G_DemoWorkspace, 2048, G_Commands);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * HTTPServer:
 *---------------------------------------------------------------------------*/
/** HTTPServer callback routines: */
static T_uezError MyHTTPServerGetValue(void *aHTTPState, const char *aVarName);
static T_uezError MyHTTPServerSetValue(
            void *aHTTPState,
            const char *aVarName,
            const char *aValue);

static T_httpServerParameters G_HTTPParams = {
    MyHTTPServerGetValue,
    MyHTTPServerSetValue
};

/*---------------------------------------------------------------------------*
 * Routine:  MyHTTPServerGetValue
 *---------------------------------------------------------------------------*/
/**
 *  When processing "${VARIABLES}" macros in the HTML, this routine is called
 *  each time a token is found.  This routine looks up the variable and calls
 *  HTTPServerSetVar to set what the variable is.  The HTTP Server will then
 *  use these variable settings to output to the browser.
 *
 *  @param [in]     aHTTPState      HTTP Workspace handle
 *  @param [in]     aVarName        String name of variable to look up
 *
 *  @return         T_uezError      Error if a problem occurred, or
 *                                  UEZ_ERROR_NONE.  An error will abort
 *                                  the parsing of the HTML page.
 */
/*---------------------------------------------------------------------------*/
static T_uezError MyHTTPServerGetValue(void *aHTTPState, const char *aVarName)
{
    // Set the default to an unknown string
    char buffer[50] = "???";

    // Look for a match of known variable names
    if (strcmp(aVarName, "time") == 0) {
        // If 'time', return the hours, minutes, and seconds
        sprintf(buffer, "%d:%02d:%02d",
            G_record.td.iTime.iHour,
            G_record.td.iTime.iMinute,
            G_record.td.iTime.iSecond);
    } else if (strcmp(aVarName, "date") == 0) {
        // If 'date', return the month, day, and year
        sprintf(buffer, "%d/%d/%4d",
            G_record.td.iDate.iMonth,
            G_record.td.iDate.iDay,
            G_record.td.iDate.iYear);
    } else if (strcmp(aVarName, "accelx") == 0) {
        // If acccelx, y, or z, return the last accelerometer
        // reading of x, y, or z.
        sprintf(buffer, "%0.2f", G_record.x);
    } else if (strcmp(aVarName, "accely") == 0) {
        sprintf(buffer, "%0.2f", G_record.y);
    } else if (strcmp(aVarName, "accelz") == 0) {
        sprintf(buffer, "%0.2f", G_record.z);
    }

    // Now that a string has been determined, set the variable to this string
    HTTPServerSetVar(aHTTPState, aVarName, buffer);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  MyHTTPServerSetValue
 *---------------------------------------------------------------------------*/
/**
 *  If web page submits a group of variables, each variable is parsed and
 *  passed into this routine.
 *
 *  @param [in]     aHTTPState      HTTP Workspace handle
 *  @param [in]     aVarName        String name of variable being set
 *  @param [in]     aValue          String of variable data
 *
 *  @return         T_uezError      Error if a problem occurred, or
 *                                  UEZ_ERROR_NONE.
 */
/*---------------------------------------------------------------------------*/
static T_uezError MyHTTPServerSetValue(
            void *aHTTPState,
            const char *aVarName,
            const char *aValue)
{
    // Tone frequency (defaults to 1000 Hz)
    static TUInt32 frequency = 1000;

    // Determine which value is being written
    if (strcmp(aVarName, "freq") == 0) {
        // If 'freq', set the next tone's frequency.
        frequency = atoi(aValue);
    } else if (strcmp(aVarName, "duration") == 0) {
        // If 'duration', go ahead and play the tone for that length in seconds
        PlayAudio(frequency, atoi(aValue) * 1000);
    }

    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Data Logging:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  DataLogXYZ
 *---------------------------------------------------------------------------*/
/**
 *  Log the data out to the serial port and log file.
 *
 *  @param [in]     x, y, z         XYZ readings from accelerometer
 *
 *  @return         T_uezError      Error if a problem occurred, or
 *                                  UEZ_ERROR_NONE.
 */
/*---------------------------------------------------------------------------*/
void DataLogXYZ(double x, double y, double z)
{
    T_uezTimeDate td;
    char line[300];
    T_uezFile file;
    TUInt32 numWritten;

    // Determine the current date and time
    UEZTimeDateGet(&td);

    // Store the date and time and the current reading
    G_record.x = x;
    G_record.y = y;
    G_record.z = z;
    G_record.td = td;

    // Output the time and date to the console
    printf("%d/%d/%4d %d:%02d:%02d ",
        td.iDate.iMonth,
        td.iDate.iDay,
        td.iDate.iYear,
        td.iTime.iHour,
        td.iTime.iMinute,
        td.iTime.iSecond);

    // Output the accelerometer readings in the x, y, and z.
    printf("Accel: (%0.2f, %0.2f, %0.2f)\n", x, y, z);

    // Create a line for the date/time and accelerator reading
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

    // Output this line to the log file.  Do this by opening the file
    // in append mode, writing the line, and then closing the file.
    if (UEZFileOpen("0:/HTTPROOT/ACCELLOG.TXT", FILE_FLAG_APPEND, &file)
            == UEZ_ERROR_NONE) {
        UEZFileWrite(file, line, strlen(line), &numWritten);
        UEZFileClose(file);
    }
}


/*---------------------------------------------------------------------------*
 * Routine:  AccelerometerMonitor
 *---------------------------------------------------------------------------*/
/**
 *  Monitor the accelerometer by oopening it and taking a reading once
 *  a second.  Readings are averages of 8 raw readings.  Readings are scaled
 *  from 15.16 fixed format values to floating point readings.
 *
 *  @param [in]     aMyTask         Handle to this task
 *  @param [in]     aParams         not used, always 0.
 *
 *  @return         TUInt32         not useed, always 0.  Never returns.
 */
/*---------------------------------------------------------------------------*/
TUInt32 AccelerometerMonitor(T_uezTask aMyTask, void *aParams)
{
    T_uezDevice Accel;
    double x, y, z;
    int i;
    AccelerometerReading ar;

    // Get access to the accelerometer
    if (UEZAccelerometerOpen("Accel0", &Accel) == UEZ_ERROR_NONE) {
        // Sit in an infinite loop reading the accelerometer once a second.
        while (1) {
            // Sum 8 readings.
            x = y = z = 0;
            for (i=0; i<8; i++) {
                if (UEZAccelerometerReadXYZ(Accel, &ar, 10) != UEZ_ERROR_NONE) {
                    // Report any errors communicating with the accelerometer.
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

            // Process the data in another routine.
            DataLogXYZ(x, y, z);

            // Sleep before next reading
            UEZTaskDelay(1000);
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  SetupTasks
 *---------------------------------------------------------------------------*/
/**
 *  Start multiple tasks in the system.
 */
/*---------------------------------------------------------------------------*/
void SetupTasks(void)
{
    T_uezError error;

    error = WebServerStart(&G_HTTPParams);
    if (error) {
        printf("Problem starting HTTPServer! (Error=%d)\n", error);
    } else {
        printf("HTTPServer started\n");
    }

    // Start up the command line
    TestCmdsInit();

    // Start the Accelerometer monitoring task
    UEZTaskCreate(AccelerometerMonitor, "AccelMon", UEZ_TASK_STACK_BYTES(1024),
        (void *)0, UEZ_PRIORITY_NORMAL, 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  MainTask
 *---------------------------------------------------------------------------*/
/**
 *  In the uEZ system, main() is a task.  Do not exit this task
 *  unless you want to the board to reset.  This function should
 *  setup the system and then run the main loop of the program.
 *
 *  @return         int     Return code, if any.
 */
/*---------------------------------------------------------------------------*/
int MainTask(void)
{
    // Output a start up banner
    printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n");

    // Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);

    // Load any non-volatile settings from the data flash.
    NVSettingsInit();
    // Load the settings from non-volatile memory
    if (NVSettingsLoad() == UEZ_ERROR_CHECKSUM_BAD) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }

    // Setup the glyph API to the LCD and draw the title screen.
    GlyphOpen(&G_glyphLCD, 0);
    IDrawTitle();

    // Delete old log file
    UEZFileDelete("0:/HTTPROOT/ACCELLOG.TXT");

    // Setup any additional misc. tasks (such as the heartbeat task)
    SetupTasks();

    // Play startup tones
    PlayAudio(523, 100);
    PlayAudio(659, 100);
    PlayAudio(783, 100);
    PlayAudio(1046, 100);
    PlayAudio(783, 100);
    PlayAudio(659, 100);
    PlayAudio(523, 100);

    // Wait in an infinite loop.
    while(1) {
        UEZTaskDelay(10000);
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

