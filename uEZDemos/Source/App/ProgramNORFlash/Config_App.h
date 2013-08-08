/*-------------------------------------------------------------------------*
 * File:  Config_App.h
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(tm)+ Bootloader - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ+ Bootloader distribution.
 *
 * uEZ(tm)+ Bootloader is commercial software licensed under the
 * Software End-User License Agreement (EULA) delivered with this source
 * code package.  The Software and Documentation contain material that is
 * protected by United States Copyright Law and trade secret law, and by
 * international treaty provisions. All rights not granted to Licensee
 * herein are expressly reserved by FDI. Licensee may not remove any
 * proprietary notice of FDI from any copy of the Software or
 * Documentation.
 *
 * For more details, please review your EULA agreement included with this
 * software package.
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

#ifndef PROJECT_NAME
#define PROJECT_NAME                        "uEZ NOR Flash Loader"
#define VERSION_AS_TEXT                     "v1.00"
#define VERSION_HIGH                        1
#define VERSION_LOW                         0
#define VERSION_DATE                        __DATE__ " " __TIME__
#endif

/*-------------------------------------------------------------------------*
 * Compile options:
 *-------------------------------------------------------------------------*/
#define PLATFORM                            FDI_UEZ_GUI

#define SERIAL_PORTS_DEFAULT_BAUD           115200

#define UEZ_ENABLE_LCD                      1
#include <Include\Device\LCD\Config_LCD.h>

// Components to allow to run
#ifndef UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_TCPIP_STACK              0
#endif
// Choose one when stack is enabled
#define UEZ_HTTP_SERVER                     0
#define DEBUG_HTTP_SERVER                   0
#define UEZ_BASIC_WEB_SERVER                0

#ifndef UEZ_ENABLE_USB_HOST_STACK
#define UEZ_ENABLE_USB_HOST_STACK           0
#endif 

/*-------------------------------------------------------------------------*
 * Default Values:
 *-------------------------------------------------------------------------*/
#define APP_DEMO_DEFAULT_FONT   font_winfreesys14x16_subset

#define APP_ENABLE_HEARTBEAT_LED_ON 1

#define DISPLAY_WIDTH       UEZ_LCD_DISPLAY_WIDTH
#define DISPLAY_HEIGHT      UEZ_LCD_DISPLAY_HEIGHT

#define FRAME_SIZE              (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(T_pixelColor))

#ifndef BOOTLOADER_INSTALL_FROM_SDCARD
#define BOOTLOADER_INSTALL_FROM_SDCARD    0
#endif

#ifndef BOOTLOADER_INSTALL_FROM_USBFLASH
#define BOOTLOADER_INSTALL_FROM_USBFLASH  1
#endif

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

#elif (COMPILER_TYPE == IAR)
    extern int __load_space__;
    extern int __ICFEDIT_region_FRAME_start__;
    extern int __ICFEDIT_size_FRAM__;
    #define LCD_FRAMES_START    ((unsigned char *) &__ICFEDIT_region_FRAME_start__)
    #define LCD_FRAMES_END      ((unsigned char *) __ICFEDIT_size_FRAM__)
    #define LCD_FRAMES_SIZE     (LCD_FRAMES_END-LCD_FRAME_BUFFER)
    #define LOAD_SPACE          ((unsigned char *)&__load_space__)
    #define LCD_FRAME_BUFFER    LCD_FRAMES_START
    #define FRAME_SIZE          (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(T_pixelColor))
    #define FRAMES_MEMORY       LCD_FRAMES_SIZE
    #define FRAME(n)            ((char *)(LCD_FRAME_BUFFER+FRAME_SIZE*(n)))

#else
    #define LCD_FRAMES_START    ((TUInt8 *)0xA0000000)
    #define LCD_FRAMES_END      ((TUInt8 *)0xA05DCFFF)
    #define LCD_FRAMES_SIZE     (LCD_FRAMES_END-LCD_FRAME_BUFFER)
    // Use hard code location if we don't know what compiler is used
    #define LOAD_SPACE ((unsigned char *)0xA05DD000)
    #define LCD_FRAME_BUFFER        LCD_FRAMES_START
    #define FRAME_SIZE              (DISPLAY_WIDTH*DISPLAY_HEIGHT*sizeof(T_pixelColor))
    #define FRAMES_MEMORY           LCD_FRAMES_SIZE
    #define FRAME(n)                ((char *)(LCD_FRAME_BUFFER+FRAME_SIZE*(n)))
#endif


#define MAIN_TASK_STACK_SIZE          UEZ_TASK_STACK_BYTES(2048)

#endif // _CONFIG_APP_H_
/*-------------------------------------------------------------------------*
 * End of File:  Config_App.h
 *-------------------------------------------------------------------------*/
