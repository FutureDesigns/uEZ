/*-------------------------------------------------------------------------*
 * File:  CharDisplay.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ CharDisplay Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_CharDisplay_H_
#define _DEVICE_CharDisplay_H_

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
 *  @file   /Include/Device/CharDisplay.h
 *  @brief  uEZ CharDisplay Device Interface
 *
 *  The uEZ CharDisplay Device Interface
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
    TUInt32 iNumColumns;
    TUInt32 iNumRows;
    TUInt32 iFeatures;
        #define CHAR_DISPLAY_FEATURE_CLEAR_SCREEN          (1<<0)
        #define CHAR_DISPLAY_FEATURE_SCROLL_UP             (1<<1)
} T_charDisplayInfo;

typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    const T_charDisplayInfo *(*GetInfo)(void *aWorkspace);
    T_uezError (*Open)(void *aWorkspace);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*Control)(
        void *aWorkspace, 
        TUInt32 aInstruction, 
        void *aInstructionData);
    T_uezError (*DrawChar)(
        void *aWorkspace, 
        TUInt32 aColumn, 
        TUInt32 aRow, 
        TUInt16 aChar);
    T_uezError (*ClearScreen)(void *aWorkspace);
    T_uezError (*ScrollUp)(void *aWorkspace);
} DEVICE_CharDisplay;

#ifdef __cplusplus
}
#endif

#endif // _DEVICE_CharDisplay_H_
/*-------------------------------------------------------------------------*
 * End of File:  CharDisplay.h
 *-------------------------------------------------------------------------*/
