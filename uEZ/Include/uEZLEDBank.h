/**
 *	@file 	uEZLEDBank.h
 *  @brief 	uEZ LED Bank Interface
 *
 *	The uEZ interface which maps to low level LED drivers.
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
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
#ifndef _UEZ_LEDBank_H_
#define _UEZ_LEDBank_H_

#include <uEZTypes.h>
#include <uEZErrors.h>

/**
 *	Open up access to the LED Bank
 *
 *	@param [in] 	*aName		Pointer to name of LED bank (usually
 *                          "LEDx" where x is bank number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
T_uezError UEZLEDBankOpen(
            const char *const aName, 
            T_uezDevice *aDevice);
						
/**
 *	End access to the LED Bank.
 *
 *	@param [in]	aDevice		Handle to opened LED Bank Device to close
 *
 *	@return		T_uezError
 */
T_uezError UEZLEDBankClose(T_uezDevice aDevice);

/**
 *	Turn ON a single LED in a LED bank
 *
 *	@param [in]		aDevice		Handle to opened LED Bank Device
 *	@param [in]		aIndex    Index to LED (bit position, not mask)
 *
 *	@return		T_uezError
 */
T_uezError UEZLEDOn(
            T_uezDevice aDevice, 
            TUInt8 aIndex);
						
/**
 *	Turn OFF a single LED in a LED bank
 *
 *	@param [in]		aDevice		Handle to opened LED Bank Device
 *	@param [in]		aIndex    Index to LED (bit position, not mask)
 *
 *	@return		T_uezError
 */
T_uezError UEZLEDOff(
            T_uezDevice aDevice, 
            TUInt8 aIndex);
						
/**
 *	Setup a single LED in the LED bank to blink using one of the 
 *  blink registers (which sets the timing).  See 
 *  UEZLEDBankSetBlinkRegister
 *
 *	@param [in]		aDevice			Handle to opened LED Bank Device
 *	@param [in]		aIndex    	Index to LED (bit position, not mask)
 *	@param [in]		aBlinkReg 	Blink register to use for blink timing.
 *
 *	@return		T_uezError
 */
T_uezError UEZLEDBlink(
            T_uezDevice aDevice, 
            TUInt8 aIndex,
            TUInt32 aBlinkReg);
						
/**
 *	Turn on a group of LEDs in a bank.
 *
 *	@param [in]		aDevice		Handle to opened LED Bank Device
 *	@param [in]		aBits			Bit mask of the LEDs
 *
 *	@return		T_uezError
 */
T_uezError UEZLEDBankOn(
            T_uezDevice aDevice, 
            TUInt32 aBits);
						
/**
 *	Turn OFF a group of LEDs in a bank.
 *
 *	@param [in]		aDevice		Handle to opened LED Bank Device
 *	@param [in]		aBits			Bit mask of the LEDs
 *
 *	@return		T_uezError
 */
T_uezError UEZLEDBankOff(
            T_uezDevice aDevice, 
            TUInt32 aBits);
						
/**
 *	Blink a group of LEDs in a bank using a specific
 *  blink registers (which sets the timing).  See 
 *  UEZLEDBankSetBlinkRegister
 *
 *	@param [in]		aDevice			Handle to opened LED Bank Device
 *	@param [in]		aBits				Bit mask of the LEDs
 *	@param [in]		aBlinkReg 	Blink register to use for blink timing.
 *
 *	@return		T_uezError
 */
T_uezError UEZLEDBankBlink(
            T_uezDevice aDevice, 
            TUInt32 aLEDBits,
            TUInt32 aBlinkReg);

/**
 *	Setup the timing of a blink register of the target (if supported). 
 *
 *	@param [in]		aDevice			Handle to opened LED Bank Device 
 *  @param [in]		aBlinkReg   Blink register used to control blink
 *  @param [in]		aPeriod     Period in milliseconds
 *  @param [in]		aDutyCycle  Duty cycle in 0 (0%) to 255 (100%)
 *
 *	@return		T_uezError
 */						
T_uezError UEZLEDBankSetBlinkRegister(
            T_uezDevice aDevice, 
            TUInt32 aBlinkReg,
            TUInt32 aPeriod,
            TUInt8 aDutyCycle);

#endif // _UEZ_LEDBank_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZLEDBank.h
 *-------------------------------------------------------------------------*/
