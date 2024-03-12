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
#include <uEZDemoCommon.h>
#include <uEZMemory.h>
#include "NVSettings.h"
#include "AppTasks.h"
#include "AppDemo.h"
#include "TestCmds.h"
#include "NVSettings.h"
#include "Audio.h"
#include <Source/ExpansionBoard/FDI/uEZGUI_EXP_DK/uEZGUI_EXP_DK.h>
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>
#include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>

#if COMPILE_OPTION_USB_SDCARD_DISK
#if(UEZ_PROCESSOR != NXP_LPC4357)
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#else
#include <Source/Library/USBDevice/LPCUSBLib/MassStorage/USBMSDrive.h>
#endif
#endif

#ifdef FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
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

#ifndef FREERTOS_HEAP_SELECTION
#define FREERTOS_HEAP_SELECTION  3
#endif

#ifndef __HEAP_SIZE__
#if(UEZ_PROCESSOR == NXP_LPC4357)
#define __HEAP_SIZE__ 5000000
#else
#define __HEAP_SIZE__ 2000000
#endif
#endif

#if ((FREERTOS_HEAP_SELECTION==1) |(FREERTOS_HEAP_SELECTION==2) | (FREERTOS_HEAP_SELECTION==4))
// In Crossworks use the Project properties setting "Heap Size" which will change the definition size automatically.
// Then both heap3 and heap4 builds will use the same number from the same spot. (otherwise you will get a build error)
UEZ_PUT_SECTION(".heap", uint8_t ucHeap [__HEAP_SIZE__]);
#endif

#if ((FREERTOS_HEAP_SELECTION==5))
// TODO dual heap (doesn't make much sense with small internal SRAM on old LPCs)
#endif

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
  //TUInt8 i;
#if COMPILE_OPTION_USB_SDCARD_DISK
     T_USBMSDriveCallbacks usbMSDiskCallbacks = {0};
#endif
     
#if (SEGGER_ENABLE_RTT ==1 )  // enable RTT
#if (SEGGER_ENABLE_SYSTEM_VIEW != 1) //systemview will auto init RTT
    SEGGER_RTT_WriteString(0, "Hello World RTT 0!\n"); // Test RTT Interface
#endif
#endif

#ifdef FREERTOS_PLUS_TRACE 
  // Don't enable SystemView with FreeRTOS+Trace
#else // Otherwise SystemView can be enabled
#if (SEGGER_ENABLE_SYSTEM_VIEW == 1) // Only include if SystemView is enabled
    SEGGER_SYSVIEW_Conf(); // This runs SEGGER_SYSVIEW_Init and SEGGER_RTT_Init
    SEGGER_SYSVIEW_Start(); // Start recording events
    // These "DEBUG_SV_" defines only compile in when systemview is enabled.
    // So you can leave them in the application for release builds.
    // Warnings and errors show different graphical icons and colors for debug.
    // Must add #include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h> even when turned off
    DEBUG_SV_Print("SystemView Started"); // SystemView terminal
    DEBUG_SV_PrintW("Warn Test"); // example warning
    DEBUG_SV_PrintE("Error Test"); // example error
#endif
#endif

#if (UEZ_PROCESSOR == NXP_LPC4357)
#if (LPC43XX_ENABLE_M0_CORES == 1)
     uEZPlatform_Start_Additonal_Cores(); // safe to call from M0 cores
#endif
#endif

#if (UEZ_ENABLE_USB_HOST_STACK == 1)     
    Storage_PrintInfo('0', EFalse);
#else
#endif

#if RENAME_INI
    UEZFileRename("1:/INSTALL.FIN", "1:/INSTALL.INI");
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

     // Setup any additional misc. tasks (such as the heartbeat task, network task, speaker test task)
     SetupTasks();

     // Setup DAC audio if available - ignore error
     UEZDACWAVConfig("Timer2");

#if (APP_MENU_ALLOW_TEST_MODE == 1)
     // initialize command console for test commands
     UEZGUITestCmdsInit();
#endif
   
     // Pass control to the main menu
     MainMenu();

     // We should not exit main unless we want to reset the board
     return 0;
}

/*---------------------------------------------------------------------------*
* Function:  uEZPlatformStartup_EXP_DK
*---------------------------------------------------------------------------*
* Description:
*
*---------------------------------------------------------------------------*/
#if (UEZGUI_EXPANSION_DEVKIT == 1)
void uEZPlatformStartup_EXP_DK()
{
    UEZPlatform_Timer2_Require();
    UEZPlatform_DAC0_Require();

    UEZGUI_EXP_DK_I2CMux_Require();
    UEZGUI_EXP_DK_Button_Require();
    UEZGUI_EXP_DK_LED_Require();
    UEZGUI_EXP_DK_LightSensor_Require();
    UEZGUI_EXP_DK_ProximitySensor_Require();
    UEZGUI_EXP_DK_SDCard_MCI_Require(1);
    UEZGUI_EXP_DK_FullDuplex_RS485_Require(
        UEZ_CONSOLE_WRITE_BUFFER_SIZE,
        UEZ_CONSOLE_READ_BUFFER_SIZE);
    UEZGUI_EXP_DK_CAN_Require();
    UEZGUI_EXP_DK_I2S_Require();
    UEZGUI_EXP_DK_AudioMixer_Require();

    #if (UEZ_ENABLE_WIRED_NETWORK == 1)
        UEZGUI_EXP_DK_EMAC_Require();
    #endif

    #if UEZ_ENABLE_USB_DEVICE_STACK
        UEZPlatform_USBDevice_Require();
    #endif

    #if UEZ_ENABLE_USB_HOST_STACK
      #if (USB_PORT_A_HOST_ENABLED == 1)
        UEZPlatform_USBHost_PortA_Require();
        UEZPlatform_USBFlash_Drive_Require(0);
      #endif
    #endif

    #if (UEZ_ENABLE_WIRED_NETWORK == 1)
        UEZPlatform_WiredNetwork0_Require();
    #endif

    #if UEZ_WIRELESS_PROGRAM_MODE
        UEZPlatform_WiFiProgramMode(EFalse);
    #endif

    #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
        UEZPlatform_WirelessNetwork0_Require();
    #endif
}
#endif //UEZGUI_EXPANSION_DEVKIT

/*---------------------------------------------------------------------------*
* Function:  uEZPlatformStartup_EXP_BRKOUT
*---------------------------------------------------------------------------*
* Description:
*
*---------------------------------------------------------------------------*/
#if (UEZGUI_EXP_BRK_OUT == 1)
void uEZPlatformStartup_EXP_BRKOUT()
{
    TBool usbIsDevice = ETrue; //Default value

    UEZPlatform_Timer2_Require();
    UEZPlatform_DAC0_Require();
    UEZGUI_EXP_DK_SDCard_MCI_Require(1);

#if USB_PORT_B_HOST_DETECT_ENABLED
    usbIsDevice = UEZPlatform_Host_Port_B_Detect();
#endif
    if (usbIsDevice) {
        #if UEZ_ENABLE_USB_DEVICE_STACK
            UEZPlatform_USBDevice_Require();
        #if UEZ_ENABLE_USB_HOST_STACK
          #if (USB_PORT_A_HOST_ENABLED == 1)
            UEZPlatform_USBHost_PortA_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
          #endif
        #endif
        #endif
    } else {
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortB_Require();
        #if(UEZ_PROCESSOR == NXP_LPC4357)
            G_OnBoardUSBIsHost = ETrue;
        #endif
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif
    }

    #if (UEZ_ENABLE_WIRED_NETWORK == 1)
        UEZPlatform_WiredNetwork0_Require();
    #endif

    #if UEZ_WIRELESS_PROGRAM_MODE
        UEZPlatform_WiFiProgramMode(EFalse);
    #endif

    #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
        UEZPlatform_WirelessNetwork0_Require();
    #endif
}
#endif //UEZGUI_EXP_BRK_OUT

/*---------------------------------------------------------------------------*
* Function:  uEZPlatformStartup_NO_EXP
*---------------------------------------------------------------------------*
* Description:
*
*---------------------------------------------------------------------------*/
void uEZPlatformStartup_NO_EXP(void)
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

    #if (UEZ_ENABLE_WIRED_NETWORK == 1)
        UEZPlatform_WiredNetwork0_Require();
    #endif

    #if UEZ_WIRELESS_PROGRAM_MODE
        UEZPlatform_WiFiProgramMode(EFalse);
    #endif

    #if (UEZ_ENABLE_WIRELESS_NETWORK == 1)
    #if 0
        /* Gainspan */
        UEZPlatform_WirelessNetwork0_Require();
    #else
        /* ESP-WROOM32 */
        UEZPlatform_WirelessNetwork1_Require();
    #endif
    #endif

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
    PARAM_NOT_USED(aMyTask);
    PARAM_NOT_USED(aParameters);
    #ifdef FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
        TUInt32 traceAddressInMemory = 0;
    #endif

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

    #if UEZ_ENABLE_BUTTON_BOARD
        UEZPlatform_ButtonBoard_Require();
	#endif

    SUIInitialize(SIMPLEUI_DOUBLE_SIZED_ICONS, EFalse, EFalse); // SWIM not flipped

    #ifdef FREERTOS_PLUS_TRACE //LPC1788/4088 only as of uEZ v2.06
         uiTraceStart();
         //vTraceStartStatusMonitor(); //Removed on new version of Trace
         traceAddressInMemory = (TUInt32)vTraceGetTraceBuffer();
         printf("%x", traceAddressInMemory);
    #endif


     // Create a main task (not running yet)
     UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
                   UEZ_PRIORITY_NORMAL, &G_mainTask);

     // For LPC4357 make sure to start other tasks first so that RTC doesn't stall boot.
     UEZPlatform_IRTC_Require();

     // Done with this task, fall out
     return 0;
}

// Is this required?
void UEZBSP_VectorTableInit(void){ /* No Implementation */ }
/*-------------------------------------------------------------------------*
* File:  main.c
*-------------------------------------------------------------------------*/
