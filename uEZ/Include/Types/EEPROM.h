/*-------------------------------------------------------------------------*
 * File:  Types/EEPROM.h
 *-------------------------------------------------------------------------*
 * Description:
 *     EEPROM types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _EEPROM_TYPES_H_
#define _EEPROM_TYPES_H_

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

/**
 *  @file   /Include/Types/EEPROM.h
 *  @brief  uEZ EEPROM Types
 *
 *  The uEZ EEPROM Types
 *  The following table is a growing list of eeprom types for easy config
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef EEPROM_MAX_PAGE_SIZE
#define EEPROM_MAX_PAGE_SIZE        128
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iCommSpeed; /** in kHz */
    TUInt32 iPageSize;  /** max 64 (EEPROM_MAX_PAGE_SIZE) */
    TUInt32 iWriteTime;
} T_EEPROMConfig;

/** The following table is a growing list of eeprom types for easy config: */
//                                      kHz   bytes     ms
#define EEPROM_CONFIG_PCA24S08          400,    16,     10
#define EEPROM_CONFIG_M24C08            400,    8,      10
#define EEPROM_CONFIG_M24C32            400,    32,     10
#define EEPROM_CONFIG_M24LC256          400,    64,     5
#define EEPROM_CONFIG_M24LC64           400,    1,      5
#define EEPROM_CONFIG_PCF85102C         100,    8,      85
#define EEPROM_CONFIG_PCF8582C_2        100,    8,      85
#define EEPROM_CONFIG_M24M01            400,    128,    10
#define EEPROM_CONFIG_R1EX24512ASAS0A   400,    128,    10

#ifdef __cplusplus
}
#endif

#endif // _EEPROM_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/EEPROM.h
 *-------------------------------------------------------------------------*/
