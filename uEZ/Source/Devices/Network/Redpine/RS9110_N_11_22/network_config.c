/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name :
 Module name  :
 File Name    : network_config.c

 File Description:
 This file contains the ipconfig, socket creation data specific code .

 List of functions:
 rsi_initialize_netwrok_config1
 rsi_initialize_ipconfig_parmas


 Author :

 Rev History:
 Ver   By               date        Description
 ---------------------------------------------------------
 1.1   Redpine Signals
 ---------------------------------------------------------
 */

#include "includes.h"
#include "rsi_util.h"
#include "rsi_api.h"
#include "rsi_nic.h"
#include "network_config.h"

uint8_t device_ip[4] = IP_ADDRESS;
uint8_t subnet_mask[4] = SUBNET_MASK;
uint8_t gateway[4] = GATEWAY_ADDRESS;
uint8_t remote_ip[4] = { 192, 168, 1, 191 };

/*FUNCTION*********************************************************************
 Function Name  : rsi_initialize_netwrok_config1
 Description    : This function iniializes the socket information
 Returned Value : NONE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 networkConfig    |     |  X  |     |  Pointer to the networkConfig_t structure

 *END****************************************************************************/

void rsi_initialize_netwrok_config1(networkConfig_t *networkConfig)
{
    /* socket type */
    networkConfig->uSocketType = SOCKET_LTCP;
    /* Local port number */
    networkConfig->stLocalPort = 800;
    /* Remote port number */
    networkConfig->remote_address.sinPort = 2054;
    /* Remote ip address */
    rsi_memcpy(networkConfig->remote_address.ip, remote_ip, 4);
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_initialize_ipconfig_parmas
 Description    : This function initializes the ipconfig information
 Returned Value :  NONE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 ipConfig         |  X  |     |     | Pointer to the ipconfig_t structure
 *END****************************************************************************/

void rsi_initialize_ipconfig_parmas(ipconfig_t *ipConfig)
{

    if (NETWORKTYPE != IBSS)
        ipConfig->DHCP = DHCP_MODE;
    else
        ipConfig->DHCP = DISABLE;
    if (ipConfig->DHCP == DISABLE) {
        /* ip address to be configured */
        rsi_memcpy(ipConfig->uIPaddr, device_ip, 4);
        /* subnet mask to be configured */
        rsi_memcpy(ipConfig->uSubnet, subnet_mask, 4);
        /* Default gateway to be configured */
        rsi_memcpy(ipConfig->uDefaultGateway, gateway, 4);
    }

}

