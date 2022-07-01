/*-------------------------------------------------------------------------*
 * File:  uEZLCD.c
 *-------------------------------------------------------------------------*
 * Description:
 *      uEZ LCD API
 * Implementation:
 *      Most of the commands are thin and map directly to the 
 *      API of the LCD device drivers.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
/**
 *    @addtogroup uEZLCD
 *  @{
 *  @brief     uEZ LCD Interface
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZ LCD interface.
 *
 *  @par Example code:
 *  Example task to open the lcd device and show a frame
 *  @par
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 * 
 * TUInt32 LCDShowFrame(T_uezTask aMyTask, void *aParams)
 * {
 *  T_uezDevice lcd;
 *  T_pixelColor *pixels;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *      UEZLCDOn(lcd);
 * 		UEZLCDGetFrame(lcd, 0, (void **)&pixels);
 *	 	UEZLCDShowFrame(lcd, 1);
 *
 *      if (UEZLCDClose(lcd) != UEZ_ERROR_NONE) {
 *            // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *     return 0;
 * }
 * @endcode
 */
#include <Config.h>
#include <uEZLCD.h>
#include <HAL/HAL.h>
#include <Device/LCD.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the LCD display (and turn on).
 *
 *  @param [in]    *aName 		Pointer to name of LCD display (usually "LCD")
 *
 *  @param [in]    *aLCDDevice	Pointer to device handle to be returned
 *
 *  @return        T_uezError   If the device is opened, returns 
 *                              UEZ_ERROR_NONE.  If the device cannot be 
 *                              found, returns UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDOpen(
            const char * const aName, 
            T_uezDevice *aLCDDevice)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the LCD display.  May turn off LCD if no one
 *      else is using the LCD.
 *
 *  @param [in]    aLCDDevice	Device handle of LCD to close
 *
 *  @return        T_uezError   If the device is successfully closed, returns 
 *                              UEZ_ERROR_NONE.  If the device handle is bad,
 *                              returns UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *      if (UEZLCDClose(lcd) != UEZ_ERROR_NONE) {
 *            // error closing the device
 *      }
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDClose(T_uezDevice aLCDDevice)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDGetInfo
 *---------------------------------------------------------------------------*/
/**
 *  Get information about the LCD device.
 *
 *  @param [in]    aLCDDevice    	Handle to opened LCD device.
 *
 *  @param [in]    *aConfiguration  Pointer to info structure
 *
 *  @return        T_uezError     	If successful, returns UEZ_ERROR_NONE.  If 
 *                                  the device cannot be found, returns 
 *                                  UEZ_ERROR_DEVICE_NOT_FOUND.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  T_uezLCDConfiguration aConfiguration;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 	UEZLCDGetInfo(lcd, &aConfiguration);
 *      // data available in aConfiguration now
 *	//	 typedef struct {
 *  // 		TUInt32 iXResolution;
 *  // 		TUInt32 iYResolution;
 *  // 		T_uezLCDColorDepth iColorDepth;
 *  // 		T_uezLCDPixelOrder iPixelOrder;
 *  // 		TUInt32 iCurrentFrame;
 *  // 		TUInt32 iNumTotalFrames;
 *  //		TUInt32 iBytesPerVisibleRaster;
 *  //		TUInt32 iBytesPerWholeRaster;
 *  //		TUInt32 iNumBacklightLevels;
 *  //   } T_uezLCDConfiguration;
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDGetInfo(
            T_uezDevice aLCDDevice,
            T_uezLCDConfiguration *aConfiguration)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDGetFrame
 *---------------------------------------------------------------------------*/
/**
 *  Returns a pointer to the frame memory in the LCD display.
 *
 *  @param [in]    aLCDDevice    	Handle to opened LCD device.
 *
 *  @param [in]    aFrame          	Index to frame (0 based)
 *
 *  @param [in]    **aFrameBuffer   Pointer to base address
 *
 *  @return        T_uezError     	If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.  If the frame is 
 *                                  incorrect, returns UEZ_ERROR_OUT_OF_RANGE
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  T_pixelColor *pixels;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 * 		UEZLCDGetFrame(lcd, 0, (void **)&pixels);
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDGetFrame(
            T_uezDevice aLCDDevice, 
            TUInt32 aFrame, 
            void **aFrameBuffer)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDShowFrame
 *---------------------------------------------------------------------------*/
/**
 *  Makes the passed frame the actively viewed frame on the LCD 
 *      (if not already).
 *
 *  @param [in]    aLCDDevice   Handle to opened LCD device.
 *
 *  @param [in]    aFrame		Frame number
 *
 *  @return        T_uezError   If successful, returns UEZ_ERROR_NONE. If an 
 *                              invalid device handle, returns 
 *                              UEZ_ERROR_HANDLE_INVALID.  If the frame is 
 *                              incorrect, returns UEZ_ERROR_OUT_OF_RANGE
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 	UEZLCDShowFrame(lcd, 1); 
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDShowFrame(
            T_uezDevice aLCDDevice, 
            TUInt32 aFrame)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDOn
 *---------------------------------------------------------------------------*/
/**
 *  Turns on the LCD display.
 *
 *  @param [in]    aLCDDevice    Handle to opened LCD device.
 *
 *  @return        T_uezError    If successful, returns UEZ_ERROR_NONE.  If 
 *                               an invalid device handle, returns 
 *                               UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 	UEZLCDOn(lcd);
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDOn(T_uezDevice aLCDDevice)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDOff
 *---------------------------------------------------------------------------*/
/**
 *  Turns off the LCD display.
 *
 *  @param [in]    aLCDDevice       Handle to opened LCD device.
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  an invalid device handle, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 	UEZLCDOff(T_uezDevice aLCDDevice);
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDOff(T_uezDevice aLCDDevice)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDBacklight
 *---------------------------------------------------------------------------*/
/**
 *  Turns on or off the backlight.  A value of 0 is off and values of 1 
 *      or higher is higher levels of brightness (dependent on the LCD 
 *      display).  If a display is on/off only, this value will be 1 or 0 
 *      respectively.
 *
 *  @param [in]    aLCDDevice       Handle to opened LCD device.
 *
 *  @param [in]    aLevel           Level of backlight
 *
 *  @return        T_uezError     	If successful, returns UEZ_ERROR_NONE.  If 
 *                                  the device handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.  If the 
 *                                  backlight intensity level is invalid, 
 *                                  returns UEZ_ERROR_OUT_OF_RANGE.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 	UEZLCDBacklight(lcd, 100);
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDBacklight(
            T_uezDevice aLCDDevice, 
            TUInt32 aLevel)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDSetPaletteColor
 *---------------------------------------------------------------------------*/
/**
 *  Change the color of a palette entry given the red, green, blue
 *      component of a particular color index.  The color components are
 *      expressed in full 16-bit values at a higher resolution than
 *      the hardware can usually perform.  The color is down shifted to what
 *      the hardware can handle.
 *
 *  @param [in]    aLCDDevice       Handle to opened LCD device.
 *
 *  @param [in]    aColorIndex      Index to palette entry
 *
 *  @param [in]    aRed             Red value
 *
 *  @param [in]    aGreen           Green value
 *
 *  @param [in]    aBlue            Blue value
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  the device handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.  If the 
 *                                  color index  is invalid, 
 *                                  returns UEZ_ERROR_OUT_OF_RANGE.
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *	 	UEZLCDSetPaletteColor(lcd, 0, 0x1F, 0x1F, 0x1F);
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDSetPaletteColor(
            T_uezDevice aLCDDevice,
            TUInt32 aColorIndex, 
            TUInt16 aRed, 
            TUInt16 aGreen, 
            TUInt16 aBlue)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDGetBacklightLevel
 *---------------------------------------------------------------------------*/
/**
 *  Returns the current backlight level from the LCD and optionally
 *      returns the maximum number of levels.
 *
 *  @param [in]    aLCDDevice		Handle to opened LCD device.
 *
 *  @param [in]    *aLevel          Level of backlight
 *
 *  @param [in]    *aNumLevels		Pointer to total number of backlight levels
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
 *                                  the device handle is bad, returns 
 *                                  UEZ_ERROR_HANDLE_INVALID.  
 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <uEZLCD.h>
 *
 *  T_uezDevice lcd;
 *  TUInt32 aLevel;
 *  TUInt32 aNumLevels;
 *  if (UEZLCDOpen("LCD", &lcd) == UEZ_ERROR_NONE) {
 *    	// the device opened properly
 *	 
 *		UEZLCDGetBacklightLevel(lcd, &aLevel, &aNumLevels);
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDGetBacklightLevel(
            T_uezDevice aLCDDevice, 
            TUInt32 *aLevel,
            TUInt32 *aNumLevels)
{
    T_uezError error = UEZ_ERROR_NONE;
    
    return error;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZLCD.c
 *-------------------------------------------------------------------------*/
