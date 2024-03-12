/*-------------------------------------------------------------------------*
 * File:  ethernetif.c
 *-------------------------------------------------------------------------*
 * Description:
 *     lwIP Ethernet Interface routines.
 *-------------------------------------------------------------------------*/

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*--------------------------------------------------------------------------
 * This software and associated documentation files (the "Software")
 * are copyright 2008 Future Designs, Inc. All Rights Reserved.
 *
 * A copyright license is hereby granted for redistribution and use of
 * the Software in source and binary forms, with or without modification,
 * provided that the following conditions are met:
 * 	-   Redistributions of source code must retain the above copyright
 *      notice, this copyright license and the following disclaimer.
 * 	-   Redistributions in binary form must reproduce the above copyright
 *      notice, this copyright license and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * 	-   Neither the name of Future Designs, Inc. nor the names of its
 *      subsidiaries may be used to endorse or promote products
 *      derived from the Software without specific prior written permission.
 *
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * 	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * 	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * 	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * 	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * 	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * 	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * 	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * 	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * 	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * 	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-------------------------------------------------------------------------*/
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <uEZ.h>
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"
#if (LWIP_IPV6 == 1)
#include "lwip/ethip6.h"
#endif
#include "arch/lwIP_EMAC.h"
#include "../emac.h"
#include <HAL/Interrupt.h>
#include <HAL/EMAC.h>
#include <string.h>

/*---------------------------------------------------------------------------*
 * Compile time options:
 *---------------------------------------------------------------------------*/
#ifndef INTERRUPT_BASED_EMAC
#define INTERRUPT_BASED_EMAC        0
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#if(UEZ_PROCESSOR == NXP_LPC4357)
#define netifINTERFACE_TASK_STACK_SIZE		(1*512) // This may need to be changed and also is MCU specific and may need to be raised on CM4/CM33 vs older MCUs!
#define netifINTERFACE_TASK_PRIORITY		UEZ_PRIORITY_VERY_HIGH // (configMAX_PRIORITIES - 1)

/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT	((portTickType) 5000)
#elif(UEZ_PROCESSOR == NXP_LPC4088)
#define netifINTERFACE_TASK_STACK_SIZE		(512) // This may need to be changed and also is MCU specific and may need to be raised on CM4/CM33 vs older MCUs!
#define netifINTERFACE_TASK_PRIORITY		UEZ_PRIORITY_VERY_HIGH // (configMAX_PRIORITIES - 1)

/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT	((portTickType) 5000)
#elif(UEZ_PROCESSOR == NXP_LPC1788)
#define netifINTERFACE_TASK_STACK_SIZE		(512) // This may need to be changed and also is MCU specific and may need to be raised on CM4/CM33 vs older MCUs!
#define netifINTERFACE_TASK_PRIORITY		UEZ_PRIORITY_VERY_HIGH // (configMAX_PRIORITIES - 1)

/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT	((portTickType) 5000)
#else
#define netifINTERFACE_TASK_STACK_SIZE		(512) // This may need to be changed and also is MCU specific and may need to be raised on CM4/CM33 vs older MCUs!
#define netifINTERFACE_TASK_PRIORITY		UEZ_PRIORITY_VERY_HIGH // (configMAX_PRIORITIES - 1)

/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT	((portTickType) 5000)
#endif

#define netifGUARD_BLOCK_TIME				(250)

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'

/* How long to wait before attempting to connect the MAC again. */
#define uipINIT_WAIT    UEZ_TIMEOUT_INFINITE // (was 0)

/* EMAC interrupt controller related definition */
#define EMAC_INT_RXDONE		0x01 << 3

T_uezTask G_EthernetIfTask = NULL;

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif {
  struct eth_addr *ethaddr;
  /* Add whatever per-interface state that is needed here. */
};

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static struct netif *xNetIf = NULL;

#if INTERRUPT_BASED_EMAC
    /* The semaphore used by the EMAC ISR to wake the EMAC task. */
    static T_uezSemaphore xEMACSemaphore = 0;
#endif

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
//static void  ethernetif_input(struct netif *netif);
static void ethernetif_input(T_uezTask, void *);
#if INTERRUPT_BASED_EMAC
static void lwIP_EMAC_ReceiveCallback(void *aWorkspace);
#endif
//extern T_uezError NVSettingsGetMACAddress(TUInt8 *aMACAddress);

/*---------------------------------------------------------------------------*
 * Routine:  low_level_init
 *---------------------------------------------------------------------------*
 * Description:
 *      In this function, the hardware is initialized.  Called from
 *      ethernetif_init().
 * Inputs:
 *      struct netif *netif         -- the already initialized lwip network
 *                                      structure for this ethernetif
 *---------------------------------------------------------------------------*/
static void low_level_init(struct netif *netif)
{
#if INTERRUPT_BASED_EMAC
    HAL_EMAC **emac;
#endif
//    struct ethernetif *ethernetif = netif->state;

    /* uEZ v2.04 -- MAC address is NOT set before calling this routine and
     *              ethernetif_init!!!  The netif structure should have fields
     *              hwaddr_len and hwaddr ALREADY set.
     */

//    netif->hwaddr_len = 6;

    /* set MAC hardware address */
//    NVSettingsGetMACAddress(netif->hwaddr);  // netif is now set before being called
//    netif->hwaddr[0] = emacETHADDR0;
//    netif->hwaddr[1] = emacETHADDR1;
//    netif->hwaddr[2] = emacETHADDR2;
//    netif->hwaddr[3] = emacETHADDR3;
//    netif->hwaddr[4] = emacETHADDR4;
//    netif->hwaddr[5] = emacETHADDR5;

    /* maximum transfer unit */
    netif->mtu = ETH_MTU;

  /* Accept broadcast address and ARP traffic */
  /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
#if LWIP_ARP
    //Removed Broadcast to see if it helps reduce traffic seen by the stack
    netif->flags |= /*NETIF_FLAG_BROADCAST |*/ NETIF_FLAG_ETHERNET | NETIF_FLAG_ETHARP;

    //TODO: using a has to test, fix before release
    netif->flags |= NETIF_FLAG_LINK_UP;
#else
    netif->flags |= NETIF_FLAG_BROADCAST;
#endif /* LWIP_ARP */
    
    xNetIf = netif;

    /* Do whatever else is needed to initialize interface. */

    /* Initialise the MAC. */


#if INTERRUPT_BASED_EMAC
    while(Init_EMAC(NATIONAL_PHY) != pdPASS)
    {
        UEZTaskDelay(uipINIT_WAIT);
    }

    UEZSemaphoreCreateBinary(&xEMACSemaphore);
#if UEZ_REGISTER
    UEZSemaphoreSetName(xEMACSemaphore, "EMAC_Int", "\0");
#endif
    if(xEMACSemaphore) {
        /* We start by 'taking' the semaphore so the ISR can 'give' it when the
        first interrupt occurs. */
        UEZSemaphoreGrab(xEMACSemaphore, 0);

        // Setup the EMAC to enable its receive interrupts and callback
        if (HALInterfaceFind("EMAC", (T_halWorkspace **)&emac) == UEZ_ERROR_NONE) {
            (*emac)->EnableReceiveInterrupt(emac, lwIP_EMAC_ReceiveCallback, 0);
        }
    }
#else
    // TBD: Technically, we detect the type of phy, so the type specified is not necessary
    while(Init_EMAC(NATIONAL_PHY) != pdPASS)
    {
        UEZTaskDelay(uipINIT_WAIT);
    }
#endif

    if(G_EthernetIfTask != NULL) { // don't create duplicate task if restarting interface
      UEZTaskDelete(G_EthernetIfTask);
    }

    /* Create the task that handles the EMAC. */
    UEZTaskCreate(
        (T_uezTaskFunction)ethernetif_input,
        "ETH_INT",
        netifINTERFACE_TASK_STACK_SIZE,
        NULL,
        netifINTERFACE_TASK_PRIORITY,
        &G_EthernetIfTask);
}

/*---------------------------------------------------------------------------*
 * Routine:  low_level_output
 *---------------------------------------------------------------------------*
 * Description:
 *      This function does the actual transmission of a packet.  A packet
 *      is contained in the passed in pbuf variable and may be chained.
 * Notes:
 *      Returning ERR_MEM on a full DMA queue can lead to strange results.
 *      Consider waiting for space in the DMA queue to become available
 *      since the stack does not retyr to send a packet dropped because
 *      of memory failure (except for TCP timers).
 * Inputs:
 *      struct netif *netif         -- lwIP network interface structure for
 *                                      for this ethernetif.
 *      struct pbuf *p              -- the MAC packet to send (e.g. IP packet
 *                                      including MAC addresses and type).
 * Outputs:
 *      err_t                       -- ERR_OK if the packet could be sent, or
 *                                      err_t value for error code.
 *---------------------------------------------------------------------------*/

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    static char packet[PBUF_POOL_BUFSIZE];
//    struct ethernetif *ethernetif = netif->state;
    static T_uezSemaphore xTxSemaphore = 0;
    struct pbuf *q;
    err_t xReturn = ERR_OK;
    int32_t txsize = 0;

#if ETH_PAD_SIZE
    pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif

    // Create semaphore if not already created
    if (xTxSemaphore == 0)
        UEZSemaphoreCreateBinary(&xTxSemaphore);

    // Access to the EMAC is guarded using a semaphore.
    if (UEZSemaphoreGrab(xTxSemaphore, netifGUARD_BLOCK_TIME) == UEZ_ERROR_NONE) {

        // Only one fragment, can send it directly */
        if (!p->next){
            if(!lEMACSend(p->payload, p->len, 0)){
                xReturn = ERR_MEM;
            }
        } else { //chain pbuf messages, needs to be merged.
            for (q=p; q!=NULL; q=q->next) {
              if (txsize + q->len > ETH_FRAG_SIZE){
                  //Add an error message
                  //LWIP_ASSERT("oversized packet, fragment %d txsize %d\n", q->len, txsize);
              }
              memcpy(&packet[txsize], q->payload, q->len);
              txsize += q->len;
            }

            // Send the data from the pbuf to the interface, one pbuf at a
            // time.  The size of the data in each pbuf is in ->lne.
            // If q->next is NULL then this is the last bbuf in the chain.
            // if (!lEMACSend(q->payload, q->len, (q->next == NULL)))
            if(!lEMACSend(packet, txsize, 0)){
                xReturn = ERR_MEM;
            }
        }

        // TODO add MIB2 stats here if needed: netif add, multicast and unicast packets
        UEZSemaphoreRelease(xTxSemaphore);
    }

#if ETH_PAD_SIZE
    pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return xReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  low_level_input
 *---------------------------------------------------------------------------*
 * Description:
 *      Allocates a pbuf and transfers the incoming packet to the pbuf.
 * Inputs:
 *      struct netif *netif         -- lwIP network interface structure for
 *                                      for this ethernetif.
 * Outputs:
 *      struct pbuf*                -- pbuf filled with received packet
 *                                      (including MAC header) or NULL on
 *                                      memory error.
 *---------------------------------------------------------------------------*/
static struct pbuf* low_level_input(struct netif *netif)
{
    struct pbuf       *p, *q;
    uint32_t      len;
    uint32_t      cpyix;
    uint32_t      cpylen;
    UEZ_PARAMETER_NOT_USED(cpyix);

    // Packet ready?
    if (CheckFrameReceived() == 0)
        return NULL;

    // How long is it?
    len = StartReadFrame();

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; // allow room for Ethernet padding
#endif

    // Allocate a pbuf chain of pbufs from the pool.
    if(len){
        p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    } else {
        p = NULL;
    }

    // Was the memory allocated?
    if (p != NULL) {
#if ETH_PAD_SIZE
        pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif

        // Iterate over the pbuf chain until the enter packet is read.
        cpyix  = 0;

        for(q=p; q!=NULL; q=q->next) {
            // Read enough bytes to fill this pbuf in the chain.
            // The available data in the pbuf is given by the q->len variable.
            if (q->len >= len)
                cpylen = len;
            else
                cpylen = q->len;

            CopyFromFrame_EMAC(q->payload, cpylen);
            len -= cpylen;
            cpyix += cpylen;
        }
        EndReadFrame();
        
#if LINK_STATS
        MIB2_STATS_NETIF_ADD(netif, ifinoctets, p->tot_len);
        if (((u8_t *)p->payload)[0] & 1) {
          /* broadcast or multicast packet*/
          MIB2_STATS_NETIF_INC(netif, ifinnucastpkts);
        } else {
          /* unicast packet*/
          MIB2_STATS_NETIF_INC(netif, ifinucastpkts);
        }
        LINK_STATS_INC(link.recv);
#endif /* LINK_STATS */

#if ETH_PAD_SIZE
        pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
    } else {
        // Drop packet because no free pbuf available
        EndReadFrame();

#if LINK_STATS
    LINK_STATS_INC(link.memerr);
    LINK_STATS_INC(link.drop);
        MIB2_STATS_NETIF_INC(netif, ifindiscards);
#endif /* LINK_STATS */
    }

    return p;
}

/*---------------------------------------------------------------------------*
 * Routine:  ethernetif_input
 *---------------------------------------------------------------------------*
 * Description:
 *      This function is called when a packet is ready to be read from
 *      the interface.  It uses the function low_level_input() that
 *      handles the actual reception of bytes from the network interface.
 *      Then the type of the received packet is determined and the
 *      appropraite input function is called.
 * Inputs:
 *      struct netif *netif         -- lwIP network interface structure for
 *                                      for this ethernetif.
 *---------------------------------------------------------------------------*/
static void ethernetif_input(T_uezTask aTask, void *pvParameters)
{
    struct pbuf *p;
    ( void ) aTask;
    ( void ) pvParameters;

    for(;;){
#if INTERRUPT_BASED_EMAC
        UEZSemaphoreGrab(xEMACSemaphore, emacBLOCK_TIME_WAITING_FOR_INPUT);
#else
        UEZTaskDelay(1);
#endif //INTERRUPT_BASED_EMAC
        do {
            p = low_level_input(xNetIf);
            if (p != NULL) {
                if (xNetIf->input(p, xNetIf) != ERR_OK) {
                    pbuf_free(p);
                }
            }
        } while (p != NULL);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ethernetif_init
 *---------------------------------------------------------------------------*
 * Description:
 *      Set up the network interface.  Calls low_level_init() to do the
 *      actual setup of the hardware.
 * Inputs:
 *      struct netif *netif         -- lwIP network interface structure for
 *                                      for this ethernetif.
 * Outputs:
 *      err_t                       -- ERR_OK if the loopif is initialized.
 *                                      ERR_MEM If private data could not
 *                                      be allocated.  Any other err_t for
 *                                      any other error.
 *---------------------------------------------------------------------------*/
err_t ethernetif_init(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));

    ethernetif = mem_malloc(sizeof(struct ethernetif));
    if (ethernetif == NULL) {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
    * Initialize the snmp variables and counters inside the struct netif.
    * The last argument should be replaced with your link speed, in units
    * of bits per second.
    */ // This is the same as MIB2_INIT_NETIF
    NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, (100*1000*1000));

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

    /* We directly use etharp_output() here to save a function call.
    * You can instead declare your own function an call etharp_output()
    * from it if you have to do some checks before sending (e.g. if link
    * is available...) */

    /** IPV4-> Called by lwIP when a packet shall be sent. 
    For ethernet netif, set this to 'etharp_output' and set 'linkoutput'. */
    netif->output = etharp_output;

#if (LWIP_IPV6 == 1)
    /** IPV6-> Called by lwIP when a packet shall be sent.
    For ethernet netif, set this to 'ethip6_output' and set 'linkoutput'. */
    netif->output_ip6 = ethip6_output;
#endif

    netif->linkoutput = low_level_output;

    ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

#if LWIP_IPV6_MLD
    netif->flags |= NETIF_FLAG_MLD6;
    netif->mld_mac_filter = mld_mac_filter; //todo
#endif

    /* initialize the hardware */
    low_level_init(netif);

#if LWIP_IPV6_MLD
    // we need to listen to the all nodes multicast address for route advertisements
    ip6_addr_t ip6_allnodes_ll;
    ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
    netif->mld_mac_filter(netif, &ip6_allnodes_ll, NETIF_ADD_MAC_FILTER);
#endif

    return ERR_OK;
}

#if INTERRUPT_BASED_EMAC
static void lwIP_EMAC_ReceiveCallback(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);
    _isr_UEZSemaphoreRelease(xEMACSemaphore);
    _isr_UEZTaskContextSwitch();
}
#endif // INTERRUPT_BASED_EMAC

/*-------------------------------------------------------------------------*
 * End of File:  ethernetif.c
 *-------------------------------------------------------------------------*/
