/*-------------------------------------------------------------------------*
 * File:  uEZHID.c
 *-------------------------------------------------------------------------*
 * Description: USB Human Interface Device interface
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
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
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
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

T_uezError UEZHIDGetFeatureReport(
    T_uezDevice aDevice,
    TUInt8 aFeature,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_HID **p;
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetFeatureReport((void *)p, aFeature, aData, aMaxLength, aTimeout);
}

T_uezError UEZHIDSetFeatureReport(
    T_uezDevice aDevice,
    TUInt8 aFeature,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_HID **p;
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetFeatureReport((void *)p, aFeature, aData, aMaxLength, aTimeout);
}

T_uezError UEZHIDGetInputReport(
    T_uezDevice aDevice,
    TUInt8 aFeature,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_HID **p;
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetInputReport((void *)p, aFeature, aData, aMaxLength, aTimeout);
}

T_uezError UEZHIDSetOutputReport(
    T_uezDevice aDevice,
    TUInt8 aFeature,
    TUInt8 *aData,
    TUInt32 aMaxLength,
    TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_HID **p;
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetOutputReport((void *)p, aFeature, aData, aMaxLength, aTimeout);
}

T_uezError UEZHIDGetString(
        T_uezDevice aDevice,
        TUInt32 aIndex,
        TUInt8 *aBuffer,
        TUInt32 aSize,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_HID **p;
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetString((void *)p, aIndex, aBuffer, aSize, aTimeout);
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZHID.c
 *-------------------------------------------------------------------------*/
