/*-------------------------------------------------------------------------*
 * File:  DirectDrive_TS.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Four Wire Touchscreen interface.
 *
 *	Under Construction - Pending an RX ADC driver
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZADC.h>
#include <uEZGPIO.h>
#include "DirectDrive_TS.h"
 
/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#ifndef NUM_CALIBRATE_POINTS_NEEDED
#define NUM_CALIBRATE_POINTS_NEEDED     3
#endif

#ifndef NUM_DUMMY_SAMPLES_PER_READING
#define NUM_DUMMY_SAMPLES_PER_READING   16
#endif

#ifndef NUM_SAMPLES_PER_READING
#define NUM_SAMPLES_PER_READING         32
#endif

#define TS_GPIO_OUTPUT_MODE(gpio)   UEZGPIOOutput(p->iConfig.gpio.iGPIO)
#define TS_GPIO_INPUT_MODE(gpio)    UEZGPIOInput(p->iConfig.gpio.iGPIO)
#define TS_GPIO_CLEAR(gpio)         UEZGPIOClear(p->iConfig.gpio.iGPIO)
#define TS_GPIO_SET(gpio)           UEZGPIOSet(p->iConfig.gpio.iGPIO)

#define COMPARE_LIMIT (256)

ADC_RequestSingle G_TS_ADCRequest;

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_TOUCHSCREEN *iDevice;
    int32_t aNumOpen;

    // Hardware linkage
    TS_DirectDrive_Configuration iConfig;

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

    T_uezSemaphore iSem;

    TUInt32 iTDSLow;
    TUInt32 iTDSHigh;
    TBool iTDSWasPressed;
	
	T_uezDevice iADC_XL;
    T_uezDevice iADC_XR;
	T_uezDevice iADC_YU;
    T_uezDevice iADC_YD;
} T_TS_DirectDrive_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN TS_DirectDrive_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_DirectDrive_InitializeWorkspace(void *aW)
{
    T_TS_DirectDrive_Workspace *p = (T_TS_DirectDrive_Workspace *)aW;
    return UEZSemaphoreCreateBinary(&p->iSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_DirectDrive_Open(void *aW)
{
    T_TS_DirectDrive_Workspace *p = (T_TS_DirectDrive_Workspace *)aW;
    p->aNumOpen++;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_DirectDrive_Close(void *aW)
{
    T_TS_DirectDrive_Workspace *p = (T_TS_DirectDrive_Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      The SPI bus and GPIO device are being declared.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_DirectDrive_Configure(
        T_uezDeviceWorkspace *aW,
        const TS_DirectDrive_Configuration *aConfig)
{
	T_uezError error;
	
    T_TS_DirectDrive_Workspace *p = (T_TS_DirectDrive_Workspace *)aW;
    p->iConfig = *aConfig;
	
	// Open up the ADC's
    error = UEZADCOpen(aConfig->iXL.iADCName, &p->iADC_XL);
    if (error)
        return error;
    error = UEZADCOpen(aConfig->iXR.iADCName, &p->iADC_XR);
    if (error)
        return error;
	error = UEZADCOpen(aConfig->iYU.iADCName, &p->iADC_YU);
    if (error)
        return error;
    error = UEZADCOpen(aConfig->iYD.iADCName, &p->iADC_YD);
    if (error)
        return error;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_ReadTouch
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the X location being touched (if any)
 * Inputs:
 *      T_TS_DirectDrive_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_DirectDrive_ReadTouch(
        T_TS_DirectDrive_Workspace *p,
        TBool *aValue)
{
    TInt16 compare;
	TUInt32 xl, xr, yu, yd;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

	TS_GPIO_CLEAR(iXDrive);
	TS_GPIO_OUTPUT_MODE(iXDrive);
	TS_GPIO_CLEAR(iYDrive);
	TS_GPIO_OUTPUT_MODE(iYDrive);

    //ad_dtc_acquire();
    UEZBSPDelayMS(1);
	
	G_TS_ADCRequest.iTrigger = ADC_TRIGGER_NOW;

	G_TS_ADCRequest.iADCChannel = p->iConfig.iXL.iADCChannel;
	G_TS_ADCRequest.iCapturedData = &xl;
	UEZADCRequestSingle(p->iADC_XL, &G_TS_ADCRequest);
	
	G_TS_ADCRequest.iADCChannel = p->iConfig.iXR.iADCChannel;
	G_TS_ADCRequest.iCapturedData = &xr;
	UEZADCRequestSingle(p->iADC_XR, &G_TS_ADCRequest);
	
	G_TS_ADCRequest.iADCChannel = p->iConfig.iYU.iADCChannel;
	G_TS_ADCRequest.iCapturedData = &yu;
	UEZADCRequestSingle(p->iADC_YU, &G_TS_ADCRequest);
	
	G_TS_ADCRequest.iADCChannel = p->iConfig.iYD.iADCChannel;
	G_TS_ADCRequest.iCapturedData = &yd;
	UEZADCRequestSingle(p->iADC_YD, &G_TS_ADCRequest);
	
	compare = (TInt16)xl + (TInt16)xr;
	compare -= (TInt16)yu + (TInt16)yd;
	compare = (compare >= 0)? compare:-compare;
	
	if (compare > COMPARE_LIMIT)
        *aValue = EFalse;
    else
        *aValue = ETrue;

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_ReadX
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the X location being touched (if any)
 * Inputs:
 *      T_TS_DirectDrive_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_DirectDrive_ReadY(
        T_TS_DirectDrive_Workspace *p,
        TUInt32 *aValue)
{
	TUInt32 xl, xr;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // read axis values...even if not touched to keep timing consistant
    // Assert Y axis drivers
	TS_GPIO_SET(iYDrive);

    // Wait for inputs to settle
    UEZBSPDelayMS(1);

    // Read Y axis inputs
	G_TS_ADCRequest.iADCChannel = p->iConfig.iXL.iADCChannel;
	G_TS_ADCRequest.iCapturedData = &xl;
	UEZADCRequestSingle(p->iADC_XL, &G_TS_ADCRequest);
	
	G_TS_ADCRequest.iADCChannel = p->iConfig.iXR.iADCChannel;
	G_TS_ADCRequest.iCapturedData = &xr;
	UEZADCRequestSingle(p->iADC_XR, &G_TS_ADCRequest);
	
	UEZBSPDelayMS(1);
	
	*aValue = (TInt16)xl + (TInt16)xr;
	
    // de-assert Y axis drivers
    TS_GPIO_CLEAR(iYDrive);
      
    UEZBSPDelayMS(1);

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_ReadY
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the Y location being touched (if any)
 * Inputs:
 *      T_TS_DirectDrive_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_DirectDrive_ReadX(
        T_TS_DirectDrive_Workspace *p,
        TUInt32 *aValue)
{
	TUInt32 yu, yd;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Assert X axis drivers
    TS_GPIO_SET(iXDrive);

    // Wait for inputs to settle
    UEZBSPDelayMS(1);

    // Read X axis input
	G_TS_ADCRequest.iADCChannel = p->iConfig.iYU.iADCChannel;
	G_TS_ADCRequest.iCapturedData = &yu;
	UEZADCRequestSingle(p->iADC_YU, &G_TS_ADCRequest);
	
	G_TS_ADCRequest.iADCChannel = p->iConfig.iYD.iADCChannel;
	G_TS_ADCRequest.iCapturedData = &yd;
	UEZADCRequestSingle(p->iADC_YD, &G_TS_ADCRequest);
	
	UEZBSPDelayMS(1);
	
	*aValue = (TInt16)yu + (TInt16)yd;

    // de-assert X axis drivers
    TS_GPIO_CLEAR(iXDrive);

    // Wait for inputs to settle
    UEZTaskDelay(1);

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_ApplyCalibration
 *---------------------------------------------------------------------------*
 * Description:
 *      Given the x, y raw coordinates, calculate the proper x, y
 *      calibrated coordinates.
 * Inputs:
 *      T_TS_DirectDrive_Workspace *p    -- Workspace
 *      TUInt32 x, y                -- Raw x, y values
 *      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void TS_DirectDrive_ApplyCalibration(
        T_TS_DirectDrive_Workspace *p,
        TUInt32 x,
        TUInt32 y,
        TUInt32 *newX,
        TUInt32 *newY)
{
    TInt32 v;

    v = x - p->iPenOffsetX;
    v *= p->iPenXScaleTop;
    v /= p->iPenXScaleBottom;
    v += p->iPenBaseX;
    *newX = (TUInt32)v;

    v = y - p->iPenOffsetY;
    v *= p->iPenYScaleTop;
    v /= p->iPenYScaleBottom;
    v += p->iPenBaseY;
    *newY = (TUInt32)v;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_DirectDrive_Poll(
        void *aWorkspace,
        T_uezTSReading *aReading)
{
    T_TS_DirectDrive_Workspace *p = (T_TS_DirectDrive_Workspace *)aWorkspace;
    T_uezError error;
    TUInt32 x, y;
    TBool isTouch, isTouch2;

    error = TS_DirectDrive_ReadTouch(p, &isTouch);
    if (error)
        return error;

    error = TS_DirectDrive_ReadX(p, (TUInt32 *)&aReading->iX);
    if (error)
        return error;

    error = TS_DirectDrive_ReadY(p, (TUInt32 *)&aReading->iY);
    if (error)
        return error;

    // Make sure we are still touching
    error = TS_DirectDrive_ReadTouch(p, &isTouch2);
    if (error)
        return error;

    x = aReading->iX;
    y = aReading->iY;

    if (!isTouch2)
        isTouch = EFalse;

    if (isTouch) {
        aReading->iFlags = TSFLAG_PEN_DOWN;

        if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {
            // Convert X & Y coordinates
            TS_DirectDrive_ApplyCalibration(p, x, y, (TUInt32 *)&aReading->iX, (TUInt32 *)&aReading->iY);
        }
    } else {
        // Pen is not down
        aReading->iFlags = 0;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_DirectDrive_CalibrateStart(void *aWorkspace)
{
    T_TS_DirectDrive_Workspace *p = (T_TS_DirectDrive_Workspace *)aWorkspace;

    p->iIsCalibrating = ETrue;
    p->iNumCalibratePoints = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_CalibrateEnd
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
static T_uezError TS_DirectDrive_CalibrateEnd(void *aWorkspace)
{
    TUInt32 minIX, maxIX, minOX, maxOX;
    TUInt32 minIY, maxIY, minOY, maxOY;
    TUInt32 i;

    T_TS_DirectDrive_Workspace *p = (T_TS_DirectDrive_Workspace *)aWorkspace;

    p->iIsCalibrating = EFalse;

    if (p->iNumCalibratePoints == NUM_CALIBRATE_POINTS_NEEDED)  {
        // Do the calibration algorithm here
        minIX = minOX = minIY = minOY = 0xFFFFFFFF;
        maxIX = maxOX = maxIY = maxOY = 0;

        for (i=0; i<p->iNumCalibratePoints; i++)  {

            // Find range of inputs
            if (p->iCalibrateReadingsTaken[i].iX < minIX) {
                minIX = p->iCalibrateReadingsTaken[i].iX;
                minOX = p->iCalibrateReadingsExpected[i].iX;
            }
            if (p->iCalibrateReadingsTaken[i].iX > maxIX) {
                maxIX = p->iCalibrateReadingsTaken[i].iX;
                maxOX = p->iCalibrateReadingsExpected[i].iX;
            }
            if (p->iCalibrateReadingsTaken[i].iY < minIY) {
                minIY = p->iCalibrateReadingsTaken[i].iY;
                minOY = p->iCalibrateReadingsExpected[i].iY;
            }
            if (p->iCalibrateReadingsTaken[i].iY > maxIY) {
                maxIY = p->iCalibrateReadingsTaken[i].iY;
                maxOY = p->iCalibrateReadingsExpected[i].iY;
            }
        }

        // Find scale factor of X
        p->iPenOffsetX = minIX;
        p->iPenBaseX = minOX;
        p->iPenXScaleTop = maxOX - minOX;
        p->iPenXScaleBottom = maxIX - minIX;

        // Find scale factor of Y
        p->iPenOffsetY = minIY;
        p->iPenBaseY = minOY;
        p->iPenYScaleTop = maxOY - minOY;
        p->iPenYScaleBottom = maxIY - minIY;

        p->iHaveCalibration = ETrue;

    } else {
        return UEZ_ERROR_NOT_ENOUGH_DATA;
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_CalibrateAdd
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
static T_uezError TS_DirectDrive_CalibrateAdd(
        void *aWorkspace,
        const T_uezTSReading *aReadingTaken,
        const T_uezTSReading *aReadingExpected)
{
    T_TS_DirectDrive_Workspace *p = (T_TS_DirectDrive_Workspace *)aWorkspace;
    TUInt32 num;

    num = p->iNumCalibratePoints;
    if (num < NUM_CALIBRATE_POINTS_NEEDED)  {
        p->iCalibrateReadingsTaken[num] = *aReadingTaken;
        p->iCalibrateReadingsExpected[num] = *aReadingExpected;
        p->iNumCalibratePoints++;
    } else {
        return UEZ_ERROR_OVERFLOW;
    }

    // Report an error if there is not enough data
    if (p->iNumCalibratePoints < NUM_CALIBRATE_POINTS_NEEDED)
        return UEZ_ERROR_NOT_ENOUGH_DATA;

    // Got enough, good.
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_DirectDrive_SetTouchDetectSensitivity
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
static T_uezError TS_DirectDrive_SetTouchDetectSensitivity(
        void *aWorkspace,
        TUInt16 aLowLevel,
        TUInt16 aHighLevel)
{
    T_TS_DirectDrive_Workspace *p = (T_TS_DirectDrive_Workspace *)aWorkspace;

    //UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    // Shift down the sensitivity to be a 10 bit reading
    // then multiply by the number of readings.
    p->iTDSLow = (aLowLevel>>6)*NUM_SAMPLES_PER_READING;
    p->iTDSHigh = (aHighLevel>>6)*NUM_SAMPLES_PER_READING;
    //UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

void Touchscreen_DirectDrive_Create(
        const char *aName,
        const TS_DirectDrive_Configuration *aConfig)
{
    T_uezDeviceWorkspace *p_ts;

    // Need to register touchscreen device
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Touchscreen_DirectDrive_Interface,
            0, &p_ts);

    // Configure it based on the given settings
    TS_DirectDrive_Configure(
            p_ts,
            aConfig);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_DirectDrive_Interface = {
    {
        // Common interface
        "Touchscreen:Generic:4Wire",
        0x0105,
        TS_DirectDrive_InitializeWorkspace,
        sizeof(T_TS_DirectDrive_Workspace),
    },

    // Functions
    TS_DirectDrive_Open,
    TS_DirectDrive_Close,
    TS_DirectDrive_Poll,

    TS_DirectDrive_CalibrateStart,
    TS_DirectDrive_CalibrateAdd,
    TS_DirectDrive_CalibrateEnd,

    TS_DirectDrive_SetTouchDetectSensitivity,
};

/*-------------------------------------------------------------------------*
 * End of File:  DirectDrive_TS.c
 *-------------------------------------------------------------------------*/
