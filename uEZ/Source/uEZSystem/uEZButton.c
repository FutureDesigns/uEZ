/*-------------------------------------------------------------------------*
 * File:  uEZButton.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
/**
 *  @addtogroup uEZButton
 *  @{
 *  @brief     uEZ Button Bank Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ Button Bank interface.
 *
 * @par Example code:
 * Example task to use the Button Bank.
 * @par
 * @code
 *  #include <uEZ.h>
 *  #include <uEZButton.h>
 *
 *   TUInt32 ButtonBankSet(T_uezTask aMyTask, void *aParams)
 *   {
 *       T_uezDevice ButtonBank;
 *       TUInt32 buttonsActive = 0x10; // button 5
 *       TUInt32 ButtonsPressed;
 *       if (UEZButtonBankOpen("Button", &ButtonBank) == UEZ_ERROR_NONE) {
 *           // the device opened properly
 *
 *           if (UEZButtonBankSetActive(ButtonBank, &buttonsActive) == UEZ_ERROR_NONE) {
 *               // set active successful
 *           }
 *           if (UEZButtonBankRead(ButtonBank, &ButtonsPressed) == UEZ_ERROR_NONE) {
 *               // got buttons pressed
 *               if (ButtonsPressed && 0x10) {
 *                    // button 5 pressed
 *               }
 *           }
 *           if (UEZButtonBankClose(ButtonBank) != UEZ_ERROR_NONE) {
 *               // error closing button bank
 *           }
 *       } else {
 *           // an error occurred opening Button Bank
 *       }
 *       return 0;
 *   }
 * @endcode
 */

#include <uEZ.h>
#include <uEZButton.h>
#include <Device/ButtonBank.h>
#include <uEZDeviceTable.h>

/*-------------------------------------------------------------------------*
 * Routines:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  UEZButtonBankOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to a button bank.
 *
 *  @param [in]    *aName 			Pointer to name of button bank (usually
 *                                      "Button0" where x is bank number)
 *  @param [out]    *aDevice 		Pointer to device handle to be returned
 * 
 *  @return        T_uezError       Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZButton.h>
 *
 *   T_uezDevice ButtonBank;
 *   if (UEZButtonBankOpen("Button", &ButtonBank) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *   } else {
 *       // an error occurred opening Button Bank
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZButtonBankOpen(const char * const aName, T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_ButtonBank **p;

    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZButtonBankClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the button bank.
 *
 *  @param [in]    aDevice 			Device handle of button bank to close
 *
 *  @return        T_uezError       Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZButton.h>
 *
 *   T_uezDevice ButtonBank;
 *   if (UEZButtonBankOpen("Button", &ButtonBank) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *       if (UEZButtonBankClose(ButtonBank) != UEZ_ERROR_NONE) {
 *           // error closing button bank
 *       }
 *   } else {
 *       // an error occurred opening Button Bank
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZButtonBankClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_ButtonBank **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZButtonBankSetActive
 *---------------------------------------------------------------------------*/
/**
 *  Declare which buttons in the button bank are active.  A bit of 1
 *      means active, a bit of 0 means inactive.
 *
 *  @param [in]    aDevice 				Device handle of button bank to set active
 *
 *  @param [in]    aButtonsActive 		Bits telling which buttons are active.
 *
 *  @return        T_uezError       	Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZButton.h>
 *
 *   T_uezDevice ButtonBank;
 *   TUInt32 buttonsActive = 0x10; // button 5
 *   if (UEZButtonBankOpen("Button", &ButtonBank) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       if (UEZButtonBankSetActive(ButtonBank, &buttonsActive) == UEZ_ERROR_NONE) {
 *           // set active successful
 *       }
 *       if (UEZButtonBankClose(ButtonBank) != UEZ_ERROR_NONE) {
 *           // error closing button bank
 *       }
 *   } else {
 *       // an error occurred opening Button Bank
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZButtonBankSetActive(T_uezDevice aDevice, TUInt32 aButtonsActive)
{
    T_uezError error;
    DEVICE_ButtonBank **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetActiveButtons((void *)p, aButtonsActive);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZButtonBankRead
 *---------------------------------------------------------------------------*/
/**
 *  Returns the buttons that have been pressed.
 *
 *  @param [in]    aDevice 				Device handle of button bank to read
 *
 *  @param [out]   *aButtonsPressed 	Bits telling which buttons are/were pressed.
 *
 *  @return        T_uezError       	Error code
 *
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZButton.h>
 *
 *   T_uezDevice ButtonBank;
 *   TUInt32 buttonsActive = 0x10; // button 5
 *   TUInt32 ButtonsPressed;
 *   if (UEZButtonBankOpen("Button", &ButtonBank) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       if (UEZButtonBankSetActive(ButtonBank, &buttonsActive) == UEZ_ERROR_NONE) {
 *           // set active successful
 *       }
 *       if (UEZButtonBankRead(ButtonBank, &ButtonsPressed) == UEZ_ERROR_NONE) {
 *           // got buttons pressed
 *           if (ButtonsPressed && 0x10) {
 *               // button 5 pressed
 *           }
 *       }
 *       if (UEZButtonBankClose(ButtonBank) != UEZ_ERROR_NONE) {
 *           // error closing button bank
 *       }
 *   } else {
 *       // an error occurred opening Button Bank
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZButtonBankRead(T_uezDevice aDevice, TUInt32 *aButtonsPressed)
{
    T_uezError error;
    DEVICE_ButtonBank **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Read((void *)p, aButtonsPressed);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZButton.c
 *-------------------------------------------------------------------------*/
