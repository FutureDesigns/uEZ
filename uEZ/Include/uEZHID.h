/*-------------------------------------------------------------------------*
* File:  uEZHID.h
*--------------------------------------------------------------------------*
* Description:
*		  uEZ Human Interface Device
*         The uEZ interface which maps to low level HID drivers.
*-------------------------------------------------------------------------*/
#ifndef UEZHID_H_
#define UEZHID_H_

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

#ifdef __cplusplus
extern "C" {
#endif


/**
 *	Open up access to the HID bank.
 *
 *	@param [out]	*aDevice	Pointer to device handle to be returned
 *	@param [in] 	*aHIDName		Pointer to name of Flash bus
 *	@param [in] 	aVenderID	Vender ID
 *	@param [out]	aProductID	Product ID
 *
 *	@return		T_uezError
 */
T_uezError UEZHIDOpen(
    T_uezDevice *aDevice,
    const char *aHIDName,
    TUInt16 aVenderID,
    TUInt16 aProductID);
		
/**
 *	Close up access to the HID bank.
 *
 *	@param [in]	aDevice	HID Device Handle to close
 *
 *	@return		T_uezError
 */
T_uezError UEZHIDClose(T_uezDevice aDevice);

/**
 *	Read bytes from a Human Interface Device
 *
 *	@param [in]		aDevice		HID Device Handle
 *	@param [out]	aData			Pointer to buffer to receive data
 *	@param [in]		aLength		Number of bytes to read
 *	@param [in]		aTimeout	Timeout in milliseconds
 *
 *	@return		T_uezError
 */
T_uezError UEZHIDRead(
    T_uezDevice aDevice,
    TUInt8 *aData,
    TUInt32 aLength,
    TUInt32 aTimeout);
		
/**
 *	Write bytes to a Human Interface Device
 *
 *	@param [in]		aDevice				HID Device Handle
 *	@param [in]		*aData				Pointer to the send data buffer
 *	@param [in]		aLength				Number of bytes to write
 *	@param [out]	*aNumWritten	Number of bytes written
 *	@param [in]		aTimeout			Timeout in milliseconds
 *
 *	@return		T_uezError
 */
T_uezError UEZHIDWrite(
    T_uezDevice aDevice,
    const TUInt8 *aData,
    TUInt32 aLength,
    TUInt32 *aNumWritten,
    TUInt32 aTimeout);

/**
 *	Get report descriptor from the Human Interface Device
 *
 *	@param [in]		aDevice				HID Device Handle
 *	@param [out]	*aData				Pointer to the data buffer
 *	@param [in]		aMaxLength		Number of bytes to store
 *
 *	@return		T_uezError
 */
T_uezError UEZHIDGetReportDescriptor(
    T_uezDevice aDevice,
    TUInt8 *aData,
    TUInt32 aMaxLength);


T_uezError UEZHIDGetFeatureReport(
    T_uezDevice aDevice,
    TUInt8 aFeature,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout);

T_uezError UEZHIDSetFeatureReport(
    T_uezDevice aDevice,
    TUInt8 aFeature,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout);

T_uezError UEZHIDGetString(
    T_uezDevice aDevice,
    TUInt32 aIndex,
    TUInt8 *aBuffer,
    TUInt32 aSize,
    TUInt32 aTimeout);

T_uezError UEZHIDGetInputReport(
    T_uezDevice aDevice,
    TUInt8 aFeature,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout);

T_uezError UEZHIDSetOutputReport(
    T_uezDevice aDevice,
    TUInt8 aFeature,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout);
	
#ifdef __cplusplus
}
#endif

#endif // UEZHID_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZHID.h
 *-------------------------------------------------------------------------*/
