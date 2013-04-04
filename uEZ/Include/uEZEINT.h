/**
 *	@file 	uEZEINT.h
 *  @brief 	uEZ External Interrupt Interface
 *
 *	The uEZ interface which maps to lower level external interrupt drivers.
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
#ifndef _UEZ_EINT_H_
#define _UEZ_EINT_H_

#include <uEZTypes.h>
#include <uEZErrors.h>
#include <Types/EINT.h>

/**
 *	Open up access to the EINT bank and setup a channel.
 *
 *	@param[in]	*aName			Pointer to name of EINT bank (usually
 *                        	"EINTx" where x is bank number)
 *	@param[out]	*aDevice		Pointer to device handle to be returned
 *                        	"EINTx" where x is bank number)
 *	@param[out]	aChannel		Channel number
 *	@param[out]	aTrigger		Trigger number
 *	@param[out]	aPriority		Priority
 *	@param[out]	aCallBackFunc		Call back function
 *	@param[out]	*aCallbackWorkspace		Pointer to call back workspace
 
 *
 *
 *	@return	T_uezError
 */ 
T_uezError UEZEINTOpen(const char *const aName,
                       T_uezDevice *aDevice,
                       TUInt32 aChannel,
                       T_EINTTrigger aTrigger,
                       T_EINTPriority aPriority,
                       EINT_Callback aCallBackFunc,
                       void *aCallbackWorkspace);
											 
/**
 *	End access to the EINT bank.
 *
 *	@param[in]	aDevice		Device handle of button bank to close
 *
 *	@return	T_uezError
 */ 
T_uezError UEZEINTClose(T_uezDevice aDevice);

/**
 *	Reset an EINT channel
 *
 *	@param[in]	aDevice		Device handle of the EINT
 *	@param[in]	aChannel	Channel Number
 *
 *	@return	T_uezError
 */ 
T_uezError UEZEINTReset(T_uezDevice aDevice, TUInt32 aChannel);

/**
 *	Clears the settings for an EINT channel
 *
 *	@param[in]	aDevice		Device handle of the EINT
 *	@param[in]	aChannel	Channel Number
 *
 *	@return	T_uezError
 */ 
T_uezError UEZEINTClear(T_uezDevice aDevice, TUInt32 aChannel);

/**
 *	Enables an EINT channel
 *
 *	@param[in]	aDevice		Device handle of the EINT
 *	@param[in]	aDevice		Device handle of the EINT
 *	@param[in]	aChannel	Channel Number
 *
 *	@return	T_uezError
 */ 
T_uezError UEZEINTDisable(T_uezDevice aDevice, TUInt32 aChannel);


#endif // _UEZ_EINT_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZEINT.h
 *-------------------------------------------------------------------------*/
