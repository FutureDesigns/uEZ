/*-------------------------------------------------------------------------*
 * File:  uEZStream.c
 *-------------------------------------------------------------------------*
 * Description:
 *      uEZ Stream API
 * Implementation:
 *      Most of the commands are thin and map directly to the 
 *      API of the Stream device drivers.
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
 *    @addtogroup uEZStream
 *  @{
 *  @brief     uEZ Stream Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ Stream interface.
 *
 * @par Example code:
 * Example task to send time stamped temperature readings out the serial port
 * @par
 * @code
 * #include <uEZ.h>
 * #include <UEZTimeDate.h>
 * #include <uEZStream.h>
 * #include <uEZTemperature.h>
 *
 *  TUInt32 TempMonitorTask(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezDevice uart0;
 *      T_uezDevice temp0;
 *      TUInt8 buffer[30];
 *      TUInt32 bytesSent;
 *      TUInt32 numBytesToSend;
 *      T_uezTimeDate timeDate;
 *      TUInt32 tempReading;
 *      TBool done = EFalse;
 *
 *      if (UEZTemperatureOpen("Temp0", &temp0) == UEZ_ERROR_NONE) {
 *          //the device opened properly
 *          if (UEZStreamOpen("UART0", &uart0) == UEZ_ERROR_NONE) {
 *              //the device opened properly
 *
 *              while (!done) {
 *                  if (UEZTimeDateGet(&timeDate) == UEZ_ERROR_NONE) {
 *                      if (UEZTemperatureRead(temp0, &tempReading) ==
 *                             UEZ_ERROR_NONE) {
 *
 *                          //got the time and temp
 *                          sprintf(buffer,
 *                                  "%03d.%01d F   "
 *                                  "%02d/%02d/%02d "
 *                                  "%02d:%02d:%02d\n",
 *                                  (tempReading>>16),
 *                                  ((((tempReading)&0xFFFF)*10)>>16),
 *                                  timeDate.iDate.iMonth,
 *                                  timeDate.iDate.iDay,
 *                                  timeDate.iDate.iYear,
 *                                  timeDate.iTime.iHour,
 *                                  timeDate.iTime.iMinute,
 *                                  timeDate.iTime.iSecond);
 *
 *                      } else {
 *                          //an error occurred
 *                          strcpy(buffer, "Error Temp");
 *                      }
 *
 *                  } else {
 *                      //an error occurred
 *                      stcpy(buffer, "Error Time");
 *                  }
 *
 *                  numBytesToSend = strlen(buffer);
 *                  if (UEZStreamWrite( uart0,
 *                                      (void*)buffer,
 *                                      numBytesToSend,
 *                                      &bytesSent,
 *                                      UEZ_TIMEOUT_INFINITE )
 *                          != UEZ_ERROR_NONE) {
 *                      //an error occurred
 *                  }
 *
 *                  if (bytesSent != numBytesToSend) {
 *                      //not all the data was sent
 *                  }
 *              }
 *
 *              if (UEZStreamClose(uart0) != UEZ_ERROR_NONE) {
 *                  //an error occurred and need to be handled.
 *              }
 *
 *          } else {
 *              //an error occurred opening the UART
 *          }
 *
 *      } else {
 *          //an error occurred opening the temp sensor
 *      }
 *
 *      return 0;
 *  }
 *  @endcode
*/

#include "Config.h"
#include "uEZStream.h"
#include "HAL/HAL.h"
#include "Device/Stream.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZStreamOpen
 *---------------------------------------------------------------------------*/
/**
 *    Open up access to the Stream bank.
 *
 *  @param [in]     *aName        Pointer to name of Stream display (usually
 *                                "Stream")
 *  @param [out]    *aDevice      Pointer to device handle to be returned blah
 *
 *  @return         T_uezError    If the device is opened, returns UEZ_ERROR_NONE.
 *                                If the device cannot be found returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZStream.h>
 *
 *  T_uezDevice uart0;
 *  if (UEZStreamOpen("UART0", &uart0) == UEZ_ERROR_NONE) {
 *      //the device opened properly
 *  } else {
 *      //an error occurred
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZStreamOpen(const char * const aName, T_uezDevice *aDevice) {
    T_uezError error;
    DEVICE_STREAM **p;

    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Open((void *) p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZStreamClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the Stream bank.
 *
 *  @param [in]    aDevice        Device handle to close
 *
 *  @return        T_uezError
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZStream.h>
 *
 *  T_uezDevice uart0;
 *  if (UEZStreamOpen("UART0", &uart0) == UEZ_ERROR_NONE) {
 *
 *      if (UEZStreamClose(uart0) != UEZ_ERROR_NONE) {
 *          //an error occurred and need to be handled.
 *      }
 *
 *  } else {
 *      //an error occurred
 *  }
 *  @endcode
 */
 /*---------------------------------------------------------------------------*/
T_uezError UEZStreamClose(T_uezDevice aDevice) {
    T_uezError error;
    DEVICE_STREAM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Close((void *) p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZStreamWrite
 *---------------------------------------------------------------------------*/
/**
 *  Write data out to the stream device.
 *
 *  @param [in]     aDevice                  Flash Device handle
 *  @param [in]     *aData                   Pointer to data to write
 *  @param [in]     aNumBytes                Number of bytes to write
 *  @param [out]    *aNumBytesWritten        Number of bytes actually written
 *                                           before timeout.
 *  @param [in]     aTimeout                 Timeout to wait per character written
 *                                           before aborting write.
 *
 *  @return        T_uezError
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZStream.h>
 *
 *  T_uezDevice uart0;
 *  TUInt8 buffer[6] = "Hello";
 *  TUInt32 bytesSent;
 *  if (UEZStreamOpen("UART0", &uart0) == UEZ_ERROR_NONE) {
 *
 *      if (UEZStreamWrite( uart0,
 *                          (void*)buffer,
 *                          sizeof(buffer) - 1,
 *                          &bytesSent,
 *                          UEZ_TIMEOUT_INFINATE)
 *              != UEZ_ERROR_NONE) {
 *          //an error occurred
 *      }
 *
 *      if (bytesSent != (sizeof(buffer) - 1)) {
 *          //not all the data was sent
 *      }
 *
 *  } else {
 *       // an error occurred - the device did not open properly
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZStreamWrite(T_uezDevice aDevice, void *aData, TUInt32 aNumBytes,
        TUInt32* aNumBytesWritten, TUInt32 aTimeout) {
    T_uezError error;
    DEVICE_STREAM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Write((void *) p, aData, aNumBytes, aNumBytesWritten, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZStreamRead
 *---------------------------------------------------------------------------*/
/**
 *    Read data from the stream device.
 *
 *  @param [in]     aDevice         Flash Device handle
 *  @param [out]    *aData          Pointer to data to receive bytes
 *  @param [in]     aNumBytes       Maximum number of bytes to receive
 *  @param [out]    *aNumBytesRead  Number of bytes actually read
 *                                  before timeout.
 *  @param [in]     aTimeout        Timeout to wait per character read
 *                                   before aborting read.
 *
 *  @return        T_uezError
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZStream.h>
 *
 *  T_uezDevice uart0;
 *  TUInt8 buffer[6] = "Hello";
 *  TUInt8 receive[6];
 *  TUInt32 bytesSent;
 *  TUInt32 bytesReceived;
 *
 *  if (UEZStreamOpen("UART0", &uart0) == UEZ_ERROR_NONE) {
 *
 *      if (UEZStreamFlush(uart0) != UEZ_ERROR_NONE) {
 *          //an error occurred
 *      }
 *
 *      if (UEZStreamWrite(uart0, (void*) buffer, sizeof(buffer) - 1,
 *              &bytesSent, UEZ_TIMEOUT_INFINITE) != UEZ_ERROR_NONE) {
 *          //an error occurred
 *      }
 *
 *      if (bytesSent != (sizeof(buffer) - 1)) {
 *          //not all the data was sent
 *      }
 *
 *      if (UEZStreamRead(uart0, (void*) receive, sizeof(receive) - 1,
 *              &bytesReceived, 1000) != UEZ_ERROR_NONE) {
 *          //an error occurred
 *      }
 *
 *  } else {
 *      // an error occurred - the device did not open properly
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZStreamRead(T_uezDevice aDevice, void *aData, TUInt32 aNumBytes,
        TUInt32* aNumBytesRead, TUInt32 aTimeout) {
    T_uezError error;
    DEVICE_STREAM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Read((void *) p, aData, aNumBytes, aNumBytesRead, aTimeout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZStreamControl
 *---------------------------------------------------------------------------*/
/**
 *  Process command specific to the underlying device.
 *
 *  @param [in]     aDevice         Flash Device handle
 *  @param [out]    aControl        Control being processed
 *  @param [in]     *aControlData   Data for control
 *
 *  @return        T_uezError
 *
 *  @par Example Code:
 *  This is an example of how to set new settings for the device.
 *  @par
 *  @code
 *  T_Serial_Settings isettings;
 *
 *  isettings.iBaud = 19200;
 *  isettings.iWordLength = 8;
 *  isettings.iStopBit = SERIAL_STOP_BITS_1;
 *  isettings.iParity = SERIAL_PARITY_NONE;
 *  isettings.iFlowControl = SERIAL_FLOW_CONTROL_NONE;
 *
 *  if (UEZStreamControl(uart0, STREAM_CONTROL_SET_SERIAL_SETTINGS, &isettings)
 *          != UEZ_ERROR_NONE) {
 *      //an error occurred
 *  }
 *  @endcode
 *  @par
 *  This is an example of how to get the current settings for the device.
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZStream.h>
 *
 *  T_Serial_Settings isettings;
 *  if (UEZStreamControl(uart0, STREAM_CONTROL_GET_SERIAL_SETTINGS, &isettings)
 *          != UEZ_ERROR_NONE) {
 *      //an error occurred
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZStreamControl(T_uezDevice aDevice, TUInt32 aControl,
        void *aControlData) {
    T_uezError error;
    DEVICE_STREAM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Control((void *) p, aControl, aControlData);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZStreamFlush
 *---------------------------------------------------------------------------*/
/**
 *  Clear the buffers for the device.
 *
 *  @param [in]     aDevice         Flash Device handle
 *
 *  @return         T_uezError
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZStream.h>
 *
 *  if (UEZStreamFlush(uart0) != UEZ_ERROR_NONE) {
 *      //an error occurred
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZStreamFlush(T_uezDevice aDevice) {
    T_uezError error;
    DEVICE_STREAM **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Flush((void *) p);
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZStream.c
 *-------------------------------------------------------------------------*/
