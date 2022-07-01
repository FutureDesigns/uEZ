/*-------------------------------------------------------------------------*
* File:  uEZPWM.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to low level PWM drivers.
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZPWM.h
 *  @brief 	uEZ PWM Interface
 *
 *	The uEZ interface which maps to low level PWM drivers.
 */
#ifndef _UEZ_PWM_H_
#define _UEZ_PWM_H_

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
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <uEZErrors.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Open up access to the PWM Bank.
 *
 *	@param [in] 	*aName		Pointer to name of PWM bank (usually
 *                          "PWMx" where x is bank number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
 T_uezError UEZPWMOpen(
            const char *const aName, 
            T_uezDevice *aDevice);
						
/**
 *	End access to the PWM Bank.
 *
 *	@param [in]	aDevice		Opened Flash Device handle
 *
 *	@return		T_uezError
 */
T_uezError UEZPWMClose(T_uezDevice aDevice);

/**
 *	PWM is going to start -- chip select enabled for device
 *
 *	@param [in]	aDevice						Opened PWM Device handle
 *	@param [in]	aMatchRegIndex		Which register is used for master counter
 *	@param [in]	aPeriod						Number of Fpclk intervals per period
 *
 *	@return		T_uezError
 */
T_uezError UEZPWMSetMaster(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex,
            TUInt32 aPeriod);
						
/**
 *	Configure a match register for this PWM bank.
 *
 *	@param [in]	aDevice						Opened PWM Device handle
 *	@param [in]	aMatchRegIndex		Index to match register (0-7)
 *	@param [in]	aMatchPoint				Number of PWM cycles until match
 *
 *	@return		T_uezError
 */
T_uezError UEZPWMSetMatchReg(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex,
            TUInt32 aMatchPoint);

/**
 *	PWM is going to have a single edge output (high until matches, then
 *  low, reset counter goes back to high).
 *
 *	@param [in]	aDevice						Opened PWM Device handle
 *	@param [in]	aMatchRegIndex		Index to match register (0-7)
 *
 *	@return		T_uezError
 */						
T_uezError UEZPWMEnableSingleEdgeOutput(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex);
						
/**
 *	Disable a match register
 *
 *	@param [in]	aDevice						Opened PWM Device handle
 *	@param [in]	aMatchRegIndex		Index to match register (0-7)
 *
 *	@return		T_uezError
 */	
T_uezError UEZPWMDisableOutput(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex);
			

/**
 *	Set a match callback directive for the given PWM Device and match reg. index
 *
 *	@param [in]	aDevice				Opened PWM Device handle
 *	@param [in]	aMatchRegIndex		Index to match register (0-7)
 *	@param [in]	aCallback			Callback function pointer
 *	@param [in]	aCallbackWorkspace	Callback Workspace
 *
 *	@return		T_uezError
 */				
T_uezError UEZPWMSetMatchCallback(
            T_uezDevice aDevice,
            TUInt8 aMatchRegIndex,
			void (*aCallback)(void *aCallbackWorkspace),
        	void *aCallbackWorkspace);

#ifdef __cplusplus
}
#endif
	
#endif // _UEZ_PWM_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZPWM.h
 *-------------------------------------------------------------------------*/
