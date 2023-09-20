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
#include "Audio.h"
#include <uEZGPIO.h>
#include <uEZI2C.h>
#include <uEZMemory.h>
#include <uEZProcessor.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_UtilityFunctions.h>
#include <Source/Library/Memory/MemoryTest/MemoryTest.h>
#include "emWin/WindowManager.h"
#include <DIALOG.h>
#include <Calibration.h>
#include <NetworkStartup.h>
#include <uEZStream.h>
#include <Source/Library/Audio/DAC/uEZDACWAVFile.h>
#include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h>
#include <Source/ExpansionBoard/FDI/uEZGUI_EXP_DK/uEZGUI_EXP_DK.h>  
#include <Config_App.h>
#include <uEZAudioMixer.h>
#include <uEZFile.h>
#include <Device/MassStorage.h>
#include <Device/I2CBus.h>
#include "Storage_Callbacks.h"
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>

#if COMPILE_OPTION_USB_SDCARD_DISK
#include <Source/Library/USBDevice/MassStorage/Generic/USBMSDrive.h>
#endif

#if FREERTOS_PLUS_TRACE 
#include <Source/RTOS/FreeRTOS-Plus/FreeRTOS-Plus-Trace/Include/trcUser.h>
#endif
   
/*---------------------------------------------------------------------------*
 * Defines:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
 TBool G_HeartBeat;		// Global Variable for uC/Probe Demo
 UEZ_PUT_SECTION(".non_init", volatile T_BootTestModeSetting G_hardwareTest);

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
#if (CONFIG_LOW_LEVEL_TEST_CODE == 1)
    TUInt32 startAddress = UEZEmWinGetRAMAddr()+UEZEmWinGetRAMSize();
    TUInt32 maxAddress = UEZBSP_SDRAM_BASE_ADDR+UEZBSP_SDRAM_SIZE;
    TUInt32 chunkSize = (16*1024);
    TUInt32 currentAddress = startAddress;
    UEZTaskDelay(5000);

  switch(G_hardwareTest.iTestMode) {
    case HARDWARE_TEST_CRYSTAL:
    
    break;
    case HARDWARE_TEST_SDRAM:
        //COM_Send("SDRAM TESTING...\n", 17);
        for (;;) {
            UEZBSP_HEARTBEAT_TOGGLE();
            printf("SDRAM address %X: ", currentAddress);
            UEZTaskDelay(25);
            MemoryTest(currentAddress,chunkSize);
            currentAddress = currentAddress + chunkSize;
            if(currentAddress > maxAddress){
              currentAddress = startAddress;
            }
            printf("PASS\n");
            UEZTaskDelay(25);
        }
    break;
    case HARDWARE_TEST_I2C:
    
    break;
    case HARDWARE_TEST_AUDIO:
    
    break;
    case HARDWARE_TEST_NORMAL:
    default:
        
    break;
  }
    // Make sure that we don't exit this task as currently that isn't supported.  
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

  return 0;
#else
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
#endif
}

 /*---------------------------------------------------------------------------*
 * Task:  GUIInterfaceTask
 *---------------------------------------------------------------------------*
 * Description:
 *      Task that starts the emWin GUI
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 GUIInterfaceTask(T_uezTask aMyTask, void *aParams)
{
    TBool done = EFalse;
    if( WindowManager_Start_emWin() != UEZ_ERROR_NONE){
        UEZFailureMsg("Failed to start emWin!");
    }
    
    //emWinDemo(); // uncomment to run stock emWin Demos included with uEZ
    
    WindowManager_Create_All_Active_Windows();
    WindowManager_Show_Window(HOME_SCREEN);

    GUI_Exec(); // make sure that first screen displays

    // Turn on backlight after initializing first screen to prevent screen tear
    WindowManager_BackLight_On(255);

    while (!done) {
        GUI_X_ExecIdle(); // By default this is just a task delay.
        GUI_Exec();
    }
    return 0;
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
    UEZTaskCreate(HeartbeatTask, "Heart", 1024, (void *)0, UEZ_PRIORITY_NORMAL, 0);

#if COMPILE_OPTION_USB_SDCARD_DISK
    T_USBMSDriveCallbacks usbMSDiskCallbacks = {0};
#endif

#if (SEGGER_ENABLE_RTT ==1 )  // enable RTT
#if (SEGGER_ENABLE_SYSTEM_VIEW != 1) //systemview will auto init RTT
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

    Storage_PrintInfo('1');
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
    
    AudioStart(); // start before other tasks so that buzzer is usable
    
#if COMPILE_OPTION_USB_SDCARD_DISK
    // Setup the USB MassStorage device to connect to MS1 (the SD Card)
    if (UEZDeviceTableIsRegistered("USBDevice"))
        USBMSDriveInitialize(&usbMSDiskCallbacks, 0, "MS1");
#endif

    /* NOTE this is the last spot in the bootup routine that it is safe to perform
    destructive testing of the .frames or .emwin memory regions.
    */
    //UEZPlatform_GetBaseAddress

    
// Calibration is only needed for resistive touch and not capacitive or no touch.
#if ((UEZ_DEFAULT_TOUCH != TOUCH_PCAP) || USE_RESISTIVE_TOUCH)
    Calibrate(CalibrateTestIfTouchscreenHeld());
#endif

    //Start emWin interface
    UEZTaskCreate(GUIInterfaceTask, "GUIInterface", (23 * 1024), (void *) 0, UEZ_PRIORITY_NORMAL, 0);
    
#if UEZ_ENABLE_WIRELESS_NETWORK || UEZ_ENABLE_WIRED_NETWORK        
    // Start the network task if needed
    UEZTaskCreate((T_uezTaskFunction)NetworkStartup, "NetStart", 
        UEZ_TASK_STACK_BYTES(1024), (void *)0, UEZ_PRIORITY_NORMAL, 0);
#endif
    
    // Raise Volume slightly for bootup sound
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, 192);

    // Example of how to properly change volume to 50% in an application.
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, 128);
    
    // Bootup Sound Example
        
    PlayAudio(523, 100);
    PlayAudio(659, 100);
    PlayAudio(783, 100);
    PlayAudio(1046, 100);
    PlayAudio(783, 100);
    PlayAudio(659, 100);
    PlayAudio(523, 100);

    // Loop forever
    while (1)
		// Good spot to add supervisory task to monitor other tasks.
        UEZTaskDelay(10);
}

/*---------------------------------------------------------------------------*
* Function:  uEZPlatformStartup_EXP_DK
*---------------------------------------------------------------------------*
* Description:
*
*---------------------------------------------------------------------------*/
#if UEZGUI_EXPANSION_DEVKIT
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
#endif //UEZGUI_EXPANSION_DEVKIT

/*---------------------------------------------------------------------------*
* Function:  uEZPlatformStartup_EXP_BRKOUT
*---------------------------------------------------------------------------*
* Description:
*
*---------------------------------------------------------------------------*/
#if UEZGUI_EXP_BRK_OUT
void uEZPlatformStartup_EXP_BRKOUT()
{
    TBool usbIsDevice = ETrue; //Default value

    UEZPlatform_Timer2_Require();
    UEZPlatform_DAC0_Require();
    UEZGUI_EXP_DK_SDCard_MCI_Require(1);
    
    // USB init section
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
#endif //UEZGUI_EXP_BRK_OUT

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

    UEZPlatform_Timer2_Require();
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

#if (defined __GNUC__) // GCC
//(uint32_t*)&.frames)));

#define RAMTEST_START_ADDRESS ((const uint32_t) &".frames")
#define RAMTEST_LENGTH (const uint32_t)(((uint32_t)UEZBSP_SDRAM_SIZE) - RAMTEST_START_ADDRESS)

#elif (defined __ICCARM__) || (defined __ICCRX__) // IAR
// Must declare these pragmas before below section placement if IAR
#pragma section = ".frames"
#define RAMTEST_START_ADDRESS &".frames"
#define RAMTEST_LENGTH (UEZBSP_SDRAM_SIZE-RAMTEST_START_ADDRESS)

#endif

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
        
    #if FREERTOS_PLUS_TRACE 
        TUInt32 traceAddressInMemory = 0;
    #endif

    T_uezDevice I2C;
    T_uezError error;
    I2C_Request r;
    TUInt8 dataIn[2];
    TUInt8 dataOut[2] = {0x00,0x00};

    switch(G_hardwareTest.iTestMode){
      case HARDWARE_TEST_CRYSTAL:
      
        UEZPlatform_Standard_Require();
      
      break;
      case HARDWARE_TEST_SDRAM:
      for(uint16_t i = 0; i < G_hardwareTest.iTestIterations; i++) {
        COM_Send("SDRAM TESTING...\n", 17);
        MemoryTest(
        UEZPlatform_GetBaseAddress(),
         UEZBSP_SDRAM_BASE_ADDR+UEZBSP_SDRAM_SIZE-UEZPlatform_GetBaseAddress());
        COM_Send("SDRAM PASS\n", 11);
      }

      // Start RTOS here and keep testing the unused RAM in the background?
      
      UEZPlatform_Standard_Require();

      break;
      case HARDWARE_TEST_I2C:
      UEZTaskDelay(100);
      UEZPlatform_Minimal_Require();
      UEZPlatform_I2C0_Require();
      UEZPlatform_I2C1_Require();


      UEZI2COpen("I2C1", &I2C);
      r.iSpeed = 400; //kHz
      r.iWriteData = &dataOut[0];
      r.iWriteLength = 1; // send 1 byte
      r.iWriteTimeout = 500;
      r.iReadData = &dataIn[0];
      r.iReadLength = 1; // read 20 bytes
      r.iReadTimeout = 500;
      COM_Send("\n", 1);

      // Detect which accelerometer is loaded
      for(uint16_t i = 0; i < G_hardwareTest.iTestIterations; i++) {
          r.iAddr = 0x18;
          dataOut[0] = 0x0F;
          r.iReadLength = 1;
          error = UEZI2CTransaction(I2C, &r);
          if(!error && (dataIn[0] == 0x33)) {
              COM_Send("LIS3DH found\n", 13);
          } else {
              COM_Send("LIS3DH not found\n", 17);
          }
          UEZTaskDelay(100);
          
          /*
          r.iAddr = 0x1D;
          dataOut[0] = 0x18;
          dataOut[0] = 0x80;
          r.iWriteLength = 2;
          r.iReadLength = 0;
          error = UEZI2CTransaction(I2C, &r);
          if(!error && (dataIn[0] == 0x33)) {
              COM_Send("MMA7455 found\n", 14);
          } else {
              COM_Send("MMA7455 not found\n", 18);
          }
          UEZTaskDelay(100);*/
          
          r.iAddr = 0x92 >> 1;
          dataOut[0] = 0x00;
          r.iReadLength = 1;
          error = UEZI2CTransaction(I2C, &r);
          if(!error && (dataIn[0] != 0x0)) {      
              COM_Send("LM75A found\n", 12);
          } else {
              COM_Send("LM75A not found\n", 16);
          }
          UEZTaskDelay(100);
/*
          r.iAddr = 0x1F;
          r.iReadLength = 1;
          error = UEZI2CTransaction(I2C, &r);
          if(!error) { // && (dataIn == 0x33)) {      
              COM_Send("LM48110 found\n", 14);
          } else {
              COM_Send("LM48110 not found\n", 18);
          }*/

          UEZTaskDelay(100);
          r.iAddr = 0x70 >> 1;
          r.iReadLength = 1;
          error = UEZI2CTransaction(I2C, &r);
          if(!error){// && (dataIn == 0x33)) {
              COM_Send("Touchscreen found\n", 18);
          } else {
              COM_Send("Touchscreen not found\n", 22);
          }
          UEZTaskDelay(100);

          COM_Send("\n", 1);
          UEZTaskDelay(1000);
       //printf(
      }
      UEZPlatform_Standard_Require();

      break;
      case HARDWARE_TEST_AUDIO:

        UEZPlatform_Standard_Require();    
      break;
      case HARDWARE_TEST_NORMAL:
      default:
        UEZPlatform_Standard_Require();
      break;
    }
    
    SUIInitialize(EFalse, EFalse, EFalse); // SWIM not flipped
        
    // Startup the desired platform configuration
    #if UEZGUI_EXPANSION_DEVKIT
        uEZPlatformStartup_EXP_DK();
    #else
        uEZPlatformStartup_NO_EXP();
    #endif

    #if FREERTOS_PLUS_TRACE
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
