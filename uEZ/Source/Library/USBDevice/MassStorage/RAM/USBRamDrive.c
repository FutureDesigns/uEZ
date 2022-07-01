/*-------------------------------------------------------------------------*
 * File:  USBRamDrive.c
 *-------------------------------------------------------------------------*
 | Description:
 |      USB implementation of the device side of a virtual comm port
 |      following CDC rules.
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
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <UEZDeviceTable.h>
#include <Device/USBDevice.h>
#include <Source/Library/USBDevice/MassStorage/RAM/USBRamDrive.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define USB_RAM_DRIVE_MAX_PACKET_SIZE               64
#define USB_RAM_DRIVE_EP_IN     USB_ENDPOINT_ADDRESS(2, USB_ENDPOINT_ADDRESS_DIR_IN)
//#define USB_RAM_DRIVE_EP_OUT    USB_ENDPOINT_ADDRESS(4, USB_ENDPOINT_ADDRESS_DIR_OUT)
#define USB_RAM_DRIVE_EP_OUT    USB_ENDPOINT_ADDRESS(5, USB_ENDPOINT_ADDRESS_DIR_OUT)

/* USB Device Classes */
#define USB_DEVICE_CLASS_RESERVED              0x00
#define USB_DEVICE_CLASS_AUDIO                 0x01
#define USB_DEVICE_CLASS_COMMUNICATIONS        0x02
#define USB_DEVICE_CLASS_HUMAN_INTERFACE       0x03
#define USB_DEVICE_CLASS_MONITOR               0x04
#define USB_DEVICE_CLASS_PHYSICAL_INTERFACE    0x05
#define USB_DEVICE_CLASS_POWER                 0x06
#define USB_DEVICE_CLASS_PRINTER               0x07
#define USB_DEVICE_CLASS_STORAGE               0x08
#define USB_DEVICE_CLASS_HUB                   0x09
#define USB_DEVICE_CLASS_VENDOR_SPECIFIC       0xFF

/* bmAttributes in Configuration Descriptor */
#define USB_CONFIG_POWERED_MASK                0xC0
#define USB_CONFIG_BUS_POWERED                 0x80
#define USB_CONFIG_SELF_POWERED                0x40
#define USB_CONFIG_REMOTE_WAKEUP               0x20

/* bMaxPower in Configuration Descriptor */
#define USB_CONFIG_POWER_MA(mA)                ((mA)/2)

/* MSC Subclass Codes */
#define MSC_SUBCLASS_RBC                0x01
#define MSC_SUBCLASS_SFF8020I_MMC2      0x02
#define MSC_SUBCLASS_QIC157             0x03
#define MSC_SUBCLASS_UFI                0x04
#define MSC_SUBCLASS_SFF8070I           0x05
#define MSC_SUBCLASS_SCSI               0x06

/* MSC Protocol Codes */
#define MSC_PROTOCOL_CBI_INT            0x00
#define MSC_PROTOCOL_CBI_NOINT          0x01
#define MSC_PROTOCOL_BULK_ONLY          0x50


/* MSC Request Codes */
#define MSC_REQUEST_RESET               0xFF
#define MSC_REQUEST_GET_MAX_LUN         0xFE


#define MSC_CBW_Signature               0x43425355
#define MSC_CSW_Signature               0x53425355

/* SCSI Commands */
#define SCSI_TEST_UNIT_READY            0x00
#define SCSI_REQUEST_SENSE              0x03
#define SCSI_FORMAT_UNIT                0x04
#define SCSI_INQUIRY                    0x12
#define SCSI_MODE_SELECT6               0x15
#define SCSI_MODE_SENSE6                0x1A
#define SCSI_START_STOP_UNIT            0x1B
#define SCSI_MEDIA_REMOVAL              0x1E
#define SCSI_READ_FORMAT_CAPACITIES     0x23
#define SCSI_READ_CAPACITY              0x25
#define SCSI_READ10                     0x28
#define SCSI_WRITE10                    0x2A
#define SCSI_VERIFY10                   0x2F
#define SCSI_MODE_SELECT10              0x55
#define SCSI_MODE_SENSE10               0x5A

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
/* Bulk-only Command Block Wrapper */
typedef struct {
  TUInt32 dSignature;
  TUInt32 dTag;
  TUInt32 dDataLength;
  TUInt8 bmFlags;
  TUInt8 bLUN;
  TUInt8 bCBLength;
  TUInt8 CB[16];
} MSC_CBW;

/* Bulk-only Command Status Wrapper */
typedef struct {
  TUInt32 dSignature;
  TUInt32 dTag;
  TUInt32 dDataResidue;
  TUInt8 bStatus;
} MSC_CSW;

/* CSW Status Definitions */
#define CSW_CMD_PASSED                  0x00
#define CSW_CMD_FAILED                  0x01
#define CSW_PHASE_ERROR                 0x02

// Stages
#define STAGE_CBW                   0       // Waiting for a CBW
#define STAGE_DATA_IN               1       // Data coming in, and more after it
#define STAGE_DATA_IN_LAST          2       // Last data being sent
#define STAGE_DATA_IN_LAST_STALL    3       // Last data being sent even while more is asked
#define STAGE_CSW                   4       // Time to send the CSW
#define STAGE_DATA_OUT              5       // Data going out
#define STAGE_ERROR                 6       // Error stage

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
TBool USBRamDriveHandleClassRequest(
        void *aWorkspace,
        T_USBSetupPacket *aSetup,
        TUInt16 *aLength,
        TUInt8 **aData);

static void USBRamDriveBulkIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);

static void USBRamDriveBulkOut(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus);

static void ISetCSW(void);
static void IMemoryRead(void);
static void IMemoryWrite(TUInt32 aLength);
static void IMemoryVerify(TUInt32 aLength);
static void IGetCBW(TUInt32 aLength);
static void ITestUnitReady(void);
static void DataInTransfer(TUInt32 aLength);

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static TUInt8 G_USBRamDriveBuffer[USB_RAM_DRIVE_MAX_PACKET_SIZE];
static TUInt8 G_USBRamDriveRequest[8];

static T_USBRamDriveCallbacks G_callbacks;
static void *G_callbackWorkspace;
static DEVICE_USB_DEVICE **G_ghDevice;
static T_uezDevice G_usbDev;
static T_uezTask G_ghTask;

TUInt8 G_maxLun = 0x00;
static MSC_CBW CBW;                   // Command Block Wrapper
static MSC_CSW CSW;                   // Command Status Wrapper
static TUInt8 G_stage;
static TUInt32 G_length;
static TUInt32 G_offset;
static TUInt32 G_memorySize;
static TUInt8 *G_memory;
static TBool G_memOK;
static const TUInt32 G_blockSize = 512;

const TUInt8 G_USBRamDrive_USBDescriptorTable[] = {
    // device descriptor
    0x12,
    USB_DESCRIPTOR_DEVICE,
    USB_UINT16(0x0110),			// bcdUSB
    0x00,						// bDeviceClass
    0x00,						// bDeviceSubClass
    0x00,						// bDeviceProtocol
    USB_ENDPOINT0_PACKET_SIZE,			// bMaxPacketSize
    USB_UINT16(0xC251),			// idVendor
    USB_UINT16(0x1303),			// idProduct
    USB_UINT16(0x0100),			// bcdDevice
    0x01,						// iManufacturer
    0x02,						// iProduct
    0x03,						// iSerialNumber
    0x01,						// bNumConfigurations

    // configuration descriptor
    0x09,
    USB_DESCRIPTOR_CONFIGURATION,
    USB_UINT16(9+9+7+7),                                     // wTotalLength (9+9+7+7=33)
    0x01,						// bNumInterfaces
    0x01,						// bConfigurationValue
    0x00,						// iConfiguration
    0xA0,						// bmAttributes
    0x32,						// bMaxPower (100 mA)

    // control class interface
    0x09,
    USB_DESCRIPTOR_INTERFACE,
    0x00,						// bInterfaceNumber
    0x00,						// bAlternateSetting
    0x02,						// bNumEndPoints
    USB_DEVICE_CLASS_STORAGE,   // bInterfaceClass (Vender)
    MSC_SUBCLASS_SCSI,          // bInterfaceSubClass (TBD: ??? Boot Interface code)
    MSC_PROTOCOL_BULK_ONLY,     // bInterfaceProtocol, 1=Keyboard
    0x04,						// iInterface

    // data EP in
    0x07,                                               // bLength
    USB_DESCRIPTOR_ENDPOINT,                            // bDescriptorType
    USB_RAM_DRIVE_EP_IN,   // bEndpointAddress
    USB_ENDPOINT_TYPE_BULK,                        // bmAttributes = interrupt
    USB_UINT16(USB_RAM_DRIVE_MAX_PACKET_SIZE),                   // wMaxPacketSize
    0x0A,                                               // bInterval

    // data EP OUT
    0x07,
    USB_DESCRIPTOR_ENDPOINT,
    USB_RAM_DRIVE_EP_OUT,
    USB_ENDPOINT_TYPE_BULK,		        // bmAttributes = interrupt
    USB_UINT16(USB_RAM_DRIVE_MAX_PACKET_SIZE),           // wMaxPacketSize
    0x0A,						// bInterval (10 ms)

    // string descriptors
    0x04,
    USB_DESCRIPTOR_STRING,
    USB_UINT16(0x0409),

    0x2A,    // string 0x01
    USB_DESCRIPTOR_STRING,
    'F', 0,
    'u', 0,
    't', 0,
    'u', 0,
    'r', 0,
    'e', 0,
    ' ', 0,
    'D', 0,
    'e', 0,
    's', 0,
    'i', 0,
    'g', 0,
    'n', 0,
    's', 0,
    ',', 0,
    ' ', 0,
    'I', 0,
    'n', 0,
    'c', 0,
    '.', 0,

    0x16,   // string 0x02 - Product
    USB_DESCRIPTOR_STRING,
    'U', 0,
    'S', 0,
    'B', 0,
    'R', 0,
    'A', 0,
    'M', 0,
    'D', 0,
    'r', 0,
    'v', 0,
    '.', 0,

    0x0C,   // string 0x03 - Serial Number
    USB_DESCRIPTOR_STRING,
    '0', 0,
    '0', 0,
    '0', 0,
    '0', 0,
    '1', 0,

    0x0E,   // string 0x04 - Interface
    USB_DESCRIPTOR_STRING,
    'M',0,
    'e',0,
    'm',0,
    'o',0,
    'r',0,
    'y',0,

    // terminating zero
    0
};


static void IStatusError(void)
{
    CSW.bStatus = CSW_PHASE_ERROR;
}

static void ISetCSW(void)
{
    CSW.dSignature = MSC_CSW_Signature;
    (*G_ghDevice)->Write(
            G_ghDevice, 
            USB_RAM_DRIVE_EP_IN, 
            (void *)&CSW, 
            13);
    G_stage = STAGE_CSW;
}

/*-------------------------------------------------------------------------*
 * Function:  USBRamDriveBulkOut
 *-------------------------------------------------------------------------*
 * Description:
 *      Bulk virtual comm data has come out of the PC.  This data
 *      is put into the fifo.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
void USBRamDriveBulkOut(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    TUInt16 length;
    
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aStatus);

    // Read data from endpoint
    length = (*G_ghDevice)->Read(
                G_ghDevice,
                aEndpoint,
                G_USBRamDriveBuffer,
                sizeof(G_USBRamDriveBuffer));
  if (CBW.CB[0] == SCSI_VERIFY10) {
      CBW.CB[0] = SCSI_VERIFY10;
  }

    switch (G_stage) {
        case STAGE_CBW:
            IGetCBW(length);
            break;
        case STAGE_DATA_OUT:
            if (CBW.CB[0] == SCSI_WRITE10) {
                IMemoryWrite(length);
                break;
            } else if (CBW.CB[0] == SCSI_VERIFY10) {
                IMemoryVerify(length);
                break;
            }
            break;
        default:
            (*G_ghDevice)->SetStallState(G_ghDevice, aEndpoint, ETrue);
            IStatusError();
            
            ISetCSW();
            break;
    }
}

static void ITestUnitReady(void)
{
    if (CBW.dDataLength != 0) {
        if ((CBW.bmFlags & 0x80) != 0) {
            (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_IN, ETrue);
        } else {
            (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_OUT, ETrue);
        }
    }
    
    CSW.bStatus = CSW_CMD_PASSED;
    ISetCSW();
}

static void DataInTransfer(TUInt32 aLength)
{    
    if (aLength > CBW.dDataLength)
        aLength = CBW.dDataLength;
    
    (*G_ghDevice)->Write(
            G_ghDevice, 
            USB_RAM_DRIVE_EP_IN, 
            G_USBRamDriveBuffer, 
            aLength);
    G_stage = STAGE_DATA_IN_LAST;    
    CSW.dDataResidue -= aLength;
    CSW.bStatus = CSW_CMD_PASSED;
}

static TBool DataInFormat(void) 
{
    if (CBW.dDataLength == 0) {
        IStatusError();
        ISetCSW();
        return EFalse;
    }
    if ((CBW.bmFlags & 0x80) == 0) {
        (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_OUT, ETrue);
        IStatusError();
        ISetCSW();
        return EFalse;
    }
    return ETrue;
}

static void IRequestSense(void)
{
    TUInt8 *p = G_USBRamDriveBuffer;
    if (!DataInFormat()) 
        return;

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
    
    DataInTransfer(18);
}

static void IInquiry(void) 
{
    TUInt8 *p = G_USBRamDriveBuffer;

    if (!DataInFormat()) 
      return;

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
    p[20] = 'R';
    p[21] = 'A';
    p[22] = 'M';
    p[23] = ' ';
    p[24] = 'D';
    p[25] = 'i';
    p[26] = 's';
    p[27] = 'k';
    p[28] = ' ';
    p[29] = ' ';
    p[30] = ' ';
    p[31] = ' ';
    
    p[32] = '1';           // Product Revision Level
    p[33] = '.';
    p[34] = '0';
    p[35] = ' ';
    
    DataInTransfer(36);
}

static void IModeSense6(void) 
{
    TUInt8 *p = G_USBRamDriveBuffer;

    if (!DataInFormat())
        return;
    
    p[ 0] = 0x03;
    p[ 1] = 0x00;
    p[ 2] = 0x00;
    p[ 3] = 0x00;
    
    DataInTransfer(4);
}

static void IModeSense10(void)
{
    TUInt8 *p = G_USBRamDriveBuffer;

    if (!DataInFormat())
        return;
    
    p[ 0] = 0x00;
    p[ 1] = 0x06;
    p[ 2] = 0x00;
    p[ 3] = 0x00;
    p[ 4] = 0x00;
    p[ 5] = 0x00;
    p[ 6] = 0x00;
    p[ 7] = 0x00;
    
    DataInTransfer(8);
}

static void IReadFormatCapacity(void)
{
    TUInt8 *p = G_USBRamDriveBuffer;
    TUInt32 numBlocks;

    if (!DataInFormat())
        return;
    
    p[ 0] = 0x00;
    p[ 1] = 0x00;
    p[ 2] = 0x00;
    p[ 3] = 0x08;          // Capacity List Length
    
    numBlocks = G_memorySize / G_blockSize;
    
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
    
    DataInTransfer(12);
}

static void IReadCapacity(void)
{
    TUInt32 numBlocks;
    TUInt8 *p = G_USBRamDriveBuffer;

    if (!DataInFormat())
        return;
    
    numBlocks = G_memorySize / G_blockSize;

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
    
    DataInTransfer(8);
}

static TBool IRWSetup(void) 
{
    TUInt32 n;
    
    // Logical Block Address of First Block
    n = (CBW.CB[2] << 24) |
        (CBW.CB[3] << 16) |
        (CBW.CB[4] <<  8) |
        (CBW.CB[5] <<  0);
    
    G_offset = n * G_blockSize;
    
    // Number of Blocks to transfer
    n = (CBW.CB[7] <<  8) |
        (CBW.CB[8] <<  0);
    
    G_length = n * G_blockSize;
    
    if (CBW.dDataLength != G_length) {
        (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_IN, ETrue);
        (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_OUT, ETrue);
        IStatusError();
        ISetCSW();
        return EFalse;
    }

    return ETrue;
}

static TBool IRWSetupVerify(void) 
{
    TUInt32 n;
    
    // Logical Block Address of First Block
    n = (CBW.CB[2] << 24) |
        (CBW.CB[3] << 16) |
        (CBW.CB[4] <<  8) |
        (CBW.CB[5] <<  0);
    
    G_offset = n * G_blockSize;
    
    // Number of Blocks to transfer
    n = (CBW.CB[7] <<  8) |
        (CBW.CB[8] <<  0);
    
    G_length = n * G_blockSize;
    
    if (CBW.dDataLength == 0) {
        ISetCSW();
        return EFalse;
    }

    return ETrue;
}

static void IGetCBW(TUInt32 aLength)
{
    TUInt32 n;
    
    for (n=0; n<aLength; n++)
        ((TUInt8 *)&CBW)[n] = G_USBRamDriveBuffer[n];
    if ((aLength == 31) && (CBW.dSignature == MSC_CBW_Signature)) {
        CSW.dTag = CBW.dTag;
        CSW.dDataResidue = CBW.dDataLength;
        if ((CBW.bLUN != 0) || (CBW.bCBLength < 1) || (CBW.bCBLength > 16)) {
            CSW.bStatus = CSW_CMD_FAILED;
            ISetCSW();
        } else {
            switch (CBW.CB[0]) {
                case SCSI_TEST_UNIT_READY:
                    ITestUnitReady();
                    break;
                case SCSI_REQUEST_SENSE:
                    IRequestSense();
                    break;
                case SCSI_INQUIRY:
                    IInquiry();
                    break;
                case SCSI_MODE_SENSE6:
                    IModeSense6();
                    break;
                case SCSI_MODE_SENSE10:
                    IModeSense10();
                    break;
                case SCSI_READ_FORMAT_CAPACITIES:
                    IReadFormatCapacity();
                    break;
                case SCSI_READ_CAPACITY:
                    IReadCapacity();
                    break;
                case SCSI_READ10:
                    if (IRWSetup()) {
                        if ((CBW.bmFlags & 0x80) != 0) {
                            G_stage = STAGE_DATA_IN;
                            IMemoryRead();
                        } else {
                            (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_OUT, ETrue);
                            IStatusError();
                            ISetCSW();
                        }
                    }
                    break;
                case SCSI_WRITE10:
                    if (IRWSetup()) {
                        if ((CBW.bmFlags & 0x80) == 0) {
                            G_stage = STAGE_DATA_OUT;
                        } else {
                            (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_IN, ETrue);
                            IStatusError();
                            ISetCSW();
                        }
                    }
                    break;
                case SCSI_VERIFY10:
                    if (IRWSetupVerify()) {
                        if ((CBW.bmFlags & 0x80) == 0) {
                            G_stage = STAGE_DATA_OUT;
                            G_memOK = ETrue;
                        } else {
                            (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_IN, ETrue);
                            IStatusError();
                            ISetCSW();
                        }
                    }
                    break;
                default:
                case SCSI_FORMAT_UNIT:
                case SCSI_MEDIA_REMOVAL:
                case SCSI_MODE_SELECT6:
                case SCSI_MODE_SELECT10:
                case SCSI_START_STOP_UNIT:
                    CSW.bStatus = CSW_CMD_FAILED;
                    ISetCSW();
                    break;
            }
        }
    } else {
        // Invalid CBW
        (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_IN, ETrue);
        (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_OUT, ETrue);
        G_stage = STAGE_ERROR;
    }
}

static void IMemoryRead(void)
{
    TUInt32 n;
    
    n = G_length;
    if (n > USB_RAM_DRIVE_MAX_PACKET_SIZE)
        n = USB_RAM_DRIVE_MAX_PACKET_SIZE;
    
    if ((G_offset+n) > G_memorySize) {
        n = G_memorySize - G_offset;
        G_stage = STAGE_DATA_IN_LAST_STALL;
    }
    
    (*G_ghDevice)->Write(
            G_ghDevice, 
            USB_RAM_DRIVE_EP_IN, 
            G_memory+G_offset, 
            n);
    G_offset += n;
    G_length -= n;
    
    CSW.dDataResidue -= n;
    
    if (G_length == 0)
        G_stage = STAGE_DATA_IN_LAST;
    
    if (G_stage != STAGE_DATA_IN) {
        // Inactive
        CSW.bStatus = CSW_CMD_PASSED;
        if (G_callbacks.iUSBRamDriveActivity)
            G_callbacks.iUSBRamDriveActivity(G_callbackWorkspace);
    }
}

static void IMemoryWrite(TUInt32 aLength) 
{
    TUInt32 n;
    TUInt32 length = aLength;
    if ((G_offset + aLength) > G_memorySize) {
        length = G_memorySize - G_offset;
        G_stage = STAGE_CSW;
        (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_OUT, ETrue);
    }
    
    for (n=0; n<aLength; n++)
        G_memory[G_offset + n] = G_USBRamDriveBuffer[n];
    
    G_offset += length;
    G_length -= length;
    CSW.dDataResidue -= length;
    
    if ((G_length == 0) || (G_stage == STAGE_CSW)) {
        CSW.bStatus = CSW_CMD_PASSED;
        if (G_callbacks.iUSBRamDriveActivity)
            G_callbacks.iUSBRamDriveActivity(G_callbackWorkspace);
        ISetCSW();
    }
}

static void IMemoryVerify(TUInt32 aLength)
{
    TUInt32 n;
    TUInt32 length = aLength;
    if ((G_offset + length) > G_memorySize) {
        length = G_memorySize - aLength;
        G_stage = STAGE_CSW;
        (*G_ghDevice)->SetStallState(G_ghDevice, USB_RAM_DRIVE_EP_OUT, ETrue);
    }
    
    for (n=0; n<length; n++) {
        if (G_memory[G_offset+n] != G_USBRamDriveBuffer[n]) {
            G_memOK = EFalse;
            break;
        }
    }
    
    G_offset += length;
    G_length -= length;
    CSW.dDataResidue -= length;
    
    if ((G_length == 0) || (G_stage == STAGE_CSW)) {
        CSW.bStatus = (G_memOK)?CSW_CMD_PASSED : CSW_CMD_FAILED;
        ISetCSW();
    }
}

/*-------------------------------------------------------------------------*
 * Function:  USBRamDriveBulkIn
 *-------------------------------------------------------------------------*
 * Description:
 *      The PC is requesting data to be sent back to it.  Pull data
 *      output of the fifo and send it back up to the maximum size
 *      packet.
 * Inputs:
 *      TUInt8 aEndpoint         -- Endpoint with interrupt
 *      T_USBEndpointStatus aStatus -- Current status of this endpoint
 *-------------------------------------------------------------------------*/
static void USBRamDriveBulkIn(
        void *aWorkspace,
        TUInt8 aEndpoint,
        T_USBEndpointStatus aStatus)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aStatus);

    switch (G_stage) {
        case STAGE_DATA_IN:
            if (CBW.CB[0] == SCSI_READ10) {
                if (G_callbacks.iUSBRamDriveActivity)
                    G_callbacks.iUSBRamDriveActivity(G_callbackWorkspace);
                IMemoryRead();
            }
            break;
        case STAGE_DATA_IN_LAST:
            ISetCSW();
            break;
        case STAGE_DATA_IN_LAST_STALL:
            (*G_ghDevice)->SetStallState(G_ghDevice, aEndpoint, ETrue);
            ISetCSW();
            break;
        case STAGE_CSW:
            G_stage = STAGE_CBW;
            break;
    }
}

/*-------------------------------------------------------------------------*
 * Function:  USBRamDriveHandleClassRequest
 *-------------------------------------------------------------------------*
 * Description:
 *      Handle CDC class requests that are required for the virtual
 *      COMM port.  Notify the system each time the baud rate is changed.
 * Inputs:
 *      T_USBSetupPacket *aSetup -- Setup packet with cmd
 *      TUInt16 *aLength         -- Pointer to length of return data
 *      TUInt8 **aData           -- Pointer to start of return data
 * Outputs:
 *      TBool                    -- ETrue if handled, else EFalse.
 *-------------------------------------------------------------------------*/
TBool USBRamDriveHandleClassRequest(
                void *aWorkspace,
                T_USBSetupPacket *aSetup,
                TUInt16 *aLength,
                TUInt8 **aData)
{
    PARAM_NOT_USED(aWorkspace);

//printf("{USBRamDrive.Class $%02X}", aSetup->iRequest);
    switch (aSetup->iRequest) {
        case MSC_REQUEST_RESET:
//            IMSCReset();
            G_stage = STAGE_CBW;
            *aLength = 0;  // For now, do nothing, return nothing
            *aData = 0;
            break;
        case MSC_REQUEST_GET_MAX_LUN:
            // Always report 0 for 1 logical unit
            *aData = &G_maxLun;
            *aLength = sizeof(G_maxLun);
            break;
        default:
            return EFalse;
    }
    return ETrue;
}

TUInt32 USBRamDriveMonitor(T_uezTask aMyTask, void *aParameters)
{
    PARAM_NOT_USED(aParameters);
    PARAM_NOT_USED(aMyTask);

    // Just constantly process endpoint data
    for (;;)  {
        ((*G_ghDevice)->ProcessEndpoints)(G_ghDevice, UEZ_TIMEOUT_INFINITE);
    }
}

/*-------------------------------------------------------------------------*
 * Function:  USBRamDriveInitialize
 *-------------------------------------------------------------------------*
 * Description:
 *      Initialize the Virtual Comm variables.
 *-------------------------------------------------------------------------*/
T_uezError USBRamDriveInitialize(
     T_USBRamDriveCallbacks *aCallbacks,
     void *aCallbackWorkspace,
     void *aBaseAddr,
     TUInt32 aRAMDriveSize)
{
    T_uezError error;

    // Copy over the callback information
    G_callbacks = *aCallbacks;
    G_callbackWorkspace = aCallbackWorkspace;
    G_memorySize = aRAMDriveSize;
    G_memory = aBaseAddr;

    // See if the USBDevice exists
    error = UEZDeviceTableFind(
                "USBDevice",
                &G_usbDev);
    if (error != UEZ_ERROR_NONE)
        return error;

    // Find the workspace for all the routines (we'll just make it
    // global for this cheap configuration).
    error = UEZDeviceTableGetWorkspace(
                G_usbDev,
                (T_uezDeviceWorkspace **)&G_ghDevice);
    if (error != UEZ_ERROR_NONE)
        return error;

    // Turn on Nak interrupts on bulk input
    // so it polls the BulkIn for data when there is none
//    ((*G_ghDevice)->InterruptNakEnable)(G_ghDevice, USB_DEVICE_SET_MODE_INAK_BI);

    // Configure the device driver
    // Tell it our descriptor table
    ((*G_ghDevice)->Configure)(G_ghDevice, G_USBRamDrive_USBDescriptorTable);
    ((*G_ghDevice)->RegisterRequestTypeCallback)(
            G_ghDevice,
            USB_REQUEST_TYPE_CLASS,
            G_USBRamDriveRequest,
            0,
            USBRamDriveHandleClassRequest);
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_IN(2),
            USBRamDriveBulkIn);
    ((*G_ghDevice)->RegisterEndpointCallback)(
            G_ghDevice,
            ENDPOINT_OUT(4),
            USBRamDriveBulkOut);

    // We are ready, let's initialize it and connect
    ((*G_ghDevice)->Initialize)(G_ghDevice);
    ((*G_ghDevice)->Connect)(G_ghDevice);

    // Now create a task that constantly process the USBRamDrive buffers
    error = UEZTaskCreate(
                USBRamDriveMonitor,
                "USBRamDrive",
                256,
                0,
                UEZ_PRIORITY_HIGH,
                &G_ghTask);

    return error;
}

/*-------------------------------------------------------------------------*
 * End of File:  USBRamDrive.c
 *-------------------------------------------------------------------------*/
