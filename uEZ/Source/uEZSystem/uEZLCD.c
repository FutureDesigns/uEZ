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
 *       // the device opened properly
 *        
 *      UEZLCDOn(lcd);
 *               UEZLCDGetFrame(lcd, 0, (void **)&pixels);
 *               UEZLCDShowFrame(lcd, 1);
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
#include <uEZ.h>
#include "Config.h"
#include "uEZLCD.h"
#include "HAL/HAL.h"
#include "Device/LCD.h"
#include "uEZDevice.h"
#include <uEZDeviceTable.h>

enum {
    LCD_SS_IDLE_START,
    LCD_SS_IDLE,
    LCD_SS_DIM_START,
    LCD_SS_DIM,
    LCD_SS_ANIMATE_START,
    LCD_SS_ANIMATE_UPDATE,
    LCD_SS_SLEEP_START,
    LCD_SS_SLEEP,
    LCD_SS_RESTART,
        LCD_SS_STOPPED
}typedef T_ssState;
T_ssState G_ssState;

static T_uezDevice G_ssLCD=0;
static T_uezLCDScreenSaverInfo G_ssInfo;
static TBool G_ssWake = EFalse;
static T_uezTask G_ssTask;
static TBool G_ssIsActive = EFalse;
static TBool G_ssTaskActive = EFalse;
static TBool G_ssBacklightON = ETrue;
//static TBool G_ssLCD_ON = ETrue;
static TBool G_ssStop = EFalse;
static TUInt32 G_ssOriginalBLLevel;

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDOpen
 *---------------------------------------------------------------------------*/
/**
 *  Open up access to the LCD display (and turn on).
 *
 *  @param [in]    *aName               Pointer to name of LCD display (usually "LCD")
 *
 *  @param [in]    *aLCDDevice       Pointer to device handle to be returned
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
 *       // the device opened properly
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
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableFind(aName, aLCDDevice);
    if (error)
        return error;

    error = UEZDeviceTableGetWorkspace(*aLCDDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    error = (*p)->Open((void *)p);
    if (error)
        return error;

//    return (*p)->On((void *)p);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDClose
 *---------------------------------------------------------------------------*/
/**
 *  End access to the LCD display.  May turn off LCD if no one
 *      else is using the LCD.
 *
 *  @param [in]    aLCDDevice       Device handle of LCD to close
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
 *       // the device opened properly
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
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableGetWorkspace(aLCDDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Close((void *)p);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDGetInfo
 *---------------------------------------------------------------------------*/
/**
 *  Get information about the LCD device.
 *
 *  @param [in]    aLCDDevice       Handle to opened LCD device.
 *
 *  @param [in]    *aConfiguration  Pointer to info structure
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
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
 *       // the device opened properly
 *               UEZLCDGetInfo(lcd, &aConfiguration);
 *      // data available in aConfiguration now
 *       //        typedef struct {
 *  //               TUInt32 iXResolution;
 *  //               TUInt32 iYResolution;
 *  //               T_uezLCDColorDepth iColorDepth;
 *  //               T_uezLCDPixelOrder iPixelOrder;
 *  //               TUInt32 iCurrentFrame;
 *  //               TUInt32 iNumTotalFrames;
 *  //               TUInt32 iBytesPerVisibleRaster;
 *  //               TUInt32 iBytesPerWholeRaster;
 *  //               TUInt32 iNumBacklightLevels;
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
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableGetWorkspace(aLCDDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetInfo((void *)p, aConfiguration);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDGetFrame
 *---------------------------------------------------------------------------*/
/**
 *  Returns a pointer to the frame memory in the LCD display.
 *
 *  @param [in]    aLCDDevice       Handle to opened LCD device.
 *
 *  @param [in]    aFrame               Index to frame (0 based)
 *
 *  @param [in]    **aFrameBuffer   Pointer to base address
 *
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
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
 *       // the device opened properly
 *               UEZLCDGetFrame(lcd, 0, (void **)&pixels);
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
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableGetWorkspace(aLCDDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetFrame((void *)p, aFrame, aFrameBuffer);
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
 *  @param [in]    aFrame               Frame number
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
 *       // the device opened properly
 *               UEZLCDShowFrame(lcd, 1); 
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
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableGetWorkspace(aLCDDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->ShowFrame((void *)p, aFrame);
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
 *       // the device opened properly
 *               UEZLCDOn(lcd);
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDOn(T_uezDevice aLCDDevice)
{
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableGetWorkspace(aLCDDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->On((void *)p);
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
 *       // the device opened properly
 *               UEZLCDOff(T_uezDevice aLCDDevice);
 *  } else {
 *      // an error occurred opening the device
 *  }
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError UEZLCDOff(T_uezDevice aLCDDevice)
{
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableGetWorkspace(aLCDDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->Off((void *)p);
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
 *  @return        T_uezError       If successful, returns UEZ_ERROR_NONE.  If 
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
 *       // the device opened properly
 *               UEZLCDBacklight(lcd, 100);
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
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableGetWorkspace(aLCDDevice, (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;
    
    return (*p)->SetBacklightLevel((void *)p, aLevel);
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
 *       // the device opened properly
 *        
 *               UEZLCDSetPaletteColor(lcd, 0, 0x1F, 0x1F, 0x1F);
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
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableGetWorkspace(
                aLCDDevice, 
                (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->SetPaletteColor(
                (void *)p, 
                aColorIndex, 
                aRed, 
                aGreen, 
                aBlue);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZLCDGetBacklightLevel
 *---------------------------------------------------------------------------*/
/**
 *  Returns the current backlight level from the LCD and optionally
 *      returns the maximum number of levels.
 *
 *  @param [in]    aLCDDevice               Handle to opened LCD device.
 *
 *  @param [in]    *aLevel          Level of backlight
 *
 *  @param [in]    *aNumLevels               Pointer to total number of backlight levels
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
 *       // the device opened properly
 *        
 *               UEZLCDGetBacklightLevel(lcd, &aLevel, &aNumLevels);
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
    T_uezError error;
    DEVICE_LCD **p;
    
    error = UEZDeviceTableGetWorkspace(
                aLCDDevice, 
                (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->GetBacklightLevel(p, aLevel, aNumLevels);
}

T_uezError UEZLCDWaitForVerticalSync(T_uezDevice aLCDDevice, TUInt32 aTimeout)
{
    T_uezError error;
    DEVICE_LCD **p;

    error = UEZDeviceTableGetWorkspace(
                aLCDDevice,
                (T_uezDeviceWorkspace **)&p);
    if (error)
        return error;

    return (*p)->WaitForVerticalSync(p, aTimeout);
}

void UEZLCDScreensaverTask(void)
{
    TUInt32 startTime=0, animUpdateTime=0;
    TUInt32 numLevels;
    G_ssTaskActive = ETrue;
    G_ssIsActive = EFalse;
    G_ssState = LCD_SS_IDLE_START;
        G_ssStop = EFalse; 
    while(!G_ssStop) {
        
        if(G_ssWake)
            G_ssState = LCD_SS_IDLE_START;
        
        switch(G_ssState) {
            case LCD_SS_IDLE_START:
                
                if((G_ssIsActive) && (G_ssInfo.iCallback_AnimationEnd))
                    G_ssInfo.iCallback_AnimationEnd();
                
                // Lets check if the backlight is already on
                if(G_ssBacklightON == EFalse) {
                    // Turn the backlight on to full if it is dimmed or off
                    UEZLCDBacklight(G_ssLCD, G_ssOriginalBLLevel);
                    G_ssBacklightON = ETrue;
                }
                /*                
                if(G_ssLCD_ON == EFalse) {
                    UEZLCDOn(G_ssLCD);
                    G_ssLCD_ON = ETrue;
                }
                */
                startTime = UEZTickCounterGet();
                G_ssIsActive = EFalse;
                G_ssWake = EFalse;
                G_ssState++;
                break;
            case LCD_SS_IDLE:
                if((UEZTickCounterGet()-startTime) > G_ssInfo.iMSTillDim)
                    G_ssState++;
    
                break;
            case LCD_SS_DIM_START:
                UEZLCDGetBacklightLevel(G_ssLCD, &G_ssOriginalBLLevel, &numLevels);
                UEZLCDBacklight(G_ssLCD, G_ssOriginalBLLevel/2);
                G_ssBacklightON = EFalse;
                G_ssIsActive = ETrue;
                G_ssState++;
                break;
            case LCD_SS_DIM:
                if((UEZTickCounterGet()-startTime) > G_ssInfo.iMSTillAnimation)
                    G_ssState++;
                break;
            case LCD_SS_ANIMATE_START:
                if(G_ssInfo.iCallback_AnimationStart)
                    G_ssInfo.iCallback_AnimationStart();
                if(G_ssInfo.iCallback_AnimationUpdate)
                    G_ssInfo.iCallback_AnimationUpdate();
                animUpdateTime = UEZTickCounterGet();
                UEZLCDBacklight(G_ssLCD, G_ssOriginalBLLevel);
                G_ssBacklightON = EFalse;
                G_ssState++;
                break;
            case LCD_SS_ANIMATE_UPDATE:
                if((UEZTickCounterGet()-animUpdateTime) > G_ssInfo.iMSAnimationRefresh) {
                    animUpdateTime = UEZTickCounterGet();
                    if(G_ssInfo.iCallback_AnimationUpdate)
                        G_ssInfo.iCallback_AnimationUpdate();
                }
                if((UEZTickCounterGet()-startTime) > G_ssInfo.iMSTillSleep)
                    G_ssState++;
                break;
            case LCD_SS_SLEEP_START:
              // TODO: modify to turn off LCD and backlight on all units
                //UEZLCDOff(G_ssLCD);
                //G_ssLCD_ON = EFalse;
                UEZLCDBacklight(G_ssLCD, 0);
                G_ssBacklightON = EFalse;
                G_ssInfo.iCallback_AnimationSleep(); // draw black screen to draw over image
                G_ssState++;
                break;
            case LCD_SS_SLEEP:
                // Wait until UEZLCDScreensaverWake()
                break;
            default:
                G_ssState = LCD_SS_IDLE_START;
                break;
        }
        UEZTaskDelay(50);
    }
        G_ssState = LCD_SS_STOPPED;
    G_ssTaskActive = EFalse;
}

T_uezError UEZLCDScreensaverStart(
    T_uezDevice aLCDDevice,
    const T_uezLCDScreenSaverInfo *aLCDScreenSaverInfo)
{
    TUInt32 numLevels;
    
    if(!G_ssTaskActive) {
        G_ssLCD = aLCDDevice;
        G_ssInfo = *aLCDScreenSaverInfo;
        G_ssInfo.iAnimationBacklightLevel = 50;
        G_ssInfo.iDimBacklightLevel = 50;
        
        UEZLCDGetBacklightLevel(G_ssLCD, &G_ssOriginalBLLevel, &numLevels);
        
        return UEZTaskCreate(
            (T_uezTaskFunction)UEZLCDScreensaverTask, 
            "Screensaver",
            UEZ_TASK_STACK_BYTES(512), 0,
            UEZ_PRIORITY_HIGH, &G_ssTask);
        
    } else {
        return UEZ_ERROR_BUSY;
    }
}

T_uezError UEZLCDScreensaverStop()
{
    if(G_ssTaskActive) {
                G_ssStop = ETrue;
                while (G_ssState != LCD_SS_STOPPED) { 
                        UEZTaskDelay(1);
                }
        G_ssIsActive = EFalse;   
    }
    
    return UEZ_ERROR_NONE;
}

void UEZLCDScreensaverWake()
{
    G_ssWake = ETrue;
}

TBool UEZLCDScreensaverIsActive() {
    return G_ssIsActive;
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZLCD.c
 *-------------------------------------------------------------------------*/
