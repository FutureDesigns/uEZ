/*-------------------------------------------------------------------------*
 * File:  Button_NXP_PCA9551_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the ButtonBank for the PCA9551
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
#include "Button_PCA9551.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

#define STATE_ON                        0
#define STATE_OFF                       1
#define STATE_BLINK0                    2
#define STATE_BLINK1                    3

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_ButtonBank *iDevice;
    T_uezSemaphore iSem;
    DEVICE_I2C_BUS **iI2C;
    TUInt8 iI2CAddr;
    TUInt8 iButtonsState[8];
} T_Button_NXP_PCA9551_Workspace;

static TUInt8 IBuildButtonSelector(TUInt8 *iButtonState)
{
    static const TUInt8 lookupTable[4][4] = {
        { (0<<0), (1<<0), (2<<0), (3<<0) },
        { (0<<2), (1<<2), (2<<2), (3<<2) },
        { (0<<4), (1<<4), (2<<4), (3<<4) },
        { (0<<6), (1<<6), (2<<6), (3<<6) },
    };

    return lookupTable[0][iButtonState[0]] |
        lookupTable[1][iButtonState[1]] |
        lookupTable[2][iButtonState[2]] |
        lookupTable[3][iButtonState[3]];
}

/*---------------------------------------------------------------------------*
 * Routine:  IUpdateButtons
 *---------------------------------------------------------------------------*
 * Description:
 *      Internal routine that does the actual I2C call
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError IUpdateButtons(T_Button_NXP_PCA9551_Workspace *p)
{
    I2C_Request r;
    TUInt8 cmd[3];

    r.iAddr = p->iI2CAddr;
    r.iSpeed = PCA9551_I2C_SPEED;

    // Write out 2 bytes 
    // (Button0 to Button3)
    // Start at register 6
    cmd[0] = 0x06;
    cmd[1] = IBuildButtonSelector(p->iButtonsState+0);
    // (Button4 to Button7)
    cmd[2] = IBuildButtonSelector(p->iButtonsState+4);

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
 * Routine:  Button_NXP_PCA9551_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup of this workspace for PCA9551.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Button_NXP_PCA9551_InitializeWorkspace(void *aW)
{
    T_uezError error;
    TUInt8 i;

    T_Button_NXP_PCA9551_Workspace *p = (T_Button_NXP_PCA9551_Workspace *)aW;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    for (i=0; i<8; i++)
        p->iButtonsState[i] = STATE_OFF;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Button_NXP_PCA9551_On
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current PCA9551 ButtonBank reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Button_NXP_PCA9551_On(void *aWorkspace, TUInt32 aButtons)
{
    T_Button_NXP_PCA9551_Workspace *p = (T_Button_NXP_PCA9551_Workspace *)aWorkspace;
    TUInt32 bitMask;
    T_uezError error;
    TUInt8 i;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    for (i=0, bitMask=1; i<8; i++, bitMask<<=1) {
        // Set the bits to on if bit is set
        if (aButtons & bitMask)
            p->iButtonsState[i] = STATE_ON;
    }

    error = IUpdateButtons(p);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Button_NXP_PCA9551_Off
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current PCA9551 ButtonBank reading
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezTimeDate *aTimeDate    -- Time and date returned
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Button_NXP_PCA9551_Off(void *aWorkspace, TUInt32 aButtons)
{
    T_Button_NXP_PCA9551_Workspace *p = (T_Button_NXP_PCA9551_Workspace *)aWorkspace;
    TUInt8 i;
    TUInt32 bitMask;
    T_uezError error;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    for (i=0, bitMask=1; i<8; i++, bitMask<<=1) {
        // Set the bits to on if bit is set
        if (aButtons & bitMask)
            p->iButtonsState[i] = STATE_OFF;
    }

    // Update the Buttons state
    error = IUpdateButtons(p);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Button_NXP_PCA9551_Blink
 *---------------------------------------------------------------------------*
 * Description:
 *      Blink the given Buttons with the blink register.
 * Inputs:
 *      void *aW                    -- Workspace
 *      TUInt32 aBlinkReg           -- Blink register used to control blink
 *      TUInt32 aButtons               -- Bit mask of Buttons to blink
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Button_NXP_PCA9551_Blink(
        void *aWorkspace, 
        TUInt32 aBlinkReg,
        TUInt32 aButtons)
{
    T_Button_NXP_PCA9551_Workspace *p = (T_Button_NXP_PCA9551_Workspace *)aWorkspace;
    TUInt8 i;
    TUInt32 bitMask;
    T_uezError error;
    TUInt8 state = STATE_BLINK0;
    if (aBlinkReg)
        state = STATE_BLINK1;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    for (i=0, bitMask=1; i<8; i++, bitMask<<=1) {
        // Set the bits to on if bit is set
        if (aButtons & bitMask)
            p->iButtonsState[i] = state;
    }

    // Update the Buttons state
    error = IUpdateButtons(p);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Button_NXP_PCA9551_SetBlinkRate
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
T_uezError Button_NXP_PCA9551_Read(
        void *aWorkspace, 
        TUInt32 *aButtons)
{
    T_Button_NXP_PCA9551_Workspace *p = (T_Button_NXP_PCA9551_Workspace *)aWorkspace;
    T_uezError error;
    I2C_Request r;
    TUInt8 reg0 = 0x00;
    TUInt8 data;

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Update the part for the right blink reg
    r.iAddr = p->iI2CAddr;
    r.iSpeed = PCA9551_I2C_SPEED;

    // Send out the 3 byte command
    r.iWriteData = &reg0;
    r.iWriteLength = 1;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;

    // Read the button status
    r.iReadData = &data;
    r.iReadLength = 1;
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;

    // Do the command
    data = 0x00;
    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);
    *aButtons = data;

    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Button_NXP_PCA9551_SetActiveButtons(
        void *aWorkspace, 
        TUInt32 aActiveButtons)
{
    T_Button_NXP_PCA9551_Workspace *p = (T_Button_NXP_PCA9551_Workspace *)aWorkspace;
    TUInt8 i;
    I2C_Request r;
    T_uezError error;
    TUInt8 cmd[3] = {0x15, 0x00, 0x00};

    // Setup the command
    for (i=0; i<4; i++)
        if (aActiveButtons & (1<<i))
            cmd[1] |= (1<<(i*2));
    for (i=0; i<4; i++)
        if (aActiveButtons & (1<<(4+i)))
            cmd[2] |= (1<<(i*2));

    // Allow only one transfer at a time
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Update the part for the right blink reg
    r.iAddr = p->iI2CAddr;
    r.iSpeed = PCA9551_I2C_SPEED;

    // Send out the 3 byte command
    r.iWriteData = cmd;
    r.iWriteLength = 3;
    r.iWriteTimeout = UEZ_TIMEOUT_INFINITE;

    // Read the button status
    r.iReadData = 0;
    r.iReadLength = 0;
    r.iReadTimeout = UEZ_TIMEOUT_INFINITE;

    // Do the command
    error = (*p->iI2C)->ProcessRequest(p->iI2C, &r);

    UEZSemaphoreRelease(p->iSem);

    return error;
}


/*---------------------------------------------------------------------------*
 * Routine:  Button_NXP_PCA9551_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the PCA9551's I2C setting.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_I2C_BUS **aI2C       -- I2C interface
 *---------------------------------------------------------------------------*/
void ButtonBank_NXP_PCA9551_Configure(
        void *aWorkspace, 
        DEVICE_I2C_BUS **aI2C,
        TUInt8 aI2CAddr)
{
    T_Button_NXP_PCA9551_Workspace *p = (T_Button_NXP_PCA9551_Workspace *)aWorkspace;

    // Set the I2C bus
    p->iI2C = aI2C;
    p->iI2CAddr = aI2CAddr;
}

/*---------------------------------------------------------------------------*
 * Routine:  ButtonBank_NXP_PCA9551_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a PCA9551 Button Bank driver.
 * Inputs:
 *      const char *aName           -- Name of this created device driver
 *      const char *aI2CBusName     -- I2C Bus to use
 *      TUInt8 aI2CAddr             -- 7-bit I2C address of PCA9551
 *---------------------------------------------------------------------------*/
T_uezError ButtonBank_NXP_PCA9551_Create(
    const char *aName,
    const char *aI2CBusName,
    TUInt8 aI2CAddr)
{
    void *p;
    T_uezDevice i2c;
    DEVICE_I2C_BUS **p_i2c;
    T_uezError error;

    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&ButtonBank_NXP_PCA9551_Interface, 0,
        (T_uezDeviceWorkspace **)&p);

    error = UEZDeviceTableFind(aI2CBusName, &i2c);
    if (error)
        return error;

    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);

    ButtonBank_NXP_PCA9551_Configure(p, p_i2c, aI2CAddr);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_ButtonBank ButtonBank_NXP_PCA9551_Interface = {
    {
        // Common device interface
        "ButtonBank:NXP:PCA9551",
        0x0100,
        Button_NXP_PCA9551_InitializeWorkspace,
        sizeof(T_Button_NXP_PCA9551_Workspace),
    },
    	
    // Functions
    Button_NXP_PCA9551_SetActiveButtons,
    Button_NXP_PCA9551_Read,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  Button_NXP_PCA9551_driver.c
 *-------------------------------------------------------------------------*/
