#define BUILD_NUMBER                        2
#define BUILD_DATE                          "12/02/2013"
#define COMPILER_TYPE                       IAR
#define UEZ_PROCESSOR                       NXP_LPC4088
#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing
#define USB_PORT_B_HOST_DETECT_ENABLED      1
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD43480272MF
#define UEZ_ENABLE_CONSOLE_ALT_PWR_COM      0 // set to 1 to enable the console on the J11 header

#ifdef FREERTOS_PLUS_TRACE
#define configUSE_TRACE_FACILITY            1
#define configUSE_TIMERS                    1
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
#define UEZ_HTTP_SERVER                     0//UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_BASIC_WEB_SERVER                1
#define UEZ_ENABLE_WIRELESS_NETWORK         0
#define UEZ_WIRELESS_PROGRAM_MODE           0

// Expansion Options
#define UEZGUI_EXP_BOARD                    UEZGUI_EXP_NONE
#define ENABLE_UEZ_BUTTON                   0

//#define UEZ_ICONS_SET                       ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS
#define SIMPLEUI_DOUBLE_SIZED_ICONS         0 // 1 to 1 icons

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY                  0
#define ACCEL_DEMO_FLIP_X                   0
#define ACCEL_DEMO_ALLOW_ROTATE             0

#define APP_MENU_ALLOW_TEST_MODE            1
#define APP_DEMO_DRAW                       1
#define APP_DEMO_APPS                       1
#define APP_DEMO_VIDEO_PLAYER               1

#define INCLUDE_EMWIN                       1
#define APP_DEMO_EMWIN                      INCLUDE_EMWIN
#ifndef FREERTOS_PLUS_TRACE
#define APP_DEMO_COM                        1
#endif
#define SHOW_GUIDEMO_AUTOMOTIVE             1
#define SHOW_GUIDEMO_GRAPH                  1
#define SHOW_GUIDEMO_LISTVIEW               1
#define SHOW_GUIDEMO_SPEED                  1
#define SHOW_GUIDEMO_TREEVIEW               1

#define UEZ_SLIDESHOW_NAME                  "uEZGUI-4088-43WQN"
#define SLIDESHOW_PREFETCH_AHEAD            5
#define SLIDESHOW_PREFETCH_BEHIND           1
#define SLIDESHOW_NUM_CACHED_SLIDES         5
