/*-------------------------------------------------------------------------*
 * File:  LPC43xx_EMAC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC43xx_EMAC_H_
#define LPC43xx_EMAC_H_

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
#include <HAL/EMAC.h>
#include <Types/GPIO.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
#define ETH_MTU           1500   /* Max. Ethernet Frame Size (Used by LwIP) */
  
#define EMAC_ETH_MAX_FLEN 1536      /* Max. Ethernet Frame Size          */
#define ETH_FRAG_SIZE     EMAC_ETH_MAX_FLEN   /* Packet Fragment size 1536 Bytes   */
#define ETH_MAX_FLEN      EMAC_ETH_MAX_FLEN   /* Max. Ethernet Frame Size          */

typedef struct {
    T_uezGPIOPortPin iTX_EN;    // ENET_TX_ENn  (O)     Transmit data enable, active low
    T_uezGPIOPortPin iTXD3;     // ENET_TXD[3]  (O)     Transmit data, bit 3
    T_uezGPIOPortPin iTXD2;     // ENET_TXD[2]  (O)     Transmit data, bit 2
    T_uezGPIOPortPin iTXD1;     // ENET_TXD[1]  (O)     Transmit data, bit 1
    T_uezGPIOPortPin iTXD0;     // ENET_TXD[0]  (O)     Transmit data, bit 0
    T_uezGPIOPortPin iTX_ER;    // ENET_TX_ER   (O)     Transmit error
    T_uezGPIOPortPin iTX_CLK;   // ENET_TX_CLK  (I)     Transmitter clock
    T_uezGPIOPortPin iRX_DV;    // ENET_RX_DV   (I)     Receive data valid
    T_uezGPIOPortPin iRXD3;     // ENET_RXD[3]  (I)     Receive data, bit 3
    T_uezGPIOPortPin iRXD2;     // ENET_RXD[2]  (I)     Receive data, bit 2
    T_uezGPIOPortPin iRXD1;     // ENET_RXD[1]  (I)     Receive data, bit 1
    T_uezGPIOPortPin iRXD0;     // ENET_RXD[0]  (I)     Receive data, bit 0
    T_uezGPIOPortPin iRX_ER;    // ENET_RX_ER   (I)     Receive error
    T_uezGPIOPortPin iRX_CLK;   // ENET_RX_CLK  (I)     Receive clock
    T_uezGPIOPortPin iCOL;      // ENET_COL     (I)     Collision detect
    T_uezGPIOPortPin iCRS;      // ENET_CRS     (I)     Carrier sense
    T_uezGPIOPortPin iMDC;      // ENET_MDC     (I)     MIIM clock
    T_uezGPIOPortPin iMDIO;     // ENET_MDIO    (IO)    MIIM data
} T_LPC43xx_EMAC_Settings;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const HAL_EMAC G_LPC43xx_EMAC_Interface;
void LPC43xx_EMAC_RMII_Require(const T_LPC43xx_EMAC_Settings *aSettings);

#endif // LPC43xx_EMAC_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_EMAC.h
 *-------------------------------------------------------------------------*/
