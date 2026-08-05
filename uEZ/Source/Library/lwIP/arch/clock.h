/*
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIP_ARCH_CLOCK_H__
#define __LWIP_ARCH_CLOCK_H__

#include "lwip/opt.h"
#include "lwip/sys.h"

#define CLOCK_MS 1

extern u32_t clock(void);

#endif /* __LWIP_ARCH_CLOCK_H__ */
