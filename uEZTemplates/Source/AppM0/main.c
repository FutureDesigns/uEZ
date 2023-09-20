/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Example M0 program that tests the TODO
 *
 * Implementation:
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
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
#include <HAL/EMAC.h>
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include "NVSettings.h"
#include <uEZGPIO.h>
#include <uEZMemory.h>
#include <uEZProcessor.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_UtilityFunctions.h>
#include <DIALOG.h>
#include <uEZStream.h>
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>
#include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h>
#include <Config_App.h>
#include <uEZAudioMixer.h>
#include <uEZFile.h>
#include <Device/MassStorage.h>


#if COMPILE_OPTION_USB_SDCARD_DISK
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#endif
   
/*---------------------------------------------------------------------------*
 * Defines:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
 TBool G_HeartBeat;		// Global Variable for uC/Probe Demo

 /*---------------------------------------------------------------------------*
 * Task:  Heartbeat
 *---------------------------------------------------------------------------*
 * Description:
 *      Task that blinks heartbeat LED
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 HeartbeatTask(T_uezTask aMyTask, void *aParams)
{
    UEZGPIOOutput(GPIO_HEARTBEAT_LED); // see uEZPlatform.h for pin def
    UEZGPIOSetMux(GPIO_HEARTBEAT_LED, 0);    
    
    for (int i = 0; i <10 ; i++) { //initial fast blink     
	UEZGPIOSet(GPIO_HEARTBEAT_LED);    	
        UEZTaskDelay(100);
        
        UEZGPIOClear(GPIO_HEARTBEAT_LED);    	
        UEZTaskDelay(100);
    }
    
    for (;;) { // Blink
        UEZGPIOSet(GPIO_HEARTBEAT_LED); 
    	G_HeartBeat = ETrue;
        UEZTaskDelay(250);
		
	UEZGPIOClear(GPIO_HEARTBEAT_LED);
    	G_HeartBeat = EFalse;
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
 *---------------------------------------------------------------------------*/
void MainTask(void)
{    
    // Start up the heart beat of the LED first thing.
    UEZTaskCreate(HeartbeatTask, "Heart", 128, (void *)0, UEZ_PRIORITY_NORMAL, 0);

#if COMPILE_OPTION_USB_SDCARD_DISK
    T_USBMSDriveCallbacks usbMSDiskCallbacks = {0};
#endif

#if (SEGGER_ENABLE_RTT ==1 )  // enable RTT
#if (SEGGER_ENABLE_SYSTEM_VIEW != 1) //systemview will auto init RTT
    #include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
    (void)_SEGGER_RTT; // Crossworks complains if we don't use this.
    SEGGER_RTT_Init();
    SEGGER_RTT_WriteString(0, "Hello World RTT 0!\n"); // Test RTT Interface
#endif  
#endif

#if FREERTOS_PLUS_TRACE 
  // Don't enable SystemView with FreeRTOS+Trace
#else // Otherwise SystemView can be enabled
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1) // Only include if SystemView is enabled
    SEGGER_SYSVIEW_Conf(); // This runs SEGGER_SYSVIEW_Init and SEGGER_RTT_Init
    SEGGER_SYSVIEW_Start(); // Start recording events
    // These "DEBUG_SV_" defines only compile in when systemview is enabled.
    // So you can leave them in the application for release builds.
    // Warnings and errors show different graphical icons and colors for debug.
    // Must add #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h> even when turned off

    // In our patched sytemview we provide both non-printf and printf functions. We use the non-printf here, so it just sends the strings.
    DEBUG_SV_Print("SystemView Started"); // SystemView terminal
    DEBUG_SV_PrintW("Warn Test"); // example warning
    DEBUG_SV_PrintE("Error Test"); // example error
#endif
#endif

#if 0
    printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner

    // Load the settings from non-volatile memory
    if (NVSettingsLoad() != UEZ_ERROR_NONE) {
        printf("EEPROM Settings\n");
        NVSettingsInit();
        NVSettingsSave();
    }
#endif
    
//    AudioStart(); // start before other tasks so that buzzer is usable
    
#if COMPILE_OPTION_USB_SDCARD_DISK
    // Setup the USB MassStorage device to connect to MS1 (the SD Card)
    if (UEZDeviceTableIsRegistered("USBDevice"))
        USBMSDriveInitialize(&usbMSDiskCallbacks, 0, "MS1");
#endif    
    

#if 0    
    // Raise Volume slightly for bootup sound
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, 192);

    // Example of how to properly change volume to 50% in an application.
    //UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, 128);
    
    // Bootup Sound Example    
    PlayAudio(523, 100);
    PlayAudio(659, 100);
    PlayAudio(783, 100);
    PlayAudio(1046, 100);
    PlayAudio(783, 100);
    PlayAudio(659, 100);
    PlayAudio(523, 100);
#endif

    // Start various M0 application tasks here.
    // TODO

    // Loop forever
    while (1)
		// Good spot to add supervisory task to monitor other tasks.
        UEZTaskDelay(10);
}

/*---------------------------------------------------------------------------*
* Function:  uEZPlatformStartup_NO_EXP
*---------------------------------------------------------------------------*
* Description:
*      Configure the uEZGUI without an expanion board connected
*---------------------------------------------------------------------------*/
void uEZPlatformStartup_NO_EXP()
{
#if UEZ_ENABLE_VIRTUAL_COM_PORT && (UEZ_PROCESSOR == NXP_LPC4357)
    static T_vcommCallbacks vcommCallbacks = {
            0, // SpeedChange
            0, // LineState
            0, // EmptyOutput
    };
#endif
    TBool usbIsDevice = ETrue; //Default value

    UEZPlatform_Timer3_Require(); // No Timer2 ISR on M0. Timer0/3 only. Other timers also available.
    UEZPlatform_DAC0_Require();
    UEZPlatform_SDCard_Drive_Require(1);

    // USB init section
    #if USB_PORT_B_HOST_DETECT_ENABLED
    usbIsDevice = UEZPlatform_Host_Port_B_Detect();
    #endif
    if (usbIsDevice) {
        #if UEZ_ENABLE_USB_DEVICE_STACK
            #if (UEZ_PROCESSOR != NXP_LPC4357)
            UEZPlatform_USBDevice_Require();
            #else
            #if UEZ_ENABLE_VIRTUAL_COM_PORT && (UEZ_PROCESSOR == NXP_LPC4357)
            VirtualCommInitialize(&vcommCallbacks, 0, 1); // USB0, force full speed for best integrity
            #endif //UEZ_ENABLE_VIRTUAL_COM_PORT && (UEZ_PROCESSOR == NXP_LPC4357)
            #endif //(UEZ_PROCESSOR != NXP_LPC4357)
        #endif //UEZ_ENABLE_USB_DEVICE_STACK
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortA_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif //UEZ_ENABLE_USB_HOST_STACK
    } else {
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortB_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif
    }

    // network section require
    #if UEZ_ENABLE_WIRED_NETWORK
        UEZPlatform_WiredNetwork0_Require();
    #endif

    #if UEZ_WIRELESS_PROGRAM_MODE
        UEZPlatform_WiFiProgramMode(EFalse);
    #endif

    #if UEZ_ENABLE_WIRELESS_NETWORK
        UEZPlatform_WirelessNetwork0_Require();
    #endif
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

    /* Note that some require routines will re-init the peripheral and interfere
     * with it operationg on M4 core. But you need to call parts of the API init
     * from M0 core before the functions can be used. Probably the best idea is
     * at this point you can get pointers to the existing intialized workspaces 
     * then be able to call uEZ API functions on those periphals from M0 cores.
     * GPIO require routines are safe to call at least.
     */
    
    UEZPlatform_Standard_Require();
//    SUIInitialize(EFalse, EFalse, EFalse); // SWIM not flipped
        
    // Startup the desired platform configuration
//        uEZPlatformStartup_NO_EXP();
    
     // Create a main task (not running yet)
     UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
                   UEZ_PRIORITY_NORMAL, &G_mainTask);

     // Done with this task, fall out
     return 0;
}


/*---------------------------------------------------------------------------*
 * Routine:  ConfigureTimerForRunTimeStats
 *---------------------------------------------------------------------------*
 * Description:
 *      This function is a hook for freeRTOS that configures a timer for
 *      run time stats.
 *---------------------------------------------------------------------------*/
void ConfigureTimerForRunTimeStats(void){ /* no config needed */ 
}

/*---------------------------------------------------------------------------*
 * Routine:  GetTimerForRunTimeStats
 *---------------------------------------------------------------------------*
 * Description:
 *      This function is a hook for freeRTOS that returns a timer for
 *      run time stats.
 *---------------------------------------------------------------------------*/
unsigned long GetTimerForRunTimeStats( void )
{
    return UEZTickCounterGet();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_VectorTableInit
 *---------------------------------------------------------------------------*
 * Description:
 *      This function hook is for any custom vector table initialization
 *---------------------------------------------------------------------------*/
void UEZBSP_VectorTableInit()
{
    /* No current implementation */
}

/*-------------------------------------------------------------------------*
 * File:  main.c
 *-------------------------------------------------------------------------*/
