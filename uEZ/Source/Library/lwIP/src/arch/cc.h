/*
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIP_ARCH_CC_H__
#define __LWIP_ARCH_CC_H__

#include <Config.h>
#include <stdio.h>
#include <uEZPacked.h>

#define LITTLE_ENDIAN 1234

#define BYTE_ORDER  LITTLE_ENDIAN

typedef unsigned char   u8_t;
typedef signed char     s8_t;
typedef unsigned short  u16_t;
typedef signed short    s16_t;
typedef uint32_t    u32_t;
typedef signed int32_t      s32_t;
typedef uint32_t    mem_ptr_t;
typedef int32_t sys_prot_t;

/* Define (sn)printf formatters for these lwIP types */
#if (COMPILER_TYPE==RenesasRX)
	#define U16_F "u"
	#define S16_F "d"
	#define X16_F "x"
	#define U32_F "u"
	#define S32_F "d"
	#define X32_F "x"
#else
	#define U16_F "hu"
	#define S16_F "hd"
	#define X16_F "hx"
	#define U32_F "u"
	#define S32_F "d"
	#define X32_F "x"
#endif

#define LWIP_PLATFORM_DIAG(x) do {printf x;} while(0)
#define LWIP_PLATFORM_ASSERT(x)


#endif /* __LWIP_ARCH_CC_H__ */
