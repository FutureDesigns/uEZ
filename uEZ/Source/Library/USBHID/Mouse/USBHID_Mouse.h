/*-------------------------------------------------------------------------*
 * File:  USBHID_Mouse.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef USBHID_MOUSE_H_
#define USBHID_MOUSE_H_

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
#define MOUSE_BUTTON_LEFT       0x01
#define MOUSE_BUTTON_RIGHT      0x02
#define MOUSE_BUTTON_MIDDLE     0x04
#define MOUSE_BUTTON_BACK       0x08
#define MOUSE_BUTTON_FORWARD    0x10

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
PACK_STRUCT_BEGIN
typedef ATTR_IAR_PACKED struct {
    PACK_STRUCT_FIELD(TUInt8 iButtons);           // Flags about other keys pressed
    PACK_STRUCT_FIELD(TInt8 iDeltaX);
    PACK_STRUCT_FIELD(TInt8 iDeltaY);
    PACK_STRUCT_FIELD(TInt8 iDeltaWheel);
    PACK_STRUCT_FIELD(TUInt8 iACPan);
    PACK_STRUCT_FIELD(TUInt8 iButtons2);
} PACK_STRUCT_STRUCT T_HIDMouseReport;
PACK_STRUCT_END

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError HIDMouseRead(T_HIDMouseReport *aMouse, TUInt32 aTimeout);
void HIDMouseRequire(const char *aUSBHost);

#ifdef __cplusplus
}
#endif

#endif // USBHID_MOUSE_H_
/*-------------------------------------------------------------------------*
 * End of File:  USBHID_Mouse.h
 *-------------------------------------------------------------------------*/
