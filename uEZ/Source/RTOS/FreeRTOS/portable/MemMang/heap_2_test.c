/*
	FreeRTOS.org V5.0.3 - Copyright (C) 2003-2008 Richard Barry.

	This file is part of the FreeRTOS.org distribution.

	FreeRTOS.org is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	FreeRTOS.org is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with FreeRTOS.org; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	A special exception to the GPL can be applied should you wish to distribute
	a combined work that includes FreeRTOS.org, without being obliged to provide
	the source code for any proprietary components.  See the licensing section
	of http://www.FreeRTOS.org for full details of how and when the exception
	can be applied.

    ***************************************************************************
    ***************************************************************************
    *                                                                         *
    * SAVE TIME AND MONEY!  We can port FreeRTOS.org to your own hardware,    *
    * and even write all or part of your application on your behalf.          *
    * See http://www.OpenRTOS.com for details of the services we provide to   *
    * expedite your project.                                                  *
    *                                                                         *
    ***************************************************************************
    ***************************************************************************

	Please ensure to read the configuration and relevant port sections of the
	online documentation.

	http://www.FreeRTOS.org - Documentation, latest information, license and 
	contact details.

	http://www.SafeRTOS.com - A version that is certified for use in safety 
	critical systems.

	http://www.OpenRTOS.com - Commercial support, development, porting, 
	licensing and training services.
*/

/*
 * A sample implementation of pvPortMalloc() and vPortFree() that permits
 * allocated blocks to be freed, but does not combine adjacent free blocks
 * into a single larger block.
 *
 * See heap_1.c and heap_3.c for alternative implementations, and the memory
 * management pages of http://www.FreeRTOS.org for more information.
 */
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"

/* Setup the correct byte alignment mask for the defined byte alignment. */

#if portBYTE_ALIGNMENT == 8
	#define heapBYTE_ALIGNMENT_MASK ( ( size_t ) 0x0007 )
#endif

#if portBYTE_ALIGNMENT == 4
	#define heapBYTE_ALIGNMENT_MASK	( ( size_t ) 0x0003 )
#endif

#if portBYTE_ALIGNMENT == 2
	#define heapBYTE_ALIGNMENT_MASK	( ( size_t ) 0x0001 )
#endif

#if portBYTE_ALIGNMENT == 1
	#define heapBYTE_ALIGNMENT_MASK	( ( size_t ) 0x0000 )
#endif

#ifndef heapBYTE_ALIGNMENT_MASK
	#error "Invalid portBYTE_ALIGNMENT definition"
#endif

/* Allocate the memory for the heap.  The struct is used to force byte
alignment without using any non-portable code. */
static struct xRTOS_HEAP
{
	unsigned portLONG ulDummy;
	unsigned portCHAR ucHeap[ configTOTAL_HEAP_SIZE ];
} xHeap __attribute__((section("RTOS_HEAP")));

/* Define the linked list structure.  This is used to link free blocks in order
of their size. */
typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
} xBlockLink;


static const unsigned portSHORT  heapSTRUCT_SIZE	= ( sizeof( xBlockLink ) + ( sizeof( xBlockLink ) % portBYTE_ALIGNMENT ) );
#define heapMINIMUM_BLOCK_SIZE	( ( size_t ) ( heapSTRUCT_SIZE * 2 ) )

/* Create a couple of list links to mark the start and end of the list. */
static xBlockLink xStart, xEnd;

static uint32_t G_bytesAllocated=0;
static uint32_t G_numAllocated=0;
static uint32_t G_biggestAllocation=0;

/* STATIC FUNCTIONS ARE DEFINED AS MACROS TO MINIMIZE THE FUNCTION CALL DEPTH. */

/*
 * Insert a block into the list of free blocks - which is ordered by size of
 * the block.  Small blocks at the start of the list and large blocks at the end
 * of the list.
 */
#define prvInsertBlockIntoFreeList( pxBlockToInsert )								\
{																					\
xBlockLink *pxIterator;																\
size_t xBlockSize;																	\
																					\
	xBlockSize = pxBlockToInsert->xBlockSize;										\
																					\
	/* Iterate through the list until a block is found that has a larger size */	\
	/* than the block we are inserting. */											\
	for( pxIterator = &xStart; pxIterator->pxNextFreeBlock->xBlockSize < xBlockSize; pxIterator = pxIterator->pxNextFreeBlock )	\
	{																				\
		/* There is nothing to do here - just iterate to the correct position. */	\
	}																				\
																					\
	/* Update the list to include the block being inserted in the correct */		\
	/* position. */																	\
	pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;					\
	pxIterator->pxNextFreeBlock = pxBlockToInsert;									\
}
/*-----------------------------------------------------------*/

#define prvHeapInit()																\
{																					\
xBlockLink *pxFirstFreeBlock;														\
																					\
	/* xStart is used to hold a pointer to the first item in the list of free */	\
	/* blocks.  The void cast is used to prevent compiler warnings. */				\
	xStart.pxNextFreeBlock = ( void * ) xHeap.ucHeap;								\
	xStart.xBlockSize = ( size_t ) 0;												\
																					\
	/* xEnd is used to mark the end of the list of free blocks. */					\
	xEnd.xBlockSize = configTOTAL_HEAP_SIZE;										\
	xEnd.pxNextFreeBlock = NULL;													\
																					\
	/* To start with there is a single free block that is sized to take up the		\
	entire heap space. */															\
	pxFirstFreeBlock = ( void * ) xHeap.ucHeap;										\
	pxFirstFreeBlock->xBlockSize = configTOTAL_HEAP_SIZE;							\
	pxFirstFreeBlock->pxNextFreeBlock = &xEnd;										\
}
/*-----------------------------------------------------------*/

void *pvPortMalloc2( size_t xWantedSize )
{
xBlockLink *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
static portBASE_TYPE xHeapHasBeenInitialised = pdFALSE;
void *pvReturn = NULL;

	vTaskSuspendAll();
	{
		/* If this is the first call to malloc then the heap will require
		initialisation to setup the list of free blocks. */
		if( xHeapHasBeenInitialised == pdFALSE )
		{
			prvHeapInit();
			xHeapHasBeenInitialised = pdTRUE;
		}

		/* The wanted size is increased so it can contain a xBlockLink
		structure in addition to the requested amount of bytes. */
		if( xWantedSize > 0 )
		{
			xWantedSize += heapSTRUCT_SIZE;

			/* Ensure that blocks are always aligned to the required number of bytes. */
			if( xWantedSize & heapBYTE_ALIGNMENT_MASK )
			{
				/* Byte alignment required. */
				xWantedSize += ( portBYTE_ALIGNMENT - ( xWantedSize & heapBYTE_ALIGNMENT_MASK ) );
			}
		}

		if( ( xWantedSize > 0 ) && ( xWantedSize < configTOTAL_HEAP_SIZE ) )
		{
			/* Blocks are stored in byte order - traverse the list from the start
			(smallest) block until one of adequate size is found. */
			pxPreviousBlock = &xStart;
			pxBlock = xStart.pxNextFreeBlock;
			while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock ) )
			{
				pxPreviousBlock = pxBlock;
				pxBlock = pxBlock->pxNextFreeBlock;
			}

			/* If we found the end marker then a block of adequate size was not found. */
			if( pxBlock != &xEnd )
			{
				/* Return the memory space - jumping over the xBlockLink structure
				at its start. */
				pvReturn = ( void * ) ( ( ( unsigned portCHAR * ) pxPreviousBlock->pxNextFreeBlock ) + heapSTRUCT_SIZE );

				/* This block is being returned for use so must be taken our of the
				list of free blocks. */
				pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

				/* If the block is larger than required it can be split into two. */
				if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
				{
					/* This block is to be split into two.  Create a new block
					following the number of bytes requested. The void cast is
					used to prevent byte alignment warnings from the compiler. */
					pxNewBlockLink = ( void * ) ( ( ( unsigned portCHAR * ) pxBlock ) + xWantedSize );
					
					/* Calculate the sizes of two blocks split from the single
					block. */
					pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;	
					pxBlock->xBlockSize = xWantedSize;			
					
					/* Insert the new block into the list of free blocks. */
					prvInsertBlockIntoFreeList( ( pxNewBlockLink ) );
				}
			}
		}
	}
	xTaskResumeAll();

	return pvReturn;
}
/*-----------------------------------------------------------*/

void vPortFree2( void *pv )
{
unsigned portCHAR *puc = ( unsigned portCHAR * ) pv;
xBlockLink *pxLink;

	if( pv )
	{
		/* The memory being freed will have an xBlockLink structure immediately
		before it. */
		puc -= heapSTRUCT_SIZE;

		/* This casting is to keep the compiler from issuing warnings. */
		pxLink = ( void * ) puc;

		vTaskSuspendAll();
		{				
			/* Add this block to the list of free blocks. */
			prvInsertBlockIntoFreeList( ( ( xBlockLink * ) pxLink ) );
		}
		xTaskResumeAll();
	}
}
/*-----------------------------------------------------------*/

#include <uEZTypes.h>

typedef struct {
    TUInt32 iHeader;
    TUInt32 iSize;
    TUInt32 iState;
    TUInt32 iBlockNum;
    TUInt32 iCopyBlockNum;
} T_checkHeader;

typedef struct {
    TUInt32 iFooter;
} T_checkFooter;

#define CHECK_HEADER        0x12345678
#define CHECK_FOOTER        0x87654321
#define CHECK_HEADER_DEAD   0xFFEEDDCC
#define CHECK_FOOTER_DEAD   0xAABBCCDD
#define CHECK_STATE_ALLOC   0x00000000
#define CHECK_STATE_DELETED 0xFFFFFFFF

static TUInt32 G_blockNum = 0;

#define MAX_BLOCKS 250
static void *G_checkList[MAX_BLOCKS];
static TUInt8 G_checkListInit = 0;

void uEZMemoryFail(void)
{
    portENTER_CRITICAL();
    while (1) 
        {}
}

void InitCheckList(void)
{
    memset(G_checkList, 0, sizeof(G_checkList));
    G_checkListInit = 1;
}

void ICheckBlock(void *pv)
{
    size_t xWantedSize;
    T_checkHeader *p_header = ((T_checkHeader *)pv)-1;

    if (p_header->iHeader != CHECK_HEADER)
        uEZMemoryFail();
    if (p_header->iState != CHECK_STATE_ALLOC)
        uEZMemoryFail();

    xWantedSize = p_header->iSize;
    T_checkFooter *p_footer = (T_checkFooter *)(((TUInt8 *)pv)+xWantedSize);
    if (p_footer->iFooter != CHECK_FOOTER)
        uEZMemoryFail();

    if (p_header->iBlockNum > MAX_BLOCKS)
        uEZMemoryFail();
    if (p_header->iCopyBlockNum != p_header->iBlockNum)
        uEZMemoryFail();

    // Check list should be pointing to this block
    if (G_checkList[p_header->iBlockNum] != pv)
        uEZMemoryFail();
}

void ICheckAllBlocks(void)
{
    TUInt32 i;

    vTaskSuspendAll();
    for (i=0; i<MAX_BLOCKS; i++) {
        if (G_checkList[i])
            ICheckBlock(G_checkList[i]);
    }
    xTaskResumeAll();
}

void MemCheckAllBlocks(void)
{
    ICheckAllBlocks();
}

void *pvPortMalloc( size_t xWantedSize )
{
    // Round up to the next 32 bit size
    xWantedSize = (xWantedSize+3)&0xFFFFFFFC;
    if (G_checkListInit == 0)
        InitCheckList();

    // Before allocating, check all existing blocks
    ICheckAllBlocks();

    size_t size = xWantedSize + sizeof(T_checkHeader) + sizeof(T_checkFooter);
    void *p = pvPortMalloc2(size);
    if (p == 0)
        return p;

    G_bytesAllocated += size;
    G_numAllocated++;
    if (size > G_biggestAllocation)
        G_biggestAllocation = size;
    if (size > 0x100) {
      size++;
      size--;
    }

    vTaskSuspendAll();
    TUInt32 n = G_blockNum;
    TUInt32 count = 0;
    for (count=0; count<MAX_BLOCKS; count++) {
        // Find next free block number
        if (G_checkList[n] == 0)
            break;
        n++;
        if (n >= MAX_BLOCKS)
            n = 0;
    }
    if (count == MAX_BLOCKS)
        uEZMemoryFail();

    T_checkHeader *p_header = (T_checkHeader *)p;
    p_header->iHeader = CHECK_HEADER;
    p_header->iSize = xWantedSize;
    p_header->iState = CHECK_STATE_ALLOC;
    if (n >= MAX_BLOCKS)
        uEZMemoryFail();
    p_header->iBlockNum = n;
    p_header->iCopyBlockNum = n;
    T_checkFooter *p_footer = (T_checkFooter *)(((char *)p) + sizeof(T_checkHeader) + xWantedSize);
    p_footer->iFooter = CHECK_FOOTER;
    G_checkList[n] = p = (void *)(p_header+1);
    xTaskResumeAll();

    // After allocating, check all existing blocks
    ICheckAllBlocks();

    if (p_header->iBlockNum == 0x78) {
        return p;
    }

    return p;
}

void vPortFree(void *pv)
{
    size_t xWantedSize;
    T_checkHeader *p_header = ((T_checkHeader *)pv)-1;

    // Before free, check all existing blocks
    ICheckAllBlocks();

    ICheckBlock(pv);

    xWantedSize = p_header->iSize;
    T_checkFooter *p_footer = (T_checkFooter *)(((char *)p_header) + sizeof(T_checkHeader) + xWantedSize);
    // Got here, okay then
    // Write bad footers and headers
    // and remove from the check list
    G_checkList[p_header->iBlockNum] = 0;
    p_footer->iFooter = CHECK_FOOTER_DEAD;
    p_header->iHeader = CHECK_HEADER_DEAD;
    p_header->iState = CHECK_STATE_DELETED;

    G_bytesAllocated -= xWantedSize + sizeof(T_checkHeader) + sizeof(T_checkFooter);
    G_numAllocated--;

    // Block checks out okay, go ahead and free it
    vPortFree2((void *)p_header);
}

