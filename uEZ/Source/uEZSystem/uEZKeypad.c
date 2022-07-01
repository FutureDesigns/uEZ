/*-------------------------------------------------------------------------*
 * File:  uEZKeypad.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Keypad Interface
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
 *  @addtogroup uEZKeypad
 *  @{
 *  @brief     uEZ Keypad Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ Keypad interface.
 *
 *  @par Example code:
 *  Example task to read the keypad
 *  @par
 *  @code
 *  #include <UEZKeypad.h>
 *  #include <Types/InputEvent.h>
 *       
 *  T_uezError error;
 *  T_uezDevice keypadDevice;
 *  T_uezInputEvent keypadEvent;
 *         
 *  UEZPlatform_ButtonBoard_Require();
 *       
 *  error = UEZKeypadOpen("BBKeypad", &keypadDevice);
 *  if(error)
 *    UEZFailureMsg("Cannot open 'BBKeypad'");
 *  else {  
 *    // Continuously poll the keypad for key strokes
 *    while(1) {
 *       error = UEZKeypadRead(keypadDevice, &keypadEvent, 500);
 *       if(error)
 *          printf("Keypad error: %d\r\n", error);
 *       else {
 *          if(keypadEvent.iEvent.iButton.iKey != KEY_NONE) {
 *             if(keypadEvent.iEvent.iButton.iAction == INPUT_EVENT_ACTION_PRESS)
 *                printf("Button Press: %x - PRESS\r\n", keypadEvent.iEvent.iButton.iKey);
 *             else if(keypadEvent.iEvent.iButton.iAction == INPUT_EVENT_ACTION_RELEASE)
 *                printf("Button Press: %x - RELEASE\r\n", keypadEvent.iEvent.iButton.iKey);
 *             else
 *                printf("Button Press: %x - REPEAT\r\n", keypadEvent.iEvent.iButton.iKey);
            }
 *       }
 *    } 
 * }
 * @endcode
 */
#include "Config.h"
#include <uEZ.h>
#include <uEZKeypad.h>
#include "Device/Keypad.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>

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
 *  @param [in]    aEventQueue  Queue that stores events
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
            T_uezDevice *aDevice,
            T_uezQueue *aEventQueue)
{
    T_uezError error;
    DEVICE_Keypad **p;
    
    error = UEZDeviceTableFind(aName, aDevice);

    if(!error)
        error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    
    if(!error)
        error = (*p)->Open(p); 
    
    if((!error) || (error == UEZ_ERROR_ALREADY_EXISTS)) {
        error = (*p)->Register(p, *aEventQueue);
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
 *  @param [in]    aEventQueue 		Queue that stored events
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
T_uezError UEZKeypadClose(T_uezDevice aDevice, T_uezQueue *aEventQueue)
{
    T_uezError error = UEZ_ERROR_NONE;
    DEVICE_Keypad **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);

    if(!error)
        (*p)->Close(p);
    
    if(!error)
        error = (*p)->Unregister(p,*aEventQueue);
    
    return error;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZKeypad.c
 *-------------------------------------------------------------------------*/
