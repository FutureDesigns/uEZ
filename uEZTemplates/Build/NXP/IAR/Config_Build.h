#define BUILD_NUMBER                        1
#define BUILD_DATE                          "08/23/2022"
#define COMPILER_TYPE                       IAR
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
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD43480272MF
#define USE_RESISTIVE_TOUCH                 0 // set to 1 to enable 4 wire resistive touch, 0 for cap touch$[endif]
$[ifdef uEZGUI_4357_50WVN]#define PROCESSOR_OSCILLATOR_FREQUENCY      204000000
#define OSCILLATOR_CLOCK_FREQUENCY          12000000
#define SDRAM_CLOCK_FREQUENCY               PROCESSOR_OSCILLATOR_FREQUENCY/2
#define EMC_CLOCK_FREQUENCY                 PROCESSOR_OSCILLATOR_FREQUENCY/2

// For Rev 2.1 units and later Newhaven updated the LCD (but kept the same PN) so use the Rev2 option for correct alignment.
// You cannot tell from looking at the LCD that it is different, so go by the uEZGUI Revision label.
//#define UEZ_DEFAULT_LCD_CONFIG            LCD_CONFIG_NEWHAVEN_NHD50800480TF // Original version of LCD
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD50800480TF_Rev2 // Rev 2 version of LCD
#define USE_RESISTIVE_TOUCH                 0 // set to 1 to enable 4 wire resistive touch, 0 for cap touch$[endif]
$[ifdef uEZGUI_4357_70WVN]#define PROCESSOR_OSCILLATOR_FREQUENCY      204000000
#define OSCILLATOR_CLOCK_FREQUENCY          12000000
#define SDRAM_CLOCK_FREQUENCY               PROCESSOR_OSCILLATOR_FREQUENCY/2
#define EMC_CLOCK_FREQUENCY                 PROCESSOR_OSCILLATOR_FREQUENCY/2

// If Newhaven revs the LCD again you cannot tell from looking at the LCD that it is different, so go by the uEZGUI Revision label.
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_NEWHAVEN_NHD70800480EF // 7" version of LCD, Rev 12 or later
#define USE_RESISTIVE_TOUCH                 0 // set to 1 to enable 4 wire resistive touch, 0 for cap touch$[endif]$[SkipNewLine]

#define UEZ_ENABLE_WATCHDOG                 0 // Turn on watchdog for testing
#define UEZGUI_EXPANSION_DEVKIT             0 // Turn on for uEZGUI-EXP-DK development board
#define UEZGUI_EXP_BRK_OUT                  0 // Turn on for uEZGUI-EXP-BRK-OUT development board
#define ENABLE_UEZ_BUTTON                   0 // Turn on for button board
#define UEZ_ENABLE_CONSOLE_ALT_PWR_COM      0 // Turn on to enable the console on the Hirose 6 or 10 pin connector

#define INCLUDE_EMWIN                       1 // This is an emWin application so this is always required to be enabled.

// Turn these 3 on to 1 to be able to use the 2 USB ports
#define USB_PORT_A_HOST_ENABLED             0
#define USB_PORT_B_HOST_DETECT_ENABLED      0
#define UEZ_ENABLE_USB_HOST_STACK           0

#define UEZ_ENABLE_USB_DEVICE_STACK         0

#define UEZ_ENABLE_TCPIP_STACK              0
#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
#define UEZ_HTTP_SERVER                     0 // Web server that runs from microSD card HTTPROOT directory (INDEX.HTM)
