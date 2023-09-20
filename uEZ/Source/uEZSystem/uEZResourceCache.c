/*-------------------------------------------------------------------------*
 * File:  uEZResourceCache.c
 *-------------------------------------------------------------------------*
 * Description:
 *      <DESCRIPTION>
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZResourceCache
 *  @{
 *  @brief     uEZ Resource Cache Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
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
