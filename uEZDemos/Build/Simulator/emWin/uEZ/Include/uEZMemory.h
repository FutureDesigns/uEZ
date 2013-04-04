/**
 *	@file 	uEZMemory.h
 *  @brief 	uEZ Memory Routines
 *
 *	uEZ Memory Allocation (thread safe) routines
 */
#ifndef _UEZ_MEMORY_H_
#define _UEZ_MEMORY_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#include "uEZTypes.h"

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
#define UEZMemRead32(addr)             *((TUInt32 *)(addr))
#endif
#ifndef UEZMemWrite32
#define UEZMemWrite32(addr, value)     *((TUInt32 *)(addr)) = (value)
#endif
#ifndef UEZMemRead16
#define UEZMemRead16(addr)             *((TUInt16 *)(addr))
#endif
#ifndef UEZMemWrite16
#define UEZMemWrite16(addr, value)     *((TUInt16 *)(addr)) = (value)
#endif
#ifndef UEZMemRead8
#define UEZMemRead8(addr)              *((TUInt8 *)(addr))
#endif
#ifndef UEZMemWrite8
#define UEZMemWrite8(addr, value)      *((TUInt8 *)(addr)) = (value)
#endif

#endif // _UEZ_MEMORY_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZMemory.h
 *-------------------------------------------------------------------------*/
