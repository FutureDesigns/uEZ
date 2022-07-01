#define BUILD_NUMBER                        1
#define BUILD_DATE                          "07/22/2016"
#define COMPILER_TYPE                       RowleyARM
#define UEZ_PROCESSOR                       NXP_LPC4088
#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD43480272MF
#define USE_RESISTIVE_TOUCH                 0 // set to 1 to enable 4 wire resistive touch, 0 for cap touch

#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing
#define UEZGUI_EXPANSION_DEVKIT             0 // Turn on for uEZGUI-EXP-DK development board
#define UEZGUI_EXP_BRK_OUT                  0 // Turn on for uEZGUI-EXP-BRK-OUT development board
#define ENABLE_UEZ_BUTTON                   0 // Turn on for button board
#define UEZ_ENABLE_CONSOLE_ALT_PWR_COM      0 // Turn on to enable the console on the Hirose 6 or 10 pin connector

#define INCLUDE_EMWIN                       1
#define UEZ_ENABLE_TCPIP_STACK              0
#define USB_PORT_B_HOST_DETECT_ENABLED      0
#define UEZ_ENABLE_USB_HOST_STACK           0
#define UEZ_ENABLE_USB_DEVICE_STACK         0
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_HTTP_SERVER                     0 // Web server that runs from microSD card HTTPROOT directory (INDEX.HTM)