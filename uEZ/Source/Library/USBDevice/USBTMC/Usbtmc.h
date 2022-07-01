/*-------------------------------------------------------------------------*
 * File:  USBMSDrive.h
 *-------------------------------------------------------------------------*
 | Description:
 |      USB implementation of the device side of a virtual comm port
 |      following CDC rules.
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#ifndef _USBTMC_H_
#define _USBTMC_H_

// USB488 status byte bit definitions
#define USBTMC_STATUS_ERROR_QUEUE           (0x04)    // error queue -error reported
#define USBTMC_STATUS_QUEST_DATA_SUMM       (0x08)    // questionable data summary
#define USBTMC_STATUS_MAV                   (0x10)    // message available
#define USBTMC_STATUS_SEV                   (0x20)    // standard event summary
#define USBTMC_STATUS_RQS                   (0x40)    // master summary
#define USBTMC_STATUS_SOS                   (0x80)    // standard operation summary

// status bit to maintain state machine -USB488 specification, section 4.3.2
#define USBTMC_CMD_REN_CONTROL              (0x01)
#define USBTMC_CMD_GOTOLOCAL                (0x02)
#define USBTMC_CMD_LLO                      (0x04)
#define USBTMC_CMD_INITIATE_CLEAR           (0x08)
#define USBTMC_CMD_TRIGGER                  (0x10)
#define USBTMC_CMD_INDICATOR                (0x80)

typedef struct
{
    TUInt8 MsgID;
    TUInt8 bTag;
    TUInt8 bTagInverse;
    TUInt8 Reserved_0;
    TUInt32 TransferSize;
} __packed usbTmcHeader_t;

typedef struct 
{
    usbTmcHeader_t header;
    __packed struct bmTransferAttributes 
    {
        TUInt8 EOM         : 1;
        TUInt8 Reserved_1  : 7;
    };
    TUInt8 Reserved_2[3];
    TUInt8 cmd[];
} __packed UsbTmcDevDepMsgOut_t;

typedef struct
{
    usbTmcHeader_t header;
    __packed struct 
    {
        TUInt8 zero            : 1; // must be zero
        TUInt8 termCharEnabled : 1;
        TUInt8 Reserved_1      : 7; // all bits must be zero
    };
    TUInt8 termChar;
    TUInt8 Reserved_2[2];
} __packed UsbTmcReqDevDepMsgIn_t;

typedef struct
{
    usbTmcHeader_t header;
    __packed struct 
    {
        TUInt8 EOM         : 1;
        TUInt8 Reserved_1  : 7;
    };
    TUInt8 Reserved_2[3];
    TUInt8 cmd[];
} __packed UsbTmcDevDepMsgIn_t;

typedef union
{
    TUInt8 data;
    struct
    {
        TUInt8 reserved_0       : 1;
        TUInt8 reserved_1       : 1;
        TUInt8 reserved_2       : 1;
        TUInt8 reserved_3       : 1;
        TUInt8 MAV              : 1;   // message available
        TUInt8 SEV              : 1;   // event status summary bit
        TUInt8 RQS              : 1;   // request service flag
        TUInt8 reserved_4       : 1;
    };
} __packed T_UsbTmcStatusByte;

// NOTE: bit offsets must match local/remote defines above.
typedef union
{
    TUInt32 data;
    struct
    {
        TUInt32 renControl      : 1;
        TUInt32 gotoLocal       : 1;
        TUInt32 llo             : 1;
        TUInt32 initiateClear   : 1;
        TUInt32 trigger         : 1;
        TUInt32 reserved        : 2;
        TUInt32 indicator       : 1;
    };
} __packed T_UsbTmcLocalRemoteStatus;

typedef struct _UEZTMCSettingsData {
    TUInt16 usVendorId;
    TUInt16 usProductId;
    TUInt8* sManf;
    TUInt8* sModel;
    TUInt8* sSerial;
    TUInt8* sVersion;
    TBool   bSupportsSRQ;
    TBool   bSupportsActivity;
    TBool   bSupportsEOS;
    TUInt8  ucEOSChar;
} T_UsbTmcSettings;

T_uezError UsbTmc_open(const char *aDeviceName, T_UsbTmcSettings *settings);
TUInt32 UsbTmc_read(TUInt8 *buf, TUInt32 len, TUInt32 timeout);
TUInt32 UsbTmc_write(TUInt8 *buf, TUInt32 len, TUInt32 timeout);
TBool UsbTmc_getStatus(TUInt32 *tmcstat, TUInt32 timeout);
TBool UsbTmc_setREN(TUInt32 timeout);
TBool UsbTmc_clearREN(TUInt32 timeout);
TBool UsbTmc_getSTB(TUInt8 *iSTB,  TUInt32 timeout);
TBool UsbTmc_setSTBBit(TUInt8 bitVector, TUInt32 timeout);
TBool UsbTmc_clearSTBBit(TUInt8 bitVector, TUInt32 timeout);

#endif // _USBTMC_H_

/*-------------------------------------------------------------------------*
 * End of File:  Usbtmc.h
 *-------------------------------------------------------------------------*/