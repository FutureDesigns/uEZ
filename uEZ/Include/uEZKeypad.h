/**
 *	@file 	uEZKeypad.h
 *  @brief 	uEZ Keypad Interface
 *
 *	The uEZ interface which maps to low level Keypad drivers.
 */
#ifndef _UEZ_Keypad_H_
#define _UEZ_Keypad_H_

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
#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/InputEvent.h>

/**
 *	Open up access to the Keypad bank.
 *
 *	@param [in] 	*aName		Pointer to name of Keypad (usually "Keypad")
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
T_uezError UEZKeypadOpen(
            const char *const aName, 
            T_uezDevice *aDevice);
						
/**
 *	End access to the Keypad bank.
 *
 *	@param [in]	aDevice		Keypad Device Handle
 *
 *	@return		T_uezError
 */
T_uezError UEZKeypadClose(T_uezDevice aDevice);

/**
 *	End access to the Keypad bank.
 *
 *	@param [in]		aDevice		Keypad Device Handle
 *	@param [out]	*aEvent		Keypad event
 *	@param [in]		aTimeout	Timeout in milliseconds
 *
 *	@return		T_uezError
 */
T_uezError UEZKeypadRead(
            T_uezDevice aDevice, 
            T_uezInputEvent *aEvent,
            TUInt32 aTimeout);

#endif // _UEZ_Keypad_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZKeypad.h
 *-------------------------------------------------------------------------*/
