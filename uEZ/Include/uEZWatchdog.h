/*-------------------------------------------------------------------------*
* File:  uEZWatchdog.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to low level Watchdog drivers.
*-------------------------------------------------------------------------*/
/**
 *	@file 	uEZWatchdog.h
 *  @brief 	uEZ Watchdog Interface
 *
 *	The uEZ interface which maps to low level Watchdog drivers.
 */
#ifndef _UEZ_Watchdog_H_
#define _UEZ_Watchdog_H_

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
 *	Open up access to the Watchdog bank.
 *
 *	@param [in] 	*aName		Pointer to name of Watchdog bank (usually
 *                          "Watchdogx" where x is bank number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
T_uezError UEZWatchdogOpen(
            const char *const aName, 
            T_uezDevice *aDevice);
						
/**
 *	End access to the Watchdog bank.
 *
 *	@param [in]	aDevice		Device handle to close
 *
 *	@return		T_uezError
 */
T_uezError UEZWatchdogClose(T_uezDevice aDevice);

/**
 *	Set the maximum amount of time that can go by before feeding the
 *  watchdog.
 *
 *	@param [in]	aDevice		Opened Watchdog Device handle
 *	@param [in]	aMaxTime	Time til watchdog expires in
 *                        milliseconds.
 *
 *	@return		T_uezError
 */
T_uezError UEZWatchdogSetMaxTime(
            T_uezDevice aDevice,
            TUInt32 aMaxTime);
						
/**
 *	Set the minimum amount of time that can go by before feeding the
 *  watchdog.
 *
 *	@param [in]	aDevice		Opened Watchdog Device handle
 *	@param [in]	aMinTime  Time needed before feeding watchdog
 *                        milliseconds.
 *
 *	@return		T_uezError
 */
T_uezError UEZWatchdogSetMinTime(
            T_uezDevice aDevice,
            TUInt32 aMinTime);
						
/**
 *	Check if the system has previously been reset due to the watchdog
 *
 *	@param [in]	aDevice		Opened Watchdog Device handle
 *
 *	@return		T_uezError
 */
TBool UEZIsResetFromWatchdog(T_uezDevice aDevice);
						
/**
 *	Clear the state of the reset flag that says the last reboot
 *  was from a watchdog reset.
 *
 *	@param [in]	aDevice		Opened Watchdog Device handle
 *
 *	@return		T_uezError
 */
void UEZWatchdogClearResetFlag(T_uezDevice aDevice);
						
/**
 *	Determine if the watchdog is actively running.
 *
 *	@param [in]	aDevice		Opened Watchdog Device handle
 *
 *	@return		T_uezError
 */
TBool UEZWatchdogIsActive(T_uezDevice aDevice);
						
/**
 *	Start the watchdog
 *
 *	@param [in]	aDevice		Opened Watchdog Device handle
 *
 *	@return		T_uezError
 */
T_uezError UEZWatchdogStart(T_uezDevice aDevice);
						
/**
 *	Start the watchdog.
 *
 *	@param [in]	aDevice		Opened Watchdog Device handle
 *
 *	@return		T_uezError
 */
T_uezError UEZWatchdogFeed(T_uezDevice aDevice);
						
/**
 *	Purposely cause the watchdog to be tripped.
 *
 *	@param [in]	aDevice		Opened Watchdog Device handle
 *
 *	@return		T_uezError
 */
T_uezError UEZWatchdogTrip(T_uezDevice aDevice);

#ifdef __cplusplus
}
#endif

#endif // _UEZ_Watchdog_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZWatchdog.h
 *-------------------------------------------------------------------------*/
