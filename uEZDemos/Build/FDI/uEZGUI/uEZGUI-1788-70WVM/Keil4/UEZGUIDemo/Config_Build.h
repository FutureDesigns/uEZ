#define BUILD_NUMBER                        2
#define BUILD_DATE                          "10/4/2012"

#define COMPILER_TYPE                       Keil4
#define UEZ_PROCESSOR                       NXP_LPC1788

#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_MICROTIPS_UMSH_8596MD_20T

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY                  0
#define ACCEL_DEMO_FLIP_X                   0
#define ACCEL_DEMO_ALLOW_ROTATE             0

#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000

#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing

//#define UEZ_ICONS_SET                       ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS         1 // 2 to 1 icons

#define USB_PORT_B_HOST_DETECT_ENABLED      0
#define APP_MENU_ALLOW_TEST_MODE            1

#define INCLUDE_EMWIN                       1
#define APP_DEMO_EMWIN                      0
#ifndef FREERTOS_PLUS_TRACE
#define APP_DEMO_YOUR_APP                   1
#define APP_DEMO_COM                        1
#define APP_DEMO_DRAW                       1
#endif
#define APP_DEMO_APPS                       1
#define UEZ_ENABLE_TCPIP_STACK              1
#define UEZ_ENABLE_USB_HOST_STACK           1
#define UEZ_ENABLE_USB_DEVICE_STACK         1
#define APP_DEMO_VIDEO_PLAYER               1
#define UEZ_BASIC_WEB_SERVER                0
#define UEZ_HTTP_SERVER                     UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRELESS_NETWORK         0
#define UEZ_WIRELESS_PROGRAM_MODE           0

#define UEZ_SLIDESHOW_NAME                  "uEZGUI-1788-70WVM"

#define SLIDESHOW_PREFETCH_AHEAD            5
#define SLIDESHOW_PREFETCH_BEHIND           1
#define SLIDESHOW_NUM_CACHED_SLIDES         5

#ifdef FREERTOS_PLUS_TRACE
#define configUSE_TRACE_FACILITY  1
#define configUSE_TIMERS          1
#define SELECTED_PORT PORT_ARM_CortexM
#endif

#ifdef NDEBUG
#define UEZ_REGISTER              0
#else
#define UEZ_REGISTER              1  //Used for registering Queues and Semaphores in the RTOS
#endif
