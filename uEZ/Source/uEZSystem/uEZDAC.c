/*-------------------------------------------------------------------------*
 * File:  uEZDAC.c
 *-------------------------------------------------------------------------*
 * Description:
 *     DAC Interface
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
 *  @addtogroup uEZDAC
 *  @{
 *  @brief     uEZ DAC Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ DAC interface.
 *
 *  @par Example code:
 *  Example task to output data from the DAC
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZDAC.h>
 *
 *  TUInt32 DACWrite(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezDevice DAC;
 *      if (UEZDACOpen("DAC0", &DAC) == UEZ_ERROR_NONE) {
 *          // the device opened properly
 *
 *          UEZDACSetBias(DAC,ETrue);
 *          UEZDACSetVRef(DAC,3000); // set reference to 3V
 *          UEZDACWriteMilliVolts(DAC, 1500);   // set output to 1.5V
 *          if (UEZDACClose(DAC) != UEZ_ERROR_NONE) {
 *              //an error occurred
 *          }
 *      } else {
 *          // an error occurred opening DAC
 *      }
 *      return 0;
 *  }
 * @endcode
 */

#include <Config.h>
#include <Device/DAC.h>
#include <uEZDeviceTable.h>
#include <uEZDAC.h>
 
 /*---------------------------------------------------------------------------*
 * Routine:  UEZDACOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the DAC.
 *
 *  @param [in]    *aName 		Pointer to name of DAC
 *                              (usually "DACx" where x is the DAC number)
 *  @param [out]   *aDevice 	Pointer to device handle to be returned
 *
 *  @return        T_uezError   If the device is opened, returns 
 *                              UEZ_ERROR_NONE.  If the device cannot be 
 *                              found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZDAC.h>
 *
 *   T_uezDevice DAC;
 *   if (UEZDACOpen("DAC0", &DAC) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *   } else {
 *       // an error occurred opening DAC
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACOpen(
            const char *const aName, 
            T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_DAC **p;
    
    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZDACClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the DAC.
 *
 *  @param [in]    aDevice 		 	Device handle of DAC to close
 *
 *  @return        T_uezError       If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZDAC.h>
 *
 *   T_uezDevice DAC;
 *   if (UEZDACOpen("DAC0", &DAC) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       if (UEZDACClose(DAC) != UEZ_ERROR_NONE) {
 *           // an error occurred
 *       }
 *   } else {
 *       // an error occurred opening DAC
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_DAC **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZDACSetBias
 *---------------------------------------------------------------------------*/
/**
 *  Turn on or off the bias voltage.
 *
 *  @param [in]    aDevice      	Handle to opened DAC device.
 *
 *  @param [in]    aBias            On (ETrue) or Off (EFalse)
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZDAC.h>
 *
 *   T_uezDevice DAC;
 *   if (UEZDACOpen("DAC0", &DAC) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       UEZDACSetBias(DAC,ETrue);
 *   } else {
 *       // an error occurred opening DAC
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACSetBias(
            T_uezDevice aDevice,
            TBool aBias)
{
    T_uezError error;
    DEVICE_DAC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->SetBias((void *)p, aBias);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZDACSetVRef
 *---------------------------------------------------------------------------*/
/**
 *  Set internal voltage reference.
 *
 *  @param [in]    aDevice      	Handle to opened DAC device.
 *
 *  @param [in]    aVRef      		Set VRef in millivolts.
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZDAC.h>
 *
 *   T_uezDevice DAC;
 *   if (UEZDACOpen("DAC0", &DAC) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       UEZDACSetVRef(DAC,3000);   // set reference to 3V
 *       if (UEZDACClose(DAC) != UEZ_ERROR_NONE) {
 *           // an error occurred
 *       }
 *   } else {
 *       // an error occurred opening DAC
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACSetVRef(
            T_uezDevice aDevice,
            TUInt32 aVRef /* in millivolts */)
{
    T_uezError error;
    DEVICE_DAC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->SetVRef((void *)p, aVRef);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZDACWriteMilliVolts
 *---------------------------------------------------------------------------*/
/**
 *  Write value to the DAC in millivolts.
 *
 *  @param [in]    aDevice      	Handle to opened DAC device.
 *
 *  @param [in]    aV               Value in millivolts to be written to DAC
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZDAC.h>
 *
 *   T_uezDevice DAC;
 *   if (UEZDACOpen("DAC0", &DAC) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       UEZDACWriteMilliVolts(DAC, 1500); // set output to 1.5V
 *       if (UEZDACClose(DAC) != UEZ_ERROR_NONE) {
 *           // an error occurred
 *       }
 *   } else {
 *       // an error occurred opening DAC
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZDACWriteMilliVolts(
        T_uezDevice aDevice,
        TUInt32 aV /* in millivolts */)
{
    T_uezError error;
    DEVICE_DAC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->WriteMilliVolts((void *)p, aV);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZHandles.c
 *-------------------------------------------------------------------------*/
