/*-------------------------------------------------------------------------*
 * File:  uEZCAN.c
 *-------------------------------------------------------------------------*
 * Description:
 *      uEZ CAN API
 * Implementation:
 *      Most of the commands are thin and map directly to the 
 *      API of the CAN device drivers.
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
 *    @addtogroup uEZCAN
 *  @{
 *  @brief     uEZ CAN Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ CAN interface.
 *
 * @par Example code:
 * // example description
 * @par
 * @code
 * void CAN_Receive_Callback(T_CAN_Message aCanMessage)
 * {
 *     UEZQueueSend(G_canQueue, &aCanMessage, UEZ_TIMEOUT_NONE);
 * }
 * 
 * void send2Bytes()
 * {
 *     T_uezDevice can;
 *     T_CAN_Settings s;
 *     T_uezHandle canQueue;
 *     T_CAN_Message canMessage;
 *     TUInt8 data[2];
 * 
 *     UEZQueueCreate(200, sizeof(T_CAN_Message),&canQueue);
 * 
 *     UEZCANOpen("CAN2", &can);
 *     s.iBaud = 1000000; // 1MHz
 *     s.iReceiveCallback = CAN_Receive_Callback;
 * 
 *     // These are the only address we’ll receive from 
 *     s.iSFFEntry[0].iID = 0x001;
 *     s.iSFFEntry[0].iEnable = ETrue;
 *     s.iSFFEntry[1].iID = 0x004;
 *     s.iSFFEntry[1].iEnable = ETrue;
 *     s.iNumSFFEntries = 2;
 *             
 *     UEZCANControl(can, CAN_CONTROL_SET_SETTINGS, &s);
 * 
 *     data[0] = 50;
 *     data[1] = 30;
 *     UEZCANWrite(can, 0x009, data, 2);
 * 
 *     // wait for response with timeout
 *     while( UEZQueueReceive(canQueue, &canMessage, 100 ) == UEZ_ERROR_NONE) {
 *          printf("CAN Message Received (ID=0x%03x, DataLength=%d, Data= ", canMessage.iID, canMessage.iDataLen);
 *          for(i=0; i< canMessage.iDataLen; i++)
 *               printf("0x%02x ", canMessage.iData[i]);
 *          printf("\n");
 *     }
 * }
 * @endcode
*/

#include "Config.h"
#include "uEZCAN.h"
#include "HAL/HAL.h"
#include "Device/CAN.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZCANOpen
 *---------------------------------------------------------------------------*/
/**
 *    Open up access to the CAN bank.
 *
 *  @param [in]     *aName        Pointer to name of CAN display (usually
 *                                "CAN")
 *  @param [out]    *aDevice      Pointer to device handle to be returned blah
 *
 *  @return         T_uezError    If the device is opened, returns UEZ_ERROR_NONE.
 *                                If the device cannot be found returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCAN.h>
 *
 *  T_uezDevice can;
 *  if (UEZCANOpen("CAN2", &can) == UEZ_ERROR_NONE) {
 *      //the device opened properly
 *  } else {
 *      //an error occurred
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCANOpen(const char * const aName, T_uezDevice *aDevice) {
    T_uezError error;
    DEVICE_CAN **p;

    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Open((void *) p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCANClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the CAN bank.
 *
 *  @param [in]    aDevice        Device handle to close
 *
 *  @return        T_uezError
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCAN.h>
 *
 *  T_uezDevice can;
 *  if (UEZCANOpen("CAN2", &can) == UEZ_ERROR_NONE) {
 *
 *      if (UEZCANClose(can0) != UEZ_ERROR_NONE) {
 *          //an error occurred and need to be handled.
 *      }
 *
 *  } else {
 *      //an error occurred
 *  }
 *  @endcode
 */
 /*---------------------------------------------------------------------------*/
T_uezError UEZCANClose(T_uezDevice aDevice) {
    T_uezError error;
    DEVICE_CAN **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Close((void *) p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCANWrite
 *---------------------------------------------------------------------------*/
/**
 *  Write data out to the CAN device.
 *
 *  @param [in]     aDevice                  Flash Device handle
 *  @param [in]     aID                      ID of end device                                           
 *  @param [in]     *aData                   Pointer to data to write
 *                                           (Up to 8 bytes)
 *  @param [in]     aDataLen                 Number of bytes to write
 *                                           before timeout.
 *
 *  @return        T_uezError
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCAN.h>
 *
 *  T_uezDevice can;
 *  // able to send up to 8 bytes
 *  TUInt8 outData[] = {0xF0, 0xF1, 0xF2, 0xF3,
 *                      0xF4, 0xF5, 0xF6, 0xF7 };
 *  if (UEZCANOpen("CAN2", &can) == UEZ_ERROR_NONE) {
 *
 *      if (UEZCANWrite( can,
 *                      (void*)outData,
 *                      8,
 *                      UEZ_TIMEOUT_NONE)
 *              != UEZ_ERROR_NONE) {
 *          //an error occurred or no message available
 *      }
 *
 *  } else {
 *       // an error occurred - the device did not open properly
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCANWrite(
        T_uezDevice aDevice,
        TUInt16 aID,
        TUInt8 *aData,
        TUInt8 aDataLen)
{
    T_uezError error;
    DEVICE_CAN **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Write((void *) p, aID, aData, aDataLen);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZCANControl
 *---------------------------------------------------------------------------*/
/**
 *  Process command specific to the underlying device.
 *
 *  @param [in]     aDevice         Flash Device handle
 *  @param [out]    aControl        Control being processed
 *  @param [in]     *aControlData   Data for control
 *
 *  @return         T_uezError
 *
 *  @par Example Code:
 *  This is an example of how to set new settings for the device.
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZCAN.h>
 *
 *  T_uezDevice can;
 *  if(UEZCANOpen("CAN2", &can) == UEZ_ERROR_NONE) {
 *      s.iBaud = 1000000; // 1MHz
 *      s.iReceiveCallback = CAN_Receive_Callback;
 * 
 *      // These are the only address we’ll receive from 
 *      s.iSFFEntry[0].iID = 0x001;
 *      s.iSFFEntry[0].iEnable = ETrue;
 *      s.iSFFEntry[1].iID = 0x004;
 *      s.iSFFEntry[1].iEnable = ETrue;
 *      s.iNumSFFEntries = 2;
 *             
 *      UEZCANControl(can, CAN_CONTROL_SET_SETTINGS, &s);
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZCANControl(
        T_uezDevice aDevice,
        TUInt32 aControl,
        void *aControlData) {
    T_uezError error;
    DEVICE_CAN **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **) &p);
    if (error)
        return error;

    return (*p)->Control((void *) p, aControl, aControlData);
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZCAN.c
 *-------------------------------------------------------------------------*/
