/*-------------------------------------------------------------------------*
* File:  uEZEEPROM.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ EEPROM interface.
*-------------------------------------------------------------------------*/
#ifndef _UEZ_EEPROM_H_
#define _UEZ_EEPROM_H_

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
 *	Open up access to the EEPROM.
 *
 *	@param [in] 	*aName		Pointer to name of EEPROM(usually
 *                              "EEPROMx" where x is bus number)
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *	@return			T_uezError
 */
T_uezError UEZEEPROMOpen(const char * const aName, T_uezDevice *aDevice);

/**
 *	End access to the EEPROM.
 *
 *	@param [in]		aDevice			Device handle to close
 *	@return			T_uezError
 */
T_uezError UEZEEPROMClose(T_uezDevice aDevice);

/**
 *	Requests an EEPROM read with the given parameters.
 *
 *      @param [in]		aDevice         Handle to opened EEPROM device
 *		@param [out]	*aData			Place to store read data
 *      @param [in]     aAddress        Address of data to read
 *		@param [in]		aDataLength		Number of bytes to read
 *
 *		@return			T_uezError
 */
T_uezError UEZEEPROMRead(
    T_uezDevice aDevice,
    TUInt8 *aData,
    TUInt32 aAddress,
    TUInt32 aDataLength);
	
/**
 *	Requests an EEPROM write with the given parameters.
 *
 *	@param [in]		aDevice     Handle to opened EEPROM device
 *	@param [in]		*aData		Pointer to data to write out
 *  @param [in]     aAddress    Address to write data
 *	@param [in]		aDataLength	Number of bytes to write
 *
 *	@return			T_uezError
 */
T_uezError UEZEEPROMWrite(
    T_uezDevice aDevice,
    TUInt8 *aData,
    TUInt32 aAddress,
    TUInt32 aDataLength);

#ifdef __cplusplus
}
#endif
	
#endif // _UEZ_EEPROM_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZEEPROM.h
 *-------------------------------------------------------------------------*/
