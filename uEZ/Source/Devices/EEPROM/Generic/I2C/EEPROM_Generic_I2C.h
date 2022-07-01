/*-------------------------------------------------------------------------*
 * File:  EEPROM_Generic_I2C_driver.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef EEPROM_GENERIC_I2C_DRIVER_H_
#define EEPROM_GENERIC_I2C_DRIVER_H_

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
#include <Device/EEPROM.h>
#include <Device/I2CBus.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_EEPROM EEPROM_Generic_I2C_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void EEPROM_Generic_I2C_Configure(
        void *aWorkspace,
        DEVICE_I2C_BUS **aI2C,
        TUInt8 aI2CAddr);

void EEPROM_Generic_I2C_Create(
        const char *aName,
        const char *aI2CBusName,
        TUInt8 aI2CAddr,
        const T_EEPROMConfig *aEEPROMConfig);

#ifdef __cplusplus
}
#endif

#endif // EEPROM_GENERIC_I2C_DRIVER_H_
/*-------------------------------------------------------------------------*
 * End of File:  EEPROM_Generic_I2C_driver.h
 *-------------------------------------------------------------------------*/
