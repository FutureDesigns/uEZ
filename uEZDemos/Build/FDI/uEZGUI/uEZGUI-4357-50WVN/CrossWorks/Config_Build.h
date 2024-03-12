#define BUILD_NUMBER                        2
#define BUILD_DATE                          "7/29/2016"
#define UEZ_PROCESSOR                       NXP_LPC4357
#define PROCESSOR_OSCILLATOR_FREQUENCY      204000000
#define OSCILLATOR_CLOCK_FREQUENCY          12000000
#define SDRAM_CLOCK_FREQUENCY               PROCESSOR_OSCILLATOR_FREQUENCY/2
#define EMC_CLOCK_FREQUENCY                 PROCESSOR_OSCILLATOR_FREQUENCY/2

#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing

#define UEZGUI_4357_50WVN_REV               2 // For Rev 1 change to 1. Need to reduce SDRAM size in projects to 16MB.

// For Rev 2.1 units and later Newhaven updated the LCD (but kept the same PN) so use the Rev2 option for correct alignment.
// You cannot tell from looking at the LCD that it is different, so go by the uEZGUI Revision label.
#if (UEZGUI_4357_50WVN_REV == 2)
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD50800480TF_Rev2 // Rev 2 version of LCD
#else
// For Rev 1.X units the UEZBSP_SDRAM_SIZE is 16MB and the project files need to be changed for 16MB only.
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD50800480TF // Original version of LCD
#endif

#define USE_RESISTIVE_TOUCH                 0 // set to 1 to enable 4 wire resistive touch, 0 for cap touch
#define UEZ_ENABLE_CONSOLE_ALT_PWR_COM      0 // set to 1 to enable the console on the J10 header

#define APP_ENABLE_HEARTBEAT_LED            1
#define LPC43XX_ENABLE_M0_CORES             0 // 1 to start M0 core. Should program M0 app first.

#ifdef NDEBUG
#define UEZ_REGISTER                        0
#else
#define UEZ_REGISTER                        1  //Used for registering Queues and Semaphores in the RTOS
#endif

#define UEZ_ENABLE_LOOPBACK_TEST            0 // set to 1 to allow loopback test to run

#if (UEZ_ENABLE_LOOPBACK_TEST == 1)
#define UEZ_ENABLE_TCPIP_STACK              0
#define USB_PORT_A_HOST_ENABLED             0
#else
#define UEZ_ENABLE_TCPIP_STACK              1
#define USB_PORT_A_HOST_ENABLED             1
#endif

#define UEZ_ENABLE_AUDIO_AMP                1
#define SEC_TO_ENABLED                      0 // TO136 security (not loaded yet)
#define SEC_MC_ENABLED                      0 // MC security (not loaded yet)
#define UEZ_ENABLE_USB_HOST_STACK           1
#define USB_PORT_B_HOST_DETECT_ENABLED      1

#define UEZ_ENABLE_USB_DEVICE_STACK         0
#define COMPILE_OPTION_USB_SDCARD_DISK      0//UEZ_ENABLE_USB_DEVICE_STACK
#define UEZ_ENABLE_VIRTUAL_COM_PORT         UEZ_ENABLE_USB_DEVICE_STACK

// Choose one when TCP/IP stack is enabled
#define UEZ_HTTP_SERVER                     UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_BASIC_WEB_SERVER                UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRELESS_NETWORK         0
#define UEZ_WIRELESS_PROGRAM_MODE           0
#define UEZ_AWS_IOT_CLIENT_DEMO             0
#define UEZ_NETWORK_DEMOS                   0

// Expansion Options, set to 1 to enable an expansion board
#define UEZGUI_EXPANSION_DEVKIT             0 //not currently supported on 50WMN
#define UEZ_ENABLE_BUTTON_BOARD             0 // Turn on for button board

//#define UEZ_ICONS_SET                     ICONS_SET_UEZ_OPEN_SOURCE
//#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS
#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS_LARGE // (requires either QSPI flash or both 512KB banks onboard flash)
#define SIMPLEUI_DOUBLE_SIZED_ICONS         0 // 2 to 1 icons

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY                  0
#define ACCEL_DEMO_FLIP_X                   0
#define ACCEL_DEMO_ALLOW_ROTATE             0

#define APP_MENU_ALLOW_TEST_MODE            1
#define APP_DEMO_DRAW                       1
#define APP_DEMO_APPS                       1
#define APP_DEMO_SLIDESHOW                  1
#define APP_DEMO_VIDEO_PLAYER               1
#define APP_DEMO_ENABLE_STARTUP_VIDEO       0

#define INCLUDE_EMWIN                       1
#define APP_DEMO_EMWIN                      INCLUDE_EMWIN
#ifndef FREERTOS_PLUS_TRACE
#define APP_DEMO_COM                        0
#endif

// smaller demos
#define SHOW_GUIDEMO_SPEED                  1
#define SHOW_GUIDEMO_AATEXT                 1
#define SHOW_GUIDEMO_BARGRAPH               1
#define SHOW_GUIDEMO_COLORBAR               1
#define SHOW_GUIDEMO_CURSOR                 1

// large demos
#define SHOW_GUIDEMO_AUTOMOTIVE             1
#define SHOW_GUIDEMO_GRAPH                  1
#define SHOW_GUIDEMO_LISTVIEW               1
#define SHOW_GUIDEMO_TREEVIEW               1
#define SHOW_GUIDEMO_ICONVIEW               1
#define SHOW_GUIDEMO_TRANSPARENTDIALOG      1

#define SHOW_GUIDEMO_SKINNING               1
#define SHOW_GUIDEMO_RADIALMENU             1
#define SHOW_GUIDEMO_TRANSPARENTDIALOG      1
#define SHOW_GUIDEMO_WASHINGMACHINE         1
#define SHOW_GUIDEMO_BITMAP                 1

#define UEZ_SLIDESHOW_NAME                  "uEZGUI-4357-50WVN"
