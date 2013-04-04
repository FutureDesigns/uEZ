/*-------------------------------------------------------------------------*
 * File:  AR1020ResistiveTouchScreen.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Four Wire Touchscreen interface.
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

#include <uEZ.h>
#define NUM_CALIBRATE_POINTS_NEEDED     3

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
 typedef struct {
     
    const DEVICE_TOUCHSCREEN *iDevice;
    int aNumOpen;

    // Calibration information
    TBool iIsCalibrating;
    TUInt32 iNumCalibratePoints;
    T_uezTSReading iCalibrateReadingsTaken[NUM_CALIBRATE_POINTS_NEEDED];
    T_uezTSReading iCalibrateReadingsExpected[NUM_CALIBRATE_POINTS_NEEDED];

    TBool iHaveCalibration;

    TInt32 iPenOffsetX;
    TInt32 iPenBaseX;
    TInt32 iPenXScaleTop;
    TInt32 iPenXScaleBottom;

    TInt32 iPenOffsetY;
    TInt32 iPenBaseY;
    TInt32 iPenYScaleTop;
    TInt32 iPenYScaleBottom;

    TInt32 iLastX;
    TInt32 iLastY;
    T_uezTSFlags iLastTouch;

    TUInt32 iTDSLow;
    TUInt32 iTDSHigh;
    TBool iTDSWasPressed;

    T_uezSemaphore iSemWaitForTouch;

} T_MC_AR1020Workspace;
/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
 extern const DEVICE_TOUCHSCREEN Touchscreen_MC_AR1020_Interface;
 T_uezError Touchscreen_MC_AR1020_Create(const char *aName);

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*
 * End of File:  AR1020ResistiveTouchScreen.h
 *-------------------------------------------------------------------------*/
