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

/* On uEZGUI for maximum http server performance use the following settings: Jumbo frames on LPC4357, release build optimime 1
* USB host disabled, disable RTT and RTOS stats, disable SNMP, IGMP, disable IPV6, download file from QSPI or SDRAM, not SD card. */

#include <Config.h>

#define LWIP_IPV4                       1 // include so that at least #defines show up properly enabled in IDEs
#define LWIP_IPV6                       0 // TODO needs working multicast for address setup code - need MAC hash acceptance
#define LWIP_SNMP                       1 // Moved up here to allow for turning on stats automatically.

#if (LWIP_IPV6 == 1)
#define LWIP_IPV6_DHCP6                   1
#define LWIP_IPV6_SCOPES                  1
#define LWIP_IPV6_SCOPES_DEBUG            1
#define LWIP_ND6                          1
//#define ETHARP_TABLE_MATCH_NETIF        0
//#define LWIP_IPV6_FORWARD               0
#define LWIP_IPV6_FRAG                    1
#define LWIP_IPV6_REASS                   1
#define LWIP_IPV6_MLD                     0 // need hash table driver for multicast

#define IP6_EXTRA_TASK_BYTES 256 // TODO figure out how much extra is needed if any. There is a small RAM increase in a few places.
#else
#define IP6_EXTRA_TASK_BYTES 0
#endif

#if(UEZ_PROCESSOR == NXP_LPC4357)
#define TCPIP_THREAD_STACKSIZE          (2048+IP6_EXTRA_TASK_BYTES) // esp examples show 2048 and above, extra is needed for some debug code
#define TCPIP_THREAD_PRIO               5

#define DEFAULT_THREAD_STACKSIZE        (2048+IP6_EXTRA_TASK_BYTES)
#define DEFAULT_THREAD_PRIO             5

#if(EMAC_ENABLE_JUMBO_FRAME == 1)
#define TCP_MSS                         8128 // TCP Maximum segment size. - Buffer limit prevents 9K
#endif

#elif(UEZ_PROCESSOR == NXP_LPC4088)
#define TCPIP_THREAD_STACKSIZE          (2048+IP6_EXTRA_TASK_BYTES)
#define TCPIP_THREAD_PRIO               3

#define DEFAULT_THREAD_STACKSIZE        (2048+IP6_EXTRA_TASK_BYTES)
#define DEFAULT_THREAD_PRIO             1

#if(EMAC_ENABLE_JUMBO_FRAME == 1)
#define TCP_MSS                         2000 // TCP Maximum segment size. - Limited AHB for really large buffers, also segment limit
#endif

#elif(UEZ_PROCESSOR == NXP_LPC1788)
#define TCPIP_THREAD_STACKSIZE          (2048+IP6_EXTRA_TASK_BYTES)
#define TCPIP_THREAD_PRIO               3

#define DEFAULT_THREAD_STACKSIZE        (2048+IP6_EXTRA_TASK_BYTES)
#define DEFAULT_THREAD_PRIO             1

#if(EMAC_ENABLE_JUMBO_FRAME == 1)
#define TCP_MSS                         2000//3840 // TCP Maximum segment size. - Limited AHB for really large buffers, also segment limit
#endif

#else
#define TCPIP_THREAD_STACKSIZE          (2048+IP6_EXTRA_TASK_BYTES)
#define TCPIP_THREAD_PRIO               3

#define DEFAULT_THREAD_STACKSIZE        (2048+IP6_EXTRA_TASK_BYTES)
#define DEFAULT_THREAD_PRIO             1

#if(EMAC_ENABLE_JUMBO_FRAME == 1)
#define TCP_MSS                         4032 // TCP Maximum segment size. - unknown MCU limit
#endif

#endif

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
#define MEM_OVERFLOW_CHECK              0
//#define MEMP_SANITY_CHECK               1 // TODO for testing
//#define MEM_SANITY_CHECK                1

#ifdef CONFIG_LWIP_ASSERT // define must be removed (not 0) to remove asserts
#define LWIP_NOASSERT                   1 // removing this include LWIP_ASSERT calls
#endif

// Due to the way the code is written to disable debug you need to comment out this line.
// If you set this line to LWIP_DBG_OFF the debug code will still be built into the executable!
// I won't rewrite all of that code as it will diverge from upstream and conflict every release.
//#define LWIP_DEBUG                      LWIP_DBG_ON // Always check this line in commented out.

#else // release build only settings
#define MEMP_OVERFLOW_CHECK             0 // disable overflow check
//#define MEMP_SANITY_CHECK             0 // TODO for testing
#define LWIP_NOASSERT                   0 // The existance of this define (any value) disables the asserts from being included

//#define LWIP_DEBUG                      LWIP_DBG_ON // Don't include the debug printout code

#endif

#define DBG_TYPES_ON                    0xff
#ifdef LWIP_DEBUG
#define ETHARP_DEBUG                    LWIP_DBG_OFF
#define NETIF_DEBUG                     LWIP_DBG_OFF
#define PBUF_DEBUG                      LWIP_DBG_OFF
#define API_LIB_DEBUG                   LWIP_DBG_ON
#define API_MSG_DEBUG                   LWIP_DBG_ON
#define SOCKETS_DEBUG                   LWIP_DBG_OFF
#define ICMP_DEBUG                      LWIP_DBG_OFF
#define IGMP_DEBUG                      LWIP_DBG_OFF
#define INET_DEBUG                      LWIP_DBG_OFF
#define IP_DEBUG                        LWIP_DBG_OFF
#define IP_REASS_DEBUG                  LWIP_DBG_OFF
#define RAW_DEBUG                       LWIP_DBG_OFF
#define MEM_DEBUG                       LWIP_DBG_OFF
#define MEMP_DEBUG                      LWIP_DBG_OFF
#define SYS_DEBUG                       LWIP_DBG_ON
#define THREAD_SAFE_DEBUG               LWIP_DBG_ON
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
#define SNMP_MSG_DEBUG                  LWIP_DBG_ON
#define SNMP_MIB_DEBUG                  LWIP_DBG_ON
#define DNS_DEBUG                       LWIP_DBG_OFF
#define DHCP6_DEBUG                     LWIP_DBG_OFF
#define SNTP_DEBUG                      LWIP_DBG_ON

/* ---------- Statistics options ---------- */
#if (LWIP_SNMP == 1)
#define LWIP_STATS              1
#else
#define LWIP_STATS              0
#endif

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
#if (LWIP_SNMP == 1)
#define LWIP_STATS              1
#else
#define LWIP_STATS              0
#endif
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

// These 3 are required to for several features, also required for full duplex com and performance improvement.
// We have separate TCP thread and network startup thread so these are critical to prevent accessing the same resources.
// Any application use also needs to check that it isn't stepping on tcp thread.
// sys_arch.c is setup to use FreeRTOS per thread sem for these features, following either lwip or esp examples
#define LWIP_TCPIP_CORE_LOCKING           1 // Required for several features to work.
#define LWIP_NETCONN_SEM_PER_THREAD       1 // Required for several features to work.
#define LWIP_FREERTOS_CHECK_CORE_LOCKING  1 // Make sure to use LOCK_TCPIP_CORE() and UNLOCK_TCPIP_CORE() if you hit core check assert.
#define LWIP_NETCONN_FULLDUPLEX           1 // Requires per thread sem

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
// Note: To run some LWIP conformance tests from upstream they need 200+ PBUFs.
#define MEMP_NUM_PBUF                   48 // With custom pbuf pool this doesn't affect LWIP_MEMPOOL size, only some below settings.
// In lwippools.h we can have over 100 pbufs which is more than set here.
// With 32 each of 6 different sizes, we have have almost 200 pbufs as an example: (1536+1024+512+256+128+64)*32=112.64KB

#define MEMP_NUM_RAW_PCB                16
#define IP_REASS_MAX_PBUFS              16

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#define MEMP_NUM_UDP_PCB                16

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
#define MEMP_NUM_TCP_SEG                128//64//32 // 8

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simulateously active timeouts.
 * (requires NO_SYS==0)
 */
#define MEMP_NUM_SYS_TIMEOUT            20 // Need to be changed to macro?

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#ifndef MEMP_NUM_NETBUF
#define MEMP_NUM_NETBUF                 32//20 // 4
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
#define MEMP_NUM_TCPIP_MSG_API          32//20
#endif

#define MEMP_NUM_REASSDATA              10
#define MEMP_NUM_FRAG_PBUF              30////15

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool. 
// */

#ifndef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE                  MEMP_NUM_PBUF
#endif

// If MBOX sizes are not defined, it will default to 0 and cause semaphores to not be initialized!
#ifndef TCPIP_MBOX_SIZE
#define TCPIP_MBOX_SIZE                 MEMP_NUM_PBUF
#endif

#ifndef DEFAULT_ACCEPTMBOX_SIZE
#define DEFAULT_ACCEPTMBOX_SIZE         TCPIP_MBOX_SIZE
#endif

#ifndef DEFAULT_UDP_RECVMBOX_SIZE
#define DEFAULT_UDP_RECVMBOX_SIZE       TCPIP_MBOX_SIZE
#endif

#ifndef DEFAULT_TCP_RECVMBOX_SIZE
#define DEFAULT_TCP_RECVMBOX_SIZE       TCPIP_MBOX_SIZE
#endif

#ifndef DEFAULT_RAW_RECVMBOX_SIZE
#define DEFAULT_RAW_RECVMBOX_SIZE       TCPIP_MBOX_SIZE
#endif

#ifndef MEMP_NUM_TCPIP_MSG_INPKT
#define MEMP_NUM_TCPIP_MSG_INPKT        TCPIP_MBOX_SIZE
#endif

/**
 * PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. The default is
 * designed to accomodate single full size TCP frame in one pbuf, including
 * TCP_MSS, IP header, and link header.
 */

#if(EMAC_ENABLE_JUMBO_FRAME == 1)
#ifndef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE               8192 
#endif
#else
#ifndef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE               1520 //1460(MSS)+40+14+4+2 (VLAN and padding included)
//#define PBUF_POOL_BUFSIZE               1522 //1460(MSS)+40+14+4+4 (VLAN and/or FCS included)
#endif
#endif

#define MEM_USE_POOLS_TRY_BIGGER_POOL     1 // Seems to be working as expected

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

// If you see a late ACK causing TCP early socket close you probably need to turn this on. This will set TF_NODELAY flag on PCB.
#define UEZ_LWIP_DISABLE_NAGLE          1 // if 1 call tcp_nagle_disable on created socket pcbs, binded sockets, and socket connect 

#define LWIP_TCP_SACK_OUT               1
#define LWIP_TCP_MAX_SACK_NUM           6
#define LWIP_TCP_RTO_TIME               5000

#if(EMAC_ENABLE_JUMBO_FRAME == 1) // adjust the total send buffer space based on how large segment size is
#define TCP_SND_BUF                     TCP_MSS*2 // TCP sender buffer space (bytes).

#else // standard frame size here, same on all MCUs we support
#define TCP_MSS                         1500 /* TCP Maximum segment size. */
#define TCP_SND_BUF                     TCP_MSS*12 // TCP sender buffer space (bytes).
#endif

/**
 * TCP_WND: The size of a TCP receive window.
 */
#if(EMAC_ENABLE_JUMBO_FRAME == 1)
#define TCP_WND                        (32768)// This is plenty to run AWS TLS demos
//#define TCP_WND                      (49152) // can go higher, but this does increase memory in send queue length

#define TCP_SND_QUEUELEN                ((8 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS)) // We need extra to run TLS handshaking
//#define TCP_SND_QUEUELEN                ((16 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS)) // If TCP window was smaller you need more snd queues

#else // no jumber frames, calculations based on MSS value will be different
//#define TCP_WND                         TCP_MSS*2 // minimum size
#define TCP_WND                        (32768)// This is plenty to run AWS TLS demos

#define TCP_SND_QUEUELEN                ((8 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS)) // We need extra to run TLS handshaking
//#define TCP_SND_QUEUELEN                ((16 * (TCP_SND_BUF) + (TCP_MSS - 1))/(TCP_MSS)) // If TCP window was smaller you need more snd queues
#endif

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
#define SNMP_PRIVATE_MIB                1
#define LWIP_SNMP_NUM_VARS              200
#define LWIP_MIB2_CALLBACKS             0 // TODO implement callbacks
#define LWIP_SNMP_V3                   (LWIP_SNMP) // V3 might still be experimental, requires mbedtls
#if (LWIP_STATS == 1)
#define MIB2_STATS                      LWIP_SNMP
#endif

//#define LWIP_TCPIP_TIMEOUT              1 // use RTOS timers for timeout TODO test
#define LWIP_SO_RCVTIMEO                1 // enable socket/netcomnn timeout processing for recv
#define LWIP_SO_SNDTIMEO                1 // enable socket/netcomnn timeout processing for send

#define LWIP_PROVIDE_ERRNO              1

#define BYTE_ORDER                      LITTLE_ENDIAN

#define NO_SYS                          0

#define LWIP_SOCKET                     1

#ifdef DEBUG // debug build only settings
    #ifndef TCP_CHECKSUM_ON_COPY_SANITY_CHECK
    #define TCP_CHECKSUM_ON_COPY_SANITY_CHECK   1
    #endif
#else // release build only settings
    #ifndef TCP_CHECKSUM_ON_COPY_SANITY_CHECK
    #define TCP_CHECKSUM_ON_COPY_SANITY_CHECK   0 // could remove on release build for speedup
    #endif
#endif

/* ---------- UDP options ---------- */
#define LWIP_UDP                1
#define LWIP_UDPLITE            LWIP_UDP
#define UDP_TTL                 255

//#define LWIP_IGMP                  LWIP_IPV4 // multiple devices, same IPV4 address, small performance decrease
// For IPV6 would need to use MLD, but we don't have it yet. It would be slower also.
#define LWIP_ICMP                  LWIP_IPV4 // not noticeable perf impact leaving this on

/* ---------- ICMP options ---------- */
//#define ICMP_TTL                255

// TODO
//#define LWIP_NETIF_LINK_CALLBACK        1
//#define LWIP_NETIF_STATUS_CALLBACK      1
//#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

/* ---------- ARP options ---------- */
// ARP is enabled by default, can override default settings here
//#define LWIP_ARP                1
//#define ARP_TABLE_SIZE          10
//#define ARP_QUEUEING            1 // default setting is queueing off

/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. */
#define LWIP_DHCP               LWIP_UDP

//#define LWIP_DHCP_AUTOIP_COOP           1
//#define LWIP_DHCP_AUTOIP_COOP_TRIES     5

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

#include "Source/Library/lwip_2.0.x/system/arch/sys_arch.h"
#define LWIP_RAND()                     ((u32_t)GetRandomNumber())

#include "arch/sntp_uEZ.h"
#define SNTP_SET_SYSTEM_TIME(sec)       sntp_set_system_time(sec)
//#define SNTP_SET_SYSTEM_TIME_NTP(sec, frac) sntp_set_system_time(sec, frac) // future use

// for testing get system time can use one of these defines
//#define SNTP_CHECK_RESPONSE 2 
//#define SNTP_COMP_ROUNDTRIP 1
#define SNTP_GET_SYSTEM_TIME(sec, us) \
    do { struct timeval tv = { .tv_sec = 0, .tv_usec = 0 }; \
         get_system_time_tv(&tv); \
         (sec) = tv.tv_sec;  \
         (us) = tv.tv_usec; \
    } while (0);
//#define SNTP_GET_SYSTEM_TIME_NTP(sec, frac) sntp_get_system_time_ntp


#define SNTP_SUPPORT                    1
#define SNTP_SERVER_DNS                 1
#define SNTP_UPDATE_DELAY 86400
#define LWIP_DHCP_GET_NTP_SRV           1
#define LWIP_DHCP_MAX_NTP_SERVERS       2

//#define LWIP_DHCP_DISCOVER_ADD_HOSTNAME 1


/*----- Value in opt.h for CHECKSUM_GEN_UDP: 1 -----*/
#define CHECKSUM_GEN_UDP                1
/*----- Value in opt.h for CHECKSUM_GEN_ICMP: 1 -----*/
#define CHECKSUM_GEN_ICMP               1
/*----- Value in opt.h for CHECKSUM_CHECK_IP: 1 -----*/
#define CHECKSUM_CHECK_IP               0
/*----- Value in opt.h for CHECKSUM_CHECK_UDP: 1 -----*/
#define CHECKSUM_CHECK_UDP              0
/*----- Value in opt.h for CHECKSUM_CHECK_TCP: 1 -----*/
#define CHECKSUM_CHECK_TCP              0

// Note that checksuming is faster when data is aligned on a 4-byte boundary, some checksum impls may not handle unaligned.
// FreeRTOS heap4 at least is suppsoed to give us aligned heap.
#define LWIP_CHKSUM_ALGORITHM           3 // fastest of these 3
//#define LWIP_CHKSUM_ALGORITHM           2
//#define LWIP_CHKSUM_ALGORITHM           1 // slowest of these 3

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

#define LWIP_TIMEVAL_PRIVATE            0 // use sys/time.h include in cc.h

#endif /* __LWIPOPTS_H__ */
