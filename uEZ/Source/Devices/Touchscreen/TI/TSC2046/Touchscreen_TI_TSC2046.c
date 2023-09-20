/*-------------------------------------------------------------------------*
 * File:  tsc2406_main.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Texas Instruments TSC2046 Touchscreen
 *      Interface Chip.
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
#include <uEZGPIO.h>
#include <HAL/HAL.h>
#include "Touchscreen_TI_TSC2046.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define NUM_CALIBRATE_POINTS_NEEDED     3

// Command bits:
#define	ADS_START		    (1 << 7)
#define	ADS_A2A1A0_d_y		(1 << 4)	/* differential */
#define	ADS_A2A1A0_d_z1		(3 << 4)	/* differential */
#define	ADS_A2A1A0_d_z2		(4 << 4)	/* differential */
#define	ADS_A2A1A0_d_x		(5 << 4)	/* differential */
#define	ADS_A2A1A0_temp0	(0 << 4)	/* non-differential */
#define	ADS_A2A1A0_vbatt	(2 << 4)	/* non-differential */
#define	ADS_A2A1A0_vaux		(6 << 4)	/* non-differential */
#define	ADS_A2A1A0_temp1	(7 << 4)	/* non-differential */
#define	ADS_8_BIT		    (1 << 3)
#define	ADS_12_BIT		    (0 << 3)
#define	ADS_SER			    (1 << 2)	/* non-differential */
#define	ADS_DFR			    (0 << 2)	/* differential */
#define	ADS_PD10_PDOWN		(0 << 0)	/* lowpower mode + penirq */
#define	ADS_PD10_ADC_ON		(1 << 0)	/* ADC on */
#define	ADS_PD10_REF_ON		(2 << 0)	/* vREF on + penirq */
#define	ADS_PD10_ALL_ON		(3 << 0)	/* ADC + vREF on */

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_TOUCHSCREEN *iDevice;
    int32_t aNumOpen;
    DEVICE_SPI_BUS **iSPI;
    HAL_GPIOPort **iGPIO;
    TUInt32 iGPIOBit;
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
} T_TI_TSC2046Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern const DEVICE_TOUCHSCREEN TS_TI_TSC2046_Interface;

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup workspace for the TI TSC2046.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_TI_TSC2046_InitializeWorkspace(void *aW)
{
    T_TI_TSC2046Workspace *p = (T_TI_TSC2046Workspace *)aW;
    p->aNumOpen = 0;
    p->iHaveCalibration = EFalse;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being opened.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_TI_TSC2046_Open(void *aW)
{
    T_TI_TSC2046Workspace *p = (T_TI_TSC2046Workspace *)aW;
    p->aNumOpen++;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      The TI TSC2046 is being closed.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_TI_TSC2046_Close(void *aW)
{
    T_TI_TSC2046Workspace *p = (T_TI_TSC2046Workspace *)aW;
    p->aNumOpen--;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      The SPI bus and GPIO device are being declared.
 * Inputs:
 *      void *aW                    -- Particular SPI workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_TI_TSC2046_Configure(
                T_uezDeviceWorkspace *aW,
                DEVICE_SPI_BUS **aSPIBus,
                HAL_GPIOPort **aGPIO,
                TUInt32 aGPIOBit)
{
    T_TI_TSC2046Workspace *p = (T_TI_TSC2046Workspace *)aW;
    p->iSPI = aSPIBus;
    p->iGPIO = aGPIO;
    p->iGPIOBit = aGPIOBit;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_ReadX
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the X location being touched (if any)
 * Inputs:
 *      T_TI_TSC2046Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_TI_TSC2046_ReadX(T_TI_TSC2046Workspace *p, TUInt32 *aValue)
{
    TUInt8 cmd[3] = { ADS_START|ADS_A2A1A0_d_x|ADS_12_BIT|ADS_DFR|0, 0, 0 };
    SPI_Request tsc2046SPI = {
        0,
        0,

        3, // num transfers
        8, // num bits
        1000, // kHz = 1.0 MHz

        EFalse, // polarity
        EFalse, // phase

        0,

        0,    // cs bit
        EFalse,     // chip select is falling
    } ;
    T_uezError error;

    tsc2046SPI.iDataMOSI = cmd;
    tsc2046SPI.iDataMISO = cmd;
    tsc2046SPI.iCSGPIOPort = p->iGPIO;
    tsc2046SPI.iCSGPIOBit = p->iGPIOBit;
    error = (*p->iSPI)->TransferPolled(p->iSPI, &tsc2046SPI);

    *aValue = (cmd[1]<<8)|cmd[2];

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_ReadY
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the Y location being touched (if any)
 * Inputs:
 *      T_TI_TSC2046Workspace *p    -- Workspace
 *      TUInt32 *aValue             -- Pointer to value read
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError TS_TI_TSC2046_ReadY(T_TI_TSC2046Workspace *p, TUInt32 *aValue)
{
    TUInt8 cmd[3] = { ADS_START|ADS_A2A1A0_d_y|ADS_12_BIT|ADS_DFR|0, 0, 0 };
    SPI_Request tsc2046SPI = {
        0,
        0,

        3, // num transfers
        8, // num bits
        1000, // kHz = 1.0 MHz

        EFalse, // polarity
        EFalse, // phase

        0,

        0,    // cs bit
        EFalse,     // chip select is falling
    } ;
    T_uezError error;

    tsc2046SPI.iDataMOSI = cmd;
    tsc2046SPI.iDataMISO = cmd;
    tsc2046SPI.iCSGPIOPort = p->iGPIO;
    tsc2046SPI.iCSGPIOBit = p->iGPIOBit;
    error = (*p->iSPI)->TransferPolled(p->iSPI, &tsc2046SPI);

    *aValue = 0x7FFF - ((cmd[1]<<8)|cmd[2]);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_ApplyCalibration
 *---------------------------------------------------------------------------*
 * Description:
 *      Given the x, y raw coordinates, calculate the proper x, y
 *      calibrated coordinates.
 * Inputs:
 *      T_TI_TSC2046Workspace *p    -- Workspace
 *      TUInt32 x, y                -- Raw x, y values
 *      TUInt32 *newX, *newY        -- Pointer to calibrated x, y values
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static void TS_TI_TSC2046_ApplyCalibration(
                    T_TI_TSC2046Workspace *p,
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
#if 0
    if (v < 0)
        v = 0;
    if (v >= DISPLAY_WIDTH)
        v = DISPLAY_WIDTH-1;
#endif
    *newX = (TUInt32)v;

    v = y - p->iPenOffsetY;
    v *= p->iPenYScaleTop;
    v /= p->iPenYScaleBottom;
    v += p->iPenBaseY;
#if 0
    if (v < 0)
        v = 0;
    if (v >= DISPLAY_HEIGHT)
        v = DISPLAY_HEIGHT-1;
#endif
    *newY = (TUInt32)v;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_Poll
 *---------------------------------------------------------------------------*
 * Description:
 *      Take a reading from the TSC2406 and put in reading structure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTSReading *aReading     -- Pointer to final reading
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_TI_TSC2046_Poll(void *aWorkspace, T_uezTSReading *aReading)
{
    T_TI_TSC2046Workspace *p = (T_TI_TSC2046Workspace *)aWorkspace;
    T_uezError error;
    TUInt32 x, y;

    error = TS_TI_TSC2046_ReadX(p, (TUInt32 *)&aReading->iX);
    if (error)
        return error;

    error = TS_TI_TSC2046_ReadY(p, (TUInt32 *)&aReading->iY);
    if (error)
        return error;

    x = aReading->iX;
    y = aReading->iY;

    // TBD: For now, just look at readings to determine if pen is touching
    if ((x != 0) && (y != 0))  {
        aReading->iFlags = TSFLAG_PEN_DOWN;

        if ((!p->iIsCalibrating) && (p->iHaveCalibration)) {
            // Convert X & Y coordinates
            TS_TI_TSC2046_ApplyCalibration(p, x, y, (TUInt32 *)&aReading->iX, (TUInt32 *)&aReading->iY);
        }
    } else {
        // Pen is not down
        aReading->iFlags = 0;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_CalibrateStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Start calibration mode.  All readings will now report raw data.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_TI_TSC2046_CalibrateStart(void *aWorkspace)
{
    T_TI_TSC2046Workspace *p = (T_TI_TSC2046Workspace *)aWorkspace;

    p->iIsCalibrating = ETrue;
    p->iNumCalibratePoints = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_CalibrateEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      Calibration is complete.  Compute the calibration matrix and use.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError TS_TI_TSC2046_CalibrateEnd(void *aWorkspace)
{
    TUInt32 minIX, maxIX, minOX, maxOX;
    TUInt32 minIY, maxIY, minOY, maxOY;
    TUInt32 i;

    T_TI_TSC2046Workspace *p = (T_TI_TSC2046Workspace *)aWorkspace;

    p->iIsCalibrating = EFalse;

    if (p->iNumCalibratePoints == NUM_CALIBRATE_POINTS_NEEDED)  {
        // Do the calibration algorithm here
        minIX = minOX = minIY = minOY = 0xFFFFFFFF;
        maxIX = maxOX = maxIY = maxOY = 0;

        for (i=0; i<p->iNumCalibratePoints; i++)  {
            // Find range of inputs
            if (p->iCalibrateReadingsTaken[i].iX < minIX)
                minIX = p->iCalibrateReadingsTaken[i].iX;
            if (p->iCalibrateReadingsTaken[i].iX > maxIX)
                maxIX = p->iCalibrateReadingsTaken[i].iX;
            if (p->iCalibrateReadingsTaken[i].iY < minIY)
                minIY = p->iCalibrateReadingsTaken[i].iY;
            if (p->iCalibrateReadingsTaken[i].iY > maxIY)
                maxIY = p->iCalibrateReadingsTaken[i].iY;

            // Find range of outputs
            if (p->iCalibrateReadingsExpected[i].iX < minOX)
                minOX = p->iCalibrateReadingsExpected[i].iX;
            if (p->iCalibrateReadingsExpected[i].iX > maxOX)
                maxOX = p->iCalibrateReadingsExpected[i].iX;
            if (p->iCalibrateReadingsExpected[i].iY < minOY)
                minOY = p->iCalibrateReadingsExpected[i].iY;
            if (p->iCalibrateReadingsExpected[i].iY > maxOY)
                maxOY = p->iCalibrateReadingsExpected[i].iY;
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
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  TS_TI_TSC2046_CalibrateAdd
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
T_uezError TS_TI_TSC2046_CalibrateAdd(
                    void *aWorkspace,
                    const T_uezTSReading *aReadingTaken,
                    const T_uezTSReading *aReadingExpected)
{
    T_TI_TSC2046Workspace *p = (T_TI_TSC2046Workspace *)aWorkspace;
    TUInt32 num;

    num = p->iNumCalibratePoints;
    if (num < NUM_CALIBRATE_POINTS_NEEDED)  {
        p->iCalibrateReadingsTaken[num] = *aReadingTaken;
        p->iCalibrateReadingsExpected[num] = *aReadingExpected;
        p->iNumCalibratePoints++;
    }

    // Report an error if there is not enough data
    if (p->iNumCalibratePoints < NUM_CALIBRATE_POINTS_NEEDED)
        return UEZ_ERROR_NOT_ENOUGH_DATA;

    // Got enough, good.
    return UEZ_ERROR_NONE;
}

T_uezError Touchscreen_TI_TSC2046_Create(
    const char *aName,
    const char *aSPIBusName,
    T_uezGPIOPortPin aChipSelect)
{
    T_uezDeviceWorkspace *p;
    T_uezDevice spi;
    DEVICE_SPI_BUS **p_spi;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Touchscreen_TI_TSC2046_Interface,
            0, &p);
    UEZDeviceTableFind(aSPIBusName, &spi);
    UEZDeviceTableGetWorkspace(spi, (T_uezDeviceWorkspace **)&p_spi);
    return TS_TI_TSC2046_Configure(p, p_spi, GPIO_TO_HAL_PORT(aChipSelect),
        GPIO_TO_PIN_BIT(aChipSelect));
}


/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_TOUCHSCREEN Touchscreen_TI_TSC2046_Interface = {
	{
	    // Common interface
	    "Touchscreen:TI:TSC2046",
	    0x0100,
	    TS_TI_TSC2046_InitializeWorkspace,
	    sizeof(T_TI_TSC2046Workspace),
	},
	
    // Functions
    TS_TI_TSC2046_Open,
    TS_TI_TSC2046_Close,
    TS_TI_TSC2046_Poll,

    TS_TI_TSC2046_CalibrateStart,
    TS_TI_TSC2046_CalibrateAdd,
    TS_TI_TSC2046_CalibrateEnd,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  tsc2406_main.c
 *-------------------------------------------------------------------------*/
