/*-------------------------------------------------------------------------*
 * File:  uEZ485GenericHalfDuplex.c
 *-------------------------------------------------------------------------*
 * Description:
 *      GenericHalfDuplex stream interface to a serial device.
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
#include <uEZGPIO.h>
#include <uEZRTOS.h>
#include "Generic_RS485.h"

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define TURN_AROUND_TIME      2 // ms

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
    T_uezSemaphore iDESem;
    TUInt32 iCountdown;
    TUInt32 iDriveEnableReleaseCountdown;
    TBool iDidOutput;

    HAL_GPIOPort **iReceiveEnablePort;
    TUInt32 iReceiveEnablePin;
    TUInt32 iReceiveEnablePolarity;
    TUInt32 iReceiveEnableReleaseCountdown;
} T_RS485_GenericHalfDuplex_Workspace;

static void RS485_GenericHalfDuplex_MonitorDriveEnable(
                T_uezTask aMyTask, 
                void *aWorkspace);


/*---------------------------------------------------------------------------*
 * Routine:  RS485_GenericHalfDuplex_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a GenericHalfDuplex serial workspace.  Configure will 
 *      setup the next step.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RS485_GenericHalfDuplex_InitializeWorkspace(void *aW)
{
    T_uezError error;

    T_RS485_GenericHalfDuplex_Workspace *p = (T_RS485_GenericHalfDuplex_Workspace *)aW;
    p->iQueueSend = 0;
    p->iQueueReceive = 0;
    p->iTxBusy = EFalse;
    p->iDidOutput = EFalse;
    p->iNumOpen = 0;
    p->iDriveEnablePort = 0;
    p->iDriveEnableReleaseCountdown = 0;
    p->iCountdown = 0;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISerialGenericHalfDuplexCallbackReceivedByte
 *---------------------------------------------------------------------------*
 * Description:
 *      A single byte has been received -- queue it.
 * Inputs:
 *      void *aCallbackWorkspace    -- Pointer to T_RS485_GenericHalfDuplex_Workspace
 *      TUInt8 aByte                -- Byte received from serial driver
 *---------------------------------------------------------------------------*/
static void ISerialGenericHalfDuplexCallbackReceivedByte(
                void *aCallbackWorkspace, 
                TUInt8 aByte)
{
    T_RS485_GenericHalfDuplex_Workspace *p = (T_RS485_GenericHalfDuplex_Workspace *)aCallbackWorkspace;

    // A single byte is received
    _isr_UEZQueueSend(p->iQueueReceive, &aByte);
}

/*---------------------------------------------------------------------------*
 * Routine:  ISerialGenericHalfDuplexCallbackTransmitEmpty
 *---------------------------------------------------------------------------*
 * Description:
 *      The transmission buffer is empty (or needs more characters).  Pull
 *      a byte out of the waiting buffer and put into the serial hardware
 *      output by calling SerialDevice->OutputByte.
 * Inputs:
 *      void *aCallbackWorkspace    -- Pointer to T_RS485_GenericHalfDuplex_Workspace
 *---------------------------------------------------------------------------*/
static void ISerialGenericHalfDuplexCallbackTransmitEmpty(
                void *aCallbackWorkspace,
                TUInt32 aNumBytesAvailable)
{
    T_RS485_GenericHalfDuplex_Workspace *p = (T_RS485_GenericHalfDuplex_Workspace *)aCallbackWorkspace;
    TUInt8 c;

    while (aNumBytesAvailable--)  {
        // Output a byte to the buffer
        if (_isr_UEZQueueReceive(p->iQueueSend, &c) == UEZ_ERROR_NONE)  {
            // Got data, send it
            (*p->iSerial)->OutputByte((T_halWorkspace *)p->iSerial, c);
        } else {
            // No more data to send
            p->iTxBusy = EFalse;
            // Start a countdown until we release
            _isr_UEZSemaphoreRelease(p->iDESem);
            break;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  RS485_GenericHalfDuplex_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the GenericHalfDuplex configuration with the given queue sizes and
 *      link to HAL Serial device.
 * Inputs:
 *      T_uezDeviceWorkspace *aWorkspace -- This hardware device
 *      HAL_Serial **aSerial      -- Serial Device to use
 *      TUInt32 aQueueSendSize    -- Size of the send queue
 *      TUInt32 aQueueReceiveSize   -- Size of the receive queue
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RS485_GenericHalfDuplex_Configure(
            T_uezDeviceWorkspace *aWorkspace, 
            HAL_Serial **aSerial,
            TUInt32 aQueueSendSize,
            TUInt32 aQueueReceiveSize,
            HAL_GPIOPort **aDriveEnablePort,
            TUInt32 aDriveEnablePin,
            TBool aDriveEnablePolarity,
            TUInt32 aDriveEnableReleaseTime,
            HAL_GPIOPort **aReceiveEnablePort,
            TUInt32 aReceiveEnablePin,
            TBool aReceiveEnablePolarity,
            TUInt32 aReceiveEnableReleaseTime)
{
    T_RS485_GenericHalfDuplex_Workspace *p = 
        (T_RS485_GenericHalfDuplex_Workspace *)aWorkspace;
    T_uezError error;

    // Record serial device
    p->iSerial = aSerial;
    p->iDriveEnablePort = aDriveEnablePort;
    p->iDriveEnablePin = aDriveEnablePin;
    p->iDriveEnablePolarity = aDriveEnablePolarity;
    p->iDriveEnableReleaseCountdown = aDriveEnableReleaseTime;
    p->iReceiveEnablePort = aReceiveEnablePort;
    p->iReceiveEnablePin = aReceiveEnablePin;
    p->iReceiveEnablePolarity = aReceiveEnablePolarity;
    p->iReceiveEnableReleaseCountdown = aReceiveEnableReleaseTime;
    p->iCountdown = 0;

    error = UEZSemaphoreCreateBinary(&p->iDESem);
    if (error)
        return error;

    // Create queue to hold sending data
    error = UEZQueueCreate(aQueueSendSize, 1, &p->iQueueSend);
    if (error)
        return error;

    // Create queue to hold receiving data
    error = UEZQueueCreate(aQueueReceiveSize, 1, &p->iQueueReceive);
    if (error)  {
        UEZQueueDelete(p->iQueueSend);
        p->iQueueSend = 0;
        return error;
    }

    // Make sure drive pins are set as outputs
    if (p->iDriveEnablePort)
        (*p->iDriveEnablePort)->SetOutputMode(p->iDriveEnablePort,
            p->iDriveEnablePin);
    if (p->iReceiveEnablePort)
        (*p->iReceiveEnablePort)->SetOutputMode(p->iReceiveEnablePort,
            p->iReceiveEnablePin);

    // Make sure we are NOT driving enabled
    if (p->iDriveEnablePort) {
        if (p->iDriveEnablePolarity) {
            (*p->iDriveEnablePort)->Clear(p->iDriveEnablePort, p->iDriveEnablePin);
        } else {
            (*p->iDriveEnablePort)->Set(p->iDriveEnablePort, p->iDriveEnablePin);
        }
    }

    // setup the callbacks
    (*p->iSerial)->Configure(
        (void *)p->iSerial, 
        (void *)p,
        ISerialGenericHalfDuplexCallbackReceivedByte,
        ISerialGenericHalfDuplexCallbackTransmitEmpty);

    UEZTaskCreate(
        (T_uezTaskFunction)RS485_GenericHalfDuplex_MonitorDriveEnable, 
        "DriveEnable", 
        128, 
        (void *)p, 
        UEZ_PRIORITY_HIGH, 
        0);


    // Return last error
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  RS485_GenericHalfDuplex_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the serial port (activating it if first user).
 * Inputs:
 *      void *aWorkspace          -- This serial GenericHalfDuplex workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RS485_GenericHalfDuplex_Open(void *aWorkspace)
{
    T_RS485_GenericHalfDuplex_Workspace *p = (T_RS485_GenericHalfDuplex_Workspace *)aWorkspace;
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
 * Routine:  RS485_GenericHalfDuplex_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the serial port (deactivating it if last user to close).
 * Inputs:
 *      void *aWorkspace          -- This serial GenericHalfDuplex workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError RS485_GenericHalfDuplex_Close(void *aWorkspace)
{
    // Decrement use count.  Are we done?
    T_RS485_GenericHalfDuplex_Workspace *p = (T_RS485_GenericHalfDuplex_Workspace *)aWorkspace;
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
 * Routine:  RS485_GenericHalfDuplex_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read data from the serial port.  If an infinite amount of time is
 *      given, the serial port will wait until data arrives.  If a value
 *      of 0 is given for the timeout, then the serial data is read
 *      only if data is available.  The timeout is the timeout between
 *      successive characters, not the complete transfer.
 * Inputs:
 *      void *aWorkspace          -- This serial GenericHalfDuplex workspace
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
T_uezError RS485_GenericHalfDuplex_Read(
            void *aWorkspace, 
            TUInt8 *aData, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesRead,
            TUInt32 aTimeout)
{
    // Decrement use count.  Are we done?
    T_RS485_GenericHalfDuplex_Workspace *p = (T_RS485_GenericHalfDuplex_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;

    for (i=0; i<aNumBytes; i++)  {
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
 * Routine:  RS485_GenericHalfDuplex_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data out the serial port or timeout trying.  If a timeout,
 *      the number of bytes written is reported.
 * Inputs:
 *      void *aWorkspace          -- This serial GenericHalfDuplex workspace
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
T_uezError RS485_GenericHalfDuplex_Write(
            void *aWorkspace, 
            TUInt8 *aData, 
            TUInt32 aNumBytes, 
            TUInt32 *aNumBytesWritten,
            TUInt32 aTimeout)
{
    // Decrement use count.  Are we done?
    T_RS485_GenericHalfDuplex_Workspace *p = (T_RS485_GenericHalfDuplex_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;

    for (i=0; i<aNumBytes; i++)  {
        // Send bytes one at a time
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

        // Don't let interrupts occur while we do this one
        (*p->iSerial)->Deactivate((T_halWorkspace *)p->iSerial);

        // Are we currently busy sending data?
        if (p->iTxBusy) {
            // Yes, busy, stuff another byte into the end of the queue
            error = UEZQueueSend(p->iQueueSend, aData, aTimeout);
        } else {
            // Transmit is not busy yet.  But we're about to be

            // Stop receive
            if (p->iReceiveEnablePort) {
                if (!p->iReceiveEnablePolarity) {
                    (*p->iReceiveEnablePort)->Set(p->iReceiveEnablePort, p->iReceiveEnablePin);
                    UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                } else {
                    UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                    (*p->iReceiveEnablePort)->Clear(p->iReceiveEnablePort, p->iReceiveEnablePin);
                }
            }

            // Start driving again
            if (p->iDriveEnablePort) {
                if (p->iDriveEnablePolarity) {
                    (*p->iDriveEnablePort)->Set(p->iDriveEnablePort, p->iDriveEnablePin);
                    UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                } else {
                    UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                    (*p->iDriveEnablePort)->Clear(p->iDriveEnablePort, p->iDriveEnablePin);
                }
            }

            // Declare transmit busy
            p->iTxBusy = ETrue;
            p->iDidOutput = ETrue;
            error = (*p->iSerial)->OutputByte((T_halWorkspace *)p->iSerial, *aData);
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
 * Routine:  RS485_GenericHalfDuplex_Command
 *---------------------------------------------------------------------------*
 * Description:
 *      Command to configure the port or do handshaking via the stream
 *      interface.  For the uEZ GenericHalfDuplex Serial driver, only
 *      STREAM_CONTROL_SET_SERIAL_SETTINGS and 
 *      STREAM_CONTROL_GET_SERIAL_SETTINGS are currently provided.
 * Inputs:
 *      void *aWorkspace          -- This serial GenericHalfDuplex workspace
 *      TUInt32 aControl          -- Command to execute
 *      void *aControlData        -- Data passed in with the command
 * Outputs:
 *      T_uezError                 -- Error code specific to command.
 *                                   If the command is unhandled, error code
 *                                   UEZ_ERROR_ILLEGAL_OPERATION is returned.
 *---------------------------------------------------------------------------*/
T_uezError RS485_GenericHalfDuplex_Control(
            void *aWorkspace, 
            TUInt32 aControl, 
            void *aControlData)
{
    T_RS485_GenericHalfDuplex_Workspace *p = 
        (T_RS485_GenericHalfDuplex_Workspace *)aWorkspace;
    HAL_Serial *ps = *(p->iSerial);

    // Process the command
    switch (aControl)  {
        case STREAM_CONTROL_SET_SERIAL_SETTINGS:
            return ps->SetSerialSettings(
                    p->iSerial, 
                    (T_Serial_Settings *)aControlData);
        case STREAM_CONTROL_GET_SERIAL_SETTINGS:   
            return ps->GetSerialSettings(
                    p->iSerial, 
                    (T_Serial_Settings *)aControlData);
        case STREAM_CONTROL_CONTROL_HANDSHAKING:
            if (ps->SetHandshakingControl)
                return ps->SetHandshakingControl(
                    p->iSerial, 
                    *((T_serialHandshakingControl *)aControlData));
            break;
        case STREAM_CONTROL_HANDSHAKING_STATUS:
            if (ps->GetHandshakingStatus)
                return ps->GetHandshakingStatus(
                    p->iSerial, 
                    (T_serialHandshakingStatus *)aControlData);
            break;
        default:
            return UEZ_ERROR_ILLEGAL_OPERATION;
    }

    // If we got here, the control was valid, but it could not be supported
    return UEZ_ERROR_NOT_SUPPORTED;
}

static void RS485_GenericHalfDuplex_MonitorDriveEnable(
                T_uezTask aMyTask, 
                void *aWorkspace)
{
    T_RS485_GenericHalfDuplex_Workspace *p = 
        (T_RS485_GenericHalfDuplex_Workspace *)aWorkspace;
    T_serialStatusByte status;

    for (;;) {
        // Wait for a release to occur (using no processor time until ready)
        // The TransmitEmpty routine will release this when
        // we no more data is being transmitted (and nothing is bufferred)
        // we'll then start counting down and if it doesn't go
        // busy again, we'll deactive then
        RTOS_ENTER_CRITICAL();
        p->iDidOutput = EFalse;
        RTOS_EXIT_CRITICAL();

        UEZSemaphoreGrab(p->iDESem, UEZ_TIMEOUT_INFINITE);

        // Reset the counter now
        p->iCountdown = p->iDriveEnableReleaseCountdown;

        if (p->iDriveEnableReleaseCountdown == 0)  {
            for (;;)  {
                // If release countdown is zero, we watch the status closely!
                UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
                // If we are busy again, don't change drive status
                if (p->iDidOutput) {
                    UEZSemaphoreRelease(p->iSem);
                    break;
                }
                UEZSemaphoreRelease(p->iSem);

                // Watch the status closely.  If we go empty, drop line and stop
                // watching.
                status = (*p->iSerial)->GetStatus(p->iSerial);
                if (status & SERIAL_STATUS_TRANSMIT_EMPTY)  {
                    // No longer need to drive
                    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

                    if (p->iReceiveEnablePort) {
                        if (!p->iReceiveEnablePolarity) {
                            (*p->iReceiveEnablePort)->Clear(p->iReceiveEnablePort, p->iReceiveEnablePin);
                            UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                        } else {
                            UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                            (*p->iDriveEnablePort)->Set(p->iReceiveEnablePort, p->iReceiveEnablePin);
                        }
                    }
                    
                    if (p->iDriveEnablePort) {
                        if (p->iDriveEnablePolarity) {
                            (*p->iDriveEnablePort)->Clear(p->iDriveEnablePort, p->iDriveEnablePin);
                            UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                        } else {
                            UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                            (*p->iDriveEnablePort)->Set(p->iDriveEnablePort, p->iDriveEnablePin);
                        }
                    }
                    UEZSemaphoreRelease(p->iSem);
                    break;
                }
            }
        } else {
            UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
            // Got released!  Start count down, but check every millisecond
            while (p->iCountdown)  {
                UEZSemaphoreRelease(p->iSem);
                UEZTaskDelay(1);
                UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

                // Did we go back to busy?  If so, stop here
                status = (*p->iSerial)->GetStatus(p->iSerial);
                if (p->iDidOutput) {
                    UEZSemaphoreRelease(p->iDESem);
                    break;
                }
                p->iCountdown--;
            }

            // Are we not busy?
            if (!p->iDidOutput)  {
                // No longer need to drive
                if (p->iDriveEnablePort) {
                    if (p->iDriveEnablePolarity) {
                        (*p->iDriveEnablePort)->Clear(p->iDriveEnablePort, p->iDriveEnablePin);
                        UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                    } else {
                        UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                        (*p->iDriveEnablePort)->Set(p->iDriveEnablePort, p->iDriveEnablePin);
                    }
                }
                // And need to receive again
                if (p->iReceiveEnablePort) {
                    if (!p->iReceiveEnablePolarity) {
                        (*p->iReceiveEnablePort)->Clear(p->iReceiveEnablePort, p->iReceiveEnablePin);
                        UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                    } else {
                        UEZTaskDelay(TURN_AROUND_TIME); // 20 ms turn around time
                        (*p->iReceiveEnablePort)->Set(p->iReceiveEnablePort, p->iReceiveEnablePin);
                    }
                }
            }
            UEZSemaphoreRelease(p->iSem);
        }
    }
}

T_uezError RS485_GenericHalfDuplex_Create(
    const char *aName,
    const T_RS485_GenericHalfDuplex_Settings *aSettings)
{
    T_uezDeviceWorkspace *p;
    HAL_Serial **p_serial;

    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&RS485_GenericHalfDuplex_Stream_Interface, 0,
        &p);
    HALInterfaceFind(aSettings->iSerialName, (T_halWorkspace **)&p_serial);
    return RS485_GenericHalfDuplex_Configure(p, p_serial,
        aSettings->iQueueSendSize, aSettings->iQueueReceiveSize,
        GPIO_TO_HAL_PORT(aSettings->iDriveEnable), GPIO_TO_PIN_BIT(
            aSettings->iDriveEnable), aSettings->iDriveEnablePolarity,
        aSettings->iDriveEnableReleaseTime, GPIO_TO_HAL_PORT(
            aSettings->iReceiveEnable), GPIO_TO_PIN_BIT(
            aSettings->iReceiveEnable), aSettings->iReceiveEnablePolarity,
        aSettings->aReceiveEnableReleaseTime);
}
/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_STREAM RS485_GenericHalfDuplex_Stream_Interface = {
    // GenericHalfDuplex device interface
    {
    "Serial:GenericHalfDuplex",
    0x0100,
    RS485_GenericHalfDuplex_InitializeWorkspace,
    sizeof(T_RS485_GenericHalfDuplex_Workspace),
    },

    // Functions
    RS485_GenericHalfDuplex_Open,
    RS485_GenericHalfDuplex_Close,
    RS485_GenericHalfDuplex_Control,
    RS485_GenericHalfDuplex_Read,
    RS485_GenericHalfDuplex_Write,
    // TODO: Need RS485_GenericHalfDuplex_Flush!
} ;

/*-------------------------------------------------------------------------*
 * End of File:  uEZ485GenericHalfDuplex.c
 *-------------------------------------------------------------------------*/
