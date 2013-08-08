#define BUILD_NUMBER                        2
#define BUILD_DATE                          "05/23/2013"
#define COMPILER_TYPE                       IAR
#define UEZ_PROCESSOR                       NXP_LPC1788
#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing

#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_SHARP_LQ043T1DG28  // for 43WQR
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM070RBHG04 // for 70WVT
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1 // for 56VI
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_SHARP_LQ043T3DG01 //for 43WQS
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_SEIKO_70WVW2T // for 70WVE

#ifdef FREERTOS_PLUS_TRACE
#define configUSE_TRACE_FACILITY  1
#define configUSE_TIMERS          1
#define SELECTED_PORT PORT_ARM_CortexM
#endif

#define USB_PORT_B_HOST_DETECT_ENABLED      0
#define APP_MENU_ALLOW_TEST_MODE            0
#define UEZ_ENABLE_TCPIP_STACK              0
#define UEZ_ENABLE_USB_HOST_STACK           0
#define UEZ_ENABLE_WIRED_NETWORK            0

#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET                       ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS         1 // 2 to 1 icons

#ifdef FREERTOS_PLUS_TRACE
#define configUSE_TRACE_FACILITY            1
#define configUSE_TIMERS                    1
#define SELECTED_PORT PORT_ARM_CortexM
#endif