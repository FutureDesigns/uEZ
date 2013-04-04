/*-------------------------------------------------------------------------*
 * File:  Types/EEPROM.h
 *-------------------------------------------------------------------------*
 * Description:
 *     EEPROM types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _EEPROM_TYPES_H_
#define _EEPROM_TYPES_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
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
//                                  kHz   bytes     ms
#define EEPROM_CONFIG_PCA24S08      400,    16,     10
#define EEPROM_CONFIG_M24C08        400,    8,      10
#define EEPROM_CONFIG_M24C32        400,    32,     10
#define EEPROM_CONFIG_M24LC256      400,    64,     5
#define EEPROM_CONFIG_M24LC64       400,    1,      5
#define EEPROM_CONFIG_PCF85102C     100,    8,      85
#define EEPROM_CONFIG_PCF8582C_2    100,    8,      85
#define EEPROM_CONFIG_M24M01        400,    128,    10

#endif // _EEPROM_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/EEPROM.h
 *-------------------------------------------------------------------------*/
