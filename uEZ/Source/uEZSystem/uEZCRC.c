/*-------------------------------------------------------------------------*
 * File:  uEZCRC.c
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZCRC
 *  @{
 *  @brief     uEZ CRC Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ CRC API
 * Implementation:
 *      Most of the commands are thin and map directly to the 
 *      API of the CRC device drivers.
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 * #include <uEZCRC.h>
 *
 * TUInt32 Function(T_uezTask aMyTask, void *aParams)
 * {
 *  T_uezDevice crc;
 *  TUInt32 data[8]; 
 *  if(UEZCRCOpen("CRC", &crc) == UEZ_ERROR_NONE){
 *    	// the device opened properly
 *	
 *      UEZCRCComputeData(crc, (void *) &aData, UEZ_CRC_DATA_SIZE_UINT32, 8);
 *
 *      if(UEZCRCClose(crc) != UEZ_ERROR_NONE){
 *            // error closing the device
 *      }
 *  }else{
 *      // an error occurred opening the device
 *  }
 *     return 0;
 * }
 * @endcode
 */

#include "Config.h"
#include "uEZCRC.h"
#include "uEZDevice.h"
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <Device/CRC.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZCRCOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the CRC.
 *
 *  @param [in]    *aName 			Pointer to name of CRC
 *
 *  @param [in]    *aDevice			Pointer to device handle to be returned
 *
 *  @return         T_uezError    	If the device is opened, returns 
 *                                  UEZ_ERROR_NONE.  If the device cannot be 
 *                                  found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZCRC.h>
 *  T_uezDevice crc;
 *  if(UEZCRCOpen("CRC", &crc) == UEZ_ERROR_NONE){
 *    	// the device opened properly
 *  }else{ // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCRCOpen(const char * const aName, T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_CRC **p;

    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Open((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCRCClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the CRC.  
 *
 *  @param [in]    aDevice 		Device handle of CRC to close
 *
 *  @return        T_uezError   If the device is successfully closed, returns
 * 								UEZ_ERROR_NONE.  If the device handle is bad, 
 * 								returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZCRC.h>
 *  T_uezDevice crc;
 *  if(UEZCRCOpen("CRC", &crc) == UEZ_ERROR_NONE){
 *    	// the device opened properly
 *	if(UEZCRCClose(crc) != UEZ_ERROR_NONE){ // error closing the device
 *      }
 *  }else{  // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCRCClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_CRC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Close((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCRCSetComputationType
 *---------------------------------------------------------------------------*/
/**
 *  Sets the computation type
 *
 *  @param [in]    aDevice 		Device handle of CRC
 *
 *  @param [in]    aType		Type of computation
 *
 *  @return        T_uezError   If successful, returns UEZ_ERROR_NONE. If the
 *								device handle is bad,returns 
 *                              UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZCRC.h>
 *  T_uezDevice crc;
 *  if(UEZCRCOpen("CRC", &crc) == UEZ_ERROR_NONE){
 *    	// the device opened properly
 *      UEZCRCSetComputationType(crc, UEZ_CRC_32);
 *  }else{ // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCRCSetComputationType(
    T_uezDevice aDevice,
    T_uezCRCComputationType aType)
{
    T_uezError error;
    DEVICE_CRC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetComputationType((void *)p, aType);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCRCAddModifier
 *---------------------------------------------------------------------------*/
/**
 *  Add the specified modifier
 *
 *  @param [in]    aDevice 		Device handle of CRC
 *
 *  @param [in]    aModifier	Modifier to add
 *
 *  @return        T_uezError   If successful, returns UEZ_ERROR_NONE. If the
 *								device handle is bad,returns 
 *                              UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZCRC.h>
 *  T_uezDevice crc;
 *  if(UEZCRCOpen("CRC", &crc) == UEZ_ERROR_NONE){
 *    	// the device opened properly
 *      UEZCRCAddModifier(crc, UEZ_CRC_MODIFIER_CHECKSUM_ONES_COMPLIMENT);
 *  }else{ // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCRCAddModifier(T_uezDevice aDevice, T_uezCRCModifier aModifier)
{
    T_uezError error;
    DEVICE_CRC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->AddModifier((void *)p, aModifier);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCRCRemoveModifier
 *---------------------------------------------------------------------------*/
/**
 * 	Remove the specified modifier
 *
 *  @param [in]    aDevice 		Device handle of CRC
 *
 *  @param [in]    aModifier	Modifier to remove 
 *
 *  @return        T_uezError   If successful, returns UEZ_ERROR_NONE. If the
 *								device handle is bad,returns 
 *                              UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZCRC.h>
 *  T_uezDevice crc;
 *  if(UEZCRCOpen("CRC", &crc) == UEZ_ERROR_NONE){
 *    	// the device opened properly
 *      UEZCRCRemoveModifier(crc, UEZ_CRC_MODIFIER_CHECKSUM_ONES_COMPLIMENT);
 *  }else{ // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCRCRemoveModifier(T_uezDevice aDevice, T_uezCRCModifier aModifier)
{
    T_uezError error;
    DEVICE_CRC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->RemoveModifier((void *)p, aModifier);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCRCSetSeed
 *---------------------------------------------------------------------------*/
/**
 *  Set the seed
 *
 *  @param [in]    aDevice 		Device handle of CRC
 *
 *  @param [in]    aSeed		Seed
 *
 *  @return        T_uezError   If successful, returns UEZ_ERROR_NONE. If the
 *								device handle is bad,returns 
 *                              UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZCRC.h>
 *  T_uezDevice crc;
 *  if(UEZCRCOpen("CRC", &crc) == UEZ_ERROR_NONE){
 *    	// the device opened properly
 *      UEZCRCSetSeed(crc, 1337);
 *  }else{ // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCRCSetSeed(T_uezDevice aDevice, TUInt32 aSeed)
{
    T_uezError error;
    DEVICE_CRC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetSeed((void *)p, aSeed);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCRCComputeData
 *---------------------------------------------------------------------------*/
/**
 *  Compute CRC of data
 *
 *  @param [in]  aDevice 			Device handle of CRC to close 
 *
 *  @param [in]	 *aData				Pointer to data to compute on
 *
 *  @param [in]  aSize 	 			Size of each data element
 *
 *  @param [in]  aNumDataElements   Number of data items to calculate on
 *
 *  @return      T_uezError         If successful, returns UEZ_ERROR_NONE. If 
 *								    the device handle is bad,returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZCRC.h>
 *  T_uezDevice crc;
 *  TUInt32 data[8]; 
 *  if(UEZCRCOpen("CRC", &crc) == UEZ_ERROR_NONE){
 *    	// the device opened properly
 *      UEZCRCComputeData(crc, (void *) &aData, UEZ_CRC_DATA_SIZE_UINT32, 8);
 *  }else{ // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCRCComputeData(
    T_uezDevice aDevice,
    void *aData,
    T_uezCRCDataElementSize aSize,
    TUInt32 aNumDataElements/*, 
     TUInt32 ulPolynomial,  //  @param [in]  ulPolynomial       Polynomial to use
     TUInt8 polynomialLength*/) //  @param [in]  polynomialLength 	Length of polynomial (8, 16, 32, etc)
{
    T_uezError error;
    DEVICE_CRC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->ComputeData((void *)p, aData, aSize, aNumDataElements/*, ulPolynomial,
     polynomialLength*/);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCRCReadChecksum
 *---------------------------------------------------------------------------*/
/**
 *  Read checksum
 *
 *  @param [in]    aDevice	 	Device handle of CRC
 *
 *  @param [out]   *aValue		Pointer to checksum variable to read into
 *
 *  @return        T_uezError   If successful, returns UEZ_ERROR_NONE. If the
 *								device handle is bad,returns 
 *                              UEZ_ERROR_HANDLE_INVALID. 
 *  @par Example Code:
 *  @code
 *  #include <uEZCRC.h>
 *  T_uezDevice crc;
 *	TUInt32 aValue; 
 *  if(UEZCRCOpen("CRC", &crc) == UEZ_ERROR_NONE){
 *    	// the device opened properly
 *      UEZCRCReadChecksum(crc, &aValue);
 *  }else{ // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCRCReadChecksum(T_uezDevice aDevice, TUInt32 *aValue)
{
    T_uezError error;
    DEVICE_CRC **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->ReadChecksum((void *)p, aValue);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZCRC.c
 *-------------------------------------------------------------------------*/
