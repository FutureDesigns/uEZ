#define BUILD_NUMBER                        2
#define BUILD_DATE                          "10/4/2012"

#define COMPILER_TYPE                       RowleyARM
#define UEZ_PROCESSOR                       NXP_LPC1788

// for older Sharp Display in Rev 4
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_SHARP_LQ043T3DG01

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY                  0
#define ACCEL_DEMO_FLIP_X                   0
#define ACCEL_DEMO_ALLOW_ROTATE             0

#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000

#define UEZ_ENABLE_WATCHDOG                 0

//#define UEZ_ICONS_SET                       ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS
#define SIMPLEUI_DOUBLE_SIZED_ICONS         0 // 1 to 1 icons

#define UEZ_USE_EXTERNAL_EEPROM             0

#define APP_MENU_ALLOW_TEST_MODE            1

#define INCLUDE_EMWIN                       0
#define APP_DEMO_EMWIN                      INCLUDE_EMWIN
#ifndef FREERTOS_PLUS_TRACE
#define APP_DEMO_YOUR_APP                   1
#define APP_DEMO_COM                        1
#endif
#define APP_DEMO_DRAW                       1
#define APP_DEMO_APPS                       1
#define UEZ_ENABLE_TCPIP_STACK              1
#define UEZ_ENABLE_USB_HOST_STACK           1

#define UEZ_ENABLE_WIRED_NETWORK            1

#define UEZ_SLIDESHOW_NAME                  "uEZGUI-1788-43WQS"

#ifdef FREERTOS_PLUS_TRACE
#define configUSE_TRACE_FACILITY  1
#define configUSE_TIMERS          1
#define SELECTED_PORT PORT_ARM_CortexM
#endif