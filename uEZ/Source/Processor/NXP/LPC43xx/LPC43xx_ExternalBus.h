/*-------------------------------------------------------------------------*
 * File:  LPC43xx_ExternalBus.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC43xx_EXTERNALBUS_H_
#define LPC43xx_EXTERNALBUS_H_

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

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define EMC_PIN_SET                 ((1 << 7) | (1 << 6) | (1 << 5) | (2 << 3))
//#define EMC_PIN_SET                 ((1 << 7) | (1 << 6) | (1 << 5) | (1 << 4))

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/


/** Port offset definition */
#define PORT_OFFSET     0x80
/** Pin offset definition */
#define PIN_OFFSET      0x04

/* Pin modes */
#define MD_PUP  (0x0<<3)
#define MD_BUK  (0x1<<3)
#define MD_PLN  (0x2<<3)
#define MD_PDN  (0x3<<3)
#define MD_EHS  (0x1<<5)
#define MD_EZI  (0x1<<6)
#define MD_ZI   (0x1<<7)
#define MD_EHD0 (0x1<<8)
#define MD_EHD1 (0x1<<8)
#define MD_PLN_FAST (MD_PLN | MD_EZI | MD_ZI | MD_EHS)
// 0xF0

/* Pin function */
#define FUNC0           0x0             /** Function 0  */
#define FUNC1           0x1             /** Function 1  */
#define FUNC2           0x2             /** Function 2  */
#define FUNC3           0x3             /** Function 3  */
#define FUNC4           0x4
#define FUNC5           0x5
#define FUNC6           0x6
#define FUNC7           0x7
/**
 * @}
 */

#define LPC_SCU_PIN(po, pi)   (*(volatile int32_t         *) (LPC_SCU_BASE + ((po) * 0x80) + ((pi) * 0x4))    )
#define LPC_SCU_CLK(c)        (*(volatile int32_t         *) (LPC_SCU_BASE + 0xC00 + ((c) * 0x4))    )

void scu_pinmux(TUInt8 port, TUInt8 pin, TUInt8 mode, TUInt8 func);

void LPC43xx_ExternalAddressBus_ConfigureIOPins(TUInt8 aBitSize);
void LPC43xx_ExternalDataBus_ConfigureIOPins(TUInt8 aBitSize);

#endif // LPC43xx_EXTERNALBUS_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_ExternalBus.h
 *-------------------------------------------------------------------------*/
