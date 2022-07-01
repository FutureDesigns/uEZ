/*-------------------------------------------------------------------------*
 * File:  uEZSPI.c
 *-------------------------------------------------------------------------*
 * Description:
 *     SPI Interface
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
 *  @addtogroup uEZSPI
 *  @{
 *  @brief     uEZ SPI Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ SPI interface.
 *
 *  @par Example code:
 *  Example task to send data over SPI
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZSPI.h>
 *
 *  TUInt32 SPITransfer(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezDevice SPI;
 *      TUInt8 data;
 *      SPI_Request SPIRequest;
 *     	if (UEZSPIOpen("SPI0", &SPI) == UEZ_ERROR_NONE) {
 *
 *          SPIRequest.iNumTransfers = 1;
 *          SPIRequest.iBitsPerTransfer = 8;
 *          SPIRequest.iRate = 1000; // 1 MHz
 *          SPIRequest.iClockOutPolarity = ETrue; // rising edge
 *          SPIRequest.iClockOutPhase = ETrue;
 *          SPIRequest.iCSGPIOPort = GPIO_TO_HAL_PORT(GPIO_P0_9);
 *          SPIRequest.iCSGPIOBit = GPIO_TO_PIN_BIT(GPIO_P0_9);
 *          SPIRequest.iCSPolarity = EFalse;
 *          SPIRequest.iDataMISO = data;
 *          SPIRequest.iDataMOSI = data;
 *
 *          if (UEZSPITransferPolled(SPI, &SPIRequest) == UEZ_ERROR_NONE) {
 *              // transfer was successful
 *          }
 *
 *      } else {
 *          //an error occurred opening SPI
 *      }
 *      return 0;
 *  }
 *  @endcode
 */

#include "Config.h"
#include "HAL/HAL.h"
#include "Device/SPIBus.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>
#include <uEZGPIO.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZSPIOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the SPI bus.
 *
 *  @param [in]    *aName 		Pointer to name of SPI bus 
 *                              (usually "SPIx" where x is bus number)
 *  @param [out]    *aDevice 	Pointer to device handle to be returned
 *
 *  @return        T_uezError   If the device is opened, returns 
 *                              UEZ_ERROR_NONE.  If the device cannot be 
 *                              found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZSPI.h>
 *
 *  T_uezDevice SPI;
 *  if (UEZSPIOpen("SPI0", &SPI) == UEZ_ERROR_NONE) {
 *   	// the device opened properly
 *  } else {
 *      //an error occurred opening SPI
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZSPIOpen(
            const char *const aName, 
            T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_SPI_BUS **p;
    
    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZSPIClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the SPI bus.
 *
 *  @param [in]    aDevice		 	Device handle of SPI to close
 *
 *  @return        T_uezError       If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZSPI.h>
 *
 *  T_uezDevice SPI;
 *  if (UEZSPIOpen("SPI0", &SPI) == UEZ_ERROR_NONE) {
 *
 *  	if (UEZSPIClose(SPI) != UEZ_ERROR_NONE) {
 *          // an error occurred
 *      }
 *
 *  } else {
 *      // an error occurred opening SPI
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZSPIClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_SPI_BUS **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZSPITransferPolled
 *---------------------------------------------------------------------------*/
/**
 *  Requests a SPI polled transfer (both read and write).
 *
 *  @param [in]    aDevice     	Handle to opened SPI bus device.
 *
 *  @param [out]   *aRequest    Structure of read and/or write SPI
 *                              transaction information.
 *  @return        T_uezError   If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  T_uezDevice SPI;
 *  TUInt8 data;
 *  SPI_Request SPIRequest;
 *  if (UEZSPIOpen("SPI0", &SPI) == UEZ_ERROR_NONE) {
 *
 *      SPIRequest.iNumTransfers = 1;
 *      SPIRequest.iBitsPerTransfer = 8;
 *      SPIRequest.iRate = 1000; // 1 MHz
 *      SPIRequest.iClockOutPolarity = ETrue; // rising edge
 *      SPIRequest.iClockOutPhase = ETrue;
 *      SPIRequest.iCSGPIOPort = GPIO_TO_HAL_PORT(GPIO_P0_9);
 *      SPIRequest.iCSGPIOBit = GPIO_TO_PIN_BIT(GPIO_P0_9);
 *      SPIRequest.iCSPolarity = EFalse;
 *      SPIRequest.iDataMISO = data;
 *      SPIRequest.iDataMOSI = data;
 *
 *      if (UEZSPITransferPolled(SPI, &SPIRequest) == UEZ_ERROR_NONE) {
 *          // transfer was successful
 *      }
 *  } else {
 *      // an error occurred opening SPI
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZSPITransferPolled(T_uezDevice aDevice, SPI_Request *aRequest)
{
    T_uezError error;
    DEVICE_SPI_BUS **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->TransferPolled((void *)p, aRequest);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZSPITransferNoBlock
 *---------------------------------------------------------------------------*/
/**
 *  Requests a SPI polled transfer (both read and write).
 *
 *  @param [in]    aDevice      	Handle to opened SPI bus device.
 *
 *  @param [out]   *aRequest    	Structure of read and/or write SPI
 *                              	transaction information.
 *  @param [in]    aCallback    	Callback function to indicate the transfer
 *                              	has been completed.
 *  @param [in] aCallbackWorkspace  Callback workspace
 *
 *  @return        T_uezError   If successful, returns UEZ_ERROR_NONE.  If
 *                                  an invalid device handle, returns
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  T_spiCompleteCallback SPITransferComplete(void *aWorkspace, SPI_Request *aRequest)
 *  {
 *      //Handle transfer completion
 *  }
 *
 *  T_uezDevice SPI;
 *  TUInt8 dataIn[3], dataOut[3];
 *  SPI_Request SPIRequest;
 *
 *  if (UEZSPIOpen("SPI0", &SPI) == UEZ_ERROR_NONE) {
 *      SPIRequest.iNumTransfers = 3;
 *      SPIRequest.iBitsPerTransfer = 8;
 *      SPIRequest.iRate = 1000; // 1 MHz
 *      SPIRequest.iClockOutPolarity = ETrue; // rising edge
 *      SPIRequest.iClockOutPhase = ETrue;
 *      SPIRequest.iCSGPIOPort = GPIO_TO_HAL_PORT(GPIO_P0_9);
 *      SPIRequest.iCSGPIOBit = GPIO_TO_PIN_BIT(GPIO_P0_9);
 *      SPIRequest.iCSPolarity = EFalse;
 *      SPIRequest.iDataMISO = dataIn;
 *      SPIRequest.iDataMOSI = dataOut;
 *
 *      if(UEZSPITransferNoBlock(SPI, &SPIRequest, SPITransferComplete, (void*)0)){
 *          //An error occurred
 *      }
 *  } else {
 *      // an error occurred opening SPI
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZSPITransferNoBlock(
        T_uezDevice aDevice,
        SPI_Request *aRequest,
        T_spiCompleteCallback aCallback,
        void *aCallbackWorkspace)
{
    T_uezError error;
    DEVICE_SPI_BUS **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->TransferNoBlock((void *)p, aRequest, aCallback,
        aCallbackWorkspace);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZSPIIsBusy
 *---------------------------------------------------------------------------*/
/**
 *  Requests a SPI polled transfer (both read and write).
 *
 *  @param [in]    aDevice      Handle to opened SPI bus device.
 *  @return        TBool
 *  @par Example Code:
 *  @code
 *  T_uezDevice SPI;
 *  TUInt8 dataIn[3], dataOut[3];
 *  SPI_Request SPIRequest;
 *
 *  if (UEZSPIOpen("SPI0", &SPI) == UEZ_ERROR_NONE) {
 *      SPIRequest.iNumTransfers = 3;
 *      SPIRequest.iBitsPerTransfer = 8;
 *      SPIRequest.iRate = 1000; // 1 MHz
 *      SPIRequest.iClockOutPolarity = ETrue; // rising edge
 *      SPIRequest.iClockOutPhase = ETrue;
 *      SPIRequest.iCSGPIOPort = GPIO_TO_HAL_PORT(GPIO_P0_9);
 *      SPIRequest.iCSGPIOBit = GPIO_TO_PIN_BIT(GPIO_P0_9);
 *      SPIRequest.iCSPolarity = EFalse;
 *      SPIRequest.iDataMISO = dataIn;
 *      SPIRequest.iDataMOSI = dataOut;
 *
 *      if(UEZSPITransferNoBlock(SPI, &SPIRequest, SPITransferComplete, (void*)0)){
 *          //An error occurred
 *      }
 *
 *      while(UEZSPIIsBusy(SPI)){
 *          //Wait for transfer to complete
 *          uEZTaskDelay(10);
 *      }
 *  } else {
 *      // an error occurred opening SPI
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TBool UEZSPIIsBusy(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_SPI_BUS **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Do the transactions and return the result
    return (*p)->IsBusy((void *)p);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZHandles.c
 *-------------------------------------------------------------------------*/
