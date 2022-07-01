/*-------------------------------------------------------------------------*
 * File:  Config_App.h
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

#ifndef _CONFIG_APP_H_
#define _CONFIG_APP_H_

#define PROJECT_NAME                        "uEZ GUI Demo"
#define VERSION_AS_TEXT                     UEZ_VERSION_STRING
#define VERSION_HIGH                        UEZ_VERSION_MAJOR
#define VERSION_LOW                         UEZ_VERSION_MINOR
#define VERSION_DATE                        __DATE__ " " __TIME__

/*-------------------------------------------------------------------------*
 * Compile options:
 *-------------------------------------------------------------------------*/
#define PLATFORM                            FDI_UEZ_GUI

#define SERIAL_PORTS_DEFAULT_BAUD           115200

#define UEZ_ENABLE_LCD                      1

#include <Include/Device/LCD/Config_LCD.h>

// Components to allow to run
#ifndef UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_TCPIP_STACK              1
#endif

// Choose one when TCP/IP stack is enabled
#ifndef UEZ_BASIC_WEB_SERVER
#define UEZ_BASIC_WEB_SERVER                0
#endif
#ifndef UEZ_HTTP_SERVER
#define UEZ_HTTP_SERVER                     0
#endif
#ifndef DEBUG_HTTP_SERVER
#define DEBUG_HTTP_SERVER                   0   // When enabled prints information to console.
#endif

#ifndef UEZ_ENABLE_USB_HOST_STACK
#define UEZ_ENABLE_USB_HOST_STACK           1
#endif

#ifndef UEZ_ENABLE_USB_DEVICE_STACK
#define UEZ_ENABLE_USB_DEVICE_STACK         1
#endif
#ifndef COMPILE_OPTION_USB_SDCARD_DISK
#define COMPILE_OPTION_USB_SDCARD_DISK      UEZ_ENABLE_USB_DEVICE_STACK
#endif

#ifndef UEZ_ENABLE_USB_DEVICE
#define UEZ_ENABLE_USB_DEVICE               UEZ_ENABLE_USB_DEVICE_STACK
#endif

#ifndef UEZ_ENABLE_USB_HOST
#define UEZ_ENABLE_USB_HOST                 UEZ_ENABLE_USB_HOST_STACK
#endif

#define UEZ_GUI_PLATFORM_USE_SPI0_OVER_SSP  1

#if (RTOS == FreeRTOS)
#define UEZ_TASK_STACK_BYTES(x)             (x/sizeof(long))
#endif

#ifndef APP_MENU_ALLOW_TEST_MODE
#define APP_MENU_ALLOW_TEST_MODE            1
#endif

#ifndef ACCEL_DEMO_ALLOW_ROTATE
#define ACCEL_DEMO_ALLOW_ROTATE             0
#endif
/*-------------------------------------------------------------------------*
 * Default Values:
 *-------------------------------------------------------------------------*/
/* MAC address definition.  The MAC address must be unique on the network. */
#ifndef ALTERNATE_NETORK_SETTING
#define emacETHADDR0 0
#define emacETHADDR1 0xbd
#define emacETHADDR2 0x33
#define emacETHADDR3 0x06
#define emacETHADDR4 0x68
#define emacETHADDR5 0x22

/* The IP address being used. */
#define emacIPADDR0 192
#define emacIPADDR1 168
#define emacIPADDR2 10
#define emacIPADDR3 20

/* The gateway address being used. */
#define emacGATEWAY_ADDR0 192
#define emacGATEWAY_ADDR1 168
#define emacGATEWAY_ADDR2 10
#define emacGATEWAY_ADDR3 1

/* The network mask being used. */
#define emacNET_MASK0 255
#define emacNET_MASK1 255
#define emacNET_MASK2 255
#define emacNET_MASK3 0

/* Ethernet MAC address to use (default). */
#define emacMACADDR0    0x00
#define emacMACADDR1    0xBD
#define emacMACADDR2    0x33
#define emacMACADDR3    0x06
#define emacMACADDR4    0x68
#define emacMACADDR5    0x22
#endif

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#define APP_DEMO_DEFAULT_FONT   font_winfreesys14x16
#else
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#define APP_DEMO_DEFAULT_FONT   font_helvr10
#endif

#define MAIN_TASK_STACK_SIZE        UEZ_TASK_STACK_BYTES(4096)

#define FAST_STARTUP                0

#define APP_ENABLE_HEARTBEAT_LED_ON 1

#define DISPLAY_WIDTH               UEZ_LCD_DISPLAY_WIDTH
#define DISPLAY_HEIGHT              UEZ_LCD_DISPLAY_HEIGHT

#define FRAME_SIZE              (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(T_pixelColor))

#define LCD_FRAMES_START        ((TUInt8 *)LCD_DISPLAY_BASE_ADDRESS)
#define LCD_FRAMES_END          ((TUInt8 *)LCD_DISPLAY_BASE_ADDRESS + 0x5DCFFF)
#define FRAME(n)                ((char *)(LCD_FRAME_BUFFER+FRAME_SIZE*(n)))

#if (COMPILER_TYPE==RowleyARM)
#if __ASSEMBLER__
#else
extern unsigned char __loadspace_start__;
extern unsigned char __loadspace_end__;
extern unsigned char __frames_start__;
extern unsigned char __frames_end__;
extern unsigned char __demoframe_start__;
extern unsigned char __demoframe_end__;
#endif

#define LCD_FRAME_BUFFER        ((unsigned char *)&__frames_start__)
#define FRAMES_MEMORY           ((&__frames_end__)-(&__frames_start__))
#define MAX_NUM_FRAMES          (FRAMES_MEMORY / FRAME_SIZE)
#define DEMO_RESERVE_FRAME      ((unsigned char *)&__demoframe_start__)

#else
	
#define LCD_FRAME_BUFFER        LCD_FRAMES_START
#define FRAMES_MEMORY           LCD_FRAMES_SIZE
#define LCD_FRAMES_SIZE         (LCD_FRAMES_END-LCD_FRAME_BUFFER)
#endif

#ifndef SLIDESHOW_PREFETCH_AHEAD
#define SLIDESHOW_PREFETCH_AHEAD 5
#endif
#ifndef SLIDESHOW_PREFETCH_BEHIND
#define SLIDESHOW_PREFETCH_BEHIND 1
#endif
#ifndef SLIDESHOW_NUM_CACHED_SLIDES
#define SLIDESHOW_NUM_CACHED_SLIDES 5
#endif

#endif // _CONFIG_APP_H_
/*-------------------------------------------------------------------------*
 * End of File:  Config_App.h
 *-------------------------------------------------------------------------*/