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
#define BUILD_NUMBER        3
#define BUILD_DATE          "11/8/2010"

#define COMPILER_TYPE RowleyARM
#define UEZ_PROCESSOR       NXP_LPC2478
#define RTOS                FreeRTOS

#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_OKAYA_VGA_LCDC_3_5          //DK-57VTS with LCD Carrier 3-5
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TOSHIBA_QVGA_LCDC_REV2    //DK-57VTS with LCD Carrier 5.7
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TOSHIBA_QVGA_LCDC_3_5     //DK-57TS with LCD Carrier 3-5
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_OKAYA_QVGA_LCDC_3_5       //DK-35TS with LCD Carrier 3-5
//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_HITACHI_TX11D06VM2APA     //DK-43WQH with LCD Carrier 4WQ

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
#define configTOTAL_HEAP_SIZE        ( ( size_t ) (( 48 * 1024 ) - 64))

#define UEZ_ENABLE_WATCHDOG       0 // Turn on watchdog for testing

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

#define INCLUDE_EMWIN               0
#define APP_DEMO_EMWIN              INCLUDE_EMWIN
#define APP_DEMO_YOUR_APP           1 //needs to be off if emWin is on
#define APP_DEMO_COM                1
#define APP_DEMO_DRAW               1
#define APP_DEMO_APPS               1
#define UEZ_ENABLE_USB_DEVICE_STACK 1
#define UEZ_ENABLE_USB_HOST_STACK   1
#define UEZ_ENABLE_TCPIP_STACK      1
#define UEZ_BASIC_WEB_SERVER        0
#define UEZ_HTTP_SERVER             UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_WIRED_NETWORK    UEZ_ENABLE_TCPIP_STACK
#define INTERRUPT_BASED_EMAC        1

#define UEZ_SLIDESHOW_NAME        "DK-LPC2478"

/*-------------------------------------------------------------------------*
 * End of File:  Config_Build.h
 *-------------------------------------------------------------------------*/
