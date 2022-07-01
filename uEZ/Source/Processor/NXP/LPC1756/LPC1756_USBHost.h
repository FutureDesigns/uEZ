/*-------------------------------------------------------------------------*
 * File:  LPC1756_USBHost.h
 *-------------------------------------------------------------------------*/
#ifndef LPC1756_USBHOST_H_
#define LPC1756_USBHOST_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
/**
 *  @file   LPC1756_USBHost.h
 *  @brief  uEZ 1756 USB Host
 *
 *  uEZ 1756 USB Host
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <HAL/USBHost.h>
#include <Types/GPIO.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        T_uezGPIOPortPin iDP;       // USB_D+x:     (IO)    USB Connector
        T_uezGPIOPortPin iDM;       // USB_D-x:     (IO)    USB Connector
        T_uezGPIOPortPin iCONNECT;  // USB_CONNECTn:(O)     Control
        T_uezGPIOPortPin iUP_LED;   // USB_UP_LEDn: (O)     Control
        T_uezGPIOPortPin iPPWRn;    // USB_PPWRn:   (O)     Host Port power enable
        T_uezGPIOPortPin iPWRDn;    // USB_PPWRDn:  (I)     Host Port power status
} T_LPC1756_USBHost_Settings;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_USBHost G_LPC1756_USBHost_Interface;

void LPC1756_USBHost_Require(const T_LPC1756_USBHost_Settings *aSettings);

#ifdef __cplusplus
}
#endif

#endif // LPC1756_USBHOST_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1756_USBHost.h
 *-------------------------------------------------------------------------*/
