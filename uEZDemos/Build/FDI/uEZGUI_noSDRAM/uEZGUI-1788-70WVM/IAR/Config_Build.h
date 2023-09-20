#define BUILD_NUMBER                        2
#define BUILD_DATE                          "12/02/2013"
#define COMPILER_TYPE                       IAR
#define UEZ_PROCESSOR                       NXP_LPC1788
#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_MICROTIPS_UMSH_8596MD_20T
#define UEZ_ENABLE_CONSOLE_ALT_PWR_COM      0 // set to 1 to enable the console on the J11 header

#ifdef FREERTOS_PLUS_TRACE
#define configUSE_TRACE_FACILITY            1
#define configUSE_TIMERS                    1
#define SELECTED_PORT PORT_ARM_CortexM
#endif

#ifdef NDEBUG
#define UEZ_REGISTER                        0
#else
#define UEZ_REGISTER                        1  //Used for registering Queues and Semaphores in the RTOS
#endif

#define UEZ_ENABLE_LOOPBACK_TEST            1 // set to 1 to allow loopback test to run

#if (UEZ_ENABLE_LOOPBACK_TEST == 1)
#define USB_PORT_A_HOST_ENABLED             0
#define USB_PORT_B_HOST_DETECT_ENABLED      0 // turn this off so we can loopback test GPIO pin
#else
#define USB_PORT_A_HOST_ENABLED             1
#define USB_PORT_B_HOST_DETECT_ENABLED      1
#endif

#define UEZ_ENABLE_AUDIO_AMP                1
#define UEZ_ENABLE_USB_HOST_STACK           1
#define UEZ_ENABLE_USB_DEVICE_STACK         1
// Choose one when TCP/IP stack is enabled
#define UEZ_ENABLE_TCPIP_STACK              0
#define UEZ_HTTP_SERVER                     UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_BASIC_WEB_SERVER                0
#define UEZ_ENABLE_WIRELESS_NETWORK         0
#define UEZ_WIRELESS_PROGRAM_MODE           0

// Expansion Options, set to 1 to enable an expansion board
#define UEZGUI_EXPANSION_DEVKIT             0
#define UEZ_ENABLE_BUTTON_BOARD             0 // Turn on for button board


#define APP_MENU_ALLOW_TEST_MODE            1

#define UEZ_SLIDESHOW_NAME                  "uEZGUI-1788-70WVM"
