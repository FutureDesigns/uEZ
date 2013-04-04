/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name :
 Module name  :
 File Name    : network_config.h

 File Description:


 Author :

 Rev History:
 Ver   By               date        Description
 ---------------------------------------------------------
 1.1   Redpine Signals
 ---------------------------------------------------------
 */

#ifndef _NETWORK_CONFIG_H_
#define _NETWORK_CONFIG_H_

#include "includes.h"
#include "rsi_util.h"

#define   AUTO_CONNECT_OPEN_AP	ENABLE				/* ENABLE / DISABLE */

#define   SCAN_CHANNEL			0					/* 0 -> Auto */
#define   IBSS_CHANNEL			6					/* Channel in which the IBSS Network is created */
#define   SCAN_SSID				NULL				/* SSID OF AP if hidden mode is enabled
													   else NULL */
#define   CONNECT_SSID			"Wi-Fi_Redpine"		/* SSID of the Access Point to connect *///#define   PSK					"12345678"			/* Pre-Shared key */
#define   PSK					"12345678"			/* Pre-Shared key */
#define   TX_RATE				0					/* 0 -> Auto Rate */
#define   TX_POWER				HIGH				/* LOW / MEDIUM / HIGH */
#define   NETWORKTYPE			INFRASTRUCTURE		/* IBSS / INFRASTRUCTURE */
#define   IBSS_MODE				CREATE				/* CREATE / JOIN */
#define   DHCP_MODE				ENABLE				/* ENABLE / DISABLE */
#define   IP_ADDRESS			{192, 168, 1, 40 }	/* Static IP Address of the device */
#define   SUBNET_MASK			{255, 255, 255, 0}	/* SUBNET MASK of the device */
#define   GATEWAY_ADDRESS		{192, 168, 1, 1}	/* GATEWAY ADDRESS of the device */

#define   POWERSAVE_MODE		POWER_MODE0			/* POWER_MODE0/POWER_MODE1/POWER_MODE2 */

/* Define any one of the following MACROS or define NONE of them */

//#define EVAL_LICENCE		/* This macro has to be defined if the standard RX Compiler's evaluation period has expired */
//#define FORCE_FW_UPGRADE	/* This macro has to be defined if a forced Firmware Upgrade of the WLAN module is required */

#endif
