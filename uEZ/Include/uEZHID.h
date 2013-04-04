/**
 *	@file 	uEZHID.h
 *  @brief 	uEZ Human Interface Device
 *
 *	The uEZ interface which maps to low level HID drivers.
 */
#ifndef UEZHID_H_
#define UEZHID_H_

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

#endif // UEZHID_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZHID.h
 *-------------------------------------------------------------------------*/
