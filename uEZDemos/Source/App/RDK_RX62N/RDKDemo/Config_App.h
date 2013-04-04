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

#define PROJECT_NAME                        "YRDKRX62N Demo"
#define VERSION_AS_TEXT                     UEZ_VERSION_STRING
#define VERSION_HIGH                        UEZ_VERSION_MAJOR
#define VERSION_LOW                         UEZ_VERSION_MINOR
#define VERSION_DATE                        __DATE__ " " __TIME__

/*-------------------------------------------------------------------------*
 * Compile options:
 *-------------------------------------------------------------------------*/
#define PLATFORM                            FDI_UEZ_GUI

#define SERIAL_PORTS_DEFAULT_BAUD           115200

#define UEZ_ENABLE_LCD                      0

// Components to allow to run
#ifndef UEZ_ENABLE_TCPIP_STACK
#define UEZ_ENABLE_TCPIP_STACK              1
#endif

// Choose one when stack is enabled
#define UEZ_HTTP_SERVER                     0
#define DEBUG_HTTP_SERVER                   0
#define UEZ_BASIC_WEB_SERVER                UEZ_ENABLE_TCPIP_STACK

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

#define MAIN_TASK_STACK_SIZE                UEZ_TASK_STACK_BYTES(8192)

#define FAsT_STARTUP        0


#endif // _CONFIG_APP_H_
/*-------------------------------------------------------------------------*
 * End of File:  Config_App.h
 *-------------------------------------------------------------------------*/
