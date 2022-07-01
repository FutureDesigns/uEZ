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
#include <stdio.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include <uEZDemoCommon.h>
#include "NVSettings.h"
#include "AppTasks.h"
#include "AppDemo.h"
#include "TestCmds.h"
#include "NVSettings.h"
#include "Audio.h"
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#include <Source/ExpansionBoard/FDI/uEZGUI_EXP_DK/uEZGUI_EXP_DK.h>
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>

#if COMPILE_OPTION_USB_SDCARD_DISK
    #include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#endif

#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
    #include <trcUser.h>
#endif

/*---------------------------------------------------------------------------*
* Externals:
*---------------------------------------------------------------------------*/
extern T_uezTask G_mainTask;

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
     
     printf("\f" PROJECT_NAME " " VERSION_AS_TEXT "\n\n"); // clear serial screen and put up banner
     
     // Load the settings from non-volatile memory
     if (NVSettingsLoad() != UEZ_ERROR_NONE) {
          printf("EEPROM Settings\n");
          NVSettingsInit();
          NVSettingsSave();
     }

#if COMPILE_OPTION_USB_SDCARD_DISK
    // Setup the USB MassStorage device to connect to MS1 (the SD Card)
    if (UEZDeviceTableIsRegistered("USBDevice"))
        USBMSDriveInitialize(&usbMSDiskCallbacks, 0, "MS1");
#endif
     
     // Setup any additional misc. tasks (such as the heartbeat task)
     SetupTasks();
     
     // Setup DAC audio if available - ignore error
     UEZDACWAVConfig("Timer2");
     
#if APP_MENU_ALLOW_TEST_MODE
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
    
    #if UEZ_ENABLE_WIRED_NETWORK
        UEZGUI_EXP_DK_EMAC_Require();
    #endif

    #if UEZ_ENABLE_USB_DEVICE_STACK
        UEZPlatform_USBDevice_Require();
    #endif
    
    #if UEZ_ENABLE_USB_HOST_STACK
        UEZPlatform_USBHost_PortA_Require();
        UEZPlatform_USBFlash_Drive_Require(0);
    #endif

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
* Function:  uEZPlatformStartup_EXP_BRKOUT
*---------------------------------------------------------------------------*
* Description:
*      
*---------------------------------------------------------------------------*/
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
            UEZPlatform_USBHost_PortA_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif
        #endif
    } else {
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortB_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif
    }
    
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
* Function:  uEZPlatformStartup_NO_EXP
*---------------------------------------------------------------------------*
* Description:
*      
*---------------------------------------------------------------------------*/
void uEZPlatformStartup_NO_EXP()
{
    TBool usbIsDevice = ETrue; //Default value
    
    UEZPlatform_Timer2_Require();
    UEZPlatform_DAC0_Require();
    UEZPlatform_SDCard_Drive_Require(1);

#if USB_PORT_B_HOST_DETECT_ENABLED
    usbIsDevice = UEZPlatform_Host_Port_B_Detect();
#endif
    if (usbIsDevice) {
        #if UEZ_ENABLE_USB_DEVICE_STACK
            UEZPlatform_USBDevice_Require();
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortA_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif
        #endif
    } else {
        #if UEZ_ENABLE_USB_HOST_STACK
            UEZPlatform_USBHost_PortB_Require();
            UEZPlatform_USBFlash_Drive_Require(0);
        #endif
    }
    
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
    #if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
        TUInt32 traceAddressInMemory = 0;
    #endif
    
    UEZPlatform_Standard_Require();
        
    // Startup the desired platform configuration
    #if UEZGUI_EXPANSION_DEVKIT
        uEZPlatformStartup_EXP_DK();
    #elif UEZGUI_EXP_BRK_OUT 
        uEZPlatformStartup_EXP_BRKOUT();
    #else
        uEZPlatformStartup_NO_EXP();
    #endif
    
    SUIInitialize(SIMPLEUI_DOUBLE_SIZED_ICONS, EFalse, EFalse); // SWIM not flipped

    #if FREERTOS_PLUS_TRACE //LPC1788/4088 only as of uEZ v2.06     
         uiTraceStart();
         //vTraceStartStatusMonitor(); //Removed on new version of Trace
         traceAddressInMemory = (TUInt32)vTraceGetTraceBuffer();
         printf("%x", traceAddressInMemory);     
    #endif
         

     // Create a main task (not running yet)
     UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
                   UEZ_PRIORITY_NORMAL, &G_mainTask);
     
     // Done with this task, fall out
     return 0;
}


/*---------------------------------------------------------------------------*
* Required by emWin: Need to find a better place for this
*---------------------------------------------------------------------------*/
TUInt32 UEZEmWinGetRAMAddr(void)
{
     static TBool init = EFalse;
     if (!init) {
          memset((void *)0xA0200000, 0x00, 0x00200000);
          init = ETrue;
     }
     return 0xA0200000;
}

TUInt32 UEZEmWinGetRAMSize(void)
{
     return 0x00200000;
}

// Is this required?
void UEZBSP_VectorTableInit(){ /* No Implementation */ }
/*-------------------------------------------------------------------------*
* File:  main.c
*-------------------------------------------------------------------------*/
