/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name :
 Module name  :
 File Name    : rsi_spi.h

 File Description:


 Author :

 Rev History:
 Ver   By               date        Description
 ---------------------------------------------------------
 1.1   Redpine Signals
 ---------------------------------------------------------
 */
#ifndef __RSI_SPI_H__
#define __RSI_SPI_H__

#include "includes.h"
#include "rsi_nic.h"

int16_t rsi_create_user_socket(networkConfig_t *networkConfig);

int16_t rsi_send_ipconfig_request(ipconfig_t *ipConfig);

int16_t rsi_close_user_socket(networkConfig_t *networkConfig);

int16_t rsi_send_data(uint32_t sockDesc, uint32_t len, uint8_t *data);

int16_t rsi_poweron_device(void);

int16_t rsi_init_wlan_device(void);

void rsi_external_inerrupt_handler(void);

int16_t rsi_send_rssi_query_req(void);

int16_t rsi_wlan_conn_query_req(void);

int32_t get_sock_status(uint16_t sd);

int32_t identify_table_entry(uint16_t uPort, uint8_t *aIP);

int32_t rsi_spi_init(void);

int32_t rsi_decision_for_image_upgrade(void);

int32_t rsi_read_fwversion(void);

int32_t get_free_table_entry();

int32_t rsi_socket_deregister(uint32_t appid);

int32_t sock_cb0(int32_t sockd, uint16_t sourcePort);

int32_t sock_cb1(int32_t sockd, uint16_t sourcePort);

uint32_t accept_cb(
        int32_t sd,
        int32_t nsd,
        uint16_t destinationPort,
        uint8_t *destinationIpAddress);

uint32_t close_cb(int32_t socketId);

uint32_t recv_cb0(uint8_t *buff, uint32_t BytesReceived, uint16_t sockHandle);

uint32_t recv_cb1(uint8_t *buff, uint32_t BytesReceived, uint16_t sockHandle);

uint32_t remote_close_cb(int32_t socketId);

uint32_t rsi_socket_register(
        uint32_t *appId,
        uint16_t uPort,
        uint32_t uIP,
        Callbacks_P pCallbacks);

uint32_t rsi_socket_register(
        uint32_t *appId,
        uint16_t uPort,
        uint32_t uIP,
        Callbacks_P pCallbacks);

int16_t Read_tcp_ip_event(void);

void rsi_get_ssid_list(uint8_t *scan_resp);

void rsi_init_socket_data_base(void);

void rsi_configure_spi_power_pins(void);

int32_t rsi_interrupt_handler(uint8_t *DataRcvPacket, uint32_t *size);

int32_t rsi_rcv_pkt(uint8_t **DataRcvPacket, uint32_t *size);

uint16_t rsi_send_sleep_request(void);
uint16_t rsi_disconnect_wlan_conn(void);

void rs22_ssp_send_and_receive(
        uint8_t *src_ptr,
        uint8_t *dst_ptr,
        uint32_t count);
int32_t rs22_ssp_receive_start_token(void);
void rs22_ssp_send_and_receive_junk(uint8_t *src_ptr, uint32_t count);
int32_t rs22_ssp_bus_request_synch(struct bus_cmd_s *cmd);
int32_t get_table_entry(uint16_t sd);
int32_t rs22_ssp_bus_request_synch(struct bus_cmd_s *cmd);
void rs22_ssp_send_junk_and_receive(uint8_t *dst_ptr, uint32_t count);

/***************************END_FUNCTION_PROTOTYPES****************************************/

#endif
