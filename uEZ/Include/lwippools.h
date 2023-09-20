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
LWIP_MALLOC_MEMPOOL(50, 256)
LWIP_MALLOC_MEMPOOL(30, 512)
LWIP_MALLOC_MEMPOOL(20, 1024)
LWIP_MALLOC_MEMPOOL(20, 1536)
LWIP_MALLOC_MEMPOOL_END
#endif /* MEM_USE_POOLS */

/* Optional: Your custom pools can go here if you would like to use
 * lwIP's memory pools for anything else.
 */
//LWIP_MEMPOOL(SYS_MBOX, 22, 100, "SYS_MBOX")
