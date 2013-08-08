/*-------------------------------------------------------------------------*
* File:  uEZADC.c
*-------------------------------------------------------------------------*
* Description:
*     ADC Interface
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
 *  @addtogroup uEZADC
 *  @{
 *  @brief     uEZ ADC Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ ADC interface.
 *
 *  @par Example code:
 *  Example to get ADC readings
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZADC.h>
 *
 *  T_uezDevice adc;
 *  ADC_RequestSingle aRequest;
 *  TUInt32 ADCReading;
 *  TUInt32 percent;
 *
 *  if (UEZADCOpen("ADC0", &adc) == UEZ_ERROR_NONE) {
 *      // the device opened properly
 *      aRequest.iADCChannel = 0;
 *      aRequest.iBitSampleSize = 10;
 *      aRequest.iTrigger = ADC_TRIGGER_NOW;
 *      aRequest.iCapturedData = &ADCReading;
 *      if (UEZADCRequestSingle(adc,&aRequest) == UEZ_ERROR_NONE) {
 *          percent = ADCReading * 100 / 1024; // got ADC in ADCReading
 *      } else {
 *          //an error occurred reading the ADC
 *      }
 *      if (UEZADCClose(adc) != UEZ_ERROR_NONE) {
 *         //an error occurred
 *      }
 *  } else {
 *      //an error occurred opening the ADC sensor
 *  }
 * @endcode
 */

#include "Config.h"
#include "Device/ADCBank.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>
#include <Types/ADC.h>
#include <uEZADC.h>

/*---------------------------------------------------------------------------*
* Routine:  UEZADCOpen
*---------------------------------------------------------------------------*/
/**
 *  Open up access to the ADC bank.
 *
 *  @param [in]    *aName 			Pointer to name of ADC bank
 *                                  (usually "ADCx" where x is bank number)
 *  @param [out]    *aDevice         Pointer to device handle to be returned
 *
 *  @return        T_uezError       If the device is opened, returns
 *                                  UEZ_ERROR_NONE.  If the device cannot be
 *                                  found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZADC.h>
 *
 *  T_uezDevice adc0;
 *  if (UEZADCOpen("ADC0", &adc0) == UEZ_ERROR_NONE) {
 *      //the device opened properly
 *  } else {
 *      //an error occurred
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZADCOpen(
                      const char *const aName, 
                      T_uezDevice *aDevice)
{
  T_uezError error;
  DEVICE_ADCBank **p;
  
  error = UEZDeviceTableFind(aName, aDevice);
  if (error)
    return error;
  
  error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
  if (error)
    return error;
  
  return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZADCClose
*---------------------------------------------------------------------------*/
/**
 *  End access to the ADC bank.
 *
 *  @param [in]    aDevice 			Device handle of ADC to close
 *
 *  @return        T_uezError       If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.  If the device
 *                                  handle is bad, returns
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZADC.h>
 *
 *  T_uezDevice adc0;
 *  if (UEZADCOpen("ADC0", &adc0) == UEZ_ERROR_NONE) {
 *      //the device opened properly
 *
 *      if (UEZADCClose(adc0) != UEZ_ERROR_NONE) {
 *          //an error occurred and needs to be handled.
 *      }
 *  } else {
 *      //an error occurred
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZADCClose(T_uezDevice aDevice)
{
  T_uezError error;
  DEVICE_ADCBank **p;
  
  error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
  if (error)
    return error;
  
  return error;
}

/*---------------------------------------------------------------------------*
* Routine:  UEZADCRequestSingle
*---------------------------------------------------------------------------*/
/**
 *  Requests a single ADC poll.
 *
 *  @param [in]    aDevice      	Handle to opened ADC bank device.
 *
 *  @param [out]   *aRequest    	Structure of read and/or write ADC
 *                               transaction information.
 *  @return        T_uezError    If successful, returns UEZ_ERROR_NONE.  If
 *                               an invalid device handle, returns
 *                               UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZADC.h>
 *
 *  T_uezDevice adc;
 *  ADC_RequestSingle aRequest;
 *  TUInt32 reading;
 *  TUInt32 percent;
 *  if (UEZADCOpen("ADC0", &adc) == UEZ_ERROR_NONE) { //the device opened properly
 *      aRequest.iADCChannel = 0;
 *      aRequest.iBitSampleSize = 10;
 *      aRequest.iTrigger = ADC_TRIGGER_NOW;
 *      aRequest.iCapturedData = &reading;
 *      if (UEZADCRequestSingle(adc,&aRequest) == UEZ_ERROR_NONE) {
 *          percent = reading * 100 / 0x200;  // got ADC in reading
 *      }
 *      else {
 *          // error getting ADC
 *      }
 *      if (UEZADCClose(adc) != UEZ_ERROR_NONE) {
 *          //an error occurred and needs to be handled.
 *      }
 *  } else {
 *      //an error occurred opening the ADC
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZADCRequestSingle(
                               T_uezDevice aDevice, 
                               ADC_RequestSingle *aRequest)
{
  T_uezError error;
  DEVICE_ADCBank **p;
  
  error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
  if (error)
    return error;
  
  // Do the transactions and return the result
  return (*p)->RequestSingle((void *)p, aRequest);
}
/** @} */
/*-------------------------------------------------------------------------*
* End of File:  uEZHandles.c
*-------------------------------------------------------------------------*/
