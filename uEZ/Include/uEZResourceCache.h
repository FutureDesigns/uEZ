/*-------------------------------------------------------------------------*
* File:  uEZResourceCache.h
*--------------------------------------------------------------------------*
* Description:
*   The uEZ system for handling resources that are mapped into memory
*  locations.  Resources may change and shift in memory, so these routines
*  provide a way to directly lock any resources in a resource device.
*  Resource ID's are unique IDs that allow easy identification of resource
*  regardless of position in the resource.  Resources can also be referenced
*  by a short name (12 characters, usually 8.3) and must be an exact match.
*  Simple resource devices do not need a true locking mechanism -- they just
*  return a pointer to the resource in flash.  More complex resource devices
*  lock a resource by copying the resource into memory.  To facilitate this,
*  the resource is 'locked' into memory and 'unlocked' when done to free
*  from memory.
*-------------------------------------------------------------------------*/
#ifndef UEZRESOURCE_H_
#define UEZRESOURCE_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
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

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
/**
 *  Open up access to the Resource
 *
 *  @param [in]     aName           Open the resource registered in the system.
 *  @param [out]    aDevice         Pointer to device handle to be returned
 *  @return         T_uezError      Returned error code
 */
T_uezError UEZResourceCacheOpen(const char *aName, T_uezDevice *aDevice);

/**
 *  Closes the previously opened resource and free up any memory used.
 *  Will not close if any resources are still locked.
 *
 *  @param [in]     aDevice         Previously opened resource device
 *  @return         T_uezError      Returned error code
 */
T_uezError UEZResourceCacheClose(T_uezDevice aDevice);

/**
 *  Find and lock in a resource by its unique ID in the resource device.
 *
 *  @param [in]     aCache          Previously opened resource cache
 *  @param [in]     aResourceID     ID to search for
 *  @param [out]    aResourcePtr    Returned pointer to the resource
 *  @return         T_uezError      Returned error code
 */
T_uezError UEZResourceCacheLockByID(
    T_uezDevice aCache,
    TUInt32 aResourceID,
    void **aResourcePtr);

/**
 *  Find and lock in a resource by its unique name in the resource device.
 *
 *  @param [in]     aCache          Previously opened resource cache
 *  @param [in]     aResourceName   name to search for
 *  @param [out]    aResourcePtr    Returned pointer to the resource
 *  @return         T_uezError      Returned error code
 */
T_uezError UEZResourceCacheLockByName(
    T_uezDevice aCache,
    const char *aResourceName,
    void **aResourcePtr);

/**
 *  Release a previously locked resource.  Not required for simple in-place
 *  resources.
 * 
 *  @param [in]     aCache          Previously opened resource cache
 *
 *  @param [in]     aResourcePtr    Pointer to previously locked resource
 *  @return         T_uezError      Returned error code
 */
T_uezError UEZResourceCacheUnlock(T_uezDevice aCache, void *aResourcePtr);

/**
 *  Return the size/length of a locked resource in bytes.
 *
 *  @param [in]     aCache          Previously opened resource device
 *  @param [in]     *aResourcePtr   Pointer to previously locked resource
 *  @param [in]     *aSize    		Pointer cache size
 *  @return         T_uezError      Returned error code
 */
T_uezError UEZResourceCacheGetSize(
    T_uezDevice aCache,
    void *aResourcePtr,
    TUInt32 *aSize);

#ifdef __cplusplus
}
#endif

#endif // UEZRESOURCE_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZResource.h
 *-------------------------------------------------------------------------*/
