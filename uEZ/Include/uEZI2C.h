/*-------------------------------------------------------------------------*
* File:  uEZI2C.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ interface which maps to low level I2C drivers.
*-------------------------------------------------------------------------*/

/**
 *	@file 	uEZI2C.h
 *  @brief 	uEZ I2C Interface
 *
 *	The uEZ interface which maps to low level I2C drivers.
 */
#ifndef _UEZ_I2C_H_
#define _UEZ_I2C_H_

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
#include <Types/I2C.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *	Open up access to the I2C bus.
 *
 *	@param [in] 	*aName		Pointer to name of I2C bus (usually
 *                          "I2Cx" where x is bus number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *
 *	@return		T_uezError
 */
T_uezError UEZI2COpen(const char * const aName, T_uezDevice *aDevice);

/**
 *	End access to the I2C bus.
 *
 *	@param [in]	aDevice	Device handle to close
 *
 *	@return		T_uezError
 */
T_uezError UEZI2CClose(T_uezDevice aDevice);

/**
 *	Requests an I2C read only transaction with the given parameters.
 *
 *	@param [in]		aDevice				Handle to opened I2C bus device.
 *	@param [in]		aAddress			Slave I2C address
 *	@param [in]		aSpeed				Speed in kHz of I2C transaction 
 *                              (usually 100 or 400).
 *	@param [in]		*aData				Place to store read data
 *	@param [in]		aDataLength		Number of bytes to read
 *	@param [in]		aTimeout			General timeout to complete transaction
 *
 *	@return		T_uezError
 */
T_uezError UEZI2CRead(
    T_uezDevice aDevice,
    TUInt8 aAddress,
    TUInt32 aSpeed,
    TUInt8 *aData,
    TUInt8 aDataLength,
    TUInt32 aTimeout);
	
/**
 *	Requests an I2C write only transaction with the given parameters.
 *
 *	@param [in]		aDevice				Handle to opened I2C bus device.
 *	@param [in]		aAddress			Slave I2C address
 *	@param [in]		aSpeed				Speed in kHz of I2C transaction 
 *                              (usually 100 or 400).
 *	@param [in]		*aData				Pointer to data to write out
 *	@param [in]		aDataLength		Number of bytes to write
 *	@param [in]		aTimeout			General timeout to complete transaction
 *
 *	@return		T_uezError
 */
T_uezError UEZI2CWrite(
    T_uezDevice aDevice,
    TUInt8 aAddress,
    TUInt32 aSpeed,
    TUInt8 *aData,
    TUInt8 aDataLength,
    TUInt32 aTimeout);
		
/**
 *	Requests a I2C transfer of a write and/or read I2C transaction.
 *
 *	@param [in]				aDevice				Handle to opened I2C bus device.
 *	@param [in,out]		*aRequest			Structure of read and/or write I2C
 *                              		transaction information.
 *
 *	@return		T_uezError
 */
T_uezError UEZI2CTransaction(T_uezDevice aDevice, I2C_Request *aRequest);

/**
 *	Start an I2C device in slave mode
 *
 *	@param [in]		aDevice				Handle to opened I2C bus device.
 *	@param [in]		*aSetup				Structure containing setup details
 *
 *	@return		T_uezError
 */
T_uezError UEZI2CSlaveStart(T_uezDevice aDevice, const T_I2CSlaveSetup *aSetup);

/**
 *	I2C slave wait for event from master
 *
 *	@param [in]				aDevice				Handle to opened I2C bus device.
 *	@param [in,out]		*aEvent				Event data
 *	@param [in]				*aLength			Number of event data bytes
 *	@param [in]				aTimeout			Timeout in milliseconds
 *
 *	@return		T_uezError
 */
T_uezError UEZI2CSlaveWaitForEvent(
    T_uezDevice aDevice,
    T_I2CSlaveEvent *aEvent,
    TUInt32 *aLength,
    TUInt32 aTimeout);
		
/**
 *	Enable an I2C device
 *
 *	@param [in]				aDevice				Handle to opened I2C bus device.
 *
 *	@return		T_uezError
 */
T_uezError UEZI2CEnable(T_uezDevice aDevice);

/**
 *	Disable an I2C device
 *
 *	@param [in]				aDevice				Handle to opened I2C bus device.
 *
 *	@return		T_uezError
 */
T_uezError UEZI2CDisable(T_uezDevice aDevice);

T_uezError UEZI2CIsHung(T_uezDevice aDevice, TBool *aBool);

T_uezError UEZI2CResetBus(T_uezDevice);

#ifdef __cplusplus
}
#endif

#endif // _UEZ_I2C_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZI2C.h
 *-------------------------------------------------------------------------*/
