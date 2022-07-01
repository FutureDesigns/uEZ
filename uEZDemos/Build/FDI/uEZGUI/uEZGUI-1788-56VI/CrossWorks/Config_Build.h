#define BUILD_NUMBER                        2
#define BUILD_DATE                          "12/02/2013"
#define COMPILER_TYPE                       RowleyARM
#define UEZ_PROCESSOR                       NXP_LPC1788
#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing
#define USB_PORT_B_HOST_DETECT_ENABLED      1
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1
#define UEZ_ENABLE_CONSOLE_ALT_PWR_COM      0 // set to 1 to enable the console on the J10 header

#ifdef FREERTOS_PLUS_TRACE
#define configUSE_TRACE_FACILITY  			1
#define configUSE_TIMERS          			1
#define SELECTED_PORT PORT_ARM_CortexM
#endif

#ifdef NDEBUG
#define UEZ_REGISTER              			0
#else
#define UEZ_REGISTER              			1  //Used for registering Queues and Semaphores in the RTOS
#endif

#define UEZ_ENABLE_AUDIO_AMP                1
#define UEZ_ENABLE_USB_HOST_STACK           1
#define UEZ_ENABLE_USB_DEVICE_STACK         1
#define UEZ_ENABLE_TCPIP_STACK              1
// Choose one when TCP/IP stack is enabled
#define UEZ_HTTP_SERVER                     UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_BASIC_WEB_SERVER                0
#define UEZ_ENABLE_WIRELESS_NETWORK         0
#define UEZ_WIRELESS_PROGRAM_MODE           0

// Expansion Options, set to 1 to enable an expansion board
#define UEZGUI_EXPANSION_DEVKIT             0
#define UEZGUI_EXP_BRK_OUT                  0
#define UEZ_ENABLE_BUTTON_BOARD             0 // Turn on for button board

//#define UEZ_ICONS_SET                     ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET                     ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS         1 // 2 to 1 icons

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY                  0
#define ACCEL_DEMO_FLIP_X                   0
#define ACCEL_DEMO_ALLOW_ROTATE             0

#define APP_MENU_ALLOW_TEST_MODE            1
#define APP_DEMO_DRAW                       1
#define APP_DEMO_APPS                       1
#define APP_DEMO_SLIDESHOW             		1
#define APP_DEMO_VIDEO_PLAYER               0

#define INCLUDE_EMWIN                       0
#define APP_DEMO_EMWIN                      INCLUDE_EMWIN
#ifndef FREERTOS_PLUS_TRACE
#define APP_DEMO_COM                        1
#endif
#define APP_DEMO_YOUR_APP                   0
#define SHOW_GUIDEMO_AUTOMOTIVE             1
#define SHOW_GUIDEMO_GRAPH                  1
#define SHOW_GUIDEMO_LISTVIE                1
#define SHOW_GUIDEMO_SPEED                  1
#define SHOW_GUIDEMO_TREEVIEW               1

#define UEZ_SLIDESHOW_NAME                  "uEZGUI-1788-56VI"
#define SLIDESHOW_PREFETCH_AHEAD            5
#define SLIDESHOW_PREFETCH_BEHIND           1
#define SLIDESHOW_NUM_CACHED_SLIDES         5
