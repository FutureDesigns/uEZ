/*-------------------------------------------------------------------------*
 * File:  SimpleUI_UtilityFunctions.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Utility routines for the Simple UI interface.
 *-------------------------------------------------------------------------*/

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
 *  @addtogroup SimpleUI
 *  @{
 *  @brief     uEZ SimpleUI Utility Functions
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZLicense.txt
 *
 *  uEZ SimpleUI routines for doing a variety of other actions.
 *
 */

#include <string.h>
#include <uEZ.h>
#include <uEZFile.h>
#include "SimpleUI.h"
#include <uEZPlatform.h>
#include <uEZPlatformAPI.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Source/Library/Graphics/SWIM/lpc_swim_driver.h>

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_SUISettings G_SUISettings = {
// Double sized icons?  Default = false/no
    EFalse,
    // FlipX
    EFalse,
    // FlipY
    EFalse, };

/*---------------------------------------------------------------------------*
 * Routine:  SUICopyFast32
 *---------------------------------------------------------------------------*/
/**
 *  Optimized routine for copying data usually in the form of whole frames
 *  of screens (where we don't care about the exact pixel order).
 *
 *  @param [in]    aDest        Destination of raw data
 *  @param [in]    aSrc         Source of raw data
 *  @param [in]    aNumBytes    Number of raw bytes to copy
 *
 *  @return        void

 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
 *
 *  SUICopyFast32((void *)FRAME(2), (void *)FRAME(0), FRAME_SIZE);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void SUICopyFast32(void *aDest, void *aSrc, TUInt32 aNumBytes)
{
    memcpy(aDest, aSrc, aNumBytes);
}

/*---------------------------------------------------------------------------*
 * Routine:  ScreenShot
 *---------------------------------------------------------------------------*/
/**
 *  Saves the screen of the given window to a file.  The format of the output
 *  is a .RAW format (each pixel is 24-bits in linear order).  Photoshop
 *  is the only known application that can read this format.
 *
 *  @param [in]    aWin         Pointer to SWIM window with screen.
 *  @param [in]    aFilename    Filename of file to output.  Suggest ends with
 *                              .RAW extension.
 *
 *  @return        T_uezError   Error code.

 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
 *
 *  swim_window_open_noclear(&win, DISPLAY_WIDTH, DISPLAY_HEIGHT, DISPLAY_ADDR,
 *      0, 0, DISPLAY_WIDTH-1, DISPLAY_HEIGHT-1, 0, WHITE, BLACK, RED);
 *  SUIScreenShot(&win, "0:\SCREEN.RAW");
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
T_uezError SUIScreenShot(SWIM_WINDOW_T *aWin, const char *aFilename)
{
    T_uezFile file;
    TUInt32 x, y;
    TUInt8 rgb[3];
    TUInt16 pixel;
    TUInt32 num;
    T_uezError error;

    error = UEZFileOpen(aFilename, FILE_FLAG_WRITE, &file);
    if (error == UEZ_ERROR_NONE) {
        for (y = 0; y < aWin->xpmax; y++) {
            for (x = 0; x < aWin->xpmax; x++) {
                swim_driver_get_raster(aWin, x, y, &pixel, 1);
                // Convert 5:5:5 to RGB
                rgb[0] = ((pixel >> 10) & 0x1F) << 3;
                rgb[1] = ((pixel >> 5) & 0x1F) << 3;
                rgb[2] = ((pixel >> 0) & 0x1F) << 3;
                error = UEZFileWrite(file, rgb, 3, &num);
                if (error)
                    break;
            }
            if (error)
                break;
        }
        UEZFileClose(file);
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SUISetSettings
 *---------------------------------------------------------------------------*/
/**
 *  Declare the settings set to use of the SimpleUI library.  This routine
 *  should be called before calling any other SimpleUI routines.
 *
 *  @param [in]    aSettings    Settings to use.
 *
 *  @return        void

 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
 *
 *  const T_SUISettings G_mySettings = {
 *      EFalse, // do not double the size of icons
 *      EFalse, // do not flip the X
 *      ETrue,  // do flip the Y
 *  };
 *  SUISetSettings(&G_mySettings);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void SUISetSettings(const T_SUISettings *aSettings)
{
    // Copy over the settings
    G_SUISettings = *aSettings;
    swim_driver_set_orientation(aSettings->iFlipX, aSettings->iFlipY);
    if (G_SUISettings.iWindow.fb == 0)
        UEZFailureMsg("SUISettings missing SWIM_WINDOW_T!");
}

SWIM_WINDOW_T *SUIGetDrawWindow()
{
    // Copy over the settings
    return &G_SUISettings.iWindow;
}

void SUISetDrawWindow(const SWIM_WINDOW_T *aWindow)
{
    // Copy over the settings
    G_SUISettings.iWindow = *aWindow;
}

/*---------------------------------------------------------------------------*
 * Routine:  SUIInitialize
 *---------------------------------------------------------------------------*/
/**
 *  Initialize the SimpleUI system to using the given doubling of icons,
 *  flipping of x, flipping of y settings based on the current platform's
 *  LCD size and width.
 *
 *  @param [in]    aDoubleSize  ETrue if icons are to be double size, else EFalse
 *  @param [in]    aFlipX       ETrue if X axis is to be reversed, else EFalse
 *  @param [in]    aFlipY       ETrue if Y axis is to be reversed, else EFalse
 *
 *  @return        void

 *  @par Example Code:
 *  @code
 *  #include <uEZ.h>
 *  #include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>
 *
 *  // Setup the SimpleUI to use normal sized icons, and flip both the
 *  // x and y axis to flip the screen 180 degrees.  It sets up the default
 *  // routines to draw to the platform's current screen size.
 *  SUIInitialize(EFalse, ETrue, ETrue);
 *
 *  @endcode
 */
/*---------------------------------------------------------------------------*/
void SUIInitialize(TBool aDoubleSize, TBool aFlipX, TBool aFlipY)
{
    T_SUISettings SUISettings;
    const TUInt32 width = UEZPlatform_LCDGetWidth();
    const TUInt32 height = UEZPlatform_LCDGetHeight();

    SUISettings.iDoubleSizeIcons = aDoubleSize;
    SUISettings.iFlipX = aFlipX;
    SUISettings.iFlipY = aFlipY;

    // Configure the SimpleUI's window for size information and where
    // the first frame buffer is located.
    swim_window_open_noclear(&SUISettings.iWindow, width, height,
        (COLOR_T *)UEZPlatform_GetBaseAddress(), 0, 0, width - 1, height - 1, 0, WHITE,
        BLACK, RED);
    SUISetSettings(&SUISettings);
}

/** @} */
/*-------------------------------------------------------------------------*
 * File:  SimpleUI_UtilityFuncs.c
 *-------------------------------------------------------------------------*/
