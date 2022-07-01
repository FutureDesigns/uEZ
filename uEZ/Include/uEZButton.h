/**
 *	@file 	uEZButton.h
 *  @brief 	uEZ Button Interface
 *
 *	The uEZ interface which maps to lower level Button drivers.
 */
#ifndef UEZBUTTON_H_
#define UEZBUTTON_H_

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Open up access to the button bank.
 *
 *	@param[in]	*aName		Pointer to name of button bank (usually "ButtonX" 
 *							where X is bank number)
 *	@param[out]	*aDevice		Pointer to device handle to be returned
 *
 *	@return	T_uezError
 */ 
T_uezError UEZButtonBankOpen(const char * const aName, T_uezDevice *aDevice);

/**
 *	End access to the button bank.
 *
 *	@param[in]	aDevice		Device handle of button bank to close
 *
 *	@return	T_uezError
 */ 
T_uezError UEZButtonBankClose(T_uezDevice aDevice);

/**
 *	Declare which buttons in the button bank are active.  A bit of 1
 *  means active, a bit of 0 means inactive.
 *
 *	@param[in]	aDevice		Device handle of button bank to set active
 *	@param[in]	aButtonsActive	Bits telling which buttons are active.
 *	
 *	@return	T_uezError
 */ 
T_uezError UEZButtonBankSetActive(T_uezDevice aDevice, TUInt32 aButtonsActive);

/*!
 *	Returns the buttons that have been pressed.
 *
 *	@param[in]	aDevice		Device handle of button bank to set active
 *	@param[in]	aButtonsActive	Bits telling which buttons are active.
 *	
 *	@return	T_uezError
 */ 
T_uezError UEZButtonBankRead(T_uezDevice aDevice, TUInt32 *aButtonsPressed);

#ifdef __cplusplus
}
#endif

#endif // UEZBUTTON_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZButton.h
 *-------------------------------------------------------------------------*/
