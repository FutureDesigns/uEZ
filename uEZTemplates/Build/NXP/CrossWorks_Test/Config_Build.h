#define BUILD_NUMBER                        1
#define BUILD_DATE                          "08/23/2022"
#define UEZ_PROCESSOR                       $[sym manufacturer]_$[sym processor]
$[ifdef uEZGUI_1788_43WQR]#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_SHARP_LQ043T1DG28$[endif]$[SkipNewLine]
$[ifdef uEZGUI_1788_56VI]#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_DEFAULT_LCD_CONFIG               LCD_CONFIG_INTELTRONIC_LMIX0560NTN53V1$[endif]$[SkipNewLine]
$[ifdef uEZGUI_1788_70WVT]#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM070RBHG04$[endif]$[SkipNewLine]
$[ifdef uEZGUI_1788_70WVM]#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_MICROTIPS_UMSH_8596MD_20T$[endif]$[SkipNewLine]
$[ifdef CARRIER_LPC1788]#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_OKAYA_VGA_LCDC_3_5  //DK-57VTS-LPC1788  
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_HITACHI_TX11D06VM2APA   //DK-43WQH-1788$[endif]$[SkipNewLine]
$[ifdef uEZGUI_4088_43WQN]#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_GENERIC_480X272 // Supports all 5 LCD models.
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_DLC0430BCP12RF // Rev 2.1 HW
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD43480272MF // Rev 2.0 HW
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD43800480CF
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD43480272EF_ASXV
// sunlight readable LCDs
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD43480272EF_ASXP

#define USE_RESISTIVE_TOUCH                 0 // set to 1 to enable 4 wire resistive touch, 0 for cap touch$[endif]
$[ifdef uEZGUI_4357_50WVN]#define PROCESSOR_OSCILLATOR_FREQUENCY      204000000
#define OSCILLATOR_CLOCK_FREQUENCY          12000000
#define SDRAM_CLOCK_FREQUENCY               PROCESSOR_OSCILLATOR_FREQUENCY/2
#define EMC_CLOCK_FREQUENCY                 PROCESSOR_OSCILLATOR_FREQUENCY/2

#define UEZGUI_4357_50WVN_REV               2 // For Rev 1 change to 1. Need to reduce SDRAM size in projects to 16MB.
// For Rev 2.1 units and later Newhaven updated the LCD (but kept the same PN) so use the Rev2 option for correct alignment.
// You cannot tell from looking at the LCD that it is different, so go by the uEZGUI Revision label.
#if (UEZGUI_4357_50WVN_REV == 2)
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD50800480TF_Rev2 // Rev 2 version of LCD
#else
// For Rev 1.X units the UEZBSP_SDRAM_SIZE is 16MB and the project files need to be changed for 16MB only.
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD50800480TF // Original version of LCD
#endif
#define USE_RESISTIVE_TOUCH                 0 // set to 1 to enable 4 wire resistive touch, 0 for cap touch

#define LPC43XX_ENABLE_M0_CORES             0 // 1 to start M0 core. Should program M0 app first.$[endif]
$[ifdef uEZGUI_4357_70WVN]#define PROCESSOR_OSCILLATOR_FREQUENCY      204000000
#define OSCILLATOR_CLOCK_FREQUENCY          12000000
#define SDRAM_CLOCK_FREQUENCY               PROCESSOR_OSCILLATOR_FREQUENCY/2
#define EMC_CLOCK_FREQUENCY                 PROCESSOR_OSCILLATOR_FREQUENCY/2

// If Newhaven revs the LCD you normally cannot tell from looking at the LCD that it is different, so go by the uEZGUI Revision label.
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD70800480EF // 7" version of LCD, Rev 12 or later
#define USE_RESISTIVE_TOUCH                 0 // set to 1 to enable 4 wire resistive touch, 0 for cap touch

#define LPC43XX_ENABLE_M0_CORES             0 // 1 to start M0 core. Should program M0 app first.$[endif]$[SkipNewLine]

#define CONFIG_LOW_LEVEL_TEST_CODE          0 // set to 1 to enable the low level routines such as testing crystal or SDRAM

#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing
#define UEZGUI_EXPANSION_DEVKIT             0 // Turn on for uEZGUI-EXP-DK development board
#define UEZGUI_EXP_BRK_OUT                  0 // Turn on for uEZGUI-EXP-BRK-OUT development board
#define ENABLE_UEZ_BUTTON                   0 // Turn on for button board
#define UEZ_ENABLE_CONSOLE_ALT_PWR_COM      0 // Turn on to enable the console on the Hirose 6 or 10 pin connector

#define INCLUDE_EMWIN                       1 // This is an emWin application so this is always required to be enabled.

// Turn these 3 on to be able to run storage test on USB flash drive on either port
#define USB_PORT_A_HOST_ENABLED             1
#define USB_PORT_B_HOST_DETECT_ENABLED      1
#define UEZ_ENABLE_USB_HOST_STACK           1

#define UEZ_ENABLE_USB_DEVICE_STACK         0
#define UEZ_ENABLE_TCPIP_STACK              0 // TODO add a network test to this application
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_HTTP_SERVER                     0 // Web server that runs from microSD card HTTPROOT directory (INDEX.HTM)
