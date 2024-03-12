/* OPTIONAL: Pools to replace heap allocation
 * Optional: Pools can be used instead of the heap for mem_malloc. If
 * so, these should be defined here, in increasing order according to 
 * the pool element size.
 *
 * LWIP_MALLOC_MEMPOOL(number_elements, element_size)
 */

// Note in uEZ you can freely increase the number_elements until you run out of SDRAM for the .network section.
// Each pool of each size uses an extra 48 bytes per buffer plus 4 byte header for that pool.
// So for example with qty 20 of 1536 size pools that equals ((1536+48)*20)+4 = 31,684 bytes of RAM.
// Beleive that the 4 quantity numbers should be less than MEMP_NUM_PBUF. So below is 120 < 140.
// An example config showed 190 pools with 200 for MEMP_NUM_PBUF number. (in lwipopts.h)
#if MEM_USE_POOLS
LWIP_MALLOC_MEMPOOL_START
#if LWIP_SNMP // Add some memoory for SNMP if enabled and using pools
LWIP_MALLOC_MEMPOOL(LWIP_SNMP_NUM_VARS, 64)   // custom pools needed for SNMP
#endif

// With some software we might not be using the larger size pbufs, 
// so then each sized pbuff group needs to be greater than emacmem to not crash? // TODO validate

// Note that including smaller pool such as 64 size yields perf improvement when we only get small packets.
// Note that putting certain macros in this file will cause the build to fail. It needs to be kept simple.

#if(UEZ_PROCESSOR == NXP_LPC4357)
#if (LWIP_SNMP == 1)
#else
LWIP_MALLOC_MEMPOOL(32, 64)
#endif
LWIP_MALLOC_MEMPOOL(32, 128)
LWIP_MALLOC_MEMPOOL(32, 256)
LWIP_MALLOC_MEMPOOL(32, 512)
LWIP_MALLOC_MEMPOOL(32, 1024)
LWIP_MALLOC_MEMPOOL(32, 1536)

#if(EMAC_ENABLE_JUMBO_FRAME == 1)
LWIP_MALLOC_MEMPOOL(4, 2048)
LWIP_MALLOC_MEMPOOL(2, 4096)
LWIP_MALLOC_MEMPOOL(4, 8192)
#else
LWIP_MALLOC_MEMPOOL(4, 2048)
LWIP_MALLOC_MEMPOOL(2, 4096)
LWIP_MALLOC_MEMPOOL(2, 8192)
#endif

LWIP_MALLOC_MEMPOOL(1, 16384)
LWIP_MALLOC_MEMPOOL(2, 32768)

#elif(UEZ_PROCESSOR == NXP_LPC4088)
#if (LWIP_SNMP == 1)
#else
LWIP_MALLOC_MEMPOOL(32, 64)
#endif
LWIP_MALLOC_MEMPOOL(32, 128)
LWIP_MALLOC_MEMPOOL(32, 256)
LWIP_MALLOC_MEMPOOL(32, 512)
LWIP_MALLOC_MEMPOOL(32, 1024)
LWIP_MALLOC_MEMPOOL(32, 1536)

#if(EMAC_ENABLE_JUMBO_FRAME == 1)
LWIP_MALLOC_MEMPOOL(4, 2048)
LWIP_MALLOC_MEMPOOL(2, 4096)
LWIP_MALLOC_MEMPOOL(4, 8192)
#else
LWIP_MALLOC_MEMPOOL(4, 2048)
LWIP_MALLOC_MEMPOOL(2, 4096)
LWIP_MALLOC_MEMPOOL(2, 8192)
#endif

LWIP_MALLOC_MEMPOOL(1, 16384)
LWIP_MALLOC_MEMPOOL(2, 32768)

#elif(UEZ_PROCESSOR == NXP_LPC1788)
#if (LWIP_SNMP == 1)
#else
LWIP_MALLOC_MEMPOOL(32, 64)
#endif
LWIP_MALLOC_MEMPOOL(32, 128)
LWIP_MALLOC_MEMPOOL(32, 256)
LWIP_MALLOC_MEMPOOL(32, 512)
LWIP_MALLOC_MEMPOOL(32, 1024)
LWIP_MALLOC_MEMPOOL(32, 1536)

#if(EMAC_ENABLE_JUMBO_FRAME == 1)
LWIP_MALLOC_MEMPOOL(4, 2048)
LWIP_MALLOC_MEMPOOL(2, 4096)
LWIP_MALLOC_MEMPOOL(4, 8192)
#else
LWIP_MALLOC_MEMPOOL(4, 2048)
LWIP_MALLOC_MEMPOOL(2, 4096)
LWIP_MALLOC_MEMPOOL(2, 8192)
#endif

LWIP_MALLOC_MEMPOOL(1, 16384)
LWIP_MALLOC_MEMPOOL(2, 32768)

#else
#if (LWIP_SNMP == 1)
#else
LWIP_MALLOC_MEMPOOL(32, 64)
#endif
LWIP_MALLOC_MEMPOOL(32, 128)
LWIP_MALLOC_MEMPOOL(32, 256)
LWIP_MALLOC_MEMPOOL(32, 512)
LWIP_MALLOC_MEMPOOL(32, 1024)
LWIP_MALLOC_MEMPOOL(32, 1536)

#if(EMAC_ENABLE_JUMBO_FRAME == 1)
LWIP_MALLOC_MEMPOOL(4, 2048)
LWIP_MALLOC_MEMPOOL(2, 4096)
LWIP_MALLOC_MEMPOOL(4, 8192)
#else
LWIP_MALLOC_MEMPOOL(4, 2048)
LWIP_MALLOC_MEMPOOL(2, 4096)
LWIP_MALLOC_MEMPOOL(2, 8192)
#endif

LWIP_MALLOC_MEMPOOL(1, 16384)
LWIP_MALLOC_MEMPOOL(2, 32768)

#endif

LWIP_MALLOC_MEMPOOL_END
#endif /* MEM_USE_POOLS */

/* Optional: Your custom pools can go here if you would like to use
 * lwIP's memory pools for anything else.
 */
//LWIP_MEMPOOL(SYS_MBOX, 22, 100, "SYS_MBOX")
