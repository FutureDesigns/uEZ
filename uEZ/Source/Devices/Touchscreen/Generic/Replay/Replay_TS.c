/*-------------------------------------------------------------------------*
 * File:  Replay_TS.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Four Wire Touchscreen interface.
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <Device/Touchscreen.h>
#include <uEZFile.h>
#include "Replay_TS.h"
#include <string.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
typedef enum {
    REPLAY_MODE_PASS_THROUGH,
    REPLAY_MODE_RECORD,
    REPLAY_MODE_PLAY,
} T_replayMode;

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
     TUInt32 iTime;
     T_uezTSReading iReading;
} T_replayEntry;

typedef struct {
    const DEVICE_TOUCHSCREEN *iDevice;
    int32_t aNumOpen;

    // Hardware linkage
    T_uezSemaphore iSem;
    DEVICE_TOUCHSCREEN **iExistingTS;
    T_replayMode iMode;
    T_uezFile iFile;
    TUInt32 iStartTime;
    T_replayEntry iLastEntry;
} T_TS_Replay_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN TS_Replay_Interface;
static T_uezError TS_Replay_WaitForTouch(
        void *aWorkspace,
        TUInt32 aTimeout);

/*---------------------------------------------------------------------------*
 * Routine:  TS_Replay_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_Replay_InitializeWorkspace(void *aW)
{
    T_TS_Replay_Workspace *p =
            (T_TS_Replay_Workspace *) aW;

    p->aNumOpen = 0;
    p->iMode = REPLAY_MODE_PASS_THROUGH;
    p->iFile = 0;

    return UEZSemaphoreCreateBinary(&p->iSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_Replay_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_Replay_Open(void *aW)
{
    T_TS_Replay_Workspace *p =
            (T_TS_Replay_Workspace *) aW;
    p->aNumOpen++;

    return (*p->iExistingTS)->Open(p->iExistingTS);
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_Replay_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_Replay_Close(void *aW)
{
    T_TS_Replay_Workspace *p =
            (T_TS_Replay_Workspace *) aW;
    p->aNumOpen--;

    return (*p->iExistingTS)->Close(p->iExistingTS);
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_Replay_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_Replay_Poll(
        void *aWorkspace,
        T_uezTSReading *aReading)
{
    T_TS_Replay_Workspace *p =
            (T_TS_Replay_Workspace *) aWorkspace;
    T_uezError error;
    TUInt32 num;
    TUInt32 time;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    time = UEZTickCounterGet() - p->iStartTime;
    if (p->iMode == REPLAY_MODE_RECORD)  {
        error = (*p->iExistingTS)->Poll(p->iExistingTS, aReading);
        p->iLastEntry.iTime = time;
        p->iLastEntry.iReading = *aReading;
        if (p->iFile)
            UEZFileWrite(p->iFile, &p->iLastEntry, sizeof(p->iLastEntry), &num);
    } else if (p->iMode == REPLAY_MODE_PLAY)  {
        // Is the screen being touched?  If so abort the playback
        error = (*p->iExistingTS)->Poll(p->iExistingTS, aReading);
        if ((error == UEZ_ERROR_NONE) && (aReading->iFlags)) {
            UEZFileClose(p->iFile);
            p->iFile = 0;
            p->iMode = REPLAY_MODE_PASS_THROUGH;
        } else {
            // Time to read more?
            if (time >= p->iLastEntry.iTime) {
                error = UEZFileRead(p->iFile, &p->iLastEntry, sizeof(p->iLastEntry), &num);
                if (error == UEZ_ERROR_NONE) {
                    // Copy over the data
                } else {
                    // Error or reached the end
                    UEZFileClose(p->iFile);
                    p->iFile = 0;
                    p->iMode = REPLAY_MODE_PASS_THROUGH;
                }
            }
            // Use the last entry for now
            *aReading = p->iLastEntry.iReading;
            error = UEZ_ERROR_NONE;
        }
    } else {
        // Passthrough catches normal stuff.
        error = (*p->iExistingTS)->Poll(p->iExistingTS, aReading);
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_Replay_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_Replay_CalibrateStart(void *aWorkspace)
{
    T_TS_Replay_Workspace *p =
            (T_TS_Replay_Workspace *) aWorkspace;
    return (*p->iExistingTS)->CalibrationStart(p->iExistingTS);
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_Replay_CalibrateEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      Calibration is complete.  Compute the calibration matrix and use.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *                                      Return ERROR_OUT_OF_RANGE if
 *                                      calibration is bad.  Returns
 *                                      UEZ_ERROR_NOT_ENOUGH_DATA if not
 *                                      enough data points were added.
 *---------------------------------------------------------------------------*/
static T_uezError TS_Replay_CalibrateEnd(void *aWorkspace)
{
    T_TS_Replay_Workspace *p =
            (T_TS_Replay_Workspace *) aWorkspace;
    return (*p->iExistingTS)->CalibrationEnd(p->iExistingTS);
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_Replay_CalibrateAdd
 *---------------------------------------------------------------------------*
 * Description:
 *      Add a calibration point to the calibration data.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const T_uezTSReading *aReadingTaken -- Raw reading values
 *      const T_uezTSReading *aReadingExpected -- Expected output reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_Replay_CalibrateAdd(
        void *aWorkspace,
        const T_uezTSReading *aReadingTaken,
        const T_uezTSReading *aReadingExpected)
{
    T_TS_Replay_Workspace *p =
            (T_TS_Replay_Workspace *) aWorkspace;
    return (*p->iExistingTS)->CalibrationAddReading(p->iExistingTS, aReadingTaken,
        aReadingExpected);
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_Replay_SetTouchDetectSensitivity
 *---------------------------------------------------------------------------*
 * Description:
 *      Sets the touch sensitivity for detecting a touch on the screen.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt16 aLowLevel           -- 16-bit low level.  0xFFFF is full 100%
 *                                      sensitivity, 0 is no sensitivity.
 *      TUInt16 aHighLevel          -- 16-bit high level.  0xFFFF is no (0%)
 *                                      sensitivity, 0xFFFF is no sensitivity.
 * Outputs:
 *      T_uezError                  -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_Replay_SetTouchDetectSensitivity(
        void *aWorkspace,
        TUInt16 aLowLevel,
        TUInt16 aHighLevel)
{
    T_TS_Replay_Workspace *p =
            (T_TS_Replay_Workspace *) aWorkspace;
    return (*p->iExistingTS)->SetTouchDetectSensitivity(p->iExistingTS,
        aLowLevel, aHighLevel);
}

static T_uezError TS_Replay_WaitForTouch(
        void *aWorkspace,
        TUInt32 aTimeout)
{
    T_TS_Replay_Workspace *p =
            (T_TS_Replay_Workspace *) aWorkspace;
    return (*p->iExistingTS)->WaitForTouch(p->iExistingTS, aTimeout);
}

void Touchscreen_Replay_Create(const char *aName, const char *aExistingTS)
{
    T_TS_Replay_Workspace *p_ts;
    T_uezDevice ts;

    // Need to register touchscreen device
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Touchscreen_Replay_Interface,
            0, (T_uezDeviceWorkspace **)&p_ts);

    UEZDeviceTableFind(aExistingTS, &ts);
    UEZDeviceTableGetWorkspace(ts, (T_uezDeviceWorkspace **)&p_ts->iExistingTS);
}

void Touchscreen_Replay_RecordStart(const char *aName, const char *aFilename)
{
    T_TS_Replay_Workspace *p;
    T_uezDevice ts;
    
    UEZDeviceTableFind(aName, &ts);
    UEZDeviceTableGetWorkspace(ts, (T_uezDeviceWorkspace **)&p);
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (UEZFileOpen(aFilename, FILE_FLAG_WRITE, &p->iFile) == UEZ_ERROR_NONE) {
        p->iStartTime = UEZTickCounterGet();
        p->iMode = REPLAY_MODE_RECORD;
    } else {
        p->iFile = 0;
    }
    UEZSemaphoreRelease(p->iSem);
}

void Touchscreen_Replay_Stop(const char *aName)
{
    T_TS_Replay_Workspace *p;
    T_uezDevice ts;

    UEZDeviceTableFind(aName, &ts);
    UEZDeviceTableGetWorkspace(ts, (T_uezDeviceWorkspace **)&p);
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p->iFile) {
        UEZFileClose(p->iFile);
        p->iFile = 0;
        p->iLastEntry.iTime = 0;
        p->iLastEntry.iReading.iFlags = 0;
        p->iLastEntry.iReading.iPressure = 0;
        p->iLastEntry.iReading.iX = 0;
        p->iLastEntry.iReading.iY = 0;
    }
    p->iMode = REPLAY_MODE_PASS_THROUGH;
    UEZSemaphoreRelease(p->iSem);
}

void Touchscreen_Replay_PlayStart(const char *aName, const char *aFilename)
{
    T_TS_Replay_Workspace *p;
    T_uezDevice ts;

    UEZDeviceTableFind(aName, &ts);
    UEZDeviceTableGetWorkspace(ts, (T_uezDeviceWorkspace **)&p);
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (UEZFileOpen(aFilename, FILE_FLAG_READ_ONLY, &p->iFile) == UEZ_ERROR_NONE) {
        p->iStartTime = UEZTickCounterGet();
        p->iMode = REPLAY_MODE_PLAY;
        p->iLastEntry.iTime = 0;
        p->iLastEntry.iReading.iFlags = 0;
        p->iLastEntry.iReading.iPressure = 0;
        p->iLastEntry.iReading.iX = 0;
        p->iLastEntry.iReading.iY = 0;
    } else {
        p->iFile = 0;
    }
    UEZSemaphoreRelease(p->iSem);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_Replay_Interface = {
    {
        // Common interface
        "Touchscreen:Generic:4Wire",
        0x0105,
        TS_Replay_InitializeWorkspace,
        sizeof(T_TS_Replay_Workspace),
    },

    // Functions
    TS_Replay_Open,
    TS_Replay_Close,
    TS_Replay_Poll,

    TS_Replay_CalibrateStart,
    TS_Replay_CalibrateAdd,
    TS_Replay_CalibrateEnd,

    TS_Replay_SetTouchDetectSensitivity,

    // v1.05 Functions
    TS_Replay_WaitForTouch,
};

/*-------------------------------------------------------------------------*
 * End of File:  Replay_TS.c
 *-------------------------------------------------------------------------*/
