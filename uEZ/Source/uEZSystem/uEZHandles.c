/*-------------------------------------------------------------------------*
 * File:  uEZHandles.c
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
 *    @addtogroup uEZHandles
 *  @{
 *  @brief     uEZ Handles Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ Handles interface.
 *
 * @par Example code:
 * Example task to initailize the handle table and allocate a handle
 * @par
 * @code
 * #include <uEZ.h>
 * #include <uEZHandles.h>
 *
 * TUInt32 uEZHandleTest(T_uezTask aMyTask, void *aParams)
 * {
 *  	T_uezHandle aHandle;
 *	   	uEZHandleTableInit();
 *  	if (uEZHandleAlloc(&aHandle) == UEZ_ERROR_NONE) {
 *  		// handle allocated succesfully
 *  	}
 *     return 0;
 * }
 * @endcode
 */
#include <string.h>
#include <stdio.h>
#include "Config.h"
#include "uEZ.h"
#include "uEZHandles.h"
#include <uEZBSP.h>

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_uezlexHandleStruct G_handles[UEZ_NUM_HANDLES];
static T_uezHandle G_index;
static T_uezHandle G_numFree;

/*---------------------------------------------------------------------------*
 * RTOS Specific Globals:
 *---------------------------------------------------------------------------*/
#if (RTOS==FreeRTOS)
#include "FreeRTOS.h"
#include "semphr.h"
#endif

static void IGrab(void)
{
    RTOS_ENTER_CRITICAL();
}
static void IRelease(void)
{
    RTOS_EXIT_CRITICAL();
}

#ifndef UEZ_DEBUG_HEAVY_ASSERTS
#define UEZ_DEBUG_HEAVY_ASSERTS     1
#endif

#if (UEZ_DEBUG_HEAVY_ASSERTS == 1)
    #define HANDLE_ASSERT(cond)       if (!(cond)) { HandleFail(); }

#else
    // Not heavy asserts.  Remove them
    #define HANDLE_ASSERT(cond)
#endif


void HandleFail(void)
{
#if UEZ_HANDLES_FAIL_CALLBACK
    UEZHandleFailCallback();
#else
    volatile int32_t i=1;
    RTOS_ENTER_CRITICAL();
    while (i) {
        UEZBSP_FatalError(3);
    }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  uEZHandleTableInit
 *---------------------------------------------------------------------------*/
/**
 *  Initialize the handle table
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZHandles.h>
 *
 *  uEZHandleTableInit();
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void uEZHandleTableInit(void)
{
    TUInt32 i;
    memset(G_handles, 0, sizeof(G_handles));
    // Reserver handle zero so we don't have 0 handles
    G_handles[0].iTypeAndFlags = UEZ_HANDLE_RESERVED;
    G_index = 1;
    G_numFree = UEZ_NUM_HANDLES-1;
    for (i=1; i<UEZ_NUM_HANDLES; i++) {
        G_handles[i].iData[0] = 0xAAAAAAAA;
        G_handles[i].iData[1] = 0xBBBBBBBB;
        G_handles[i].iData[2] = 0xCCCCCCCC;
    }
}

static T_uezError IFindFreeHandle(T_uezHandle *aHandle)
{
    HANDLE_ASSERT(aHandle != 0);

    if (G_numFree == 0) {
//        printf("IFindFreeHandle: Out of handles\n");
        return UEZ_ERROR_OUT_OF_HANDLES;
    }

    // Uncomment this line if you want your handles to always start
    // from the beginning.  However, by not resetting each time,
    // the handles are rotated through and this helps ensure handle
    // numbers tend to be new.
//    G_index = 0;
    do {
        G_index++;
        // Check if we are out of bounds.  If so, restart at the top.
        if (G_index >= UEZ_NUM_HANDLES)
            G_index = 1;
        if (G_handles[G_index].iTypeAndFlags == 0)  {
            *aHandle = G_index;
            HANDLE_ASSERT(G_numFree > 0);
            G_numFree--;
            HANDLE_ASSERT(*aHandle < UEZ_NUM_HANDLES);

            return UEZ_ERROR_NONE;
        }
    } while (1);

    // Never reach here
}
/*---------------------------------------------------------------------------*
 * Routine:  uEZHandleAlloc
 *---------------------------------------------------------------------------*/
/**
 *  Allocate a handle.
 *
 *  @param [in]	*aHandle 		Handle to allocate
 *
 *  @return     T_uezError      Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZHandles.h>
 *
 *  T_uezHandle aHandle;
 *	uEZHandleTableInit();
 *  if (uEZHandleAlloc(&aHandle) == UEZ_ERROR_NONE) {
 *      // handle allocated succesfully
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError uEZHandleAlloc(T_uezHandle *aHandle)
{
    T_uezlexHandleStruct *p;
    T_uezError error;
#if UEZ_HANDLES_CHECK
    uEZHandlesCheck();
#endif

    IGrab();
    // Get a unique handle
    error = IFindFreeHandle(aHandle);
    if (error) {
        HANDLE_ASSERT(0);
        IRelease();
        return error;
    }

    HANDLE_ASSERT(*aHandle < UEZ_NUM_HANDLES);

    // Fill in the data
    p = G_handles+*aHandle;
    p->iTypeAndFlags = UEZ_HANDLE_ZOMBIE;  // allocate but don't do anything with it
    p->iData[0] = 0;
    p->iData[1] = 0;
    p->iData[2] = 0;
    IRelease();

    return UEZ_ERROR_NONE;
}

T_uezError uEZHandleFree(T_uezHandle aHandle)
{
    T_uezlexHandleStruct *p;

    HANDLE_ASSERT(aHandle < UEZ_NUM_HANDLES);
    HANDLE_ASSERT(aHandle != UEZ_NULL_HANDLE);
#if UEZ_HANDLES_CHECK
    uEZHandlesCheck();
#endif

    // Is this handle in range?
    if ((aHandle >= UEZ_NUM_HANDLES) || (aHandle == UEZ_NULL_HANDLE))
        return UEZ_ERROR_HANDLE_INVALID;

    p = G_handles+aHandle;
    IGrab();

    // Is this a free handle already?
    if ((p->iTypeAndFlags & UEZ_HANDLE_TYPE_MASK)==UEZ_HANDLE_FREE) {
        IRelease();
        return UEZ_ERROR_HANDLE_INVALID;
    }

    // Free this handle (just set it to free)
    G_handles[aHandle].iTypeAndFlags = UEZ_HANDLE_FREE;
    G_handles[aHandle].iData[0] = 0xAAAAAAAA;
    G_handles[aHandle].iData[1] = 0xBBBBBBBB;
    G_handles[aHandle].iData[2] = 0xCCCCCCCC;
    G_numFree++;

    IRelease();
    return UEZ_ERROR_NONE;
}

T_uezError uEZHandleSet(
    T_uezHandle aHandle,
    TUInt32 iTypeAndFlags,
    TUInt32 aData1,
    TUInt32 aData2,
    TUInt32 aData3)
{
    T_uezlexHandleStruct *p;

    HANDLE_ASSERT(aHandle < UEZ_NUM_HANDLES);
    HANDLE_ASSERT(aHandle != UEZ_NULL_HANDLE);
#if UEZ_HANDLES_CHECK
    uEZHandlesCheck();
#endif

    // Is this handle in range?
    if ((aHandle >= UEZ_NUM_HANDLES) || (aHandle == UEZ_NULL_HANDLE))
        return UEZ_ERROR_HANDLE_INVALID;

    p = G_handles+aHandle;

    IGrab();
    // Is this a free handle already?
    if ((p->iTypeAndFlags & UEZ_HANDLE_TYPE_MASK)==UEZ_HANDLE_FREE) {
        IRelease();
        return UEZ_ERROR_HANDLE_INVALID;
    }

    // Modify the data
    p->iTypeAndFlags = iTypeAndFlags;
    p->iData[0] = aData1;
    p->iData[1] = aData2;
    p->iData[2] = aData3;

    IRelease();
    return UEZ_ERROR_NONE;
}

T_uezError uEZHandleGet(
    T_uezHandle aHandle,
    TUInt32 *aTypeAndFlags,
    TUInt32 *aData1,
    TUInt32 *aData2,
    TUInt32 *aData3)
{
    T_uezlexHandleStruct *p;

    HANDLE_ASSERT(aHandle < UEZ_NUM_HANDLES);
    HANDLE_ASSERT(aHandle != UEZ_NULL_HANDLE);
#if UEZ_HANDLES_CHECK
    uEZHandlesCheck();
#endif

    // Is this handle in range?
    if ((aHandle >= UEZ_NUM_HANDLES) || (aHandle == UEZ_NULL_HANDLE))
        return UEZ_ERROR_HANDLE_INVALID;

    // Get the data
    p = G_handles+aHandle;

    IGrab();

    if (aTypeAndFlags)
        *aTypeAndFlags = p->iTypeAndFlags;
    if (aData1)
        *aData1 = p->iData[0];
    if (aData2)
        *aData2 = p->iData[1];
    if (aData3)
        *aData3 = p->iData[2];

    IRelease();

    return UEZ_ERROR_NONE;
}

T_uezError uEZHandleGetFromISR(
    T_uezHandle aHandle,
    TUInt32 *aTypeAndFlags,
    TUInt32 *aData1,
    TUInt32 *aData2,
    TUInt32 *aData3)
{
    T_uezlexHandleStruct *p;

    HANDLE_ASSERT(aHandle < UEZ_NUM_HANDLES);
    HANDLE_ASSERT(aHandle != UEZ_NULL_HANDLE);

    // Is this handle in range?
    if ((aHandle >= UEZ_NUM_HANDLES) || (aHandle == UEZ_NULL_HANDLE))
        return UEZ_ERROR_HANDLE_INVALID;

    // Get the data
    p = G_handles+aHandle;

    if (aTypeAndFlags)
        *aTypeAndFlags = p->iTypeAndFlags;
    if (aData1)
        *aData1 = p->iData[0];
    if (aData2)
        *aData2 = p->iData[1];
    if (aData3)
        *aData3 = p->iData[2];

    return UEZ_ERROR_NONE;
}

T_uezError uEZHandleFindOfType(TUInt32 aType, T_uezHandle *aHandle, TUInt32 *aIndex)
{
    TUInt32 i = *aIndex;

    HANDLE_ASSERT(aHandle != 0);
    HANDLE_ASSERT(aIndex != 0);

    // Next field
    while ((++i) < UEZ_NUM_HANDLES)  {
        if ((G_handles[i].iTypeAndFlags & UEZ_HANDLE_TYPE_MASK)==aType)  {
            // Found a match, return this one
            *aHandle = i;
            *aIndex = i;
            HANDLE_ASSERT(*aHandle < UEZ_NUM_HANDLES);
            return UEZ_ERROR_NONE;
        }
    }

    *aIndex = i;
    return UEZ_ERROR_NOT_FOUND;
}

TUInt32 uEZHandleGetType(T_uezHandle aHandle)
{
    HANDLE_ASSERT(aHandle != UEZ_NULL_HANDLE);
    HANDLE_ASSERT(aHandle < UEZ_NUM_HANDLES);

    // Is this handle in range?
    if ((aHandle >= UEZ_NUM_HANDLES) || (aHandle == UEZ_NULL_HANDLE))
        return UEZ_ERROR_HANDLE_INVALID;

    return G_handles[aHandle].iTypeAndFlags & UEZ_HANDLE_TYPE_MASK;
}

void uEZHandlesCheck(void)
{
    TUInt32 i;
    IGrab();
    for (i=1; i<UEZ_NUM_HANDLES; i++) {
        if (G_handles[i].iTypeAndFlags == UEZ_HANDLE_FREE) {
            HANDLE_ASSERT(G_handles[i].iData[0] == 0xAAAAAAAA);
            HANDLE_ASSERT(G_handles[i].iData[1] == 0xBBBBBBBB);
            HANDLE_ASSERT(G_handles[i].iData[2] == 0xCCCCCCCC);
        } else {
            HANDLE_ASSERT(G_handles[i].iTypeAndFlags <= UEZ_HANDLE_DEVICE);
        }
    }
    IRelease();
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZHandles.c
 *-------------------------------------------------------------------------*/
