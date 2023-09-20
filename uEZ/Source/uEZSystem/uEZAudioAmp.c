/*-------------------------------------------------------------------------*
 * File:  uEZAudioAmp.c
 *-------------------------------------------------------------------------*
 * Description:
 *      uEZ AudioAmp API
 * Implementation:
 *      Most of the commands are thin and map directly to the 
 *      API of the Audio Amp device drivers.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZAudioAmp
 *  @{
 *  @brief     uEZ Audio Amp Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ Audio Amp interface.
 *
 * @par Example code:
 * Example task to 
 * @par
 * @code
 *  #include <uEZ.h>
 *  #include <uEZAudioAmp.h>
 *
 *   TUInt32 AudioAmpSet(T_uezTask aMyTask, void *aParams)
 *   {
 *       T_uezDevice amp;
 *       TUInt8 level;
 *       if (UEZAudioAmpOpen("AMP0", &amp) == UEZ_ERROR_NONE) {
 *           // the device opened properly
 *
 *           UEZAudioAmpMute(amp);
 *           UEZAudioAmpUnMute(amp);
 *           level = UEZAudioAmpGetLevel(amp);
 *           UEZAudioAmpSetLevel(amp, 50);
 *           if (UEZAudioAmpClose(amp) != UEZ_ERROR_NONE) {
 *               // error closing device
 *           }
 *       } else {
 *           // an error occurred opening the device
 *       }
 *       return 0;
 *   }
 * @endcode
 */
#include "Config.h"
#include "uEZDevice.h"
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <Device/AudioAmp.h>
#include <uEZAudioAmp.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZAudioAmpOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the Audio Amp.
 *
 * @param [in] *aName   Pointer to name of Audio Amp display (usually "AMP0")
 *
 * @param [in] aDevice  Pointer to device handle to be returned
 *
 * @return T_uezError   If the device is opened, returns UEZ_ERROR_NONE. If the
 * 					    device cannot be found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAudioAmp.h>
 *
 *   T_uezDevice amp;
 *   if (UEZAudioAmpOpen("AMP0", &amp) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *   } else {
 *      // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAudioAmpOpen(const char * const aName, T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_AudioAmp **p;

    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Open((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZAudioAmpClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the Audio Amp
 *
 *  @param [in]    aDevice 	Device handle of ToneGenerator to close
 *
 *  @return     T_uezError	If the device is successfully closed, returns 
 *                          UEZ_ERROR_NONE.  If the device handle is bad, 
 *                          returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAudioAmp.h>
 *
 *   T_uezDevice amp;
 *   if (UEZAudioAmpOpen("AMP0", &amp) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       if (UEZAudioAmpClose(amp) != UEZ_ERROR_NONE) {
 *           // error closing device
 *       }
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAudioAmpClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_AudioAmp **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Close((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZAudioAmpMute
 *---------------------------------------------------------------------------*/
/**
 *  Mute the amp
 *
 *  @param [in]	aDevice 	Device handle of ToneGenerator to close
 *
 *  @return     T_uezError	If the device is successfully closed, returns 
 *                          UEZ_ERROR_NONE.  If the device handle is bad, 
 *                          returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAudioAmp.h>
 *
 *   T_uezDevice amp;
 *   if (UEZAudioAmpOpen("AMP0", &amp) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       UEZAudioAmpMute(amp);
 *       UEZAudioAmpUnMute(amp);
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAudioAmpMute(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_AudioAmp **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Mute((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZAudioAmpUnMute
 *---------------------------------------------------------------------------*/
/**
 *  UnMute the amp 
 *
 *  @param [in]    aDevice 		Device handle of ToneGenerator to close
 *
 *  @return        T_uezError  	If the device is successfully closed, returns
 *	 							UEZ_ERROR_NONE.  If the device handle is bad,
 * 								returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAudioAmp.h>
 *
 *   T_uezDevice amp;
 *   TUInt8 level;
 *   if (UEZAudioAmpOpen("AMP0", &amp) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       UEZAudioAmpMute(amp);
 *       UEZAudioAmpUnMute(amp);
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAudioAmpUnMute(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_AudioAmp **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->UnMute((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZAudioAmpSetLevel
 *---------------------------------------------------------------------------*/
/**
 *  Change the level of the amp up or down  
 *
 *  @param [in]    aDevice		Device handle of Audio Amp to close
 *
 *  @param [in]    aLevel    	the new level to set the device to
 *
 *  @return        T_uezError  	If the device is successfully closed, returns
 *                              UEZ_ERROR_NONE.  If the device handle is bad,
 *                              returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAudioAmp.h>
 *
 *   T_uezDevice amp;
 *   TUInt8 level;
 *   if (UEZAudioAmpOpen("AMP0", &amp) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       level = UEZAudioAmpGetLevel(amp);
 *       UEZAudioAmpSetLevel(amp, 50);
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZAudioAmpSetLevel(T_uezDevice aDevice, TUInt8 aLevel)
{
    T_uezError error;
    DEVICE_AudioAmp **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetLevel(p, aLevel);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZAudioAmpGetLevel
 *---------------------------------------------------------------------------*/
/**
 *  Get the current level of the amp.
 *
 *  @param [in]    aDevice 		Device handle of Audio amp to close
 *
 *  @return        TUInt8    	returns the current level
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZAudioAmp.h>
 *
 *   T_uezDevice amp;
 *   TUInt8 level;
 *   if (UEZAudioAmpOpen("AMP0", &amp) == UEZ_ERROR_NONE) {
 *       // the device opened properly
 *
 *       level = UEZAudioAmpGetLevel(amp);
 *       UEZAudioAmpSetLevel(amp, 50);
 *   } else {
 *       // an error occurred opening the device
 *   }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
TUInt8 UEZAudioAmpGetLevel(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_AudioAmp **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetLevel((void *)p);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZAudioAmp.c
 *-------------------------------------------------------------------------*/
