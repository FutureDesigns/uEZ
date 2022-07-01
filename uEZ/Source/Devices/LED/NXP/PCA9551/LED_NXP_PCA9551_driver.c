/*-------------------------------------------------------------------------*
 * File:  LED_NXP_PCA9551_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the LEDBank for the PCA9551
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <Device/LEDBank.h>
#include <Device/I2CBus.h>
#include "LED_NXP_PCA9551.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define PCA9551_I2C_SPEED               400 // kHz

#define STATE_ON                        0
#define STATE_OFF                       1
#define STATE_BLINK0                    2
#define STATE_BLINK1                    3

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_LEDBank *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
    TUInt8 iI2CAddr;
    TUInt8 iLEDsState[8];
} T_LED_NXP_PCA9551_Workspace;

static const TUInt8 lookupTable[4][4] = {
    { (0<<0), (1<<0), (2<<0), (3<<0) },
    { (0<<2), (1<<2), (2<<2), (3<<2) },
    { (0<<4), (1<<4), (2<<4), (3<<4) },
    { (0<<6), (1<<6), (2<<6), (3<<6) },
};

static TUInt8 IBuildLEDSelector(TUInt8 *iLEDState)
{
    return lookupTable[0][iLEDState[0]] |
        lookupTable[1][iLEDState[1]] |
        lookupTable[2][iLEDState[2]] |
        lookupTable[3][iLEDState[3]];
}

static void IUnbuildLEDSelector(TUInt8 *aLEDState, TUInt8 bits)
{
    TUInt8 i;

    for (i=0; i<4; i++) {
        *(aLEDState++) = (bits & 3);
        bits >>= 2;
    }
}


/*---------------------------------------------------------------------------*
 * Routine:  IReadLEDConfig
 *---------------------------------------------------------------------------*
 * Description:
 *      Read led configurate and store in settings
 * Inputs:
 *      T_LED_NXP_PCA9551_Workspace *p  -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError IReadLEDConfig(T_LED_NXP_PCA9551_Workspace *p)
{
    I2C_Request r;
    TUInt8 cmd[3];
    TUInt8 settings[2];
    T_uezError error;

    r.iAddr = p->iI2CAddr;
    r.iSpeed = PCA9551_I2C_SPEED;

    // Send out the 1 byte command
    cmd[0] = 0x15;
    r.iWriteData = cmd;
    r.iWriteLength = 1;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;

    // Don't read any data
    r.iReadData = settings;
    r.iReadLength = 2;
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;

    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
    if (error)
        return error;

    // Interpret the settings
    IUnbuildLEDSelector(p->iLEDsState+0, settings[0]);
    IUnbuildLEDSelector(p->iLEDsState+4, settings[1]);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IUpdateLEDs
 *---------------------------------------------------------------------------*
 * Description:
 *      Internal routine that does the actual I2C call
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError IUpdateLEDs(T_LED_NXP_PCA9551_Workspace *p)
{
    I2C_Request r;
    TUInt8 cmd[3];

    r.iAddr = p->iI2CAddr;
    r.iSpeed = PCA9551_I2C_SPEED;

    // Write out 2 bytes 
    // (LED0 to LED3)
    // Start at register 5
    cmd[0] = 0x15;
    cmd[1] = IBuildLEDSelector(p->iLEDsState+0);
    // (LED4 to LED7)
    cmd[2] = IBuildLEDSelector(p->iLEDsState+4);

    // Send out the 3 byte command
    r.iWriteData = cmd;
    r.iWriteLength = 3;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;

    // Don't read any data
    r.iReadData = 0;
    r.iReadLength = 0;
    r.iReadTimeout = 0;

    return (*p->iI2C)->ProcessRequest(p->iI2C, &r);
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_NXP_PCA9551_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCA9551.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_NXP_PCA9551_InitializeWorkspace(void *aW)
{
    T_uezError error;
    TUInt8 i;

    T_LED_NXP_PCA9551_Workspace *p = (T_LED_NXP_PCA9551_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    for (i=0; i<8; i++)
        p->iLEDsState[i] = STATE_OFF;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_NXP_PCA9551_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn on multiple LEDs
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_NXP_PCA9551_On(void *aWorkspace, TUInt32 aLEDs)
{
    T_LED_NXP_PCA9551_Workspace *p = (T_LED_NXP_PCA9551_Workspace *)aWorkspace;
    TUInt32 bitMask;
    T_uezError error;
    TUInt8 i;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = IReadLEDConfig(p);
    if (!error) {
        for (i=0, bitMask=1; i<8; i++, bitMask<<=1) {
            // Set the bits to on if bit is set
            if (aLEDs & bitMask)
                p->iLEDsState[i] = STATE_ON;
        }

        error = IUpdateLEDs(p);
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_NXP_PCA9551_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn off multiple LEDs
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_NXP_PCA9551_Off(void *aWorkspace, TUInt32 aLEDs)
{
    T_LED_NXP_PCA9551_Workspace *p = (T_LED_NXP_PCA9551_Workspace *)aWorkspace;
    TUInt8 i;
    TUInt32 bitMask;
    T_uezError error;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = IReadLEDConfig(p);
    if (!error) {
        for (i=0, bitMask=1; i<8; i++, bitMask<<=1) {
            // Set the bits to on if bit is set
            if (aLEDs & bitMask)
                p->iLEDsState[i] = STATE_OFF;
        }

        // Update the LEDs state
        error = IUpdateLEDs(p);
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_NXP_PCA9551_Blink
 *---------------------------------------------------------------------------*
 * Description:
 *      Blink the given LEDs with the blink register.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aBlinkReg           -- Blink register used to control blink
 *      TUInt32 aLEDs               -- Bit mask of LEDs to blink
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_NXP_PCA9551_Blink(
        void *aWorkspace, 
        TUInt32 aBlinkReg,
        TUInt32 aLEDs)
{
    T_LED_NXP_PCA9551_Workspace *p = (T_LED_NXP_PCA9551_Workspace *)aWorkspace;
    TUInt8 i;
    TUInt32 bitMask;
    T_uezError error;
    TUInt8 state = STATE_BLINK0;
    if (aBlinkReg)
        state = STATE_BLINK1;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    error = IReadLEDConfig(p);
    if (!error) {
        for (i=0, bitMask=1; i<8; i++, bitMask<<=1) {
            // Set the bits to on if bit is set
            if (aLEDs & bitMask)
                p->iLEDsState[i] = state;
        }

        // Update the LEDs state
        error = IUpdateLEDs(p);
    }

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_NXP_PCA9551_SetBlinkRate
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the rate of a given blink register (as close as possible).
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aBlinkReg           -- Blink register used to control blink
 *      TUInt32 aPeriod             -- Period in ms
 *      TUInt8 aDutyCycle           -- Duty cycle in 0 (0%) to 255 (100%)
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LED_NXP_PCA9551_SetBlinkRate(
        void *aWorkspace, 
        TUInt32 aBlinkReg,
        TUInt32 aPeriod,
        TUInt8 aDutyCycle)
{
    T_LED_NXP_PCA9551_Workspace *p = (T_LED_NXP_PCA9551_Workspace *)aWorkspace;
    I2C_Request r;
    TUInt8 cmd[3] = { 0x02, 0x00, 0x00};
    TUInt32 v;
    T_uezError error;

    if (aBlinkReg)
        cmd[0] = 0x04;

    // Determine what the value is for the hertz (0-255)
    // Formula:  period = (1+v)/38 sec;
    //  or 38/(1+v) Hz
    if (aPeriod > 1000)
        v = 0;
    else
        v = ((aPeriod * 38) / 1000) - 1;
    if (v > 255)
        v = 255;
    cmd[1] = v;

    // The duty cycle is (256-aDutyCycle)/256 percent
    // We just need to flip it around so 0 is 255 and 255 is 0.
    cmd[2] = 255-aDutyCycle;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Update the part for the right blink reg
    r.iAddr = p->iI2CAddr;
    r.iSpeed = PCA9551_I2C_SPEED;

    // Send out the 3 byte command
    r.iWriteData = cmd;
    r.iWriteLength = 3;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;

    // Don't read any data
    r.iReadData = 0;
    r.iReadLength = 0;
    r.iReadTimeout = 0;

    // Do the command
    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  LED_NXP_PCA9551_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the PCA9551's I2C setting.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_I2C_BUS **aI2C       -- I2C interface
 *---------------------------------------------------------------------------*/
void LED_NXP_PCA9551_Configure(
        void *aWorkspace, 
        DEVICE_I2C_BUS **aI2C,
        TUInt8 aI2CAddr)
{
    T_LED_NXP_PCA9551_Workspace *p = (T_LED_NXP_PCA9551_Workspace *)aWorkspace;

    // Set the I2C bus
    p->iI2C = aI2C;
    p->iI2CAddr = aI2CAddr;
}

T_uezError LED_NXP_PCA9551_Create(
    const char *aName,
    const char *aI2CBusName,
    TUInt8 aI2CAddr)
{
    void *p;
    T_uezDevice i2c;
    DEVICE_I2C_BUS **p_i2c;
    T_uezError error;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&LEDBank_NXP_PCA9551_Interface, 0,
            (T_uezDeviceWorkspace **)&p);

    error = UEZDeviceTableFind(aI2CBusName, &i2c);
    if (error)
        return error;
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);

    LED_NXP_PCA9551_Configure(p, p_i2c, aI2CAddr);

    return UEZ_ERROR_NONE;
}
/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_LEDBank LEDBank_NXP_PCA9551_Interface = {
	{
	    // Common device interface
	    "LEDBank:NXP:PCA9551",
	    0x0100,
	    LED_NXP_PCA9551_InitializeWorkspace,
	    sizeof(T_LED_NXP_PCA9551_Workspace),
	},
	
    // Functions
    LED_NXP_PCA9551_On,
    LED_NXP_PCA9551_Off,
    LED_NXP_PCA9551_Blink,
    LED_NXP_PCA9551_SetBlinkRate,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  LED_NXP_PCA9551_driver.c
 *-------------------------------------------------------------------------*/
