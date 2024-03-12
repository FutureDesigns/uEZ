/*
 * Copyright 2017-2020 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MFLASH_DRV_H__
#define __MFLASH_DRV_H__

#include "mflash_common.h"
#include "uEZProcessor.h"

#if(UEZ_PROCESSOR == NXP_LPC4357) // Use begining smaller sectors of flash b the first sector after CRP. (up to 64KB)

/* Flash constants */
#ifndef MFLASH_SECTOR_SIZE
//#define MFLASH_SECTOR_SIZE (65536)
#define MFLASH_SECTOR_SIZE (8192)
#endif

#ifndef MFLASH_PAGE_SIZE
#define MFLASH_PAGE_SIZE (512)
#endif

#ifndef MFLASH_DRIVE_START_ADDR
#define MFLASH_DRIVE_START_ADDR (0x1B000000)
#endif

#ifndef MFLASH_FILE_BASEADDR
//#define MFLASH_FILE_BASEADDR (MFLASH_DRIVE_START_ADDR + 0x40000)
#define MFLASH_FILE_BASEADDR (MFLASH_DRIVE_START_ADDR + 0x02000)
#endif

#ifndef MFLASH_DRIVE_END_ADDR
#define MFLASH_DRIVE_END_ADDR (MFLASH_DRIVE_START_ADDR + (0x10000) - 1)
//#define MFLASH_DRIVE_END_ADDR (MFLASH_DRIVE_START_ADDR + (0x80000) - 1)
#endif

#endif

#if(UEZ_PROCESSOR == NXP_LPC4088) // Use begining smaller sectors of flash a the first sector after CRP. (up to 64KB)

/* Flash constants */
#ifndef MFLASH_SECTOR_SIZE
#define MFLASH_SECTOR_SIZE (4096)
#endif

#ifndef MFLASH_PAGE_SIZE
#define MFLASH_PAGE_SIZE (512) //(256) algo broken with 256 from reverse order
#endif

#ifndef MFLASH_DRIVE_START_ADDR
//#define MFLASH_DRIVE_START_ADDR (0x00000000)
#define MFLASH_DRIVE_START_ADDR (0x00001000)
#endif

#ifndef MFLASH_FILE_BASEADDR
//#define MFLASH_FILE_BASEADDR (MFLASH_DRIVE_START_ADDR + 0x01000)
#define MFLASH_FILE_BASEADDR (MFLASH_DRIVE_START_ADDR + 0x00000)
#endif

#ifndef MFLASH_DRIVE_END_ADDR
#define MFLASH_DRIVE_END_ADDR (MFLASH_DRIVE_START_ADDR + (0x10000) - 1)
#endif

#endif

#if(UEZ_PROCESSOR == NXP_LPC1788) // Use begining smaller sectors of flash a the first sector after CRP. (up to 64KB)

/* Flash constants */
#ifndef MFLASH_SECTOR_SIZE
#define MFLASH_SECTOR_SIZE (4096)
#endif

#ifndef MFLASH_PAGE_SIZE
#define MFLASH_PAGE_SIZE (512) //(256) algo broken with 256 from reverse order
#endif

#ifndef MFLASH_DRIVE_START_ADDR
//#define MFLASH_DRIVE_START_ADDR (0x00000000)
#define MFLASH_DRIVE_START_ADDR (0x00001000)
#endif

#ifndef MFLASH_FILE_BASEADDR
//#define MFLASH_FILE_BASEADDR (MFLASH_DRIVE_START_ADDR + 0x01000)
#define MFLASH_FILE_BASEADDR (MFLASH_DRIVE_START_ADDR + 0x00000)
#endif

#ifndef MFLASH_DRIVE_END_ADDR
#define MFLASH_DRIVE_END_ADDR (MFLASH_DRIVE_START_ADDR + (0x10000) - 1)
#endif

#endif

// common

#ifndef MFLASH_SPIFI
#define MFLASH_SPIFI ("IAP")
#endif

#ifndef MFLASH_BAUDRATE
#define MFLASH_BAUDRATE (96000000) // Not used.
#endif

#endif

