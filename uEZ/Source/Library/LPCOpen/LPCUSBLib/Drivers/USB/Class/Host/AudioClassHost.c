/*
 * @brief Host mode driver for the library USB Audio 1.0 Class driver
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

#define  __INCLUDE_FROM_AUDIO_DRIVER
#define  __INCLUDE_FROM_AUDIO_HOST_C
#include "AudioClassHost.h"

uint8_t Audio_Host_ConfigurePipes(USB_ClassInfo_Audio_Host_t* const AudioInterfaceInfo,
                                  uint16_t ConfigDescriptorSize,
                                  void* ConfigDescriptorData)
{
	USB_Descriptor_Endpoint_t*  DataINEndpoint          = NULL;
	USB_Descriptor_Endpoint_t*  DataOUTEndpoint         = NULL;
	USB_Descriptor_Interface_t* AudioControlInterface   = NULL;
	USB_Descriptor_Interface_t* AudioStreamingInterface = NULL;
	uint8_t portnum = AudioInterfaceInfo->Config.PortNumber;

	memset(&AudioInterfaceInfo->State, 0x00, sizeof(AudioInterfaceInfo->State));

	if (DESCRIPTOR_TYPE(ConfigDescriptorData) != DTYPE_Configuration)
	  return AUDIO_ENUMERROR_InvalidConfigDescriptor;

	while ((AudioInterfaceInfo->Config.DataINPipeNumber  && !(DataINEndpoint)) ||
	       (AudioInterfaceInfo->Config.DataOUTPipeNumber && !(DataOUTEndpoint)))
	{
		if (!(AudioControlInterface) ||
		    USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
		                              DCOMP_Audio_Host_NextAudioInterfaceDataEndpoint) != DESCRIPTOR_SEARCH_COMP_Found)
		{
			if (!(AudioControlInterface) ||
			    USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
			                              DCOMP_Audio_Host_NextAudioStreamInterface) != DESCRIPTOR_SEARCH_COMP_Found)
			{
				if (USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
				                              DCOMP_Audio_Host_NextAudioControlInterface) != DESCRIPTOR_SEARCH_COMP_Found)
				{
					return AUDIO_ENUMERROR_NoCompatibleInterfaceFound;
				}

				AudioControlInterface = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Interface_t);			

				if (USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
				                              DCOMP_Audio_Host_NextAudioStreamInterface) != DESCRIPTOR_SEARCH_COMP_Found)
				{
					return AUDIO_ENUMERROR_NoCompatibleInterfaceFound;
				}
			}

			AudioStreamingInterface = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Interface_t);
			
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
		bool     DoubleBanked;

		if (PipeNum == AudioInterfaceInfo->Config.DataINPipeNumber)
		{
			Size            = le16_to_cpu(DataINEndpoint->EndpointSize);
			EndpointAddress = DataINEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_IN;
			Type            = EP_TYPE_ISOCHRONOUS;
			DoubleBanked    = true;

			AudioInterfaceInfo->State.DataINPipeSize = DataINEndpoint->EndpointSize;
		}
		else if (PipeNum == AudioInterfaceInfo->Config.DataOUTPipeNumber)
		{
			Size            = le16_to_cpu(DataOUTEndpoint->EndpointSize);
			EndpointAddress = DataOUTEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_OUT;
			Type            = EP_TYPE_ISOCHRONOUS;
			DoubleBanked    = true;

			AudioInterfaceInfo->State.DataOUTPipeSize = DataOUTEndpoint->EndpointSize;
		}
		else
		{
			continue;
		}
		
		if (!(Pipe_ConfigurePipe(portnum,PipeNum, Type, Token, EndpointAddress, Size,
		                         DoubleBanked ? PIPE_BANK_DOUBLE : PIPE_BANK_SINGLE)))
		{
			return AUDIO_ENUMERROR_PipeConfigurationFailed;
		}
	}

	AudioInterfaceInfo->State.ControlInterfaceNumber    = AudioControlInterface->InterfaceNumber;
	AudioInterfaceInfo->State.StreamingInterfaceNumber  = AudioStreamingInterface->InterfaceNumber;
	AudioInterfaceInfo->State.EnabledStreamingAltIndex  = AudioStreamingInterface->AlternateSetting;
	AudioInterfaceInfo->State.IsActive = true;
	
	return AUDIO_ENUMERROR_NoError;
}

static uint8_t DCOMP_Audio_Host_NextAudioControlInterface(void* CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Interface)
	{
		USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);

		if ((Interface->Class    == AUDIO_CSCP_AudioClass) &&
		    (Interface->SubClass == AUDIO_CSCP_ControlSubclass) &&
		    (Interface->Protocol == AUDIO_CSCP_ControlProtocol))
		{
			return DESCRIPTOR_SEARCH_Found;
		}
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

static uint8_t DCOMP_Audio_Host_NextAudioStreamInterface(void* CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Interface)
	{
		USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);

		if ((Interface->Class    == AUDIO_CSCP_AudioClass) &&
		    (Interface->SubClass == AUDIO_CSCP_AudioStreamingSubclass) &&
		    (Interface->Protocol == AUDIO_CSCP_StreamingProtocol))
		{
			return DESCRIPTOR_SEARCH_Found;
		}
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

static uint8_t DCOMP_Audio_Host_NextAudioInterfaceDataEndpoint(void* CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Endpoint)
	{
		USB_Descriptor_Endpoint_t* Endpoint = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Endpoint_t);

		if ((Endpoint->Attributes & EP_TYPE_MASK) == EP_TYPE_ISOCHRONOUS)
		  return DESCRIPTOR_SEARCH_Found;
	}
	else if (Header->Type == DTYPE_Interface)
	{
		return DESCRIPTOR_SEARCH_Fail;
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

uint8_t Audio_Host_StartStopStreaming(USB_ClassInfo_Audio_Host_t* const AudioInterfaceInfo,
			                          const bool EnableStreaming)
{
	if (!(AudioInterfaceInfo->State.IsActive))
	  return HOST_SENDCONTROL_DeviceDisconnected;

	return USB_Host_SetInterfaceAltSetting(AudioInterfaceInfo->Config.PortNumber,
										   AudioInterfaceInfo->State.StreamingInterfaceNumber,
	                                       EnableStreaming ? AudioInterfaceInfo->State.EnabledStreamingAltIndex : 0);
}

uint8_t Audio_Host_GetSetEndpointProperty(USB_ClassInfo_Audio_Host_t* const AudioInterfaceInfo,
			                              const uint8_t DataPipeIndex,
			                              const uint8_t EndpointProperty,
			                              const uint8_t EndpointControl,
			                              const uint16_t DataLength,
			                              void* const Data)
{
	uint8_t portnum = AudioInterfaceInfo->Config.PortNumber;

	uint8_t RequestType;
	uint8_t EndpointAddress;

	if (!(AudioInterfaceInfo->State.IsActive))
	  return HOST_SENDCONTROL_DeviceDisconnected;
	if (EndpointProperty & 0x80)
	  RequestType = (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_ENDPOINT);
	else
	  RequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_ENDPOINT);
	  
	Pipe_SelectPipe(portnum,DataPipeIndex);
	EndpointAddress = Pipe_GetBoundEndpointAddress(portnum);

	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = RequestType,
			.bRequest      = EndpointProperty,
			.wValue        = ((uint16_t)EndpointControl << 8),
			.wIndex        = EndpointAddress,
			.wLength       = DataLength,
		};

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	return USB_Host_SendControlRequest(portnum,Data);
}

#endif

