/*-------------------------------------------------------------------------*
 * File:  main0_start_template.c
 *-------------------------------------------------------------------------*
 * Description:
 *      <DESCRIPTION>
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

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
/** GPIO for Heartbeat LED pin */
#define GPIO_HEARTBEAT              GPIO_PD0

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
/** Macro for centering text on the glyph display based on a width per
 * character. */
#define CENTER_X(len, wid)  DISPLAY_WIDTH/2 - len*wid/2

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
/** Handle to glyph LCD */
T_glyphHandle G_glyphLCD;
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

/** List of UEZ GUI Commands */
static const T_consoleCmdEntry G_Commands[] = {
        { "PING", CmdPing },
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
 * Routine:  SetupTasks
 *---------------------------------------------------------------------------*/
/**
 *  Start multiple tasks in the system.
 */
/*---------------------------------------------------------------------------*/
void SetupTasks(void)
{
    // Start up the command line
    TestCmdsInit();
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

/*-------------------------------------------------------------------------*
 * End of File:  main0_start_template.c
 *-------------------------------------------------------------------------*/
