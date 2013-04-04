/*-------------------------------------------------------------------------*
 * File:  CharDisplay.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Vacuum 
 * Implementation:
 *      Four serial ports are available (although they can be individually
 *      setup as needed).  Buffers/Queues are not stored here but handled
 *      by the layer above.  Only callback functions from within
 *      the IRQ are provided.
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
#include <Config.h>
#include <uEZTypes.h>
#include <uEZBSP.h>

// TBD:  not here!
//#include <uEZRTOS.h>

#include <uEZ.h>
#include <uEZRTOS.h>
#include "CharDisplay_Generic_Stream.h"

/*---------------------------------------------------------------------------*
 * Constants & Macros:
 *---------------------------------------------------------------------------*/
#define INSTR_CLEAR                 0x01                // 1.53 ms

#define INSTR_RETURN_HOME(settings) 0x02                // 1.53 ms

#define INSTR_ENTRY_MODE(settings)  (0x06|(settings))   // 39 uS
#define     IENTRY_INCREMENT            (1<<1)
#define     IENTRY_DECREMENT            (0<<1)
#define     IENTRY_SHIFT_ON             (1<<0)
#define     IENTRY_SHIFT_OFF            (0<<0)

#define INSTR_DISPLAY(settings)     (0x08|(settings))   // 39 uS
#define     IDISPLAY_ON                 (1<<2)
#define     IDISPLAY_OFF                (0<<2)
#define     IDISPLAY_CURSOR_ON          (1<<1)
#define     IDISPLAY_CURSOR_OFF         (0<<1)
#define     IDISPLAY_BLINK_ON           (1<<0)
#define     IDISPLAY_BLINK_OFF          (0<<0)

#define INSTR_SHIFT(settings)       (0x10|(settings))   // 39 uS
#define     ISHIFT_SCREEN               (1<<3)
#define     ISHIFT_CURSOR               (0<<3)
#define     ISHIFT_RIGHT                (1<<2)
#define     ISHIFT_LEFT                 (0<<2)

#define INSTR_FUNC(settings)        (0x20|(settings))   // 39 uS
#define     IFUNC_8BIT                  (1<<4)
#define     IFUNC_4BIT                  (0<<4)
#define     IFUNC_TWO_LINE              (1<<3)
#define     IFUNC_ONE_LINE              (0<<3)
#define     IFUNC_DISPLAY_ON            (1<<2)
#define     IFUNC_DISPLAY_OFF           (0<<2)

#define INSTR_SET_CGRAM_ADDR(x)     (0x40|((x)&0x3F))   // 39 uS

#define INSTR_SET_DDRAM_ADDR(x)     (0x80|((x)&0x7F))   // 39 uS

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_STREAM *iDevice;
    T_uezSemaphore iSem;
    TUInt32 iNumOpen;

    HAL_GPIOPort **iEnablePort;
    TUInt32 iEnablePin;
    HAL_GPIOPort **iReadWritePort;
    TUInt32 iReadWritePin;
    HAL_GPIOPort **iDataInstructModePort;
    TUInt32 iDataInstructModePin;
    HAL_GPIOPort **iDataPort;
    TUInt32 iDataPinShift;
} T_Lumex_SO2004DSR_Workspace;

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a generic serial workspace.  Configure will setup the next
 *      step.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Lumex_SO2004DSR_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Lumex_SO2004DSR_Workspace *p = (T_Lumex_SO2004DSR_Workspace *)aW;
    p->iNumOpen = 0;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the generic configuration with the given queue sizes and
 *      link to HAL Serial device.
 * Inputs:
 *      T_uezDeviceWorkspace *aWorkspace -- This hardware device
 *      HAL_GPIOPort **aEnablePort -- Enable port
 *      TUInt32 aEnablePin -- Enable port pin (0-31)
 *      HAL_GPIOPort **aReadWritePort -- Read/Write port
 *      TUInt32 aReadWritePin -- Read/Write pin (0-31)
 *      HAL_GPIOPort **aDataInstructModePort -- Data/Instruction Mode Port
 *      TUInt32 aDataInstructModePin -- Data/Instruction Mode Port
 *      HAL_GPIOPort **aDataPort -- Data Port
 *      TUInt32 aDataPinShift -- Data port number of bits to shift over
 *                                  before 8 consecutive pins
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Lumex_SO2004DSR_Configure(
            T_uezDeviceWorkspace *aWorkspace, 
            HAL_GPIOPort **aEnablePort,
            TUInt32 aEnablePin,
            HAL_GPIOPort **aReadWritePort,
            TUInt32 aReadWritePin,
            HAL_GPIOPort **aDataInstructModePort,
            TUInt32 aDataInstructModePin,
            HAL_GPIOPort **aDataPort,
            TUInt32 aDataPinShift)
{
    T_Lumex_SO2004DSR_Workspace *p = (T_Lumex_SO2004DSR_Workspace *)aWorkspace;

    // Since this driver is all port pins, just copy over the info
    p->iEnablePort = aEnablePort;
    p->iEnablePin = aEnablePin;
    p->iReadWritePort = aReadWritePort;
    p->iReadWritePin = aReadWritePin;
    p->iDataInstructModePort = aDataInstructModePort;
    p->iDataInstructModePin = aDataInstructModePin;
    p->iDataPort = aDataPort;
    p->iDataPinShift = aDataPinShift;

    // Return last error
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILumex_SO2004DSR_SetDataLines
 *---------------------------------------------------------------------------*
 * Description:
 *      Put 8-bits of data on the LCD data lines
 * Inputs:
 *      T_Lumex_SO2004DSR_Workspace *p -- Workspace
 *      TUInt8 aLines               -- 8 bits of data to put on lines
 *---------------------------------------------------------------------------*/
void ILumex_SO2004DSR_SetDataLines(T_Lumex_SO2004DSR_Workspace *p, TUInt8 aLines)
{
    // Set the output data lines
    GPIO_SET(p->iDataPort, ((TUInt32)aLines)<<p->iDataPinShift);
    GPIO_CLEAR(p->iDataPort, ((TUInt32)(aLines^0xFF))<<p->iDataPinShift);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILumex_SO2004DSR_ReadLines
 *---------------------------------------------------------------------------*
 * Description:
 *      Read the 8-bits of data on the LCD data lines
 * Inputs:
 *      T_Lumex_SO2004DSR_Workspace *p -- Workspace
 *      TUInt8 *aLines               -- 8 bits of data to read on lines
 *---------------------------------------------------------------------------*/
void ILumex_SO2004DSR_ReadLines(
                T_Lumex_SO2004DSR_Workspace *p, 
                TUInt8 *aLines)
{
    TUInt32 pins;

    // Set the output data lines
    GPIO_READ(p->iDataPort, ((TUInt32)0xFF)<<p->iDataPinShift, &pins);
    *aLines = (pins >> p->iDataPinShift);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILumex_SO2004DSR_WriteDataOrInstr
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data to the display regardless if it is data or instruction.
 * Inputs:
 *      T_Lumex_SO2004DSR_Workspace *p -- Workspace
 *      TUInt8 aData                -- Data or Instruction to write
 *---------------------------------------------------------------------------*/
void ILumex_SO2004DSR_WriteDataOrInstr(
                T_Lumex_SO2004DSR_Workspace *p, 
                TUInt8 aData)
{
    // Make sure we are in write mode (LOW)
    GPIO_CLEAR(p->iReadWritePort, p->iReadWritePin);
    // Setup for latching
    GPIO_SET(p->iEnablePort, p->iEnablePin);

    // Now output the data lines to be the control value
    ILumex_SO2004DSR_SetDataLines(p, aData);

    // Give the circuit time to change
    UEZBSPDelayUS(25);

    // Latch the write
    GPIO_CLEAR(p->iEnablePort, p->iEnablePin);

    // Hold a bit
    UEZBSPDelayUS(2);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILumex_SO2004DSR_WriteData
 *---------------------------------------------------------------------------*
 * Description:
 *      Write a data byte to the display.
 * Inputs:
 *      T_Lumex_SO2004DSR_Workspace *p -- Workspace
 *      TUInt8 aData                -- Data to write
 *---------------------------------------------------------------------------*/
void ILumex_SO2004DSR_WriteData(
                T_Lumex_SO2004DSR_Workspace *p, 
                TUInt8 aData)
{
    // Set to a data mode
    // Instructions/Control/Commands are HIGH on the RS line
    GPIO_SET(p->iDataInstructModePort, p->iDataInstructModePin);

    ILumex_SO2004DSR_WriteDataOrInstr(p, aData);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILumex_SO2004DSR_WriteInstruction
 *---------------------------------------------------------------------------*
 * Description:
 *      Write an instruction byte to the display.
 * Inputs:
 *      T_Lumex_SO2004DSR_Workspace *p -- Workspace
 *      TUInt8 aInstruction            -- Instruction to write
 *---------------------------------------------------------------------------*/
void ILumex_SO2004DSR_WriteInstruction(
                T_Lumex_SO2004DSR_Workspace *p, 
                TUInt8 aInstruction)
{
    // Set to a control mode
    // Instructions/Control/Commands are Low on the RS line
    GPIO_CLEAR(p->iDataInstructModePort, p->iDataInstructModePin);

    // Write the control byte
    ILumex_SO2004DSR_WriteDataOrInstr(p, aInstruction);
}

/*---------------------------------------------------------------------------*
 * Routine:  ILumex_SO2004DSR_ActivateLCD
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn on the char display and initialize.
 * Inputs:
 *      T_Lumex_SO2004DSR_Workspace *p -- Workspace
 *---------------------------------------------------------------------------*/
T_uezError ILumex_SO2004DSR_ActivateLCD(T_Lumex_SO2004DSR_Workspace *p)
{
    // Ensure enable is LOW
    GPIO_CLEAR(p->iEnablePort, p->iEnablePin);

    // Put into instruction mode
    // Wait at least 15 ms (we'll do 25 ms)
    GPIO_CLEAR(p->iDataInstructModePort, p->iDataInstructModePin);
    UEZBSPDelayMS(25);

    // Two lines, 1/16 duty cycle, 5x8 dots Operation Mode
    // Needs 39 uS, use 100 uS
    ILumex_SO2004DSR_WriteInstruction(p,
        INSTR_FUNC(IFUNC_8BIT|IFUNC_TWO_LINE|IFUNC_DISPLAY_ON));
    UEZBSPDelayUS(100);

    // Display On: display on, cursor off, blinking off
    // Needs 39 uS, use 100 uS
    ILumex_SO2004DSR_WriteInstruction(p,
        INSTR_DISPLAY(IDISPLAY_ON|IDISPLAY_CURSOR_OFF|IDISPLAY_BLINK_OFF));
    UEZBSPDelayUS(100);

    // Clear the display
    // Needs 1.53 ms, use 5 ms
    ILumex_SO2004DSR_WriteInstruction(p, INSTR_CLEAR);
    UEZBSPDelayMS(5);

    // Now go into data entry mode with auto increment and auto shift off
    // Needs 39 uS, use 100 uS
    ILumex_SO2004DSR_WriteInstruction(p, 
        INSTR_ENTRY_MODE(IENTRY_INCREMENT|IENTRY_SHIFT_OFF));
    UEZBSPDelayUS(100);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILumex_SO2004DSR_DeactivateLCD
 *---------------------------------------------------------------------------*
 * Description:
 *      Turn off the char display.
 * Inputs:
 *      T_Lumex_SO2004DSR_Workspace *p -- Workspace
 *---------------------------------------------------------------------------*/
T_uezError ILumex_SO2004DSR_DeactivateLCD(
                T_Lumex_SO2004DSR_Workspace *p)
{
    // Display: display off, cursor off, blinking off
    // Needs 39 uS, use 100 uS
    ILumex_SO2004DSR_WriteInstruction(p, 0x08);
    UEZBSPDelayUS(100);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the serial port (activating it if first user).
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Lumex_SO2004DSR_Open(void *aWorkspace)
{
    T_Lumex_SO2004DSR_Workspace *p = (T_Lumex_SO2004DSR_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Note how many users are using the serial port
    p->iNumOpen++;

    // If the first user, activate the port
    if (p->iNumOpen == 1)
        error = ILumex_SO2004DSR_ActivateLCD(p);

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the serial port (deactivating it if last user to close).
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Lumex_SO2004DSR_Close(void *aWorkspace)
{
    // Decrement use count.  Are we done?
    T_Lumex_SO2004DSR_Workspace *p = (T_Lumex_SO2004DSR_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // If the last user, deactivate
    p->iNumOpen--;
    if (p->iNumOpen == 0)
        error = ILumex_SO2004DSR_DeactivateLCD(p);

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Reading data is not allowed.  This is an output port only.
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 *      TUInt8 *aData             -- Place to store data
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesRead    -- Number of bytes actually read.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 * Outputs:
 *      T_uezError                 -- Error code.  Always reports
 *                                      UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Lumex_SO2004DSR_Read(
            void *aWorkspace, 
            TUInt8 *aData, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesRead,
            TUInt32 aTimeout)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data out to the display.
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 *      TUInt8 *aData             -- Data to send
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesWritten -- Number of bytes actually written.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 *      TUInt32 aTimeout          -- Not used.  Fixed time used to send.
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_NONE is reported if
 *                                   all data is written.
 *---------------------------------------------------------------------------*/
T_uezError Lumex_SO2004DSR_Write(
            void *aWorkspace, 
            TUInt8 *aData, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesWritten,
            TUInt32 aTimeout)
{
    // Decrement use count.  Are we done?
    T_Lumex_SO2004DSR_Workspace *p = (T_Lumex_SO2004DSR_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    for (i=0; i<aNumBytes; i++)
        ILumex_SO2004DSR_WriteData(p, *(aData++));

    UEZSemaphoreRelease(p->iSem);

    // Report how many bytes we did get
    if (aNumBytesWritten)
        *aNumBytesWritten = i;
    
    // Report final error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_Command
 *---------------------------------------------------------------------------*
 * Description:
 *      Command to configure the port or do handshaking via the stream
 *      interface.  For the uEZ Generic Serial driver, only
 *      STREAM_CONTROL_SET_SERIAL_SETTINGS and 
 *      STREAM_CONTROL_GET_SERIAL_SETTINGS are currently provided.
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 *      TUInt32 aInstruction          -- Command to execute
 *      void *aInstructionData        -- Data passed in with the command
 *                                   In this case, a pointer to command code
 *                                   stored in a 'char' type.
 * Outputs:
 *      T_uezError                 -- Error code specific to command.
 *                                   If the command is unhandled, error code
 *                                   UEZ_ERROR_ILLEGAL_OPERATION is returned.
 *---------------------------------------------------------------------------*/
T_uezError Lumex_SO2004DSR_Control(
            void *aWorkspace, 
            TUInt32 aInstruction, 
            void *aInstructionData)
{
    T_Lumex_SO2004DSR_Workspace *p = (T_Lumex_SO2004DSR_Workspace *)aWorkspace;

    // Process the command
    switch (aInstruction)  {
        default:
            return UEZ_ERROR_ILLEGAL_OPERATION;
    }

    // If we got here, the control was valid, but it could not be supported
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_STREAM Lumex_SO2004DSR_Stream_Interface = {
    // Generic device interface
    "CharDisplay:Lumex:SO2004DSR",
    0x0100,
    Lumex_SO2004DSR_InitializeWorkspace,
    sizeof(T_Lumex_SO2004DSR_Workspace),

    // Functions
    Lumex_SO2004DSR_Open,
    Lumex_SO2004DSR_Close,
    Lumex_SO2004DSR_Control,
    Lumex_SO2004DSR_Read,
    Lumex_SO2004DSR_Write,
} ;

/*===========================================================================*
 * END OF FILE:  CharDisplay.c
 *===========================================================================*/
