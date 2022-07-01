#define BUILD_NUMBER        2
#define BUILD_DATE          "10/19/2015"
#define COMPILER_TYPE       RowleyARM
#define UEZ_PROCESSOR       NXP_LPC1768
//#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing
#define IRD_STARTUP                 1
#define FAST_STARTUP 				0

#define APP_MENU_ALLOW_TEST_MODE    0

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
// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY        0
#define ACCEL_DEMO_FLIP_X         0
#define ACCEL_DEMO_G_TO_PIXELS    32
#define ACCEL_DEMO_ALLOW_ROTATE   0

//#define UEZ_ICONS_SET       ICONS_SET_PROFESSIONAL_ICONS
//#define SIMPLEUI_DOUBLE_SIZED_ICONS 0 // 1 to 1 icons
#define UEZ_ENABLE_TCPIP_STACK      0
#define UEZ_ENABLE_USB_HOST_STACK   0

#define UEZ_ENABLE_AUDIO_AMP                0
#define UEZ_ENABLE_USB_DEVICE_STACK         0
// Choose one when TCP/IP stack is enabled
#define UEZ_HTTP_SERVER                     UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_BASIC_WEB_SERVER                0
#define UEZ_ENABLE_WIRELESS_NETWORK         0
#define UEZ_WIRELESS_PROGRAM_MODE           0

//#define UEZ_SLIDESHOW_NAME        "IRD-1768"