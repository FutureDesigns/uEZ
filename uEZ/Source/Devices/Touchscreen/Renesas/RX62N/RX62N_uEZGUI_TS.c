/*-------------------------------------------------------------------------*
 * File:  RX62N_uEZGUI_TS.c
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
#include <Device/Touchscreen.h>
#include <HAL/HAL.h>
#include <HAL/GPIO.h>
#include <Types/ADC.h>
#include <Device/ADCBank.h>
#include "RX62N_uEZGUI_TS.h"
#include <uEZProcessor.h>
 
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

#define COMPARE_LIMIT (64)



#define size_ra(ra) (sizeof(ra)/sizeof(ra[0]))


/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_TOUCHSCREEN *iDevice;
    int32_t aNumOpen;

    // Hardware linkage
    RX62N_uEZGUI_TS_Configuration iConfig;

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
} T_RX62N_uEZGUI_TS_Workspace;

#pragma bit_order left
#pragma unpack
struct st_dtc_xfer_full
{
  union
  {
    struct
    {
      unsigned long MRA_MD    :2;
      unsigned long MRA_SZ    :2;
      unsigned long MRA_SM    :2;
      unsigned long           :2;
      unsigned long MRB_CHNE  :1;
      unsigned long MRB_CHNS  :1;
      unsigned long MRB_DISEL :1;
      unsigned long MRB_DTS   :1;
      unsigned long MRB_DM    :2;
      unsigned long           :2;
      unsigned long           :16;
    }BIT;
    unsigned long LONG;
  }MR;
  void * SAR;
  void * DAR;
  struct
  {
    unsigned long CRA:16;
    unsigned long CRB:16;
  }CR;
};

struct st_dtc_xfer_short
{
  union
  {
    struct
    {
      unsigned long MRA_MD  :2;
      unsigned long MRA_SZ  :2;
      unsigned long MRA_SM  :2;
      unsigned long         :2;
      unsigned long SAR     :24;
    }BIT;
    unsigned long LONG;
  }SAR_MRA;
  union
  {
    struct
    {
      unsigned long MRB_CHNE  :1;
      unsigned long MRB_CHNS  :1;
      unsigned long MRB_DISEL :1;
      unsigned long MRB_DTS   :1;
      unsigned long MRB_DM    :2;
      unsigned long           :2;
      unsigned long DAR       :24;
    }BIT;
    unsigned long LONG;
  }DAR_MRB;
  struct
  {
    unsigned long CRA:16;
    unsigned long CRB:16;
  }CR;
};

#pragma bit_order
#pragma packoption

struct st_dtc_table
{
  union
  {
    struct
    {
      unsigned char MD  :2;
      unsigned char SZ  :2;
      unsigned char SM  :2;
      unsigned char     :2;
    }BIT;
    unsigned char BYTE;
  }MRA;
  union
  {
    struct
    {
      unsigned char CHNE  :1;
      unsigned char CHNS  :1;
      unsigned char DISEL :1;
      unsigned char DTS   :1;
      unsigned char DM    :2;
      unsigned char       :2;
    }BIT;
    unsigned char BYTE;
  }MRB;
  unsigned short dummy;
  unsigned long SAR;
  unsigned long DAR;
  unsigned short CRA;
  unsigned short CRB;
};

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN RX62N_uEZGUI_TS_Interface;

#pragma section DTC_VECT
const unsigned long dtc_vectors = 0;    /* start of DTC vector table */
#pragma section
#pragma section DTC_TABLE
struct st_dtc_xfer_full ad_dtc_table;

#pragma section DTC_VECT_S12AD
const struct st_dtc_xfer_full *const ad_dtc_vector = &ad_dtc_table;

/* return to normal section definition */
#pragma section
TUInt16 AD_Buffer[8][8];         // allocate buffer for 8 samples on each A/D channel

void ad_dtc_init(void)
{
    memset(AD_Buffer, 0xCC, sizeof(AD_Buffer));
    
    /* configure A/D Converter */
    MSTP(S12AD) = 0;     /* enable A/D */
    S12AD.ADCSR.BIT.ADIE = 1; /* enable A/D to feed ICU */
    S12AD.ADCSR.BIT.ADCS = 1; /* continous scan */
    S12AD.ADCSR.BIT.CKS = 3; /* clock at pclk/1 */
    S12AD.ADANS.WORD = 0xFF;  /* enable sampling of all 8 channel */
    S12AD.ADADS.WORD = 0xFF;  /* enable multi-sample of all 8 channel */
    S12AD.ADADC.BYTE = 3;     /* configure for 4x addition */
    S12AD.ADCSR.BIT.ADST = 1; /* start conversions */

    ad_dtc_table.SAR = (void *)&S12AD.ADDRA;
    ad_dtc_table.DAR = AD_Buffer;
    ad_dtc_table.MR.LONG = 0;
    ad_dtc_table.MR.BIT.MRA_MD = 2;  // block transfer
    ad_dtc_table.MR.BIT.MRA_SZ = 1;  // long word transfer size (less bus cycles)
    ad_dtc_table.MR.BIT.MRA_SM = 2;  // increment source address
    ad_dtc_table.MR.BIT.MRB_DM = 2;  // increment destination address
    ad_dtc_table.MR.BIT.MRB_DTS = 1; // source side is block source
    ad_dtc_table.CR.CRA = 0x0808;            // 8 words (16 bytes) per transfer
    ad_dtc_table.CR.CRB = size_ra(AD_Buffer);  // number of block transfers

    S12AD.ADCSR.BIT.ADIE = 1;
    IEN(S12AD,ADI) = 1;
}

static void ad_dtc_acquire(void)
{
    ad_dtc_table.DAR = AD_Buffer;
    ad_dtc_table.CR.CRB = size_ra(AD_Buffer);  // number of block transfers
    IR(S12AD,ADI) = 0;
    DTCE(S12AD,ADI) = 1;
}

static TUInt16 ad_dtc_value(int32_t channel)
{
    int32_t index;
    TUInt32 value;

    if (channel >= size_ra(AD_Buffer[1]))
      return(0);

    /* sum values */
    for (index=0, value=0; index < size_ra(AD_Buffer); index++)
      value += AD_Buffer[index][channel];

    /* shift data */
    value /= 64;
    return ((TUInt16)value);
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_uEZGUI_TS_InitializeWorkspace(void *aW)
{
    T_RX62N_uEZGUI_TS_Workspace *p = (T_RX62N_uEZGUI_TS_Workspace *)aW;
    return UEZSemaphoreCreateBinary(&p->iSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_uEZGUI_TS_Open(void *aW)
{
    T_RX62N_uEZGUI_TS_Workspace *p = (T_RX62N_uEZGUI_TS_Workspace *)aW;
    p->aNumOpen++;

    if (p->aNumOpen == 1) {
			
		(*p->iConfig.iXDrive.iGPIO)->Clear(
			p->iConfig.iXDrive.iGPIO,
			p->iConfig.iXDrive.iGPIOBit);
			
		(*p->iConfig.iXDrive.iGPIO)->SetOutputMode(
			p->iConfig.iXDrive.iGPIO,
			p->iConfig.iXDrive.iGPIOBit);
			
		(*p->iConfig.iYDrive.iGPIO)->Clear(
			p->iConfig.iYDrive.iGPIO,
			p->iConfig.iYDrive.iGPIOBit);
			
		(*p->iConfig.iYDrive.iGPIO)->SetOutputMode(
			p->iConfig.iYDrive.iGPIO,
			p->iConfig.iYDrive.iGPIOBit);
		
        /*PINo(OUT_TS_XR) = 0;
        PINo(OUT_TS_XL) = 0;
        PINo(OUT_TS_YU) = 0;
        PINo(OUT_TS_YL) = 0;
        DDRo(OUT_TS_XL);
        DDRo(OUT_TS_XR);
        DDRo(OUT_TS_YU);
        DDRo(OUT_TS_YL);*/
		
       	ad_dtc_init();
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_uEZGUI_TS_Close(void *aW)
{
    T_RX62N_uEZGUI_TS_Workspace *p = (T_RX62N_uEZGUI_TS_Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      The SPI bus and GPIO device are being declared.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RX62N_uEZGUI_TS_Configure(
        T_uezDeviceWorkspace *aW,
        RX62N_uEZGUI_TS_Configuration *aConfig)
{
    T_RX62N_uEZGUI_TS_Workspace *p = (T_RX62N_uEZGUI_TS_Workspace *)aW;
    p->iConfig = *aConfig;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_ReadTouch
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the X location being touched (if any)
 * Inputs:
 *      T_RX62N_uEZGUI_TS_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_uEZGUI_TS_ReadTouch(
        T_RX62N_uEZGUI_TS_Workspace *p,
        TBool *aValue)
{
    TInt16 compare;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    ad_dtc_acquire();
    UEZTaskDelay(1);

    compare = (TInt16)ad_dtc_value(p->iConfig.iInputXL.iADCChannel) + (TInt16)ad_dtc_value(p->iConfig.iInputXR.iADCChannel);
    compare -= (TInt16)ad_dtc_value(p->iConfig.iInputYU.iADCChannel) + (TInt16)ad_dtc_value(p->iConfig.iInputYD.iADCChannel);
    compare = (compare >= 0)? compare:-compare;
    if (compare > COMPARE_LIMIT)
        *aValue = EFalse;
    else
        *aValue = ETrue;

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}


/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_ReadX
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the X location being touched (if any)
 * Inputs:
 *      T_RX62N_uEZGUI_TS_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_uEZGUI_TS_ReadY(
        T_RX62N_uEZGUI_TS_Workspace *p,
        TUInt32 *aValue)
{
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // read axis values...even if not touched to keep timing consistant
    // Assert Y axis drivers
    (*p->iConfig.iYDrive.iGPIO)->Set(
		p->iConfig.iYDrive.iGPIO,
		p->iConfig.iYDrive.iGPIOBit);

    // Wait for inputs to settle
    UEZTaskDelay(1);

    // Read Y axis inputs
    ad_dtc_acquire();
    UEZTaskDelay(1);
    *aValue = (TInt16)ad_dtc_value(p->iConfig.iInputXL.iADCChannel) + (TInt16)ad_dtc_value(p->iConfig.iInputXR.iADCChannel);

    // de-assert Y axis drivers
    (*p->iConfig.iYDrive.iGPIO)->Clear(
		p->iConfig.iYDrive.iGPIO,
		p->iConfig.iYDrive.iGPIOBit);
      
    UEZTaskDelay(1);

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_ReadY
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the Y location being touched (if any)
 * Inputs:
 *      T_RX62N_uEZGUI_TS_Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_uEZGUI_TS_ReadX(
        T_RX62N_uEZGUI_TS_Workspace *p,
        TUInt32 *aValue)
{
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Assert X axis drivers
    (*p->iConfig.iXDrive.iGPIO)->Set(
		p->iConfig.iXDrive.iGPIO,
		p->iConfig.iXDrive.iGPIOBit);

    // Wait for inputs to settle
    UEZTaskDelay(1);

    // Read X axis input
    ad_dtc_acquire();
    UEZTaskDelay(1);
    *aValue = (TInt16)ad_dtc_value(p->iConfig.iInputYU.iADCChannel) + (TInt16)ad_dtc_value(p->iConfig.iInputYD.iADCChannel);

    // de-assert X axis drivers
    (*p->iConfig.iXDrive.iGPIO)->Clear(
		p->iConfig.iXDrive.iGPIO,
		p->iConfig.iXDrive.iGPIOBit);

    // Wait for inputs to settle
    UEZTaskDelay(1);

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_ApplyCalibration
 *---------------------------------------------------------------------------*
 * Description:
 *      Given the x, y raw coordinates, calculate the proper x, y
 *      calibrated coordinates.
 * Inputs:
 *      T_RX62N_uEZGUI_TS_Workspace *p    -- Workspace
 *      TUInt32 x, y                -- Raw x, y values
 *      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void RX62N_uEZGUI_TS_ApplyCalibration(
        T_RX62N_uEZGUI_TS_Workspace *p,
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
 * Routine:  RX62N_uEZGUI_TS_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_uEZGUI_TS_Poll(
        void *aWorkspace,
        T_uezTSReading *aReading)
{
    T_RX62N_uEZGUI_TS_Workspace *p = (T_RX62N_uEZGUI_TS_Workspace *)aWorkspace;
    T_uezError error;
    TUInt32 x, y;
    TBool isTouch, isTouch2;

    error = RX62N_uEZGUI_TS_ReadTouch(p, &isTouch);
    if (error)
        return error;

    error = RX62N_uEZGUI_TS_ReadX(p, (TUInt32 *)&aReading->iX);
    if (error)
        return error;

    error = RX62N_uEZGUI_TS_ReadY(p, (TUInt32 *)&aReading->iY);
    if (error)
        return error;

    // Make sure we are still touching
    error = RX62N_uEZGUI_TS_ReadTouch(p, &isTouch2);
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
            RX62N_uEZGUI_TS_ApplyCalibration(p, x, y, (TUInt32 *)&aReading->iX, (TUInt32 *)&aReading->iY);
        }
    } else {
        // Pen is not down
        aReading->iFlags = 0;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RX62N_uEZGUI_TS_CalibrateStart(void *aWorkspace)
{
    T_RX62N_uEZGUI_TS_Workspace *p = (T_RX62N_uEZGUI_TS_Workspace *)aWorkspace;

    p->iIsCalibrating = ETrue;
    p->iNumCalibratePoints = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  RX62N_uEZGUI_TS_CalibrateEnd
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
static T_uezError RX62N_uEZGUI_TS_CalibrateEnd(void *aWorkspace)
{
    TUInt32 minIX, maxIX, minOX, maxOX;
    TUInt32 minIY, maxIY, minOY, maxOY;
    TUInt32 i;

    T_RX62N_uEZGUI_TS_Workspace *p = (T_RX62N_uEZGUI_TS_Workspace *)aWorkspace;

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
 * Routine:  RX62N_uEZGUI_TS_CalibrateAdd
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
static T_uezError RX62N_uEZGUI_TS_CalibrateAdd(
        void *aWorkspace,
        const T_uezTSReading *aReadingTaken,
        const T_uezTSReading *aReadingExpected)
{
    T_RX62N_uEZGUI_TS_Workspace *p = (T_RX62N_uEZGUI_TS_Workspace *)aWorkspace;
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
 * Routine:  RX62N_uEZGUI_TS_SetTouchDetectSensitivity
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
static T_uezError RX62N_uEZGUI_TS_SetTouchDetectSensitivity(
        void *aWorkspace,
        TUInt16 aLowLevel,
        TUInt16 aHighLevel)
{
    T_RX62N_uEZGUI_TS_Workspace *p = (T_RX62N_uEZGUI_TS_Workspace *)aWorkspace;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    // Shift down the sensitivity to be a 10 bit reading
    // then multiply by the number of readings.
    p->iTDSLow = (aLowLevel>>6)*NUM_SAMPLES_PER_READING;
    p->iTDSHigh = (aHighLevel>>6)*NUM_SAMPLES_PER_READING;
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_DirectDrive_Interface = {
    {
        // Common interface
        "Touchscreen:Generic:4Wire",
        0x0105,
        RX62N_uEZGUI_TS_InitializeWorkspace,
        sizeof(T_RX62N_uEZGUI_TS_Workspace),
    },

    // Functions
    RX62N_uEZGUI_TS_Open,
    RX62N_uEZGUI_TS_Close,
    RX62N_uEZGUI_TS_Poll,

    RX62N_uEZGUI_TS_CalibrateStart,
    RX62N_uEZGUI_TS_CalibrateAdd,
    RX62N_uEZGUI_TS_CalibrateEnd,

    RX62N_uEZGUI_TS_SetTouchDetectSensitivity,
};

/*-------------------------------------------------------------------------*
 * End of File:  RX62N_uEZGUI_TS.c
 *-------------------------------------------------------------------------*/
