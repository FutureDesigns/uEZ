/*-------------------------------------------------------------------------*
 * File:  Config_App.h
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
#ifndef _CONFIG_APP_H_
#define _CONFIG_APP_H_

#define PROJECT_NAME                        "uEZ Demo for DKTSKits"
#define VERSION_AS_TEXT                     UEZ_VERSION_STRING
#define VERSION_HIGH                        UEZ_VERSION_MAJOR
#define VERSION_LOW                         UEZ_VERSION_MINOR

/*-------------------------------------------------------------------------*
 * Compile options:
 *-------------------------------------------------------------------------*/
#define SERIAL_PORTS_DEFAULT_BAUD           115200

#define UEZ_ENABLE_LCD                      1
#include <Include\Device\LCD\Config_LCD.h>

// Components to allow to run
#ifndef UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_TCPIP_STACK              1
#endif

// Choose one when stack is enabled
#ifndef UEZ_BASIC_WEB_SERVER
#define UEZ_BASIC_WEB_SERVER                0//UEZ_ENABLE_TCPIP_STACK
#endif

#ifndef UEZ_HTTP_SERVER
#define UEZ_HTTP_SERVER                     1
#endif

#ifndef DEBUG_HTTP_SERVER
#define DEBUG_HTTP_SERVER                   0
#endif

#ifndef UEZ_ENABLE_USB_HOST_STACK
#define UEZ_ENABLE_USB_HOST_STACK           1
#endif

#ifndef UEZ_ENABLE_USB_DEVICE_STACK
#define UEZ_ENABLE_USB_DEVICE_STACK         1
#endif
#define COMPILE_OPTION_USB_SDCARD_DISK      UEZ_ENABLE_USB_DEVICE_STACK

#define UEZ_ENABLE_USB_HOST                 UEZ_ENABLE_USB_HOST_STACK
#define UEZ_ENABLE_USB_DEVICE               UEZ_ENABLE_USB_DEVICE_STACK

#define UEZ_TASK_STACK_BYTES(x)             (x/sizeof(long))

#define MAIN_TASK_STACK_SIZE                UEZ_TASK_STACK_BYTES(4096)
#define BASICWEB_SERVER_STACK_SIZE          UEZ_TASK_STACK_BYTES(1400)
#define WEBSERVER_PRIORITY                  UEZ_PRIORITY_NORMAL

/*-------------------------------------------------------------------------*
 * Default Values:
 *-------------------------------------------------------------------------*/
/* MAC address definition.  The MAC address must be unique on the network. */
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

#if UEZ_ENABLE_LCD
  #if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
#include <Source/Library/Graphics/SWIM/lpc_winfreesystem14x16.h>
    #define APP_DEMO_DEFAULT_FONT   font_winfreesys14x16
  #else
#include <Source/Library/Graphics/SWIM/lpc_helvr10.h>
    #define APP_DEMO_DEFAULT_FONT   font_helvr10
  #endif
#endif

#define ConsoleIsExpansionBoardConnected()   	1
#define ConsoleIsPoeExpansionBoardConnected()   1

/*-------------------------------------------------------------------------*
 * Frame buffer definitions
 *-------------------------------------------------------------------------*/
#define DISPLAY_WIDTH       UEZ_LCD_DISPLAY_WIDTH
#define DISPLAY_HEIGHT      UEZ_LCD_DISPLAY_HEIGHT

#define FRAME_SIZE              (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(T_pixelColor))
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
    #define FRAME_SIZE              (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(T_pixelColor))
    #define FRAMES_MEMORY           ((&__frames_end__)-(&__frames_start__))
    #define MAX_NUM_FRAMES          (FRAMES_MEMORY / FRAME_SIZE)
    #define DEMO_RESERVE_FRAME      ((unsigned char *)&__demoframe_start__)
    #define FRAME(n)                ((char *)(LCD_FRAME_BUFFER+FRAME_SIZE*(n)))

    #define LOAD_SPACE              ((unsigned char *)&__loadspace_start__)

#else
    #define LCD_FRAMES_START    ((TUInt8 *)0xA0000000)
    #define LCD_FRAMES_END      ((TUInt8 *)0xA05DCFFF)
    #define LCD_FRAMES_SIZE     (LCD_FRAMES_END-LCD_FRAME_BUFFER)
    // Use hard code location if we don't know what compiler is used
    #define LOAD_SPACE ((unsigned char *)0xA05dd000)

    #define LCD_FRAME_BUFFER        LCD_FRAMES_START
    #define FRAME_SIZE              (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(T_pixelColor))
    #define FRAMES_MEMORY           LCD_FRAMES_SIZE
    #define FRAME(n)                ((char *)(LCD_FRAME_BUFFER+FRAME_SIZE*(n)))
#endif

#define APP_ENABLE_HEARTBEAT_LED_ON       1

#if (DISPLAY_WIDTH > 480)
#define SIMPLEUI_DOUBLE_SIZED_ICONS 1 // 1 for double sized icons, 0 for single sized
#else
#define SIMPLEUI_DOUBLE_SIZED_ICONS 0 // 1 for double sized icons, 0 for single sized
#endif

#endif // _CONFIG_APP_H_
/*-------------------------------------------------------------------------*
 * End of File:  Config_App.h
 *-------------------------------------------------------------------------*/
