/*-------------------------------------------------------------------------*
 * File:  Timer.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_TIMER_H_
#define _DEVICE_TIMER_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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
 *  @file   /Include/Device/Timer.h
 *  @brief  uEZ Timer Device Interface
 *
 *  The uEZ Timer Device Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Types/Timer.h>
#include <uEZDevice.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef void (*T_DeviceTimer_Callback)(void *aCallbackWorkspace);

typedef struct {
        // Header
        T_uezDeviceInterface iDevice;

        // Functions
        /**
		 * Added uEZ v1.13
		 */
        T_uezError (*Reset)(void *aWorkspace);
        T_uezError (*Enable)(void *aWorkspace);
        T_uezError (*Disable)(void *aWorkspace);

        T_uezError (*SetTimerMode)(void *aWorkspace, T_Timer_Mode aMode);
        T_uezError (*SetCaptureSource)(void *aWorkspace, TUInt32 aSourceIndex);
        T_uezError (*SetMatchRegister)(
                void *aWorkspace,
                TUInt8 aMatchRegister,
                TUInt32 aMatchPoint,
                T_Timer_MatchPointFunction aFunctions);
        T_uezError (*SetExternalControl)(
                void *aWorkspace,
                TUInt8 aMatchRegister,
                T_Timer_ExternalControl aExtControl);

        T_uezError (*SetMatchCallback)(
                void *aWorkspace,
                TUInt8 aMatchRegister,
                T_DeviceTimer_Callback aCallbackFunc,
                void *aCallbackWorkspace);
} DEVICE_Timer;

#endif // _DEVICE_TIMER_H_
/*-------------------------------------------------------------------------*
 * End of File:  Timer.h
 *-------------------------------------------------------------------------*/
