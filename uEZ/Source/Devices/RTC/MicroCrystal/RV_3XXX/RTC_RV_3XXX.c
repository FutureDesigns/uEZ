/*-------------------------------------------------------------------------*
 * File:  RTC_RV_3XXX.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of a RTC that communicates over I2C
 *      to a standard RV_3XXX.
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
#include "RTC_RV_3XXX.h"

/* Note: This currently targets the RV-3028-C7 but the basic registers should be common across several other parts.
 * Note: This part requires register configuration to allow the battery pin to be used for VBATT. It comes disabled. 
 *  For a case where VDD is 3.3V, and VBAT is 3.0-3.3V, Level Switching Mode (LSM) should be used.
 * LSM will switch from VDD to VBATT if VDD<2V and VBATT>2V.  */
/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define RV_REG_SEC                      0x00 // BCD 0-59
#define RV_REG_MIN                      0x01 // BCD 0-59
#define RV_REG_HR                       0x02 // BCD, if 12_24==0 then 24 hour mode (default), otherwise PM=1 (0-23, or 1-12)
#define RV_REG_WEEKDAY                  0x03 // Can be any day, just counts 0-6
#define RV_REG_DATE                     0x04 // Day of month, BCD 1-31
#define RV_REG_MON                      0x05 // BCD 1-12
#define RV_REG_YEAR                     0x06 // BCD 0-99

#define AM_PM_BIT                       (1<<5) // PM bit in RV_REG_HR register (not used yet)

#define RV_REG_ID                       0x28   // Upper 4 bits=HW ID, lower 4 bits=Version ID

#define RV_REG_STATUS                   0x0E   // Status register
#define PORF_BIT                       (1<<0)  // power on reset detect flag
#define BSF_BIT                        (1<<5)  // backup switchover detect flag (did we switch to VBATT?)
#define RV_REG_CONTROL                  0x0F   // Timer controls
// control 1 bits TODO (timer functions)
#define RV_REG_CONTROL2                 0x10   // controls AM/PM, clock output, timestamp modes
// control 2 bits
#define CLKIE_BIT                      (1<<6)  // clock output, which is enabled by default to 32,768Hz
#define M12_24_BIT                     (1<<1)  // change from 24 hour to am/pm mode (not supported yet)
#define RESET_BIT                      (1<<0)  // software reset

#define RV_REG_EEPROM_CLKOUT            0x35   // Bit 0 to 2 is FD bits for clock out frequency

#define RV_REG_EEPROM_BACKUP            0x37   // backup/misc settings
#define BSM_BITS                       (3<<2)  // Level Switching Mode

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_RTC *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
} T_RTC_RV_3XXX_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  RTC_RV_3XXX_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for RV_3XXX.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RTC_RV_3XXX_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_RTC_RV_3XXX_Workspace *p = (T_RTC_RV_3XXX_Workspace *)aW;

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
 * Routine:  RTC_RV_3XXX_Get
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current RTC clock reading.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RTC_RV_3XXX_Get(void *aWorkspace, T_uezTimeDate *aTimeDate)
{
    T_uezError error;
    T_RTC_RV_3XXX_Workspace *p = (T_RTC_RV_3XXX_Workspace *)aWorkspace;
    I2C_Request r;
    TUInt8 data[7];
    TUInt8 reg0[1] = {RV_REG_SEC}; // address 0

    r.iAddr = RV_3XXX_I2C_ADDR;
    r.iSpeed = RV_3XXX_I2C_SPEED;
    r.iWriteData = reg0;
    r.iWriteLength = 1;
    r.iWriteTimeout = 5000; //UEZ_TIMEOUT_INFINITE;
    r.iReadData = data;
    r.iReadLength = 7;
    r.iReadTimeout = 5000; //UEZ_TIMEOUT_INFINITE;  // wait until bus ready

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
    if (!error) {
        aTimeDate->iTime.iSecond = IBCDToDecimal(data[0] & 0x7F);
        aTimeDate->iTime.iMinute = IBCDToDecimal(data[1] & 0x7F);
        aTimeDate->iTime.iHour = IBCDToDecimal(data[2] & 0x3F);
        aTimeDate->iDate.iDay = IBCDToDecimal(data[4] & 0x3F);
        aTimeDate->iDate.iMonth = IBCDToDecimal(data[5] & 0x1F);
        aTimeDate->iDate.iYear = 2000+IBCDToDecimal(data[6] & 0xFF);
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  RTC_RV_3XXX_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current RTC clock (without regard to semaphoring actions)
 * Inputs:
 *      T_RTC_RV_3XXX_Workspace *p -- Workspace
 *      const T_uezTimeDate *aTimeDate -- Time and date to set to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_RV_3XXX_LowLevelSet(
        T_RTC_RV_3XXX_Workspace *p,
        const T_uezTimeDate *aTimeDate)
{
    I2C_Request r;
    TUInt8 dataout[8];
    // Note: If seconds are not set, it will auto update an in-progess seconds update when chaning the other registers.

    // Setup structure
    r.iAddr = RV_3XXX_I2C_ADDR;
    r.iSpeed = RV_3XXX_I2C_SPEED;

    r.iWriteData = dataout; // Write out the new configuration
    r.iWriteLength = 8; // register address then send 7 bytes of data to program the first 7 registers
    r.iWriteTimeout = 5000; //UEZ_TIMEOUT_INFINITE;  // wait until bus ready

    r.iReadData = 0; // no reading, just writing
    r.iReadLength = 0;
    r.iReadTimeout = 0;

    // Setup data to go out
    dataout[0] = RV_REG_SEC; // reset to register 0
    dataout[1] = IDecimalToBCD(aTimeDate->iTime.iSecond);
    dataout[2] = IDecimalToBCD(aTimeDate->iTime.iMinute);
    dataout[3] = IDecimalToBCD(aTimeDate->iTime.iHour);
    dataout[4] = 0x0; // user defined day of week TODO (0 will set the programmed date as the 1st day of the week)
    dataout[5] = IDecimalToBCD(aTimeDate->iDate.iDay);
    dataout[6] = IDecimalToBCD(aTimeDate->iDate.iMonth);
    dataout[7] = IDecimalToBCD(aTimeDate->iDate.iYear-2000);

    return (*p->iI2C)->ProcessRequest(p->iI2C, &r); // perform the transaction
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  RTC_RV_3XXX_ReadReg
 *---------------------------------------------------------------------------*
 * Description:
 *      Read a RTC clock register
 * Inputs:
 *      T_RTC_RV_3XXX_Workspace *p -- Workspace
 *      TUInt8 aReg                 -- Register to write
 *      TUInt8 aValue               -- Value to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_RV_3XXX_ReadReg(
        T_RTC_RV_3XXX_Workspace *p,
        TUInt8 aReg,
        TUInt8 *aValue)
{
    I2C_Request r;

    // Read the whole RV_3XXX's registers
    r.iAddr = RV_3XXX_I2C_ADDR;
    r.iSpeed = RV_3XXX_I2C_SPEED;
    r.iWriteData = &aReg;
    r.iWriteLength = 1;
    r.iWriteTimeout = 5000; //UEZ_TIMEOUT_INFINITE;
    r.iReadData = aValue;
    r.iReadLength = 1;
    r.iReadTimeout = 5000; //UEZ_TIMEOUT_INFINITE;  // wait until bus ready

    // Do the read
    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  RTC_RV_3XXX_WriteReg
 *---------------------------------------------------------------------------*
 * Description:
 *      Write a RTC clock register
 * Inputs:
 *      T_RTC_RV_3XXX_Workspace *p -- Workspace
 *      TUInt8 aReg                 -- Register to write
 *      TUInt8 aValue               -- Value to write
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError RTC_RV_3XXX_WriteReg(
        T_RTC_RV_3XXX_Workspace *p,
        TUInt8 aReg,
        TUInt8 aValue)
{
    I2C_Request r;
    TUInt8 cmd[2];

    cmd[0] = aReg;
    cmd[1] = aValue;

    // Read the whole RV_3XXX's registers
    r.iAddr = RV_3XXX_I2C_ADDR;
    r.iSpeed = RV_3XXX_I2C_SPEED;
    r.iWriteData = cmd;
    r.iWriteLength = 2;
    r.iWriteTimeout = 5000; //UEZ_TIMEOUT_INFINITE;
    r.iReadData = 0;
    r.iReadLength = 0;
    r.iReadTimeout = 0;

    // Do the write
    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_RV_3XXX_Set
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the current RTC clock.
 * Inputs:
 *      void *aW                    -- Workspace
 *      const T_uezTimeDate *aTimeDate -- Time and date to set to
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RTC_RV_3XXX_Set(void *aWorkspace, const T_uezTimeDate *aTimeDate)
{
    T_RTC_RV_3XXX_Workspace *p = (T_RTC_RV_3XXX_Workspace *)aWorkspace;
    T_uezError error;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    error = RTC_RV_3XXX_LowLevelSet(p, aTimeDate);
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_RV_3XXX_Validate
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
T_uezError RTC_RV_3XXX_Validate(
        void *aWorkspace,
        const T_uezTimeDate *aTimeDate)
{
    T_RTC_RV_3XXX_Workspace *p = (T_RTC_RV_3XXX_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;
    T_uezError ret_error = UEZ_ERROR_NONE;
    TUInt8 status_flags;
    TUInt8 backup_register;
    UEZ_PARAMETER_NOT_USED(RTC_RV_3XXX_ReadReg);
    UEZ_PARAMETER_NOT_USED(RTC_RV_3XXX_WriteReg);

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    
    error = RTC_RV_3XXX_ReadReg(p, RV_REG_STATUS, &status_flags); // read the status registesr
    if (error) {
        // An error occurred in the communication to the part
        // Stop and return this error
        UEZSemaphoreRelease(p->iSem);
        return error;
    }

    if ((status_flags & BSF_BIT) == BSF_BIT) { // if the flag is 1, a backup switchover to vbatt happened
      // RTC should still be valid, unless the PORF was tripped.
    }
        
    if ((status_flags & PORF_BIT) == PORF_BIT) { // if the flag is 1, RTC needs to be reset
        // Integrity of time/date data is bad
        ret_error = UEZ_ERROR_INVALID;
        
        error = RTC_RV_3XXX_ReadReg(p, RV_REG_EEPROM_BACKUP, &backup_register); // read and modify the register to enable a VBAT mode
        backup_register = backup_register | BSM_BITS; // switch to level switching mode
        error = RTC_RV_3XXX_WriteReg(p, RV_REG_EEPROM_BACKUP, backup_register); // program the new mode
        
        error = RTC_RV_3XXX_WriteReg(p, RV_REG_STATUS, ~PORF_BIT); // clear the bit, so we can check if it is still 0 on next bootup
        
        // Check if settings actually got applied or we failed to program registers correctly.
        error = RTC_RV_3XXX_ReadReg(p, RV_REG_EEPROM_BACKUP, &backup_register);
        error = RTC_RV_3XXX_ReadReg(p, RV_REG_STATUS, &status_flags);
        if ((status_flags & PORF_BIT) == PORF_BIT) { ret_error = UEZ_ERROR_NAK; }
        if ((backup_register & BSM_BITS) != BSM_BITS) { ret_error = UEZ_ERROR_NAK; }

        if (aTimeDate) { // Was a new time/date setup passed in? If so then set it.
            // Set the time and date (without using semaphores since
            // we already have captured the semaphore)
            error = RTC_RV_3XXX_LowLevelSet(aWorkspace, aTimeDate);

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

T_uezError RTC_RV_3XXX_SetClockOutHz(
        void *aWorkspace,
        TUInt32 aHertz)
{
    T_uezError error = UEZ_ERROR_NOT_SUPPORTED;
    T_RTC_RV_3XXX_Workspace *p = (T_RTC_RV_3XXX_Workspace *)aWorkspace;
    UEZ_PARAMETER_NOT_USED(p);

    //RTC_RV_3XXX_ReadReg
    // configure CLKIE_BIT, configure RV_REG_EEPROM_CLKOUT, then write the new values
    //RTC_RV_3XXX_WriteReg

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RTC_RV_3XXX_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the RTC's interface.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_I2C_BUS **aI2C       -- I2C interface
 *---------------------------------------------------------------------------*/
void RTC_RV_3XXX_Configure(
        void *aWorkspace,
        DEVICE_I2C_BUS **aI2C)
{
    T_RTC_RV_3XXX_Workspace *p = (T_RTC_RV_3XXX_Workspace *)aWorkspace;

    // Set the I2C bus
    p->iI2C = aI2C;
}

void RTC_RV_3XXX_Create(const char *aName, const char *aI2CBusName)
{
    T_uezDeviceWorkspace *p_rtcdev;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    // Setup with exteranl RTC
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&RTC_MICRO_CRYSTAL_RV_3XXX_Interface,
            0,
            &p_rtcdev);
    UEZDeviceTableFind(aI2CBusName, &i2c);
    UEZDeviceTableGetWorkspace(i2c, &p_i2c);
    RTC_RV_3XXX_Configure(p_rtcdev, (DEVICE_I2C_BUS **)p_i2c);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_RTC RTC_MICRO_CRYSTAL_RV_3XXX_Interface = {
	{
	    // Common device interface
	    "RTC:MICROCRYSTAL:RV_3XXX",
	    0x0100,
	    RTC_RV_3XXX_InitializeWorkspace,
	    sizeof(T_RTC_RV_3XXX_Workspace),
	},
	
    // Functions
    RTC_RV_3XXX_Get,
    RTC_RV_3XXX_Set,
    RTC_RV_3XXX_Validate,
    RTC_RV_3XXX_SetClockOutHz,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  RTC_RV_3XXX.c
 *-------------------------------------------------------------------------*/
