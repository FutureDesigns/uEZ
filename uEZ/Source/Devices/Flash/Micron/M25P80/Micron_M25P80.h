/*-------------------------------------------------------------------------*
 * File:  Micron_M25P80.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef Micron_M25P80_H_
#define Micron_M25P80_H_

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
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
* licensing section of http://goo.gl/UDtTCR for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Device/Flash.h>
#include <Types/Flash.h>
#include <UEZGPIO.h>
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void Flash_Micron_M25P80_Create(
        const char *aName,
        const char* aSPIPortName,
        T_uezGPIOPortPin aChipSelect,
        T_uezGPIOPortPin aWriteProtect,
        T_uezGPIOPortPin aReset);
extern const DEVICE_Flash Flash_Micron_M25P80_Interface;

#endif // Micron_M25P80_H_
/*-------------------------------------------------------------------------*
 * End of File:  Micron_M25P80.h
 *-------------------------------------------------------------------------*/
