/*-------------------------------------------------------------------------*
 * File:  LPC1768_USBHost.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#ifndef LPC1768_USBHOST_H_
#define LPC1768_USBHOST_H_

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
        T_uezGPIOPortPin iINTn;     // USB_INTxn:   (I)     OTG ATX Interrupt
        T_uezGPIOPortPin iSCL;      // USB_SCLx:    (IO)    OTG I2C serial clock
        T_uezGPIOPortPin iSDA;      // USB_SDAx:    (IO)    OTG I2C serial data USB SDAx
        T_uezGPIOPortPin iTX_E;     // USB_TX_Exn:  (O)     OTG Transmit enable
        T_uezGPIOPortPin iTX_DP;    // USB_RX_DPx:  (O)     OTG D+ receive data
        T_uezGPIOPortPin iTX_DM;    // USB_RX_DMx:  (O)     OTG D- receive data
        T_uezGPIOPortPin iRCV;      // USB_RCVx:    (I)     OTG diffirential receive data
        T_uezGPIOPortPin iRX_DP;    // USB_RX_DPx:  (I)     OTG D+ receive data
        T_uezGPIOPortPin iRX_DM;    // USB_RX_DMx:  (I)     OTG D- receive data
        T_uezGPIOPortPin iLSn;      // USB_LSxn:    (O)     OTG Low speed status
        T_uezGPIOPortPin iSSPNDn;   // USB_SSPNDn:  (O)     OTG Bus suspend status
        T_uezGPIOPortPin iPPWRn;    // USB_PPWRn:   (O)     Host Port power enable
        T_uezGPIOPortPin iPWRDn;    // USB_PPWRDn:  (I)     Host Port power status
        T_uezGPIOPortPin iOVRCRn;   // USB_OVRCRn:  (I)     Host Port Over-current status
        T_uezGPIOPortPin iHSTENn;   // USB_HSTENxn: (O)     Host enabled status
} T_LPC1768_USBHost_Settings;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_USBHost G_LPC1768_USBHost_Interface_PortA;
extern const HAL_USBHost G_LPC1768_USBHost_Interface_PortB;

void LPC1768_USBHost_PortA_Require(const T_LPC1768_USBHost_Settings *aSettings);
void LPC1768_USBHost_PortB_Require(const T_LPC1768_USBHost_Settings *aSettings);

#ifdef __cplusplus
}
#endif

#endif // LPC1768_USBHOST_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC1768_USBHost.h
 *-------------------------------------------------------------------------*/
