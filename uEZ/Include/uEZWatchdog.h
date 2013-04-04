/**
 *	@file 	uEZWatchdog.h
 *  @brief 	uEZ Watchdog Interface
 *
 *	The uEZ interface which maps to low level Watchdog drivers.
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

#ifndef _UEZ_Watchdog_H_
#define _UEZ_Watchdog_H_

#include <uEZTypes.h>
#include <uEZErrors.h>

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

#endif // _UEZ_Watchdog_H_

/*-------------------------------------------------------------------------*
 * End of File:  uEZWatchdog.h
 *-------------------------------------------------------------------------*/
