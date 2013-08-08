/*-------------------------------------------------------------------------*
 * File:  ToneGenerator.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ ToneGenerator Device
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_ToneGenerator_H_
#define _DEVICE_ToneGenerator_H_

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
 *  @file   /Include/Device/ToneGenerator.h
 *  @brief  uEZ Tone Generator Device Interface
 *
 *  The uEZ Tone Generator Device Interface
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
 * Constants:
 *-------------------------------------------------------------------------*/
/**
 * Macro to convert number (typically float) into 16.16 fixed point Hz
 * format.  Works fine on integers too.
 */
#define TONE_GENERATOR_OFF        0
#define TONE_GENERATOR_HZ(freq) \
    ((TUInt32)(PROCESSOR_OSCILLATOR_FREQUENCY/(freq)))

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_uezDeviceInterface iDevice;

   /**
    * Set the ToneGenerator output in number of processor cycles
    */
    T_uezError (*Open)(void *aWorkspace);
    T_uezError (*Close)(void *aWorkspace);
    T_uezError (*SetTone)(void *aWorkspace, TUInt32 aTonePeriod);
} DEVICE_ToneGenerator;

#endif // _DEVICE_ToneGenerator_H_
/*-------------------------------------------------------------------------*
 * End of File:  ToneGenerator.h
 *-------------------------------------------------------------------------*/

