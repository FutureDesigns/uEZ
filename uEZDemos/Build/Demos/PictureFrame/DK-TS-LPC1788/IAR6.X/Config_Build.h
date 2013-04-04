#define BUILD_NUMBER        2
#define BUILD_DATE          "5/31/2012"

#define COMPILER_TYPE       IAR
#define UEZ_PROCESSOR       NXP_LPC1788


//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_OKAYA_VGA_LCDC_3_5          //DK-57VTS with LCD Carrier 3-5
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TOSHIBA_QVGA_LCDC_REV2    //DK-57VTS with LCD Carrier 5.7
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TOSHIBA_QVGA_LCDC_3_5     //DK-57TS with LCD Carrier 3-5
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_OKAYA_QVGA_3x5_LCDC_3_5//LCD_CONFIG_OKAYA_QVGA_LCDC_3_5       //DK-35TS with LCD Carrier 3-5
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_HITACHI_TX11D06VM2APA     //DK-43WQH with LCD Carrier 4WQ
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_SEIKO_43WQW1T 

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY        0
#define ACCEL_DEMO_FLIP_X         0
#define ACCEL_DEMO_ALLOW_ROTATE   0

#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000

#define UEZ_ENABLE_WATCHDOG       0

//#define UEZ_ICONS_SET       ICONS_SET_UEZ_OPEN_SOURCE
//#define UEZ_ICONS_SET       ICONS_SET_PROFESSIONAL_ICONS
#define UEZ_ICONS_SET ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS 0 // 1 to 1 icons

#define FDI_PLATFORM        CARRIER_R4
//#define FDI_PLATFORM        CARRIER_R2

#if (FDI_PLATFORM == CARRIER_R4)
#define UEZ_ENABLE_WIRELESS_NETWORK     0
#define UEZ_ENABLE_AUDIO_CODEC          1
#define UEZ_ENABLE_AUDIO_AMP            1
#define UEZ_ENABLE_I2S_AUDIO            0
#else
#define UEZ_ENABLE_WIRELESS_NETWORK     0   //not avalible
#define UEZ_ENABLE_AUDIO_CODEC          0   //not avalible
#define UEZ_ENABLE_AUDIO_AMP            0   //not avalible
#define UEZ_ENABLE_I2S_AUDIO            0   //not avalible
#endif

#define INCLUDE_EMWIN               1
#define APP_DEMO_EMWIN              INCLUDE_EMWIN
#define UEZ_ENABLE_TCPIP_STACK      1
#define UEZ_ENABLE_USB_HOST_STACK   1

#define UEZ_ENABLE_WIRED_NETWORK   1

#define UEZ_SLIDESHOW_NAME        "DK-TS-LPC1788"

#define SLIDESHOW_PREFETCH_AHEAD 5
#define SLIDESHOW_PREFETCH_BEHIND 1
#define SLIDESHOW_NUM_CACHED_SLIDES 5
