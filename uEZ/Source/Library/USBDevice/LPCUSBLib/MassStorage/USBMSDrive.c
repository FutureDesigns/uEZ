/*-------------------------------------------------------------------------*
 * File:  USBMSDrive.c
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "USBMSDrive.h"
#include <uEZPacked.h>
#include <Source/Library/LPCOpen/LPCUSBLib/Drivers/USB/USB.h>
#include <Source/Library/LPCOpen/LPCUSBLib/LPCUSBLib_uEZ.h>
#include <Source/Library/LPCOpen/LPCUSBLib/Drivers/USB/Class/Common/MassStorageClassCommon.h>

//ToDo: Remove
#ifndef NULL
#define NULL ((void *)0)
#endif
/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#if 0
    #define dprintf printf
#else
    #define dprintf(...)
#endif
#if 0
    #define dprintfc printf
#else
    #define dprintfc(...)
#endif

#define USB_DRIVE_MAX_PACKET_SIZE          64

/* USB Device Classes */
#define USB_DEVICE_CLASS_RESERVED           0x00
#define USB_DEVICE_CLASS_AUDIO              0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS     0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE    0x03
#define USB_DEVICE_CLASS_MONITOR            0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE 0x05
#define USB_DEVICE_CLASS_POWER              0x06
#define USB_DEVICE_CLASS_PRINTER            0x07
#define USB_DEVICE_CLASS_STORAGE            0x08
#define USB_DEVICE_CLASS_HUB                0x09
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC    0xFF

/* bmAttributes in Configuration Descriptor */
#define USB_CONFIG_POWERED_MASK             0xC0
#define USB_CONFIG_BUS_POWERED              0x80
#define USB_CONFIG_SELF_POWERED             0x40
#define USB_CONFIG_REMOTE_WAKEUP            0x20

/* SCSI Commands */
#define SCSI_TEST_UNIT_READY                0x00
#define SCSI_REQUEST_SENSE                  0x03
#define SCSI_FORMAT_UNIT                    0x04
#define SCSI_INQUIRY                        0x12
#define SCSI_MODE_SELECT6                   0x15
#define SCSI_MODE_SENSE6                    0x1A
#define SCSI_START_STOP_UNIT                0x1B
#define SCSI_MEDIA_REMOVAL                  0x1E
#define SCSI_READ_FORMAT_CAPACITIES         0x23
#define SCSI_READ_CAPACITY                  0x25
#define SCSI_READ10                         0x28
#define SCSI_WRITE10                        0x2A
#define SCSI_VERIFY10                       0x2F
#define SCSI_MODE_SELECT10                  0x55
#define SCSI_MODE_SENSE10                   0x5A

#define MSC_ENDPOINT_IN                     (2)
#define MSC_ENDPOINT_OUT                    (5)

#define MSC_ENDPOINT_IN_DES                 (ENDPOINT_DIR_IN | MSC_ENDPOINT_IN)
#define MSC_ENDPOINT_OUT_DES                (ENDPOINT_DIR_OUT | MSC_ENDPOINT_OUT)


/** Convenience macro to easily create \ref USB_Descriptor_String_t instances from a wide character string.
 *
 *  \note This macro is for little-endian systems only.
 *
 *  \param[in] String  String to initialize a USB String Descriptor structure with.
 */
#define USB_STRING_DESCRIPTOR(aString)     \
    { \
        .Header = {\
           .Size = sizeof(USB_Descriptor_Header_t) + (sizeof(aString) - 2), \
           .Type = DTYPE_String \
        }, \
        .String = aString \
    }

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
/* Type Defines: */
    /** Type define for the device configuration descriptor structure. This must be defined in the
     *  application code, as the configuration descriptor contains several sub-descriptors which
     *  vary between devices, and which describe the device's usage to the host.
     */
enum StringDescriptors_t
{
    STRING_ID_Language     = 0, /**< Supported Languages string descriptor ID (must be zero) */
    STRING_ID_Manufacturer = 1, /**< Manufacturer string ID */
    STRING_ID_Product      = 2, /**< Product string ID */
};

typedef struct
{
    USB_Descriptor_Configuration_Header_t    Config;

    // MSD Control Interface
    USB_Descriptor_Interface_t               MSD_CCI_Interface;
    USB_Descriptor_Endpoint_t                MSD_DataOutEndpoint;
    USB_Descriptor_Endpoint_t                MSD_DataInEndpoint;
} USB_Descriptor_Configuration_t;

typedef TUInt32 TUIntDisk;
/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static uint16_t MassStorageCommGetDescriptor(uint8_t corenum,
                                    const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress);
static void MassStorageConnected(void);
static void MassStorageDisconnected(void);
static void MassStorageConfigChanged(void);
static void MassStorageControlRequest(void);
static int32_t MassStorageUpdate(int32_t aUnitAddress);

static void ISetCSW(USB_ClassInfo_MS_Device_t *const MSInterfaceInf);
static void IMSRead(USB_ClassInfo_MS_Device_t *const MSInterfaceInf);
static void IMSWrite(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo);
static void DataInTransfer(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo, TUInt32 aLength);


/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_LPCUSBLib_Device_Callbacks G_USBDeviceCallbacks = {
        MassStorageConnected, // Connected
        MassStorageDisconnected, // Disconnected
        MassStorageConfigChanged, // ConfigurationChanged
        MassStorageControlRequest, // ControlRequest
        0, // Suspend
        0, // WakeUp
        0, // Reset
        MassStorageUpdate, // Update
        MassStorageCommGetDescriptor, // GetDescriptor
};

static TUIntDisk G_length;
static TUIntDisk G_offset;
static TUIntDisk G_memorySize;
//static TBool G_memOK;
static const TUInt32 G_blockSize = 512;
static DEVICE_MassStorage **G_ms;
static T_msSizeInfo G_msSizeInfo;
static TUInt8 *G_msBuffer;
static TUIntDisk G_msBufferBlock;
static TBool G_msBufferNeedWrite;
static TUInt8 G_USBMSDriveBuffer[USB_DRIVE_MAX_PACKET_SIZE];

static const USB_Descriptor_Device_t DeviceDescriptor =
{
       .Header                  = {.Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device},

       .USBSpecification        = VERSION_BCD(1.1),
       .Class                   = MS_CSCP_MassStorageClass,
       .SubClass                = MS_CSCP_SCSITransparentSubclass,
       .Protocol                = MS_CSCP_BulkOnlyTransportProtocol,

       .Endpoint0Size           = FIXED_CONTROL_ENDPOINT_SIZE,

       .VendorID               = 0xC251,
       .ProductID              = 0x1303,
       .ReleaseNumber          = VERSION_BCD(1.0),

       .ManufacturerStrIndex   = STRING_ID_Manufacturer,
       .ProductStrIndex        = STRING_ID_Product,
       .SerialNumStrIndex      = NO_DESCRIPTOR,

       .NumberOfConfigurations = FIXED_NUM_CONFIGURATIONS
};

/** Configuration descriptor structure. This descriptor, located in SRAM memory, describes the usage
 *  of the device in one of its supported configurations, including information about any device interfaces
 *  and endpoints. The descriptor is read out by the USB host during the enumeration process when selecting
 *  a configuration so that the host may correctly communicate with the USB device.
 */
static const USB_Descriptor_Configuration_t ConfigurationDescriptor =
{
    .Config =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration},

            .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
            .TotalInterfaces        = 1,

            .ConfigurationNumber    = 1,
            .ConfigurationStrIndex  = NO_DESCRIPTOR,

            .ConfigAttributes       = USB_CONFIG_ATTR_BUSPOWERED,

            .MaxPowerConsumption    = USB_CONFIG_POWER_MA(100)
        },

    .MSD_CCI_Interface =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface},

            .InterfaceNumber        = 0,
            .AlternateSetting       = 0,

            .TotalEndpoints         = 2,

            .Class                  = MS_CSCP_MassStorageClass,
            .SubClass               = MS_CSCP_SCSITransparentSubclass,
            .Protocol               = MS_CSCP_BulkOnlyTransportProtocol,

            .InterfaceStrIndex      = NO_DESCRIPTOR
        },

    .MSD_DataOutEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = MSC_ENDPOINT_OUT_DES,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = USB_DRIVE_MAX_PACKET_SIZE,
            .PollingIntervalMS      = 0x0A
        },

    .MSD_DataInEndpoint =
        {
            .Header                 = {.Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint},

            .EndpointAddress        = MSC_ENDPOINT_IN_DES,
            .Attributes             = (EP_TYPE_BULK | ENDPOINT_ATTR_NO_SYNC | ENDPOINT_USAGE_DATA),
            .EndpointSize           = USB_DRIVE_MAX_PACKET_SIZE,
            .PollingIntervalMS      = 0x0A
        }
};

static USB_ClassInfo_MS_Device_t UEZ_MS_Device = {
        .Config = {
            .InterfaceNumber = 0,

            .DataINEndpointNumber = MSC_ENDPOINT_IN,
            .DataINEndpointSize = USB_DRIVE_MAX_PACKET_SIZE,
            .DataINEndpointDoubleBank = false,

            .DataOUTEndpointNumber = MSC_ENDPOINT_OUT,
            .DataOUTEndpointSize = USB_DRIVE_MAX_PACKET_SIZE,
            .DataOUTEndpointDoubleBank = false,

            .TotalLUNs = 1,
            .PortNumber = 0
        }
};

static T_USBMSDriveCallbacks G_callbacks;
static void *G_callbackWorkspace;

/** Manufacturer descriptor string. This is a Unicode string containing the manufacturer's details in human readable
 *  form, and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
static const ATTR_IAR_PACKED struct { USB_Descriptor_Header_t Header; TUInt8 String[sizeof("Future Designs, Inc.")-1]; } ATTR_PACKED
ManufacturerString = {
    .Header = {
        .Size = sizeof(USB_Descriptor_Header_t) + sizeof("Future Designs, Inc.")-2,
        .Type = DTYPE_String
    },
    .String = "Future Designs, Inc."
};

/** Product descriptor string. This is a Unicode string containing the product's details in human readable form,
 *  and is read out upon request by the host when the appropriate string ID is requested, listed in the Device
 *  Descriptor.
 */
static const ATTR_IAR_PACKED struct { USB_Descriptor_Header_t Header; TUInt8 String[sizeof("uEZGUI")-1]; } ATTR_PACKED
ProductString = USB_STRING_DESCRIPTOR("uEZGUI");


static void IBlockFlush(void)
{
//    TUInt32 i, j = 0;

    if (G_msBufferNeedWrite) {
dprintf("MSFlush: block %d\n", G_msBufferBlock);
        (*G_ms)->Write(G_ms, G_msBufferBlock, 1, G_msBuffer);
        G_msBufferNeedWrite = EFalse;
    }
}

static void IBlockPrepWrite(TUInt32 aBlockNum)
{
    if (aBlockNum != G_msBufferBlock) {
        IBlockFlush();
        G_msBufferBlock = aBlockNum;
    }
}

static void IBlockRead(TUInt32 aBlockNum)
{
    if (aBlockNum != G_msBufferBlock) {
//dprintf("MSBlockRead: block %d\n", aBlockNum);
        IBlockFlush();
        G_msBufferBlock = aBlockNum;
        (*G_ms)->Read(G_ms, G_msBufferBlock, 1, G_msBuffer);
    }
}

static void IBlockReadBytes(TUIntDisk aOffset, TUInt32 aNumBytes, TUInt8 *aBuffer)
{
    TUIntDisk n;
    TUInt32 m;
    TUIntDisk block;

//dprintf("MSR: %08X %08X\n", aOffset, aNumBytes);
//dprintf("MSRead: %08X-%08X (size %d)\n", aOffset, aOffset+aNumBytes-1, aNumBytes);
    while (aNumBytes) {
        block = aOffset / G_msSizeInfo.iSectorSize;
        m = aOffset % G_msSizeInfo.iSectorSize;
        n = G_msSizeInfo.iSectorSize-m;
        if (n > aNumBytes)
            n = aNumBytes;

        IBlockRead(block);
        memcpy(aBuffer, G_msBuffer+m, n);
        aBuffer += n;
        aOffset += n;
        aNumBytes -= n;
    }
}

static void IBlockWriteBytes(TUIntDisk aOffset, TUInt32 aNumBytes, TUInt8 *aBuffer)
{
    TUIntDisk n;
    TUInt32 m;
    TUInt32 m2;
    TUIntDisk block;

dprintf("MSWrite: %08X-%08X\n", aOffset, aOffset+aNumBytes-1);
    while (aNumBytes) {
        block = aOffset / G_msSizeInfo.iSectorSize;
        m = aOffset % G_msSizeInfo.iSectorSize;
        n = G_msSizeInfo.iSectorSize-m;
        if (n > aNumBytes)
            n = aNumBytes;

        IBlockPrepWrite(block);
        // Copy data into the buffer (to be written later)
        memcpy(G_msBuffer+m, aBuffer, n);
        G_msBufferNeedWrite = ETrue;
        aBuffer += n;
        aOffset += n;
        aNumBytes -= n;

        // Did we roll over to another block?
        // If so, flush it
        m2 = aOffset % G_msSizeInfo.iSectorSize;
        if (m2 < m)
            IBlockFlush();
    }
}

static void ISetCSW(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    Endpoint_SelectEndpoint(MSInterfaceInfo->Config.PortNumber,
            MSInterfaceInfo->Config.DataINEndpointNumber);
    Endpoint_Write_Stream_LE(MSInterfaceInfo->Config.PortNumber,
            (const void*)&MSInterfaceInfo->State.CommandStatus,
            sizeof(MS_CommandStatusWrapper_t), NULL);
    Endpoint_ClearIN(MSInterfaceInfo->Config.PortNumber);
}

static TBool IRWSetup(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    TUIntDisk n;

    // Logical Block Address of First Block
    n = (MSInterfaceInfo->State.CommandBlock.SCSICommandData[2] << 24) |
        (MSInterfaceInfo->State.CommandBlock.SCSICommandData[3] << 16) |
        (MSInterfaceInfo->State.CommandBlock.SCSICommandData[4] <<  8) |
        (MSInterfaceInfo->State.CommandBlock.SCSICommandData[5] <<  0);

    G_offset = n * G_blockSize;

    // Number of Blocks to transfer
    n = (MSInterfaceInfo->State.CommandBlock.SCSICommandData[7] <<  8) |
        (MSInterfaceInfo->State.CommandBlock.SCSICommandData[8] <<  0);

    G_length = n * G_blockSize;

    if (MSInterfaceInfo->State.CommandBlock.DataTransferLength != G_length) {
        Endpoint_SelectEndpoint(MSInterfaceInfo->Config.PortNumber, MSInterfaceInfo->Config.DataINEndpointNumber);
        Endpoint_StallTransaction(MSInterfaceInfo->Config.PortNumber);
        Endpoint_SelectEndpoint(MSInterfaceInfo->Config.PortNumber, MSInterfaceInfo->Config.DataOUTEndpointNumber);
        Endpoint_StallTransaction(MSInterfaceInfo->Config.PortNumber);
        ISetCSW(MSInterfaceInfo);
        return EFalse;
    }

    return ETrue;
}

static void IMSRead(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    TUInt32 n;

    while(G_length){
        n = G_length;
        if (n > USB_DRIVE_MAX_PACKET_SIZE)
            n = USB_DRIVE_MAX_PACKET_SIZE;

        if ((G_offset+n) > G_memorySize) {
            n = G_memorySize - G_offset;
        }

        IBlockReadBytes(G_offset, n, G_USBMSDriveBuffer);
        Endpoint_SelectEndpoint(MSInterfaceInfo->Config.PortNumber,
                MSInterfaceInfo->Config.DataINEndpointNumber);
        Endpoint_Write_Stream_LE(MSInterfaceInfo->Config.PortNumber,
                (const void*)G_USBMSDriveBuffer, n, NULL);
        Endpoint_ClearIN(MSInterfaceInfo->Config.PortNumber);

        G_offset += n;
        G_length -= n;
        MSInterfaceInfo->State.CommandBlock.DataTransferLength = G_length;

        if (G_length == 0){
            if (G_callbacks.iUSBMSDriveActivity)
                G_callbacks.iUSBMSDriveActivity(G_callbackWorkspace);
        }
    }
}

static void IMSWrite(USB_ClassInfo_MS_Device_t * const MSInterfaceInfo)
{
    TBool exit = EFalse;

    Endpoint_SelectEndpoint(MSInterfaceInfo->Config.PortNumber,
            MSInterfaceInfo->Config.DataOUTEndpointNumber);

    Endpoint_ClearOUT(MSInterfaceInfo->Config.PortNumber);
    UEZTaskDelay(1);

    while (G_length) {
        if ((G_offset + G_length) > G_memorySize) {
            exit = ETrue;
            Endpoint_SelectEndpoint(MSInterfaceInfo->Config.PortNumber,
                    MSInterfaceInfo->Config.DataOUTEndpointNumber);
            Endpoint_StallTransaction(MSInterfaceInfo->Config.PortNumber);
        }

        IBlockWriteBytes(G_offset,
                512,
                usb_data_buffer_OUT[MSInterfaceInfo->Config.PortNumber]);

        G_offset += 512;
        G_length -= 512;
        MSInterfaceInfo->State.CommandBlock.DataTransferLength = G_length;

        Endpoint_ClearOUT(MSInterfaceInfo->Config.PortNumber);

        UEZTaskDelay(1);
        if ((G_length == 0) || (exit)) {
            if (G_callbacks.iUSBMSDriveActivity)
                G_callbacks.iUSBMSDriveActivity(G_callbackWorkspace);
            break;
        }
    }
}

static TBool IRWSetupVerify(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    TUIntDisk n;

    // Logical Block Address of First Block
    n = (MSInterfaceInfo->State.CommandBlock.SCSICommandData[2] << 24) |
        (MSInterfaceInfo->State.CommandBlock.SCSICommandData[3] << 16) |
        (MSInterfaceInfo->State.CommandBlock.SCSICommandData[4] <<  8) |
        (MSInterfaceInfo->State.CommandBlock.SCSICommandData[5] <<  0);

    G_offset = n * G_blockSize;

    // Number of Blocks to transfer
    n = (MSInterfaceInfo->State.CommandBlock.SCSICommandData[7] <<  8) |
        (MSInterfaceInfo->State.CommandBlock.SCSICommandData[8] <<  0);

    G_length = n * G_blockSize;

    if (MSInterfaceInfo->State.CommandBlock.DataTransferLength == 0) {
        ISetCSW(MSInterfaceInfo);
        return EFalse;
    }

    return ETrue;
}

/** This function is called by the library when in device mode, and must be overridden (see LUFA library "USB Descriptors"
 *  documentation) by the application code so that the address and size of a requested descriptor can be given
 *  to the USB library. When the device receives a Get Descriptor request on the control endpoint, this function
 *  is called so that the descriptor details can be passed back and the appropriate descriptor sent back to the
 *  USB host.
 */
static uint16_t MassStorageCommGetDescriptor(uint8_t corenum,
                                    const uint16_t wValue,
                                    const uint8_t wIndex,
                                    const void** const DescriptorAddress)
{
    const uint8_t  DescriptorType   = (wValue >> 8);
    const uint8_t  DescriptorNumber = (wValue & 0xFF);

    const void* Address = NULL;
    uint16_t    Size    = NO_DESCRIPTOR;

    if (corenum == 0) {
        // USB0 descriptor here
        switch (DescriptorType)
        {
            case DTYPE_Device:
                Address = &DeviceDescriptor;
                Size    = sizeof(USB_Descriptor_Device_t);
                //dprintf("MS Device Des\n");
                break;
            case DTYPE_Configuration:
                Address = &ConfigurationDescriptor;
                Size    = sizeof(USB_Descriptor_Configuration_t);
                //dprintf("MS Config Des\n");
                break;
            case DTYPE_String:
                if (DescriptorNumber == STRING_ID_Manufacturer)
                {
                    Address = &ManufacturerString;
                    Size    = ManufacturerString.Header.Size;
                }
                else if (DescriptorNumber == STRING_ID_Product)
                {
                    Address = &ProductString;
                    Size    = ProductString.Header.Size;
                }
                //dprintf("MS String Des\n");
                break;
        }

        *DescriptorAddress = Address;
    } else {
        // USB1 descriptor here
    }
    return Size;
}

static void MassStorageConnected(void)
{
    dprintf("MS Connected!\n");
}

static void MassStorageDisconnected(void)
{
    dprintf("MS Disconnected\n");
}

static void MassStorageConfigChanged(void)
{
    //dprintf("MS Config Changed\n");
    if (!(MS_Device_ConfigureEndpoints(&UEZ_MS_Device))) {
        UEZFailureMsg("USB Configure Endpoints Error!");
    }
}

static void MassStorageControlRequest(void)
{
    //dprintf("MS Control Request\n");
    MS_Device_ProcessControlRequest(&UEZ_MS_Device);
}

static int32_t MassStorageUpdate(int32_t aUnitAddress)
{
    int32_t activity = 0;

    MS_Device_USBTask(&UEZ_MS_Device);

    return activity;
}

static void DataInTransfer(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo, TUInt32 aLength)
{
    if ((USB_DeviceState[MSInterfaceInfo->Config.PortNumber]
            != DEVICE_STATE_Configured))
        return;

    Endpoint_SelectEndpoint(MSInterfaceInfo->Config.PortNumber,
            MSInterfaceInfo->Config.DataINEndpointNumber);
    Endpoint_Write_Stream_LE(MSInterfaceInfo->Config.PortNumber,
            (const void*)G_USBMSDriveBuffer, aLength, NULL);
    Endpoint_ClearIN(MSInterfaceInfo->Config.PortNumber);

    MSInterfaceInfo->State.CommandBlock.DataTransferLength -= aLength;
}

static void IInquiry(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    TUInt8 *p = G_USBMSDriveBuffer;

    //if (!DataInFormat(MSInterfaceInfo))
    //  return;

    p[ 0] = 0x00;          // Direct Access Device
    p[ 1] = 0x80;          // RMB = 1: Removable Medium
    p[ 2] = 0x00;          // Version: No conformance claim to any standard
    p[ 3] = 0x01;

    p[ 4] = 36-4;          // Additional Length
    p[ 5] = 0x80;          // SCCS = 1: Storage Controller Component
    p[ 6] = 0x00;
    p[ 7] = 0x00;

    p[ 8] = 'F';           // Vendor Identification
    p[ 9] = 'D';
    p[10] = 'I';
    p[11] = ' ';
    p[12] = ' ';
    p[13] = ' ';
    p[14] = ' ';
    p[15] = ' ';

    p[16] = 'u';           // Product Identification
    p[17] = 'E';
    p[18] = 'Z';
    p[19] = ' ';
    p[20] = 'M';
    p[21] = 'S';
    p[22] = ' ';
    p[23] = 'D';
    p[24] = 'i';
    p[25] = 's';
    p[26] = 'k';
    p[27] = ' ';
    p[28] = ' ';
    p[29] = ' ';
    p[30] = ' ';
    p[31] = ' ';

    p[32] = '1';           // Product Revision Level
    p[33] = '.';
    p[34] = '0';
    p[35] = ' ';

    DataInTransfer(MSInterfaceInfo, 36);
}

static void IRequestSense(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    TUInt8 *p = G_USBMSDriveBuffer;

    //if (!DataInFormat(MSInterfaceInfo))
    //    return;

    // See http://en.wikipedia.org/wiki/SCSI_Request_Sense_Command for details
    // and http://docs.hp.com/en/J6373-90030/apas01.html
    p[ 0] = 0x70;          // Response Code (current errors, fixed format)
    p[ 1] = 0x00;          // Segment number (obsolete)
//    p[ 2] = 0x02;          // Sense Key (Not Ready?)
    p[ 2] = 0x06;          // Sense Key (Unit Attention)
    p[ 3] = 0x00;
    p[ 4] = 0x00;
    p[ 5] = 0x00;
    p[ 6] = 0x00;
    p[ 7] = 0x0A;          // Additional Length
    p[ 8] = 0x00;
    p[ 9] = 0x00;
    p[10] = 0x00;
    p[11] = 0x00;
//    p[12] = 0x30;          // ASC
//    p[13] = 0x01;          // ASCQ (Cannot read medium, unknown format)
    p[12] = 0x28;          // ASC
    p[13] = 0x00;          // ASCQ (Not Ready to Ready change, door(s) opened and closed)
    p[14] = 0x00;
    p[15] = 0x00;
    p[16] = 0x00;
    p[17] = 0x00;

    DataInTransfer(MSInterfaceInfo, 18);
}

static void IReadFormatCapacity(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    TUInt8 *p = G_USBMSDriveBuffer;
    TUInt32 numBlocks;

    //if (!DataInFormat(MSInterfaceInfo))
    //    return;

    p[ 0] = 0x00;
    p[ 1] = 0x00;
    p[ 2] = 0x00;
    p[ 3] = 0x08;          // Capacity List Length

    numBlocks = G_msSizeInfo.iNumSectors;

    // Block Count
    p[ 4] = (numBlocks >> 24) & 0xFF;
    p[ 5] = (numBlocks >> 16) & 0xFF;
    p[ 6] = (numBlocks >>  8) & 0xFF;
    p[ 7] = (numBlocks >>  0) & 0xFF;

    // Block Length
    p[ 8] = 0x02;          // Descriptor Code: Formatted Media
    p[ 9] = (G_blockSize >> 16) & 0xFF;
    p[10] = (G_blockSize >>  8) & 0xFF;
    p[11] = (G_blockSize >>  0) & 0xFF;

    DataInTransfer(MSInterfaceInfo, 12);
}

static void IReadCapacity(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    TUInt32 numBlocks;
    TUInt8 *p = G_USBMSDriveBuffer;

    //if (!DataInFormat(MSInterfaceInfo))
    //    return;

    numBlocks = G_msSizeInfo.iNumSectors;

    // Last Logical Block
    p[ 0] = ((numBlocks - 1) >> 24) & 0xFF;
    p[ 1] = ((numBlocks - 1) >> 16) & 0xFF;
    p[ 2] = ((numBlocks - 1) >>  8) & 0xFF;
    p[ 3] = ((numBlocks - 1) >>  0) & 0xFF;

    // Block Length
    p[ 4] = (G_blockSize >> 24) & 0xFF;
    p[ 5] = (G_blockSize >> 16) & 0xFF;
    p[ 6] = (G_blockSize >>  8) & 0xFF;
    p[ 7] = (G_blockSize >>  0) & 0xFF;

    DataInTransfer(MSInterfaceInfo, 8);
}

static void IModeSense6(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    TUInt8 *p = G_USBMSDriveBuffer;

    //if (!DataInFormat(MSInterfaceInfo))
    //    return;

    p[ 0] = 0x03;
    p[ 1] = 0x00;
    p[ 2] = 0x00;
    p[ 3] = 0x00;

    DataInTransfer(MSInterfaceInfo, 4);
}

static void IModeSense10(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    TUInt8 *p = G_USBMSDriveBuffer;

    //if (!DataInFormat(MSInterfaceInfo))
    //    return;

    p[ 0] = 0x00;
    p[ 1] = 0x06;
    p[ 2] = 0x00;
    p[ 3] = 0x00;
    p[ 4] = 0x00;
    p[ 5] = 0x00;
    p[ 6] = 0x00;
    p[ 7] = 0x00;

    DataInTransfer(MSInterfaceInfo, 8);
}

bool CALLBACK_MS_Device_SCSICommandReceived(USB_ClassInfo_MS_Device_t *const MSInterfaceInfo)
{
    bool ret = true;

    switch(MSInterfaceInfo->State.CommandBlock.SCSICommandData[0]){
        case SCSI_TEST_UNIT_READY:
            //dprintfc("[T]");
            //ITestUnitReady(MSInterfaceInfo);
            break;
        case SCSI_REQUEST_SENSE:
            dprintfc("[S]");
            IRequestSense(MSInterfaceInfo);
            break;
        case SCSI_INQUIRY:
            dprintfc("[I]");
            IInquiry(MSInterfaceInfo);
            break;
        case SCSI_MODE_SENSE6:
            dprintfc("[M6]");
            IModeSense6(MSInterfaceInfo);
            break;
        case SCSI_MODE_SENSE10:
            dprintfc("[M0]");
            IModeSense10(MSInterfaceInfo);
            break;
        case SCSI_READ_FORMAT_CAPACITIES:
            dprintfc("[F]");
            IReadFormatCapacity(MSInterfaceInfo);
            break;
        case SCSI_READ_CAPACITY:
            dprintfc("[C]");
            IReadCapacity(MSInterfaceInfo);
            break;
        case SCSI_READ10:
            dprintfc("[R0]");
            IRWSetup(MSInterfaceInfo);
            if((MSInterfaceInfo->State.CommandBlock.Flags & 0x80) != 0){
                IMSRead(MSInterfaceInfo);
            } else {
                ret = false;
            }
            break;
        case SCSI_WRITE10:
            dprintfc("[W0]");
            IRWSetup(MSInterfaceInfo);
            if((MSInterfaceInfo->State.CommandBlock.Flags & 0x80) == 0){
                IMSWrite(&UEZ_MS_Device);
            } else {
                ret = false;
            }
            break;
        case SCSI_VERIFY10:
            dprintfc("[V0]");
            if (IRWSetupVerify(MSInterfaceInfo)) {
                if ((MSInterfaceInfo->State.CommandBlock.Flags & 0x80) == 0) {
                } else {
                    Endpoint_SelectEndpoint(MSInterfaceInfo->Config.PortNumber, MSInterfaceInfo->Config.DataOUTEndpointNumber);
                    Endpoint_StallTransaction(MSInterfaceInfo->Config.PortNumber);
                }
            }
            break;
        case SCSI_MEDIA_REMOVAL:
            dprintfc("[MR]");
            break;
        default:
        case SCSI_FORMAT_UNIT:
        case SCSI_MODE_SELECT6:
        case SCSI_MODE_SELECT10:
        case SCSI_START_STOP_UNIT:
            dprintfc("[?]");
            ret = false;
            ISetCSW(MSInterfaceInfo);
            break;
    }

    return ret;
}

/*-------------------------------------------------------------------------*
 * Function:  USBMSDriveInitialize
 *-------------------------------------------------------------------------*
 * Description:
 *      Initialize the MS Device.
 *-------------------------------------------------------------------------*/
T_uezError USBMSDriveInitialize(
     T_USBMSDriveCallbacks *aCallbacks,
     void *aCallbackWorkspace,
     const char *aDeviceName,
     int32_t aUnitAddress,
     TUInt8 aForceFullSpeed)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_uezDevice ms;

    // Copy over the callback information
    G_callbacks = *aCallbacks;
    G_callbackWorkspace = aCallbackWorkspace;

    error = UEZDeviceTableFind(aDeviceName, &ms); // SDCard
    if (error)
        return error;
    error = UEZDeviceTableGetWorkspace(ms, (T_uezDeviceWorkspace **)&G_ms);
    if (error)
        return error;

    // Initialize the MS device as address 0
    error = (*G_ms)->Init(G_ms, 0);
    if (error)
        return error;
    // Get information about the MS device
    error = (*G_ms)->GetSizeInfo(G_ms, &G_msSizeInfo);
    if (error)
        return error;
    G_memorySize = G_msSizeInfo.iNumSectors;
    G_memorySize *= G_msSizeInfo.iSectorSize;
    G_msBuffer = UEZMemAlloc(G_msSizeInfo.iSectorSize);
    G_msBufferBlock = (TUIntDisk)-1;
    G_msBufferNeedWrite = EFalse;

    // Setup full speed USB device
    UEZ_LPCUSBLib_Device_Require(aUnitAddress, &G_USBDeviceCallbacks, aForceFullSpeed);
    return error;
}
/*-------------------------------------------------------------------------*
 * End of File:  USBMSDrive.c
 *-------------------------------------------------------------------------*/
