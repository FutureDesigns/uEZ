/**
 *	@file 	uEZTemperature.h
 *  @brief 	uEZ Temperature Interface
 *
 *	The uEZ interface which maps to low level Temperature drivers.
 */
#ifndef _UEZ_Temperature_H_
#define _UEZ_Temperature_H_

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

#include <uEZTypes.h>
#include <uEZErrors.h>

/**
 *	Open up access to the Temperature bank.
 *
 *	@param [in] 	*aName		Pointer to name of Temperature bank (usually
 *                          "Temperaturex" where x is bank number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
T_uezError UEZTemperatureOpen(
            const char *const aName, 
            T_uezDevice *aDevice);

/**
 *	End access to the Temperature bank.
 *
 *	@param [in]	aDevice		Device handle to close
 *
 *	@return		T_uezError
 */
T_uezError UEZTemperatureClose(T_uezDevice aDevice);

/**
 *	End access to the Temperature bank.
 *
 *	@param [in]		aDevice					Device handle to close
 * 	@param [out]	*aRequest   Place to store read temperature
 *																(signed 15.16 format)
 *
 *	@return		T_uezError
 */
T_uezError UEZTemperatureRead(
            T_uezDevice aDevice, 
            TInt32 *aRequest);

//! Integer part of a temperature
#define TEMPERATURE_INTEGER(x)  (x >> 16)
//! Factional part of a temperature given a range of 00 to 99
//! (representing 0.00 to 0.99)
#define TEMPERATURE_FRAC100(x)  (((((TUInt32)x) & 0xFFFF) * 100)>>16)

#endif // _UEZ_Temperature_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZTemperature.h
 *-------------------------------------------------------------------------*/
