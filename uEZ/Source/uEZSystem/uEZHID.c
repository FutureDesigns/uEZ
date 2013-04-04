/*-------------------------------------------------------------------------*
 * File:  uEZHID.c
 *-------------------------------------------------------------------------*
 * Description: USB Human Interface Device interface
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZHID
 *  @{
 *  @brief     uEZ HID Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ HID (Human Interface Device) interface.
 *
 * @par Example code:
 * Example task to test the HID functions
 * @par
 * @code
 * #include <uEZ.h>
 * #include <Device/HID.h>
 * #include <Device/USBHost.h>
 *
 * TUInt32 HIDTest(T_uezTask aMyTask, void *aParams)
 * {
 *  T_uezDevice HID;
 *  TUInt32 aNumWritten;
 *  TUInt8 aData[512];
 *  // 0x046d is logitech vendor ID for their HID devices,  0xc52b is product id for unifying receivers
 *  if (UEZHIDOpen(&HID, "Logitech Receiver", 0x0789, 0xc52b) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *	    UEZHIDGetReportDescriptor(HID, aData, 5); // find out capabilities of device
 *      UEZHIDRead(HID, aData, 20,  1000);
 *	    UEZHIDWrite(HID, aData, 20, &aNumWritten, 1000);
 *      if (UEZHIDClose(HID) != UEZ_ERROR_NONE) {
 *            // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  return 0;
 * }
 * @endcode
 */
#include <stdio.h>
#include <uEZ.h>
#include <Device/HID.h>
#include <Device/USBHost.h>
#include <uEZDeviceTable.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*
 * Routine:  UEZHIDOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open the HID.
 *
 *  @param [in]    aDevice		Device handle
 *
 *  @param [in]    *aHIDName	String for name of HID
 *
 *  @param [in]    aVenderID	USB HID Vendor ID
 *
 *  @param [in]    aProductID	USB HID Product ID
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Device/HID.h>
 *  #include <Device/USBHost.h>
 *
 *  T_uezDevice HID;
 *  // 0x046d is logitech vendor ID for their HID devices,  0xc52b is product id for unifying receivers
 *  if (UEZHIDOpen(&HID, "Logitech Receiver", 0x0789, 0xc52b) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZHIDOpen(
    T_uezDevice *aDevice,
    const char *aHIDName,
    TUInt16 aVenderID,
    TUInt16 aProductID)
{
    T_uezError error;
    DEVICE_HID **p;

    error = UEZDeviceTableFind(aHIDName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Open((void *)p, aVenderID, aProductID);
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZHIDClose
 *---------------------------------------------------------------------------*/
/**
 *  Close the HID.
 *
 *  @param [in]    aDevice		Device handle
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Device/HID.h>
 *  #include <Device/USBHost.h>
 *
 *  T_uezDevice HID;
 *  // 0x046d is logitech vendor ID for their HID devices,  0xc52b is product id for unifying receivers
 *  if (UEZHIDOpen(&HID, "Logitech Receiver", 0x0789, 0xc52b) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *      if (UEZHIDClose(HID) != UEZ_ERROR_NONE) {
 *            // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZHIDClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_HID **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Close((void *)p);
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZHIDRead
 *---------------------------------------------------------------------------*/
/**
 *  Read data from USB HID
 *
 *  @param [in]    aDevice		Device handle
 *
 *  @param [in]    *aData		Pointer to buffer to store read data
 *
 *  @param [in]    aLength		Length of data to read
 * 
 *  @param [in]    aTimeout		Timeout value in ms
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Device/HID.h>
 *  #include <Device/USBHost.h>
 *
 *  T_uezDevice HID;
 *  TUInt8 aData[512];
 *  // 0x046d is logitech vendor ID for their HID devices,  0xc52b is product id for unifying receivers
 *  if (UEZHIDOpen(&HID, "Logitech Receiver", 0x0789, 0xc52b) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *	    UEZHIDGetReportDescriptor(HID, aData, 5); // find out capabilities of device
 *      UEZHIDRead(HID, aData, 20,  1000);
 *      if (UEZHIDClose(HID) != UEZ_ERROR_NONE) {
 *            // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZHIDRead(
    T_uezDevice aDevice,
    TUInt8 *aData,
    TUInt32 aLength,
    TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_HID **p;
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Read((void *)p, aData, aLength, aTimeout);
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZHIDWrite
 *---------------------------------------------------------------------------*/
/**
 *  Write data to USB HID
 *
 *  @param [in]    aDevice		Device handle
 *
 *  @param [in]    *aData		Pointer to buffer with data to write
 *
 *  @param [in]    aLength		Length of data to write
 *
 *  @param [in]    *aNumWritten	Pointer to return amount of data actually written
 *
 *  @param [in]    aTimeout		Timeout value in ms
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Device/HID.h>
 *  #include <Device/USBHost.h>
 *
 *  T_uezDevice HID;
 *  TUInt32 aNumWritten;
 *  TUInt8 aData[512];
 *  // 0x046d is logitech vendor ID for their HID devices,  0xc52b is product id for unifying receivers
 *  if (UEZHIDOpen(&HID, "Logitech Receiver", 0x0789, 0xc52b) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *	    UEZHIDGetReportDescriptor(HID, aData, 5); // find out capabilities of device
 *	    UEZHIDWrite(HID, aData, 20, &aNumWritten, 1000);
 *      if (UEZHIDClose(HID) != UEZ_ERROR_NONE) {
 *            // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZHIDWrite(
    T_uezDevice aDevice,
    const TUInt8 *aData,
    TUInt32 aLength,
    TUInt32 *aNumWritten,
    TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_HID **p;
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Write((void *)p, aData, aLength, aNumWritten, aTimeout);
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZHIDGetReportDescriptor
 *---------------------------------------------------------------------------*/
/**
 *  Ready USB device descriptor to determine devices features/capabilities
 *
 *  @param [in]    aDevice		Device handle
 *
 *  @param [in]    *aData		Pointer to buffer to store descriptor data
 *
 *  @param [in]    aMaxLength	max length of descriptor data to read
 *
 *  @return        T_uezError	Error
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Device/HID.h>
 *  #include <Device/USBHost.h>
 *
 *  T_uezDevice HID;
 *  TUInt8 aData[512];
 *  // 0x046d is logitech vendor ID for their HID devices,  0xc52b is product id for unifying receivers
 *  if (UEZHIDOpen(&HID, "Logitech Receiver", 0x0789, 0xc52b) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *		UEZHIDGetReportDescriptor(HID, aData, 5); // find out capabilities of device
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZHIDGetReportDescriptor(
    T_uezDevice aDevice,
    TUInt8 *aData,
    TUInt32 aMaxLength)
{
    T_uezError error;
    DEVICE_HID **p;
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetReportDescriptor((void *)p, aData, aMaxLength);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZHID.c
 *-------------------------------------------------------------------------*/
