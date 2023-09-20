/*-------------------------------------------------------------------------*
 * File:  uEZMemory.c
 *-------------------------------------------------------------------------*
 * Description:
 *     List of handles in the system.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZMemory
 *  @{
 *  @brief     uEZ Memory Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ Memory interface.
 *
 *  @par Example code:
 *  Example task to allocate memory from the system heap, then free it
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  TUInt32 allocateHeap(T_uezTask aMyTask, void *aParams)
 *  {
 *      void *memoryPtr;
 *      UEZMemInit();
 *      memoryPtr = UEZMemAlloc(2048);
 *      if (memoryPtr == NULL) {
 *		    // memory not available
 *      }
 *      UEZMemFree(memoryPtr); // free memory
 *      return 0;
 *  }
 * @endcode
 */
#include "Config.h"
#include "uEZ.h"
#include "uEZMemory.h"


#if (RTOS==FreeRTOS)

#include "FreeRTOS.h"
#include "portable.h"
#include "task.h"

#undef malloc
#undef free
#undef realloc

#if (COMPILER_TYPE==VisualC)
#include <malloc.h>
#endif

// Turn this feature on to do thorough checking of memory to see if
// the blocks are going corrupt.  For this to work, all external memory
// allocators must use UEZMemAlloc too.  This check also depends on
// malloc existing.

#if UEZ_MEM_CHECK_MEMORY
typedef struct _header_tag {
    TUInt32 iHeader;
    struct _header_tag *iNext;
    TUInt32 iSize;
} T_checkMemHeader;
typedef struct {
    TUInt32 iTail;
} T_checkMemTail;

#define CHECK_MEM_HEAD      0xAA55AA11
#define CHECK_MEM_TAIL      0x4433292A
static T_checkMemHeader *G_first = 0;
static TUInt32 G_totalMem = 0;

static void IUEZMemError(void)
{
#if UEZ_MEM_CHECK_MEMORY_FAIL_CALLBACK
    UEZMemFailedCallback();
#else
    RTOS_ENTER_CRITICAL();
    while (1)
        {}
#endif
}

static TBool ICheckBlockExists(T_checkMemHeader *q)
{
    T_checkMemHeader *p = G_first;
    while (p) {
        if (p == q)
            return ETrue;
        p = p->iNext;
    }
    return EFalse;
}

static void ICheckBlock(T_checkMemHeader *p)
{
    T_checkMemTail *q;

    if (!ICheckBlockExists(p)) {
        IUEZMemError();
    } else if (p->iHeader != CHECK_MEM_HEAD) {
        IUEZMemError();
    } else {
        q = (T_checkMemTail *)(((TUInt32)p)+sizeof(T_checkMemHeader)+(p->iSize));
        if (q->iTail != CHECK_MEM_TAIL)
            IUEZMemError();
    }
}

static void IMemRemove(T_checkMemHeader *aElement)
{
    T_checkMemHeader *p = G_first;
    T_checkMemHeader *prev = 0;

    if (!p) {
        // Nothing in the list!
        IUEZMemError();
    }
    while (p) {
        if (p == aElement) {
            if (prev) {
                prev->iNext = p->iNext;
            } else {
                G_first = p->iNext;
            }
            p->iNext = 0;
            return;
        }
        prev = p;
        p = p->iNext;
    }
    // Never found it!
    IUEZMemError();
}

static void ICheckAll(void)
{
    T_checkMemHeader *p = G_first;
    while (p) {
        ICheckBlock(p);
        p = p->iNext;
    }
}
#endif // UEZ_MEM_CHECK_MEMORY

/*---------------------------------------------------------------------------*
 * Routine:  UEZMemAlloc
 *---------------------------------------------------------------------------*/
/**
 *  Allocate memory from the system heap.
 *
 *  @param [in]    aSize        Number of bytes to allocate
 *
 *  @return        *void 		Pointer to memory allocated, or NULL if
 *                              memory not available.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  void *memoryPtr;
 *  memoryPtr = UEZMemAlloc(2048);
 *  if (memoryPtr == NULL) {
 *		// memory not available
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void *UEZMemAlloc(TUInt32 aSize)
{
#if UEZ_MEM_CHECK_MEMORY
    T_checkMemHeader *p; 
    T_checkMemTail *q;

    // Align to multiples of 4 bytes
    if (aSize & 3) {
        aSize += 4-(aSize&3);
    }

    vTaskSuspendAll();

    ICheckAll();
    p = (T_checkMemHeader *)pvPortMalloc(aSize+sizeof(T_checkMemHeader)+sizeof(T_checkMemTail));
    if (p) {
        p->iNext = G_first;
        p->iSize = aSize;
        G_totalMem += aSize;
        G_first = p;
        p->iHeader = CHECK_MEM_HEAD;
        q = (T_checkMemTail *)(((TUInt32)p)+sizeof(T_checkMemHeader)+(p->iSize));
        q->iTail = CHECK_MEM_TAIL;
    }
	xTaskResumeAll();
    if (p)
        return (void *)(((TUInt32)p)+sizeof(T_checkMemHeader));
    return 0;
#else
    return pvPortMalloc(aSize);
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZMemAllocPermanent
 *---------------------------------------------------------------------------*/
/**
 *  Allocate a permanent block of memory that will never be freed.
 *
 *  @param [in]    aSize         	Number of bytes to allocate
 *
 *  @return        *void            Pointer to memory allocated, or NULL if
 *                                  memory not available.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  void *memoryPtr;
 *  memoryPtr = UEZMemAllocPermanent(2048);
 *  if (memoryPtr == NULL) {
 *		// memory not available
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void *UEZMemAllocPermanent(TUInt32 aSize)
{
#if UEZ_PLATFORM_MEMALLOC_PERMANENT
    return UEZPlatformMemAllocPermanent(aSize);
#else
    return UEZMemAlloc(aSize);
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZMemFree
 *---------------------------------------------------------------------------*/
/**
 *  Free a previously allocated memory block.
 *
 *  @param [in]    *aMemory   	Pointer to memory previously allocated.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  void *memoryPtr;
 *  UEZMemFree(memoryPtr);
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZMemFree(void *aMemory)
{
#if UEZ_MEM_CHECK_MEMORY
    T_checkMemHeader *p; 
    T_checkMemTail *q;

    ICheckAll();
    p = (T_checkMemHeader *)(((TUInt32)aMemory)-sizeof(T_checkMemHeader));
    if (!ICheckBlockExists(p)) {
        IUEZMemError();
    } else {
        ICheckBlock(p);
        q = (T_checkMemTail *)(((TUInt32)p)+sizeof(T_checkMemHeader)+(p->iSize));
        // Clear head and tail
        p->iHeader = 0;
        q->iTail = 0;
        G_totalMem -= p->iSize;

        // Remove from the list
        IMemRemove(p);

	    vTaskSuspendAll();
        vPortFree((void *)p);
	    xTaskResumeAll();
    }
#else
    vPortFree(aMemory);
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZMemInit
 *---------------------------------------------------------------------------*/
/**
 *  Free a previously allocated memory block.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  UEZMemInit();
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZMemInit(void)
{
    // prvHeapInit is called internally by pvPortMalloc
//    prvHeapInit();
#if UEZ_MEM_CHECK_MEMORY
    // Remove all previous memory blocks (if any)
    while (G_first) {
        T_checkMemHeader *p = G_first->iNext;
        free(G_first);
        G_first = p;
        G_totalMem = 0;
    }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZMemRealloc
 *---------------------------------------------------------------------------*/
/**
 *  Reallocate a block of memory, truncating or copying the contents.
 *
 *  @param [in]    *aMemory 	Pointer to memory previously allocated.
 *
 *  @param [in]    aSize    	Number of bytes to allocate
 *
 *  @return        *void  		Pointer to memory allocated, or NULL if
 *                          	memory not available.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  void *memoryPtr;
 *  memoryPtr = UEZMemRealloc(memoryPtr,2048);
 *  if (memoryPtr == NULL) {
 *		// memory not available
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void *UEZMemRealloc(void *aMemory, TUInt32 aSize)
{
    // TBD:
    return 0;
}

#if UEZ_MEM_CHECK_MEMORY
TUInt32 UEZMemGetSize(void *aMemory)
{
    T_checkMemHeader *p; 

    p = (T_checkMemHeader *)(((TUInt32)aMemory)-sizeof(T_checkMemHeader));

    return p->iSize;
}

TUInt32 UEZMemGetNumBlocks(void)
{
    T_checkMemHeader *p = G_first;
    TUInt32 count = 0;

    // Manually count the blocks each time
    while (p) {
        count++;
        p = p->iNext;
    }
    return count;
}

#endif

/*****************************************************************************/
/*                     SafeRTOS FUNCTIONS                                    */
/*****************************************************************************/
#elif (RTOS == SafeRTOS)

#undef malloc
#undef free

#ifndef UEZ_DEBUG_HEAVY_ASSERTS
#define UEZ_DEBUG_HEAVY_ASSERTS     1
#endif

#if UEZ_DEBUG_HEAVY_ASSERTS
    #define UEZAssert(x)        if( !( x ) ) UEZHalt()

static void UEZHalt(void)
{
    RTOS_ENTER_CRITICAL();
    while( 1 )
    {
	}    
}
#else
    /* Not heavy asserts.  Remove them */
    #define UEZAssert(x)
#endif


/* SafeRTOS will use a simple heap1 type system where buffers are allocated
from a pool of memory. However as we are not using dynamic memory allocation, 
these buffers can not be free'd. */

/* Allocate the memory for the heap.  The struct is used to force byte
alignment without using any non-portable code. */
static union xRTOS_HEAP
{
	volatile unsigned long long ullDummy;
	unsigned char ucHeap[ configSMALL_BUFFERS_SIZE ];
} xHeap;

static unsigned long ulNextFreeByte = ( unsigned long ) 0;

/* Allocate a separate structure for the stack heap. Stacks tend to be large and
have large alignment requirements, therefore sharing a heap with small items 
will be very inefficient. */
static union xRTOS_STACK_HEAP
{
	volatile unsigned long long ullDummy;
	unsigned char ucHeap[ configTASK_STACK_BUF_SIZE ];
} xStackHeap;

static unsigned long ulNextFreeStackByte = ( unsigned long ) 0;

/*---------------------------------------------------------------------------*
 * Routine:  UEZMemInit
 *---------------------------------------------------------------------------*/
/**
 *  Initialise the pseudo heap.
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  UEZMemInit();
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZMemInit(void)
{
	/* Only required when static memory is not cleared. */
	ulNextFreeByte = ( unsigned long ) 0;
	ulNextFreeStackByte = ( unsigned long ) 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZMemAlloc
 *---------------------------------------------------------------------------*/
/**
 *  Allocate memory from the system heap.
 *
 *  @param [in]    aSize    Number of bytes to allocate
 *
 *  @param [in]    *void 	Pointer to memory allocated, or NULL if
 *                          memory not available.
 *  @return        
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  void *memoryPtr;
 *  memoryPtr = UEZMemAlloc(2048); 
 *  if (memoryPtr == NULL) {
 *		// memory not available
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void *UEZMemAlloc( TUInt32 xWantedSize )
{
void *pvReturn = 0; 

	/* Ensure that blocks are always aligned to the required number of bytes. */
	if( xWantedSize & portBYTE_ALIGNMENT_MASK )
	{
		/* Byte alignment required. */
		xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & portBYTE_ALIGNMENT_MASK ) );
	}
	
	vTaskSuspendScheduler();
	{
		/* Check there is enough room left for the allocation. */
		if( ( ( ulNextFreeByte + xWantedSize ) < ( configSMALL_BUFFERS_SIZE ) ) &&
			( ( ulNextFreeByte + xWantedSize ) > ulNextFreeByte )	)/* Check for overflow. */
		{
			/* Return the next free byte then increment the index past this
			block. */
			pvReturn = &( xHeap.ucHeap[ ulNextFreeByte ] );
			ulNextFreeByte += xWantedSize;			
		}	
	}
	xTaskResumeScheduler();
	
	/* Trap alloc failures during development. */
	UEZAssert( pvReturn );
	
	return pvReturn;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZMemAllocPermanent
 *---------------------------------------------------------------------------*/
/**
 *  Allocate a permanent block of memory that will never be freed.
 *
 *  @param [in]    aSize   	Number of bytes to allocate
 *
 *  @return        void *  	Pointer to memory allocated, or NULL if
 *                          memory not available.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  void *memoryPtr;
 *  memoryPtr = UEZMemAllocPermanent(2048);
 *  if (memoryPtr == NULL) {
 *		// memory not available
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void *UEZMemAllocPermanent(TUInt32 aSize)
{
	/* all memory allocation is permanent in this implementation. */
  	return UEZMemAlloc( aSize );
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZStackMemAlloc 
 *---------------------------------------------------------------------------*/
/**
 *  Allocate a block of memory that will never be freed. The buffer is 
 *      incresed in size to a power of 2 and aligned to the size boundary. This
 *      is necessary to satisfy the MPU.
 *
 *  @param [in]    aSize    Number of bytes to allocate
 *
 *  @return        *void   	Pointer to memory allocated, or NULL if
 *                          memory not available.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  void *memoryPtr;
 *  memoryPtr = UEZStackMemAlloc(2048);
 *  if (memoryPtr == NULL) {
 *		// memory not available
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void *UEZStackMemAlloc( TUInt32 aSize )
{
void *pvReturn = 0; 
unsigned long ulRequiredSize;
unsigned long ulOffsetAdjReq;
short usLoop;

	/* Cannot do anything if aSize is 0. */
	if( 0 != aSize )
	{
		ulRequiredSize = aSize - 1;
        for( usLoop = 1; usLoop < 32; usLoop++ )
		{
        	ulRequiredSize = ulRequiredSize | ulRequiredSize >> usLoop;
		}
        ulRequiredSize++;

		vTaskSuspendScheduler();
		{
	  		/* Ensure that xStackHeap.ucHeap[ ulNextFreeStackByte ] is aligned to 
		    the required number of bytes. */
		    ulOffsetAdjReq = ( unsigned long ) ( &xStackHeap.ucHeap[ ulNextFreeStackByte ] ) & ( ulRequiredSize - 1 );
			if( 0 != ulOffsetAdjReq )
			{
				/* Byte alignment required. */
				ulNextFreeStackByte += ( ulRequiredSize - ulOffsetAdjReq );
			}
	
			/* Check there is enough room left for the allocation. */
			if( ( ( ulNextFreeStackByte + ulRequiredSize ) < configTASK_STACK_BUF_SIZE ) &&
				( ( ulNextFreeStackByte + ulRequiredSize ) > ulNextFreeStackByte )	)/* Check for overflow. */
			{
				/* Return the next free byte then increment the index past this
				block. */
				pvReturn = &( xStackHeap.ucHeap[ ulNextFreeStackByte ] );
				ulNextFreeStackByte += ulRequiredSize;	
			}
		}	
		xTaskResumeScheduler();
	}
	
	
	/* Trap alloc failures during development. */
	UEZAssert( pvReturn );

	return pvReturn;}

/*---------------------------------------------------------------------------*
 * Routine:  UEZMemFree
 *---------------------------------------------------------------------------*/
/**
 *  Free a previously allocated memory block.
 *
 *  @param [in]    *aMemory           -- Pointer to memory previously allocated.
 *
 *  @return        
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZMemory.h>
 *
 *  UEZMemFree(Memory); 		// void pointer to memory to be freed
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void UEZMemFree(void *aMemory)
{
	( void ) aMemory;
	
	/* no freeing of memory is allowed. */
}

#else
#error "No RTOS defined."
#endif  
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZMemory.c
 *-------------------------------------------------------------------------*/
