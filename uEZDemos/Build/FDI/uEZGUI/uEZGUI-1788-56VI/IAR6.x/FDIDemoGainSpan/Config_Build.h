#define BUILD_NUMBER        2
#define BUILD_DATE          "11/30/2010"

#define COMPILER_TYPE       IAR
#define UEZ_PROCESSOR       NXP_LPC1788

#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY        0
#define ACCEL_DEMO_FLIP_X         0
#define ACCEL_DEMO_G_TO_PIXELS    32
#define ACCEL_DEMO_ALLOW_ROTATE   0

#if (RTOS == FreeRTOS)
    // Use up 48K of the heap
    #define configTOTAL_HEAP_SIZE        ( ( size_t ) (( 48 * 1024 ) - 64))

#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#elif (RTOS == SafeRTOS)
    #define configSMALL_BUFFERS_SIZE    ( ( size_t ) ( 30 * 1024 ) )
    #define configTASK_STACK_BUF_SIZE    ( ( size_t ) ( 30 * 1024 ) )

    #define NO_DYNAMIC_MEMORY_ALLOC

    /* IAR Stateviewer Queue Registry Options. */
    #define configQUEUE_REGISTRY_OFF    ( 0 )
    #define configQUEUE_REGISTRY_SIMPLE    ( 1 )
    #define configQUEUE_REGISTRY_FULL    ( 2 )

    #define configQUEUE_REGISTRY_OPTION    configQUEUE_REGISTRY_FULL
#endif


#define UEZ_ENABLE_WATCHDOG       1 // Turn on watchdog for testing

//#define UEZ_ICONS_SET       ICONS_SET_UEZ_OPEN_SOURCE
//#define UEZ_ICONS_SET       ICONS_SET_PROFESSIONAL_ICONS
#define UEZ_ICONS_SET ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS 0 // 2 to 1 icons

#define UEZ_USE_EXTERNAL_EEPROM               0
#define USB_PORT_B_HOST_DETECT_ENABLED        0
#define LIGHT_SENSOR_ENABLED                  0
#define APP_MENU_ALLOW_TEST_MODE              1

#define INCLUDE_EMWIN               1
#define APP_DEMO_EMWIN              INCLUDE_EMWIN
#define APP_DEMO_YOUR_APP           1
#define APP_DEMO_COM                1
#define APP_DEMO_DRAW               1
#define APP_DEMO_APPS               1
#define UEZ_ENABLE_TCPIP_STACK      1
#define UEZ_ENABLE_USB_HOST_STACK   0
#define UEZ_ENABLE_WIRED_NETWORK    UEZ_ENABLE_TCPIP_STACK
#define INTERRUPT_BASED_EMAC        1
#define UEZ_ENABLE_GAINSPAN         1

#define UEZ_SLIDESHOW_NAME        "uEZGUI-1788-56VI"

#define SLIDESHOW_PREFETCH_AHEAD 5
#define SLIDESHOW_PREFETCH_BEHIND 1
#define SLIDESHOW_NUM_CACHED_SLIDES 5

#define CONFIG_USE_EXP4   0

