/*-------------------------------------------------------------------------*
* File:  uEZEINT.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to lower level external interrupt drivers.
*-------------------------------------------------------------------------*/
#ifndef _UEZ_EINT_H_
#define _UEZ_EINT_H_

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
#include <Types/EINT.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _UEZ_EINT_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZEINT.h
 *-------------------------------------------------------------------------*/
