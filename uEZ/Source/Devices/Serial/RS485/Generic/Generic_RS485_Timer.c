/*-------------------------------------------------------------------------*
 * File:  uEZ485Generic_Timer.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Generic_Timer stream interface to a serial device.
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
#include <uEZRTOS.h>
#include <uEZDeviceTable.h>
#include <uEZGPIO.h>
#include <Device/Stream.h>
#include <HAL/Serial.h>
#include <HAL/GPIO.h>
#include <HAL/Interrupt.h>
#include <HAL/Timer.h>
#include "Generic_RS485_Timer.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_STREAM *iDevice;
    T_uezSemaphore iSem;
    HAL_Serial **iSerial;
    T_uezQueue iQueueSend;
    T_uezQueue iQueueReceive;
    TBool iTxBusy;
    TUInt32 iNumOpen;
    HAL_GPIOPort **iDriveEnablePort;
    TUInt32 iDriveEnablePin;
    TUInt32 iDriveEnablePolarity;
    TUInt32 iCountdown;
    TBool iDidOutput;

    HAL_GPIOPort **iReceiveEnablePort;
    TUInt32 iReceiveEnablePin;
    TUInt32 iReceiveEnablePolarity;

    HAL_Timer **iTimer;
    TUInt32 iReleaseTimeCPUCycles;
    TUInt32 iDriveTimeMS;
} T_RS485_Generic_Timer_Workspace;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  RS485_Generic_Timer_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a Generic_Timer serial workspace.  Configure will
 *      setup the next step.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RS485_Generic_Timer_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aW;
    p->iQueueSend = 0;
    p->iQueueReceive = 0;
    p->iTxBusy = EFalse;
    p->iDidOutput = EFalse;
    p->iNumOpen = 0;
    p->iDriveEnablePort = 0;
    p->iCountdown = 0;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISerialGeneric_TimerCallbackReceivedByte
 *---------------------------------------------------------------------------*
 * Description:
 *      A single byte has been received -- queue it.
 * Inputs:
 *      void *aCallbackWorkspace    -- Pointer to T_RS485_Generic_Timer_Workspace
 *      TUInt8 aByte                -- Byte received from serial driver
 *---------------------------------------------------------------------------*/
static void ISerialGeneric_TimerCallbackReceivedByte(
    void *aCallbackWorkspace,
    TUInt8 aByte)
{
    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aCallbackWorkspace;

    // A single byte is received
    _isr_UEZQueueSend(p->iQueueReceive, &aByte);
}

/*---------------------------------------------------------------------------*
 * Routine:  IRS485_TimerCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      When the transmit has timed out, this routine is called to drop
 *      the drive lines.
 * Inputs:
 *      void *aCallbackWorkspace    -- RS485 Generic Timer workspace
 *---------------------------------------------------------------------------*/
static void IRS485_TimerCallback(void *aCallbackWorkspace)
{
    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aCallbackWorkspace;

    // No longer need to drive
    if (p->iDriveEnablePort) {
        if (p->iDriveEnablePolarity) {
            (*p->iDriveEnablePort)->Clear(p->iDriveEnablePort,
                p->iDriveEnablePin);
        } else {
            (*p->iDriveEnablePort)->Set(p->iDriveEnablePort, p->iDriveEnablePin);
        }
    }
    // And need to receive again
    if (p->iReceiveEnablePort) {
        if (!p->iReceiveEnablePolarity) {
            (*p->iReceiveEnablePort)->Clear(p->iReceiveEnablePort,
                p->iReceiveEnablePin);
        } else {
            (*p->iReceiveEnablePort)->Set(p->iReceiveEnablePort,
                p->iReceiveEnablePin);
        }
    }
    // This timer is one shot only, stop it here
    (*p->iTimer)->Disable(p->iTimer);
}

/*---------------------------------------------------------------------------*
 * Routine:  ISerialGeneric_TimerCallbackTransmitEmpty
 *---------------------------------------------------------------------------*
 * Description:
 *      The transmission buffer is empty (or needs more characters).  Pull
 *      a byte out of the waiting buffer and put into the serial hardware
 *      output by calling SerialDevice->OutputByte.
 * Inputs:
 *      void *aCallbackWorkspace    -- Pointer to T_RS485_Generic_Timer_Workspace
 *---------------------------------------------------------------------------*/
static void ISerialGeneric_TimerCallbackTransmitEmpty(
    void *aCallbackWorkspace,
    TUInt32 aNumBytesAvailable)
{
    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aCallbackWorkspace;
    TUInt8 c;
    PARAM_NOT_USED(aNumBytesAvailable);

    // Only send one byte at a time because we have greater control
    // over the timing (the LPC2478 takes 1 character minus 1 stop bit to process
    // this many characters
    // Output a byte to the buffer
    if (_isr_UEZQueueReceive(p->iQueueSend, &c) == UEZ_ERROR_NONE) {
        // Got data, send it
        (*p->iSerial)->OutputByte((T_halWorkspace *)p->iSerial, c);
    } else {
        // No more data to send
        p->iTxBusy = EFalse;

        // Set the time it takes until we get a match
        (*p->iTimer)->SetMatchRegister(p->iTimer, 0,
            p->iReleaseTimeCPUCycles, ETrue, EFalse, EFalse);

        // Reset and start the counter
        (*p->iTimer)->Reset(p->iTimer);
        (*p->iTimer)->Enable(p->iTimer);
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  RS485_Generic_Timer_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Generic_Timer configuration with the given queue sizes and
 *      link to HAL Serial device.
 * Inputs:
 *      T_uezDeviceWorkspace *aWorkspace -- This hardware device
 *      HAL_Serial **aSerial      -- Serial Device to use
 *      TUInt32 aQueueSendSize    -- Size of the send queue
 *      TUInt32 aQueueReceiveSize   -- Size of the receive queue
 *      HAL_GPIOPort **aDriveEnablePort -- Port with drive pin
 *      TUInt32 aDriveEnablePin -- Port pin bits of gpio to drive
 *      TBool aDriveEnablePolarity -- ETrue for drive HIGH, else EFalse for LOW
 *      HAL_GPIOPort **aReceiveEnablePort -- Port with receive pin
 *      TUInt32 aReceiveEnablePin -- Bits of port to use for receive
 *      TBool aReceiveEnablePolarity -- ETrue for HIGH, else EFlase for LOW
 *      HAL_Timer **aTimer -- Dedicated timer for use with RS485
 *      TUInt32 aReleaseTimeCPUCycles -- Number of CPU cycles to wait before 
 *                                       dropping drive enable
 *      TUInt32 aDriveTimeMS  -- Number of milliseconds to drive enable
 *                                  before starting to send data
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RS485_Generic_Timer_Configure(
    T_uezDeviceWorkspace *aWorkspace,
    HAL_Serial **aSerial,
    TUInt32 aQueueSendSize,
    TUInt32 aQueueReceiveSize,
    HAL_GPIOPort **aDriveEnablePort,
    TUInt32 aDriveEnablePin,
    TBool aDriveEnablePolarity,
    HAL_GPIOPort **aReceiveEnablePort,
    TUInt32 aReceiveEnablePin,
    TBool aReceiveEnablePolarity,

    HAL_Timer **aTimer,
    TUInt32 aReleaseTimeCPUCycles,
    TUInt32 aDriveTimeMS)
{
    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aWorkspace;
    T_uezError error;

    // Record serial device
    p->iSerial = aSerial;
    p->iDriveEnablePort = aDriveEnablePort;
    p->iDriveEnablePin = aDriveEnablePin;
    p->iDriveEnablePolarity = aDriveEnablePolarity;
    p->iReceiveEnablePort = aReceiveEnablePort;
    p->iReceiveEnablePin = aReceiveEnablePin;
    p->iReceiveEnablePolarity = aReceiveEnablePolarity;
    p->iTimer = aTimer;
    p->iReleaseTimeCPUCycles = aReleaseTimeCPUCycles;
    p->iDriveTimeMS = aDriveTimeMS;
    p->iCountdown = 0;

    // Create queue to hold sending data
    error = UEZQueueCreate(aQueueSendSize, 1, &p->iQueueSend);
    if (error)
        return error;

    // Create queue to hold receiving data
    error = UEZQueueCreate(aQueueReceiveSize, 1, &p->iQueueReceive);
    if (error) {
        UEZQueueDelete(p->iQueueSend);
        p->iQueueSend = 0;
        return error;
    }

    // Make sure we are NOT driving enabled
    if (p->iDriveEnablePort) {
        if (p->iDriveEnablePolarity) {
            (*p->iDriveEnablePort)->Clear(p->iDriveEnablePort,
                p->iDriveEnablePin);
        } else {
            (*p->iDriveEnablePort)->Set(p->iDriveEnablePort, p->iDriveEnablePin);
        }
    }

    // Setup the callbacks
    (*p->iSerial)->Configure((void *)p->iSerial, (void *)p,
        ISerialGeneric_TimerCallbackReceivedByte,
        ISerialGeneric_TimerCallbackTransmitEmpty);

    // Prepare the timer's interrupt callback routine
    (*p->iTimer)->SetMatchCallback(p->iTimer, 0,
        IRS485_TimerCallback, (void *)p);

    // Return last error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RS485_Generic_Timer_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the serial port (activating it if first user).
 * Inputs:
 *      void *aWorkspace          -- This serial Generic_Timer workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RS485_Generic_Timer_Open(void *aWorkspace)
{
    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Note how many users are using the serial port
    p->iNumOpen++;

    // If the first user, activate the port
    if (p->iNumOpen == 1)
        error = (*p->iSerial)->Activate((T_halWorkspace *)p->iSerial);

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RS485_Generic_Timer_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the serial port (deactivating it if last user to close).
 * Inputs:
 *      void *aWorkspace          -- This serial Generic_Timer workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RS485_Generic_Timer_Close(void *aWorkspace)
{
    // Decrement use count.  Are we done?
    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // If the last user, deactivate
    p->iNumOpen--;
    if (p->iNumOpen == 0)
        error = (*p->iSerial)->Deactivate((T_halWorkspace *)p->iSerial);

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RS485_Generic_Timer_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read data from the serial port.  If an infinite amount of time is
 *      given, the serial port will wait until data arrives.  If a value
 *      of 0 is given for the timeout, then the serial data is read
 *      only if data is available.  The timeout is the timeout between
 *      successive characters, not the complete transfer.
 * Inputs:
 *      void *aWorkspace          -- This serial Generic_Timer workspace
 *      TUInt8 *aData             -- Place to store data
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesRead    -- Number of bytes actually read.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_TIMEOUT is returned
 *                                   if timeout occurs trying to read
 *                                   the full amount and not enough data is
 *                                   provided.  UEZ_ERROR_NONE is reported if
 *                                   all data is returned.
 *---------------------------------------------------------------------------*/
T_uezError RS485_Generic_Timer_Read(
    void *aWorkspace,
    TUInt8 *aData,
    TUInt32 aNumBytes,
    TUInt32 *aNumBytesRead,
    TUInt32 aTimeout)
{
    // Decrement use count.  Are we done?
    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;

    for (i = 0; i < aNumBytes; i++) {
        // Keep trying to receive bytes
        error = UEZQueueReceive(p->iQueueReceive, aData, aTimeout);
        if (error)
            break;
        aData++;
    }

    // Report how many bytes we did get
    if (aNumBytesRead)
        *aNumBytesRead = i;

    // Report final error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RS485_Generic_Timer_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data out the serial port or timeout trying.  If a timeout,
 *      the number of bytes written is reported.
 * Inputs:
 *      void *aWorkspace          -- This serial Generic_Timer workspace
 *      TUInt8 *aData             -- Data to send
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesWritten -- Number of bytes actually written.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_TIMEOUT is returned
 *                                   if timeout occurs trying to write
 *                                   the full amount and not enough room was
 *                                   available.  UEZ_ERROR_NONE is reported if
 *                                   all data is written.
 *---------------------------------------------------------------------------*/
T_uezError RS485_Generic_Timer_Write(
    void *aWorkspace,
    TUInt8 *aData,
    TUInt32 aNumBytes,
    TUInt32 *aNumBytesWritten,
    TUInt32 aTimeout)
{
    // Decrement use count.  Are we done?
    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;

    for (i = 0; i < aNumBytes; i++) {
        // Send bytes one at a time
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

        // Don't let interrupts occur while we do this one
        (*p->iSerial)->Deactivate((T_halWorkspace *)p->iSerial);
        // Ensure we don't have a disable coming up
        (*p->iTimer)->Disable(p->iTimer);

        // Are we currently busy sending data?
        if (p->iTxBusy) {
            // Yes, busy, stuff another byte into the end of the queue
            error = UEZQueueSend(p->iQueueSend, aData, aTimeout);
        } else {
            // Transmit is not busy yet.  But we're about to be
            // Stop receive
            if (p->iReceiveEnablePort) {
                if (!p->iReceiveEnablePolarity) {
                    (*p->iReceiveEnablePort)->Set(p->iReceiveEnablePort,
                        p->iReceiveEnablePin);
                    UEZTaskDelay(p->iDriveTimeMS);
                } else {
                    UEZTaskDelay(p->iDriveTimeMS);
                    (*p->iReceiveEnablePort)->Clear(p->iReceiveEnablePort,
                        p->iReceiveEnablePin);
                }
            }

            // Start driving again
            if (p->iDriveEnablePort) {
                if (p->iDriveEnablePolarity) {
                    (*p->iDriveEnablePort)->Set(p->iDriveEnablePort,
                        p->iDriveEnablePin);
                    UEZTaskDelay(p->iDriveTimeMS);
                } else {
                    UEZTaskDelay(p->iDriveTimeMS);
                    (*p->iDriveEnablePort)->Clear(p->iDriveEnablePort,
                        p->iDriveEnablePin);
                }
            }

            // Declare transmit busy
            p->iTxBusy = ETrue;
            p->iDidOutput = ETrue;
            error = (*p->iSerial)->OutputByte((T_halWorkspace *)p->iSerial,
                *aData);
        }
        (*p->iSerial)->Activate((T_halWorkspace *)p->iSerial);

        UEZSemaphoreRelease(p->iSem);

        // Report any errors up to this point
        if (error)
            break;

        // Next byte
        aData++;
    }

    // Report how many bytes we did get
    if (aNumBytesWritten)
        *aNumBytesWritten = i;

    // Report final error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RS485_Generic_Timer_Command
 *---------------------------------------------------------------------------*
 * Description:
 *      Command to configure the port or do handshaking via the stream
 *      interface.  For the uEZ Generic_Timer Serial driver, only
 *      STREAM_CONTROL_SET_SERIAL_SETTINGS and 
 *      STREAM_CONTROL_GET_SERIAL_SETTINGS are currently provided.
 * Inputs:
 *      void *aWorkspace          -- This serial Generic_Timer workspace
 *      TUInt32 aControl          -- Command to execute
 *      void *aControlData        -- Data passed in with the command
 * Outputs:
 *      T_uezError                 -- Error code specific to command.
 *                                   If the command is unhandled, error code
 *                                   UEZ_ERROR_ILLEGAL_OPERATION is returned.
 *---------------------------------------------------------------------------*/
T_uezError RS485_Generic_Timer_Control(
    void *aWorkspace,
    TUInt32 aControl,
    void *aControlData)
{
    T_RS485_Generic_Timer_Workspace *p =
        (T_RS485_Generic_Timer_Workspace *)aWorkspace;
    HAL_Serial *ps = *(p->iSerial);

    // Process the command
    switch (aControl) {
        case STREAM_CONTROL_SET_SERIAL_SETTINGS:
            return ps->SetSerialSettings(p->iSerial,
                (T_Serial_Settings *)aControlData);
        case STREAM_CONTROL_GET_SERIAL_SETTINGS:
            return ps->GetSerialSettings(p->iSerial,
                (T_Serial_Settings *)aControlData);
        case STREAM_CONTROL_CONTROL_HANDSHAKING:
            if (ps->SetHandshakingControl)
                return ps->SetHandshakingControl(p->iSerial,
                    *((T_serialHandshakingControl *)aControlData));
            break;
        case STREAM_CONTROL_HANDSHAKING_STATUS:
            if (ps->GetHandshakingStatus)
                return ps->GetHandshakingStatus(p->iSerial,
                    (T_serialHandshakingStatus *)aControlData);
            break;
        default:
            return UEZ_ERROR_ILLEGAL_OPERATION;
    }

    // If we got here, the control was valid, but it could not be supported
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError RS485_Generic_Timer_Create(
    const char *aName,
    const T_RS485_Generic_Timer_Settings *aSettings)
{
    T_uezDeviceWorkspace *p;
    HAL_Serial **p_serial;
    HAL_Timer **p_timer;

    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&RS485_Generic_Timer_Stream_Interface, 0, &p);
    HALInterfaceFind(aSettings->iSerialName, (T_halWorkspace **)&p_serial);
    HALInterfaceFind(aSettings->iTimerName, (T_halWorkspace **)&p_timer);
    return RS485_Generic_Timer_Configure(p, p_serial,
        aSettings->iQueueSendSize, aSettings->iQueueReceiveSize,
        GPIO_TO_HAL_PORT(aSettings->iDriveEnable),
        GPIO_TO_PIN_BIT(aSettings->iDriveEnable),
        aSettings->iDriveEnablePolarity,
        GPIO_TO_HAL_PORT(aSettings->iReceiveEnable),
        GPIO_TO_PIN_BIT(aSettings->iReceiveEnable),
        aSettings->iReceiveEnablePolarity, p_timer, aSettings->iReleaseTime,
        aSettings->iDriveTime);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_STREAM RS485_Generic_Timer_Stream_Interface = {
// Generic_Timer device interface
    {
    "Serial:RS485:Generic_Timer",
    0x0100,
    RS485_Generic_Timer_InitializeWorkspace,
    sizeof(T_RS485_Generic_Timer_Workspace),
    },

    // Functions
    RS485_Generic_Timer_Open,
    RS485_Generic_Timer_Close,
    RS485_Generic_Timer_Control,
    RS485_Generic_Timer_Read,
    RS485_Generic_Timer_Write,
    // TODO: Need RS485_Generic_Timer_Flush!
};

/*-------------------------------------------------------------------------*
 * End of File:  uEZ485Generic_Timer.c
 *-------------------------------------------------------------------------*/
