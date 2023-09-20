/*
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIP_ARCH_ETHERNETIF_H__
#define __LWIP_ARCH_ETHERNETIF_H__

#include "lwip/opt.h"
#include "lwip/sys.h"

#define ETHERNETIF_NO_PACKET      0
#define ETHERNETIF_PACKET_READY   1

extern err_t        ethernetif_init(struct netif *netif);
extern void         ethernetif_handlepackets(struct netif *netif);

#endif /* __LWIP_ARCH_ETHERNETIF_H__ */
