/*-------------------------------------------------------------------------*
 * File:  ResourceCache_Direct.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <string.h>
#include <UEZDeviceTable.h>
#include "ResourceCache_DirectAccess.h"
#include "uEZPacked.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define RESOURCECACHE_DIRECT_ACCESS_TAG  "RcDA"

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_ResourceCache *iDevice;
    
    T_uezSemaphore iSem;
    TUInt8 *iBaseAddress;
    TBool iIsOpen;
    TUInt32 iNumOpen;
} T_ResourceCache_DirectAccess_Workspace;

PACK_STRUCT_BEGIN
typedef struct {
    PACK_STRUCT_FIELD(char iTag[4]);
    PACK_STRUCT_FIELD(TUInt32 iFirstEntry);
}PACK_STRUCT_STRUCT T_ResourceCache_DirectAccess_Header;
PACK_STRUCT_END

#define RESOURCECACHE_DIRECT_ACCESS_NAME_LEN        12
PACK_STRUCT_BEGIN
typedef struct {
    PACK_STRUCT_FIELD(TUInt32 iID); // 0 is reserved for end
    PACK_STRUCT_FIELD(char iName[RESOURCECACHE_DIRECT_ACCESS_NAME_LEN]);
    PACK_STRUCT_FIELD(TUInt32 iSize);
    PACK_STRUCT_FIELD(TUInt32 iReserved[2]);
    PACK_STRUCT_FIELD(TUInt32 iNext);
}PACK_STRUCT_STRUCT T_ResourceCache_DirectAccess_Entry;
PACK_STRUCT_END

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError ResourceCache_DirectAccess_Create(
    const char *aName,
    void *aBaseAddress)
{
    T_ResourceCache_DirectAccess_Workspace *p;
    T_uezError error;

    error = UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&ResourceCache_DirectAccess_Interface, 0,
        (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    p->iBaseAddress = (TUInt8 *)aBaseAddress;

    return error;
}

T_uezError ResourceCache_Generic_InitializeWorkspace(void *aWorkspace)
{
    T_ResourceCache_DirectAccess_Workspace *p =
        (T_ResourceCache_DirectAccess_Workspace *)aWorkspace;
    T_uezError error;

    // Create a semaphore for task synchronization
    error = UEZSemaphoreCreateBinary(&p->iSem);
    p->iIsOpen = EFalse;
    p->iNumOpen = 0;

    return error;
}

T_uezError ResourceCache_DirectAccess_Open(void *aWorkspace)
{
    T_ResourceCache_DirectAccess_Workspace *p =
        (T_ResourceCache_DirectAccess_Workspace *)aWorkspace;
    T_ResourceCache_DirectAccess_Header *p_header;

    p->iNumOpen++;
    if (p->iNumOpen == 1) {
        // When opening, check to see if the Tag is at the beginning
        // of the resource.  If not, then this is NOT a resource
        // and cannot be opened.
        p_header = (T_ResourceCache_DirectAccess_Header *)p->iBaseAddress;
        if ((p_header->iTag[0] != RESOURCECACHE_DIRECT_ACCESS_TAG[0]) ||
            (p_header->iTag[1] != RESOURCECACHE_DIRECT_ACCESS_TAG[1]) ||
            (p_header->iTag[2] != RESOURCECACHE_DIRECT_ACCESS_TAG[2]) ||
            (p_header->iTag[3] != RESOURCECACHE_DIRECT_ACCESS_TAG[3])) {
            // Not able to open!  Tag not found
            return UEZ_ERROR_NOT_FOUND;
        }
        p->iIsOpen = ETrue;
    }
    // Do nothing here
    return UEZ_ERROR_NONE;
}

T_uezError ResourceCache_DirectAccess_Close(void *aWorkspace)
{
    T_ResourceCache_DirectAccess_Workspace *p =
        (T_ResourceCache_DirectAccess_Workspace *)aWorkspace;
    p->iNumOpen--;
    if (p->iNumOpen == 0) {
        p->iIsOpen = EFalse;
    }
    // Do nothing here
    return UEZ_ERROR_NONE;
}

T_uezError ResourceCache_DirectAccess_LockByID(
    void *aWorkspace,
    TUInt32 aResourceID,
    void **aResourcePtr)
{
    T_ResourceCache_DirectAccess_Workspace *p =
        (T_ResourceCache_DirectAccess_Workspace *)aWorkspace;
    T_ResourceCache_DirectAccess_Header *p_header;
    T_ResourceCache_DirectAccess_Entry *p_entry;

    // Open handle if we were successfully opened
    *aResourcePtr = 0;
    if (!p->iIsOpen)
        return UEZ_ERROR_NOT_OPEN;

    p_header = (T_ResourceCache_DirectAccess_Header *)p->iBaseAddress;
    p_entry = (T_ResourceCache_DirectAccess_Entry *)(p->iBaseAddress
        + p_header->iFirstEntry);

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Search all the entries in order
    while (p_entry->iID) {
        // Find a match?
        if (p_entry->iID == aResourceID) {
            // The data immediately after this entry is the data entry
            *aResourcePtr = (void *)(&p_entry[1]);
            UEZSemaphoreRelease(p->iSem);
            return UEZ_ERROR_NONE;
        }
        p_entry = (T_ResourceCache_DirectAccess_Entry *)(p->iBaseAddress
            + p_entry->iNext);
    }

    UEZSemaphoreRelease(p->iSem);

    // Got here?  Then return not found
    return UEZ_ERROR_NOT_FOUND;
}

T_uezError ResourceCache_DirectAccess_LockByName(
    void *aWorkspace,
    const char *aResourceName,
    void **aResourcePtr)
{
    T_ResourceCache_DirectAccess_Workspace *p =
        (T_ResourceCache_DirectAccess_Workspace *)aWorkspace;
    T_ResourceCache_DirectAccess_Header *p_header;
    T_ResourceCache_DirectAccess_Entry *p_entry;
    TUInt8 name[RESOURCECACHE_DIRECT_ACCESS_NAME_LEN];
    TUInt32 i;

    // Open handle if we were successfully opened
    *aResourcePtr = 0;
    if (!p->iIsOpen)
        return UEZ_ERROR_NOT_OPEN;

    // Setup the name for the search
    for (i = 0; i < 12; i++) {
        // Stop at the end
        if (!aResourceName[i])
            break;
        // Copy over the characters
        name[i] = aResourceName[i];
    }

    // Now fill the rest of name with 0's so we can use memcmp
    for (; i < 12; i++)
        name[i] = 0;

    p_header = (T_ResourceCache_DirectAccess_Header *)p->iBaseAddress;
    p_entry = (T_ResourceCache_DirectAccess_Entry *)(p->iBaseAddress
        + p_header->iFirstEntry);

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Search all the entries in order
    while (p_entry->iID) {
        // Find a match?
        if (memcmp(p_entry->iName, name, RESOURCECACHE_DIRECT_ACCESS_NAME_LEN)
            == 0) {
            // Found a match!
            // The data immediately after this entry is the data entry
            *aResourcePtr = (void *)(&p_entry[1]);
            UEZSemaphoreRelease(p->iSem);
            return UEZ_ERROR_NONE;
        }
        p_entry = (T_ResourceCache_DirectAccess_Entry *)(p->iBaseAddress
            + p_entry->iNext);
    }

    UEZSemaphoreRelease(p->iSem);

    // Got here?  Then return not found
    return UEZ_ERROR_NOT_FOUND;
}

T_uezError ResourceCache_DirectAccess_Unlock(
    void *aWorkspace,
    void **aResourcePtr)
{
    PARAM_NOT_USED(aWorkspace);PARAM_NOT_USED(aResourcePtr);
    // Do nothing
    return UEZ_ERROR_NONE;
}

T_uezError ResourceCache_DirectAccess_GetSize(
    void *aWorkspace,
    void *aResourcePtr,
    TUInt32 *aSize)
{
    T_ResourceCache_DirectAccess_Workspace *p =
        (T_ResourceCache_DirectAccess_Workspace *)aWorkspace;
    T_ResourceCache_DirectAccess_Entry *p_entry;

    // Open handle if we were successfully opened
    *aSize = 0;
    if (!p->iIsOpen)
        return UEZ_ERROR_NOT_OPEN;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p_entry = (T_ResourceCache_DirectAccess_Entry *)aResourcePtr;
    p_entry--;
    *aSize = p_entry->iSize;
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_ResourceCache ResourceCache_DirectAccess_Interface = { {
// Common device interface
    "Generic:ResourceCache:Direct",
    0x0100,
    ResourceCache_Generic_InitializeWorkspace,
    sizeof(T_ResourceCache_DirectAccess_Workspace), },

// Functions v1.13
    ResourceCache_DirectAccess_Open,
    ResourceCache_DirectAccess_Close,
    ResourceCache_DirectAccess_LockByID,
    ResourceCache_DirectAccess_LockByName,
    ResourceCache_DirectAccess_Unlock,
    ResourceCache_DirectAccess_GetSize, };

/*-------------------------------------------------------------------------*
 * End of File:  ResourceCache_Direct.c
 *-------------------------------------------------------------------------*/
