/*-------------------------------------------------------------------------*
 * File:  uEZToneGenerator.c
 *-------------------------------------------------------------------------*
 * Description:
 *      uEZ ToneGenerator API
 * Implementation:
 *      Most of the commands are thin and map directly to the 
 *      API of the ToneGenerator device drivers.
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
 *  @addtogroup uEZToneGenerator
 *  @{
 *  @brief     uEZ ToneGenerator Interface
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *    The uEZ ToneGenerator interface.
 *
 *  @par Example code:
 *  Example task to play tone
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZToneGenerator.h>
 *
 *  TUInt32 TonePlay(T_uezTask aMyTask, void *aParams)
 *  {
 *      T_uezDevice tone;
 *      if (UEZToneGeneratorOpen("ToneGenerator", &tone) == UEZ_ERROR_NONE) {
 *	 
 *	        if (UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1000), 100)
 *                  == UEZ_ERROR_NONE) {
 *              // play 1000Hz sound for 100 ms
 *          }
 *
 *      } else {
 *          // an error occurred opening the tone generator
 *      }
 *      return 0;
 *  }
 *  @endcode
 */
#include "Config.h"
#include "uEZToneGenerator.h"
#include "uEZDevice.h"
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <Device/ToneGenerator.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZToneGeneratorOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the Tone Generator.
 *
 *  @param [in]    *aName 		Pointer to name of ToneGenerator display 
 *								(usually "ToneGenerator")
 *  @param [in]    *aDevice		Pointer to device handle to be returned
 *
 *  @return        T_uezError   If the device is opened, returns 
 *                              UEZ_ERROR_NONE.  If the device cannot be 
 *                              found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZToneGenerator.h>
 *
 *  T_uezDevice tone;
 *  if (UEZToneGeneratorOpen("ToneGenerator", &tone) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *  } else {
 *      // an error occurred opening the tone generator
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZToneGeneratorOpen(
            const char * const aName, 
            T_uezDevice *aDevice)
{
    T_uezError error;
    DEVICE_ToneGenerator **p;
    
    error = UEZDeviceTableFind(aName, aDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Open((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZToneGeneratorClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the Tone Generator.
 *
 *  @param [in]    aDevice 			Device handle of ToneGenerator to close
 *
 *  @return        T_uezError 		If the device is successfully closed, 
 *                                  returns UEZ_ERROR_NONE.  If the device 
 *                                  handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZToneGenerator.h>
 *
 *  T_uezDevice tone;
 *  if (UEZToneGeneratorOpen("ToneGenerator", &tone) == UEZ_ERROR_NONE) {
 *
 *      if (UEZToneGeneratorClose(tone) != UEZ_ERROR_NONE) {
 *            // error closing tone generator
 *      }
 *
 *  } else {
 *      // an error occurred opening the tone generator
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZToneGeneratorClose(T_uezDevice aDevice)
{
    T_uezError error;
    DEVICE_ToneGenerator **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Close((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZToneGeneratorPlayTone
 *---------------------------------------------------------------------------*/
/**
 *  Play the given tone on the tone generator
 *
 *  @param [in]    aDevice     		Handle to opened ToneGenerator device.
 *
 *  @param [in]    aTonePeriod 		Number of cycles for tone (1/Frequency)
 *
 *  @param [in]    aDuration       	Number of milliseconds to play tone
 *
 *  @return        T_uezError      	If successful, returns UEZ_ERROR_NONE.  If
 *                                 	a timeout occurs, returns 
 *                                  UEZ_ERROR_TIMEOUT.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZToneGenerator.h>
 *
 *  T_uezDevice tone;
 *  if (UEZToneGeneratorOpen("ToneGenerator", &tone) == UEZ_ERROR_NONE) {
 *	 
 *	    if (UEZToneGeneratorPlayTone(tone, TONE_GENERATOR_HZ(1000),100)
 *              == UEZ_ERROR_NONE) {
 *          // play 1000Hz sound for 100 ms
 *      }
 *
 *  } else {
 *      // an error occurred opening the tone generator
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZToneGeneratorPlayTone(
            T_uezDevice aDevice, 
            TUInt32 aTonePeriod,
            TUInt32 aDuration)
{
    T_uezError error;
    DEVICE_ToneGenerator **p;
    
    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->SetTone(p, aTonePeriod);
    if (error)
        return error;
    UEZTaskDelay(aDuration);

    return (*p)->SetTone(p, TONE_GENERATOR_OFF);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZToneGeneratorPlayToneContinuous
 *---------------------------------------------------------------------------*/
/**
 *  Play the given tone on the tone generator forever
 *
 *  @param [in]    aDevice      	Handle to opened ToneGenerator device.
 *
 *  @param [in]    aTonePeriod      Number of cycles for tone (1/Frequency)
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If
 *                                  a timeout occurs, returns
 *                                  UEZ_ERROR_TIMEOUT.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZToneGenerator.h>
 *
 *  T_uezDevice tone;
 *  if (UEZToneGeneratorOpen("ToneGenerator", &tone) == UEZ_ERROR_NONE) {
 *	 
 *	    if (UEZToneGeneratorPlayToneContinuous(tone, TONE_GENERATOR_HZ(1000))
 *	            == UEZ_ERROR_NONE) {
 *          // play 1000Hz tone
 *      }
 *
 *  } else {
 *      // an error occurred opening the tone generator
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZToneGeneratorPlayToneContinuous(
            T_uezDevice aDevice,
            TUInt32 aTonePeriod)
{
    T_uezError error;
    DEVICE_ToneGenerator **p;

    error = UEZDeviceTableGetWorkspace(aDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetTone(p, aTonePeriod);
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZToneGenerator.c
 *-------------------------------------------------------------------------*/
