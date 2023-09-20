/*
 * @brief Host mode driver for the library USB CDC Class driver
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * Copyright(C) Dean Camera, 2011, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */


#define  __INCLUDE_FROM_USB_DRIVER
#include "../../Core/USBMode.h"

#if defined(USB_CAN_BE_HOST)

#define  __INCLUDE_FROM_CDC_DRIVER
#define  __INCLUDE_FROM_CDC_HOST_C
#include "CDCClassHost.h"

uint8_t CDC_Host_ConfigurePipes(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo,
                                uint16_t ConfigDescriptorSize,
                                void* ConfigDescriptorData)
{
	USB_Descriptor_Endpoint_t*  DataINEndpoint       = NULL;
	USB_Descriptor_Endpoint_t*  DataOUTEndpoint      = NULL;
	USB_Descriptor_Endpoint_t*  NotificationEndpoint = NULL;
	USB_Descriptor_Interface_t* CDCControlInterface  = NULL;
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	memset(&CDCInterfaceInfo->State, 0x00, sizeof(CDCInterfaceInfo->State));

	if (DESCRIPTOR_TYPE(ConfigDescriptorData) != DTYPE_Configuration)
	  return CDC_ENUMERROR_InvalidConfigDescriptor;

	while (!(DataINEndpoint) || !(DataOUTEndpoint) || !(NotificationEndpoint))
	{
		if (!(CDCControlInterface) ||
		    USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
		                              DCOMP_CDC_Host_NextCDCInterfaceEndpoint) != DESCRIPTOR_SEARCH_COMP_Found)
		{
			if (NotificationEndpoint)
			{
				if (USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
				                              DCOMP_CDC_Host_NextCDCDataInterface) != DESCRIPTOR_SEARCH_COMP_Found)
				{
					return CDC_ENUMERROR_NoCompatibleInterfaceFound;
				}

				DataINEndpoint  = NULL;
				DataOUTEndpoint = NULL;
			}
			else
			{
				if (USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
				                              DCOMP_CDC_Host_NextCDCControlInterface) != DESCRIPTOR_SEARCH_COMP_Found)
				{
					return CDC_ENUMERROR_NoCompatibleInterfaceFound;
				}

				CDCControlInterface = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Interface_t);

				NotificationEndpoint = NULL;
			}

			continue;
		}

		USB_Descriptor_Endpoint_t* EndpointData = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t);

		if ((EndpointData->EndpointAddress & ENDPOINT_DIR_MASK) == ENDPOINT_DIR_IN)
		{
			if ((EndpointData->Attributes & EP_TYPE_MASK) == EP_TYPE_INTERRUPT)
			  NotificationEndpoint = EndpointData;
			else
			  DataINEndpoint = EndpointData;
		}
		else
		{
			DataOUTEndpoint = EndpointData;
		}
	}

	for (uint8_t PipeNum = 1; PipeNum < PIPE_TOTAL_PIPES; PipeNum++)
	{
		uint16_t Size;
		uint8_t  Type;
		uint8_t  Token;
		uint8_t  EndpointAddress;
		uint8_t  InterruptPeriod;
		bool     DoubleBanked;

		if (PipeNum == CDCInterfaceInfo->Config.DataINPipeNumber)
		{
			Size            = le16_to_cpu(DataINEndpoint->EndpointSize);
			EndpointAddress = DataINEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_IN;
			Type            = EP_TYPE_BULK;
			DoubleBanked    = CDCInterfaceInfo->Config.DataINPipeDoubleBank;
			InterruptPeriod = 0;

			CDCInterfaceInfo->State.DataINPipeSize = DataINEndpoint->EndpointSize;
		}
		else if (PipeNum == CDCInterfaceInfo->Config.DataOUTPipeNumber)
		{
			Size            = le16_to_cpu(DataOUTEndpoint->EndpointSize);
			EndpointAddress = DataOUTEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_OUT;
			Type            = EP_TYPE_BULK;
			DoubleBanked    = CDCInterfaceInfo->Config.DataOUTPipeDoubleBank;
			InterruptPeriod = 0;

			CDCInterfaceInfo->State.DataOUTPipeSize = DataOUTEndpoint->EndpointSize;
		}
		else if (PipeNum == CDCInterfaceInfo->Config.NotificationPipeNumber)
		{
			Size            = le16_to_cpu(NotificationEndpoint->EndpointSize);
			EndpointAddress = NotificationEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_IN;
			Type            = EP_TYPE_INTERRUPT;
			DoubleBanked    = CDCInterfaceInfo->Config.NotificationPipeDoubleBank;
			InterruptPeriod = NotificationEndpoint->PollingIntervalMS;

			CDCInterfaceInfo->State.NotificationPipeSize = NotificationEndpoint->EndpointSize;
		}
		else
		{
			continue;
		}
		
		if (!(Pipe_ConfigurePipe(portnum,PipeNum, Type, Token, EndpointAddress, Size,
		                         DoubleBanked ? PIPE_BANK_DOUBLE : PIPE_BANK_SINGLE)))
		{
			return CDC_ENUMERROR_PipeConfigurationFailed;
		}
		
		if (InterruptPeriod)
		  Pipe_SetInterruptPeriod(InterruptPeriod);
	}

	CDCInterfaceInfo->State.ControlInterfaceNumber = CDCControlInterface->InterfaceNumber;
	CDCInterfaceInfo->State.ControlLineStates.HostToDevice = (CDC_CONTROL_LINE_OUT_RTS | CDC_CONTROL_LINE_OUT_DTR);
	CDCInterfaceInfo->State.ControlLineStates.DeviceToHost = (CDC_CONTROL_LINE_IN_DCD  | CDC_CONTROL_LINE_IN_DSR);
	CDCInterfaceInfo->State.IsActive = true;

	return CDC_ENUMERROR_NoError;
}

uint8_t CDC_Host_FindConfigInterface(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo,
                                uint16_t * ConfigDescriptorSize,
                                void** ConfigDescriptorData)
{
	USB_Descriptor_Endpoint_t*  DataINEndpoint       = NULL;
	USB_Descriptor_Endpoint_t*  DataOUTEndpoint      = NULL;
	USB_Descriptor_Endpoint_t*  NotificationEndpoint = NULL;
	USB_Descriptor_Interface_t* CDCControlInterface  = NULL;
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	memset(&CDCInterfaceInfo->State, 0x00, sizeof(CDCInterfaceInfo->State));

	while (!(DataINEndpoint) || !(DataOUTEndpoint) || !(NotificationEndpoint))
	{
		if (!(CDCControlInterface) ||
		    USB_GetNextDescriptorComp(ConfigDescriptorSize, ConfigDescriptorData,
		                              DCOMP_CDC_Host_NextCDCInterfaceEndpoint) != DESCRIPTOR_SEARCH_COMP_Found)
		{
			if (NotificationEndpoint)
			{
				if (USB_GetNextDescriptorComp(ConfigDescriptorSize, ConfigDescriptorData,
				                              DCOMP_CDC_Host_NextCDCDataInterface) != DESCRIPTOR_SEARCH_COMP_Found)
				{
					return CDC_ENUMERROR_NoCompatibleInterfaceFound;
				}

				DataINEndpoint  = NULL;
				DataOUTEndpoint = NULL;
			}
			else
			{
				if (USB_GetNextDescriptorComp(ConfigDescriptorSize, ConfigDescriptorData,
				                              DCOMP_CDC_Host_NextCDCControlInterface) != DESCRIPTOR_SEARCH_COMP_Found)
				{
					return CDC_ENUMERROR_NoCompatibleInterfaceFound;
				}

				CDCControlInterface = DESCRIPTOR_PCAST(*ConfigDescriptorData, USB_Descriptor_Interface_t);

				NotificationEndpoint = NULL;
			}

			continue;
		}

		USB_Descriptor_Endpoint_t* EndpointData = DESCRIPTOR_PCAST(*ConfigDescriptorData, USB_Descriptor_Endpoint_t);

		if ((EndpointData->EndpointAddress & ENDPOINT_DIR_MASK) == ENDPOINT_DIR_IN)
		{
			if ((EndpointData->Attributes & EP_TYPE_MASK) == EP_TYPE_INTERRUPT)
			  NotificationEndpoint = EndpointData;
			else
			  DataINEndpoint = EndpointData;
		}
		else
		{
			DataOUTEndpoint = EndpointData;
		}
	}

	for (uint8_t PipeNum = 1; PipeNum < PIPE_TOTAL_PIPES; PipeNum++)
	{
		uint16_t Size;
		uint8_t  Type;
		uint8_t  Token;
		uint8_t  EndpointAddress;
		uint8_t  InterruptPeriod;
		bool     DoubleBanked;

		if (PipeNum == CDCInterfaceInfo->Config.DataINPipeNumber)
		{
			Size            = le16_to_cpu(DataINEndpoint->EndpointSize);
			EndpointAddress = DataINEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_IN;
			Type            = EP_TYPE_BULK;
			DoubleBanked    = CDCInterfaceInfo->Config.DataINPipeDoubleBank;
			InterruptPeriod = 0;

			CDCInterfaceInfo->State.DataINPipeSize = DataINEndpoint->EndpointSize;
		}
		else if (PipeNum == CDCInterfaceInfo->Config.DataOUTPipeNumber)
		{
			Size            = le16_to_cpu(DataOUTEndpoint->EndpointSize);
			EndpointAddress = DataOUTEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_OUT;
			Type            = EP_TYPE_BULK;
			DoubleBanked    = CDCInterfaceInfo->Config.DataOUTPipeDoubleBank;
			InterruptPeriod = 0;

			CDCInterfaceInfo->State.DataOUTPipeSize = DataOUTEndpoint->EndpointSize;
		}
		else if (PipeNum == CDCInterfaceInfo->Config.NotificationPipeNumber)
		{
			Size            = le16_to_cpu(NotificationEndpoint->EndpointSize);
			EndpointAddress = NotificationEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_IN;
			Type            = EP_TYPE_INTERRUPT;
			DoubleBanked    = CDCInterfaceInfo->Config.NotificationPipeDoubleBank;
			InterruptPeriod = NotificationEndpoint->PollingIntervalMS;

			CDCInterfaceInfo->State.NotificationPipeSize = NotificationEndpoint->EndpointSize;
		}
		else
		{
			continue;
		}
		
		if (!(Pipe_ConfigurePipe(portnum,PipeNum, Type, Token, EndpointAddress, Size,
		                         DoubleBanked ? PIPE_BANK_DOUBLE : PIPE_BANK_SINGLE)))
		{
			return CDC_ENUMERROR_PipeConfigurationFailed;
		}
		
		if (InterruptPeriod)
		  Pipe_SetInterruptPeriod(InterruptPeriod);
	}

	CDCInterfaceInfo->State.ControlInterfaceNumber = CDCControlInterface->InterfaceNumber;
	CDCInterfaceInfo->State.ControlLineStates.HostToDevice = (CDC_CONTROL_LINE_OUT_RTS | CDC_CONTROL_LINE_OUT_DTR);
	CDCInterfaceInfo->State.ControlLineStates.DeviceToHost = (CDC_CONTROL_LINE_IN_DCD  | CDC_CONTROL_LINE_IN_DSR);
	CDCInterfaceInfo->State.IsActive = true;

	return CDC_ENUMERROR_NoError;
}

static uint8_t DCOMP_CDC_Host_NextCDCControlInterface(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Interface)
	{
		USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);

		if ((Interface->Class    == CDC_CSCP_CDCClass)    &&
		    (Interface->SubClass == CDC_CSCP_ACMSubclass) &&
		    (Interface->Protocol == CDC_CSCP_ATCommandProtocol))
		{
			return DESCRIPTOR_SEARCH_Found;
		}

                // Accept vender specific
		if ((Interface->Class    == 0xFF)    &&
		    (Interface->SubClass == 0x00) &&
		    (Interface->Protocol == 0x00))
		{
			return DESCRIPTOR_SEARCH_Found;
		}
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

static uint8_t DCOMP_CDC_Host_NextCDCDataInterface(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Interface)
	{
		USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);

		if ((Interface->Class    == CDC_CSCP_CDCDataClass)   &&
		    (Interface->SubClass == CDC_CSCP_NoDataSubclass) &&
		    (Interface->Protocol == CDC_CSCP_NoDataProtocol))
		{
			return DESCRIPTOR_SEARCH_Found;
		}
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

static uint8_t DCOMP_CDC_Host_NextCDCInterfaceEndpoint(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Endpoint)
	{
		USB_Descriptor_Endpoint_t* Endpoint = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Endpoint_t);

		uint8_t EndpointType = (Endpoint->Attributes & EP_TYPE_MASK);

		if (((EndpointType == EP_TYPE_BULK) || (EndpointType == EP_TYPE_INTERRUPT)) &&
		    !(Pipe_IsEndpointBound(Endpoint->EndpointAddress)))
		{
			return DESCRIPTOR_SEARCH_Found;
		}
	}
	else if (Header->Type == DTYPE_Interface)
	{
		return DESCRIPTOR_SEARCH_Fail;
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

void CDC_Host_USBTask(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
	  return;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.NotificationPipeNumber);
	Pipe_Unfreeze();

	if (Pipe_IsINReceived(portnum))
	{
		USB_Request_Header_t Notification;
		Pipe_Read_Stream_LE(portnum,&Notification, sizeof(USB_Request_Header_t), NULL);

		if ((Notification.bRequest      == CDC_NOTIF_SerialState) &&
		    (Notification.bmRequestType == (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE)))
		{
			Pipe_Read_Stream_LE(portnum,
								&CDCInterfaceInfo->State.ControlLineStates.DeviceToHost,
			                    sizeof(CDCInterfaceInfo->State.ControlLineStates.DeviceToHost),
			                    NULL);

			Pipe_ClearIN(portnum);

			EVENT_CDC_Host_ControLineStateChanged(CDCInterfaceInfo);
		}
		else
		{
			Pipe_ClearIN(portnum);
		}
	}

	Pipe_Freeze();

	#if !defined(NO_CLASS_DRIVER_AUTOFLUSH)
	CDC_Host_Flush(CDCInterfaceInfo);
	#endif
}

uint8_t CDC_Host_SetLineEncoding(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo)
{
	USB_ControlRequest = (USB_Request_Header_t)
	{
		.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
		.bRequest      = CDC_REQ_SetLineEncoding,
		.wValue        = 0,
		.wIndex        = CDCInterfaceInfo->State.ControlInterfaceNumber,
		.wLength       = sizeof(CDCInterfaceInfo->State.LineEncoding),
	};
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,&CDCInterfaceInfo->State.LineEncoding);
}

uint8_t CDC_Host_SendControlLineStateChange(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo)
{
	USB_ControlRequest = (USB_Request_Header_t)
	{
		.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
		.bRequest      = CDC_REQ_SetControlLineState,
		.wValue        = CDCInterfaceInfo->State.ControlLineStates.HostToDevice,
		.wIndex        = CDCInterfaceInfo->State.ControlInterfaceNumber,
		.wLength       = 0,
	};
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,NULL);
}

uint8_t CDC_Host_SendBreak(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo,
                           const uint8_t Duration)
{
	USB_ControlRequest = (USB_Request_Header_t)
	{
		.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
		.bRequest      = CDC_REQ_SendBreak,
		.wValue        = Duration,
		.wIndex        = CDCInterfaceInfo->State.ControlInterfaceNumber,
		.wLength       = 0,
	};
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,NULL);
}

uint8_t CDC_Host_SendData(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo,
                          const uint8_t* const Buffer,
                          const uint16_t Length)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
	  return PIPE_READYWAIT_DeviceDisconnected;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataOUTPipeNumber);

	Pipe_Unfreeze();
	ErrorCode = Pipe_Write_Stream_LE(portnum,Buffer, Length, NULL);
	Pipe_Freeze();

	return ErrorCode;
}

uint8_t CDC_Host_SendString(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo,
                            const char* const String)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
	  return PIPE_READYWAIT_DeviceDisconnected;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataOUTPipeNumber);

	Pipe_Unfreeze();
	ErrorCode = Pipe_Write_Stream_LE(portnum,String, strlen(String), NULL);
	Pipe_Freeze();

	return ErrorCode;
}

uint8_t CDC_Host_SendByte(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo,
                          const uint8_t Data)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
	  return PIPE_READYWAIT_DeviceDisconnected;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataOUTPipeNumber);
	Pipe_Unfreeze();

	if (!(Pipe_IsReadWriteAllowed(portnum)))
	{
		Pipe_ClearOUT(portnum);

		if ((ErrorCode = Pipe_WaitUntilReady(portnum)) != PIPE_READYWAIT_NoError)
		  return ErrorCode;
	}

	Pipe_Write_8(portnum,Data);
	Pipe_Freeze();

	return PIPE_READYWAIT_NoError;
}

uint16_t CDC_Host_BytesReceived(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
	  return 0;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataINPipeNumber);
	Pipe_Unfreeze();

	if (Pipe_IsINReceived(portnum))
	{
		if (!(Pipe_BytesInPipe(portnum)))
		{
			Pipe_ClearIN(portnum);
			Pipe_Freeze();
			return 0;
		}
		else
		{
			Pipe_Freeze();
			return Pipe_BytesInPipe(portnum);
		}
	}
	else
	{
		Pipe_Freeze();

		return 0;
	}
}

int16_t CDC_Host_ReceiveByte(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	int16_t ReceivedByte = -1;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
	  return -1;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataINPipeNumber);
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

uint8_t CDC_Host_Flush(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo)
{
	uint8_t portnum = CDCInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(CDCInterfaceInfo->State.IsActive))
	  return PIPE_READYWAIT_DeviceDisconnected;

	Pipe_SelectPipe(portnum,CDCInterfaceInfo->Config.DataOUTPipeNumber);
	Pipe_Unfreeze();

	if (!(Pipe_BytesInPipe(portnum)))
	  return PIPE_READYWAIT_NoError;

	bool BankFull = !(Pipe_IsReadWriteAllowed(portnum));

	Pipe_ClearOUT(portnum);

	if (BankFull)
	{
		if ((ErrorCode = Pipe_WaitUntilReady(portnum)) != PIPE_READYWAIT_NoError)
		  return ErrorCode;

		Pipe_ClearOUT(portnum);
	}

	Pipe_Freeze();

	return PIPE_READYWAIT_NoError;
}

#if (defined(FDEV_SETUP_STREAM) && (!defined(__IAR_SYSTEMS_ICC__) || (_DLIB_FILE_DESCRIPTOR == 1)))
void CDC_Host_CreateStream(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo,
                           FILE* const Stream)
{
	*Stream = (FILE)FDEV_SETUP_STREAM(CDC_Host_putchar, CDC_Host_getchar, _FDEV_SETUP_RW);
	fdev_set_udata(Stream, CDCInterfaceInfo);
}

void CDC_Host_CreateBlockingStream(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo,
                                   FILE* const Stream)
{
	*Stream = (FILE)FDEV_SETUP_STREAM(CDC_Host_putchar, CDC_Host_getchar_Blocking, _FDEV_SETUP_RW);
	fdev_set_udata(Stream, CDCInterfaceInfo);
}

static int32_t CDC_Host_putchar(char c,
                            FILE* Stream)
{
	return CDC_Host_SendByte((USB_ClassInfo_CDC_Host_t*)fdev_get_udata(Stream), c) ? _FDEV_ERR : 0;
}

static int32_t CDC_Host_getchar(FILE* Stream)
{
	int16_t ReceivedByte = CDC_Host_ReceiveByte((USB_ClassInfo_CDC_Host_t*)fdev_get_udata(Stream));

	if (ReceivedByte < 0)
	  return _FDEV_EOF;

	return ReceivedByte;
}

static int32_t CDC_Host_getchar_Blocking(FILE* Stream)
{
	int16_t ReceivedByte;

	while ((ReceivedByte = CDC_Host_ReceiveByte((USB_ClassInfo_CDC_Host_t*)fdev_get_udata(Stream))) < 0)
	{
		if (USB_HostState == HOST_STATE_Unattached)
		  return _FDEV_EOF;

		CDC_Host_USBTask((USB_ClassInfo_CDC_Host_t*)fdev_get_udata(Stream));
		USB_USBTask();
	}

	return ReceivedByte;
}
#endif

void CDC_Host_Event_Stub(USB_ClassInfo_CDC_Host_t* const CDCInterfaceInfo)
{

}

#endif

