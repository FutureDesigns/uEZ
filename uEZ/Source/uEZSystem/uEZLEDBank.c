/*-------------------------------------------------------------------------*
 * File:  uEZLEDBank.c
 *-------------------------------------------------------------------------*
 * Description:
 *     LED Interface
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

/**
 *    @addtogroup uEZLEDBank
 *  @{
 *  @brief     uEZ LED Bank Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ LED Bank interface.
 *
 *  @par Example code:
 *  Example task to blink LEDs
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 * TUInt32 LEDBankBlinkSet(T_uezTask aMyTask, void *aParams)
 * {
 * 		T_uezDevice leds;
 * 		if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *			for (i=0; i<12; i++) {
 *      		UEZLEDOn(leds, i); 
 *      		UEZTaskDelay(100);
 *      		UEZLEDOff(leds, i);
 *      		UEZTaskDelay(10);
 *   		}
 *   	UEZLEDBankClose(leds);
 * 		}
 *     	return 0;
 * }
 * @endcode
 */
 
#include "Config.h"
#include "Device/LEDBank.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>
#include <uEZLEDBank.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZLEDBankOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the LED bank.
 *
 *  @param [in]    *aName 			Pointer to name of LED bank (usually
 *                                      "LEDx" where x is bank number)
 *  @param [in]    *aDevice			Pointer to device handle to be returned
 *
 *  @return        T_uezError       If the device is opened, returns 
 *                                  UEZ_ERROR_NONE.  If the device cannot be 
 *                                  found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 *  T_uezDevice leds;
 *  if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *	    // opened successfully
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLEDBankOpen(
            const char *const aName, 
            T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_LEDBank **p;
    
    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLEDBankClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the LED bank.
 *
 *  @param [in]    aDevice 			Device handle of LED to close
 *
 *  @return        T_uezError       If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 *  T_uezDevice leds;
 *  if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *		// opened successfully
 *      UEZLEDBankClose(leds);
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLEDBankClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_LEDBank **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLEDOn
 *---------------------------------------------------------------------------*/
/**
 *  Turn on a single LED in a LED bank
 *
 *  @param [in]    aDevice          Handle to opened LED bank device.
 *
 *  @param [in]    aIndex           Index to LED (bit position, not mask)
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 *  T_uezDevice leds;
 *  if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *		for (i=0; i<12; i++) {
 *      	UEZLEDOn(leds, i); 
 *      	UEZTaskDelay(100);
 *  }
 *   UEZLEDBankClose(leds);
 * }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLEDOn(
            T_uezDevice aDevice, 
            TUInt8 aIndex)
{
    return UEZLEDBankOn(aDevice, 1<<aIndex);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLEDOff
 *---------------------------------------------------------------------------*/
/**
 *  Turn off a single LED in a LED bank
 *
 *  @param [in]    aDevice          Handle to opened LED bank device.
 *
 *  @param [in]    aIndex           Index to LED (bit position, not mask)
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 *  T_uezDevice leds;
 *  if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *		for (i=0; i<12; i++) {
 *      	UEZLEDOn(leds, i); 
 *      	UEZTaskDelay(100);
 *      	UEZLEDOff(leds, i);
 *      	UEZTaskDelay(10);
 *  }
 *  UEZLEDBankClose(leds);
 * }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLEDOff(
            T_uezDevice aDevice, 
            TUInt8 aIndex)
{
    return UEZLEDBankOff(aDevice, 1<<aIndex);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLEDBankSetBlinkRegister
 *---------------------------------------------------------------------------*/
/**
 *  Setup the timing of a blink register of the target (if supported). 
 *
 *  @param [in]    aDevice          Handle to opened LED bank device.
 *
 *  @param [in]    aBlinkReg        Blink register used to control blink
 *
 *  @param [in]    aPeriod          Period in ms
 *
 *  @param [in]    aDutyCycle       Duty cycle in 0 (0%) to 255 (100%)
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 *  T_uezDevice leds;
 *  if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *	    UEZLEDBankSetBlinkRegister(leds, 1, 100, 255);
 *      UEZLEDBankClose(leds);
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLEDBankSetBlinkRegister(
            T_uezDevice aDevice, 
            TUInt32 aBlinkReg,
            TUInt32 aPeriod,    // in ms
            TUInt8 aDutyCycle)
{
    T_uezError error;
    DEVICE_LEDBank **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetBlinkRegister((void *)p, aBlinkReg, aPeriod, aDutyCycle);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLEDBlink
 *---------------------------------------------------------------------------*/
/**
 *  Setup a single LED in the LED bank to blink using one of the 
 *      blink registers (which sets the timing).  See 
 *      UEZLEDBankSetBlinkRegister
 *
 *  @param [in]    aDevice          Handle to opened LED bank device.
 *
 *  @param [in]    aIndex           Index to LED (bit position, not mask)
 *
 *  @param [in]    aBlinkReg        Blink register to use for blink timing.
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 *  T_uezDevice leds;
 *  if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *      UEZLEDBankSetBlinkRegister(leds, 1, 100, 255);
 *		UEZLEDBlink(leds, 5, 1); 
 *  }
 *  UEZLEDBankClose(leds);
 * }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLEDBlink(
            T_uezDevice aDevice, 
            TUInt8 aIndex,
            TUInt32 aBlinkReg)
{
    return UEZLEDBankBlink(aDevice, 1<<aIndex, aBlinkReg);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLEDBankOn
 *---------------------------------------------------------------------------*/
/**
 *  Turn on a group of LEDs in a bank.
 *
 *  @param [in]    aDevice          Handle to opened LED bank device.
 *
 *  @param [in]    aBits            Index to LED (bit position, not mask)
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 *  T_uezDevice leds;
 *  if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *      UEZLEDBankOn(leds,5);
 *      UEZLEDBankClose(leds);
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLEDBankOn(
            T_uezDevice aDevice, 
            TUInt32 aBits)
{
    T_uezError error;
    DEVICE_LEDBank **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->On((void *)p, aBits);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLEDBankOff
 *---------------------------------------------------------------------------*/
/**
 *  Turn off a group of LEDs in a bank.
 *
 *  @param [in]    aDevice      	Handle to opened LED bank device.
 *
 *  @param [in]    aBits           Index to LED (bit position, not mask)
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 *  T_uezDevice leds;
 *  if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *      UEZLEDBankOff(leds,5);
 *      UEZLEDBankClose(leds);
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLEDBankOff(
            T_uezDevice aDevice, 
            TUInt32 aBits)
{
    T_uezError error;
    DEVICE_LEDBank **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->Off((void *)p, aBits);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLEDBankBlink
 *---------------------------------------------------------------------------*/
/**
 *  Blink a group of LEDs in a bank using a specific
 *      blink registers (which sets the timing).  See 
 *      UEZLEDBankSetBlinkRegister
 *
 *  @param [in]    aDevice          Handle to opened LED bank device.
 *
 *  @param [in]    aLEDBits           Index to LED (bit position, not mask)
 *
 *  @param [in]    aBlinkReg        Blink register with timing.
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLEDBank.h>
 *
 *  T_uezDevice leds;
 *  if (UEZLEDBankOpen("LEDBank0", &leds) == UEZ_ERROR_NONE) {
 *	    UEZLEDBankSetBlinkRegister(leds, 1, 100, 255);
 *      UEZLEDBankBlink(leds,5, 1);
 *      UEZLEDBankClose(leds);
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLEDBankBlink(
            T_uezDevice aDevice, 
            TUInt32 aLEDBits,
            TUInt32 aBlinkReg)
{
    T_uezError error;
    DEVICE_LEDBank **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->Blink((void *)p, aLEDBits, aBlinkReg);
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZLEDBank.c
 *-------------------------------------------------------------------------*/
