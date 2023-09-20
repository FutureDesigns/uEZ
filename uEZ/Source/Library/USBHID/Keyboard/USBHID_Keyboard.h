/*-------------------------------------------------------------------------*
 * File:  USBHID_Keyboard.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef USBHID_KEYBOARD_H_
#define USBHID_KEYBOARD_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
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
#include <uEZPacked.h>
#include <uEZ.h>

#ifdef __cplusplus
extern "C" {
#endif
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define KEYBOARD_SCAN_FLAG_LEFT_CONTROL     0x0001
#define KEYBOARD_SCAN_FLAG_LEFT_SHIFT       0x0002
#define KEYBOARD_SCAN_FLAG_LEFT_ALT         0x0004
#define KEYBOARD_SCAN_FLAG_LEFT_GUI         0x0008
#define KEYBOARD_SCAN_FLAG_RIGHT_CONTROL    0x0010
#define KEYBOARD_SCAN_FLAG_RIGHT_SHIFT      0x0020
#define KEYBOARD_SCAN_FLAG_RIGHT_ALT        0x0040
#define KEYBOARD_SCAN_FLAG_RIGHT_GUI        0x0080

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
PACK_STRUCT_BEGIN
typedef struct {
    PACK_STRUCT_FIELD(TUInt8 iFlags);           // Flags about other keys pressed
    PACK_STRUCT_FIELD(TUInt8 iReserved);
    PACK_STRUCT_FIELD(TUInt8 iKeys[6]);         // Up to 6 keys held simultaneously
} PACK_STRUCT_STRUCT T_HIDKeyboardReport;
PACK_STRUCT_END

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError HIDKeyboardRead(T_HIDKeyboardReport *aKeyscan, TUInt32 aTimeout);
void HIDKeyboardRequire(const char *aUSBHost);
const char *HIDKeyboardScanKeyAsText(TUInt8 aScankey);
T_uezError HIDKeyboardSetIndicators(TUInt8 aIndicators);

#ifdef __cplusplus
}
#endif

#endif // USBHID_KEYBOARD_H_
/*-------------------------------------------------------------------------*
 * End of File:  USBHID_Keyboard.h
 *-------------------------------------------------------------------------*/
