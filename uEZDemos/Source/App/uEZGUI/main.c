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
#include <HAL/EMAC.h>
#include <uEZI2C.h>
#include <uEZGPIO.h>
#include <UEZPlatform.h>
#include "Source/Library/Web/BasicWeb/BasicWEB.h"
#include <uEZDemoCommon.h>
#include "NVSettings.h"
#include "AppTasks.h"
#include "AppDemo.h"
#include "NVSettings.h"
#include "Audio.h"
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#include <Source/ExpansionBoard/FDI/UEZGUI_EXP_DK/UEZGUI_EXP_DK.h>

#include <Source/Library/Audio/DAC/uEZDACWavFile.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>

#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
#include <trcUser.h>
#endif

extern T_uezTask G_mainTask;
TBool G_usbIsDevice = ETrue;
TBool G_expdkDetected;

/*---------------------------------------------------------------------------*
* Options:
*---------------------------------------------------------------------------*/
#ifndef UEZ_ENABLE_WIRED_NETWORK
#define UEZ_ENABLE_WIRED_NETWORK 1
#endif

/*---------------------------------------------------------------------------*
* Additional Includes:
*---------------------------------------------------------------------------*/
#if COMPILE_OPTION_USB_SDCARD_DISK
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#endif

/*---------------------------------------------------------------------------*
* Globals:
*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
* Prototypes:
*---------------------------------------------------------------------------*/
extern T_uezError UEZGUITestCmdsInit(void);
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
#if UEZ_ENABLE_USB_DEVICE_STACK
     if (G_usbIsDevice) {
          // Setup the USB MassStorage device to connect to MS1 (the SD Card)

          USBMSDriveInitialize(
                               &usbMSDiskCallbacks,
                               0,
                               "MS1");
     }
#endif
     
     // Setup any additional misc. tasks (such as the heartbeat task)
     SetupTasks();
     
     AudioStart();
     
#if UEZGUI_EXP_DK_FCT_TEST
      // no command console so functional test can run
#else
     // initialize command console for test commands
     UEZGUITestCmdsInit();
#endif
     
     // Pass control to the main menu
     MainMenu();
     // We should not exit main unless we want to reset the board
     return 0;
}

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

void UEZBSP_VectorTableInit()
{
     
}

void UEZGUIGainSpanProgramEXP4(void);
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
     UEZPlatform_Timer2_Require();
     UEZPlatform_DAC0_Require();
     SUIInitialize(SIMPLEUI_DOUBLE_SIZED_ICONS, EFalse, EFalse); // SWIM not flipped
     G_expdkDetected = UEZGUI_EXP_DK_Detect();
     
     if (G_expdkDetected) { // Add EXP_DK Features we want
#if UEZGUI_EXP_DK_FCT_TEST
     // no command console so functional test can run
     // Setup standard UART without console
     UEZGUI_EXP_DK_RS232_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE, 
                                     UEZ_CONSOLE_READ_BUFFER_SIZE);
#else
     UEZGUI_EXP_DK_I2S_Require();
#endif

#if UEZ_ENABLE_WIRED_NETWORK
          UEZGUI_EXP_DK_EMAC_Require();
#endif
          UEZGUI_EXP_DK_I2CMux_Require();
          UEZGUI_EXP_DK_Button_Require();
          UEZGUI_EXP_DK_LED_Require();
          UEZGUI_EXP_DK_LightSensor_Require();
          UEZGUI_EXP_DK_ProximitySensor_Require();
          
#if (UEZ_PROCESSOR==NXP_LPC1788)
          UEZGUI_EXP_DK_SDCard_MCI_Require(1); //currently only supported on LPC1788
#endif
          UEZGUI_EXP_DK_FullDuplex_RS485_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
                                                 UEZ_CONSOLE_READ_BUFFER_SIZE);
          UEZGUI_EXP_DK_CAN_Require();
          
          UEZPlatform_DAC0_Require();
          UEZPlatform_Timer2_Require(); // Needed by Video player
          UEZDACWAVConfig("Timer2");
          UEZGUI_EXP_DK_AudioMixer_Require();
     } else {          
          UEZPlatform_SDCard_Drive_Require(1);
     }
     
#if UEZ_ENABLE_USB_HOST_STACK
     #if USB_PORT_B_HOST_DETECT_ENABLED
     if (G_expdkDetected) {
          G_usbIsDevice = UEZGUI_EXP_DK_USB_Host_Port_B_Detect();
     } else {
          G_usbIsDevice = UEZPlatform_Host_Port_B_Detect();
     }
     if (G_usbIsDevice) {       // High for a device
      #if UEZ_ENABLE_USB_DEVICE_STACK
          UEZPlatform_USBDevice_Require();
      #endif
          UEZPlatform_USBHost_PortA_Require();
          UEZPlatform_USBFlash_Drive_Require(0);
     } else {                   // Low for a host
          UEZPlatform_USBHost_PortB_Require();
          UEZPlatform_USBFlash_Drive_Require(0);
     }
     #else
          #if UEZ_ENABLE_USB_DEVICE_STACK
                UEZPlatform_USBDevice_Require();
          #endif
          UEZPlatform_USBHost_PortA_Require();
          UEZPlatform_USBFlash_Drive_Require(0);
     #endif
#else 
    #if UEZ_ENABLE_USB_DEVICE_STACK
        UEZPlatform_USBDevice_Require();
    #endif
        UEZTaskDelay(500);//ensure the host task has itiilaized before continuing.
#endif
     
     // Network needed?
#if UEZ_ENABLE_WIRED_NETWORK
     UEZPlatform_WiredNetwork0_Require();
#endif
     
#if UEZ_ENABLE_WIRELESS_NETWORK
#if UEZ_WIRELESS_PROGRAM_MODE
     UEZPlatform_WiFiProgramMode();
#endif
     UEZPlatform_WirelessNetwork0_Require();
#endif
     
#if FREERTOS_PLUS_TRACE //LPC1788 only as of uEZ v2.04
     uiTraceStart();
     vTraceStartStatusMonitor();
     
     traceAddressInMemory = (TUInt32)vTraceGetTraceBuffer();
     printf("%x", traceAddressInMemory);
#endif
     
#if ENABLE_UEZ_BUTTON
     UEZPlatform_ButtonBoard_Require();
#endif
     
     // Create a main task (not running yet)
     UEZTaskCreate((T_uezTaskFunction)MainTask, "Main", MAIN_TASK_STACK_SIZE, 0,
                   UEZ_PRIORITY_NORMAL, &G_mainTask);
     
     // Done with this task, fall out
     return 0;
}
/*-------------------------------------------------------------------------*
* File:  main.c
*-------------------------------------------------------------------------*/
