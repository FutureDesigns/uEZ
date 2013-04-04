/*-------------------------------------------------------------------------*
 * File:  AudioAmp.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Audio Amp Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_AudioAmp_H_
#define _DEVICE_AudioAmp_H_

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
 *  @file   /Include/Device/AudioAmp.h
 *  @brief  uEZ AudioAmp Device Interface
 *
 *  The uEZ AudioAmp Device Interface
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

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
 typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

    // Functions
    T_uezError (*Open)(void *aWorkspace);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*UnMute)(void *aWorkspace);
    T_uezError (*Mute)(void *aWorkspace);
    TUInt8 (*GetLevel)(void *aWorkspace);
    T_uezError (*SetLevel)(
            void *aWorkspace, 
            TUInt8 aLevel);
} DEVICE_AudioAmp;
#endif // _DEVICE_AudioAmp_H_
/*-------------------------------------------------------------------------*
 * End of File:  AudioAmp.h
 *-------------------------------------------------------------------------*/
