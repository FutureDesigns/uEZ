#define BUILD_NUMBER                        2
#define BUILD_DATE                          "10/4/2012"

#define COMPILER_TYPE                       RowleyARM
#define UEZ_PROCESSOR                       NXP_LPC2478
#define PROCESSOR_OSCILLATOR_FREQUENCY      72000000

#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_SEIKO_70WVW2T

#define PLATFORM_USB_PORT_IS_USB_HOST       0 // Use built on USB Host

#define UEZ_ENABLE_CONSOLE_TYPE             CONSOLE_TYPE_UART0

//#define UEZ_ICONS_SET                       ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS         1

#define INTERRUPT_BASED_EMAC                1

#define INCLUDE_EMWIN                       0
#define APP_DEMO_EMWIN                      INCLUDE_EMWIN
#define APP_DEMO_YOUR_APP                   0
#define APP_DEMO_COM                        1
#define APP_DEMO_DRAW                       1
#define APP_DEMO_APPS                       1
#define UEZ_ENABLE_TCPIP_STACK              1
#define UEZ_ENABLE_USB_HOST_STACK           1
#define UEZ_ENABLE_USB_DEVICE_STACK         1
#define UEZ_BASIC_WEB_SERVER                0
#define UEZ_HTTP_SERVER                     UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRELESS_NETWORK         0
#define UEZ_WIRELESS_PROGRAM_MODE           0

#define UEZ_SLIDESHOW_NAME                  "uEZGUI-2478-70WVE"

#define SLIDESHOW_PREFETCH_AHEAD            5
#define SLIDESHOW_PREFETCH_BEHIND           1
#define SLIDESHOW_NUM_CACHED_SLIDES         5
