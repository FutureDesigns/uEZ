/*-------------------------------------------------------------------------*
 * File:  RTC_PCF8563.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a RTC that communicates over I2C
 *      to a standard PCF8563.
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "RTC_PCF8563.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define CLKOUT_control                  0x0D
#define     CLKOUT_FE                       (1<<7)
#define VL_seconds                      0X02

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_RTC *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
} T_RTC_PCF8563_Workspace;


/*---------------------------------------------------------------------------*
 * Routine:  RTC_PCF8563_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCF8563.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RTC_PCF8563_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_RTC_PCF8563_Workspace *p = (T_RTC_PCF8563_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IBCDToDecimal
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert 8-bit BCD value to decimal.
 * Inputs:
 *      TUInt8 aValue                -- 8-bit BCD value
 * Outputs:
 *      TUInt8                       -- Resulting non-BCD value
 *---------------------------------------------------------------------------*/
static TUInt8 IBCDToDecimal(TUInt8 aValue)
{
    return (aValue >> 4)*10+(aValue&0x0F);
}

/*---------------------------------------------------------------------------*
 * Routine:  IDecimalToBCD
 *---------------------------------------------------------------------------*
 * Description:
 *      Convert 8-bit decimal value to 8-bit BCD.
 * Inputs:
 *      TUInt8 aValue                -- 8-bit non-BCD value
 * Outputs:
 *      TUInt8                       -- Resulting BCD value
 *---------------------------------------------------------------------------*/
static TUInt8 IDecimalToBCD(TUInt8 aValue)
{
    return ((aValue/10)<<4)+(aValue % 10);
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_PCF8563_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current RTC clock reading.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RTC_PCF8563_Get(void *aWorkspace, T_uezTimeDate *aTimeDate)
{
    T_uezError error;
    T_RTC_PCF8563_Workspace *p = (T_RTC_PCF8563_Workspace *)aWorkspace;
    I2C_Request r;
    TUInt8 data[16];
    TUInt8 reg0[1] = {0x00};

    r.iAddr = PCF8563_I2C_ADDR;
    r.iSpeed = PCF8563_I2C_SPEED;
    r.iWriteData = reg0;
    r.iWriteLength = 1;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = data;
    r.iReadLength = 16;
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;  // wait until bus ready

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
    if (!error) {
        aTimeDate->iTime.iSecond = IBCDToDecimal(data[2] & 0x7F);
        aTimeDate->iTime.iMinute = IBCDToDecimal(data[3] & 0x7F);
        aTimeDate->iTime.iHour = IBCDToDecimal(data[4] & 0x3F);
        aTimeDate->iDate.iMonth = IBCDToDecimal(data[7] & 0x1F);
        aTimeDate->iDate.iDay = IBCDToDecimal(data[5] & 0x3F);
        aTimeDate->iDate.iYear = 2000+IBCDToDecimal(data[8] & 0xFF);
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  RTC_PCF8563_ReadReg
 *---------------------------------------------------------------------------*
 * Description:
 *      Read a RTC clock register
 * Inputs:
 *      T_RTC_PCF8563_Workspace *p -- Workspace
 *      TUInt8 aReg                 -- Register to write
 *      TUInt8 aValue               -- Value to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_PCF8563_ReadReg(
        T_RTC_PCF8563_Workspace *p,
        TUInt8 aReg,
        TUInt8 *aValue)
{
    I2C_Request r;

    // Read the whole PCF8563's registers
    r.iAddr = PCF8563_I2C_ADDR;
    r.iSpeed = PCF8563_I2C_SPEED;
    r.iWriteData = &aReg;
    r.iWriteLength = 1;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = aValue;
    r.iReadLength = 1;
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;  // wait until bus ready

    // Do the read
    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  RTC_PCF8563_WriteReg
 *---------------------------------------------------------------------------*
 * Description:
 *      Write a RTC clock register
 * Inputs:
 *      T_RTC_PCF8563_Workspace *p -- Workspace
 *      TUInt8 aReg                 -- Register to write
 *      TUInt8 aValue               -- Value to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_PCF8563_WriteReg(
        T_RTC_PCF8563_Workspace *p,
        TUInt8 aReg,
        TUInt8 aValue)
{
    I2C_Request r;
    TUInt8 cmd[2];

    cmd[0] = aReg;
    cmd[1] = aValue;

    // Read the whole PCF8563's registers
    r.iAddr = PCF8563_I2C_ADDR;
    r.iSpeed = PCF8563_I2C_SPEED;
    r.iWriteData = cmd;
    r.iWriteLength = 2;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;
    r.iReadData = 0;
    r.iReadLength = 0;
    r.iReadTimeout = 0;

    // Do the write
    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  RTC_PCF8563_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current RTC clock (without regard to semaphoring actions)
 * Inputs:
 *      T_RTC_PCF8563_Workspace *p -- Workspace
 *      const T_uezTimeDate *aTimeDate -- Time and date to set to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_PCF8563_LowLevelSet(
        T_RTC_PCF8563_Workspace *p,
        const T_uezTimeDate *aTimeDate)
{
    I2C_Request r;
    TUInt8 dataout[17];
    TUInt8 *data = dataout+1;

    dataout[0] = 0x00;

    // Setup structure
    r.iAddr = PCF8563_I2C_ADDR;
    r.iSpeed = PCF8563_I2C_SPEED;

    r.iWriteData = dataout;   // Write out the new configuration
    r.iWriteLength = 10;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;  // wait until bus ready

    r.iReadData = 0;    // no reading, just writing
    r.iReadLength = 0;
    r.iReadTimeout = 0;

    // Setup data to go out
    data[0] = 0x00; // controls with it running
    data[1] = 0x00; // reset all alarms
    data[2] = IDecimalToBCD(aTimeDate->iTime.iSecond);
    data[3] = IDecimalToBCD(aTimeDate->iTime.iMinute);
    data[4] = IDecimalToBCD(aTimeDate->iTime.iHour);
    data[5] = IDecimalToBCD(aTimeDate->iDate.iDay);
    data[6] = 0; // day of week not used
    data[7] = IDecimalToBCD(aTimeDate->iDate.iMonth);
    data[8] = IDecimalToBCD(aTimeDate->iDate.iYear-2000);

    // Do the I2C
    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_PCF8563_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current RTC clock.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const T_uezTimeDate *aTimeDate -- Time and date to set to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RTC_PCF8563_Set(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
    T_RTC_PCF8563_Workspace *p = (T_RTC_PCF8563_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    error = RTC_PCF8563_LowLevelSet(p, aTimeDate);
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_PCF8563_Validate
 *---------------------------------------------------------------------------*
 * Description:
 *      Validate the current RTC date and time.  If the date or time is
 *      invalid, reset to the given time and date.  If no date and time is
 *      given, just report that the RTC is invalid by returning
 *      UEZ_ERROR_INVALID.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const T_uezTimeDate *aTimeDate -- Time and date to set to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RTC_PCF8563_Validate(
        void *aWorkspace,
        const T_uezTimeDate *aTimeDate)
{
    T_RTC_PCF8563_Workspace *p = (T_RTC_PCF8563_Workspace *)aWorkspace;
    T_uezError error;
    T_uezError ret_error = UEZ_ERROR_NONE;
    TUInt8 vlsec;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Do the read
    error = RTC_PCF8563_ReadReg(p, VL_seconds, &vlsec);
    if (error) {
        // An error occurred in the communication to the part
        // Stop and return this error
        UEZSemaphoreRelease(p->iSem);
        return error;
    }

    // Is the data still got integrity?
    // (check high bit of VL_seconds)
    if (vlsec & 0x80) {
        // Integrity of data is bad
        ret_error = UEZ_ERROR_INVALID;

        // Was a new time/date setup passed in?
        if (aTimeDate) {
            // Set the time and date (without using semaphores since
            // we already have captured the semaphore)
            error = RTC_PCF8563_LowLevelSet(aWorkspace, aTimeDate);

            // Report any problems setting the time/date
            if (error) {
                UEZSemaphoreRelease(p->iSem);
                return error;
            }
        }
    }

    UEZSemaphoreRelease(p->iSem);

    // Return if we were invalid or not
    return ret_error;
}

T_uezError RTC_PCF8563_SetClockOutHz(
        void *aWorkspace,
        TUInt32 aHertz)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;
    T_RTC_PCF8563_Workspace *p = (T_RTC_PCF8563_Workspace *)aWorkspace;
    TUInt8 value;
    typedef struct { TUInt32 iHertz; TUInt8 iControl; } T_pcf8563_clockOut;
    const T_pcf8563_clockOut clockOutLookupTable[] = {
        { 32768, CLKOUT_FE|0 },
        { 1024, CLKOUT_FE|1 },
        { 32, CLKOUT_FE|2 },
        { 1, CLKOUT_FE|3 },
        { 0, 0 },
    };
    TUInt32 i;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Look in the above table and if found, change the value
    // otherwise, fall through and report NOT_SUPPORTED
    for (i=0; i<ARRAY_COUNT(clockOutLookupTable); i++) {
        if (clockOutLookupTable[i].iHertz == aHertz) {
            error = RTC_PCF8563_ReadReg(p, CLKOUT_control, &value);
            value &= ~0x83;
            if (error == UEZ_ERROR_NONE)
                error = RTC_PCF8563_WriteReg(p, CLKOUT_control, value|clockOutLookupTable[i].iControl);
            break;
        }
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_PCF8563_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the RTC's interface.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_I2C_BUS **aI2C       -- I2C interface
 *---------------------------------------------------------------------------*/
void RTC_PCF8563_Configure(
        void *aWorkspace,
        DEVICE_I2C_BUS **aI2C)
{
    T_RTC_PCF8563_Workspace *p = (T_RTC_PCF8563_Workspace *)aWorkspace;

    // Set the I2C bus
    p->iI2C = aI2C;
}

void RTC_PCF8563_Create(const char *aName, const char *aI2CBusName)
{
    T_uezDeviceWorkspace *p_rtcdev;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup with exteranl RTC
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&RTC_NXP_PCF8563_Interface,
            0,
            &p_rtcdev);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, &p_i2c);
    RTC_PCF8563_Configure(p_rtcdev, (DEVICE_I2C_BUS **)p_i2c);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_RTC RTC_NXP_PCF8563_Interface = {
	{
	    // Common device interface
	    "RTC:NXP:PCF8563",
	    0x0100,
	    RTC_PCF8563_InitializeWorkspace,
	    sizeof(T_RTC_PCF8563_Workspace),
	},
	
    // Functions
    RTC_PCF8563_Get,
    RTC_PCF8563_Set,
    RTC_PCF8563_Validate,
    RTC_PCF8563_SetClockOutHz,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  RTC_PCF8563.c
 *-------------------------------------------------------------------------*/
