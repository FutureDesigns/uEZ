/*
 * @brief Host mode driver for the library USB HID Class driver
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

#define  __INCLUDE_FROM_HID_DRIVER
#define  __INCLUDE_FROM_HID_HOST_C
#include "HIDClassHost.h"

uint8_t HID_Host_ConfigurePipes(USB_ClassInfo_HID_Host_t* const HIDInterfaceInfo,
                                uint16_t ConfigDescriptorSize,
                                void* ConfigDescriptorData)
{
	USB_Descriptor_Endpoint_t*  DataINEndpoint  = NULL;
	USB_Descriptor_Endpoint_t*  DataOUTEndpoint = NULL;
	USB_Descriptor_Interface_t* HIDInterface    = NULL;
	USB_HID_Descriptor_HID_t*   HIDDescriptor   = NULL;
	uint8_t portnum = HIDInterfaceInfo->Config.PortNumber;

	memset(&HIDInterfaceInfo->State, 0x00, sizeof(HIDInterfaceInfo->State));

	if (DESCRIPTOR_TYPE(ConfigDescriptorData) != DTYPE_Configuration)
	  return HID_ENUMERROR_InvalidConfigDescriptor;

	while (!(DataINEndpoint) || !(DataOUTEndpoint))
	{
		if (!(HIDInterface) ||
		    USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
		                              DCOMP_HID_Host_NextHIDInterfaceEndpoint) != DESCRIPTOR_SEARCH_COMP_Found)
		{
			if (DataINEndpoint || DataOUTEndpoint)
			  break;

			do
			{
				if (USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
				                              DCOMP_HID_Host_NextHIDInterface) != DESCRIPTOR_SEARCH_COMP_Found)
				{
					return HID_ENUMERROR_NoCompatibleInterfaceFound;
				}

				HIDInterface = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Interface_t);
			} while (HIDInterfaceInfo->Config.HIDInterfaceProtocol &&
					 (HIDInterface->Protocol != HIDInterfaceInfo->Config.HIDInterfaceProtocol));

			if (USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
			                              DCOMP_HID_Host_NextHIDDescriptor) != DESCRIPTOR_SEARCH_COMP_Found)
			{
				return HID_ENUMERROR_NoCompatibleInterfaceFound;
			}

			HIDDescriptor = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_HID_Descriptor_HID_t);

			DataINEndpoint  = NULL;
			DataOUTEndpoint = NULL;

			continue;
		}

		USB_Descriptor_Endpoint_t* EndpointData = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Endpoint_t);

		if ((EndpointData->EndpointAddress & ENDPOINT_DIR_MASK) == ENDPOINT_DIR_IN)
		  DataINEndpoint  = EndpointData;
		else
		  DataOUTEndpoint = EndpointData;
	}

	for (uint8_t PipeNum = 1; PipeNum < PIPE_TOTAL_PIPES; PipeNum++)
	{
		uint16_t Size;
		uint8_t  Type;
		uint8_t  Token;
		uint8_t  EndpointAddress;
		uint8_t  InterruptPeriod;
		bool     DoubleBanked;

		if (PipeNum == HIDInterfaceInfo->Config.DataINPipeNumber)
		{
			Size            = le16_to_cpu(DataINEndpoint->EndpointSize);
			EndpointAddress = DataINEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_IN;
			Type            = EP_TYPE_INTERRUPT;
			DoubleBanked    = HIDInterfaceInfo->Config.DataINPipeDoubleBank;
			InterruptPeriod = DataINEndpoint->PollingIntervalMS;

			HIDInterfaceInfo->State.DataINPipeSize = DataINEndpoint->EndpointSize;
		}
		else if (PipeNum == HIDInterfaceInfo->Config.DataOUTPipeNumber)
		{
			if (DataOUTEndpoint == NULL)
			  continue;
		
			Size            = le16_to_cpu(DataOUTEndpoint->EndpointSize);
			EndpointAddress = DataOUTEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_OUT;
			Type            = EP_TYPE_INTERRUPT;
			DoubleBanked    = HIDInterfaceInfo->Config.DataOUTPipeDoubleBank;
			InterruptPeriod = DataOUTEndpoint->PollingIntervalMS;

			HIDInterfaceInfo->State.DataOUTPipeSize   = DataOUTEndpoint->EndpointSize;
			HIDInterfaceInfo->State.DeviceUsesOUTPipe = true;
		}
		else
		{
			continue;
		}

		if (!(Pipe_ConfigurePipe(portnum,PipeNum, Type, Token, EndpointAddress, Size,
		                         DoubleBanked ? PIPE_BANK_DOUBLE : PIPE_BANK_SINGLE)))
		{
			return HID_ENUMERROR_PipeConfigurationFailed;
		}
		
		if (InterruptPeriod)
		  Pipe_SetInterruptPeriod(InterruptPeriod);
	}

	HIDInterfaceInfo->State.InterfaceNumber      = HIDInterface->InterfaceNumber;
	HIDInterfaceInfo->State.HIDReportSize        = LE16_TO_CPU(HIDDescriptor->HIDReportLength);
	HIDInterfaceInfo->State.SupportsBootProtocol = (HIDInterface->SubClass != HID_CSCP_NonBootProtocol);
	HIDInterfaceInfo->State.LargestReportSize    = 8;
	HIDInterfaceInfo->State.IsActive             = true;

	return HID_ENUMERROR_NoError;
}

static uint8_t DCOMP_HID_Host_NextHIDInterface(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Interface)
	{
		USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);

		if (Interface->Class == HID_CSCP_HIDClass)
		  return DESCRIPTOR_SEARCH_Found;
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

static uint8_t DCOMP_HID_Host_NextHIDDescriptor(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == HID_DTYPE_HID)
	  return DESCRIPTOR_SEARCH_Found;
	else if (Header->Type == DTYPE_Interface)
	  return DESCRIPTOR_SEARCH_Fail;
	else
	  return DESCRIPTOR_SEARCH_NotFound;
}

static uint8_t DCOMP_HID_Host_NextHIDInterfaceEndpoint(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Endpoint)
	{
		USB_Descriptor_Endpoint_t* Endpoint = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Endpoint_t);

		if (!(Pipe_IsEndpointBound(Endpoint->EndpointAddress)))
		  return DESCRIPTOR_SEARCH_Found;
	}
	else if (Header->Type == DTYPE_Interface)
	{
		return DESCRIPTOR_SEARCH_Fail;
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

#if !defined(HID_HOST_BOOT_PROTOCOL_ONLY)
uint8_t HID_Host_ReceiveReportByID(USB_ClassInfo_HID_Host_t* const HIDInterfaceInfo,
                                   const uint8_t ReportID,
                                   void* Buffer)
{
	USB_ControlRequest = (USB_Request_Header_t)
	{
		.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
		.bRequest      = HID_REQ_SetReport,
		.wValue        = ((HID_REPORT_ITEM_In + 1) << 8) | ReportID,
		.wIndex        = HIDInterfaceInfo->State.InterfaceNumber,
		.wLength       = USB_GetHIDReportSize(HIDInterfaceInfo->Config.HIDParserData, ReportID, HID_REPORT_ITEM_In),
	};
	uint8_t portnum = HIDInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,Buffer);
}
#endif

uint8_t HID_Host_ReceiveReport(USB_ClassInfo_HID_Host_t* const HIDInterfaceInfo,
                               void* Buffer)
{
	uint8_t portnum = HIDInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;
	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(HIDInterfaceInfo->State.IsActive))
	  return PIPE_READYWAIT_DeviceDisconnected;

	Pipe_SelectPipe(portnum,HIDInterfaceInfo->Config.DataINPipeNumber);
	Pipe_Unfreeze();

	uint16_t ReportSize;
	uint8_t* BufferPos = Buffer;

#if !defined(HID_HOST_BOOT_PROTOCOL_ONLY)
	if (!(HIDInterfaceInfo->State.UsingBootProtocol))
	{
		uint8_t ReportID = 0;

		if (HIDInterfaceInfo->Config.HIDParserData->UsingReportIDs)
		{
			ReportID = Pipe_Read_8(portnum);
			*(BufferPos++) = ReportID;
		}

		ReportSize = USB_GetHIDReportSize(HIDInterfaceInfo->Config.HIDParserData, ReportID, HID_REPORT_ITEM_In);
	}
	else
#endif
	{
		ReportSize = Pipe_BytesInPipe(portnum);
	}

	if ((ErrorCode = Pipe_Read_Stream_LE(portnum,BufferPos, ReportSize, NULL)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;

	Pipe_ClearIN(portnum);
	Pipe_Freeze();

	return PIPE_RWSTREAM_NoError;
}

uint8_t HID_Host_SendReportByID(USB_ClassInfo_HID_Host_t* const HIDInterfaceInfo,
#if !defined(HID_HOST_BOOT_PROTOCOL_ONLY)
                                const uint8_t ReportID,
#endif
                                const uint8_t ReportType,
                                void* Buffer,
                                const uint16_t ReportSize)
{
	uint8_t portnum = HIDInterfaceInfo->Config.PortNumber;
#if !defined(HID_HOST_BOOT_PROTOCOL_ONLY)
	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(HIDInterfaceInfo->State.IsActive))
	  return false;

	if (HIDInterfaceInfo->State.DeviceUsesOUTPipe && (ReportType == HID_REPORT_ITEM_Out))
	{
		uint8_t ErrorCode;

		Pipe_SelectPipe(portnum,HIDInterfaceInfo->Config.DataOUTPipeNumber);
		Pipe_Unfreeze();

		if (ReportID)
		  Pipe_Write_Stream_LE(portnum,&ReportID, sizeof(ReportID), NULL);

		if ((ErrorCode = Pipe_Write_Stream_LE(portnum,Buffer, ReportSize, NULL)) != PIPE_RWSTREAM_NoError)
		  return ErrorCode;

		Pipe_ClearOUT(portnum);
		Pipe_Freeze();

		return PIPE_RWSTREAM_NoError;
	}
	else
#endif
	{
		USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = HID_REQ_SetReport,
#if !defined(HID_HOST_BOOT_PROTOCOL_ONLY)
			.wValue        = ((ReportType + 1) << 8) | ReportID,
#else
			.wValue        = ((ReportType + 1) << 8),
#endif
			.wIndex        = HIDInterfaceInfo->State.InterfaceNumber,
			.wLength       = ReportSize,
		};

		Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

		return USB_Host_SendControlRequest(portnum,Buffer);
	}
}

bool HID_Host_IsReportReceived(USB_ClassInfo_HID_Host_t* const HIDInterfaceInfo)
{
	uint8_t portnum = HIDInterfaceInfo->Config.PortNumber;
	bool ReportReceived;

	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(HIDInterfaceInfo->State.IsActive))
	  return false;

	Pipe_SelectPipe(portnum,HIDInterfaceInfo->Config.DataINPipeNumber);
	Pipe_Unfreeze();

	ReportReceived = Pipe_IsINReceived(portnum);

	Pipe_Freeze();

	return ReportReceived;
}

uint8_t HID_Host_SetBootProtocol(USB_ClassInfo_HID_Host_t* const HIDInterfaceInfo)
{
	uint8_t ErrorCode;
	uint8_t portnum = HIDInterfaceInfo->Config.PortNumber;

	if (!(HIDInterfaceInfo->State.SupportsBootProtocol))
	  return HID_ERROR_LOGICAL;

	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = HID_REQ_SetProtocol,
			.wValue        = 0,
			.wIndex        = HIDInterfaceInfo->State.InterfaceNumber,
			.wLength       = 0,
		};

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	if ((ErrorCode = USB_Host_SendControlRequest(portnum,NULL)) != HOST_SENDCONTROL_Successful)
	  return ErrorCode;

	HIDInterfaceInfo->State.LargestReportSize = 8;
	HIDInterfaceInfo->State.UsingBootProtocol = true;

	return HOST_SENDCONTROL_Successful;
}

uint8_t HID_Host_SetIdlePeriod(USB_ClassInfo_HID_Host_t* const HIDInterfaceInfo,
                               const uint16_t MS)
{
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = HID_REQ_SetIdle,
			.wValue        = ((MS << 6) & 0xFF00),
			.wIndex        = HIDInterfaceInfo->State.InterfaceNumber,
			.wLength       = 0,
		};
	uint8_t portnum = HIDInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,NULL);
}

#if !defined(HID_HOST_BOOT_PROTOCOL_ONLY)
uint8_t HID_Host_SetReportProtocol(USB_ClassInfo_HID_Host_t* const HIDInterfaceInfo)
{
	uint8_t ErrorCode;
	uint8_t portnum = HIDInterfaceInfo->Config.PortNumber;
	//uint8_t HIDReportData[HIDInterfaceInfo->State.HIDReportSize];
    uint8_t HIDReportData[128];

	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_STANDARD | REQREC_INTERFACE),
			.bRequest      = REQ_GetDescriptor,
			.wValue        = (HID_DTYPE_Report << 8),
			.wIndex        = HIDInterfaceInfo->State.InterfaceNumber,
			.wLength       = HIDInterfaceInfo->State.HIDReportSize,
		};

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	if ((ErrorCode = USB_Host_SendControlRequest(portnum,HIDReportData)) != HOST_SENDCONTROL_Successful)
	  return ErrorCode;

	if (HIDInterfaceInfo->State.UsingBootProtocol)
	{
		USB_ControlRequest = (USB_Request_Header_t)
			{
				.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
				.bRequest      = HID_REQ_SetProtocol,
				.wValue        = 1,
				.wIndex        = HIDInterfaceInfo->State.InterfaceNumber,
				.wLength       = 0,
			};

		if ((ErrorCode = USB_Host_SendControlRequest(portnum,NULL)) != HOST_SENDCONTROL_Successful)
		  return ErrorCode;

		HIDInterfaceInfo->State.UsingBootProtocol = false;
	}

	if (HIDInterfaceInfo->Config.HIDParserData == NULL)
	  return HID_ERROR_LOGICAL;

	if ((ErrorCode = USB_ProcessHIDReport(HIDReportData, HIDInterfaceInfo->State.HIDReportSize,
	                                      HIDInterfaceInfo->Config.HIDParserData)) != HID_PARSE_Successful)
	{
		return HID_ERROR_LOGICAL | ErrorCode;
	}

	uint8_t LargestReportSizeBits = HIDInterfaceInfo->Config.HIDParserData->LargestReportSizeBits;
	HIDInterfaceInfo->State.LargestReportSize = (LargestReportSizeBits >> 3) + ((LargestReportSizeBits & 0x07) != 0);

	return 0;
}
#endif

#endif

