/*-------------------------------------------------------------------------*
 * File:  Config_Build.h
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#define BUILD_NUMBER        2
#define BUILD_DATE          "11/30/2010"

#define COMPILER_TYPE       IAR
#define UEZ_PROCESSOR       NXP_LPC1788
#define RTOS                FreeRTOS

//#define PROCESSOR_OSCILLATOR_FREQUENCY       80000000
//#define PROCESSOR_OSCILLATOR_FREQUENCY       96000000
#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000

//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TOSHIBA_QVGA_LCDC_REV2    //DK-57VTS with LCD Carrier 5.7
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TOSHIBA_QVGA_LCDC_3_5     //DK-57TS with LCD Carrier 3-5
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_OKAYA_QVGA_LCDC_3_5       //DK-35TS with LCD Carrier 3-5
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_HITACHI_TX11D06VM2APA     //DK-43WQH with LCD Carrier 4WQ
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_SEIKO_43WQW1T
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_KOE_TX13D06VM2BAA     //5" WVGA 800x480 KOE display
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_OKAYA_VGA_LCDC_3_5          //DK-57VTS with LCD Carrier 3-5


// Modify the default accelerometer demo settings
#if 0 // Set to 1 for Sekio displays
#define ACCEL_DEMO_SWAP_XY        0
#define ACCEL_DEMO_FLIP_X         0
#define ACCEL_DEMO_G_TO_PIXELS    32
#define ACCEL_DEMO_ALLOW_ROTATE   0
#else
#define ACCEL_DEMO_SWAP_XY        1
#define ACCEL_DEMO_FLIP_X         1
#define ACCEL_DEMO_G_TO_PIXELS    32
#define ACCEL_DEMO_ALLOW_ROTATE   0
#endif

// Use up 48K of the heap
#define configTOTAL_HEAP_SIZE		( ( size_t ) (( 48 * 1024 ) - 64))

#define UEZ_ENABLE_WATCHDOG       0 // Turn on watchdog for testing

#define DKTS_BUTTON_SLIDE_SHOW_DEMO        0 // set to 1 to be able to load emwin or slideshows using buttons
//#define UEZ_ICONS_SET       ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET       ICONS_SET_PROFESSIONAL_ICONS

#define CARRIER_R2      0
#define CARRIER_R4      1

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

#define INCLUDE_EMWIN                   1
#define APP_DEMO_EMWIN                  INCLUDE_EMWIN
#define APP_DEMO_YOUR_APP               0 //needs to be off if emWin is on
#define APP_DEMO_COM                    0
#define APP_DEMO_DRAW                   1
#define APP_DEMO_APPS                   0
#define UEZ_ENABLE_TCPIP_STACK          1
#define UEZ_ENABLE_USB_HOST_STACK       1
#define UEZ_ENABLE_WIRED_NETWORK        1
#define INTERRUPT_BASED_EMAC            1

#define UEZ_SLIDESHOW_NAME              "DK-LPC1788"

#define UEZ_ENABLE_GAINSPAN             1
#define CONFIG_USE_DK_APP               1