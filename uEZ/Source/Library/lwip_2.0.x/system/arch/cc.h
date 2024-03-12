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

/*
 * SPDX-FileCopyrightText: 2001 Swedish Institute of Computer Science
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * SPDX-FileContributor: 2018-2022 Espressif Systems (Shanghai) CO LTD
 */
#ifndef __CC_H__
#define __CC_H__

#include "cpu.h"
#include <Config.h>
#include <stdlib.h>
#include <stdint.h> /* for int types */
#include <uEZTypes.h> // bring in cmsis functions for btye swap, they are different for each toolchain
#include <uEZPacked.h>
//#include <time.h> // this isn't working properly in IAR yet
#include <sys/time.h> // uez has own timeval definition

#ifdef __cplusplus
extern "C" {
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif // BYTE_ORDER

#define LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS // provide our platform specific byte swap for perf

#define htons(x) __REVSH(x) // IAR doesn't have __builtin_bswap16 and related, but has iar__ versions
#define ntohs(x) __REVSH(x) // get __builtin_bswap16(x) from cmsis
#define htonl(x) __REV(x) // get __builtin_bswap32(x) from cmsis
#define ntohl(x) __REV(x)

//typedef uint8_t  u8_t;
//typedef int8_t   s8_t;
//typedef uint16_t u16_t;
//typedef int16_t  s16_t;
//typedef uint32_t u32_t;
//typedef int32_t  s32_t;

typedef int32_t sys_prot_t;

#define S16_F "d"
#define U16_F "d"
#define X16_F "x"

#define S32_F "d"
#define U32_F "u"
#define X32_F "x"

/* define compiler specific symbols */
#if defined (__ICCARM__)
// use uEZPacked.h definitions
//#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
//#define PACK_STRUCT_USE_INCLUDES

#elif defined (__CC_ARM)

#define PACK_STRUCT_BEGIN __packed
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#elif defined (__GNUC__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#elif defined (__TASKING__)

#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x

#endif

// use time.h so we don't have duplicate timeval structure in sockets.h
// This may be broken in IAR builds currently and require customized DLIB settings.
#define LWIP_TIMEVAL_PRIVATE 0 

// Note: Can log also to a file here. ESP-IDF does this.

#if (SEGGER_ENABLE_RTT ==1 )  // use rtt for fast diag messages. Don't use it for asserts currently
#define LWIP_PLATFORM_DIAG(message) do {SEGGER_RTT_printf_0 message;} while(0)
#else
#define LWIP_PLATFORM_DIAG(message) do {printf message;} while(0)
#endif

#include <stdio.h>

#if 0
#ifdef NDEBUG
#define LWIP_NOASSERT 1
#else // Assertions enabled
#if CONFIG_OPTIMIZATION_ASSERTIONS_SILENT
//#define LWIP_PLATFORM_ASSERT(message) abort()
#else
// __assert_func is the assertion failure handler from newlib, defined in assert.h
#define LWIP_PLATFORM_ASSERT(message) __assert_func(__FILE__, __LINE__, __ASSERT_FUNC, message)
#endif

// If assertions are on, the default LWIP_ERROR handler behaviour is to
// abort w/ an assertion failure. Don't do this, instead just print the error (if LWIP_DEBUG is set)
// and run the handler (same as the LWIP_ERROR behaviour if LWIP_NOASSERT is set).
#ifdef LWIP_DEBUG
#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  puts(message); handler;}} while(0)
#else
// If LWIP_DEBUG is not set, return the error silently (default LWIP behaviour, also.)
#define LWIP_ERROR(message, expression, handler) do { if (!(expression)) { \
  handler;}} while(0)
#endif // LWIP_DEBUG

#endif /* NDEBUG */
#endif

#ifdef __cplusplus
}
#endif

//#define LWIP_PLATFORM_ASSERT(x) //do { if(!(x)) while(1); } while(0)

//#define LWIP_RAND() ((u32_t)rand())

//#define LWIP_NO_STDINT_H 1 // We use stdint.h above so don't define this.

#endif /* __CC_H__ */
