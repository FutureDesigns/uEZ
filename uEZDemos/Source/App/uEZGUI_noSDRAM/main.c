/*-------------------------------------------------------------------------*
* File:  main.c
*-------------------------------------------------------------------------*
* Description:
*      Example program that tests the LCD, Queue, Semaphore, and
*      touchscreen features.
*
* Implementation:
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
#include <string.h>
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include "NVSettings.h"
#include "AppTasks.h"
#include <uEZProcessor.h>
#include "TestCmds.h"
#include "NVSettings.h"
#include "Audio.h"
#include <Source/ExpansionBoard/FDI/uEZGUI_EXP_DK/uEZGUI_EXP_DK.h>
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>
#include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>
#include <Source/Library/Memory/MemoryTest/MemoryTest.h>

#if COMPILE_OPTION_USB_SDCARD_DISK
#if(UEZ_PROCESSOR != NXP_LPC4357)
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#else
#include <Source/Library/USBDevice/LPCUSBLib/MassStorage/USBMSDrive.h>
#endif
#endif

#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
    #include <trcUser.h>
#endif

#if UEZ_ENABLE_USB_DEVICE_STACK && UEZ_ENABLE_VIRTUAL_COM_PORT //LPC4357
#include <Source/Library/USBDevice/LPCUSBLib/VirtualComm/VirtualComm.h>
#endif
/*---------------------------------------------------------------------------*
* Externals:
*---------------------------------------------------------------------------*/
extern T_uezTask G_mainTask;

#if (UEZ_PROCESSOR == NXP_LPC4357)
static TBool G_OnBoardUSBIsHost = EFalse;
#endif

TUInt32 G_romChecksum;
TBool G_romChecksumCalculated;
TBool volatile G_mmTestMode = ETrue;

/*---------------------------------------------------------------------------*
 * Routine:  ROMChecksumCalculate
 *---------------------------------------------------------------------------*
 * Description:
 *      Calculate the checksum over the executable area excluding
 *      bootloader and vector table (which can change).
 * Outputs:
 *      TUInt32                   -- 32-bit additive checksum
 *---------------------------------------------------------------------------*/
// This currently has to be defined for FCT test command console even though it isn't correct yet.
TUInt32 ROMChecksumCalculate()
{
#if (UEZ_PROCESSOR != NXP_LPC4357)
    TUInt8 *p = (TUInt8 *)0x00000000;
#else
    TUInt8 *p = (TUInt8 *)0x1A000000;
#endif
    TUInt32 checksum = 0;
    TUInt32 count;

    for (count=32; count<0x7E000; count++) {
        checksum += *(p++);
    }

    return checksum; // Don't use this for anything important! Does not check the full memory size of the application!
}

#define HEARTBEAT_BLINK_DELAY			 250
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
TUInt32 RAM_Test(T_uezTask aMyTask, void *aParams)
{
    // initial quick blink at bootup
    for (int i = 0; i < 20; i++) {
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY/5);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY/5);
    }

    // Blink
    while(1) {/*
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY);*/        
        MemoryTest(UEZBSP_SDRAM_BASE_ADDR, UEZBSP_SDRAM_SIZE); // RAM test will blink heartbeat LED
        // can increment a pass count value here and return to 0
        //printf("Pass RAM Test\n");
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  SetupTasks
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup tasks that run besides main or the main menu.
 *---------------------------------------------------------------------------*/
T_uezError SetupTasks(void)
{  
   //TODO background tasks to start before console   
   return UEZ_ERROR_NONE;
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
#if COMPILE_OPTION_USB_SDCARD_DISK
     T_USBMSDriveCallbacks usbMSDiskCallbacks = {0};
#endif
     
#if (SEGGER_ENABLE_RTT ==1 )  // enable RTT
#if (SEGGER_ENABLE_SYSTEM_VIEW != 1) //systemview will auto init RTT
    (void)_SEGGER_RTT; // GCC complains if we don't use this.
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
    DEBUG_SV_Printf("SystemView Started"); // SystemView terminal
    DEBUG_SV_PrintfW("Warn Test"); // example warning
    DEBUG_SV_PrintfE("Error Test"); // example error
#endif
#endif
     
     printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner

     // Load the settings from non-volatile memory
     if (NVSettingsLoad() != UEZ_ERROR_NONE) {
          printf("EEPROM Settings\n");
          NVSettingsInit();
          NVSettingsSave();
     }
#if (UEZ_PROCESSOR == NXP_LPC4357)
 (void)G_OnBoardUSBIsHost;
#endif
#if COMPILE_OPTION_USB_SDCARD_DISK
    // Setup the USB MassStorage device to connect to MS1 (the SD Card)
#if (UEZ_PROCESSOR != NXP_LPC4357)
    if (UEZDeviceTableIsRegistered("USBDevice"))
        USBMSDriveInitialize(&usbMSDiskCallbacks, 0, "MS1");
#else
    if(!G_OnBoardUSBIsHost){
        USBMSDriveInitialize(&usbMSDiskCallbacks, 0, "MS1", 0, 1);
    }
#endif
#endif

    // Setup any additional misc. tasks 
    SetupTasks();

#if APP_MENU_ALLOW_TEST_MODE
    // initialize command console for test commands
    UEZGUITestCmdsInit();
#endif
    // Setup DAC audio if available - ignore error
    UEZDACWAVConfig("Timer2");

    // Our heatbeat LED already should be set a low level, but we should replace this code here with a platform callback to set it.
    // This old code isn't complete on LPC4357 for example and mux could be non-zero on a platform.
    //UEZGPIOOutput(GPIO_HEARTBEAT_LED);
    //UEZGPIOSetMux(GPIO_HEARTBEAT_LED, 0);

    while(1) 
    {
        UEZGPIOSet(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY);
        UEZGPIOClear(GPIO_HEARTBEAT_LED);
        UEZTaskDelay(HEARTBEAT_BLINK_DELAY);
    }

     // We should not exit main unless we want to reset the board    
#if (defined __GNUC__) // GCC
    return 0;
#elif (defined __ICCARM__) || (defined __ICCRX__) // IAR
     
#endif
}

/*---------------------------------------------------------------------------*
* Function:  uEZPlatformStartup_NO_EXP
*---------------------------------------------------------------------------*
* Description:
*
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

    UEZPlatform_Timer2_Require();
    UEZPlatform_DAC0_Require();

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
          #if (USB_PORT_A_HOST_ENABLED == 1)
            UEZPlatform_USBHost_PortA_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
          #endif
        #endif //UEZ_ENABLE_USB_HOST_STACK
    } else {
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortB_Require();
        #if(UEZ_PROCESSOR == NXP_LPC4357)
            G_OnBoardUSBIsHost = ETrue;
        #endif
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif
    }

    UEZPlatform_SDCard_Drive_Require(1);
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
    UEZPlatform_Standard_Require();
    UEZPlatform_Timer0_Require();

    // Startup the desired platform configuration
    #if UEZGUI_EXPANSION_DEVKIT
        uEZPlatformStartup_EXP_DK();
    #elif UEZGUI_EXP_BRK_OUT
        uEZPlatformStartup_EXP_BRKOUT();
    #else
        uEZPlatformStartup_NO_EXP();
    #endif
   
     // Create a main task (not running yet)
     UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
                   UEZ_PRIORITY_NORMAL, &G_mainTask);

     // Done with this task, fall out
     return 0;
}

// Is this required?
void UEZBSP_VectorTableInit(){ /* No Implementation */ }
/*-------------------------------------------------------------------------*
* File:  main.c
*-------------------------------------------------------------------------*/
