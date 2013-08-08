/*-------------------------------------------------------------------------*
 * File:  Keypad_Generic_GPIO.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef KEYPAD_GPIO_H
#define KEYPAD_GPIO_H

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2013 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Device/Keypad.h>
#include <Types/InputEvent.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezGPIOPin iPin;
    TUInt32 iKey;
} T_GPIOKeypadAssignment;

enum {
    KEYPAD_LOW_TRUE_SIGNALS,
    KEYPAD_HIGH_TRUE_SIGNALS
}typedef T_KeypadGPIOPolarity;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_Keypad Keypad_Generic_GPIO_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError Keypad_Generic_GPIO_Create(
    const char *aName,
    TUInt32 aGPIOPort,
    const T_GPIOKeypadAssignment *aKeyAssignment,
    TUInt32 aNumKeys,
    T_KeypadGPIOPolarity aGPIOPolarity,
    T_uezGPIOPortPin aPowerLedPortPin,
    T_uezGPIOPortPin aKeyPressLedPortPin);

#endif // KEYPAD_GPIO_H
/*-------------------------------------------------------------------------*
 * End of File:  Keypad_Generic_GPIO.h
 *-------------------------------------------------------------------------*/