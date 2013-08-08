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
#define UEZ_PROCESSOR       RENESAS_RX62N
#define RTOS                FreeRTOS

#define UEZ_DEFAULT_LCD_CONFIG	LCD_CONFIG_TIANMA_QVGA_LCDC_3_5

// Use up 48K of the heap
#define configTOTAL_HEAP_SIZE		( ( size_t ) (( 50 * 1024 ) - 64))
#define configMINIMAL_STACK_SIZE	( 140 )

#define PROCESSOR_OSCILLATOR_FREQUENCY      96000000
#define EXTAL_FREQUENCY		12000000
#define ICLK_MUL        	(8)
#define PCLK_MUL        	(4)
#define BCLK_MUL        	(4)

#define ICLK_FREQUENCY	EXTAL_FREQUENCY * ICLK_MUL
#define PCLK_FREQUENCY	EXTAL_FREQUENCY * PCLK_MUL
#define BCLK_FREQUENCY	EXTAL_FREQUENCY * BCLK_MUL

#define UEZ_ENABLE_WATCHDOG       0 // Turn on watchdog for testing

//#define UEZ_ICONS_SET       ICONS_SET_UEZ_OPEN_SOURCE
#define UEZ_ICONS_SET       ICONS_SET_PROFESSIONAL_ICONS
//#define UEZ_ICONS_SET ICONS_SET_PROFESSIONAL_ICONS_LARGE
#define SIMPLEUI_DOUBLE_SIZED_ICONS 0 // 1 to 1 icons

#define UEZ_USE_EXTERNAL_EEPROM   0

#define GUI_SUPPORT_MEMDEV        (0)

#define UEZ_ENABLE_TCPIP_STACK      0
#define UEZ_ENABLE_USB_HOST_STACK   0
#define UEZ_ENABLE_USB_DEVICE_STACK	0
#define INTERRUPT_BASED_EMAC        0

#define UEZ_ENABLE_WIRED_NETWORK            UEZ_ENABLE_TCPIP_STACK
