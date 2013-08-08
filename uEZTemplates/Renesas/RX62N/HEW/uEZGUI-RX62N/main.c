/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*
 * Description:
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
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
#include "NVSettings.h"
#include "Calibration.h"
#include "Hello_World.h"
#include "Audio.h"
#include <HAL/GPIO.h>

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
    HAL_GPIOPort **p_gpio;
    TUInt8 heartbeatLED = 13;
    
    HALInterfaceFind("GPIO1", (T_halWorkspace **)&p_gpio);

    (*p_gpio)->SetOutputMode(p_gpio, 1<<heartbeatLED);
    (*p_gpio)->SetMux(p_gpio, heartbeatLED, 0); // set to GPIO
    // Blink
    for (;;) {
        (*p_gpio)->Set(p_gpio, 1<<heartbeatLED);
        UEZTaskDelay(250);
        (*p_gpio)->Clear(p_gpio, 1<<heartbeatLED);
        UEZTaskDelay(250);
    }
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
    T_uezDevice lcd;
    T_pixelColor *pixels;

    //Load the NVSetting from the EEPROM, includes calibration data
    if (NVSettingsLoad() != UEZ_ERROR_NONE) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }

    // Start up the heart beat of the LED
    UEZTaskCreate(Heartbeat, "Heart", 64, (void *)0, UEZ_PRIORITY_NORMAL, 0);
    // Force calibration?
    Calibrate(CalibrateTestIfTouchscreenHeld());

    // Open the LCD and get the pixel buffer
    if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE)  {
        UEZLCDGetFrame(lcd, 0, (void **)&pixels);
        UEZLCDOn(lcd);
        UEZLCDBacklight(lcd, 255);

        AudioStart();

        //Application code goes here:

        HelloWorld();
        //End application code.
        //Should not reach this point
        UEZLCDClose(lcd);
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

    UEZPlatform_Standard_Require();

    // USB Flash drive needed?
    UEZPlatform_USBFlash_Drive_Require(0);

    // USB Device needed?
    //UEZPlatform_USBDevice_Require();

    // SDCard needed?
    UEZPlatform_SDCard_Drive_Require(1);

    // Network needed?
    UEZPlatform_WiredNetwork0_Require();

    // Create a main task (not running yet)
    UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
            UEZ_PRIORITY_NORMAL, &G_mainTask);

	// Initialize SimpleUI, use single size icons and no XY flip
	SUIInitialize(EFalse, EFalse, EFalse); 

    // Done with this task, fall out
    return 0;
}
