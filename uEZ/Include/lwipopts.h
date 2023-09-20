/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
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
#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#define TCPIP_THREAD_NAME               "lwIP"
#define TCPIP_THREAD_STACKSIZE          24576//16384
#define TCPIP_THREAD_PRIO               3

#define DEFAULT_THREAD_STACKSIZE        4096
#define DEFAULT_THREAD_PRIO             1

/**
 * MEMP_OVERFLOW_CHECK: memp overflow protection reserves a configurable
 * amount of bytes before and after each memp element in every pool and fills
 * it with a prominent default value.
 *    MEMP_OVERFLOW_CHECK == 0 no checking
 *    MEMP_OVERFLOW_CHECK == 1 checks each element when it is freed
 *    MEMP_OVERFLOW_CHECK >= 2 checks each element in every pool every time
 *      memp_malloc() or memp_free() is called (useful but slow!)
 */

#ifdef DEBUG // debug build only settings
#define MEMP_OVERFLOW_CHECK             0 // This flag should be 0 for releases!
//#define MEMP_SANITY_CHECK             0 // TODO for testing
//#define LWIP_NOASSERT                 0 // removing this include LWIP_ASSERT calls

// Due to the way the code is written to disable debug you need to comment out this line.
// If you set this line to LWIP_DBG_OFF the debug code will still be built into the executable!
// I won't rewrite all of that code as it will diverge from upstream and conflict every release.
//#define LWIP_DEBUG                      LWIP_DBG_ON

#else // release build only settings
#define MEMP_OVERFLOW_CHECK             0 // disable overflow check
//#define MEMP_SANITY_CHECK             0 // TODO for testing
#define LWIP_NOASSERT                   0 // The existance of this define (any value) disables the asserts from being included

//#define LWIP_DEBUG                      LWIP_DBG_ON // Don't include the debug printout code

#endif

#define DBG_TYPES_ON                    0xff
#ifdef LWIP_DEBUG
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#define NETIF_DEBUG                     LWIP_DBG_ON
#define PBUF_DEBUG                      LWIP_DBG_ON
#define API_LIB_DEBUG                   LWIP_DBG_OFF
#define API_MSG_DEBUG                   LWIP_DBG_OFF
#define SOCKETS_DEBUG                   LWIP_DBG_ON
#define ICMP_DEBUG                      LWIP_DBG_ON
#define IGMP_DEBUG                      LWIP_DBG_OFF
#define INET_DEBUG                      LWIP_DBG_OFF
#define IP_DEBUG                        LWIP_DBG_ON
#define IP_REASS_DEBUG                  LWIP_DBG_ON
#define RAW_DEBUG                       LWIP_DBG_ON
#define MEM_DEBUG                       LWIP_DBG_OFF
#define MEMP_DEBUG                      LWIP_DBG_ON
#define SYS_DEBUG                       LWIP_DBG_OFF
#define TCP_DEBUG                       LWIP_DBG_OFF
#define TCP_INPUT_DEBUG                 LWIP_DBG_ON
#define TCP_FR_DEBUG                    LWIP_DBG_OFF
#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
#define TCP_WND_DEBUG                   LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG                LWIP_DBG_ON
#define TCP_RST_DEBUG                   LWIP_DBG_OFF
#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF
#define UDP_DEBUG                       LWIP_DBG_OFF
#define TCPIP_DEBUG                     LWIP_DBG_OFF
#define PPP_DEBUG                       LWIP_DBG_OFF
#define SLIP_DEBUG                      LWIP_DBG_OFF
#define DHCP_DEBUG                      LWIP_DBG_OFF
#define AUTOIP_DEBUG                    LWIP_DBG_OFF
#define SNMP_MSG_DEBUG                  LWIP_DBG_OFF
#define SNMP_MIB_DEBUG                  LWIP_DBG_OFF
#define DNS_DEBUG                       LWIP_DBG_OFF

/* ---------- Statistics options ---------- */
#define LWIP_STATS              0
#define LWIP_STATS_DISPLAY      0

#if LWIP_STATS
#define LINK_STATS              1
#define IP_STATS                1
#define ICMP_STATS              1
#define IGMP_STATS              1
#define IPFRAG_STATS            1
#define UDP_STATS               1
#define TCP_STATS               1
#define MEM_STATS               1
#define MEMP_STATS              1
#define PBUF_STATS              1
#define SYS_STATS               1
#endif /* LWIP_STATS */

#else
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#define NETIF_DEBUG                     LWIP_DBG_OFF
#define PBUF_DEBUG                      LWIP_DBG_OFF
#define API_LIB_DEBUG                   LWIP_DBG_OFF
#define API_MSG_DEBUG                   LWIP_DBG_OFF
#define SOCKETS_DEBUG                   LWIP_DBG_OFF
#define ICMP_DEBUG                      LWIP_DBG_OFF
#define IGMP_DEBUG                      LWIP_DBG_OFF
#define INET_DEBUG                      LWIP_DBG_OFF
#define IP_DEBUG                        LWIP_DBG_OFF
#define IP_REASS_DEBUG                  LWIP_DBG_OFF
#define RAW_DEBUG                       LWIP_DBG_OFF
#define MEM_DEBUG                       LWIP_DBG_OFF
#define MEMP_DEBUG                      LWIP_DBG_OFF
#define SYS_DEBUG                       LWIP_DBG_OFF
#define TCP_DEBUG                       LWIP_DBG_OFF
#define TCP_INPUT_DEBUG                 LWIP_DBG_OFF
#define TCP_FR_DEBUG                    LWIP_DBG_OFF
#define TCP_RTO_DEBUG                   LWIP_DBG_OFF
#define TCP_CWND_DEBUG                  LWIP_DBG_OFF
#define TCP_WND_DEBUG                   LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG                LWIP_DBG_OFF
#define TCP_RST_DEBUG                   LWIP_DBG_OFF
#define TCP_QLEN_DEBUG                  LWIP_DBG_OFF
#define UDP_DEBUG                       LWIP_DBG_OFF
#define TCPIP_DEBUG                     LWIP_DBG_OFF
#define PPP_DEBUG                       LWIP_DBG_OFF
#define SLIP_DEBUG                      LWIP_DBG_OFF
#define DHCP_DEBUG                      LWIP_DBG_OFF
#define AUTOIP_DEBUG                    LWIP_DBG_OFF
#define SNMP_MSG_DEBUG                  LWIP_DBG_OFF
#define SNMP_MIB_DEBUG                  LWIP_DBG_OFF
#define DNS_DEBUG                       LWIP_DBG_OFF

/* ---------- Statistics options ---------- */
#define LWIP_STATS              0
#define LWIP_STATS_DISPLAY      0

#if LWIP_STATS
#define LINK_STATS              1
#define IP_STATS                1
#define ICMP_STATS              1
#define IGMP_STATS              1
#define IPFRAG_STATS            1
#define UDP_STATS               1
#define TCP_STATS               1
#define MEM_STATS               1
#define MEMP_STATS              1
#define PBUF_STATS              1
#define SYS_STATS               1
#endif /* LWIP_STATS */

#endif

/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT            1

/**
 * MEM_ALIGNMENT: should be set to the alignment of the CPU
 *    4 byte alignment -> #define MEM_ALIGNMENT 4
 *    2 byte alignment -> #define MEM_ALIGNMENT 2
 */
#define MEM_ALIGNMENT                   4 // We require word alignment on most MCU AHB access

// performance/thread options

// These 3 seem to be required to fix some stabiilty issues, also required for full duplex com.
#define LWIP_FREERTOS_CHECK_CORE_LOCKING  1
#define LWIP_TCPIP_CORE_LOCKING           1
#define LWIP_NETCONN_SEM_PER_THREAD       1 // Testing this to help prevent some crashing during flush routines. Required for planned future usage.

//#define LWIP_NETCONN_FULLDUPLEX         1 // requires per thread sem

//#define LWIP_TCPIP_TIMEOUT              1

/* ------------------------------------------------
   ---------- Internal Memory Pool Sizes ----------
   ------------------------------------------------ */
// NOTE: See lwippools.h to config the size and number of each pool!
// Can override some of these in library project Config_Build.h.

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 * This is not used with MEMP pools so instead change the settings below!
 */
#define MEM_SIZE                        (256*1024) // not used with custom buffer pools

/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#define MEMP_NUM_PBUF                   140 // Note: To run some LWIP conformance tests from upstream they need 200+ PBUFs.

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#define MEMP_NUM_UDP_PCB                8

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_PCB
#define MEMP_NUM_TCP_PCB                60
#endif

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#ifndef MEMP_NUM_TCP_PCB_LISTEN
#define MEMP_NUM_TCP_PCB_LISTEN         20
#endif

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_SEG                32 // 8

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simulateously active timeouts.
 * (requires NO_SYS==0)
 */
#define MEMP_NUM_SYS_TIMEOUT            20

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifndef MEMP_NUM_NETBUF
#define MEMP_NUM_NETBUF                 20 // 4
#endif

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETCONN                50

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication. 
 * (only needed if you use tcpip.c)
 */
#ifndef MEMP_NUM_TCPIP_MSG_API
#define MEMP_NUM_TCPIP_MSG_API          20
#endif

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool. 
// */

#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE                  MEMP_NUM_PBUF
#endif

#ifndef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE                 MEMP_NUM_PBUF
#endif

#define MEMP_NUM_TCPIP_MSG_INPKT        MEMP_NUM_PBUF

/**
 * PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. The default is
 * designed to accomodate single full size TCP frame in one pbuf, including
 * TCP_MSS, IP header, and link header.
 */

#ifndef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE               1520 //1460(MSS)+40+14+4+2 (VLAN and padding included)
//#define PBUF_POOL_BUFSIZE               1522 //1460(MSS)+40+14+4+4 (VLAN and/or FCS included)
#endif

//#define MEM_USE_POOLS_TRY_BIGGER_POOL   0 // TODO test

//#define LWIP_IPV4                       1 // not required to be included
//#define LWIP_IPV6                       1 // TOOD

/*
   ---------------------------------
   ---------- TCP options ----------
   ---------------------------------
*/
/**
 * LWIP_TCP==1: Turn on TCP.
 */
#define TCP_TMR_INTERVAL                250
#define LWIP_TCP                        1

/* TCP Maximum segment size. */
#define TCP_MSS                         1500

/* TCP sender buffer space (bytes). */
#define TCP_SND_BUF                     TCP_MSS*2

/**
 * TCP_WND: The size of a TCP receive window.
 */
//#define TCP_WND                         TCP_MSS*2
#define TCP_WND                        (20 * 1024)

/**
 * TCP_MAXRTX: Maximum number of retransmissions of data segments.
 */
#define TCP_MAXRTX                      12

/**
 * TCP_SYNMAXRTX: Maximum number of retransmissions of SYN segments.
 */
#define TCP_SYNMAXRTX                   4

#ifndef LWIP_DNS
#define LWIP_DNS                        1
#endif

// Turn on SNMP, say we are using the .private section, and declare some memory for it
#define LWIP_SNMP                       0//1
#define SNMP_PRIVATE_MIB                1
#define LWIP_SNMP_NUM_VARS              200

#define LWIP_SO_RCVTIMEO                1

#define LWIP_PROVIDE_ERRNO              1

#define BYTE_ORDER                      LITTLE_ENDIAN

#define NO_SYS                          0

#define LWIP_SOCKET                     1

/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define LWIP_UDPLITE            LWIP_UDP
#define UDP_TTL                 255

// TODO
//#define MIB2_STATS                 LWIP_SNMP
#ifdef LWIP_HAVE_MBEDTLS
#define LWIP_SNMP_V3               (LWIP_SNMP)
#endif

//#define LWIP_IGMP                  LWIP_IPV4
//#define LWIP_ICMP                  LWIP_IPV4
/* ---------- ICMP options ---------- */
//#define ICMP_TTL                255

// TODO
//#define LWIP_NETIF_LINK_CALLBACK        1
//#define LWIP_NETIF_STATUS_CALLBACK      1
//#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

/* ---------- ARP options ---------- */
// TODO
//#define LWIP_ARP                1
//#define ARP_TABLE_SIZE          10
//#define ARP_QUEUEING            1

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP               LWIP_UDP

/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#define DHCP_DOES_ARP_CHECK    (LWIP_DHCP)

/* ---------- AUTOIP options ------- */
#define LWIP_AUTOIP            (LWIP_DHCP)
#define LWIP_DHCP_AUTOIP_COOP  (LWIP_DHCP && LWIP_AUTOIP)

#if LWIP_2_0_x
// for LWIP2 you need both normal and custom pools enabled!
#define MEM_USE_POOLS                   1 
#define MEMP_USE_CUSTOM_POOLS           1

#define htons(x)                        lwip_htons(x)
#include "Source/Library/lwip_2.0.x/system/arch/sys_arch.h"
#define LWIP_RAND()                     ((u32_t)GetRandomNumber())
/*----- Value in opt.h for CHECKSUM_GEN_UDP: 1 -----*/
#define CHECKSUM_GEN_UDP                0
/*----- Value in opt.h for CHECKSUM_GEN_ICMP: 1 -----*/
#define CHECKSUM_GEN_ICMP               0
/*----- Value in opt.h for CHECKSUM_CHECK_IP: 1 -----*/
#define CHECKSUM_CHECK_IP               0
/*----- Value in opt.h for CHECKSUM_CHECK_UDP: 1 -----*/
#define CHECKSUM_CHECK_UDP              0
/*----- Value in opt.h for CHECKSUM_CHECK_TCP: 1 -----*/
#define CHECKSUM_CHECK_TCP              0

#define LWIP_CHKSUM_ALGORITHM           3

#define LWIP_ETHERNET                   1
#define LWIP_NETCONN                    1

/** ETH_PAD_SIZE: number of bytes added before the ethernet header to ensure
 * alignment of payload after that header. Since the header is 14 bytes long,
 * without this padding e.g. addresses in the IP header will not be aligned
 * on a 32-bit boundary, so setting this to 2 can speed up 32-bit-platforms.
 */
//#define ETH_PAD_SIZE                    2 //Currently broken



#else // LWIP1
// for LWIP1 you can only enable 1 or the other pool setting depending on config
#define MEM_USE_POOLS                   1 
#define MEMP_USE_CUSTOM_POOLS           1 

#endif

#endif /* __LWIPOPTS_H__ */
