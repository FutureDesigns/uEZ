/**
 *	@file 	uEZButton.h
 *  @brief 	uEZ Button Interface
 *
 *	The uEZ interface which maps to lower level Button drivers.
 */
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
 #ifndef UEZBUTTON_H_
#define UEZBUTTON_H_

#include <uEZTypes.h>

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

#endif // UEZBUTTON_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZButton.h
 *-------------------------------------------------------------------------*/
