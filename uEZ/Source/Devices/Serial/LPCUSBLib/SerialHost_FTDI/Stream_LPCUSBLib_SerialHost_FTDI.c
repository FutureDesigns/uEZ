/*-------------------------------------------------------------------------*
 * File:  Stream_LPCUSBLib_SerialHost.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Stream interface for the Segger SWO Cortex M debugger terminal.
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
#include <uEZProcessor.h>
#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include <Types/Serial.h>
#include "Stream_LPCUSBLib_SerialHost_FTDI.h"
#include <Source/Library/LPCOpen/LPCUSBLib/Drivers/USB/USB.h>
#include <Source/Library/LPCOpen/LPCUSBLib/LPCUSBLib_uEZ.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define FTDI_SERIAL_HOST_RESET                  0  //!< Reset the port
#define FTDI_SERIAL_HOST_MODEM_CTRL             1  //!< Set the modem control register
#define FTDI_SERIAL_HOST_SET_FLOW_CTRL          2  //!< Set flow control register
#define FTDI_SERIAL_HOST_SET_BAUD_RATE          3  //!< Set baud rate
#define FTDI_SERIAL_HOST_SET_DATA               4  //!< Set the data characteristics of the port
#define FTDI_SERIAL_HOST_GET_MODEM_STATUS       5  //!< Retrieve current value of modem status register
#define FTDI_SERIAL_HOST_SET_EVENT_CHAR         6  //!< Set the event character
#define FTDI_SERIAL_HOST_SET_ERROR_CHAR         7  //!< Set the error character
#define FTDI_SERIAL_HOST_SET_LATENCY_TIMER      9  //!< Set the latency timer
#define FTDI_SERIAL_HOST_GET_LATENCY_TIMER      10 //!< Get the latency timer


#define FTDI_RS0_CTS    (1 << 4)
#define FTDI_RS0_DSR    (1 << 5)
#define FTDI_RS0_RI     (1 << 6)
#define FTDI_RS0_RLSD   (1 << 7)

#define FTDI_RS_DR      1
#define FTDI_RS_OE      (1<<1)
#define FTDI_RS_PE      (1<<2)
#define FTDI_RS_FE      (1<<3)
#define FTDI_RS_BI      (1<<4)
#define FTDI_RS_THRE    (1<<5)
#define FTDI_RS_TEMT    (1<<6)
#define FTDI_RS_FIFO    (1<<7)

#define FTDI_STATUS_B0_MASK     (FTDI_RS0_CTS | FTDI_RS0_DSR | FTDI_RS0_RI | FTDI_RS0_RLSD)
#define FTDI_STATUS_B1_MASK     (FTDI_RS_BI)
#define FTDI_RS_ERR_MASK        (FTDI_RS_BI | FTDI_RS_PE | FTDI_RS_FE | FTDI_RS_OE)


#define FTDI_SIO_SET_DTR_MASK 0x1
#define FTDI_SIO_SET_DTR_HIGH ((FTDI_SIO_SET_DTR_MASK  << 8) | 1)
#define FTDI_SIO_SET_DTR_LOW  ((FTDI_SIO_SET_DTR_MASK  << 8) | 0)
#define FTDI_SIO_SET_RTS_MASK 0x2
#define FTDI_SIO_SET_RTS_HIGH ((FTDI_SIO_SET_RTS_MASK << 8) | 2)
#define FTDI_SIO_SET_RTS_LOW  ((FTDI_SIO_SET_RTS_MASK << 8) | 0)

#define FTDI_CLASS      0xFF
#define FTDI_SUBCLASS   0xFF
#define FTDI_PROTOCOL   0xFF

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct
{
    struct
    {
        uint8_t  DataINPipeNumber; /**< Pipe number of the CDC interface's IN data pipe. */
        bool     DataINPipeDoubleBank; /**< Indicates if the CDC interface's IN data pipe should use double banking. */

        uint8_t  DataOUTPipeNumber; /**< Pipe number of the CDC interface's OUT data pipe. */
        bool     DataOUTPipeDoubleBank; /**< Indicates if the CDC interface's OUT data pipe should use double banking. */

        uint8_t  PortNumber;        /**< Port number that this interface is running.
                                    */
    } iConfig; /**< Config data for the USB class interface within the device. All elements in this section
               *   <b>must</b> be set or the interface will fail to enumerate and operate correctly.
               */
    struct
    {
        bool IsActive; /**< Indicates if the current interface instance is connected to an attached device, valid
                        *   after @ref CDC_Host_ConfigurePipes() is called and the Host state machine is in the
                        *   Configured state.
                        */
        uint8_t  ControlInterfaceNumber; /**< Interface index of the CDC-ACM control interface within the attached device. */

        uint16_t DataINPipeSize; /**< Size in bytes of the CDC interface's IN data pipe. */
        uint16_t DataOUTPipeSize;  /**< Size in bytes of the CDC interface's OUT data pipe. */

        struct
        {
            uint8_t HostToDevice; /**< Control line states from the host to device, as a set of \c CDC_CONTROL_LINE_OUT_*
                                   *   masks - to notify the device of changes to these values, call the
                                   *   @ref CDC_Host_SendControlLineStateChange() function.
                                   */
            uint8_t DeviceToHost; /**< Control line states from the device to host, as a set of \c CDC_CONTROL_LINE_IN_*
                                   *   masks. This value is updated each time @ref CDC_Host_USBTask() is called.
                                   */
        } ControlLineStates; /**< Current states of the virtual serial port's control lines between the device and host. */

        CDC_LineEncoding_t LineEncoding; /**< Line encoding used in the virtual serial port, for the device's information.
                                          *   This is generally only used if the virtual serial port data is to be
                                          *   reconstructed on a physical UART. When set by the host application, the
                                          *   @ref CDC_Host_SetLineEncoding() function must be called to push the changes
                                          *   to the device.
                                          */
        TUInt8 iTransmitEmpty;
    } iState; /**< State data for the USB class interface within the device. All elements in this section
              *   <b>may</b> be set to initial values, but may also be ignored to default to sane values when
              *   the interface is enumerated.
              */
} USB_ClassInfo_FTDI_Serial_Host_t;

typedef struct {
    const DEVICE_STREAM *iDevice;
    T_uezSemaphore iSem;
    TUInt8 iUnitAddress;
    USB_ClassInfo_FTDI_Serial_Host_t iInterface;
    TUInt32 iWriteBufferSize; //!< Currently not used
    TUInt32 iReadBufferSize;
    T_uezQueue iQueueReceive;
    TUInt8 iStatus;
    T_serialHandshakingStatus iHandshakingStatus;
    T_serialHandshakingControl iHandshakingControl;
    TUInt32 iBRK;
    TUInt32 iParityError;
    TUInt32 iFrameError;
    TUInt32 iOverrunError;
} T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static void IEVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum);
static int32_t IUpdate(int32_t aUnitAddress);

static T_LPCUSBLib_Host_Callbacks G_HostCallbacks = {
        0, // HostError
        IEVENT_USB_Host_DeviceEnumerationComplete, // DeviceEnumerationComplete
        0, // DeviceEnumerationFailed
        0, // DeviceUnattached
        0, // DeviceAttached
        IUpdate, // Update
};

static T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *G_workspaces[2] = { 0, 0 };

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static uint8_t ISetLineEncoding(USB_ClassInfo_FTDI_Serial_Host_t* const aInterfaceInfo);

static uint8_t FTDI_SerialHost_NextCDCInterfaceEndpoint(void* const CurrentDescriptor)
{
    USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

    if (Header->Type == DTYPE_Endpoint) {
        USB_Descriptor_Endpoint_t* Endpoint = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Endpoint_t);

        uint8_t EndpointType = (Endpoint->Attributes & EP_TYPE_MASK);

        if (((EndpointType == EP_TYPE_BULK) || (EndpointType == EP_TYPE_INTERRUPT))
                && !(Pipe_IsEndpointBound(Endpoint->EndpointAddress))) {
            return DESCRIPTOR_SEARCH_Found;
        }
    } else if (Header->Type == DTYPE_Interface) {
        return DESCRIPTOR_SEARCH_Fail;
    }

    return DESCRIPTOR_SEARCH_NotFound;
}

static uint8_t FTDI_SerialHost_NextCDCDataInterface(void* const CurrentDescriptor)
{
    USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

    if (Header->Type == DTYPE_Interface) {
        USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);

//        if ((Interface->Class == CDC_CSCP_CDCDataClass) && (Interface->SubClass == CDC_CSCP_NoDataSubclass)
//                && (Interface->Protocol == CDC_CSCP_NoDataProtocol)) {
//            return DESCRIPTOR_SEARCH_Found;
//        }
        if ((Interface->Class == FTDI_CLASS) && (Interface->SubClass == FTDI_SUBCLASS)
                && (Interface->Protocol == FTDI_PROTOCOL)) {
            return DESCRIPTOR_SEARCH_Found;
        }
    }

    return DESCRIPTOR_SEARCH_NotFound;
}



static uint8_t FTDI_SerialHost_FindConfigInterface(USB_ClassInfo_FTDI_Serial_Host_t* const aInterfaceInfo,
        uint16_t * aConfigDescriptorSize, void** aConfigDescriptorData)
{
    USB_Descriptor_Endpoint_t* DataINEndpoint = NULL;
    USB_Descriptor_Endpoint_t* DataOUTEndpoint = NULL;
    USB_Descriptor_Interface_t* ControlInterface = NULL;
    uint8_t portnum = aInterfaceInfo->iConfig.PortNumber;

    CDC_LineEncoding_t lineEncoding = aInterfaceInfo->iState.LineEncoding; // remember setting
    memset(&aInterfaceInfo->iState, 0x00, sizeof(aInterfaceInfo->iState));
    aInterfaceInfo->iState.LineEncoding = lineEncoding;
    DataINEndpoint  = NULL;
    DataOUTEndpoint = NULL;

    while (!(DataINEndpoint) || !(DataOUTEndpoint)) {
        // FIrst, we need the CDC-like Control interface
        if (!(ControlInterface)
                || USB_GetNextDescriptorComp(aConfigDescriptorSize, aConfigDescriptorData, FTDI_SerialHost_NextCDCInterfaceEndpoint)
                        != DESCRIPTOR_SEARCH_COMP_Found)
        {
            // Look for a descriptor
            if (USB_GetNextDescriptorComp(aConfigDescriptorSize, aConfigDescriptorData, FTDI_SerialHost_NextCDCDataInterface)
                    != DESCRIPTOR_SEARCH_COMP_Found) {
                // End of list?  Nothing?  Done.
                return CDC_ENUMERROR_NoCompatibleInterfaceFound;
            }

            // The first one is the interface, take that
            ControlInterface = DESCRIPTOR_PCAST(*aConfigDescriptorData, USB_Descriptor_Interface_t);
            continue;
        }

        // By this point, we do have a control interface.  Now we need the endpoints
        USB_Descriptor_Endpoint_t* EndpointData = DESCRIPTOR_PCAST(*aConfigDescriptorData, USB_Descriptor_Endpoint_t);

        // Is this an in or an out endpoint?
        if ((EndpointData->EndpointAddress & ENDPOINT_DIR_MASK) == ENDPOINT_DIR_IN) {
            // IN Output
            DataINEndpoint = EndpointData;
        } else {
            // OUT Endpoint
            DataOUTEndpoint = EndpointData;
        }
    }

    // At this point, we have pointers into the descriptor structure, but now we need to
    // use that data and fill our configuration.  Walk through all the pipes and configure.
    for (uint8_t PipeNum = 1; PipeNum < PIPE_TOTAL_PIPES; PipeNum++) {
        uint16_t Size;
        uint8_t Type;
        uint8_t Token;
        uint8_t EndpointAddress;
        uint8_t InterruptPeriod;
        bool DoubleBanked;

        if (PipeNum == aInterfaceInfo->iConfig.DataINPipeNumber) {
            Size = le16_to_cpu(DataINEndpoint->EndpointSize);
            EndpointAddress = DataINEndpoint->EndpointAddress;
            Token = PIPE_TOKEN_IN;
            Type = EP_TYPE_BULK;
            DoubleBanked = aInterfaceInfo->iConfig.DataINPipeDoubleBank;
            InterruptPeriod = 0;

            aInterfaceInfo->iState.DataINPipeSize = DataINEndpoint->EndpointSize;
        } else if (PipeNum == aInterfaceInfo->iConfig.DataOUTPipeNumber) {
            Size = le16_to_cpu(DataOUTEndpoint->EndpointSize);
            EndpointAddress = DataOUTEndpoint->EndpointAddress;
            Token = PIPE_TOKEN_OUT;
            Type = EP_TYPE_BULK;
            DoubleBanked = aInterfaceInfo->iConfig.DataOUTPipeDoubleBank;
            InterruptPeriod = 0;

            aInterfaceInfo->iState.DataOUTPipeSize = DataOUTEndpoint->EndpointSize;
        } else {
            continue;
        }

        if (!(Pipe_ConfigurePipe(portnum, PipeNum, Type, Token, EndpointAddress, Size,
                DoubleBanked ? PIPE_BANK_DOUBLE : PIPE_BANK_SINGLE))) {
            return CDC_ENUMERROR_PipeConfigurationFailed;
        }

        if (InterruptPeriod)
            Pipe_SetInterruptPeriod(InterruptPeriod);
    }

    // Setup the default state
    aInterfaceInfo->iState.ControlInterfaceNumber = ControlInterface->InterfaceNumber;
    aInterfaceInfo->iState.ControlLineStates.HostToDevice = (CDC_CONTROL_LINE_OUT_RTS | CDC_CONTROL_LINE_OUT_DTR);
    aInterfaceInfo->iState.ControlLineStates.DeviceToHost = (CDC_CONTROL_LINE_IN_DCD | CDC_CONTROL_LINE_IN_DSR);
    aInterfaceInfo->iState.IsActive = true;

    return CDC_ENUMERROR_NoError;
}


/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
static void IEVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
    uint16_t ConfigDescriptorSize;
    uint8_t ConfigDescriptorData[512];
    uint8_t *pCfgDesc;
    uint8_t err_code;
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p = G_workspaces[corenum];
    USB_ClassInfo_FTDI_Serial_Host_t *p_serial = &p->iInterface;
    int32_t serialInfCount;

    /* Read Configuration descriptor */
    if (USB_Host_GetDeviceConfigDescriptor(corenum, 1, &ConfigDescriptorSize, ConfigDescriptorData, sizeof(ConfigDescriptorData))
            != HOST_GETCONFIG_Successful) {
        printf("Error Retrieving Configuration Descriptor.\r\n");
        return;
    }

    serialInfCount = 0;
    pCfgDesc = ConfigDescriptorData;
    /* Parse configuration descriptor to find CDC interfaces, note that the pointer to the configuration data is changed each time the function is called */
    while ((err_code = FTDI_SerialHost_FindConfigInterface(&p_serial[serialInfCount], &ConfigDescriptorSize, (void **)&pCfgDesc))
            == CDC_ENUMERROR_NoError) {
        serialInfCount++;
        if (serialInfCount >= 1) {
            printf("Reached Max Serial Interface of %d\r\n", 1);
            break;
        }
    }

    if (err_code == CDC_ENUMERROR_PipeConfigurationFailed) {
        printf("Error in Configuring Pipe.\r\n");
        return;
    }
    /* No CDC interface was found */
    if (serialInfCount == 0) {
        printf("Attached Device Not a Valid CDC Device.\r\n");
        return;
    }

    // Tell the unit we are using the first configuration in the USB Descriptor
    if (USB_Host_SetDeviceConfiguration(p_serial->iConfig.PortNumber, 1) != HOST_SENDCONTROL_Successful) {
        printf("Error Setting Device Configuration.\r\n");
        return;
    }

    /* Set line coding for each interface */
    ISetLineEncoding(p_serial);

    printf("Serial Device Enumerated.\r\n");
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FTDI_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a generic serial workspace.  Configure will setup the next
 *      step.
 * Inputs:
 *      void *aW                    -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_FTDI_InitializeWorkspace(void *aW)
{
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p = (T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *)aW;
    T_uezError error;

    p->iQueueReceive = 0;
    p->iReadBufferSize = 512;
    p->iWriteBufferSize = 512;
    p->iUnitAddress = 0;
    p->iStatus = 0;

    error = UEZSemaphoreCreateBinary(&p->iSem);

    return error;
}

uint16_t IBytesAvailableToRead(T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p)
{
    USB_ClassInfo_FTDI_Serial_Host_t *p_serial = &p->iInterface;
    uint8_t portnum = p_serial->iConfig.PortNumber;

    // If not configured or active, there are no bytes to get
    if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(p_serial->iState.IsActive))
        return 0;

    // Does our input pipe have data?
    Pipe_SelectPipe(portnum, p_serial->iConfig.DataINPipeNumber);
    Pipe_Unfreeze();

    // Any IN data received?
    if (Pipe_IsINReceived(portnum)) {
        // Yes, but is it more than zero?
        if (!(Pipe_BytesInPipe(portnum))) {
            // Nope, no bytes.
            Pipe_ClearIN(portnum);
            Pipe_Freeze();
            return 0;
        } else {
            // Data is waiting.  Freeze the pipe and get the number
            Pipe_Freeze();
            return Pipe_BytesInPipe(portnum);
        }
    } else {
        // Nothing yet.  Release it and stop here
        Pipe_Freeze();
        return 0;
    }
}

TInt16 IReadByte(T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p)
{
    USB_ClassInfo_FTDI_Serial_Host_t *p_serial = &p->iInterface;
    uint8_t portnum = p_serial->iConfig.PortNumber;
    int16_t ReceivedByte = -1;

    if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(p_serial->iState.IsActive))
      return -1;

    Pipe_SelectPipe(portnum,p_serial->iConfig.DataINPipeNumber);
    Pipe_Unfreeze();

    if (Pipe_IsINReceived(portnum))
    {
        if (Pipe_BytesInPipe(portnum))
          ReceivedByte = Pipe_Read_8(portnum);

        if (!(Pipe_BytesInPipe(portnum)))
          Pipe_ClearIN(portnum);
    }

    Pipe_Freeze();

    return ReceivedByte;
}

static uint8_t ISetBaud(USB_ClassInfo_FTDI_Serial_Host_t* const aInterfaceInfo)
{
    TUInt16 BaudDivisor;
    TUInt32 BaseClock = 48000000; // For FT232R
    TUInt32 aBaudRate = aInterfaceInfo->iState.LineEncoding.BaudRateBPS;

    // Calculate the fractional divisor
    BaudDivisor = ((BaseClock / 16) / aBaudRate) |
        ((((BaseClock / 2) / aBaudRate) & 4) ? 0x4000    // +0.5
        : (((BaseClock / 2) / aBaudRate) & 2) ? 0x8000  // +0.25
        : (((BaseClock / 2) / aBaudRate) & 1) ? 0xC000  // +0.125
        : 0);

    USB_ControlRequest = (USB_Request_Header_t )
            {
                    .bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR | REQREC_DEVICE),
                    .bRequest = FTDI_SERIAL_HOST_SET_BAUD_RATE,
                    .wValue = BaudDivisor,
                    .wIndex = aInterfaceInfo->iState.ControlInterfaceNumber,
                    .wLength = 0,
            };
    uint8_t portnum = aInterfaceInfo->iConfig.PortNumber;

    Pipe_SelectPipe(portnum, PIPE_CONTROLPIPE);

    return USB_Host_SendControlRequest(portnum, NULL);
}

static uint8_t ISetLineEncoding(USB_ClassInfo_FTDI_Serial_Host_t* const p_serial)
{
    uint8_t error;
    CDC_LineEncoding_t *p_encoding = &p_serial->iState.LineEncoding;

    // Send the baud rate
    error = ISetBaud(p_serial);

    if (error == 0) {
        TUInt16 value;

        // Calculate the data in value
        value = ((p_encoding->DataBits & 0xFF) << 0) |  // Word length
                ((p_encoding->ParityType & 0x03) << 8) | // Pairty type
                ((p_encoding->CharFormat & 0x03) << 11) | // Stop bits
                ((0 & 0x01) << 14) ; // TX break on/off (not used)

        USB_ControlRequest = (USB_Request_Header_t )
                {
                        .bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR | REQREC_DEVICE),
                        .bRequest = FTDI_SERIAL_HOST_SET_DATA,
                        .wValue = value,
                        .wIndex = p_serial->iState.ControlInterfaceNumber,
                        .wLength = 0,
                };
        uint8_t portnum = p_serial->iConfig.PortNumber;

        Pipe_SelectPipe(portnum, PIPE_CONTROLPIPE);

        error = USB_Host_SendControlRequest(portnum, NULL);
    }
    return error;
}

static T_uezError ISetSerialSettings(
        T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p,
        T_Serial_Settings *aSettings)
{
    T_uezError error = UEZ_ERROR_NONE;
    USB_ClassInfo_FTDI_Serial_Host_t *p_serial = &p->iInterface;
    CDC_LineEncoding_t *p_encoding = &p_serial->iState.LineEncoding;

    // Set the baud
    p_encoding->BaudRateBPS = aSettings->iBaud;
    ISetBaud(p_serial);

    // Translate word length
    p_encoding->DataBits = aSettings->iWordLength;

    // Translate stop bits
    if (aSettings->iStopBit == SERIAL_STOP_BITS_1)
        p_encoding->CharFormat = 0; // 1 stop bit
    else if (aSettings->iStopBit == SERIAL_STOP_BITS_1_POINT_5)
        p_encoding->CharFormat = 1; // 1.5 stop bits
    else if (aSettings->iStopBit == SERIAL_STOP_BITS_2)
        p_encoding->CharFormat = 2; // 2 stop bits

    // Translate parity bits
    if (aSettings->iParity == SERIAL_PARITY_NONE)
        p_encoding->ParityType = 0; // 1 stop bit
    else if (aSettings->iParity == SERIAL_PARITY_ODD)
        p_encoding->ParityType = 1; // 1 stop bit
    else if (aSettings->iParity == SERIAL_PARITY_EVEN)
        p_encoding->ParityType = 2; // 1 stop bit
    else if (aSettings->iParity == SERIAL_PARITY_MARK)
        p_encoding->ParityType = 3; // 1 stop bit
    else if (aSettings->iParity == SERIAL_PARITY_SPACE)
        p_encoding->ParityType = 4; // 1 stop bit

    // Send it all out
    if(ISetLineEncoding(p_serial) == 0){
        error = UEZ_ERROR_NONE;
    } else {
        error = UEZ_ERROR_NOT_READY;
    }

    return error;
}

static T_uezError IGetSerialSettings(
        T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p,
        T_Serial_Settings *aSettings)
{
    USB_ClassInfo_FTDI_Serial_Host_t *p_serial = &p->iInterface;
    T_uezError error = UEZ_ERROR_NONE;

    // Get the current baud rate
    aSettings->iBaud = p_serial->iState.LineEncoding.BaudRateBPS;

    // Hard coded settings for now
    aSettings->iWordLength = 8;
    aSettings->iStopBit = SERIAL_STOP_BITS_1;
    aSettings->iParity = SERIAL_PARITY_NONE;
    aSettings->iFlowControl = SERIAL_FLOW_CONTROL_NONE;

    return error;
}
static T_uezError IUSBErrorToUEZError(TUInt8 aError)
{
    switch (aError) {
        case HOST_SENDCONTROL_Successful:
            return UEZ_ERROR_NONE;
        case HOST_SENDCONTROL_DeviceDisconnected:
            return UEZ_ERROR_DEVICE_NOT_FOUND;
        case HOST_SENDCONTROL_PipeError:
            return UEZ_ERROR_COULD_NOT_CONNECT;
        case HOST_SENDCONTROL_SetupStalled:
            return UEZ_ERROR_STALL;
        case HOST_SENDCONTROL_SoftwareTimeOut:
            return UEZ_ERROR_TIMEOUT;
        default:
            return UEZ_ERROR_INTERNAL_ERROR;
    }
    //return UEZ_ERROR_UNKNOWN;
}

static T_uezError ISetHandshakingControl(
        T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p,
        T_serialHandshakingControl *aHandshaking)
{
    T_serialHandshakingControl c;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt16 value = 0;
    TUInt8 usb_error;

    p->iHandshakingControl = *aHandshaking;
    c = *aHandshaking;

    if (c & SERIAL_HANDSHAKING_RTS_HIGH)
        value |= FTDI_SIO_SET_RTS_HIGH;
    else
        value |= FTDI_SIO_SET_RTS_LOW;

    if (c & SERIAL_HANDSHAKING_DTR_HIGH)
        value |= FTDI_SIO_SET_DTR_HIGH;
    else
        value |= FTDI_SIO_SET_DTR_LOW;

    // Setup the USB control request
    USB_ControlRequest = (USB_Request_Header_t )
            {
                    .bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_VENDOR | REQREC_DEVICE),
                    .bRequest = FTDI_SERIAL_HOST_MODEM_CTRL,
                    .wValue = value,
                    .wIndex = p->iInterface.iState.ControlInterfaceNumber,
                    .wLength = 0,
            };

    uint8_t portnum = p->iInterface.iConfig.PortNumber;

    Pipe_SelectPipe(portnum, PIPE_CONTROLPIPE);

    usb_error = USB_Host_SendControlRequest(portnum, NULL);
    error = IUSBErrorToUEZError(usb_error);

    return error;
}

static T_uezError IGetHandshakingStatus(
        T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p,
        T_serialHandshakingStatus *aHandshaking)
{
    T_uezError error = UEZ_ERROR_NONE;

    // Return the current handshaking status
    *aHandshaking = p->iHandshakingStatus;

    return error;
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  ILPCUSBLibHostTask
 *---------------------------------------------------------------------------*
 * Description:
 *      Task that monitors the USB drive and configures the connected/
 *      unconnected state while running the LPCLibUsb task thread.
 * Inputs:
 *      T_uezTask aTask              -- This task
 *      void *aParameters            -- Parameters (workspace)
 * Outputs:
 *      TUInt32                      -- return code (never returns)
 *---------------------------------------------------------------------------*/
static int32_t IUpdate(int32_t aUnitAddress)
{
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p = G_workspaces[aUnitAddress];
    USB_ClassInfo_FTDI_Serial_Host_t *p_serial = &p->iInterface;
    TInt32 i;
    TUInt16 dataLen;
    int32_t activity = 0;

    if (p == 0)
        return 0; // Not ready

    // Try to get access, if not timeout and return
    if (UEZSemaphoreGrab(p->iSem, 5) == UEZ_ERROR_TIMEOUT)
        return 0;

    /* For each CDC interface check if any data is received, if so read data, modify data and send it back to device. */
    if((dataLen = IBytesAvailableToRead(p)) > 0) {
        if (dataLen >= 2) {
            // The first two bytes are the control bytes
            TUInt8 control0 = IReadByte(p);
            TUInt8 control1 = IReadByte(p);

            // Update the control0 lines
            /* Compare new line status to the old one, signal if different/
             N.B. packet may be processed more than once, but differences
             are only processed once.  */
            TUInt8 status = control0 & FTDI_STATUS_B0_MASK;
            if (status != p->iStatus) {
                char diff_status = status ^ p->iStatus;
                // Record the new status
                p->iStatus = status;

                // Update CTS
                if (diff_status & FTDI_RS0_CTS)
                    p->iHandshakingStatus |= SERIAL_HANDSHAKING_DELTA_CTS;
                if (status & FTDI_RS0_CTS)
                    p->iHandshakingStatus |= SERIAL_HANDSHAKING_CTS;
                else
                    p->iHandshakingStatus &= ~SERIAL_HANDSHAKING_CTS;

                // Update DSR
                if (diff_status & FTDI_RS0_DSR)
                    p->iHandshakingStatus |= SERIAL_HANDSHAKING_DELTA_DSR;
                if (status & FTDI_RS0_DSR)
                    p->iHandshakingStatus |= SERIAL_HANDSHAKING_DSR;
                else
                    p->iHandshakingStatus &= ~SERIAL_HANDSHAKING_DSR;

                // Update RI
                if (diff_status & FTDI_RS0_RI)
                    p->iHandshakingStatus |= SERIAL_HANDSHAKING_TRAILING_EDGE_RI;
                if (status & FTDI_RS0_RI)
                    p->iHandshakingStatus |= SERIAL_HANDSHAKING_RING;
                else
                    p->iHandshakingStatus &= ~SERIAL_HANDSHAKING_RING;

                // Update DCD
                if (diff_status & FTDI_RS0_RLSD)
                    p->iHandshakingStatus |= SERIAL_HANDSHAKING_DELTA_DCD;
                if (status & FTDI_RS0_RLSD)
                    p->iHandshakingStatus |= SERIAL_HANDSHAKING_DCD;
                else
                    p->iHandshakingStatus &= ~SERIAL_HANDSHAKING_DCD;
            }

            // Update the control1 lines
            if (control1 & FTDI_RS_ERR_MASK) {
                /* Break takes precedence over parity, which takes precedence
                 * over framing errors */
                if (control1 & FTDI_RS_BI) {
                    p->iBRK++;
                } else if (control1 & FTDI_RS_PE) {
                    p->iParityError++;
                } else if (control1 & FTDI_RS_FE) {
                    p->iFrameError++;
                }
                if (control1 & FTDI_RS_OE) {
                    p->iOverrunError++;
                }
            }

            /* note if the transmitter is empty or not */
            if (control1 & FTDI_RS_TEMT)
                p_serial->iState.iTransmitEmpty = 1;
            else
                p_serial->iState.iTransmitEmpty = 0;

            // Shove the bytes into the queue
            for (i=2; i<dataLen; i++) {
                // Stuff the data into queue one at a time overflowing if there is not enough room
                TUInt8 dataByte = IReadByte(p);
                UEZQueueSend(p->iQueueReceive, &dataByte, 0);
            }
        } else {
            // Got a short packet from the FTDI chip.  Must be at least 2!
            // ignore this
        }
    }
    UEZSemaphoreRelease(p->iSem);
    return activity;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FTDI_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the generic configuration with the given queue sizes and
 *      link to HAL Serial device.
 * Inputs:
 *      T_uezDeviceWorkspace *aWorkspace -- This hardware device
 *      TUInt32 aUnitAddress        -- Address of USB unit
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_FTDI_Configure(
            T_uezDeviceWorkspace *aWorkspace,
            TUInt8 aUnitAddress,
            TUInt32 aWriteBufferSize,
            TUInt32 aReadBufferSize)
{
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p =
            (T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *)aWorkspace;
    USB_ClassInfo_FTDI_Serial_Host_t *p_serial = &p->iInterface;
    T_uezError error;

    // Write buffer is currently not used.  No buffering on output
    p->iWriteBufferSize = aWriteBufferSize;

    // Create a receive queue
    p->iReadBufferSize = aReadBufferSize;
    error = UEZQueueCreate(aReadBufferSize, 1, &p->iQueueReceive);
    if (error)  {
#if UEZ_REGISTER
        UEZQueueSetName(p->iQueueReceive, "\0", "\0");
#endif
        UEZQueueDelete(p->iQueueReceive);
        p->iQueueReceive = 0;
        return error;
    }
#if UEZ_REGISTER
    else {
        UEZQueueSetName(p->iQueueReceive, "Receive", "FTDI");
    }
#endif

    p_serial->iConfig.DataINPipeDoubleBank = false;
    p_serial->iConfig.DataINPipeNumber = 1;
    p_serial->iConfig.DataOUTPipeDoubleBank = false;
    p_serial->iConfig.DataOUTPipeNumber = 2;
    p_serial->iConfig.PortNumber = aUnitAddress;

    // Initial state is 115200 baud
    p_serial->iState.LineEncoding.BaudRateBPS = 115200;
    p_serial->iState.LineEncoding.CharFormat = 0;
    p_serial->iState.LineEncoding.ParityType = 0;
    p_serial->iState.LineEncoding.DataBits = 8;

    extern void Chip_USB0_Init(void);
    extern void Chip_USB1_Init(void);
    p->iUnitAddress = aUnitAddress;
    if (aUnitAddress == 0) {
        Chip_USB0_Init();
    } else {
        Chip_USB1_Init();
    }
    G_workspaces[aUnitAddress] = p;

    USB_Init(aUnitAddress, USB_MODE_Host);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FTDI_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open the stream
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_FTDI_Open(void *aWorkspace)
{
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p = (T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FTDI_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the device.
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_FTDI_Close(void *aWorkspace)
{
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p = (T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FTDI_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Reading characters from the debug terminal is not possible. This
 *      is an output only stream. Therefore, we simulate a timeout to make
 *      this stream behave like a normal stream device.
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
 *      TUInt8 *aData             -- Place to store data
 *      TUInt32 aNumBytes         -- Number of bytes to read.
 *      TUInt32 *aNumBytesRead    -- Number of bytes actually read.  If
 *                                   timeout occurs, this value is less than
 *                                   aNumBytes.
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_FTDI_Read(
            void *aWorkspace,
            TUInt8 *aData,
            TUInt32 aNumBytes,
            TUInt32 *aNumBytesRead,
            TUInt32 aTimeout)
{
    // Decrement use count.  Are we done?
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p = (T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *)aWorkspace;
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

static T_uezError IPipeErrorToUEZError(uint8_t error)
{
    switch (error) {
        case PIPE_RWSTREAM_NoError:
            return UEZ_ERROR_NONE;
        case PIPE_RWSTREAM_PipeStalled:
            return UEZ_ERROR_STALL;
        case PIPE_RWSTREAM_DeviceDisconnected:
            return UEZ_ERROR_DEVICE_NOT_FOUND;
        case PIPE_RWSTREAM_Timeout:
            return UEZ_ERROR_TIMEOUT;
        case PIPE_RWSTREAM_IncompleteTransfer:
            return UEZ_ERROR_INCOMPLETE;
        default:
            return UEZ_ERROR_INTERNAL_ERROR;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FTDI_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write data out the segger SWO debug terminal.
 * Inputs:
 *      void *aWorkspace          -- This stream workspace
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
T_uezError Stream_LPCUSBLib_SerialHost_FTDI_Write(
            void *aWorkspace,
            TUInt8 *aData,
            TUInt32 aNumBytes,
            TUInt32 *aNumBytesWritten,
            TUInt32 aTimeout)
{
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p = (T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *)aWorkspace;
    USB_ClassInfo_FTDI_Serial_Host_t *p_serial = &p->iInterface;
    TUInt8 pipe_error;
    TUInt32 blockLen;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (aNumBytesWritten)
        *aNumBytesWritten = 0;

    TUInt32 startTime = UEZTickCounterGet();

    while (aNumBytes) {
        uint8_t portnum = p_serial->iConfig.PortNumber;
        blockLen = aNumBytes;

        if (blockLen > p_serial->iState.DataOUTPipeSize)
            blockLen = p_serial->iState.DataOUTPipeSize;

        Pipe_SelectPipe(portnum, PIPE_CONTROLPIPE);

        // Send out the data
        Pipe_SelectPipe(portnum, p_serial->iConfig.DataOUTPipeNumber);
        Pipe_Unfreeze();
        if ((pipe_error = Pipe_Write_Stream_LE(
                            portnum,
                            aData,
                            blockLen,
                            NULL)) != PIPE_RWSTREAM_NoError) {
            UEZSemaphoreRelease(p->iSem);
            return IPipeErrorToUEZError(pipe_error);
        }
        Pipe_ClearOUT(portnum);
        Pipe_WaitUntilReady(portnum);
        Pipe_Freeze();

        // Update the counters
        aData += blockLen;
        aNumBytes -= blockLen;
        if (aNumBytesWritten)
            *aNumBytesWritten += blockLen;

        // Check if we have a timeout doing all this data and there is more to go
        if (aNumBytes) {
            if (UEZTickCounterGetDelta(startTime) >= aTimeout) {
                UEZSemaphoreRelease(p->iSem);
                return UEZ_ERROR_TIMEOUT;
            }
        }
    }

    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FTDI_Control
 *---------------------------------------------------------------------------*
 * Description:
 *      Not supported by this device
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 *      TUInt32 aControl          -- Command to execute
 *      void *aControlData        -- Data passed in with the command
 * Outputs:
 *      T_uezError                 -- Error code. UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_FTDI_Control(
            void *aWorkspace,
            TUInt32 aControl,
            void *aControlData)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p = (T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    switch (aControl)  {
        case STREAM_CONTROL_SET_SERIAL_SETTINGS:
            error = ISetSerialSettings(p, (T_Serial_Settings *)aControlData);
            break;
        case STREAM_CONTROL_GET_SERIAL_SETTINGS:
            error = IGetSerialSettings(p, (T_Serial_Settings *)aControlData);
            break;
        case STREAM_CONTROL_CONTROL_HANDSHAKING:
            error = ISetHandshakingControl(p, (T_serialHandshakingControl *)aControlData);
            break;
        case STREAM_CONTROL_HANDSHAKING_STATUS:
            error = IGetHandshakingStatus(p, (T_serialHandshakingStatus *)aControlData);
            break;
        case STREAM_CONTROL_GET_READ_NUM_WAITING:
            error = UEZQueueGetCount(p->iQueueReceive, aControlData);
            break;
        default:
            error = UEZ_ERROR_ILLEGAL_OPERATION;
            break;
    }

    UEZSemaphoreRelease(p->iSem);
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FTDI_Flush
 *---------------------------------------------------------------------------*
 * Description:
 *      Not supported by this device.
 * Inputs:
 *      void *aWorkspace          -- This serial generic workspace
 *      TUInt32 aTimeout          -- Timeout to wait for flush to finish
 * Outputs:
 *      T_uezError                 -- Error code.  UEZ_ERROR_NOT_SUPPORTED
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_FTDI_Flush(void *aWorkspace)
{
    T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *p = (T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace *)aWorkspace;
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    UEZSemaphoreRelease(p->iSem);
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  Stream_LPCUSBLib_SerialHost_FTDI_FullDuplex_Stream_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create a full duplex serial stream to a HAL serial device and create
 *      the read and write buffers.
 * Inputs:
 *      const char *aName -- Name of this created driver
 *      const char *aSerialHALName -- Name of low level Serial HAL driver
 *      TUInt32 aWriteBufferSize -- Size of the write buffer size in bytes
 *      TUInt32 aReadBufferSize -- Size of the read buffer size in bytes
 * Outputs:
 *      T_uezError -- Error code.
 *---------------------------------------------------------------------------*/
T_uezError Stream_LPCUSBLib_SerialHost_FTDI_Create(
                const char *aName,
                TUInt8 aUnitAddress,
                TUInt32 aWriteBufferSize,
                TUInt32 aReadBufferSize)
{
    T_uezDeviceWorkspace *p_stream;
    extern const DEVICE_STREAM Stream_LPCUSBLib_SerialHost_FTDI_Interface;

    UEZ_LPCUSBLib_Host_Require(aUnitAddress, &G_HostCallbacks, 0);

    // Create serial stream device and link to HAL_Serial driver
    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Stream_LPCUSBLib_SerialHost_FTDI_Interface, 0,
            &p_stream);
    return Stream_LPCUSBLib_SerialHost_FTDI_Configure(p_stream, aUnitAddress, aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_STREAM Stream_LPCUSBLib_SerialHost_FTDI_Interface = {
    {
        // Generic device interface
        "Serial:LPCUSBLib:SerialHost",
        0x0100,
        Stream_LPCUSBLib_SerialHost_FTDI_InitializeWorkspace,
        sizeof(T_Stream_LPCUSBLib_SerialHost_FTDI_Workspace),
    },

    // Functions
    Stream_LPCUSBLib_SerialHost_FTDI_Open,
    Stream_LPCUSBLib_SerialHost_FTDI_Close,
    Stream_LPCUSBLib_SerialHost_FTDI_Control,
    Stream_LPCUSBLib_SerialHost_FTDI_Read,
    Stream_LPCUSBLib_SerialHost_FTDI_Write,
    Stream_LPCUSBLib_SerialHost_FTDI_Flush,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  uEZSerialGeneric.c
 *-------------------------------------------------------------------------*/
