/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name :
 Module name  :
 File Name    : rsi_spi.c

 File Description:

 List of functions:

 rsi_rcv_pkt
 rsi_interrupt_handler
 rs22_init_wlan_device
 rs22_send_data
 rs22_regster_recv_api
 rs22_regester_user_memaAlloc_api
 rs22_read_pending_event
 rsi_configure_spi_power_pins
 rs22_get_pkt_count
 rs22_get_scan_info
 rs22_init_wlan_parameters
 rs22_configure_throughput

 Author :

 Rev History:
 Sl  By           date        change     details
 ---------------------------------------------------------
 1   Redpine Signals
 ---------------------------------------------------------
 *END*************************************************************************/

#include "includes.h"
#include "rsi_nic.h"
#include "rsi_spi.h"
#include "rsi_api.h"
#include "process_data.h"
#include "rsi_util.h"
#include "network_config.h"
#include "internals.h"
#include "Redpine_RS9110_N_11_22.h"

/* exetrn declarations start */
extern networkConfig_t networkConfig;
extern ipconfig_t *ipConfig;
extern int16_t SockHandle0;

/* exetrn declarations  end*/

static void rsi_rejoin(void);

uint8_t auto_connect_open_ap = 0;
uint8_t ssid_found = 0;
uint8_t re_connect = 0;
uint8_t resend_ipconf = 0;
RSI_ADAPTER Adaptor;

struct sock_table_t gsock_table[MAX_TABLE_ENTRY];
struct appid_t stapid[MAX_APP_ID_ENTRIES];
int16_t rssi_val = 0;
uint8_t fwversion[20] = { 0 };

volatile uint8_t is_pwr_save_enable = TRUE;
extern volatile uint8_t ps_mode;

#if 0
#define dprintf printf
#else
#define dprintf(...)
#endif

/*FUNCTION*********************************************************************
 Function Name  : rsi_spi_init
 Description    : this function initializes the SPI master device, Initializes the
 network interface structure and loads the software boot loader
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/
int32_t rsi_spi_init(void) // checked
{

    int32_t status, ii;

    /*
     *	Configuring PORTD in Master mode, which communicates
     * 	with RS22-SPI interface (Slave SPI)
     */

    /* Init SS pin as output with wired AND and pull-up. */
    /* Initialize SPI master on port D. */

    rsi_spi_MasterInit();

    /* Initialize network interface structure */
    Adaptor.isr_intr_cnt = 0; //To identify how many times we got an interrupt
//    Adaptor.image_upgrade = 0; // By default soft bootloading
    Adaptor.cnt = 0;

    Adaptor.datasend = FALSE;
    Adaptor.pkt_received = 0; // By default pkt is not received
    // Once we get an interrupt we will make it as 1
    Adaptor.Data_recieved = 0;
    Adaptor.join_count = 0;
    Adaptor.socket_created = 0;
    Adaptor.tcp_configured = FALSE;
    Adaptor.sock0_created = 0;
    Adaptor.sock1_created = 0;
    Adaptor.norsp = 0;

#ifdef FORCE_FW_UPGRADE	
    Adaptor.image_upgrade = ENABLE;
#endif 	//FORCE_FW_UPGRADE
    /* Preparing data */
    for (ii = 0; ii < 1400; ii++) {
        Adaptor.buffer0[ii] = ii;
    }
    status = rsi_decision_for_image_upgrade();

    return (status);
}

#if 1
int32_t rsi_decision_for_image_upgrade(void) // checked
{
    int32_t status = RSI_STATUS_SUCCESS;

    status = rsi_send_ssp_init_cmd();
    if (status == RSI_STATUS_SUCCESS) {

#if BOOT_LOAD
#ifndef EVAL_LICENCE

        /*  0 for Soft Boot Load
         1 for Image Upgrade */
        if (Adaptor.image_upgrade == 1) {

            if (rsi_load_firmware((unsigned int)MGMT_LOAD_IMAGE_UPGRADER) != 0) {
                while (0)
                    ;
            } else {
                while (0)
                    ;
            }
        } else
#endif		//EVAL_LICENCE
        {

            /* Display Progress on the LCD */
            //TBD: displayInfo(IN_PROGRESS);
            if (rsi_load_firmware((unsigned int)MGMT_LOAD_SOFT_BOOTLOADER) != 0) {
                status = status;
                while (0)
                    ;
            } else {
                status = status;
                while (0)
                    ;
            }
        }

#endif
    }
    return status;

}
#else
//TBD:
int32_t rsi_decision_for_image_upgrade(void)
{
    return RSI_STATUS_SUCCESS;
}
#endif

/*FUNCTION*********************************************************************
 Function Name  : rsi_auto_firmware_upgrade
 Description    :
 Returned Value :
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 |     |  X  |     |
 |     |  X  |     |
 *END**************************************************************************/
#ifndef EVAL_LICENCE
void rsi_auto_firmware_upgrade(uint8_t upgrade)
{
    rsi_configure_spi_power_pins();
    Adaptor.image_upgrade = upgrade;
    rsi_spi_MasterInit();
    rsi_decision_for_image_upgrade();
}
#endif  //EVAL_LICENCE
/*FUNCTION*********************************************************************
 Function Name  : rsi_rcv_pkt
 Description    : Upon the occurence of a data pending interrupt, this
 function will be called. Determine what type of packet it is
 and handle it accordingly.
 Returned Value : On success RS22_STATUS_SUCCESS will be returned
 else RS22_STATUS_FAILURE is returned
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 DataRcvPacket	 |     |  X  |     | Pointer to the recv buffer
 size             |     |  X  |     | Pointer to the length
 *END**************************************************************************/
//uint32_t ip_addr;
//uint16_t sleep_count = 0;
//volatile uint16_t  second_sock_created = 0;
TCPIPEvt_t temp_event[1];
uint8_t join_retry = 0;
uint8_t disconnected = 0;

#define IPCONF_FAILED   2

RS22_STATUS rsi_rcv_pkt(uint8_t **DataRcvPacket, uint32_t *size)
{
    RS22_STATUS Status = RS22_STATUS_SUCCESS;
    uint32_t pktLen;
    uint32_t q_no;
    uint16_t type;
    uint8_t desc[16];
    TCPIPEvt_t *tcp_event;
    uint32_t subtype;
    uint8_t *Data_packet = 0;
    uint8_t index = 2;
    uint16_t uSocketDescriptor = 0;
    int32_t ii, tab_indx, app_indx, cnt;

    dprintf("rsi_rcv_pct: ");
    do {
        Status = rsi_card_read(desc, &DataRcvPacket, &pktLen, &q_no);

        if (Status != RS22_STATUS_SUCCESS) {
            break;
        }

        switch (q_no) {
            case RCV_DATA_Q: {/* Recieve data queue */
                dprintf("RCV_DATA_Q - ");
                *size = pktLen;
                //if(Adaptor.tcp_configured == TRUE)
                {
                    tcp_event = &temp_event[0];
                    rs22_memcpy(tcp_event,((*DataRcvPacket)), pktLen);
                    Data_packet = (*DataRcvPacket);

                    subtype = CPU_TO_LE16(tcp_event->uEvtSubType);

                    switch (subtype) {
                        case E_TCP_EVT_SocketCreateEvent: /* Socket create event */
                        {
                            dprintf("E_TCP_EVT_SocketCreateEvent\n");
                            /* Decode the socket type */
                            rs22_memcpy((void *)&tcp_event->EvtBody.stSocketResp.uSocketType,
                                    ((Data_packet) + 2), 2);
                            /* Decode the socket descriptor */
                            rs22_memcpy((void *)&tcp_event->EvtBody.stSocketResp.uSocketDescriptor,
                                    ((Data_packet) + 4),2);
                            /* Decode the local port number */
                            rs22_memcpy((void *)&tcp_event->EvtBody.stSocketResp.stLocalAddress.sin_port,
                                    ((Data_packet)+6), 2);
                            /* Decode the local ip address  */
                            rs22_memcpy((void *)tcp_event->EvtBody.stSocketResp.stLocalAddress.sin_addr.s_addr,
                                    ((Data_packet)+8),4);

                            ii
                                    = CPU_TO_LE16(tcp_event->EvtBody.stSocketResp.uSocketDescriptor);
                            uSocketDescriptor = ii;
                            /* Get the free entry in the socket data base */
                            if ((tab_indx = get_free_table_entry()) >= 0) {
                                app_indx
                                        = identify_table_entry(
                                                CPU_TO_LE16(tcp_event->EvtBody.stSocketResp.stLocalAddress.sin_port),
                                                &tcp_event->EvtBody.stSocketResp.stLocalAddress.sin_addr.s_addr[0]);
                                if (app_indx >= 0) {
                                    gsock_table[tab_indx].sockd = ii;
                                    gsock_table[tab_indx].apid = app_indx;
                                    gsock_table[tab_indx].prototype
                                            = CPU_TO_LE16(tcp_event->EvtBody.stSocketResp.uSocketType);
                                    /* Socket create event callback function */
                                    stapid[app_indx].stCb.cb_sock_create_event(
                                            ii,
                                            CPU_TO_LE16(tcp_event->EvtBody.stSocketResp.stLocalAddress.sin_port));
                                    Adaptor.datasend = TRUE;
                                }
                            }
                            Adaptor.InterruptStatus = Adaptor.InterruptStatus
                                    & (~(SD_DATA_PENDING));
                        }
                            //TBD: LED7 = LED_OFF;
                            //return uSocketDescriptor;
                            return tcp_event->EvtBody.stSocketResp.uErrorCode;

                        case E_TCP_EVT_TCPConfigEvent: /* Ipconfig event */
                        {
                            dprintf("E_TCP_EVT_TCPConfigEvent\n");
                            index += 6;
                            rs22_memcpy( ipConfig->uIPaddr, ((Data_packet) + index), 4);
                            index += 4;
                            rs22_memcpy( ipConfig->uSubnet, ((Data_packet) + index), 4);
                            index += 4;
                            rs22_memcpy( ipConfig->uDefaultGateway, ((Data_packet) + index), 4);
                            index += 4;
                            if ((ipConfig->uIPaddr[0] == 0)
                                    && (ipConfig->uIPaddr[1] == 0)
                                    && (ipConfig->uIPaddr[2] == 0)
                                    && (ipConfig->uIPaddr[3] == 0)) {
                                if ((auto_connect_open_ap == ENABLE)
                                        && (re_connect
                                                < Adaptor.ScanRsp.uScanCount)) {
                                    resend_ipconf = 1;
                                    rsi_disconnect_wlan_conn();
                                    break;
                                } else {
                                    Adaptor.event_type = TCP_CFG_DONE;
                                    return IPCONF_FAILED;
                                }
                            }
                            Adaptor.event_type = TCP_CFG_DONE;
                        }
                            Adaptor.InterruptStatus = Adaptor.InterruptStatus
                                    & (~(SD_DATA_PENDING));
                            resend_ipconf = 0;
                            return RS22_STATUS_SUCCESS;

                        case E_TCP_EVT_Close: /* Socket close event */
                        {
                            dprintf("E_TCP_EVT_Close\n");
                            /* Get the entry of the socket in the table */
                            for (cnt = 0; cnt < MAX_TABLE_ENTRY; cnt++) {
                                if (gsock_table[cnt].sockd
                                        == tcp_event->EvtBody.stCloseResp.uSocketDescriptor) {
                                    Adaptor.apid = gsock_table[cnt].apid;
                                    //gsock_table[cnt].prototype = -1;
                                    gsock_table[cnt].sockd = -1;
                                    gsock_table[cnt].sock_status = 0;
                                }
                            }
                            /* Close event call back function */
                            stapid[Adaptor.apid].stCb.cb_close_event(
                                    tcp_event->EvtBody.stCloseResp.uSocketDescriptor);
                        }
                            return RS22_STATUS_SUCCESS;

                        case E_TCP_EVT_ConnectEvent: /* Got the TCP client connect event */
                        {
                            uint32_t Delay = 0;
                            dprintf("E_TCP_EVT_ConnectEvent\n");
                            rs22_memcpy((void *)&tcp_event->EvtBody.stConnect.uSocketDescriptor,
                                    ((Data_packet) + 2), 2);
                            for (cnt = 0; cnt < MAX_TABLE_ENTRY; cnt++) {
                                if ((gsock_table[cnt].sockd
                                        == tcp_event->EvtBody.stConnect.uSocketDescriptor)) {
                                    if (tcp_event->EvtBody.stConnect.uSocketDescriptor
                                            >= 0) //changed to >=
                                    {
                                        int32_t apid;
                                        apid = gsock_table[cnt].apid;
                                        gsock_table[cnt].sock_status = 1;

                                        // FDI: Give notification that we have accepted a connection
                                        // and it is ready to use.
                                        stapid[apid].stCb.cb_accept_event(
                                                tcp_event->EvtBody.stConnect.uSocketDescriptor,
                                                0, // not used
                                                tcp_event->EvtBody.stConnect.rtAddress.sin_port,
                                                *((TUInt32 *)tcp_event->EvtBody.stConnect.rtAddress.sin_addr.s_addr));
                                    } else {
                                        Adaptor.apid = gsock_table[cnt].apid;
                                        gsock_table[cnt].sockd = -1;
                                        gsock_table[cnt].sock_status = 0;

                                        rsi_socket_deregister(Adaptor.apid);
                                        while (Delay++ < 20000)
                                            ;
                                        /* Initialize socket creation info */

                                        rsi_initialize_netwrok_config1(
                                                &networkConfig);
                                        /* Create the first socket */

                                        SockHandle0 = rsi_create_user_socket(
                                                &networkConfig);

                                        if (SockHandle0 == RSI_STATUS_FAILURE)

                                        {

                                            while (1)
                                                ;

                                        }
                                        break;
                                    }

                                }
                            }
                            Adaptor.event_con = 1;
                            //TBD: LED7 = LED_ON;
                        }
                            break;
                        case E_TCP_EVT_MessageRecvEvent: /* This event generated when the TCP or UDP
                         application RECV some data packet*/
                        {
                            uint16_t uDataOffset=0;
                            uint16_t prototype;
                            dprintf("E_TCP_EVT_MessageRecvEvent\n");
                            /* Decode the socket Descriptor */
                            rs22_memcpy((void *)&tcp_event->EvtBody.stRecv.uSocketDescriptor,((Data_packet) + 2),2);
                            /* decode the data paket length */
                            rs22_memcpy((void *)&tcp_event->EvtBody.stRecv.uBufferLength,((Data_packet)+4), 4);
                            rs22_memcpy((void *)&tcp_event->EvtBody.stRecv.uDataOffset,
                                    ((Data_packet)+8),2);
                            prototype = rsi_get_proto_type(uSocketDescriptor);
                            switch (prototype) {
                                case RSI_PROTO_UDP:
                                    uDataOffset = RSI_PROTO_UDP_OFFSET;
                                    break;
                                case RSI_PROTO_TCP:
                                case RSI_PROTO_LTCP:
                                    uDataOffset = RSI_PROTO_TCP_OFFSET;
                                    break;
                            }
                            /* Copy the data received */
                            rs22_memcpy(&tcp_event->EvtBody.stRecv.buff[0], ((Data_packet)+ uDataOffset),
                                    tcp_event->EvtBody.stRecv.uBufferLength);

                            for (cnt = 0; cnt < MAX_TABLE_ENTRY; cnt++) {
                                /* Check the socket descriptor is valid */
                                if (gsock_table[cnt].sockd
                                        == tcp_event->EvtBody.stRecv.uSocketDescriptor) {
                                    Adaptor.apid = gsock_table[cnt].apid;
                                    break;
                                }
                            }
                            /* Handover the data packet to the Recv callback function */
                            stapid[Adaptor.apid].stCb.cb_recv_event(
                                    tcp_event->EvtBody.stRecv.buff,
                                    CPU_TO_LE32(tcp_event->EvtBody.stRecv.uBufferLength),
                                    tcp_event->EvtBody.stRecv.uSocketDescriptor);
                        }
                            break;
                        case E_TCP_EVT_RemoteTerminateEvent: {
                            dprintf("E_TCP_EVT_RemoteTerminateEvent\n");

                            /* This event is generated when the remote server or client closes the connection */

                            for (cnt = 0; cnt < MAX_TABLE_ENTRY; cnt++) {
                                /* Check wether the socket descriptor is valid */
                                if (gsock_table[cnt].sockd
                                        == tcp_event->EvtBody.stRemoteTerminate.uSocketDescriptor) {
                                    Adaptor.apid = gsock_table[cnt].apid;
                                    gsock_table[cnt].sockd = -1;
                                    gsock_table[cnt].sock_status = 0;
                                }
                            }
                            /* Hand over to the  remote close event Call back function  */
                            stapid[Adaptor.apid].stCb.cb_remoteclose_event(
                                    tcp_event->EvtBody.stRemoteTerminate.uSocketDescriptor);
#if 0
                            rsi_socket_deregister(Adaptor.apid);
                            while (Delay++ < 20000)
                                ;
                            /* Initialize socket creation info */

                            rsi_initialize_netwrok_config1(&networkConfig);
                            /* Create the first socket */

                            SockHandle0
                                    = rsi_create_user_socket(&networkConfig);

                            if (SockHandle0 == RSI_STATUS_FAILURE)

                            {

                                while (1)
                                    ;

                            }
                            //TBD: LED7 = LED_OFF;
#endif
                        }
                            break;

                        case E_TCP_EVT_SendRequestResponse: {
                            dprintf("E_TCP_EVT_SendRequestResponse\n");
                            //Failed to send data
                            Adaptor.cnt++;
                            Adaptor.datasend = TRUE;
                        }
                            break;
                        case E_TCP_EVT_RssiEvent: {
                            dprintf("E_TCP_EVT_RssiEvent\n");
                            /* Decode the RSSI value */
                            rs22_memcpy(&rssi_val,
                                    ((Data_packet) + 2), 2);

                        }
                            break;
                        case E_TCP_EVT_Connection_status: {
                            uint16_t wlan_state;
                            dprintf("E_TCP_EVT_Connection_status\r");

                            /* Decode the Wlan state value */
                            rs22_memcpy(&wlan_state,
                                    ((Data_packet) + 2), 2);
                            if (wlan_state == DISCONNECT) {
                                if (disconnected == 0) {
                                    // TBD: LED7 = LED_OFF;
                                    //TBD: displayInfo(LINK_FAIL);

                                }
                                disconnected = 1;
                            }
                        }
                            break;

                        case E_TCP_EVT_FW_VERSION_Get: {
                            dprintf("E_TCP_EVT_FW_VERSION_Get\n");
                            /* Decode the firmware version */
                            rs22_memcpy(fwversion,
                                    ((Data_packet) + 2), 10);

#if (!defined FORCE_FW_UPGRADE && !defined EVAL_LICENCE)					 
                            /* Check if the retrieved firmware version is maching with the one provided in
                             network_config.h file */
                            if (Adaptor.event_type < INIT_DONE) {
                                if ((rs22_strcmp((const char *)fwversion, FWVERSION))
                                        != 0) {
                                    rsi_auto_firmware_upgrade(ENABLE);
                                } else {
                                    rsi_send_init_request();
                                }
                            }
#endif			   		
                        }
                            break;
                        case E_WLAN_EVT_DisconnectResp: {
                            dprintf("E_WLAN_EVT_DisconnectResp\n");
                            Adaptor.event_type = SCAN_DONE;
                            if ((auto_connect_open_ap == ENABLE) && (re_connect
                                    <= Adaptor.ScanRsp.uScanCount)) {
                                uint32_t Delay = 0;
                                while (Delay++ < 200000)
                                    ;
                                rsi_rejoin();
                                if (re_connect >= Adaptor.ScanRsp.uScanCount)
                                    return IPCONF_FAILED;
                                break;
                            }
                        }
                            break;

                        default: {
                            //Nothing to do
                        }
                    }//End of 2nd switch
                }//End of if(tcp_configured)
            }//End of case RCV_DATA_Q
                break;

            case RCV_LMAC_MGMT_Q: {/* LMAC management queue */
                dprintf("RCV_LMAC_MGMT_Q\n");
            }
                break;

            case RCV_TA_MGMT_Q: {/* TA management queue */
                dprintf("RCV_TA_MGMT_Q - ");
                if (desc[0] & 0xFF) {
                    pktLen = LE16_TO_CPU(*(uint16_t *)&(desc[8]));
                } else {
                    pktLen = LE32_TO_CPU((uint32_t)(desc[0]));
                } /* End if <condition> */

                if (!pktLen) {
                    pktLen = 4;
                }
                pktLen += 16;

                /* Type is upper 5 bits of descriptor's second byte */
                type = ((desc[1] & 0xFF) << 8);

                switch (type) {
                    case MGMT_DESC_TYP_CARD_RDY_IND: /* Card ready indication firmware loaded */
                    {
                        dprintf("MGMT_DESC_TYP_CARD_RDY_IND\n");
                        Adaptor.event_type = CARD_READY;
#if BOOT_LOAD
#ifndef EVAL_LICENCE
                        if (Adaptor.image_upgrade == 1) {
                            uint16_t Delay = 0;
                            //TBD: displayInfo(IMAGE_UPGRADE);
                            rsi_upgrade_firmware();
                            //TBD: displayInfo(IMAGE_UPGRADE_COMPLETED);
                            while (Delay++ < 50000)
                                ;
                            rsi_auto_firmware_upgrade(DISABLE);
                            break;
                        } else
#endif	//EVAL_LICENCE			
                        {
#endif	             /* Send the WLAN init request */

#ifdef LOAD_INST_FROM_HOST

                            rsi_load_firmware((uint16_t)LOAD_SB_TADM2);
#endif		

#if (!defined FORCE_FW_UPGRADE && !defined EVAL_LICENCE)
                            rsi_send_fwversion_Req();
                            break;

#else	//FORCE_FW_UPGRADE		
                            rsi_send_init_request();
#endif                            
#if BOOT_LOAD
                        }
#endif
                    }
#if !BOOT_LOAD                    
                        break;
#endif
                    case MGMT_DESC_INIT_RESPONSE: /* WLAN init Response  */
                    {
                        dprintf("MGMT_DESC_INIT_RESPONSE\n");
                        Adaptor.event_type = INIT_DONE;

                        /* Send the WLAN scan request */
                        rsi_send_scan_request(Adaptor.scan_params.uChannelNo,
                                Adaptor.scan_params.uSSID);

                    }
                        break;

                    case MGMT_DESC_SCAN_RESPONSE: /* WLAN scan response */
                    {
                        dprintf("MGMT_DESC_SCAN_RESPONSE\n");
                        //TBD: displayInfo(SCAN_OVER);
                        /* Decode the APs list */
                        rsi_get_ssid_list(Adaptor.RcvPkt);
                        Adaptor.event_type = SCAN_DONE;
                        Network_Redpine_RS9110_Scan_Complete_Callback();

                        if (auto_connect_open_ap == 0) {
                            for (index = 0; index < Adaptor.ScanRsp.uScanCount; index++) {
                                if ((rs22_strcmp((const char *)Adaptor.ScanRsp.stScanInfo[index].uSSID, (const char *)Adaptor.join_params.uSSID))
                                        == 0) {
                                    ssid_found = 1;
                                    break;
                                }

                            }
                        } else {
                            for (index = re_connect; index
                                    < Adaptor.ScanRsp.uScanCount; index++) {
                                if (Adaptor.ScanRsp.stScanInfo[index].uSecMode
                                        == 0) {
                                    rs22_memcpy(Adaptor.join_params.uSSID, Adaptor.ScanRsp.stScanInfo[index].uSSID, 32);
                                    re_connect = index + 1;
                                    break;
                                }
                            }
                        }
                        /* Send the join request in security mode */
                        rsi_send_join_request(&Adaptor.join_params);

                    }
                        break;

                    case MGMT_DESC_JOIN_RESPONSE: /* Join response */
                    {
                        dprintf("MGMT_DESC_JOIN_RESPONSE\n");
                        if (desc[2]) {
                            if ((auto_connect_open_ap == ENABLE) && (re_connect
                                    < Adaptor.ScanRsp.uScanCount)) {
                                rsi_rejoin();
                                if (re_connect >= Adaptor.ScanRsp.uScanCount) {
                                    Network_Redpine_RS9110_Join_Complete_Callback(
                                            EFalse);
                                    return RS22_STATUS_FAILURE;
                                }
                                break;
                            } else if ((auto_connect_open_ap == ENABLE)
                                    && (re_connect
                                            >= Adaptor.ScanRsp.uScanCount)) {
                                Network_Redpine_RS9110_Join_Complete_Callback(
                                        EFalse);
                                return RS22_STATUS_FAILURE;
                            } else {
                                if (((auto_connect_open_ap == DISABLE))
                                        && (ssid_found == 1) && (join_retry
                                        < 10)) {
                                    join_retry++;
                                    rsi_rejoin();
                                } else {
                                    join_retry = 0;
                                    Network_Redpine_RS9110_Join_Complete_Callback(
                                            EFalse);
                                    return RS22_STATUS_FAILURE;
                                }
                            }
                        } else {
                            //TBD: displayInfo(JOIN_OVER);
                            /* Link setup copleted successfully */
                            Adaptor.event_type = JOIN_DONE;
                            Network_Redpine_RS9110_Join_Complete_Callback(ETrue);
                            Adaptor.join_count++;
                            if (resend_ipconf == 1) {
                                rsi_send_TCP_config_request(
                                        Adaptor.ipConfig.DHCP,
                                        Adaptor.ipConfig.uIPaddr,
                                        Adaptor.ipConfig.uSubnet,
                                        Adaptor.ipConfig.uDefaultGateway);
                                break;
                            }
                        }

                    }
                        break;

                    default: {
                        while (0)
                            ;
                    }
                }
            }
                break;
            default: {
                while (0)
                    ;
                break;
            }
        }
    } while (FALSE);

    return Status;
}

void rsi_rejoin(void)
{
    uint8_t index;

    dprintf("rsi_rejoin:\n");
    if (auto_connect_open_ap == 1) {
        for (index = re_connect; index < Adaptor.ScanRsp.uScanCount; index++) {
            if (Adaptor.ScanRsp.stScanInfo[index].uSecMode == 0) {
                rs22_memcpy(Adaptor.join_params.uSSID, Adaptor.ScanRsp.stScanInfo[index].uSSID, 32);
                re_connect = index + 1;
                break;
            }
        }
        if (index >= Adaptor.ScanRsp.uScanCount) {
            re_connect = index;
        }
    }

    /* Send the join request in security mode */
    rsi_send_join_request(&Adaptor.join_params);
}
/*FUNCTION*********************************************************************
 Function Name  : rsi_interrupt_handler
 Description    : Upon the occurence of an interrupt, the interrupt handler will
 be called
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 DataRcvPacket    |     |  X  |     | Ptr to recv buffer
 size             |     |  X  |     | pointer to the size
 *END**************************************************************************/
uint32_t InterruptStatus;
uint8_t InterruptMaskReg = 0xD3;
uint16_t busy = 0;

RS22_STATUS rsi_interrupt_handler(uint8_t *DataRcvPacket, uint32_t *size)
{
    INTERRUPT_TYPE InterruptType;

    uint32_t pktLen = 0;
    RS22_STATUS status;

    do {

        /* Check any interrupt is pending */
        InterruptStatus = rsi_send_ssp_read_intrStatus_cmd();
        if (InterruptStatus == 0xffff) {
            busy++;
            continue;
        }
        Adaptor.InterruptStatus = ((uint8_t)InterruptStatus
                & (~InterruptMaskReg));
        if (Adaptor.InterruptStatus == 0) {
            if (Adaptor.pkt_received >= 1) {
                Adaptor.pkt_received--;
            }
            break;
        }
        do {
            /* Interrupt is pending */
            InterruptType = rs22_get_interrupt_type( Adaptor.InterruptStatus );

            switch (InterruptType) {
                case BUFFER_FULL: {
                    /* This event is recvd when the hardware buffer is full */
                    Adaptor.BufferFull = TRUE;
                    Adaptor.once_full++;
                    Adaptor.datasend = FALSE;
                    Adaptor.InterruptStatus = Adaptor.InterruptStatus
                            & (~(SD_PKT_BUFF_FULL));
                    *size = 0;
                    rsi_send_ssp_intrAck_cmd(CPU_TO_LE32(SD_PKT_BUFF_FULL));
                }
                    break;

                case BUFFER_FREE: {
                    /* This event is recvd when the hardware buffer becomes empty */
                    Adaptor.BufferFull = FALSE;
                    Adaptor.datasend = TRUE;
                    Adaptor.once_empty++;
                    *size = 0;
                    Adaptor.InterruptStatus = Adaptor.InterruptStatus
                            & (~(SD_PKT_BUFF_EMPTY));
                    rsi_send_ssp_intrAck_cmd(CPU_TO_LE32(SD_PKT_BUFF_EMPTY));
                }
                    break;

                case SLEEP_INDCN: {
                    *size = 0;
                    Adaptor.InterruptStatus = Adaptor.InterruptStatus
                            & (~(SD_SLEEP_INDCN));
                }
                    break;
                case WAKEUP_INDCN: {
                    *size = 0;
                    InterruptMaskReg = (InterruptMaskReg | 0x20);

                    rsi_ssp_write_mask_register(InterruptMaskReg);
                    is_pwr_save_enable = TRUE;
                    Adaptor.InterruptStatus = Adaptor.InterruptStatus
                            & (~(SD_WAKEUP_INDCN));
                }
                    break;

                case MGMT_PENDING: {
                    /* This event is rcvd when MGMT data is pending */
                    *size = 0;
                    Adaptor.InterruptStatus = Adaptor.InterruptStatus
                            & (~(SD_MGMT_PENDING));
                }
                    break;

                case DATA_PENDING: {
                    if ((status = rsi_rcv_pkt(&DataRcvPacket, &pktLen))
                            != RS22_STATUS_SUCCESS) {
                        /* Indicates unable to receive pkt */
                        return status;

                    } else {
                        /* Indicates pkt received successfully */
                        *size = pktLen;
                        status = RS22_STATUS_SUCCESS;
                    }
                    Adaptor.InterruptStatus = Adaptor.InterruptStatus
                            & (~(SD_DATA_PENDING));

                }
                    break;

                default: {
                    *size = 0;
                    Adaptor.int_ctr++;
                    Adaptor.InterruptStatus = Adaptor.InterruptStatus & 0;
                }
                    break;
            }

        } while (Adaptor.InterruptStatus != 0);
        Adaptor.pkt_received--;

    } while (1);
    //TBD: ICU.IER[9].BIT.IEN5 = 1;
    return status;

}

int16_t rsi_send_rssi_query_req(void)
{
    uint32_t size;
    int16_t status = 0;

    rsi_Send_Rssi_Req();
    /* Poll for the interrupt */
    do {
        status = rsi_interrupt_handler(Adaptor.RcvPkt, &size);

    } while (status == RS22_STATUS_FAILURE);

    return rssi_val;

}

int16_t rsi_wlan_conn_query_req(void)
{
    uint32_t size;
    int16_t status = 0;

    rsi_Send_Wlan_Conn_Status_Req();
    /* Poll for the interrupt */
    do {
        status = rsi_interrupt_handler(Adaptor.RcvPkt, &size);

    } while (status == RS22_STATUS_FAILURE);

    return RS22_STATUS_SUCCESS;

}
/*FUNCTION*********************************************************************
 Function Name  : rsi_send_scan_request
 Description    : This function  sends WLAN scan request
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 channel          |  X  |     |     | Channel number on which to scan
 SSID             |  X  |     |     | SSID of the Access Point
 *END****************************************************************************/

uint16_t rsi_send_sleep_request(void)
{
    uint8_t InterruptStatus;

    InterruptStatus = rsi_send_ssp_read_intrStatus_cmd(Adaptor);
    InterruptStatus = ((uint8_t)InterruptStatus & 0x2F);

    if (InterruptStatus & 0x20) {
        InterruptMaskReg = (InterruptMaskReg & 0xDF);
        rsi_send_ssp_intrAck_cmd(0x20);
        rsi_ssp_write_mask_register(InterruptMaskReg);
        is_pwr_save_enable = FALSE;
    }
    return 0;
}
/*FUNCTION*********************************************************************
 Function Name  : get_free_table_entry
 Description    : This function  finds the free table entry in the socket data base
 Returned Value : if success free table entry else FAILURE
 Parameters     : NONE
 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/
int32_t get_free_table_entry(void)
{
    int32_t index;

    /* Search for free table entry */
    for (index = 0; index < MAX_TABLE_ENTRY; index++) {
        if (gsock_table[index].sockd == -1) {
            /* Got the free table entry */
            return index;
        }
    }
    /* Table is full */
    return -1;
}

/*FUNCTION*********************************************************************
 Function Name  : get_table_entry
 Description    : This function  finds the table entry of the socket descriptor
 stored
 Returned Value : if success Table entry else FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 sd               |  X  |     |     | socket descriptor
 *END****************************************************************************/
int32_t get_table_entry(uint16_t sd)
{
    uint32_t index;

    /* Search for the table entry  where the socket descriptor is stored */
    for (index = 0; index < MAX_TABLE_ENTRY; index++) {
        if (gsock_table[index].sockd == sd) {
            /* Got the table entry */
            return (index);
        }
    }
    /* entry not found  */
    return (-1);
}

int32_t get_sock_status(uint16_t sd)
{
    return gsock_table[get_table_entry(sd)].sock_status;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_socket_register
 Description    : This function registers the callback functions
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+------+-----+-----+------------------------------
 Name             | I/P  | O/P | I/O | Purpose
 -----------------+------+-----+-----+------------------------------
 appId			 |  	| X   |		| Table entry where the call back functions are registered
 uPort			 |	X	|	  |		| port number
 uIP				 |	X	|	  |		| IP address
 pCallbacks		 |	X	|	  |		| Pointer to the callback functions
 *END****************************************************************************/
uint32_t rsi_socket_register(
        uint32_t *appId,
        uint16_t uPort,
        uint32_t uIP,
        Callbacks_P pCallbacks)
{
    uint32_t i;

    for (i = 0; i < MAX_APP_ID_ENTRIES; i++) {
        if (stapid[i].apid == -1) {
            /* Found the entry in the socket data base */
            stapid[i].apid = i;
            break;
        }
    }

    if (i == MAX_APP_ID_ENTRIES) {
        /* Socket data base is full */
        return (uint32_t) - 1;
    }

    *appId = i;
    /* Regester cllback functions , ip adress and port numbers */
    stapid[i].port = uPort;
    stapid[i].ip = uIP;
    stapid[i].stCb.cb_sock_create_event = pCallbacks->cb_sock_create_event;
    stapid[i].stCb.cb_accept_event = pCallbacks->cb_accept_event;
    stapid[i].stCb.cb_recv_event = pCallbacks->cb_recv_event;
    stapid[i].stCb.cb_close_event = pCallbacks->cb_close_event;
    stapid[i].stCb.cb_remoteclose_event = pCallbacks->cb_remoteclose_event;

    return 0;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_socket_deregister
 Description    : This function deregisters the call back functions
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 appid            |  X  |     |     | Entry of the table where the call back functions are registered

 *END****************************************************************************/

int32_t rsi_socket_deregister(uint32_t appid) /* Socket deregester function */
{
    if (stapid[appid].apid != -1) {
        stapid[appid].apid = -1;
        stapid[appid].port = 0;
        stapid[appid].ip = 0;
        stapid[appid].stCb.cb_sock_create_event = NULL;
        stapid[appid].stCb.cb_accept_event = NULL;
        stapid[appid].stCb.cb_recv_event = NULL;
        // stapid[appid].stCb.cb_close_event		= NULL;
        stapid[appid].stCb.cb_remoteclose_event = NULL;
        Adaptor.socket_created--;
        return 0;
    }
    return -1;
}

/*FUNCTION*********************************************************************
 Function Name  : identify_table_entry
 Description    : This function identifies the table entry associated with port number and ip address
 Returned Value : On success Table entry else FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 uPort			 |	X  |     |     | port number
 aIP				 |	X  |     |     | IP address
 *END****************************************************************************/

int32_t identify_table_entry(uint16_t uPort, uint8_t *aIP)
{
    volatile int32_t index;

    for (index = 0; index < MAX_APP_ID_ENTRIES; index++) {
        if ((stapid[index].port == uPort)) {
            return index;

        }

    }

    return -1;
}

/*FUNCTION*********************************************************************
 Function Name  : sock_cb0
 Description    : This function stores the socket descriptor of the socket created,
 to the network interface structure
 Returned Value : SUCCESS
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 sockd            |  X  |     |     | Socket Descriptor to be stored
 sourcePort       |  X  |     |     | source port
 *END****************************************************************************/

int32_t sock_cb0(int32_t sockd, uint16_t sourcePort)
{
    Adaptor.sock0_created = 1;
    Adaptor.sd0 = sockd;
    return 0;

}

/*FUNCTION*********************************************************************
 Function Name  : sock_cb1
 Description    : This function stores the socket descriptor of the socket created,
 to the network interface structure
 Returned Value : SUCCESS
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 sockd            |  X  |     |     | Socket Descriptor to be stored
 sourcePort       |  X  |     |     | source port
 *END****************************************************************************/

int32_t sock_cb1(int32_t sockd, uint16_t sourcePort)
{
    /* Set the socket created flag */
    Adaptor.sock1_created = 1;
    Adaptor.sd1 = sockd;
    return 0;
}

/*FUNCTION*********************************************************************
 Function Name  : accept_cb
 Description    : This is the callback function to handle server connection TCP accept
 Returned Value :  SUCCESS
 Parameters     :

 -----------------------+-----+-----+-----+------------------------------
 Name                   | I/P | O/P | I/O | Purpose
 -----------------------+-----+-----+-----+------------------------------
 sd					   |  X  |     |     | Socket Descriptor
 nsd					   |  X  |     |     | nsd
 destinationPort		   |  X  |     |     | Destination port
 destinationIpAddress   |  X  |     |     | Destination ip address

 *END****************************************************************************/

uint32_t accept_cb(
        int32_t sd,
        int32_t nsd,
        uint16_t destinationPort,
        uint8_t *destinationIpAddress)
{
    /* Add something in future, if necessary */
    return 0;
}

/*FUNCTION*********************************************************************
 Function Name  : recv_cb0
 Description    : This he callback function to handle the recieved data
 from the remote server or client
 Returned Value : SUCCESS
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 buff             |   X |	 |	   | Received data buffer
 BytesReceived    |   X |	 |	   | Length of the received data
 sockHandle       |	 X |	 |	   | socket handle on which the data recieved

 *END****************************************************************************/
uint32_t recv_cb0(uint8_t *data, uint32_t BytesReceived, uint16_t sockHandle)
{
    uint16_t list_sent = 0;
    uint16_t len = 0;

    if (data[0] == LED) {
        /* Process the LED Related data */

        if ((rsi_process_data(data)) == RSI_STATUS_SUCCESS) {

            /* send LED status to the client */
            while ((rsi_send_data(SockHandle0, 10, data)) != RSI_STATUS_SUCCESS)

            {
                ;
            }

        }
    }
    /* Send the APs list */
    if (list_sent == 0) {

        /* Prepare the packet to send SSID List */

        len = rsi_prepare_pkt_to_send(SSID_LIST, 0, data);
        if (len > 1) {
            while ((rsi_send_data(SockHandle0, len, data))
                    != RSI_STATUS_SUCCESS)
                ;
        }
        list_sent++;
    }
    return (0);
}

/*FUNCTION*********************************************************************
 Function Name  : recv_cb1
 Description    : This he callback function to handle the recieved data
 from the remote server or client
 Returned Value : SUCCESS
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 buff             |   X |	 |	   | Received data buffer
 BytesReceived    |   X |	 |	   | Length of the received data
 sockHandle       |	 X |	 |	   | socket handle on which the data recieved

 *END****************************************************************************/

uint32_t recv_cb1(uint8_t *buff, uint32_t BytesReceived, uint16_t sockHandle)
{

    return (0);
}

/*FUNCTION*********************************************************************
 Function Name  : close_cb
 Description    : This is the call back function to handle the socket close event
 Returned Value : SUCCESS
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 socketId         |   X |     |     | Socket descriptor of the socket closed
 *END****************************************************************************/

uint32_t close_cb(int32_t socketId)
{
    /* Add something in future, if necessary */
    switch (socketId) {
        case 0:
            Adaptor.sock0_created = 0;
            break;
        case 1:
            Adaptor.sock1_created = 0;
            break;

    }
    return 0;
}

/*FUNCTION*********************************************************************
 Function Name  : remote_close_cb
 Description    : This is the call back function to handle the remote peer socket close event
 Returned Value : SUCCESS
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 socketId         |  X  |     |     | Socket descriptor
 *END****************************************************************************/

uint32_t remote_close_cb(int32_t socketId)
{
    switch (socketId) {

        case 0:
            /* Initialize to default value */
            Adaptor.sd0 = -1;
            break;
        case 1:
            /* Initialize to default value */
            Adaptor.sd1 = -1;
            break;
        default:
            break;
    }
    return 0;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_create_user_socket
 Description    : This function regesters callback functions asociated with the socket
 and calls the socket create function
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 networkConfig    | X   |     |     | Pointer to the networkconfig structure
 *END****************************************************************************/
int16_t rsi_create_user_socket(networkConfig_t *networkConfig)
{

    Callbacks_T stcbs0;
    Callbacks_T stcbs1;
    uint32_t appid0;
    uint32_t appid1;
    int32_t Status = RS22_STATUS_SUCCESS;

    if (Adaptor.socket_created >= 4) {
        return -1;
    }
    switch (Adaptor.socket_created) {

        case 0: /* First socket */
        {
            rs22_memcpy(&Adaptor.networkConfig[Adaptor.socket_created], networkConfig,
                    sizeof(networkConfig_t));
            /* Regester the CallBacks for Socket 0 */
            stcbs0.cb_sock_create_event = (CB_Sock_Create_Event)sock_cb0;
            stcbs0.cb_accept_event = (CB_Accept_Event)accept_cb;
            stcbs0.cb_recv_event = (CB_Recv_Event)recv_cb0;
            stcbs0.cb_close_event = (CB_Close_Event)close_cb;
            stcbs0.cb_remoteclose_event = (CB_RemoteClose_Event)remote_close_cb;

            rsi_socket_register(&appid0, networkConfig->stLocalPort,
                    *((uint32_t *)&Adaptor.ipConfig.uIPaddr[0]), &stcbs0);
            /* Send the socket create request */
            Status = rsi_socket_create((SOCKET_TYPE)networkConfig->uSocketType,
                    networkConfig->stLocalPort, &networkConfig->remote_address);
            if (Status == RS22_STATUS_SUCCESS) {
                Adaptor.socket_created++;
            } else {
                return RS22_STATUS_FAILURE;
            }
        }
            break;
        case 1: /* Second socket */
        {
            rs22_memcpy(&Adaptor.networkConfig[Adaptor.socket_created], &networkConfig,
                    sizeof(networkConfig_t));
            /* Regester the CallBacks for Socket 1 */
            stcbs1.cb_sock_create_event = (CB_Sock_Create_Event)sock_cb1;
            stcbs1.cb_accept_event = (CB_Accept_Event)accept_cb;
            stcbs1.cb_recv_event = (CB_Recv_Event)recv_cb1;
            stcbs1.cb_close_event = (CB_Close_Event)close_cb;
            stcbs1.cb_remoteclose_event = (CB_RemoteClose_Event)remote_close_cb;

            rsi_socket_register(&appid1, networkConfig->stLocalPort,
                    *((uint32_t *)&Adaptor.ipConfig.uIPaddr[0]), &stcbs1);

            /* Send the socket create request */
            Status = rsi_socket_create((SOCKET_TYPE)networkConfig->uSocketType,
                    networkConfig->stLocalPort, &networkConfig->remote_address);
            if (Status == RS22_STATUS_SUCCESS) {
                Adaptor.socket_created++;
            } else {
                return RS22_STATUS_FAILURE;
            }
        }
            break;

        default:
            return RS22_STATUS_FAILURE;

    }
    do

    {
        /* Read the socket create event */

        Status = Read_tcp_ip_event();

    } while (Status == RS22_STATUS_FAILURE);
    return Status;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_close_user_socket
 Description    : This function calls socket close function and deregisters the callback functions
 associated with the socket
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 networkConfig    |  X  |     |     | Pointer to the network config structure
 *END****************************************************************************/
int16_t rsi_close_user_socket(networkConfig_t *networkConfig)
{
    uint32_t appId = 0;
    uint16_t sock_entry = 0;
    volatile int32_t Status = 0;

    /* Get table entry for the socket to be closed */
    for (sock_entry = 0; sock_entry < MAX_APP_ID_ENTRIES; sock_entry++) {
        if ((stapid[sock_entry].port == networkConfig->stLocalPort)
                && (stapid[sock_entry].apid != -1)) {
            appId = sock_entry;
            break;
        }
    }

    if (sock_entry == MAX_APP_ID_ENTRIES) {
        return RS22_STATUS_FAILURE;
    }
    /* Send the socket close request */
    Status = rs22_socket_close(networkConfig->uSocketDescriptor);
    if (Status == RS22_STATUS_SUCCESS) {
        Adaptor.socket_created--;
        /* Deregester the socket from the socket data base */
        rsi_socket_deregister(appId);
        do

        {
            /* Read the socket close event */

            Status = Read_tcp_ip_event();

        } while (Status == RS22_STATUS_FAILURE);
        return Status;

    }
    return RS22_STATUS_FAILURE;

}
/*FUNCTION*********************************************************************
 Function Name  : rsi_send_ipconfig_request
 Description    : This function sends the ipconfig request
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 ipConfig         | X   |     |     | Pointer to the ipconfig structure
 *END****************************************************************************/

int16_t rsi_send_ipconfig_request(ipconfig_t *ipConfig)
{
    int16_t Status = RS22_STATUS_SUCCESS;

    Adaptor.ipConfig.DHCP = (uint8_t)ipConfig->DHCP;
    /* Store the ipconfig information in the Adapter structure */
    rs22_memcpy(&Adaptor.ipConfig.uIPaddr[0], &ipConfig->uIPaddr[0] , 4);
    rs22_memcpy(&Adaptor.ipConfig.uSubnet[0], &ipConfig->uSubnet[0] , 4);
    rs22_memcpy(&Adaptor.ipConfig.uDefaultGateway[0], &ipConfig->uDefaultGateway[0] , 4);

    /* Send the ipconfig request */
    Status = rsi_send_TCP_config_request(Adaptor.ipConfig.DHCP,
            Adaptor.ipConfig.uIPaddr, Adaptor.ipConfig.uSubnet,
            Adaptor.ipConfig.uDefaultGateway);
    if (Status == RS22_STATUS_SUCCESS) {
        Adaptor.tcp_configured = TRUE;
    } else {
        Adaptor.tcp_configured = FALSE;
    }

    do

    {
        /* Read the ipconfig done event */

        Status = Read_tcp_ip_event();

    } while (Adaptor.event_type != TCP_CFG_DONE);//while(Status == RS22_STATUS_FAILURE);

    return Status;

}

/*FUNCTION*********************************************************************
 Function Name  : Read_tcp_ip_event
 Description    : This function reads the ipconfig
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/
int16_t Read_tcp_ip_event(void)
{
    volatile RS22_STATUS Status = RS22_STATUS_FAILURE;
    INTERRUPT_TYPE InterruptType;
    uint32_t InterruptStatus = 0;
    uint8_t *RcvPacket = Adaptor.RcvPkt;
    uint32_t size;

    /* Check any interrupt is pending */
    InterruptStatus = rsi_send_ssp_read_intrStatus_cmd();
    if (InterruptStatus == 0xffff) {
        while (1)
            ;
    }
    Adaptor.InterruptStatus = (InterruptStatus & 0x0008);
    if (InterruptStatus == 0) {
        if (Adaptor.pkt_received >= 1) {
            Adaptor.pkt_received--;
        }
        return RS22_STATUS_FAILURE;
    }
    /* read the interrupt type */
    InterruptType = rs22_get_interrupt_type( Adaptor.InterruptStatus );

    if (InterruptType == DATA_PENDING) {/* Data pending interrupt */
        Status = rsi_rcv_pkt(&RcvPacket, &size);
    }
    //TBD: ICU.IER[9].BIT.IEN5 = 1;
    return Status;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_get_ssid_list
 Description    : This function copies the scan response information from scan response
 buffer to network interface

 Returned Value : NONE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 scan_resp        |     |  X  |     | scan_resp buffer

 *END****************************************************************************/

void rsi_get_ssid_list(uint8_t *scan_resp)

{

    uint32_t scan_count = 0, index1 = 0;

    uint32_t err_count = 0;

    uint32_t scan_index = 0;

    /* Decode the APs count that are scanned */

    rs22_memcpy(&scan_count, scan_resp, 4);

    scan_index += 4;

    Adaptor.ScanRsp.uScanCount = scan_count;
    dprintf("rsi_get_ssid_list: scan_count = %d\n", scan_count);
    /* Decode the error count */

    rs22_memcpy(&err_count, scan_resp + scan_index, 4);
    Adaptor.ScanRsp.uErrorcode = err_count;
    scan_index += 4;
    while (index1 < scan_count)

    { /* Decode the channel number */
        Adaptor.ScanRsp.stScanInfo[index1].uChannelNumber
                = scan_resp[scan_index++];
        /* Decode the security mode */

        Adaptor.ScanRsp.stScanInfo[index1].uSecMode = scan_resp[scan_index++];
        /* Decode the RSSI value */

        Adaptor.ScanRsp.stScanInfo[index1].uRssiVal = scan_resp[scan_index++];
        /* Decode the SSID */

        dprintf("rsi_get_ssid_list: index %d - \"%s\"\n", index1, scan_resp
                + scan_index);
        rs22_memcpy(Adaptor.ScanRsp.stScanInfo[index1].uSSID, (scan_resp + scan_index), 32);

        scan_index += 32;

        index1++;

    }

    return;

}

/*FUNCTION*********************************************************************
 Function Name  : rsi_init_socket_data_base
 Description    : This function initializes the socket data base
 Returned Value : NONE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/

void rsi_init_socket_data_base(void) /* Initialize the socket data base */
{
    uint16_t count = 0;
    /* Initialize the socket data base */
    for (count = 0; count < MAX_APP_ID_ENTRIES; count++) {
        stapid[count].apid = -1;
    }
    for (count = 0; count < MAX_TABLE_ENTRY; count++) {
        gsock_table[count].sockd = -1;
    }
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_external_inerrupt_handler
 Description    : This is the external interrupt ISR
 Returned Value : NONE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/

void rsi_external_inerrupt_handler(void)
{
    /* External interrupt generated */
    Adaptor.pkt_received++;

}

/*FUNCTION*********************************************************************
 Function Name  : rsi_poweron_device
 Description    : This function is used to program the power pins and initializes the SPI master.
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/

int16_t rsi_poweron_device(void) // checked
{
    int16_t status = 0;
    /* Configure the SPI power pins */
    rsi_configure_spi_power_pins();
    /* Initialize the spi master */
    status = rsi_spi_init();
    /* Initialize SPI interface between host and RS22 */
    if (status != RSI_STATUS_SUCCESS) {
        return RSI_STATUS_FAILURE;
    }

    return RSI_STATUS_SUCCESS;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_init_wlan_device
 Description    : This function initializes the WLAN device
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/

int16_t rsi_init_wlan_device(void)
{
    int16_t status;
    uint32_t size;

    /* Initialize Socket data base */
    rsi_init_socket_data_base();

    while ((Adaptor.join_count == 0) && (Adaptor.event_type != JOIN_DONE)) {
        /* Got the slave interrupt */
        if (Adaptor.pkt_received > 0) {
            /* Poll for the interrupt */
            status = rsi_interrupt_handler(Adaptor.RcvPkt, &size);
            if (status != RS22_STATUS_SUCCESS)
                return RS22_JOIN_FAILURE;
        }
    }
    return RSI_STATUS_SUCCESS;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_send_data
 Description    : This function handles the buffer full type events and sends the data to the
 remote peer
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 sockDesc         |  X  |     |     | Socket descriptor of the socket to send on
 len              |  X  |     |     | Length of the data
 data             |  X  |     |     | data

 *END****************************************************************************/
int16_t rsi_send_data(uint32_t sockDesc, uint32_t len, uint8_t *data)
{
    uint32_t InterruptStatus;
    uint32_t size;
    RS22_STATUS status = RS22_STATUS_FAILURE;

    do {
        /* Read the interrupt */
        InterruptStatus = rsi_send_ssp_read_intrStatus_cmd();
        if (InterruptStatus == 0xffff) {
            continue;
        }
        if (InterruptStatus & ~(0xD3)) {
            Adaptor.pkt_received++;
            rsi_interrupt_handler(Adaptor.RcvPkt, &size);
        }
    } while ((InterruptStatus & 0x01) == 1);

    if (!(InterruptStatus & 0x01) && !(((ps_mode == 1) && (is_pwr_save_enable
            == FALSE)))) {
        if (rsi_socket_send(sockDesc, len, data) == RSI_STATUS_SUCCESS) {
            status = RSI_STATUS_SUCCESS;
        }
        return status;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_configure_spi_power_pins
 Description    : This function is is used to program the spi power pins
 Returned Value :
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/

void rsi_configure_spi_power_pins(void)
{
    RedpineRS9110_Internal_ResetPower();
}

void rsi_spi_MasterInit(void)
{
    // Do nothing
}
/* $EOF */
/* Log */

