/*-------------------------------------------------------------------------*
 * File:  uEZI2C.c
 *-------------------------------------------------------------------------*
 * Description:
 *     I2C Interface
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
/**
 *  @addtogroup uEZI2C
 *  @{
 *  @brief     uEZ I2C Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ I2C interface.
 *
 * @par Example code:
 * Example task to send/receive data as I2C master
 * @par
 * @code
 * #include <uEZ.h>
 * #include <uEZI2C.h>
 *
 * TUInt32 I2CReadWrite(T_uezTask aMyTask, void *aParams)
 * {
 *      T_uezDevice I2C;
 *      I2C_Request r;
 *      TUInt8 dataIn[20];
 *      TUInt8 dataOut[5];
 *     	if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *          // the device opened properly
 *
 *          dataOut[0] = 0x00;
 *          r.iAddr = 0x10;
 *          r.iSpeed = 100; //kHz
 *          r.iWriteData = dataOut;
 *          r.iWriteLength = 1; // send 1 byte
 *          r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
 *          r.iReadData = dataIn;
 *          r.iReadLength = 20; // read 20 bytes
 *          r.iReadTimeout = UEZ_TIMEOUT_INFINITE;
 *
 *          UEZI2CTransaction(I2C, &r);
 *          // got 20 bytes in dataIn now
 *
 *          if (UEZI2CClose(I2C) != UEZ_ERROR_NONE) {
 *              //an error occurred
 *          }
 *      } else {
 *          // an error occurred opening I2C
 *      }
 *     return 0;
 * }
 * @endcode
 */

#include "Config.h"
#include "uEZI2C.h"
#include "HAL/HAL.h"
#include "Device/I2CBus.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZI2COpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the I2C bus.
 *
 *  @param [in]    *aName 			Pointer to name of I2C bus (usually
 *                                  "I2Cx" where x is bus number)
 *  @param [in]    *aDevice 		Pointer to device handle to be returned
 *
 *  @return        T_uezError       If the device is opened, returns 
 *                                  UEZ_ERROR_NONE.  If the device cannot be 
 *                                  found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZI2C.h>
 *
 *  T_uezDevice I2C;
 *  if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *     	// the device opened properly
 *
 *  } else {
 *     // an error occurred opening I2C
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZI2COpen(
        const char * const aName,
        T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_I2C_BUS **p;

    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZI2CClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the I2C bus.
 *
 *  @param [in]    aDevice 			Device handle of I2C to close
 *
 *  @return        T_uezError      	If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZI2C.h>
 *
 *  T_uezDevice I2C;
 *  if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *     	// the device opened properly
 *  
 *    	if (UEZI2CClose(I2C) != UEZ_ERROR_NONE) {
 *    		//an error occurred
 *     }
 *  } else {
 *     // an error occurred opening I2C
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZI2CClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_I2C_BUS **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZI2CTransaction
 *---------------------------------------------------------------------------*/
/**
 *  Requests a I2C transfer of a write and/or read I2C transaction.
 *
 *  @param [in]    aDevice          Handle to opened I2C bus device.
 *
 *  @param [in]    *aRequest        Structure of read and/or write I2C
 *                                  transaction information.
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZI2C.h>
 *
 *  T_uezDevice I2C;
 *  I2C_Request r;
 *  TUInt8 dataIn[20];
 *  TUInt8 dataOut[5];
 *  if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *      // the device opened properly
 *
 *      dataOut[0] = 0x00;
 *      r.iAddr = 0x10;
 *      r.iSpeed = 100; //kHz
 *      r.iWriteData = dataOut;
 *      r.iWriteLength = 1; // send 1 byte
 *      r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
 *      r.iReadData = dataIn;
 *      r.iReadLength = 20; // read 20 bytes
 *      r.iReadTimeout = UEZ_TIMEOUT_INFINITE;
 *
 *      UEZI2CTransaction(I2C, &r);
 *      // got 20 bytes in dataIn now
 *
 *      if (UEZI2CClose(I2C) != UEZ_ERROR_NONE) {
 *          //an error occurred
 *      }
 *  } else {
 *      // an error occurred opening I2C
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZI2CTransaction(
        T_uezDevice aDevice,
        I2C_Request *aRequest)
{
    T_uezError error;
    DEVICE_I2C_BUS **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->ProcessRequest((void *)p, aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZI2CRead
 *---------------------------------------------------------------------------*/
/**
 *  Requests an I2C read only transaction with the given parameters.
 *
 *  @param [in]    aDevice          Handle to opened I2C bus device.
 *
 *  @param [in]    aAddress         Slave I2C address
 *
 *  @param [in]    aSpeed           Speed in kHz of I2C transaction 
 *                                  (usually 100 or 400).
 *  @param [in]    *aData           Place to store read data
 *
 *  @param [in]    aDataLength      Number of bytes to read
 *
 *  @param [in]    aTimeout         General timeout to complete transaction
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.  If a timeout,
 *                                  returns UEZ_ERROR_TIMEOUT.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZI2C.h>
 *
 *  T_uezDevice I2C;
 *  TUInt8 addr = 0;
 *  TUInt8 data[10];
 *  if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *     	// the device opened properly
 *  
 *     UEZI2CRead(I2C, addr, 100, data, 0, 1000); // 1 second timeout
 *     if (UEZI2CClose(I2C) != UEZ_ERROR_NONE) {
 *    		//an error occurred
 *     }
 *  } else {
 *     // an error occurred opening I2C
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZI2CRead(
        T_uezDevice aDevice,
        TUInt8 aAddress,
        TUInt32 aSpeed,
        TUInt8 *aData,
        TUInt8 aDataLength,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_I2C_BUS **p;
    I2C_Request request;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Fill the request structure on the stack and do a standard request
    request.iAddr = aAddress;
    request.iReadData = aData;
    request.iReadLength = aDataLength;
    request.iReadTimeout = aTimeout;
    request.iSpeed = aSpeed;
    request.iWriteData = 0;
    request.iWriteLength = 0;
    request.iWriteTimeout = 0;

    // Do the transactions and return the result
    return (*p)->ProcessRequest((void *)p, &request);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZI2CWrite
 *---------------------------------------------------------------------------*/
/**
 *  Requests an I2C write only transaction with the given parameters.
 *
 *  @param [in]    aDevice          Handle to opened I2C bus device.
 *
 *  @param [in]    aAddress         Slave I2C address
 *
 *  @param [in]    aSpeed           Speed in kHz of I2C transaction 
 *                                  (usually 100 or 400).
 *  @param [in]    *aData           Pointer to data to write out   
 *
 *  @param [in]    aDataLength      Number of bytes to write
 *
 *  @param [in]    aTimeout         General timeout to complete transaction
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.  If a timeout,
 *                                  returns UEZ_ERROR_TIMEOUT.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZI2C.h>
 *
 *  T_uezDevice I2C;
 *  TUInt8 addr = 0;
 *  TUInt8 data[2];
 *  if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *     	// the device opened properly
 *
 *     	data[0] = 0x40;
 *      data[1] = 0x60;
 *      UEZI2CWrite(I2C, addr, 100, data, 0,2, 1000); // 1 second timeout
 *    	if (UEZI2CClose(I2C) != UEZ_ERROR_NONE) {
 *    		//an error occurred
 *     }
 *  } else {
 *     // an error occurred opening I2C
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZI2CWrite(
        T_uezDevice aDevice,
        TUInt8 aAddress,
        TUInt32 aSpeed,
        TUInt8 *aData,
        TUInt8 aDataLength,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_I2C_BUS **p;
    I2C_Request request;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Fill the request structure on the stack and do a standard request
    request.iAddr = aAddress;
    request.iWriteData = aData;
    request.iWriteLength = aDataLength;
    request.iWriteTimeout = aTimeout;
    request.iSpeed = aSpeed;
    request.iReadData = 0;
    request.iReadLength = 0;
    request.iReadTimeout = 0;

    // Do the transactions and return the result
    return (*p)->ProcessRequest((void *)p, &request);
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZI2CSlaveStart
 *---------------------------------------------------------------------------*/
/**
 *  Start the I2C in slave mode
 *
 *  @param [in]    aDevice      	Handle to opened I2C device.
 *
 *  @param [in]    *aSetup      	Slave configuration
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZI2C.h>
 *
 *  T_uezDevice I2C;
 *  T_I2CSlaveSetup I2CSlaveSetup;
 *  TUInt8 sBuf[10];
 *  TUInt8 rBuf[10];
 *  if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *     	// the device opened properly
 *  
 *      I2CSlaveSetup.iAddressLower7Bits = 0;
 *      I2CSlaveSetup.iSendBuffer = sBuf;
 *      I2CSlaveSetup.iMaxSendLength = 10;
 *      I2CSlaveSetup.iReceiveBuffer = rBuf;
 *      I2CSlaveSetup.iMaxReceiveLength = 10;
 *      UEZI2CSlaveStart(I2C, &I2CSlaveSetup);
 *
 *    	if (UEZI2CClose(I2C) != UEZ_ERROR_NONE) {
 *    		//an error occurred
 *     }
 *  } else {
 *     // an error occurred opening I2C
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZI2CSlaveStart(T_uezDevice aDevice, const T_I2CSlaveSetup *aSetup)
{
    T_uezError error;
    DEVICE_I2C_BUS **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SlaveStart((void *)p, aSetup);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZI2CSlaveWaitForEvent
 *---------------------------------------------------------------------------*/
/**
 *  Wait for event from I2C master
 *
 *  @param [in]    aDevice      	Handle to opened I2C device.
 *
 *  @param [in]    *aEvent      	Pointer to T_I2CSlaveEvent
 *
 *  @param [in]    *aLength      	Pointer to length of message
 *
 *  @param [in]    aTimeout      	Timeout in 
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZI2C.h>
 *
 *  T_uezDevice I2C;
 *  T_I2CSlaveSetup I2CSlaveSetup;
 *  TUInt8 sBuf[10];
 *  TUInt8 rBuf[10];
 *  T_I2CSlaveEvent I2Cevent;
 *  if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *      // the device opened properly
 *
 *      I2CSlaveSetup.iAddressLower7Bits = 0;
 *      I2CSlaveSetup.iSendBuffer = sBuf;
 *      I2CSlaveSetup.iMaxSendLength = 10;
 *      I2CSlaveSetup.iReceiveBuffer = rBuf;
 *      I2CSlaveSetup.iMaxReceiveLength = 10;
 *      UEZI2CSlaveStart(I2C, &I2CSlaveSetup);
 *  
 *      UEZI2CSlaveWaitForEvent(I2C, &I2Cevent, 10, 1000);
 *
 *    	if (UEZI2CClose(I2C) != UEZ_ERROR_NONE) {
 *    		//an error occurred
 *     }
 *  } else {
 *     // an error occurred opening I2C
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZI2CSlaveWaitForEvent(
        T_uezDevice aDevice,
        T_I2CSlaveEvent *aEvent,
        TUInt32 *aLength,
        TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_I2C_BUS **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SlaveWaitForEvent((void *)p, aEvent, aLength, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZI2CEnable
 *---------------------------------------------------------------------------*/
/**
 *  Enable the specified I2C device.
 *
 *  @param [in]    aDevice      	Handle to opened I2C device.
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZI2C.h>
 *
 *  T_uezDevice I2C;
 *  if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *     	// the device opened properly
 *  
 *      UEZI2CDisable(I2C);
 *      UEZI2CEnable(I2C);
 *
 *    	if (UEZI2CClose(I2C) != UEZ_ERROR_NONE) {
 *    		//an error occurred
 *     }
 *  } else {
 *     // an error occurred opening I2C
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZI2CEnable(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_I2C_BUS **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Enable((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZI2CDisable
 *---------------------------------------------------------------------------*/
/**
 *  Disable the specified I2C device.
 *
 *  @param [in]    aDevice      	Handle to opened I2C device.
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZI2C.h>
 *
 *  T_uezDevice I2C;
 *  if (UEZI2COpen("I2C0", &I2C) == UEZ_ERROR_NONE) {
 *     	// the device opened properly
 *  
 *      UEZI2CDisable(I2C);
 *      UEZI2CEnable(I2C);
 *
 *    	if (UEZI2CClose(I2C) != UEZ_ERROR_NONE) {
 *    		//an error occurred
 *     }
 *  } else {
 *     // an error occurred opening I2C
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZI2CDisable(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_I2C_BUS **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Disable((void *)p);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZI2C.c
 *-------------------------------------------------------------------------*/
