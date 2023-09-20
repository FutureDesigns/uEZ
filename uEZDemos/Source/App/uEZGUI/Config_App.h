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

//#include <Source/Library/Graphics/SWIM/lpc_swim.h> // To get just the T_pixelColor def.

// To get both T_pixelColor and RGB def must include the SimpleUI_Types here.
// We need to get pixel color type size here from simpleui types, but not in assembler.
#if (COMPILER_TYPE == IAR)
#ifdef __ICCARM__  
     //Ensure the #include is only used by the compiler, and not the assembler.
     #include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>
#endif
#else 
   #include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>
#endif

#include <uEZPlatform.h> // To get SDRAM size, but currently we don't use that here.

#if ((UEZ_DEFAULT_LCD_RES_VGA == 1) || (UEZ_DEFAULT_LCD_RES_WVGA == 1))
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
#define APP_DEMO_DEFAULT_FONT   font_winfreesys14x16
#define APP_DEMO_DEFAULT_FONT_W 14 // largest char width in font
#define APP_DEMO_DEFAULT_FONT_H 16
#else
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
#define APP_DEMO_DEFAULT_FONT   font_helvr10
#define APP_DEMO_DEFAULT_FONT_W 11
#define APP_DEMO_DEFAULT_FONT_H 10
#endif

#if(UEZ_PROCESSOR == NXP_LPC4357)
#define MAIN_TASK_STACK_SIZE        UEZ_TASK_STACK_BYTES(6*1024)
#else
#define MAIN_TASK_STACK_SIZE        UEZ_TASK_STACK_BYTES(5*1024)
#endif

#define FAST_STARTUP                0

#define APP_ENABLE_HEARTBEAT_LED_ON 1

/*-------------------------------------------------------------------------*
 * Display and frames section:
 *-------------------------------------------------------------------------*/
#define MAX_NUM_FRAMES          5 // for largest screen uezgui frames memory = (800*480*2*5)/1024/1024 = 3.662MB
// It is important not to lower this or slideshow will break.
// Slideshow memory can't use the first 3 frames partially as they are used by the SWIM GUI.
// We set 5 frames, but slideshow needs an extra 3*UEZ_LCD_DISPLAY_WIDTH to load a picture, so only cache up to 3 max.

#define DISPLAY_WIDTH           UEZ_LCD_DISPLAY_WIDTH
#define DISPLAY_HEIGHT          UEZ_LCD_DISPLAY_HEIGHT
#define FRAME_SIZE              (UEZ_LCD_DISPLAY_WIDTH*UEZ_LCD_DISPLAY_HEIGHT*sizeof(T_pixelColor))
#define LCD_FRAMES_START        ((TUInt8 *)LCD_DISPLAY_BASE_ADDRESS)
#define LCD_FRAMES_END          (((TUInt8 *)LCD_FRAMES_START + (FRAME_SIZE*MAX_NUM_FRAMES))-1)
#define LCD_FRAMES_SIZE         (FRAME_SIZE*MAX_NUM_FRAMES+(UEZ_LCD_DISPLAY_WIDTH*3))
#define FRAME(n)                ((TUInt8 *)(LCD_FRAMES_START + (FRAME_SIZE*(n))))
#define LCD_FRAME_BUFFER        LCD_FRAMES_START

// emWin settings section (for these old demo projects, this RAM will be allocated even with emWin demos turned off)
// TODO: Note define this size close to the actual amount needed by the application.
// It appears that about 100 KB for GUI elements 
// (depending on the GUI design and num GUI widgets total)
// plus 2x LCD buffers should be a sufficient minimum.
// So 1 MB should be plenty for typical GUI on 480x272x2 bytes per pixel size
// Then 2 MB would be good for 800x480x2 bytes per pixel size
// See https://www.segger.com/products/user-interface/emwin/technology/emwin-resource-usage/
// 16+16+14+32+20+32+16+52+44+56+32+48+12+28+52+52+40+16+(17+38+7+5)*1024+80+20
// +2*800*480*2 = 1,605,286 bytes
//#define EMWIN_RAM_SIZE          0x00200000
#define EMWIN_RAM_SIZE          (FRAME_SIZE*2)+(1024*1536) // 2 frames plus 1.5MB
// Note video player adds 1 more frame memory

// Network Settings for future implementation
#define NETWORK_STACK_RAM_SIZE  4 // This is the number for the section allocation
// TODO Implement per application configurable settings for the primary network stack (LWIP2)
// Will be allocated with section placement into SDRAM, so that you can see total RAM usage.
/* Placeholder defines for now as an example
#define MAX_NUM_SOCKETS          20
#define MAX_NUM_PACKET_BUFS      20
#define PACKET_BUFFER_SIZE       1536
#define SOCKET_BUFFER_SIZE       2000
#define SOCKET_BUFFER_START      ((TUInt8 *)SOCKET_BUFFER_BASE_ADDRESS)
#define SOCKET_BUFFERS_SIZE      (SOCKET_BUFFER_SIZE*MAX_NUM_SOCKETs)
#define SOCKET(n)                ((char *)(SOCKET_BUFFER_START+SOCKET_BUFFER_SIZE*(n)))
*/

#ifndef SLIDESHOW_NUM_CACHED_SLIDES
#define SLIDESHOW_NUM_CACHED_SLIDES (MAX_NUM_FRAMES-3)
#endif   
#ifndef SLIDESHOW_PREFETCH_AHEAD
#define SLIDESHOW_PREFETCH_AHEAD (SLIDESHOW_NUM_CACHED_SLIDES-1)
#endif
#ifndef SLIDESHOW_PREFETCH_BEHIND
#define SLIDESHOW_PREFETCH_BEHIND 1
#endif



//#define DEBUG   1 // Uncomment to enable debug printf
/* 
#ifdef DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif*/

#endif // _CONFIG_APP_H_
/*-------------------------------------------------------------------------*
 * End of File:  Config_App.h
 *-------------------------------------------------------------------------*/
