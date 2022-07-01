/*-------------------------------------------------------------------------*
 * File:  uEZPWM.c
 *-------------------------------------------------------------------------*
 * Description:
 *     PWM Interface
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
 *    @addtogroup uEZPWM
 *  @{
 *  @brief     uEZ PWM Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ PWM interface.
 *
 *  @par Example code:
 *  Example task to use the PWM
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZPWM.h>
 *
 *  TUInt32 PWMSet(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezDevice PWM;
 *      if (UEZPWMOpen("PWM0", &PWM) == UEZ_ERROR_NONE) {
 *	 
 *          UEZPWMEnableSingleEdgeOutput(PWM, 1);
 *
 *      } else {
 *          // an error occurred opening the PWM
 *      }
 *      return 0;
 *  }
 *
 *  @endcode
*/
#include "Config.h"
#include "Device/PWM.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>
#include <uEZPWM.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZPWMOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the PWM bank.
 *
 *  @param [in]    *aName 		Pointer to name of PWM bank (usually
 *                                      "PWMx" where x is bank number)
 *  @param [in]    *aDevice     Pointer to device handle to be returned
 *
 *  @return        T_uezError   If the device is opened, returns
 *                              UEZ_ERROR_NONE.  If the device cannot be
 *                              found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZPWM.h>
 *
 *  T_uezDevice PWM;
 *  if (UEZPWMOpen("PWM0", &PWM) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *  } else {
 *      // an error occurred opening the PWM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZPWMOpen(
            const char *const aName,
            T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_PWM **p;

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
 *  End access to the PWM bank.
 *
 *  @param [in]    aDevice 			Device handle of PWM to close
 *
 *  @return        T_uezError       If the device is successfully closed,
 *                                  returns UEZ_ERROR_NONE.  If the device
 *                                  handle is bad, returns
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZPWM.h>
 *
 *  T_uezDevice PWM;
 *  if (UEZPWMOpen("PWM0", &PWM) == UEZ_ERROR_NONE) {
 *
 *      if (UEZPWMClose(PWM) != UEZ_ERROR_NONE) {
 *          // error closing PWM
 *      }
 *
 *  } else {
 *      // an error occurred opening the PWM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZPWMClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_PWM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPWMSetMaster
 *---------------------------------------------------------------------------*/
/**
 *  PWM is going to start -- chip select enabled for device
 *
 *  @param [in]    aDevice         Device handle of PWM
 *
 *  @param [in]    aMatchRegIndex  Which register is used for master counter
 *
 *  @param [in]    aPeriod         Number of Fpclk intervals per period
 *
 *  @return        T_uezError      If successful, returns UEZ_ERROR_NONE.  If
 *                                 an invalid device handle, returns
 *                                 UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZPWM.h>
 *
 *  T_uezDevice PWM;
 *  if (UEZPWMOpen("PWM0", &PWM) == UEZ_ERROR_NONE) {
 *	 
 *      UEZPWMSetMaster(PWM, 1, 1000);
 *
 *  } else {
 *      // an error occurred opening the PWM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZPWMSetMaster(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex,
            TUInt32 aPeriod
            )
{
    T_uezError error;
    DEVICE_PWM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetMaster((void *)p, aPeriod, aMatchRegIndex);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPWMSetMatchReg
 *---------------------------------------------------------------------------*/
/**
 *  Configure a match register for this PWM bank.
 *
 *  @param [in]    aDevice          Device handle of PWM
 *
 *  @param [in]    aMatchRegIndex   Index to match register (0-7)
 *
 *  @param [in]    aMatchPoint      Number of PWM cycles until match
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If
 *                                  an invalid device handle, returns
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZPWM.h>
 *
 *  T_uezDevice PWM;
 *  if (UEZPWMOpen("PWM0", &PWM) == UEZ_ERROR_NONE) {
 *	 
 *      UEZPWMSetMatchReg(PWM, 1, 1000);
 *
 *  } else {
 *      // an error occurred opening the PWM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZPWMSetMatchReg(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex,
            TUInt32 aMatchPoint)
{
    T_uezError error;
    DEVICE_PWM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetMatchRegister((void *)p,
                                  aMatchRegIndex,
                                  aMatchPoint,
                                  EFalse,
                                  EFalse,
                                  EFalse);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPWMEnableSingleEdgeOutput
 *---------------------------------------------------------------------------*/
/**
 *  PWM is going to have a single edge output (high until matches, then
 *      low, reset counter goes back to high).
 *
 *  @param [in]    aDevice         Device handle of PWM
 *
 *  @param [in]    aMatchRegIndex  Index of match register (0-7)
 *
 *  @return        T_uezError      If successful, returns UEZ_ERROR_NONE.  If
 *                                 an invalid device handle, returns
 *                                 UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZPWM.h>
 *
 *  T_uezDevice PWM;
 *  if (UEZPWMOpen("PWM0", &PWM) == UEZ_ERROR_NONE) {
 *	 
 *      UEZPWMEnableSingleEdgeOutput(PWM, 1);
 *
 *  } else {
 *      // an error occurred opening the PWM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZPWMEnableSingleEdgeOutput(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex)
{
    T_uezError error;
    DEVICE_PWM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->EnableSingleEdgeOutput((void *)p, aMatchRegIndex);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPWMDisableOutput
 *---------------------------------------------------------------------------*/
/**
 *  Disable a match register
 *
 *  @param [in]		aDevice     	Device handle of PWM
 *
 *  @param [in]		aMatchRegIndex  Index of match register (0-7)
 *
 *  @return			T_uezError     	If successful, returns UEZ_ERROR_NONE.  If
 *                                 	an invalid device handle, returns
 *                                	UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZPWM.h>
 *
 *  T_uezDevice PWM;
 *  if (UEZPWMOpen("PWM0", &PWM) == UEZ_ERROR_NONE) {
 *	 
 *      UEZPWMDisableOutput(PWM, 1);
 *
 *  } else {
 *      // an error occurred opening the PWM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZPWMDisableOutput(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex)
{
    T_uezError error;
    DEVICE_PWM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->DisableOutput((void *)p, aMatchRegIndex);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPWMSetMatchCallback
 *---------------------------------------------------------------------------*/
/**
 *  Disable a match register
 *
 *  @param [in]		aDevice     		Device handle of PWM
 *
 *  @param [in]		aMatchRegIndex   	Index of match register (0-7)
 *
 *  @param [in]		*aCallback     		Pointer to callback
 *
 *  @param [in]		*aCallbackWorkspace Pointer to callback workspace
 *                                      or 0 (if no workspace)
 *
 *  @return			T_uezError      	If successful, returns UEZ_ERROR_NONE.  If
 *                                		an invalid device handle, returns
 *                                 		UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZPWM.h>
 *
 *  void CallbackFunction(void *aWorkspace)
 *  {
 *      // Match callback code here
 *  }
 *
 *  TUInt32 Function()
 *  {
 *      T_uezDevice PWM;
 *      TUInt32 callbackFuncParameter;   // Workspace - can be any type
 *      if (UEZPWMOpen("PWM0", &PWM) == UEZ_ERROR_NONE) {
 *	 
 * 	        UEZPWMSetMatchCallback(PWM, 1, CallbackFunction,
 * 	            &callbackFuncParameter);
 *
 *      } else {
 *          // an error occurred opening the PWM
 *      }
 *      return 0;
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZPWMSetMatchCallback(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex,
			void (*aCallback)(void *aCallbackWorkspace),
        	void *aCallbackWorkspace)
{
    T_uezError error;
    DEVICE_PWM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetMatchCallback((void *)p, aMatchRegIndex, aCallback,
									aCallbackWorkspace);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZPWM.c
 *-------------------------------------------------------------------------*/
