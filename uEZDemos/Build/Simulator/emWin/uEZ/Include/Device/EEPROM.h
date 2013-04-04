/*-------------------------------------------------------------------------*
 * File:  EEPROM.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ EEPROM Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_EEPROM_H_
#define _DEVICE_EEPROM_H_

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
 *  @file   /Include/Device/EEPROM.h
 *  @brief  uEZ EEPROM Device Interface
 *
 *  The uEZ EEPROM Device Interface
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
#include <uEZDevice.h>
#include <Types/EEPROM.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*SetConfiguration)(
            void *aWorkspace, 
            const T_EEPROMConfig *aConfig);
    T_uezError (*Read)(
            void *aWorkspace, 
            TUInt32 aAddress,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
    T_uezError (*Write)(
            void *aWorkspace, 
            TUInt32 aAddress,
            TUInt8 *aBuffer,
            TUInt32 aNumBytes);
} DEVICE_EEPROM;

#endif // _DEVICE_EEPROM_H_
/*-------------------------------------------------------------------------*
 * End of File:  EEPROM.h
 *-------------------------------------------------------------------------*/

