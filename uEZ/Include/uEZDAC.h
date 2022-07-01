/*-------------------------------------------------------------------------*
* File:  uEZDAC.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to lower level DAC drivers.
*-------------------------------------------------------------------------*/
#ifndef _UEZ_DAC_H_
#define _UEZ_DAC_H_

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
 *	Open up access to the DAC bank.
 *
 *	@param [in]		*aName		Pointer to name of DAC bank (usually "DACx" 
 *													where x is bank number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return	T_uezError
 */
T_uezError UEZDACOpen(
            const char *const aName, 
            T_uezDevice *aDevice);

/**
 *	End access to the ADC bank.
 *
 *	@param [in]	aDevice		Device handle of DAC to close
 *
 *	@return	T_uezError
 */
T_uezError UEZDACClose(T_uezDevice aDevice);

/**
 *  @ingroup UEZDAC
 *	Turn on or off the bias voltage.
 *
 *	@param [in]	aDevice		Handle to opened DAC device.
 *	@param [in]	aBias			On (ETrue) or Off (EFalse)
 *
 *	@return	T_uezError
 */
T_uezError UEZDACSetBias(
            T_uezDevice aDevice,
            TBool aBias);
						
/**
 *  @ingroup UEZDAC
 *	Set internal voltage reference.
 *
 *	@param [in]	aDevice		Handle to opened DAC device.
 *	@param [in]	aVRef			Voltage reference in millivolts.
 *
 *	@return	T_uezError
 */
T_uezError UEZDACSetVRef(
            T_uezDevice aDevice,
            TUInt32 aVRef);

/**
 *  @ingroup UEZDAC
 *	Write value to the DAC in millivolts.
 *
 *	@param [in]	aDevice		Handle to opened DAC device.
 *	@param [in]	aV				Value in millivolts to be written to DAC
 *
 *	@return	T_uezError
 */						
T_uezError UEZDACWriteMilliVolts(
        T_uezDevice aDevice,
        TUInt32 aV);

#ifdef __cplusplus
}
#endif
	
#endif // _UEZ_DAC_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZDAC.h
 *-------------------------------------------------------------------------*/
