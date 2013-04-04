/*-------------------------------------------------------------------------*
 * File:  uEZResourceCache.c
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZResourceCache
 *  @{
 *  @brief     uEZ Resource Cache Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ Resource Cache interface.
 *
 *  @par Example code:
 *  Example task to open and close the resource cache
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZResourceCache.h>
 *
 *  TUInt32 ResourceCacheTest(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezDevice aCache;
 *      if (UEZResourceCacheOpen("Name", &aCache) == UEZ_ERROR_NONE) {
 *
 *          if (UEZResourceCacheClose(aCache) != UEZ_ERROR_NONE) {
 *              // error closing the cache
 *          }
 *
 *      } else {
 *          // an error occurred opening the cache
 *      }
 *      return 0;
 *  }
 *  @endcode
 */
#include <uEZ.h>
#include <uEZResourceCache.h>
#include <uEZDeviceTable.h>
#include <Device/ResourceCache.h>

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
/*---------------------------------------------------------------------------*
 * Routine:  UEZResourceCacheOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the resource cache.
 *
 *  @param [in]	*aName  	Unique identifier for device
 *
 *  @param [in] *aCache     Pointer to device handle to be returned
 *
 *  @return     T_uezError      Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Device/ResourceCache.h>
 *
 *  T_uezDevice aCache;
 *  if (UEZResourceCacheOpen("Name", &aCache) == UEZ_ERROR_NONE) {
 *      // the cache opened properly
 *  } else {
 *      // an error occurred opening the cache
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZResourceCacheOpen(const char *aName, T_uezDevice *aCache)
{
    T_uezError error;
    DEVICE_ResourceCache **p;

    error = UEZDeviceTableFind(aName, aCache);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aCache, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Open((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZResourceCacheClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the resource cache.
 *
 *  @param [in] aCache     Device handle of Resource Cache to close
 *
 *  @return     T_uezError      Error code
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Device/ResourceCache.h>
 *
 *  T_uezDevice aCache;
 *  if (UEZResourceCacheOpen("Name", &aCache) == UEZ_ERROR_NONE) {
 *
 *     if (UEZResourceCacheClose(aCache) != UEZ_ERROR_NONE) {
 *         // error closing cache
 *     }
 *
 *  } else {
 *      // an error occurred opening the cache
 *  }
 *  @endcode
 */
T_uezError UEZResourceCacheClose(T_uezDevice aCache)
{
    T_uezError error;
    DEVICE_ResourceCache **p;

    error = UEZDeviceTableGetWorkspace(aCache, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Close((void *)p);
}

T_uezError UEZResourceCacheLockByID(
    T_uezDevice aCache,
    TUInt32 aResourceID,
    void **aResourcePtr)
{
    T_uezError error;
    DEVICE_ResourceCache **p;

    error = UEZDeviceTableGetWorkspace(aCache, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->LockByID((void *)p, aResourceID, aResourcePtr);
}

T_uezError UEZResourceCacheLockByName(
    T_uezDevice aCache,
    const char *aResourceName,
    void **aResourcePtr)
{
    T_uezError error;
    DEVICE_ResourceCache **p;

    error = UEZDeviceTableGetWorkspace(aCache, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->LockByName((void *)p, aResourceName, aResourcePtr);
}

T_uezError UEZResourceCacheUnlock(T_uezDevice aCache, void *aResourcePtr)
{
    T_uezError error;
    DEVICE_ResourceCache **p;

    error = UEZDeviceTableGetWorkspace(aCache, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->Unlock((void *)p, aResourcePtr);
}

T_uezError UEZResourceCacheGetSize(T_uezDevice aCache, void *aResourcePtr, TUInt32 *aSize)
{
    T_uezError error;
    DEVICE_ResourceCache **p;

    error = UEZDeviceTableGetWorkspace(aCache, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->GetSize((void *)p, aResourcePtr, aSize);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZResourceCache.c
 *-------------------------------------------------------------------------*/
