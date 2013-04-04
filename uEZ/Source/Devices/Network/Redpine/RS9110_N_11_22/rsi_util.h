/*HEADER**********************************************************************
Copyright (c)
All rights reserved
This software embodies materials and concepts that are confidential to Redpine
Signals and is made available solely pursuant to the terms of a written license
agreement with Redpine Signals

Project name : 
Module name  :
File Name    : rsi_util.h

File Description:
    

Author :

Rev History:
Ver   By               date        Description
---------------------------------------------------------
1.1   Redpine Signals
---------------------------------------------------------
*/

#ifndef _RSI_UTIL_H
#define _RSI_UTIL_H

#include "includes.h"
#include "rsi_nic.h"
#include "network_config.h"

#define RSSI_REQ            4

#define SSID_LIST 			11
#define THROUGHPUT 			12
#define POWER_CONSUMPTION 	13
#define RSSI  				14
#define CONFIG_THRPUT		15	
#define WIFI_CONFIG_ACK		16

#define SEPARATOR			','	
 
#define rsi_memcpy(a,b,c)            memcpy(a,b,c)
#define rsi_memset(a,b,c)            memset(a,b,c)
#define rsi_strcpy(a,b)              strcpy(a,b)
//#define rsi_strcat(a,b)              strcat(a,b)
#define rsi_strncpy(a,b,c)           strncpy(a,b,c)
#define rsi_equal_string(a,b)        strcmp(a,b)


void rsi_get_ssid_list(uint8_t *scan_resp);
uint16_t rsi_prepare_pkt_to_send(uint16_t type, uint16_t val,  uint8_t *buff);
uint16_t rsi_strlen(uint8_t *str);
int16_t rsi_memcmp(uint8_t *src_buf, uint8_t *dst_buf, uint16_t len);
void str_to_ip(uint8_t *dest, uint8_t *src, uint8_t src_len);
void rsi_initialize_netwrok_config1(networkConfig_t *networkConfig);
void rsi_initialize_ipconfig_parmas(ipconfig_t *ipConfig);
uint16_t rsi_convert_ip_to_string(uint8_t *num_buff, uint8_t *ip_buff);
uint8_t rsi_strrev(void *src, void *dst, uint8_t len);
void rsi_strcat(void *src, void *dst, uint8_t len);

/* This macro should not be changed */
#define   FWVERSION             "4.0.3"

#endif
