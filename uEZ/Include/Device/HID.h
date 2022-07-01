/*-------------------------------------------------------------------------*
 * File:  HID.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef DEVICE_HID_H_
#define DEVICE_HID_H_

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
 *  @file   /Include/Device/HID.h
 *  @brief  uEZ HID Device Interface
 *
 *  The uEZ HID Device Interface
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
#define HID_DEVICE_IDLE_INDEFINITE              0

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

   /**
	* v1.11 uEZ Functions:
	*/
    T_uezError (*Open)(
        void *aWorkspace,
        TUInt16 aVenderID,
        TUInt16 aProductID);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*Read)(
        void *aWorkspace,
        TUInt8 *aData,
        TUInt32 aLength,
        TUInt32 aTimeout);
    T_uezError (*Write)(
        void *aWorkspace,
        const TUInt8 *aData,
        TUInt32 aLength,
        TUInt32 *aNumWritten,
        TUInt32 aTimeout);
    T_uezError (*GetReportDescriptor)(
        void *aWorkspace,
        TUInt8 *aData,
        TUInt32 aMaxLength);
    T_uezError (*GetFeatureReport)(
        void *aWorkspace,
        TUInt8 aReportID,
        TUInt8 *aData,
        TUInt32 aMaxLength,
        TUInt32 aTimeout);
    T_uezError (*SetFeatureReport)(
        void *aWorkspace,
        TUInt8 aReportID,
        TUInt8 *aData,
        TUInt32 aMaxLength,
        TUInt32 aTimeout);
    T_uezError (*GetString)(
        void *aWorkspace,
        TUInt8 aIndex,
        TUInt8 *aData,
        TUInt32 aMaxLength,
        TUInt32 aTimeout);
    T_uezError (*GetInputReport)(
        void *aWorkspace,
        TUInt8 aReportID,
        TUInt8 *aData,
        TUInt32 aMaxLength,
        TUInt32 aTimeout);
    T_uezError (*SetOutputReport)(
        void *aWorkspace,
        TUInt8 aReportID,
        TUInt8 *aData,
        TUInt32 aMaxLength,
        TUInt32 aTimeout);
} DEVICE_HID;

#ifdef __cplusplus
}
#endif

#endif // DEVICE_HID_H_
/*-------------------------------------------------------------------------*
 * End of File:  HID.h
 *-------------------------------------------------------------------------*/
