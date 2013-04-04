#define BUILD_NUMBER                        3
#define BUILD_DATE                          "10/9/2012"

#define COMPILER_TYPE                       IAR
#define UEZ_PROCESSOR                       NXP_LPC1768

#define PROCESSOR_OSCILLATOR_FREQUENCY      100000000

// Components to allow to run
#define UEZ_ENABLE_TCPIP_STACK              1

// Choose one when stack is enabled
#define UEZ_HTTP_SERVER                     UEZ_ENABLE_TCPIP_STACK
#define UEZ_BASIC_WEB_SERVER                0
#define DEBUG_HTTP_SERVER                   0

#define UEZ_ENABLE_USB_HOST_STACK           1

#define UEZ_ENABLE_USB_DEVICE_STACK         0   // Requires HW modification
#define COMPILE_OPTION_GENERIC_BULK         UEZ_ENABLE_USB_DEVICE_STACK
#define COMPILE_OPTION_USB_KEYBOARD         0

#define UEZ_ENABLE_WIRED_NETWORK            1

#define configTOTAL_HEAP_SIZE               ( ( size_t ) (( 26 * 1024 ) - 64))

#ifndef MAIN_TASK_STACK_SIZE
#define MAIN_TASK_STACK_SIZE                UEZ_TASK_STACK_BYTES(2048)
#endif

