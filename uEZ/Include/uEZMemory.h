/*-------------------------------------------------------------------------*
* File:  uEZMemory.h
*--------------------------------------------------------------------------*
* Description:
*         uEZ Memory Allocation (thread safe) routines
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZMemory.h
 *  @brief 	uEZ Memory Routines
 *
 *	uEZ Memory Allocation (thread safe) routines
 */
#ifndef _UEZ_MEMORY_H_
#define _UEZ_MEMORY_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
#include "uEZTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UEZ_MEM_CHECK_MEMORY
#define UEZ_MEM_CHECK_MEMORY    0
#endif
#ifndef UEZ_MEM_CHECK_MEMORY_FAIL_CALLBACK
#define UEZ_MEM_CHECK_MEMORY_FAIL_CALLBACK 0
#endif

/**
 *	Remove all previous memory blocks (if any)
 */
void UEZMemInit(void);

/**
 *	Allocate memory from the system heap.
 *
 *	@param [in]		aSize		Number of bytes to allocate
 *
 *	@return	Pointer to memory allocated, or NULL if memory not available.
 */
void *UEZMemAlloc(TUInt32 aSize);

/**
 *	Allocate a block of memory that will never be freed. The buffer is 
 *  incresed in size to a power of 2 and aligned to the size boundary. This
 *  is necessary to satisfy the MPU.
 *
 *	@param [in]		aSize		Number of bytes to allocate
 *
 *	@return	Pointer to memory allocated, or NULL if memory not available.
 */
void *UEZStackMemAlloc(TUInt32 aSize);

/**
 *	Free a previously allocated memory block.
 *
 *	@param [in]		*aMemory		Pointer to the memory block
 *
 *	@return	Pointer to memory allocated, or NULL if memory not available.
 */
void UEZMemFree(void *aMemory);

/**
 *	Reallocate a block of memory, truncating or copying the contents.
 *
 *	@param [in]		*aMemory		Pointer to the memory
 *	@param [in]		aSize				Number of bytes to reallocate
 *
 *	@return	Pointer to memory allocated, or NULL if memory not available.
 */
void *UEZMemRealloc(void *aMemory, TUInt32 aSize); //NOT IMPLEMENTED YET

/**
 *	Allocate a permanent block of memory that will never be freed.
 *
 *	@param [in]		aSize				Number of bytes to reallocate
 *
 *	@return	Pointer to memory allocated, or NULL if memory not available.
 */
void *UEZMemAllocPermanent(TUInt32 aSize);

void *UEZPlatformMemAllocPermanent(TUInt32 aSize);

#if UEZ_MEM_CHECK_MEMORY_FAIL_CALLBACK
void UEZMemFailedCallback(void);
#endif
#if UEZ_MEM_CHECK_MEMORY
TUInt32 UEZMemGetSize(void *aMemory);
TUInt32 UEZMemGetNumBlocks(void);
#endif

/*-------------------------------------------------------------------------*
 * Macros:
 *-------------------------------------------------------------------------*/
// Provide standard C compatible routines
#define malloc(a)           UEZMemAlloc(a)
#define free(a)             UEZMemFree(a)
#define realloc(a, b)       UEZMemRealloc(a, b)

#ifndef UEZMemRead32
#define UEZMemRead32(addr)             *((TVUInt32 *)(addr))
#endif
#ifndef UEZMemWrite32
#define UEZMemWrite32(addr, value)     *((TVUInt32 *)(addr)) = (value)
#endif
#ifndef UEZMemRead16
#define UEZMemRead16(addr)             *((TVUInt16 *)(addr))
#endif
#ifndef UEZMemWrite16
#define UEZMemWrite16(addr, value)     *((TVUInt16 *)(addr)) = (value)
#endif
#ifndef UEZMemRead8
#define UEZMemRead8(addr)              *((TVUInt8 *)(addr))
#endif
#ifndef UEZMemWrite8
#define UEZMemWrite8(addr, value)      *((TVUInt8 *)(addr)) = (value)
#endif

// Macros for memory placement and alignment. Tested on GCC/IAR
#ifndef   UEZ_BUFFER_ALIGNMENT
  #define UEZ_BUFFER_ALIGNMENT         0 // will disable alignment for placement macro
#endif

#if (defined __ICCARM__) || (defined __ICCRX__)
  #define UEZ_PRAGMA(P) _Pragma(#P)
  #define PRAGMA(A) _Pragma(#A)
#endif

#if UEZ_BUFFER_ALIGNMENT
  #define UEZ_BUFFER_ALIGN(Var)  UEZ_ALIGN(UEZ_BUFFER_ALIGNMENT, Var)
#else
  #define UEZ_BUFFER_ALIGN(Var)  Var
#endif

#if UEZ_BUFFER_ALIGNMENT
  #if (defined __GNUC__)
    #define UEZ_ALIGN(Alignment, ...) __attribute__ ((aligned (Alignment))) __VA_ARGS__
  #elif (defined __ICCARM__) || (defined __ICCRX__)
#define UEZ_ALIGN(Alignment, ...) UEZ_PRAGMA(data_alignment=Alignment) \
                                  __VA_ARGS__
  #elif (defined __CC_ARM)
    #define UEZ_ALIGN(Alignment, ...) __attribute__ ((aligned (Alignment))) __VA_ARGS__
  #else
    #error "Alignment not supported for this compiler."
  #endif
#else
  #define UEZ_ALIGN(Alignment, ...) __VA_ARGS__
#endif

  #if (defined __GNUC__)
    #define UEZ_PUT_SECTION(Section, ...) __attribute__ ((section (Section))) __attribute__ ((__used__)) __VA_ARGS__
  #elif (defined __ICCARM__) || (defined __ICCRX__)
// There are now 2 different IAR USED defs as IAR is changing to support attribute keywords (lol)
#define UEZ_PUT_SECTION(Section, ...)  UEZ_PRAGMA(location=Section) __USED \
                                          __VA_ARGS__
  #elif (defined __CC_ARM)
    #define UEZ_PUT_SECTION(Section, ...) __attribute__ ((section (Section), zero_init)) __VA_ARGS__
  #else
    #error "Section placement not supported for this compiler."
  #endif

/***************************************************************************************
 * Macro for general purpose variable alignment. This macro is always available.
 ***************************************************************************************/
#if (defined __GNUC__)
  #define UEZ_ALIGN_VAR(Alignment, ...) __attribute__ ((aligned (Alignment))) __VA_ARGS__
#elif (defined __ICCARM__) || (defined __ICCRX__)
#define UEZ_ALIGN_VAR(Alignment, ...) UEZ_PRAGMA(data_alignment=Alignment) \
                                  __VA_ARGS__
#elif (defined __CC_ARM)
  #define UEZ_ALIGN_VAR(Alignment, ...) __attribute__ ((aligned (Alignment))) __VA_ARGS__
#else
  #error "Alignment macro not supported for this compiler."
#endif
                                    
/***************************************************************************************
 * Macro for optimization settings. Mainly to turn it off for certain functions.
 ***************************************************************************************/
#if (defined __GNUC__)
#if defined(__clang__) // Currently other levels not supported yet. https://reviews.llvm.org/D126984
  #define UEZ_OPT_LEVEL_NONE  optnone
  #define UEZ_OPT_LEVEL_LOW   
  #define UEZ_OPT_LEVEL_MED   
  #define UEZ_OPT_LEVEL_HIGH  
#else
  #define UEZ_OPT_LEVEL_NONE  0  //Note: Clang may expect optnone
  #define UEZ_OPT_LEVEL_LOW   1
  #define UEZ_OPT_LEVEL_MED   2
  #define UEZ_OPT_LEVEL_HIGH  3
#endif

#if defined(__clang__)
#define UEZ_FUNC_OPT(Optimization, ...) __attribute__ ((Optimization)) __VA_ARGS__
#else
#define UEZ_FUNC_OPT(Optimization, ...) __attribute__ ((__optimize__(Optimization))) __VA_ARGS__
#endif

#elif (defined __ICCARM__) || (defined __ICCRX__)
/* http://ftp.iar.se/WWWfiles/arm/webic/doc/EWARM_DevelopmentGuide.ENU.pdf
 * Use this pragma directive to decrease the optimization level, or to turn 
 * off some specific optimizations. This pragma directive only affects the
 * function that follows immediately after the directive. */
  #define UEZ_OPT_LEVEL_NONE  none
  #define UEZ_OPT_LEVEL_LOW   low
  #define UEZ_OPT_LEVEL_MED   medium
  #define UEZ_OPT_LEVEL_HIGH  high
  #define UEZ_FUNC_OPT(Optimization, ...) UEZ_PRAGMA(optimize=Optimization) \
                                  __VA_ARGS__
#elif (defined __CC_ARM)
  #define UEZ_OPT_LEVEL_NONE  -0
  #define UEZ_OPT_LEVEL_LOW   -1
  #define UEZ_OPT_LEVEL_MED   -2
  #define UEZ_OPT_LEVEL_HIGH  -3
  #define UEZ_FUNC_OPT(Optimization, ...) __attribute__ ((optimize(Optimization))) __VA_ARGS__
#else
  #error "Optimization macro not supported for this compiler."
#endif

#ifdef __cplusplus
}
#endif

#endif // _UEZ_MEMORY_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZMemory.h
 *-------------------------------------------------------------------------*/
