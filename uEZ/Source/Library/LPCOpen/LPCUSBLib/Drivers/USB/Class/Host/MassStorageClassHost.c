/*
 * @brief Host mode driver for the library USB Mass Storage Class driver
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

#define  __INCLUDE_FROM_MS_DRIVER
#define  __INCLUDE_FROM_MASSSTORAGE_HOST_C
#include "MassStorageClassHost.h"

uint8_t MS_Host_ConfigurePipes(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                               uint16_t ConfigDescriptorSize,
							   void* ConfigDescriptorData)
{
	USB_Descriptor_Endpoint_t*  DataINEndpoint       = NULL;
	USB_Descriptor_Endpoint_t*  DataOUTEndpoint      = NULL;
	USB_Descriptor_Interface_t* MassStorageInterface = NULL;
	uint8_t portnum = MSInterfaceInfo->Config.PortNumber;

	memset(&MSInterfaceInfo->State, 0x00, sizeof(MSInterfaceInfo->State));

	if (DESCRIPTOR_TYPE(ConfigDescriptorData) != DTYPE_Configuration)
	  return MS_ENUMERROR_InvalidConfigDescriptor;

	while (!(DataINEndpoint) || !(DataOUTEndpoint))
	{
		if (!(MassStorageInterface) ||
		    USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
		                              DCOMP_MS_Host_NextMSInterfaceEndpoint) != DESCRIPTOR_SEARCH_COMP_Found)
		{
			if (USB_GetNextDescriptorComp(&ConfigDescriptorSize, &ConfigDescriptorData,
			                              DCOMP_MS_Host_NextMSInterface) != DESCRIPTOR_SEARCH_COMP_Found)
			{
				return MS_ENUMERROR_NoCompatibleInterfaceFound;
			}

			MassStorageInterface = DESCRIPTOR_PCAST(ConfigDescriptorData, USB_Descriptor_Interface_t);

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

		if (PipeNum == MSInterfaceInfo->Config.DataINPipeNumber)
		{
			Size            = le16_to_cpu(DataINEndpoint->EndpointSize);
			EndpointAddress = DataINEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_IN;
			Type            = EP_TYPE_BULK;
			DoubleBanked    = MSInterfaceInfo->Config.DataINPipeDoubleBank;

			MSInterfaceInfo->State.DataINPipeSize = DataINEndpoint->EndpointSize;
		}
		else if (PipeNum == MSInterfaceInfo->Config.DataOUTPipeNumber)
		{
			Size            = le16_to_cpu(DataOUTEndpoint->EndpointSize);
			EndpointAddress = DataOUTEndpoint->EndpointAddress;
			Token           = PIPE_TOKEN_OUT;
			Type            = EP_TYPE_BULK;
			DoubleBanked    = MSInterfaceInfo->Config.DataOUTPipeDoubleBank;
			
			MSInterfaceInfo->State.DataOUTPipeSize = DataOUTEndpoint->EndpointSize;
		}
		else
		{
			continue;
		}

		if (!(Pipe_ConfigurePipe(portnum,PipeNum, Type, Token, EndpointAddress, Size,
		                         DoubleBanked ? PIPE_BANK_DOUBLE : PIPE_BANK_SINGLE)))
		{
			return MS_ENUMERROR_PipeConfigurationFailed;
		}
	}

	MSInterfaceInfo->State.InterfaceNumber = MassStorageInterface->InterfaceNumber;
	MSInterfaceInfo->State.IsActive = true;

	return MS_ENUMERROR_NoError;
}

static uint8_t DCOMP_MS_Host_NextMSInterface(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Interface)
	{
		USB_Descriptor_Interface_t* Interface = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Interface_t);

		if ((Interface->Class    == MS_CSCP_MassStorageClass)        &&
		    (Interface->SubClass == MS_CSCP_SCSITransparentSubclass) &&
		    (Interface->Protocol == MS_CSCP_BulkOnlyTransportProtocol))
		{
			return DESCRIPTOR_SEARCH_Found;
		}
	}

	return DESCRIPTOR_SEARCH_NotFound;
}

static uint8_t DCOMP_MS_Host_NextMSInterfaceEndpoint(void* const CurrentDescriptor)
{
	USB_Descriptor_Header_t* Header = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Header_t);

	if (Header->Type == DTYPE_Endpoint)
	{
		USB_Descriptor_Endpoint_t* Endpoint = DESCRIPTOR_PCAST(CurrentDescriptor, USB_Descriptor_Endpoint_t);

		uint8_t EndpointType = (Endpoint->Attributes & EP_TYPE_MASK);

		if ((EndpointType == EP_TYPE_BULK) && (!(Pipe_IsEndpointBound(Endpoint->EndpointAddress))))
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

static uint8_t MS_Host_SendCommand(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                                   MS_CommandBlockWrapper_t* const SCSICommandBlock,
                                   const void* const BufferPtr)
{
	uint8_t ErrorCode = PIPE_RWSTREAM_NoError;
	uint8_t portnum = MSInterfaceInfo->Config.PortNumber;

	if (++MSInterfaceInfo->State.TransactionTag == 0xFFFFFFFF)
	  MSInterfaceInfo->State.TransactionTag = 1;

	SCSICommandBlock->Signature = CPU_TO_LE32(MS_CBW_SIGNATURE);
	SCSICommandBlock->Tag       = cpu_to_le32(MSInterfaceInfo->State.TransactionTag);

	Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataOUTPipeNumber);
	Pipe_Unfreeze();

	if ((ErrorCode = Pipe_Write_Stream_LE(portnum,SCSICommandBlock, sizeof(MS_CommandBlockWrapper_t),
	                                      NULL)) != PIPE_RWSTREAM_NoError)
	{
		return ErrorCode;
	}
	
	Pipe_ClearOUT(portnum);
	Pipe_WaitUntilReady(portnum);

	Pipe_Freeze();

	if (BufferPtr != NULL)
	{
		ErrorCode = MS_Host_SendReceiveData(MSInterfaceInfo, SCSICommandBlock, (void*)BufferPtr);
		
		if ((ErrorCode != PIPE_RWSTREAM_NoError) && (ErrorCode != PIPE_RWSTREAM_PipeStalled))
		{
			Pipe_Freeze();
			return ErrorCode;
		}
	}
	
	MS_CommandStatusWrapper_t SCSIStatusBlock;
	return MS_Host_GetReturnedStatus(MSInterfaceInfo, &SCSIStatusBlock);
}

static uint8_t MS_Host_WaitForDataReceived(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo)
{
	uint16_t TimeoutMSRem        = MS_COMMAND_DATA_TIMEOUT_MS;
	uint16_t PreviousFrameNumber = USB_Host_GetFrameNumber();
	uint8_t portnum = MSInterfaceInfo->Config.PortNumber;

	Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataINPipeNumber);
	Pipe_Unfreeze();

	while (!(Pipe_IsINReceived(portnum)))
	{
		uint16_t CurrentFrameNumber = USB_Host_GetFrameNumber();

		if (CurrentFrameNumber != PreviousFrameNumber)
		{
			PreviousFrameNumber = CurrentFrameNumber;

			if (!(TimeoutMSRem--))
			  return PIPE_RWSTREAM_Timeout;
		}

		Pipe_Freeze();
		Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataOUTPipeNumber);
		Pipe_Unfreeze();

		if (Pipe_IsStalled(portnum))
		{
			Pipe_ClearStall(portnum);
			USB_Host_ClearEndpointStall(portnum,Pipe_GetBoundEndpointAddress(portnum));
			return PIPE_RWSTREAM_PipeStalled;
		}

		Pipe_Freeze();
		Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataINPipeNumber);
		Pipe_Unfreeze();

		if (Pipe_IsStalled(portnum))
		{
			Pipe_ClearStall(portnum);
			USB_Host_ClearEndpointStall(portnum,Pipe_GetBoundEndpointAddress(portnum));
			return PIPE_RWSTREAM_PipeStalled;
		}

		if (USB_HostState[portnum] == HOST_STATE_Unattached)
		  return PIPE_RWSTREAM_DeviceDisconnected;
	};

	Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataINPipeNumber);
	Pipe_Freeze();

	Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataOUTPipeNumber);
	Pipe_Freeze();

	return PIPE_RWSTREAM_NoError;
}

static uint8_t MS_Host_SendReceiveData(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                                       MS_CommandBlockWrapper_t* const SCSICommandBlock,
                                       void* BufferPtr)
{
	uint16_t BytesRem  = le32_to_cpu(SCSICommandBlock->DataTransferLength);
	uint8_t portnum = MSInterfaceInfo->Config.PortNumber;
#if defined(__LPC177X_8X__) || defined(__LPC407X_8X__)
	uint8_t  ErrorCode = PIPE_RWSTREAM_NoError;
	
	if (SCSICommandBlock->Flags & MS_COMMAND_DIR_DATA_IN)
	{
		if ((ErrorCode = MS_Host_WaitForDataReceived(MSInterfaceInfo)) != PIPE_RWSTREAM_NoError)
		{
			Pipe_Freeze();
			return ErrorCode;
		}

		Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataINPipeNumber);
		Pipe_Unfreeze();

		if ((ErrorCode = Pipe_Read_Stream_LE(portnum,BufferPtr, BytesRem, NULL)) != PIPE_RWSTREAM_NoError)
		  return ErrorCode;

		Pipe_ClearIN(portnum);
	}
	else
	{
		Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataOUTPipeNumber);
		Pipe_Unfreeze();

		if ((ErrorCode = Pipe_Write_Stream_LE(portnum,BufferPtr, BytesRem, NULL)) != PIPE_RWSTREAM_NoError)
		  return ErrorCode;

		Pipe_ClearOUT(portnum);

		while (!(Pipe_IsOUTReady(portnum)))
		{
			if (USB_HostState[portnum] == HOST_STATE_Unattached)
			  return PIPE_RWSTREAM_DeviceDisconnected;
		}
	}

	Pipe_Freeze();

	return ErrorCode;
#else
	uint16_t packsize;
	if (SCSICommandBlock->Flags & MS_COMMAND_DIR_DATA_IN)
	{
		Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataINPipeNumber);
		packsize = MSInterfaceInfo->State.DataINPipeSize;
	}
	else
	{
		Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataOUTPipeNumber);
		packsize = MSInterfaceInfo->State.DataOUTPipeSize;
	}

	Pipe_Streaming(portnum,(uint8_t*)BufferPtr,BytesRem,packsize);

	while(!Pipe_IsStatusOK(portnum));

	Pipe_ClearIN(portnum);
	return PIPE_RWSTREAM_NoError;
#endif
}

static uint8_t MS_Host_GetReturnedStatus(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                                         MS_CommandStatusWrapper_t* const SCSICommandStatus)
{
	uint8_t ErrorCode = PIPE_RWSTREAM_NoError;
	uint8_t portnum = MSInterfaceInfo->Config.PortNumber;

	if ((ErrorCode = MS_Host_WaitForDataReceived(MSInterfaceInfo)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;

	Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataINPipeNumber);
	Pipe_Unfreeze();

	if ((ErrorCode = Pipe_Read_Stream_LE(portnum,SCSICommandStatus, sizeof(MS_CommandStatusWrapper_t),
	                                     NULL)) != PIPE_RWSTREAM_NoError)
	{
		return ErrorCode;
	}

	Pipe_ClearIN(portnum);
	Pipe_Freeze();

	if (SCSICommandStatus->Status != MS_SCSI_COMMAND_Pass)
	  ErrorCode = MS_ERROR_LOGICAL_CMD_FAILED;

	return ErrorCode;
}

uint8_t MS_Host_ResetMSInterface(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo)
{
	uint8_t ErrorCode;
	uint8_t portnum = MSInterfaceInfo->Config.PortNumber;
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = MS_REQ_MassStorageReset,
			.wValue        = 0,
			.wIndex        = MSInterfaceInfo->State.InterfaceNumber,
			.wLength       = 0,
		};

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	if ((ErrorCode = USB_Host_SendControlRequest(portnum,NULL)) != HOST_SENDCONTROL_Successful)
	  return ErrorCode;
	
	Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataINPipeNumber);
	
	if ((ErrorCode = USB_Host_ClearEndpointStall(portnum,Pipe_GetBoundEndpointAddress(portnum))) != HOST_SENDCONTROL_Successful)
	  return ErrorCode;

	Pipe_SelectPipe(portnum,MSInterfaceInfo->Config.DataOUTPipeNumber);

	if ((ErrorCode = USB_Host_ClearEndpointStall(portnum,Pipe_GetBoundEndpointAddress(portnum))) != HOST_SENDCONTROL_Successful)
	  return ErrorCode;

	return HOST_SENDCONTROL_Successful;
}

uint8_t MS_Host_GetMaxLUN(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                          uint8_t* const MaxLUNIndex)
{
	uint8_t ErrorCode;
	uint8_t portnum = MSInterfaceInfo->Config.PortNumber;
	USB_ControlRequest = (USB_Request_Header_t)
		{
			.bmRequestType = (REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE),
			.bRequest      = MS_REQ_GetMaxLUN,
			.wValue        = 0,
			.wIndex        = MSInterfaceInfo->State.InterfaceNumber,
			.wLength       = 1,
		};

	Pipe_SelectPipe(portnum,PIPE_CONTROLPIPE);

	if ((ErrorCode = USB_Host_SendControlRequest(portnum,MaxLUNIndex)) == HOST_SENDCONTROL_SetupStalled)
	{
		*MaxLUNIndex = 0;
		ErrorCode    = HOST_SENDCONTROL_Successful;
	}
        if (ErrorCode == HOST_SENDCONTROL_PipeError) {
                // Command was refused.  Use default
		*MaxLUNIndex = 0;
		ErrorCode    = HOST_SENDCONTROL_Successful;
        }

	return ErrorCode;
}

uint8_t MS_Host_GetInquiryData(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                               const uint8_t LUNIndex,
                               SCSI_Inquiry_Response_t* const InquiryData)
{
	uint8_t portnum = MSInterfaceInfo->Config.PortNumber;
	uint8_t ErrorCode;
	if ((USB_HostState[portnum] != HOST_STATE_Configured) || !(MSInterfaceInfo->State.IsActive))
	  return HOST_SENDCONTROL_DeviceDisconnected;

	MS_CommandBlockWrapper_t SCSICommandBlock = (MS_CommandBlockWrapper_t)
		{
			.DataTransferLength = CPU_TO_LE32(sizeof(SCSI_Inquiry_Response_t)),
			.Flags              = MS_COMMAND_DIR_DATA_IN,
			.LUN                = LUNIndex,
			.SCSICommandLength  = 6,
			.SCSICommandData    =
				{
					SCSI_CMD_INQUIRY,
					0x00,                            // Reserved
					0x00,                            // Reserved
					0x00,                            // Reserved
					sizeof(SCSI_Inquiry_Response_t), // Allocation Length
					0x00                             // Unused (control)
				}
		};

	if ((ErrorCode = MS_Host_SendCommand(MSInterfaceInfo, &SCSICommandBlock, InquiryData)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;

	return PIPE_RWSTREAM_NoError;
}

uint8_t MS_Host_TestUnitReady(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                              const uint8_t LUNIndex)
{
	if ((USB_HostState[MSInterfaceInfo->Config.PortNumber] != HOST_STATE_Configured) || !(MSInterfaceInfo->State.IsActive))
	  return HOST_SENDCONTROL_DeviceDisconnected;

	uint8_t ErrorCode;

	MS_CommandBlockWrapper_t SCSICommandBlock = (MS_CommandBlockWrapper_t)
		{
			.DataTransferLength = CPU_TO_LE32(0),
			.Flags              = MS_COMMAND_DIR_DATA_IN,
			.LUN                = LUNIndex,
			.SCSICommandLength  = 6,
			.SCSICommandData    =
				{
					SCSI_CMD_TEST_UNIT_READY,
					0x00,                   // Reserved
					0x00,                   // Reserved
					0x00,                   // Reserved
					0x00,                   // Reserved
					0x00                    // Unused (control)
				}
		};

	if ((ErrorCode = MS_Host_SendCommand(MSInterfaceInfo, &SCSICommandBlock, NULL)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;
	
	return PIPE_RWSTREAM_NoError;
}

uint8_t MS_Host_ReadDeviceCapacity(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                                   const uint8_t LUNIndex,
                                   SCSI_Capacity_t* const DeviceCapacity)
{
	if ((USB_HostState[MSInterfaceInfo->Config.PortNumber] != HOST_STATE_Configured) || !(MSInterfaceInfo->State.IsActive))
	  return HOST_SENDCONTROL_DeviceDisconnected;

	uint8_t ErrorCode;

	MS_CommandBlockWrapper_t SCSICommandBlock = (MS_CommandBlockWrapper_t)
		{
			.DataTransferLength = CPU_TO_LE32(sizeof(SCSI_Capacity_t)),
			.Flags              = MS_COMMAND_DIR_DATA_IN,
			.LUN                = LUNIndex,
			.SCSICommandLength  = 10,
			.SCSICommandData    =
				{
					SCSI_CMD_READ_CAPACITY_10,
					0x00,                   // Reserved
					0x00,                   // MSB of Logical block address
					0x00,
					0x00,
					0x00,                   // LSB of Logical block address
					0x00,                   // Reserved
					0x00,                   // Reserved
					0x00,                   // Partial Medium Indicator
					0x00                    // Unused (control)
				}
		};

	if ((ErrorCode = MS_Host_SendCommand(MSInterfaceInfo, &SCSICommandBlock, DeviceCapacity)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;

	DeviceCapacity->Blocks    = BE32_TO_CPU(DeviceCapacity->Blocks);
	DeviceCapacity->BlockSize = BE32_TO_CPU(DeviceCapacity->BlockSize);

	return PIPE_RWSTREAM_NoError;
}

uint8_t MS_Host_RequestSense(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                             const uint8_t LUNIndex,
                             SCSI_Request_Sense_Response_t* const SenseData)
{
	if ((USB_HostState[MSInterfaceInfo->Config.PortNumber] != HOST_STATE_Configured) || !(MSInterfaceInfo->State.IsActive))
	  return HOST_SENDCONTROL_DeviceDisconnected;

	uint8_t ErrorCode;

	MS_CommandBlockWrapper_t SCSICommandBlock = (MS_CommandBlockWrapper_t)
		{
			.DataTransferLength = CPU_TO_LE32(sizeof(SCSI_Request_Sense_Response_t)),
			.Flags              = MS_COMMAND_DIR_DATA_IN,
			.LUN                = LUNIndex,
			.SCSICommandLength  = 6,
			.SCSICommandData    =
				{
					SCSI_CMD_REQUEST_SENSE,
					0x00,                                  // Reserved
					0x00,                                  // Reserved
					0x00,                                  // Reserved
					sizeof(SCSI_Request_Sense_Response_t), // Allocation Length
					0x00                                   // Unused (control)
				}
		};

	if ((ErrorCode = MS_Host_SendCommand(MSInterfaceInfo, &SCSICommandBlock, SenseData)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;

	return PIPE_RWSTREAM_NoError;
}

uint8_t MS_Host_PreventAllowMediumRemoval(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                                          const uint8_t LUNIndex,
                                          const bool PreventRemoval)
{
	if ((USB_HostState[MSInterfaceInfo->Config.PortNumber] != HOST_STATE_Configured) || !(MSInterfaceInfo->State.IsActive))
	  return HOST_SENDCONTROL_DeviceDisconnected;

	uint8_t ErrorCode;

	MS_CommandBlockWrapper_t SCSICommandBlock = (MS_CommandBlockWrapper_t)
		{
			.DataTransferLength = CPU_TO_LE32(0),
			.Flags              = MS_COMMAND_DIR_DATA_OUT,
			.LUN                = LUNIndex,
			.SCSICommandLength  = 6,
			.SCSICommandData    =
				{
					SCSI_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL,
					0x00,                   // Reserved
					0x00,                   // Reserved
					PreventRemoval,         // Prevent flag
					0x00,                   // Reserved
					0x00                    // Unused (control)
				}
		};

	if ((ErrorCode = MS_Host_SendCommand(MSInterfaceInfo, &SCSICommandBlock, NULL)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;

	return PIPE_RWSTREAM_NoError;
}

uint8_t MS_Host_ReadDeviceBlocks(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                                 const uint8_t LUNIndex,
                                 const uint32_t BlockAddress,
                                 const uint8_t Blocks,
                                 const uint16_t BlockSize,
                                 void* BlockBuffer)
{
	if ((USB_HostState[MSInterfaceInfo->Config.PortNumber] != HOST_STATE_Configured) || !(MSInterfaceInfo->State.IsActive))
	  return HOST_SENDCONTROL_DeviceDisconnected;

	uint8_t ErrorCode;

	MS_CommandBlockWrapper_t SCSICommandBlock = (MS_CommandBlockWrapper_t)
		{
			.DataTransferLength = cpu_to_le32((uint32_t)Blocks * BlockSize),
			.Flags              = MS_COMMAND_DIR_DATA_IN,
			.LUN                = LUNIndex,
			.SCSICommandLength  = 10,
			.SCSICommandData    =
				{
					SCSI_CMD_READ_10,
					0x00,                   // Unused (control bits, all off)
					(BlockAddress >> 24),   // MSB of Block Address
					(BlockAddress >> 16),
					(BlockAddress >> 8),
					(BlockAddress & 0xFF),  // LSB of Block Address
					0x00,                   // Reserved
					0x00,                   // MSB of Total Blocks to Read
					Blocks,                 // LSB of Total Blocks to Read
					0x00                    // Unused (control)
				}
		};

	if ((ErrorCode = MS_Host_SendCommand(MSInterfaceInfo, &SCSICommandBlock, BlockBuffer)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;

	return PIPE_RWSTREAM_NoError;
}

uint8_t MS_Host_WriteDeviceBlocks(USB_ClassInfo_MS_Host_t* const MSInterfaceInfo,
                                  const uint8_t LUNIndex,
                                  const uint32_t BlockAddress,
                                  const uint8_t Blocks,
                                  const uint16_t BlockSize,
                                  const void* BlockBuffer)
{
	if ((USB_HostState[MSInterfaceInfo->Config.PortNumber] != HOST_STATE_Configured) || !(MSInterfaceInfo->State.IsActive))
	  return HOST_SENDCONTROL_DeviceDisconnected;

	uint8_t ErrorCode;

	MS_CommandBlockWrapper_t SCSICommandBlock = (MS_CommandBlockWrapper_t)
		{
			.DataTransferLength = cpu_to_le32((uint32_t)Blocks * BlockSize),
			.Flags              = MS_COMMAND_DIR_DATA_OUT,
			.LUN                = LUNIndex,
			.SCSICommandLength  = 10,
			.SCSICommandData    =
				{
					SCSI_CMD_WRITE_10,
					0x00,                   // Unused (control bits, all off)
					(BlockAddress >> 24),   // MSB of Block Address
					(BlockAddress >> 16),
					(BlockAddress >> 8),
					(BlockAddress & 0xFF),  // LSB of Block Address
					0x00,                   // Reserved
					0x00,                   // MSB of Total Blocks to Write
					Blocks,                 // LSB of Total Blocks to Write
					0x00                    // Unused (control)
				}
		};

	if ((ErrorCode = MS_Host_SendCommand(MSInterfaceInfo, &SCSICommandBlock, BlockBuffer)) != PIPE_RWSTREAM_NoError)
	  return ErrorCode;

	return PIPE_RWSTREAM_NoError;
}

#endif

