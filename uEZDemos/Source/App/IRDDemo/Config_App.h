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

#define PROJECT_NAME                        "uEZ IRD Demo"
#define VERSION_AS_TEXT                     UEZ_VERSION_STRING
#define VERSION_HIGH                        UEZ_VERSION_MAJOR
#define VERSION_LOW                         UEZ_VERSION_MINOR

/*-------------------------------------------------------------------------*
 * Compile options:
 *-------------------------------------------------------------------------*/
#define SERIAL_PORTS_DEFAULT_BAUD           115200

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

#endif // _CONFIG_APP_H_
/*-------------------------------------------------------------------------*
 * End of File:  Config_App.h
 *-------------------------------------------------------------------------*/
