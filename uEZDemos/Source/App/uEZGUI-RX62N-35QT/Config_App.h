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

#define PROJECT_NAME                        "uEZGUI RX62N Demo"
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

  #include <Include\Device\LCD\Config_LCD.h>

// Components to allow to run
#ifndef UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_TCPIP_STACK              1
#endif

// Choose one when stack is enabled
#ifndef UEZ_HTTP_SERVER
#define UEZ_HTTP_SERVER                     0
#endif
#ifndef DEBUG_HTTP_SERVER
#define DEBUG_HTTP_SERVER                   0
#endif
#ifndef UEZ_BASIC_WEB_SERVER
#define UEZ_BASIC_WEB_SERVER                0
#endif

#ifndef UEZ_ENABLE_USB_HOST_STACK
#define UEZ_ENABLE_USB_HOST_STACK           1
#endif

#ifndef UEZ_ENABLE_USB_DEVICE_STACK
#define UEZ_ENABLE_USB_DEVICE_STACK         1
#endif
#define COMPILE_OPTION_USB_SDCARD_DISK      UEZ_ENABLE_USB_DEVICE_STACK

#ifndef UEZ_ENABLE_USB_DEVICE
#define UEZ_ENABLE_USB_DEVICE                 UEZ_ENABLE_USB_DEVICE_STACK
#endif

#ifndef UEZ_ENABLE_USB_HOST
#define UEZ_ENABLE_USB_HOST                   UEZ_ENABLE_USB_HOST_STACK
#endif

#define UEZ_GUI_PLATFORM_USE_SPI0_OVER_SSP    1

#define UEZ_TASK_STACK_BYTES(x)             (x/sizeof(long))

#define APP_MENU_ALLOW_TEST_MODE            1
#define ACCEL_DEMO_ALLOW_ROTATE             0

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

#if (UEZ_DEFAULT_LCD_RES_VGA || UEZ_DEFAULT_LCD_RES_WVGA)
    #define APP_DEMO_DEFAULT_FONT   font_winfreesys14x16
#else
    #define APP_DEMO_DEFAULT_FONT   font_helvr10
#endif

#define MAIN_TASK_STACK_SIZE                UEZ_TASK_STACK_BYTES(8192)

#define FAsT_STARTUP        0

#define DISPLAY_WIDTH       UEZ_LCD_DISPLAY_WIDTH
#define DISPLAY_HEIGHT      UEZ_LCD_DISPLAY_HEIGHT

#define FRAME_SIZE              (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(T_pixelColor))
#define LCD_FRAME_BUFFER    	0x08000000
#define FRAME_SIZE              (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(T_pixelColor))
#define FRAMES_MEMORY           (FRAME_SIZE*5)
#define MAX_NUM_FRAMES          (FRAMES_MEMORY / FRAME_SIZE)
#define DEMO_RESERVE_FRAME      ((unsigned char *)(LCD_FRAME_BUFFER+FRAME_SIZE*5))
#define FRAME(n)                ((char *)(LCD_FRAME_BUFFER+FRAME_SIZE*(n)))

#define LOAD_SPACE              ((unsigned char *)(LCD_FRAME_BUFFER+FRAME_SIZE*6))

#define SLIDESHOW_PREFETCH_AHEAD 10
#define SLIDESHOW_PREFETCH_BEHIND 1
#define SLIDESHOW_NUM_CACHED_SLIDES 10

// Modify the default accelerometer demo settings
#define ACCEL_DEMO_SWAP_XY        1
#define ACCEL_DEMO_FLIP_X         1 // MMA setting
#define ACCEL_DEMO_FLIP_Y         0 // MMA setting
#define ACCEL_DEMO_G_TO_PIXELS    32
#define ACCEL_DEMO_ALLOW_ROTATE   0

#define UEZ_DEFAULT_AUDIO_LEVEL  60

#define CONSOLE_ROOT_DIRECTORY      "1:/"

#define UEZ_ICONS_SET ICONS_SET_PROFESSIONAL_ICONS


#endif // _CONFIG_APP_H_
/*-------------------------------------------------------------------------*
 * End of File:  Config_App.h
 *-------------------------------------------------------------------------*/
