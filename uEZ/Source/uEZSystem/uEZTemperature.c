/*-------------------------------------------------------------------------*
 * File:  uEZTemperature.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Temperature Interface
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
/**
 *  @addtogroup uEZTemperature
 *  @{
 *  @brief     uEZ Temperature Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ Temperature interface.
 *
 *  @par Example code:
 *  Example task to read the temperature
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTemperature.h>
 *
 *  TUInt32 TemperatureRead(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezDevice temp;
 *      TInt32 Temperature;
 *      if (UEZTemperatureOpen("Temp0", &temp) == UEZ_ERROR_NONE) {
 *	 
 *          if (UEZTemperatureRead(temp, &Temperature) == UEZ_ERROR_NONE) {
 *              // got temp
 *          }
 *
 *      } else {
 *          // an error occurred opening device
 *      }
 *      return 0;
 *  }
 *  @endcode
 */
 
#include "Config.h"
#include "Device/Temperature.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>
#include <uEZTemperature.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZTemperatureOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the Temperature bank.
 *
 *  @param [in]    *aName 			Pointer to name of Temperature bank 
 *									(usually "Temperaturex" where x is bank 
 *                                  number)
 *  @param [in]    *aDevice 		Pointer to device handle to be returned
 *
 *  @return        T_uezError       If the device is opened, returns 
 *                                  UEZ_ERROR_NONE.  If the device cannot be 
 *                                  found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTemperature.h>
 *
 *  T_uezDevice temp;
 *  if (UEZTemperatureOpen("Temp0", &temp) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *  } else {
 *      // an error occurred opening  device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTemperatureOpen(
            const char *const aName, 
            T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_Temperature **p;
    
    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTemperatureClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the Temperature bank.
 *
 *  @param [in]    aDevice 			Device handle of Temperature to close
 *
 *  @return        T_uezError       If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTemperature.h>
 *
 *  T_uezDevice temp;
 *  if (UEZTemperatureOpen("Temp0", &temp) == UEZ_ERROR_NONE) {
 *	 
 *      if (UEZTemperatureClose(temp) != UEZ_ERROR_NONE) {
 *            // error closing device
 *      }
 *
 *  } else {
 *      // an error occurred opening device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTemperatureClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_Temperature **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZTemperatureRead
 *---------------------------------------------------------------------------*/
/**
 *  Requests a single Temperature poll.
 *
 *  @param [in]    aDevice      	Handle to opened Temperature bank device
 *
 *  @param [in]    *aTemperature    Place to store read temperature
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZTemperature.h>
 *
 *  T_uezDevice temp;
 *  TInt32 Temperature;
 *  if (UEZTemperatureOpen("Temp0", &temp) == UEZ_ERROR_NONE) {
 *	 
 *      if (UEZTemperatureRead(temp, &Temperature) == UEZ_ERROR_NONE) {
 *            // got temp
 *      }
 *
 *  } else {
 *      // an error occurred opening device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZTemperatureRead(
    T_uezDevice aDevice, 
    TInt32 *aTemperature)
{
    T_uezError error;
    DEVICE_Temperature **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->Read((void *)p, aTemperature);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZTemperature.c
 *-------------------------------------------------------------------------*/
