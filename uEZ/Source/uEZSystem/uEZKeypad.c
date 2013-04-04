/*-------------------------------------------------------------------------*
 * File:  uEZKeypad.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Keypad Interface
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
 *  @addtogroup uEZKeypad
 *  @{
 *  @brief     uEZ Keypad Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *    The uEZ Keypad interface.
 *
 *  @par Example code:
 *  Example task to read the keypad
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZKeypad.h>
 *
 * TUInt32 KeypadRead(T_uezTask aMyTask, void *aParams)
 * {
 *  T_uezDevice keypad;
 *  T_uezInputEvent event;
 *  if (UEZKeypadOpen("Keypad", &keypad) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *	    if (UEZKeypadRead(keypad, &event, 1000)) == UEZ_ERROR_NONE) {
 *            // keypad read successful, stored in event 
 *            // event.iEvent.iButton.iKey 		// 32 bit
 *            // event.iEvent.iButton.iAction 	//  8 bit
 *      }
 *      if (UEZKeypadClose(keypad) != UEZ_ERROR_NONE) {
 *            // error closing keypad
 *      }
 *  } else {
 *      // an error occurred opening keypad
 *  }
 *  return 0;
 * }
 * @endcode
 */
#include "Config.h"
#include <uEZ.h>
#include <uEZKeypad.h>
#include "Device/Keypad.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>

static T_uezQueue G_uezKeypadQueue = 0;

/*---------------------------------------------------------------------------*
 * Routine:  UEZKeypadOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the Keypad bank.
 *
 *  @param [in]    *aName 		Pointer to name of Keypad (usually "Keypad")
 *
 *  @param [in]    *aDevice		Pointer to device handle to be returned
 *
 *  @return        T_uezError   If the device is opened, returns 
 *                              UEZ_ERROR_NONE.  If the device cannot be found,
 *                              returns UEZ_ERROR_DEVICE_NOT_FOUND. Returns 
 *                              UEZ_ERROR_NOT_AVAILABLE if in use elsewhere.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZKeypad.h>
 *
 *  T_uezDevice keypad;
 *  if (UEZKeypadOpen("Keypad", &keypad) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	
 *  } else {
 *      // an error occurred opening keypad
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZKeypadOpen(
            const char *const aName, 
            T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_Keypad **p;
    
    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    // Create the default queue the first time
    if (G_uezKeypadQueue == 0) {
        // Open the keypad
        (*p)->Open(p);

        error = UEZQueueCreate(6, sizeof(T_uezInputEvent), &G_uezKeypadQueue);
        if (!error) {
            // Now register it for events
            (*p)->Register(p, G_uezKeypadQueue);
        }
    } else {
        error = UEZ_ERROR_NOT_AVAILABLE;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZKeypadClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the Keypad bank.
 *
 *  @param [in]    aDevice 			Device handle of Keypad to close
 *
 *  @return        T_uezError       If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZKeypad.h>
 *
 *  T_uezDevice keypad;
 *  T_uezInputEvent event;
 *  if (UEZKeypadOpen("Keypad", &keypad) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *      if (UEZKeypadClose(keypad) != UEZ_ERROR_NONE) {
 *            // error closing keypad
 *      }
 *  } else {
 *      // an error occurred opening keypad
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZKeypadClose(T_uezDevice aDevice)
{
    T_uezError error = UEZ_ERROR_NONE;
    DEVICE_Keypad **p;
    
    if (G_uezKeypadQueue == 0) {
        // Not in use?
    } else {
        error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
        if (!error) {

            // Now remove it from the registry
            (*p)->Unregister(p, G_uezKeypadQueue);
            
            // And Free the queue
            UEZQueueDelete(G_uezKeypadQueue);
            G_uezKeypadQueue = 0;

            // close the keypad
            (*p)->Close(p);
        }
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZKeypadRead
 *---------------------------------------------------------------------------*/
/**
 *  Waits for a keypad event on the default keypad queue.
 *
 *  @param [in]    aDevice      	Handle to opened Keypad bank device
 *
 *  @param [in]    *aEvent  		Returned event if found
 *
 *  @param [in]    aTimeout         How long to wait until try event
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.  If timeout,
 *                                  returns UEZ_ERROR_TIMEOUT.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZKeypad.h>
 *
 *  T_uezDevice keypad;
 *  T_uezInputEvent event;
 *  if (UEZKeypadOpen("Keypad", &keypad) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *	    if (UEZKeypadRead(keypad, &event, 1000)) == UEZ_ERROR_NONE) {
 *            // keypad read successful, stored in event 
 *            // event.iEvent.iButton.iKey 		// 32 bit
 *            // event.iEvent.iButton.iAction 	//  8 bit
 *      }
 *      if (UEZKeypadClose(keypad) != UEZ_ERROR_NONE) {
 *            // error closing keypad
 *      }
 *  } else {
 *      // an error occurred opening keypad
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZKeypadRead(
            T_uezDevice aDevice, 
            T_uezInputEvent *aEvent,
            TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_Keypad **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return UEZQueueReceive(G_uezKeypadQueue, (void *)aEvent, aTimeout);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZKeypad.c
 *-------------------------------------------------------------------------*/
