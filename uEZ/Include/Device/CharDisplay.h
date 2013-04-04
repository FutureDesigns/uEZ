/*-------------------------------------------------------------------------*
 * File:  CharDisplay.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ CharDisplay Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _DEVICE_CharDisplay_H_
#define _DEVICE_CharDisplay_H_

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

#endif // _DEVICE_CharDisplay_H_
/*-------------------------------------------------------------------------*
 * End of File:  CharDisplay.h
 *-------------------------------------------------------------------------*/
