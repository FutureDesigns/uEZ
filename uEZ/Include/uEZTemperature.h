/*-------------------------------------------------------------------------*
* File:  uEZTemperature.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to low level Temperature drivers.
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZTemperature.h
 *  @brief 	uEZ Temperature Interface
 *
 *	The uEZ interface which maps to low level Temperature drivers.
 */
#ifndef _UEZ_Temperature_H_
#define _UEZ_Temperature_H_

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
#include <uEZTypes.h>
#include <uEZErrors.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _UEZ_Temperature_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZTemperature.h
 *-------------------------------------------------------------------------*/
