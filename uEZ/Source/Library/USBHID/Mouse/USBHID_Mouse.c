/*-------------------------------------------------------------------------*
 * File:  USBHID_Mouse.c
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
#include <stdio.h>
#include <uEZ.h>
#include <uEZDevice.h>
#include <uEZHID.h>
#include <Source/Devices/HID/Generic/HID_Generic.h>
#include "USBHID_Mouse.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
T_uezDevice G_HIDMouse0=0;

T_uezError HIDMouseRead(T_HIDMouseReport *aMouse, TUInt32 aTimeout)
{
    return UEZHIDRead(G_HIDMouse0, (void *)aMouse, sizeof(*aMouse), aTimeout);
}

void HIDMouseRequire(const char *aUSBHost)
{
    DEVICE_CREATE_ONCE();

    // Create a HID to the TrippLite on this USB Host
    HID_Generic_Create("HIDMouse0", aUSBHost);

    // Leave it open all the time (only one in system)
    UEZHIDOpen(&G_HIDMouse0, "HIDMouse0", 0, 0);
}

/*-------------------------------------------------------------------------*
 * End of File:  USBHID_Mouse.c
 *-------------------------------------------------------------------------*/
