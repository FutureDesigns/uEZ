#define BUILD_NUMBER        0
#define BUILD_DATE          "9/5/2012"

#define COMPILER_TYPE       RenesasRX
#define UEZ_PROCESSOR       RENESAS_RX62N
#define RTOS                FreeRTOS

//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM043NBH02
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM047NBH01

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY        0
#define ACCEL_DEMO_FLIP_X         0
#define ACCEL_DEMO_ALLOW_ROTATE   0

#define PROCESSOR_OSCILLATOR_FREQUENCY      96000000
#define EXTAL_FREQUENCY		12000000
#define ICLK_MUL        	(8)
#define PCLK_MUL        	(4)
#define BCLK_MUL        	(4)

#define EMWIN_RAM_BASE_ADDR  0x08400000
#define EMWIN_RAM_SIZE       0x00200000

#define ICLK_FREQUENCY	EXTAL_FREQUENCY * ICLK_MUL
#define PCLK_FREQUENCY	EXTAL_FREQUENCY * PCLK_MUL
#define BCLK_FREQUENCY	EXTAL_FREQUENCY * BCLK_MUL

#define UEZ_ENABLE_WATCHDOG       0

//#define UEZ_ICONS_SET       ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET       ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET ICONS_SET_PROFESSIONAL_ICONS_LARGE
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
#define UEZ_ENABLE_USB_HOST_STACK   0

#define UEZ_ENABLE_WIRED_NETWORK   1

#define UEZ_SLIDESHOW_NAME        "DK-TS-RX62N"

#define SLIDESHOW_PREFETCH_AHEAD 5
#define SLIDESHOW_PREFETCH_BEHIND 1
#define SLIDESHOW_NUM_CACHED_SLIDES 5

#define STATIC_MEMORY_ALLOC		1
