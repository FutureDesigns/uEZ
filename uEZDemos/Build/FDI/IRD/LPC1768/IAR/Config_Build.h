#define BUILD_NUMBER                        3
#define BUILD_DATE                          "10/9/2012"
#define COMPILER_TYPE                       IAR
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
#define SEGGER_ENABLE_SYSTEM_VIEW           1
#endif

// Components to allow to run
#define UEZ_ENABLE_TCPIP_STACK              0

// Choose one when stack is enabled
#define UEZ_HTTP_SERVER                     UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_BASIC_WEB_SERVER                0
#define DEBUG_HTTP_SERVER                   0

#define UEZ_ENABLE_USB_HOST_STACK           0

#define UEZ_ENABLE_USB_DEVICE_STACK         0   // Requires HW modification
#define COMPILE_OPTION_GENERIC_BULK         UEZ_ENABLE_USB_DEVICE_STACK
#define COMPILE_OPTION_USB_KEYBOARD         0

//#define configTOTAL_HEAP_SIZE               ( ( size_t ) (( 26 * 1024 ) - 64))

