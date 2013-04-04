#define BUILD_NUMBER                        2
#define BUILD_DATE                          "10/4/2012"

#define COMPILER_TYPE                       IAR
#define UEZ_PROCESSOR                       NXP_LPC1788

#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM070RBHG04

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY                  0
#define ACCEL_DEMO_FLIP_X                   0
#define ACCEL_DEMO_ALLOW_ROTATE             0

#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000

#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing

//#define UEZ_ICONS_SET                       ICONS_SET_UEZ_OPEN_SOURCE
//#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS
#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS         0 // 2 to 1 icons

#define USB_PORT_B_HOST_DETECT_ENABLED      0
#define APP_MENU_ALLOW_TEST_MODE            1

#define APP_DEMO_YOUR_APP                   1
#define APP_DEMO_COM                        1
#define APP_DEMO_DRAW                       1
#define APP_DEMO_APPS                       1
#define UEZ_ENABLE_TCPIP_STACK              1
#define UEZ_ENABLE_USB_HOST_STACK           1

#define INCLUDE_EMWIN                       1
#define APP_DEMO_EMWIN                      INCLUDE_EMWIN
#define SHOW_GUIDEMO_AUTOMOTIVE		    	1
#define SHOW_GUIDEMO_GRAPH		   	 		1
#define SHOW_GUIDEMO_LISTVIEW		    	1
#define SHOW_GUIDEMO_SPEED		    		1
#define SHOW_GUIDEMO_TREEVIEW		    	1

#define UEZ_ENABLE_WIRED_NETWORK            1

#define UEZ_SLIDESHOW_NAME                  "uEZGUI-1788-70WVT"

#define SLIDESHOW_PREFETCH_AHEAD            5
#define SLIDESHOW_PREFETCH_BEHIND           1
#define SLIDESHOW_NUM_CACHED_SLIDES         5

#ifdef FREERTOS_PLUS_TRACE
#define configUSE_TRACE_FACILITY            1
#define configUSE_TIMERS                    1
#define SELECTED_PORT PORT_ARM_CortexM
#endif
