/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name :
 Module name  :
 File Name    : rsi_api.h

 File Description:


 Author :

 Rev History:
 Ver   By               date        Description
 ---------------------------------------------------------
 1.1   Redpine Signals
 ---------------------------------------------------------
 */
#ifndef __RSI_API_H__
#define __RSI_API_H__

#include "rsi_nic.h"
#include "includes.h"

/***************************START_FUNCTION_PROTOTYPES****************************************/

int8_t rsi_send_ssp_init_cmd();
uint32_t rsi_send_ssp_read_intrStatus_cmd();
uint32_t rsi_send_ssp_intrAck_cmd(uint32_t val);
#if BOOT_LOAD
int32_t rsi_load_firmware(uint16_t type);
#endif
int32_t rsi_card_read(
        uint8_t *frame_desc,
        uint8_t ***ptr,
        uint32_t *pktLen,
        uint32_t *queueno);
uint16_t rsi_send_join_request(struct WiFi_CNFG_Join_t *join_params);
int32_t rsi_send_init_request();
uint16_t rsi_send_scan_request(uint32_t channel, uint8_t *SSID);
int32_t rsi_send_TCP_config_request(
        uint8_t uUseDHCP,
        uint8_t uIPaddr[],
        uint8_t uNetMask[],
        uint8_t uDefaultGateway[]);
int32_t rsi_get_proto_type(uint16_t sd);

int32_t rsi_socket_create(
        SOCKET_TYPE uSocketType,
        uint16_t stLocalPort,
        stRemoteAdress_t *stRemoteAddress);
uint16_t rsi_socket_close(uint16_t uSocketDescriptor);
uint16_t rsi_socket_send(
        uint16_t uSocketDescriptor,
        uint32_t uBufferLength,
        uint8_t *pBuffer);

uint16_t rsi_Send_Rssi_Req(void);
uint16_t rsi_Send_Wlan_Conn_Status_Req(void);

void rsi_spi_MasterInit();

uint32_t rsi_ssp_write_mask_register(uint32_t val); //added for power save
uint16_t rsi_send_power_save_request(uint8_t power_mode);
int16_t rsi_upgrade_firmware(void);
#endif
