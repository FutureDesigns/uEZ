/*-------------------------------------------------------------------------*
 * File:  Config_Build.h
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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
#define COMPILER_TYPE       RenesasRX
#define UEZ_PROCESSOR       RENESAS_RX63N
#define RTOS                FreeRTOS

//#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM043NBH02
#define UEZ_DEFAULT_LCD_CONFIG              LCD_CONFIG_TIANMA_TM047NBH01

// Use up 48K of the heap
#define configTOTAL_HEAP_SIZE		( ( size_t ) (( 48 * 1024 ) - 64))
#define configMINIMAL_STACK_SIZE	( 140 )

#define PROCESSOR_OSCILLATOR_FREQUENCY      96000000
#define EXTAL_FREQUENCY		12000000
#define ICLK_MUL        	(8)
#define PCLK_MUL        	(4)
#define BCLK_MUL        	(4)

#define ICLK_FREQUENCY	EXTAL_FREQUENCY * ICLK_MUL
#define PCLK_FREQUENCY	EXTAL_FREQUENCY * PCLK_MUL
#define BCLK_FREQUENCY	EXTAL_FREQUENCY * BCLK_MUL

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

#define UEZ_ENABLE_WATCHDOG       0 // Turn on watchdog for testing

//#define UEZ_ICONS_SET       ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET       ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS 0 // 1 to 1 icons

#define UEZ_USE_EXTERNAL_EEPROM   0

#define FDI_PLATFORM        CARRIER_R4
//#define FDI_PLATFORM        CARRIER_R2

#if (FDI_PLATFORM == CARRIER_R4)
#define CARRIER_R2      0
#define CARRIER_R4      1
#define UEZ_ENABLE_WIRELESS_NETWORK     0
#define UEZ_ENABLE_AUDIO_CODEC          1
#define UEZ_ENABLE_AUDIO_AMP            1
#define UEZ_ENABLE_I2S_AUDIO            0
#else
#define CARRIER_R2      1
#define CARRIER_R4      0
#define UEZ_ENABLE_WIRELESS_NETWORK     0   //not avalible
#define UEZ_ENABLE_AUDIO_CODEC          0   //not avalible
#define UEZ_ENABLE_AUDIO_AMP            0   //not avalible
#define UEZ_ENABLE_I2S_AUDIO            0   //not avalible
#endif

#define INCLUDE_EMWIN               0
#define EMWIN_LOAD_ONCE             1
#define APP_DEMO_EMWIN              INCLUDE_EMWIN
#define INCLUDE_UEZ_TITLE_SCREEN    1
#define APP_DEMO_YOUR_APP           0
#define APP_DEMO_COM                1
#define APP_DEMO_DRAW               1
#define APP_DEMO_APPS               1
#define APP_SLIDESHOW				1
#define UEZ_ENABLE_TCPIP_STACK      1
#define UEZ_ENABLE_WIRED_NETWORK    1
#define INTERRUPT_BASED_EMAC        0
#define UEZ_ENABLE_USB_HOST_STACK   0
#define UEZ_ENABLE_USB_DEVICE_STACK	0

#define UEZ_SLIDESHOW_NAME        "DK-RX63N"

#define GUI_SUPPORT_CURSOR 0
