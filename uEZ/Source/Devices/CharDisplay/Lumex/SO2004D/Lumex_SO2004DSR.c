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
#include "Config.h"

#include <uEZTypes.h>
#include <uEZBSP.h>
#include <uEZ.h>
#include <uEZRTOS.h>
#include <uEZDeviceTable.h>
#include "Lumex_SO2004DSR.h"

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

#define LUMEX_SO2004DSR_NUM_COLUMNS     20
#define LUMEX_SO2004DSR_NUM_ROWS        4

//#define LumexDelayUS(x)     UEZTaskDelay(1)
#define LumexDelayUS(x)     UEZBSPDelayUS((x))

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_CharDisplay *iDevice;
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

    TUInt32 iRow;
    TUInt32 iColumn;
} T_Lumex_SO2004DSR_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
const T_charDisplayInfo Lumex_SO2004DSR_Info = {
    LUMEX_SO2004DSR_NUM_COLUMNS,
    LUMEX_SO2004DSR_NUM_ROWS,
    CHAR_DISPLAY_FEATURE_CLEAR_SCREEN,
};

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
static T_uezError Lumex_SO2004DSR_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_Lumex_SO2004DSR_Workspace *p = (T_Lumex_SO2004DSR_Workspace *)aW;
    p->iNumOpen = 0;
    p->iRow = 0;
    p->iColumn = 0;

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
static void ILumex_SO2004DSR_SetDataLines(T_Lumex_SO2004DSR_Workspace *p, TUInt8 aLines)
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
#if 0 // not currently used
static void ILumex_SO2004DSR_ReadLines(
                T_Lumex_SO2004DSR_Workspace *p,
                TUInt8 *aLines)
{
    TUInt32 pins;

    // Set the output data lines
    GPIO_READ(p->iDataPort, ((TUInt32)0xFF)<<p->iDataPinShift, &pins);
    *aLines = (pins >> p->iDataPinShift);
}
#endif

/*---------------------------------------------------------------------------*
 * Routine:  ILumex_SO2004DSR_WriteDataOrInstr
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data to the display regardless if it is data or instruction.
 * Inputs:
 *      T_Lumex_SO2004DSR_Workspace *p -- Workspace
 *      TUInt8 aData                -- Data or Instruction to write
 *---------------------------------------------------------------------------*/
static void ILumex_SO2004DSR_WriteDataOrInstr(
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
    LumexDelayUS(50);

    // Latch the write
    GPIO_CLEAR(p->iEnablePort, p->iEnablePin);

    // Hold a bit
    LumexDelayUS(5);
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
static void ILumex_SO2004DSR_WriteData(
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
static void ILumex_SO2004DSR_WriteInstruction(
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
static T_uezError ILumex_SO2004DSR_ActivateLCD(T_Lumex_SO2004DSR_Workspace *p)
{
    // Ensure enable is LOW
    GPIO_SET(p->iEnablePort, p->iEnablePin);

    // Put into instruction mode
    // Wait at least 15 ms (we'll do 25 ms)
    GPIO_CLEAR(p->iDataInstructModePort, p->iDataInstructModePin);
    UEZBSPDelayMS(25);

    // Two lines, 1/16 duty cycle, 5x8 dots Operation Mode
    // Needs 39 uS, use 100 uS
    ILumex_SO2004DSR_WriteInstruction(p,
        INSTR_FUNC(IFUNC_8BIT|IFUNC_TWO_LINE|IFUNC_DISPLAY_ON));
    LumexDelayUS(100);

    // Display On: display on, cursor off, blinking off
    // Needs 39 uS, use 100 uS
    ILumex_SO2004DSR_WriteInstruction(p,
        INSTR_DISPLAY(IDISPLAY_ON|IDISPLAY_CURSOR_OFF|IDISPLAY_BLINK_OFF));
    LumexDelayUS(100);

    // Clear the display
    // Needs 1.53 ms, use 5 ms
    ILumex_SO2004DSR_WriteInstruction(p, INSTR_CLEAR);
    UEZBSPDelayMS(5);

    // Now go into data entry mode with auto increment and auto shift off
    // Needs 39 uS, use 100 uS
    ILumex_SO2004DSR_WriteInstruction(p,
        INSTR_ENTRY_MODE(IENTRY_INCREMENT|IENTRY_SHIFT_OFF));
    LumexDelayUS(100);

    p->iRow = 0;
    p->iColumn = 0;

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
static T_uezError ILumex_SO2004DSR_DeactivateLCD(
                T_Lumex_SO2004DSR_Workspace *p)
{
    // Display: display off, cursor off, blinking off
    // Needs 39 uS, use 100 uS
    ILumex_SO2004DSR_WriteInstruction(p, 0x08);
    LumexDelayUS(100);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ILumex_SO2004DSR_SetCursor
 *---------------------------------------------------------------------------*
 * Description:
 *      Move the cursor to the given column and row
 * Inputs:
 *      T_Lumex_SO2004DSR_Workspace *p -- Workspace
 *      TUInt32 aColumn         -- Column (left to right) location (0 based)
 *      TUInt32 aRow            -- Row (top to bottom) location (0 based)
 *---------------------------------------------------------------------------*/
static void ILumex_SO2004DSR_SetCursor(
    T_Lumex_SO2004DSR_Workspace *p,
    TUInt8 aColumn,
    TUInt8 aRow)
{
    // Change the cursor position
    ILumex_SO2004DSR_WriteInstruction(
        p,
        0x80+((aRow&1)?0x40:0x00) +
            ((aRow&2)?LUMEX_SO2004DSR_NUM_COLUMNS:0) +
            aColumn);
}

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_GetInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Return information about the Lumex SO2004DSR's screen.
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 * Outputs:
 *      const T_charDisplayInfo * -- Pointer to information structure.
 *---------------------------------------------------------------------------*/
const T_charDisplayInfo *Lumex_SO2004DSR_GetInfo(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);

    return &Lumex_SO2004DSR_Info;
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
 * Routine:  Lumex_SO2004DSR_DrawChar
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw a single character at the given location
 * Inputs:
 *      void *aWorkspace        -- This serial generic workspace
 *      TUInt32 aColumn         -- Column (left to right) location (0 based)
 *      TUInt32 aRow            -- Row (top to bottom) location (0 based)
 *      TUInt16 aChar           -- Character to draw
 * Outputs:
 *      T_uezError              -- Error code.  UEZ_ERROR_NONE is reported if
 *                                   all data is written.
 *---------------------------------------------------------------------------*/
T_uezError Lumex_SO2004DSR_DrawChar(
            void *aWorkspace,
            TUInt32 aColumn,
            TUInt32 aRow,
            TUInt16 aChar)
{
    // Decrement use count.  Are we done?
    T_Lumex_SO2004DSR_Workspace *p = (T_Lumex_SO2004DSR_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Only do anything if the row and column are within range
    if ((aRow < LUMEX_SO2004DSR_NUM_ROWS) &&
            (aColumn < LUMEX_SO2004DSR_NUM_COLUMNS)) {
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

        // Does the cursor need to be moved?
        if ((aRow != p->iRow) || (aColumn != p->iColumn)) {
            // Move the cursor to the proper location
            p->iColumn = aColumn;
            p->iRow = aRow;
            ILumex_SO2004DSR_SetCursor(
                p,
                (TUInt8)p->iColumn,
                (TUInt8)p->iRow);
        }

        // Got the cursor at the right place, draw character
        ILumex_SO2004DSR_WriteData(p, aChar);

        // Note that our new location is one to the right now
        // so we can continue from here
        p->iColumn++;

        UEZSemaphoreRelease(p->iSem);
    }

    // Report final error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_ClearScreen
 *---------------------------------------------------------------------------*
 * Description:
 *      Clear the screen.
 * Inputs:
 *      void *aWorkspace        -- This serial generic workspace
 * Outputs:
 *      T_uezError              -- Error code.  UEZ_ERROR_NONE is reported if
 *                                   all data is written.
 *---------------------------------------------------------------------------*/
T_uezError Lumex_SO2004DSR_ClearScreen(void *aWorkspace)
{
    // Decrement use count.  Are we done?
    T_Lumex_SO2004DSR_Workspace *p = (T_Lumex_SO2004DSR_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    // Clear the display
    // Needs 1.53 ms, use 5 ms
    ILumex_SO2004DSR_WriteInstruction(p, INSTR_CLEAR);
    UEZTaskDelay(5);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Lumex_SO2004DSR_Control
 *---------------------------------------------------------------------------*
 * Description:
 *      Command to configure the device in other ways specific to this
 *      hardware.
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
    /*
    // Process the command
    switch (aInstruction)  {
        default:
            return UEZ_ERROR_ILLEGAL_OPERATION;
    }
    */
    // If we got here, the control was valid, but it could not be supported
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError Lumex_SO2004DSR_Create(
                const char *aName,
                T_uezGPIOPortPin aEnable,
                T_uezGPIOPortPin aReadWrite,
                T_uezGPIOPortPin aDataInstructMode,
                T_uezGPIOPortPin aData)
{
    T_uezDeviceWorkspace *p_charDisplay;
    T_uezError error;
    int32_t i;

    error = UEZDeviceTableRegister(
        aName,
        (T_uezDeviceInterface *)&CharDisplay_Lumex_SO2004DSR_Interface,
        0,
        &p_charDisplay);
    if (error)
        return error;

    error = Lumex_SO2004DSR_Configure(
        p_charDisplay,
        GPIO_TO_HAL_PORT(aEnable), GPIO_TO_PIN_BIT(aEnable),
        GPIO_TO_HAL_PORT(aReadWrite), GPIO_TO_PIN_BIT(aReadWrite),
        GPIO_TO_HAL_PORT(aDataInstructMode), GPIO_TO_PIN_BIT(aDataInstructMode),
        GPIO_TO_HAL_PORT(aData), GPIO_TO_PIN_INDEX(aData));
    if (error)
        return error;
    UEZGPIOLock(aReadWrite);
    UEZGPIOClear(aReadWrite);
    UEZGPIOOutput(aReadWrite);
    UEZGPIOLock(aEnable);
    UEZGPIOSet(aEnable);
    UEZGPIOOutput(aEnable);
    UEZGPIOLock(aDataInstructMode);
    UEZGPIOSet(aDataInstructMode);
    UEZGPIOOutput(aDataInstructMode);
    for (i=0; i<8; i++) {
        UEZGPIOLock(aData+i);
        UEZGPIOOutput(aData+i);
    }
    return (*((DEVICE_CharDisplay **)p_charDisplay))->Open(p_charDisplay);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_CharDisplay CharDisplay_Lumex_SO2004DSR_Interface = {
    // Generic device interface
    {
	    "CharDisplay:Lumex:SO2004DSR",
	    0x0100,
	    Lumex_SO2004DSR_InitializeWorkspace,
	    sizeof(T_Lumex_SO2004DSR_Workspace),
	},
	
    // Functions
    Lumex_SO2004DSR_GetInfo,
    Lumex_SO2004DSR_Open,
    Lumex_SO2004DSR_Close,
    Lumex_SO2004DSR_Control,
    Lumex_SO2004DSR_DrawChar,
    Lumex_SO2004DSR_ClearScreen,
    0, // No scroll up
} ;

/*-------------------------------------------------------------------------*
 * End of File:  CharDisplay.c
 *-------------------------------------------------------------------------*/
