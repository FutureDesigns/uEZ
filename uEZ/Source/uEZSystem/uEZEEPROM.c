/*-------------------------------------------------------------------------*
 * File:  uEZEEPROM.c
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZEEPROM Interface
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
/**
 *  @addtogroup uEZEEPROM
 *  @{
 *  @brief     uEZ uEZEEPROM Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ uEZEEPROM interface.
 *
 * @par Example code:
 * Example task to store and load data from EEPROM
 * @par
 * @code
 * #include <uEZ.h>
 * #include <uEZEEPROM.h>
 *
 * TUInt32 EEPROMReadWrite(T_uezTask aMyTask, void *aParams)
 * {
 *      TUInt8 dataWrite[20];
 *      TUInt8 dataRead[5];
 *      T_uezDevice eeprom;
 *     	if (UEZEEPROMOpen("EEPROM0", &eeprom) == UEZ_ERROR_NONE) {
 *          // the device opened properly
 *
 *          // read data from EEPROMaddress 0
 *          if(UEZEEPROMRead(eeprom, (TUInt8 *)&dataRead, 0, 5) != UEZ_ERROR_NONE){
 *                // error reading eeprom
 *          }
 *          
 *          // write data to EEPROM address 0
 *          if(UEZEEPROMWrite(eeprom, (TUInt8 *)&dataWrite, 0, 20) != UEZ_ERROR_NONE){
 *                // error writing eeprom
 *          }
 *          
 *          if (UEZEEPROMClose(eeprom) != UEZ_ERROR_NONE) {
 *              // an error occurred
 *          }
 *      } else {
 *          // an error occurred opening EEPROM
 *      }
 *     return 0;
 * }
 * @endcode
 */

#include <uEZ.h>
#include <Device/EEPROM.h>
#include <uEZDeviceTable.h>
#include <uEZEEPROM.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZEEPROMOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the EEPROM.
 *
 *  @param [in]    *aName 	 Pointer to name of EEPROM (usually
 *                                  "EEPROMx" where x is bus number)
 *  @param [in]    *aDevice      Pointer to device handle to be returned
 *
 *  @return        T_uezError    If the device is opened, returns 
 *                               UEZ_ERROR_NONE.  If the device cannot be 
 *                               found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZEEPROM.h>
 *
 *  T_uezDevice eeprom;
 *  if (UEZEEPROMOpen("EEPROM0", &eeprom) == UEZ_ERROR_NONE) {
 *     // the device opened properly
 *  
 *  } else {
 *       // an error occurred opening EEPROM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZEEPROMOpen(const char * const aName, T_uezDevice *aDevice)
{
    DEVICE_EEPROM **p_eeprom;
    T_uezError error;

    // Find the eeprom0 device
    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    // look up the workspace, return any errors
    return UEZDeviceTableGetWorkspace(*aDevice,
        (T_uezDeviceWorkspace **)&p_eeprom);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZEEPROMClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the EEPROM.
 *
 *  @param [in]    aDevice 	    Device handle of EEPROM to close
 *
 *  @return        T_uezError       If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZEEPROM.h>
 *
 *  T_uezDevice eeprom;
 *  if (UEZEEPROMOpen("EEPROM0", &eeprom) == UEZ_ERROR_NONE) {
 *     // the device opened properly
 *  
 *     if (UEZEEPROMClose(eeprom) != UEZ_ERROR_NONE) {
 *         // an error occurred
 *     }
 *  } else {
 *       // an error occurred opening EEPROM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZEEPROMClose(T_uezDevice aDevice)
{
    DEVICE_EEPROM **p_eeprom;

    // Look up its workspace, return any errors
    return UEZDeviceTableGetWorkspace(aDevice,
        (T_uezDeviceWorkspace **)&p_eeprom);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZEEPROMRead
 *---------------------------------------------------------------------------*/
/**
 *  Read the EEPROM.
 *
 *  @param [in]    aDevice          Handle to opened EEPROM device
 *
 *  @param [out]   *aData           Place to store read data
 *
 *  @param [in]    aAddress         Address of data to read
 *
 *  @param [in]    aDataLength      Number of bytes to read
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.  If a timeout,
 *                                  returns UEZ_ERROR_TIMEOUT.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZEEPROM.h>
 *
 *  T_uezDevice eeprom;
 *  TUInt8 dataRead[5]; // array that will store data thta is read from EEPROM
 *  if (UEZEEPROMOpen("EEPROM0", &eeprom) == UEZ_ERROR_NONE) {
 *     // the device opened properly
 *  
 *    // Read data from EEPROM address 0
 *    if(UEZEEPROMRead(eeprom, (TUInt8*)dataRead, 0, 5); == UEZ_ERROR_NONE){
 *        // Read successfull
 *    }
 *  } else {
 *       // an error occurred opening EEPROM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZEEPROMRead(
    T_uezDevice aDevice,
    TUInt8* aData,
    TUInt32 aAddress,
    TUInt32 aDataLength)
{
    T_uezError error;
    DEVICE_EEPROM **p_eeprom;

    // Look up its workspace, return any errors
    error = UEZDeviceTableGetWorkspace(aDevice,
        (T_uezDeviceWorkspace **)&p_eeprom);
    if (error)
        return error;

    // Store the data and return any errors;
    return (*p_eeprom)->Read(p_eeprom, aAddress, aData, aDataLength);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZEEPROMWrite
 *---------------------------------------------------------------------------*/
/**
 *  Write data to the EERPROM.
 *
 *  @param [in]    aDevice          Handle to opened EEPROM device
 *
 *  @param [in]    *aData           Pointer to data to write out   
 *
 *  @param [in]    aAddress         address to write data
 *
 *  @param [in]    aDataLength      Number of bytes to write
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.  If a timeout,
 *                                  returns UEZ_ERROR_TIMEOUT.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZEEPROM.h>
 *
 *  T_uezDevice eeprom;
 *  TUInt8 dataWrite[20]; // array that will store data to write to EEPROM
 *  if (UEZEEPROMOpen("EEPROM0", &eeprom) == UEZ_ERROR_NONE) {
 *     // the device opened properly
 *
 *    // write data to EEPROM address 0
 *    if(UEZEEPROMWrite(eeprom, (TUInt8*)dataWrite, 0, 20) == UEZ_ERROR_NONE){
 *        // Write successfull
 *    }
 *  } else {
 *       // an error occurred opening EEPROM
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZEEPROMWrite(
    T_uezDevice aDevice,
    TUInt8 *aData,
    TUInt32 aAddress,
    TUInt32 aDataLength)
{
    T_uezError error;
    DEVICE_EEPROM **p_eeprom;

    // Look up its workspace, return any errors
    error = UEZDeviceTableGetWorkspace(aDevice,
        (T_uezDeviceWorkspace **)&p_eeprom);
    if (error)
        return error;

    // Save the data and return any errors
    return ((*p_eeprom)->Write(p_eeprom, aAddress, aData, aDataLength));
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZEEPROM.c
 *-------------------------------------------------------------------------*/
