/*-------------------------------------------------------------------------*
 * File:  SDCard_MS_driver_SPI.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the SDCard MassStorage Device.
 * This implementation supports MMC, SDC version 1, and SDC version 2(SDHC).
 * This implementation was created from the original SDCard_MS_driver_SPI.c
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZGPIO.h>
#include <Types/GPIO.h>
#include <string.h>
#include "SDCard_MS_driver_SPI.h"

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef SDCARD_INIT_TIMEOUT
    #define SDCARD_INIT_TIMEOUT                 4000    // ms, approximate
#endif
#ifndef SDCARD_DEBUG_OUTPUT
    #define SDCARD_DEBUG_OUTPUT 1
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
typedef enum
{
    GO_IDLE_STATE,
    SEND_OP_COND,
    SEND_IF_COND,
    SEND_CSD,
    SEND_CID,
    STOP_TRANSMISSION,
    SEND_STATUS,
    SET_BLOCKLEN,
    READ_SINGLE_BLOCK,
    READ_MULTI_BLOCK,
    SET_BLOCK_COUNT,
    WRITE_SINGLE_BLOCK,
    WRITE_MULTI_BLOCK,
    TAG_SECTOR_START,
    TAG_SECTOR_END,
    UNTAG_SECTOR,
    TAG_ERASE_GRP_START,
    TAG_ERASE_GRP_END,
    UNTAG_ERASE_GRP,
    ERASE,
    LOCK_UNLOCK,
    SD_APP_OP_COND,
    APP_CMD,
    READ_OCR,
    CRC_ON_OFF,

    SD_SEND_OP_COND,
} T_SDCmd;

#define MOREDATA                !0
#define NODATA                  0
#define SDC_SECTOR_SIZE         512 // bytes
#define CSD_SIZE                16 // bytes

// The SD Card Commands (SPI Bus Mode)
// Bit 7 = 0, Bit 6 = 1, Bits 5-0=Command Code
#define     cmdGO_IDLE_STATE        0x40        //0 is the 6-bit command
#define     cmdSEND_OP_COND         0x41        //1
#define     cmdSEND_IF_COND         0x48        //8
#define     cmdSEND_CSD             0x49        //9
#define     cmdSEND_CID             0x4a        //10
#define     cmdSTOP_TRANSMISSION    0x4c        //12
#define     cmdSEND_STATUS          0x4d        //13
#define     cmdSET_BLOCKLEN         0x50        //16
#define     cmdREAD_SINGLE_BLOCK    0x51        //17
#define     cmdREAD_MULTI_BLOCK     0x52        //18
#define     cmdSET_BLOCK_COUNT      0x57        //23
#define     cmdWRITE_SINGLE_BLOCK   0x58        //24
#define     cmdWRITE_MULTI_BLOCK    0x59        //25
#define     cmdTAG_SECTOR_START     0x60        //32
#define     cmdTAG_SECTOR_END       0x61        //33
#define     cmdUNTAG_SECTOR         0x62        //34
#define     cmdTAG_ERASE_GRP_START  0x63        //35
#define     cmdTAG_ERASE_GRP_END    0x64        //36
#define     cmdUNTAG_ERASE_GRP      0x65        //37
#define     cmdERASE                0x66        //38
#define     cmdSD_APP_OP_COND       0x69        //41
#define     cmdLOCK_UNLOCK          0x71        //49
#define     cmdAPP_CMD              0x77        //55
#define     cmdREAD_OCR             0x7a        //58
#define     cmdCRC_ON_OFF           0x7b        //59

#define     cmdSD_SEND_OP_COND      (0x29|0x80|0x40)    //41

/* Card type definitions (iCardType) */
#define CT_MMC              0x01
#define CT_SD1              0x02
#define CT_SD2              0x04
#define CT_SDC              (CT_SD1|CT_SD2)
#define CT_BLOCK            0x08

#define SDC_FLOATING_BUS    0xFF
#define SDC_BAD_RESPONSE    SDC_FLOATING_BUS
#define SDC_ILLEGAL_CMD     0x04
#define SDC_GOOD_CMD        0x00

#define BLOCKLEN_64                 0x0040
#define BLOCKLEN_128                0x0080
#define BLOCKLEN_256                0x0100
#define BLOCKLEN_512                0x0200

/* Data Token */
#define DATA_START_TOKEN            0xFE
#define DATA_MULT_WRT_START_TOK     0xFC
#define DATA_MULT_WRT_STOP_TOK      0xFD

/* Data Response */
#define DATA_ACCEPTED               0x05 // 0b00000101
#define DATA_CRC_ERR                0x0B // 0b00001011
#define DATA_WRT_ERR                0x0D // 0b00001101

#define IGrab()         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()      UEZSemaphoreRelease(p->iSem)

#if SDCARD_DEBUG_OUTPUT
    #define dprintf printf
#else
    #define dprintf(...)
#endif

#ifndef SDCARD_SPI_RATE
#define SDCARD_SPI_RATE   18000
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef union
{
    struct
    {
        TUInt8 field[5];
    } fields;
    struct
    {
        TUInt8 crc;
        TUInt8 addr0;  // NOTE:  This assumes little endian storage!
        TUInt8 addr1;
        TUInt8 addr2;
        TUInt8 addr3;
        TUInt8 cmd;
    } addr;
    struct
    {
        TUInt8 END_BIT:1;
        TUInt8 CRC7:7;
        TUInt32 address;
        TUInt8 command;
    } cmdblock;
}T_cmdPacket;

typedef enum
{
    CMD_RESPONSE_R1,
    CMD_RESPONSE_R1B,
    CMD_RESPONSE_R2,
    CMD_RESPONSE_R3,    // we don't use CMD_RESPONSE_R3 since we don't care about OCR
    CMD_RESPONSE_R7,
    CMD_RESPONSE_R7B,
} T_cmdResponeType;

typedef union
{
    TUInt8 iByte;
    struct
    {
        TUInt8 IN_IDLE_STATE:1;
        TUInt8 ERASE_RESET:1;
        TUInt8 ILLEGAL_CMD:1;
        TUInt8 CRC_ERR:1;
        TUInt8 ERASE_SEQ_ERR:1;
        TUInt8 ADDRESS_ERR:1;
        TUInt8 PARAM_ERR:1;
        TUInt8 B7:1;
    } iStatus;
} T_SDResponseTypeR1;

typedef struct
    {
        TUInt8      iByte0;
        TUInt8      iByte1;
    } T_r2Bytes;

typedef struct {
        TUInt16 IN_IDLE_STATE:1;
        TUInt16 ERASE_RESET:1;
        TUInt16 ILLEGAL_CMD:1;
        TUInt16 CRC_ERR:1;
        TUInt16 ERASE_SEQ_ERR:1;
        TUInt16 ADDRESS_ERR:1;
        TUInt16 PARAM_ERR:1;
        TUInt16 B7:1;
        TUInt16 CARD_IS_LOCKED:1;
        TUInt16 WP_ERASE_SKIP_LK_FAIL:1;
        TUInt16 ERROR:1;
        TUInt16 CC_ERROR:1;
        TUInt16 CARD_ECC_FAIL:1;
        TUInt16 WP_VIOLATION:1;
        TUInt16 ERASE_PARAM:1;
        TUInt16 OUTRANGE_CSD_OVERWRITE:1;
} T_r2Bits;

typedef union
{
    TUInt16 _word;
    T_r2Bytes _bytes;
    T_r2Bits _bits;
} T_SDResponseTypeR2;

typedef struct
{
    TUInt8      iByte0;
    TUInt8      iByte1;
    TUInt8      iByte2;
    TUInt8      iByte3;
    TUInt8      iByte4;
} T_r7Bytes;

typedef struct {
    // Byte 0
    T_SDResponseTypeR1 r1;

    // Bytes 1-4
    TUInt32 COMMAND_VERSION:4;
    TUInt32 RESERVED:16;
    TUInt32 VOLTAGE_ACCEPTED:4;
    TUInt32 CHECK_PATTERN:8;
} T_r7Bits;

typedef union
{
    T_r7Bytes _bytes;
    T_r7Bits _bits;
} T_SDResponseTypeR7;

typedef struct
{
    TUInt8      iByte0;
    TUInt8      iByte1;
    TUInt8      iByte2;
    TUInt8      iByte3;
    TUInt8      iByte4;
} T_r3Bytes;

typedef struct {
    // Byte 0
    T_SDResponseTypeR1 r1;

    // Bytes 1-4
    TUInt32 OCR:32;
} T_r3Bits;

typedef union
{
    T_r3Bytes _bytes;
    T_r3Bits _bits;
} T_SDResponseTypeR3;


typedef union
{
    T_SDResponseTypeR1  r1;
    T_SDResponseTypeR2  r2;
    T_SDResponseTypeR3  r3;
    T_SDResponseTypeR7  r7;
} T_SDResponse;


typedef struct
{
    TUInt8             CmdCode;                // the command number
    TUInt8             CRC;             // the CRC value (CRC's are
                                                // not required once you turn
                                                // the option off!)
    T_cmdResponeType    iResponseType;      // the Response Type
    TUInt8             iIsMoreDataExpected;    // True if more data is expected
    TBool           iDoSend8Clocks;
} T_cmdEntry ;

typedef TUInt8 T_csd[CSD_SIZE];

typedef struct {
    const DEVICE_MassStorage *iDevice;
    DEVICE_SPI_BUS **iSPI;
    HAL_GPIOPort **iGPIOPort;
    TUInt32 iGPIOBit;
    T_csd G_csd;
    TUInt8 G_sector[512];
    TUInt8 iCmd[50];
    SPI_Request r;
    TBool iSWWriteProtect;
    TUInt32 iRCA;       // Relative Card Address
    TUInt32 iOCR;
    TUInt16 iCardType;
    TBool iInitPerformed;
    T_uezSemaphore iSem;
} T_MassStorage_SDCard_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*********************************************************************
 * G_cmdTable
 * - Provides information for all the sdmmc commands that we support
 *
 * Notes: We turn off the CRC as soon as possible, so the commands with
 *        0xFF don't need to be calculated in runtime
 *********************************************************************/
const T_cmdEntry G_cmdTable[] =
{
    // cmd                  crc   response
    {cmdGO_IDLE_STATE,      0x95, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdSEND_OP_COND,       0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdSEND_IF_COND,       0x87, CMD_RESPONSE_R7B, NODATA,      ETrue},
    {cmdSEND_CSD,           0xFF, CMD_RESPONSE_R1,  MOREDATA,    EFalse},
    {cmdSEND_CID,           0xFF, CMD_RESPONSE_R1,  MOREDATA,    ETrue},
    {cmdSTOP_TRANSMISSION,  0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdSEND_STATUS,        0xFF, CMD_RESPONSE_R2,  NODATA,      ETrue},
    {cmdSET_BLOCKLEN,       0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdREAD_SINGLE_BLOCK,  0xFF, CMD_RESPONSE_R1,  MOREDATA,    ETrue},
    {cmdREAD_MULTI_BLOCK,   0xFF, CMD_RESPONSE_R1,  MOREDATA,    ETrue},
    {cmdSET_BLOCK_COUNT,    0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdWRITE_SINGLE_BLOCK, 0xFF, CMD_RESPONSE_R1,  MOREDATA,    ETrue},
    {cmdWRITE_MULTI_BLOCK,  0xFF, CMD_RESPONSE_R1,  MOREDATA,    ETrue},
    {cmdTAG_SECTOR_START,   0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdTAG_SECTOR_END,     0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdUNTAG_SECTOR,       0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdTAG_ERASE_GRP_START, 0xFF, CMD_RESPONSE_R1, NODATA,      ETrue},
    {cmdTAG_ERASE_GRP_END,  0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdUNTAG_ERASE_GRP,    0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdERASE,              0xFF, CMD_RESPONSE_R1B, NODATA,      ETrue},
    {cmdLOCK_UNLOCK,        0xFF, CMD_RESPONSE_R1B, NODATA,      ETrue},
    {cmdSD_APP_OP_COND,     0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdAPP_CMD,            0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
    {cmdREAD_OCR,           0xFF, CMD_RESPONSE_R7B, NODATA,      EFalse},
    {cmdCRC_ON_OFF,         0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},

    {cmdSD_SEND_OP_COND,    0xFF, CMD_RESPONSE_R1,  NODATA,      ETrue},
};

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static T_uezError ISDCard_MS_GetSizeInfo(void *aWorkspace, T_msSizeInfo *aInfo);

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_SDCard_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a MassStorage device's workspace on the SDCard.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_SDCard_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_MassStorage_SDCard_Workspace *p = (T_MassStorage_SDCard_Workspace *)aWorkspace;

    p->iCardType = 1;
    p->iInitPerformed = EFalse;

    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_SDCard_ConfigureSPI
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the SPI settings specific to a given platform.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 *      DEVICE_SPI_BUS **aSPI        -- The given SPI platform structure.
 *      HAL_GPIOPort **aGPIOPort     -- The given IO Port Address
 *      TUInt32 **aGPIOBit           -- The given IO Port Bit
 *---------------------------------------------------------------------------*/
void MassStorage_SDCard_ConfigureSPI(
        void *aWorkspace,
        DEVICE_SPI_BUS **aSPI,
        HAL_GPIOPort **aGPIOPort,
        TUInt32 aGPIOBit)
{
    T_MassStorage_SDCard_Workspace *p = (T_MassStorage_SDCard_Workspace *)aWorkspace;
    p->iSPI = aSPI;
    p->iGPIOPort = aGPIOPort;
    p->iGPIOBit = aGPIOBit;

    // Set common SPI request block
    p->r.iDataMOSI = p->iCmd;
    p->r.iDataMISO = p->iCmd;
    p->r.iNumTransfers = 0;
    p->r.iBitsPerTransfer = 8;
    p->r.iRate = SDCARD_SPI_RATE;

    p->r.iClockOutPolarity = EFalse;
    p->r.iClockOutPhase = EFalse;
    p->r.iCSGPIOPort = p->iGPIOPort;
    p->r.iCSGPIOBit = p->iGPIOBit;
    p->r.iCSPolarity = EFalse; // LOW true
    p->iSWWriteProtect = EFalse;
    p->iRCA = 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISendSPI
 *---------------------------------------------------------------------------*
 * Description:
 *      Used to transfer 8 bits into the SPI Port, and transfer 8 bits from
 * the SPI Port.
 * Inputs:
 *      T_MassStorage_SDCard_Workspace *p -- Workspace Pointer to this
 *                                           instance.
 *      TUInt32 aValue                    -- Value to send to the SPI Port.
 * Outputs:
 *      TUInt32                           -- Value received from the SPI Port.
 *---------------------------------------------------------------------------*/
static TUInt32 ISendSPI(T_MassStorage_SDCard_Workspace *p, TUInt32 aValue)
{
    return (*p->iSPI)->TransferInOut(p->iSPI, &p->r, aValue);
}

/*---------------------------------------------------------------------------*
 * Routine:  ISendSPIBytes
 *---------------------------------------------------------------------------*
 * Description:
 *      Used to transfer the given number of bytes into the SPI Port,
 * and transfer the given number of bytes from the SPI Port.
 * Inputs:
 *      T_MassStorage_SDCard_Workspace *p -- Workspace Pointer to this
 *                                           instance.
 *      TUInt8* aBytesIn                  -- Pointer to bytes in.
 *      TUInt8* aBytesOut                 -- Pointer to bytes out.
 *      TUInt32 aNumBytes                 -- Given number of in/out bytes.
 * Outputs:
 *      T_uezError                        -- Returns UEZ_ERROR_NONE for
 *                                           success.
 *---------------------------------------------------------------------------*/
static T_uezError ISendSPIBytes(
                                T_MassStorage_SDCard_Workspace *p,
                                TUInt8* aBytesIn,
                                TUInt8* aBytesOut,
                                TUInt32 aNumBytes
                               )
{
    T_uezError error = UEZ_ERROR_NONE;

    // Direct to the given pointers
    p->r.iDataMOSI = aBytesOut;
    p->r.iDataMISO = aBytesIn;
    p->r.iNumTransfers = aNumBytes;

    error = (*p->iSPI)->TransferInOutBytes(p->iSPI, &p->r);

    // Put back into command mode
    p->r.iDataMOSI = p->iCmd;
    p->r.iDataMISO = p->iCmd;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISPIEnd
 *---------------------------------------------------------------------------*
 * Description:
 *      Close the SPI Port.  Used when an entire transaction is totally
 * completed.
 * Inputs:
 *      T_MassStorage_SDCard_Workspace *p -- Workspace Pointer to this
 *                                           instance.
 *---------------------------------------------------------------------------*/
static void ISPIEnd(T_MassStorage_SDCard_Workspace *p)
{
    (*p->iSPI)->Stop(p->iSPI, &p->r);
}

/*---------------------------------------------------------------------------*
 * Routine:  SendCmd
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a command to the given MMC or SDv1 or SDv2 Card.
 * Inputs:
 *      T_MassStorage_SDCard_Workspace *p -- Workspace Pointer to this
 *                                           instance.
 *      TUInt8 aCmd                       -- Command to send from enumeration.
 *      TUInt32 aAddress                  -- Target address to receive.
 * Outputs:
 *      TUInt8                            -- Command response.
 *---------------------------------------------------------------------------*/
static T_SDResponse SendCmd(
            T_MassStorage_SDCard_Workspace *p,
            TUInt8 aCmd,
            TUInt32 aAddress)
{
    TUInt8 index;
    TUInt32 timeout;
    T_SDResponse response;
    TUInt8 cmd[6];
    TUInt32 start;

    // Is this a application specific code?
    if (G_cmdTable[aCmd].CmdCode & 0x80) {
        // Send prefix for app specific command
        response = SendCmd(p, APP_CMD, 0);
        if (response.r1.iByte > 0x01) {
            return response;
        }
    }

    (*p->iSPI)->Start(p->iSPI, &p->r);

    cmd[0] = G_cmdTable[aCmd].CmdCode & 0x7F;
    cmd[1] = (aAddress>>24);
    cmd[2] = (aAddress>>16);
    cmd[3] = (aAddress>>8);
    cmd[4] = (aAddress>>0);
    cmd[5] = G_cmdTable[aCmd].CRC ;

    // Send Command with the address being the first MSB TUInt8
    ISendSPI(p, cmd[0]);
    ISendSPI(p, cmd[1]);
    ISendSPI(p, cmd[2]);
    ISendSPI(p, cmd[3]);
    ISendSPI(p, cmd[4]);
    ISendSPI(p, cmd[5]);

    // see if  we are going to get a response
    if ((G_cmdTable[aCmd].iResponseType == CMD_RESPONSE_R1) ||
            (G_cmdTable[aCmd].iResponseType == CMD_RESPONSE_R1B))  {
        start = UEZTickCounterGet();
        while (1) {
            response.r1.iByte = ISendSPI(p, 0xFF);
            if (response.r1.iByte != 0xFF)
                break;
            if (UEZTickCounterGetDelta(start) > 1000) {
                ISPIEnd(p);
                return response;
            }
        }
    } else if (G_cmdTable[aCmd].iResponseType == CMD_RESPONSE_R2) {
        response.r2._bytes.iByte1 = ISendSPI(p, 0xFF);
        response.r2._bytes.iByte0 = ISendSPI(p, 0xFF);
    } else if (G_cmdTable[aCmd].iResponseType == CMD_RESPONSE_R7) {
        response.r7._bytes.iByte0 = ISendSPI(p, 0xFF);
        response.r7._bytes.iByte4 = ISendSPI(p, 0xFF);
        response.r7._bytes.iByte3 = ISendSPI(p, 0xFF);
        response.r7._bytes.iByte2 = ISendSPI(p, 0xFF);
        response.r7._bytes.iByte1 = ISendSPI(p, 0xFF);
    } else if (G_cmdTable[aCmd].iResponseType == CMD_RESPONSE_R3) {
        response.r3._bytes.iByte0 = ISendSPI(p, 0xFF);
        response.r3._bytes.iByte4 = ISendSPI(p, 0xFF);
        response.r3._bytes.iByte3 = ISendSPI(p, 0xFF);
        response.r3._bytes.iByte2 = ISendSPI(p, 0xFF);
        response.r3._bytes.iByte1 = ISendSPI(p, 0xFF);
    }

    if (G_cmdTable[aCmd].iResponseType == CMD_RESPONSE_R7B)  {
        start = UEZTickCounterGet();
        while (1) {
            response.r7._bytes.iByte0 = ISendSPI(p, 0xFF);
            if (response.r7._bytes.iByte0 != 0xFF)
                break;
            if (UEZTickCounterGetDelta(start) > 1000) {
                ISPIEnd(p);
                return response;
            }
        }
        response.r7._bytes.iByte4 = ISendSPI(p, 0xFF);
        response.r7._bytes.iByte3 = ISendSPI(p, 0xFF);
        response.r7._bytes.iByte2 = ISendSPI(p, 0xFF);
        response.r7._bytes.iByte1 = ISendSPI(p, 0xFF);
    }
    if (G_cmdTable[aCmd].iResponseType == CMD_RESPONSE_R1B)  {
        response.r1.iByte = 0x00;

        for (index =0; index < 0xFF && response.r1.iByte == 0x00; index++)  {
            timeout = 0xFFFF;
            do {
                response.r1.iByte = ISendSPI(p, 0xFF);
                timeout--;
            } while ((response.r1.iByte == 0x00) && (timeout != 0));
        }
    }

    // Required extra clocking (see spec)
    if (G_cmdTable[aCmd].iDoSend8Clocks)
        ISendSPI(p, 0xFF);

    // see if we are expecting data or not
    if (!(G_cmdTable[aCmd].iIsMoreDataExpected))
        (*p->iSPI)->Stop(p->iSPI, &p->r);

    return response;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCardSectorRead
 *---------------------------------------------------------------------------*
 * Description:
 *      Send a request to read a sector worth of data
 * Inputs:
 *      T_MassStorage_SDCard_Workspace *p -- Workspace Pointer to this
 *                                           instance.
 *      T_uint32 aSectorAddr              -- Sector number
 *      TUInt8 *aBuffer                   -- Pointer to receiving buffer
 *                                           (512 bytes)
 * Outputs:
 *      T_uezError                        -- UEZ_ERROR_NONE if no error.
 *---------------------------------------------------------------------------*/
static T_uezError ISDCardSectorRead(
                T_MassStorage_SDCard_Workspace *p,
                TUInt32 aSectorAddr,
                TUInt8 *aBuffer)
{
    TUInt32 index;
    T_SDResponse response;
    TUInt8 data_token;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt8 timeout;
    TUInt32 sector = aSectorAddr ;

    // send the cmd to read a block (starts, not done yet)
    if (!(p->iCardType & CT_BLOCK))  {
        sector = aSectorAddr << 9 ;
    }

    response = SendCmd(p, READ_SINGLE_BLOCK, sector);

    while (1)  {
        // Make sure the command was accepted
        if (response.r1.iByte != 0x00) {
            timeout = 255;
            while (timeout) {
                response.r1.iByte = ISendSPI(p, 0xFF);
                if (response.r1.iByte == 0x00)
                    break;
                timeout--;
            }
            if (timeout == 0) {
                ISPIEnd(p);
                return UEZ_ERROR_NAK;
            }
        }

        //Now, must wait for the start token of data block
        // Hopefully that zero is the datatoken
        // This timeout is based on a 24 MHz clock.  Running this loop
        // 300,000 times at 8 bits per SPI is roughly 100 ms.
        // Therefore this timeout is roughly 100 ms. (it is longer
        // if the clock rate is lower.  The default is 18 MHz for most
        // designs, but may go as low as 12 MHz, or about 200 ms).
        // In any case, the timeout is mostly to keep us from being
        // trapped here forever. -- lshields 10/18/2011
        // This timeout was increased from its original 0x2FF because
        // cstocker found SDCards with pecularly long response times.
        index = 300000;
        do  {
            data_token = ISendSPI(p, 0xFF);
            index--;
        } while((data_token == SDC_FLOATING_BUS) && (index != 0));
        if ((index == 0) || (data_token != DATA_START_TOKEN))  {
            error = UEZ_ERROR_TIMEOUT;
            break;
        }

        //Reads in 512-byte of data
        ISendSPIBytes(p, aBuffer, 0, SDC_SECTOR_SIZE);

        // Read the next 2 bytes of the CRC (well, we really
        // are just ignoring them here)
        ISendSPI(p, 0xFF);
        ISendSPI(p, 0xFF);

        ISendSPI(p, 0xFF);
        break;
    }

    // Recover the SPI bus
    ISPIEnd(p);

    return error;
}


/*---------------------------------------------------------------------------*
 * Routine:  IReadCardSpecificData
 *---------------------------------------------------------------------------*
 * Description:
 *      Send the CMD9 to read the card specific data.
 * Inputs:
 *      T_MassStorage_SDCard_Workspace *p -- Workspace Pointer to this
 *                                           instance.
 * Outputs:
 *      T_uezError                        -- UEZ_ERROR_NONE if no error.
 *---------------------------------------------------------------------------*/
static T_uezError IReadCardSpecificData(T_MassStorage_SDCard_Workspace *p)
{
    TUInt16 index;
    T_SDResponse response;
    TUInt8 data_token;
    T_uezError status = UEZ_ERROR_NONE ;

    while (1)  {
        response = SendCmd(p, SEND_CSD, 0); // This command leaves it open for more data, need Stop called

        // Make sure the command was accepted (non-zero = error)
        if (response.r1.iByte != 0x00)  {
            status = UEZ_ERROR_ILLEGAL_OPERATION;
            break;
        }

        //Now, must wait for the start token of data block
        index = 0x4FF*2;
        do {
            data_token = ISendSPI(p, 0xFF);
            index--;
        } while ((data_token == SDC_FLOATING_BUS) && (index != 0));

        // Time out
        if (index==0)  {
            status = UEZ_ERROR_TIMEOUT;
            break;
        }

        // Got something other than a proper data start token?
        if (data_token != DATA_START_TOKEN)  {
            status = UEZ_ERROR_NOT_FOUND;
            break;
        }

        // Ok, we're now reading data
        memset(p->G_csd, 0xFF, CSD_SIZE);
        ISendSPIBytes(p, p->G_csd, p->G_csd, CSD_SIZE);

        ISendSPI(p, 0xFF);
        break;
    }

    // End the transaction
    ISPIEnd(p);

    return(status);
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_Init
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize MMC, SDv1, or SDv2 Card Disk Drive.
 * Inputs:
 *      void *aWorkspace                  -- Workspace for this instance.
 *      TUInt32 aAddress                  -- Required to match
 *                                           DEVICE_MassStorage structure.
 * Outputs:
 *      T_uezError                        -- UEZ_ERROR_NONE means Success,
 *                                           other T_uezError values
 *                                           indicate failure.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_Init(void *aWorkspace, TUInt32 aAddress)
{
    T_uezError error;
    TInt32 timeout;
    TUInt32 startTimer ;
    T_SDResponse response;
    T_MassStorage_SDCard_Workspace *p = (T_MassStorage_SDCard_Workspace *)aWorkspace;
    TUInt32 i;
    T_msSizeInfo si;
    TUInt8 ty, cmd ;

    IGrab();

    /* Ensure we are not active */
    ISPIEnd(p);
    (*p->iSPI)->Start(p->iSPI, &p->r);
    ISPIEnd(p);

    /* Clock many times to activate it */
    for (i=0; i<10; i++)
        (*p->iSPI)->TransferInOut(p->iSPI, &p->r, 0xFF);

    ty = 0 ;
    /* Enter Idle state */
    /* Command 0 */
    /* Send CMD0 to reset the media */
    response = SendCmd(p, GO_IDLE_STATE, 0x0);
    if (response.r1.iByte == SDC_BAD_RESPONSE) {
        IRelease();
        return UEZ_ERROR_DEVICE_NOT_FOUND;
    }

    /* See if the device is ready */
    if (response.r1.iByte != 0x01) {
        IRelease();
        return UEZ_ERROR_NOT_AVAILABLE;
    }

    /* Now in Idle state */
    /* Initialization timeout of 1000 msec */
    startTimer = UEZTickCounterGet() ;
    /* SDC version 2 */
    /* Command 8 */
    response = SendCmd(p, SEND_IF_COND, 0x1AA) ;
    if (response.r7._bytes.iByte0 == 0x01) {
        /* Check trailing return value of R7 response */
        /* The card can work at vdd range of 2.7-3.6V */
        if ((response.r7._bytes.iByte2 == 0x01) && (response.r7._bytes.iByte1 == 0xAA)) {
            /* Wait for leaving idle state (ACMD41 with HCS bit) */
            /* Command 41 */
            response = SendCmd(p, SD_SEND_OP_COND, 0x40000000) ;
            while ((UEZTickCounterGetDelta(startTimer) < 1000) && (response.r1.iByte)) {
                response = SendCmd(p, SD_SEND_OP_COND, 0x40000000) ;
            }
            if (UEZTickCounterGetDelta(startTimer) < 1000) {
                /* Check CCS bit in the OCR */
                /* Command 58 */
                response = SendCmd(p, READ_OCR, 0) ;
                if (response.r7._bytes.iByte0 == 0x00) {
                    /* Check trailing return value of R7 response */
                    if (response.r7._bytes.iByte4 & 0x40) {
                        /* SDC version 2 Block Device */
                        ty = CT_SD2 | CT_BLOCK ;
                    } else {
                        /* SDC version 2 non block device */
                        ty = CT_SD2 ;
                    }
                }
            }
        }
    } else {
        /* SDC version 1 or MMC version 3 */
        /* Command 41 */
        response = SendCmd(p, SD_APP_OP_COND, 0) ;
        if (response.r1.iByte <= 0x01) {
            /* SDC version 1 */
            ty = CT_SD1 ;
            /* Command 41 */
            cmd = SD_APP_OP_COND ;
        } else {
            /* MMC version 3 */
            ty = CT_MMC ;
            /* Command 1 */
            cmd = SEND_OP_COND ;
        }
        /* Wait for leaving idle state */
        while (UEZTickCounterGetDelta(startTimer) < 1000) {
            response = SendCmd(p, cmd, 0) ;
            if (response.r1.iByte == 0x00) {
                ISPIEnd(p) ;
                break ;
            }
        }
        if (UEZTickCounterGetDelta(startTimer) > 1000) {
            ty = 0 ;
        }
    }
    /* Set the card type to MMCv3, SDCv1, or SDCv2 */
    p->iCardType = ty ;

    /* Ensure we are not active. */
    ISendSPI(p, 0xFF) ;
    ISPIEnd(p) ;
    ISendSPI(p, 0xFF) ;

    /* Flush out any further data in the SPI buffers. */
    startTimer = UEZTickCounterGet() ;
    do {
        cmd = ISendSPI(p, 0xFF) ;
        if (UEZTickCounterGetDelta(startTimer) > 500) {
            break ;
        }
    } while (cmd != 0xFF) ;
    if (cmd != 0xFF) {
        ISendSPI(p, 0xFF) ;
        ISPIEnd(p) ;
        IRelease();
        return UEZ_ERROR_TIMEOUT ;
    }

    /* Read the CSD and return an error if we got one */
    if (IReadCardSpecificData(p)) {
        IRelease();
        return UEZ_ERROR_UNKNOWN;
    }

    /* Set the OCR to zero. */
    p->iOCR = 0;

    /* Ok Now set the block length to 512. It should be already */
    /* (ignore response)                                        */
    SendCmd(p, SET_BLOCKLEN, BLOCKLEN_512);

    /* read it a couple times until we sucessfully read it, seen san disks that really do not */
    /* like this init routine for somereason, almost have to prime the device...              */
    for (timeout=255; timeout>0; timeout--)
        if (ISDCardSectorRead(p, 0x10, p->G_sector) == UEZ_ERROR_NONE)
            break;

    /* Could we never read one sector correctly?  Return an error */
    if (timeout == 0)
        error = UEZ_ERROR_NOT_FOUND;

    error = ISDCard_MS_GetSizeInfo(aWorkspace, &si);

    if (ty) {
        /* Initialization succeded */
        p->iInitPerformed = ETrue;
    } else {
        /* Initialization failed */
        p->iInitPerformed = EFalse;

        error = UEZ_ERROR_DEVICE_NOT_FOUND ;
    }

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_Status
 *---------------------------------------------------------------------------*
 * Description:
 *      Send initialized status.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      T_msStatus *aStatus      -- Status value to send.
 * Outputs:
 *      T_uezError               -- always returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_Status(void *aWorkspace, T_msStatus *aStatus)
{
    T_msStatus status;
    T_MassStorage_SDCard_Workspace *p = (T_MassStorage_SDCard_Workspace *)aWorkspace;

    IGrab();

    status = 0;

    // Unfortunately, there is no hardware detection pins
    // We'll just point out when we need to initialize
    if (!p->iInitPerformed)
        status |= MASS_STORAGE_STATUS_NEED_INIT;

    *aStatus = status;

    IRelease();

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_Read
 *---------------------------------------------------------------------------*
 * Description:
 *      Read Sector(s).
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      const TUInt32 aStart     -- Starting block or byte.
 *      const TUInt32 aNumBlocks -- Number of blocks to read.
 *      void *aBuffer            -- Pointer to the data buffer to store read
 *                                  data.
 * Outputs:
 *      T_uezError               -- UEZ_ERROR_NONE means Success,
 *                                  other T_uezError values
 *                                  indicate failure.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_Read(
            void *aWorkspace,
            const TUInt32 aStart,
            const TUInt32 aNumBlocks,
            void *aBuffer)
{
    T_uezError error;
    T_MassStorage_SDCard_Workspace *p =
        (T_MassStorage_SDCard_Workspace *)aWorkspace;
    TUInt32 block = aStart;
    TUInt32 num = aNumBlocks;

    IGrab();

    // Although there are faster ways to do this, we'll
    // read one block at a time for now and let someone optimize it later
    while (num--) {
        error = ISDCardSectorRead(
                    p,
                    block,
                    aBuffer);
        if (error != UEZ_ERROR_NONE) {
            IRelease();
            return error;
        }
        aBuffer = (void *)(((char *)aBuffer)+512);
        block++;
    }

    IRelease();
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISDCard_WriteSingleBlock
 *---------------------------------------------------------------------------*
 * Description:
 *      Write one Sector block or bytes.
 * Inputs:
 *      T_MassStorage_SDCard_Workspace *p -- Workspace Pointer to this
 *                                           instance.
 *      const TUInt32 aBlockNum           -- Start sector number (LBA) block
 *                                           or byte.
 *      const void *aBuffer               -- Pointer to the data to be written.
 * Outputs:
 *      T_uezError                        -- UEZ_ERROR_NONE means Success,
 *                                           other T_uezError values
 *                                           indicate failure.
 *---------------------------------------------------------------------------*/
static T_uezError ISDCard_WriteSingleBlock(
                    T_MassStorage_SDCard_Workspace *p,
                    const TUInt32 aBlockNum,
                    const void *aBuffer)
{
    T_SDResponse response;
    TUInt32 start;
    TUInt32 timeout;
    TUInt32 sector = aBlockNum ;


    // Start the block, must do stop to end afterward
    if (!(p->iCardType & CT_BLOCK)) {
        sector = aBlockNum << 9 ;
    }

    response = SendCmd(p, WRITE_SINGLE_BLOCK, sector);

    // Make sure the command was accepted
    if (response.r1.iByte != 0x00) {
        timeout = 255;
        while (timeout) {
            response.r1.iByte = ISendSPI(p, 0xFF);
            if (response.r1.iByte == 0x00)
                break;
            timeout--;
        }
        if (timeout == 0) {
            ISPIEnd(p);
            return UEZ_ERROR_NAK;
        }
    }

    //Send data begin token (bit 0 = 0)
    ISendSPI(p, 0xFE);

    //Send the data
    ISendSPIBytes(p, 0, (TUInt8 *)aBuffer, SDC_SECTOR_SIZE);

    //Send checksum bytes
    ISendSPI(p, 0xFF);
    ISendSPI(p, 0xFF);

    // Send dummy data to get response info
    response.r1.iByte = ISendSPI(p, 0xFF);

    // Was the data accepted?
    if ((response.r1.iByte & 0x0F) != 0x05) {
        ISPIEnd(p);

        if ((response.r1.iByte & 0x0F) == 11)
            return UEZ_ERROR_NAK; // CRC failure

        return UEZ_ERROR_NAK;
    }

    // Now wait for completing of the write cycle
    start = UEZTickCounterGet();
    while (1) {
        if (UEZTickCounterGetDelta(start) >= 1000) {
            ISPIEnd(p);
            ISendSPI(p, 0xFF);
            return UEZ_ERROR_TIMEOUT;
        }
        response.r1.iByte = ISendSPI(p, 0xFF);
        if (response.r1.iByte == 0x00)
            break;
    }

    start = UEZTickCounterGet();
    while (1) {
        response.r1.iByte = ISendSPI(p, 0xFF);
        if (response.r1.iByte == 0xFF)
            break;
        if (UEZTickCounterGetDelta(start) >= 1000) {
            ISPIEnd(p);
            ISendSPI(p, 0xFF);
            return UEZ_ERROR_TIMEOUT;
        }
    }

    response.r1.iByte = ISendSPI(p, 0xFF);

    ISPIEnd(p);
    ISendSPI(p, 0xFF);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_Write
 *---------------------------------------------------------------------------*
 * Description:
 *      Write Sector(s).
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      const TUInt32 aStart     -- Start sector number (LBA) block or byte.
 *      const TUInt32 aNumBlocks -- Sector count to write.
 *      const void *aBuffer      -- Pointer to the data to be written.
 * Outputs:
 *      T_uezError               -- UEZ_ERROR_NONE means Success,
 *                                  other T_uezError values
 *                                  indicate failure.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_Write(
            void *aWorkspace,
            const TUInt32 aStart,
            const TUInt32 aNumBlocks,
            const void *aBuffer)
{
    T_MassStorage_SDCard_Workspace *p =
        (T_MassStorage_SDCard_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 num = aNumBlocks;
    TUInt32 block = aStart;

    IGrab();

    // Write each block in order
    while (num--) {
        error = ISDCard_WriteSingleBlock(p, block, aBuffer);
        if (error)
            break;
        block++;
        aBuffer = (void *)(((char *)aBuffer)+SDC_SECTOR_SIZE);
    }

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IWaitReady
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for status bit to return 0x08.
 * Inputs:
 *      T_MassStorage_SDCard_Workspace *p -- Workspace Pointer to this
 *                                           instance.
 * Outputs:
 *      T_uezError                        -- UEZ_ERROR_NONE means Success,
 *                                           other T_uezError values
 *                                           indicate failure.
 *---------------------------------------------------------------------------*/
static T_uezError IWaitReady(T_MassStorage_SDCard_Workspace *p)
{
    T_SDResponse response;
    TUInt32 start = UEZTickCounterGet();

    while (UEZTickCounterGetDelta(start) < 500) {
        response = SendCmd(p, SEND_STATUS, p->iRCA<<16);
        if ((response.r2._word & 0x01E00) == 0x0800)
            return UEZ_ERROR_NONE;
    }
    return UEZ_ERROR_TIMEOUT;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_Sync
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait until we are ready or timeout.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 * Outputs:
 *      T_uezError               -- UEZ_ERROR_NONE means Success,
 *                                  other T_uezError values
 *                                  indicate failure.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_Sync(void *aWorkspace)
{
    T_MassStorage_SDCard_Workspace *p =
        (T_MassStorage_SDCard_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();

    // Just wait until we are ready or timeout
    error = IWaitReady(p);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISDCard_MS_GetSizeInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Fill in the workspace size variables
 * Inputs:
 *      void *aWorkspace      -- Workspace for this instance.
 *      T_msSizeInfo *aInfo   -- Pointer to a structure to receive info.
 * Outputs:
 *      T_uezError            -- always returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError ISDCard_MS_GetSizeInfo(void *aWorkspace, T_msSizeInfo *aInfo)
{
    T_MassStorage_SDCard_Workspace *p =
        (T_MassStorage_SDCard_Workspace *)aWorkspace;
    TUInt8 *p_csd = (TUInt8 *)&p->G_csd;
    TUInt32 v;
    TUInt32 n;

    // Get the number of sectors
    // What SDC version?
    if ((p_csd[0] >> 6) == 1) {
        // SDC ver 2.00
        v = p_csd[9] + ((TUInt16)p_csd[8] << 8) + 1;
        aInfo->iNumSectors = v << 10;
    } else {
        // MMC or SDC ver 1.XX
        n = (p_csd[5] & 15) + ((p_csd[10] & 128)>>7) + ((p_csd[9] & 3) << 1) + 2;
        v = (p_csd[8] >> 6) + ((TUInt16)p_csd[7] << 2) + ((TUInt16)(p_csd[6] & 3) << 10) + 1;
        aInfo->iNumSectors = (TUInt32)v << (n-9);
    }

    // Get the size of the sectors (just hard code to 512)
    aInfo->iSectorSize = 512;

    // Determine the block size
    // SDC or MMC?
    if (p->iCardType & 2) {
        // SDC
        aInfo->iBlockSize = (((p_csd[10] & 63) << 1) + ((TUInt16)(p_csd[11] & 128) >> 7) + 1) << ((p_csd[13] >> 16) - 1);
    } else {
        // MMC
        aInfo->iBlockSize = ((TUInt16)((p_csd[10] & 124) >> 2) + 1) * (((p_csd[11] & 3) << 3) + ((p_csd[11] & 224) >> 5) + 1);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_GetSizeInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Fill in the workspace size variables
 * Inputs:
 *      void *aWorkspace      -- Workspace for this instance.
 *      T_msSizeInfo *aInfo   -- Pointer to a structure to receive info.
 * Outputs:
 *      T_uezError            -- always returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_GetSizeInfo(void *aWorkspace, T_msSizeInfo *aInfo)
{
    T_uezError error;
    T_MassStorage_SDCard_Workspace *p =
        (T_MassStorage_SDCard_Workspace *)aWorkspace;

    IGrab();
    error = ISDCard_MS_GetSizeInfo(aWorkspace, aInfo);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SetPower
 *---------------------------------------------------------------------------*
 * Description:
 *      Unsupported Function.  Just a placeholder in a structure of functions.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      TBool aOn                -- Power setting to set.
 * Outputs:
 *      T_uezError               -- always returns
 *                                  UEZ_ERROR_NOT_SUPPORTED.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_SetPower(void *aWorkspace, TBool aOn)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SetLock
 *---------------------------------------------------------------------------*
 * Description:
 *      Unsupported Function.  Just a placeholder in a structure of functions.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      TBool aLock              -- Lock setting to set.
 * Outputs:
 *      T_uezError               -- always returns
 *                                  UEZ_ERROR_NOT_SUPPORTED.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_SetLock(void *aWorkspace, TBool aLock)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SetSoftwareWriteProtect
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the write protect bit.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      TBool aSWWriteProtect    -- Write Protect setting to set.
 * Outputs:
 *      T_uezError               -- always returns
 *                                  UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_SetSoftwareWriteProtect(
        void *aWorkspace,
        TBool aSWWriteProtect)
{
    T_MassStorage_SDCard_Workspace *p =
        (T_MassStorage_SDCard_Workspace *)aWorkspace;
    IGrab();
    p->iSWWriteProtect = aSWWriteProtect;
    IRelease();
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_Eject
 *---------------------------------------------------------------------------*
 * Description:
 *      Unsupported Function.  Just a placeholder in a structure of functions.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 * Outputs:
 *      T_uezError               -- always returns
 *                                  UEZ_ERROR_ILLEGAL_OPERATION.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_Eject(void *aWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MiscControl
 *---------------------------------------------------------------------------*
 * Description:
 *      Unsupported Function.  Just a placeholder in a structure of functions.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      TUInt32 aControlCode     -- Placeholder variable.
 *      void *aBuffer            -- Placeholder variable.
 * Outputs:
 *      T_uezError               -- always returns
 *                                  UEZ_ERROR_ILLEGAL_OPERATION.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_MiscControl(
                                        void *aWorkspace,
                                        TUInt32 aControlCode,
                                        void *aBuffer
                                       )
{
    return UEZ_ERROR_ILLEGAL_OPERATION;
}

void MassStorage_SDCard_Create(
        const char *aName,
        const char *aSPIName,
        T_uezGPIOPortPin aChipSelect)
{
    DEVICE_MassStorage **p_ms;
    T_uezDeviceWorkspace *p_spi;
    T_uezDevice spi;
    HAL_GPIOPort **p_gpio;

    // Register the MMC card device
    UEZDeviceTableRegister(
            aName,
            (T_uezDeviceInterface *)&MassStorage_SDCard_Interface,
            0,
            (T_uezDeviceWorkspace **)&p_ms);

    UEZDeviceTableFind(aSPIName, &spi);
    UEZDeviceTableGetWorkspace(spi, (T_uezDeviceWorkspace **)&p_spi);
    p_gpio = UEZGPIOGetPort(UEZ_GPIO_PORT_FROM_PORT_PIN(aChipSelect));
    UEZGPIOLock(aChipSelect);
    UEZGPIOSet(aChipSelect);
    UEZGPIOOutput(aChipSelect);
    MassStorage_SDCard_ConfigureSPI(
            p_ms,
            (DEVICE_SPI_BUS **)p_spi,
            (HAL_GPIOPort **)p_gpio,
            1<<UEZ_GPIO_PIN_FROM_PORT_PIN(aChipSelect));
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_MassStorage MassStorage_SDCard_Interface = {
    {
        // Common device interface
        "MS:USB",
        0x0100,
        MassStorage_SDCard_InitializeWorkspace,
        sizeof(T_MassStorage_SDCard_Workspace),
    },
        
    // Functions
    SDCard_MS_Init,
    SDCard_MS_Status,
    SDCard_MS_Read,
    SDCard_MS_Write,
    SDCard_MS_Sync,
    SDCard_MS_GetSizeInfo,
    SDCard_MS_SetPower,
    SDCard_MS_SetLock,
    SDCard_MS_SetSoftwareWriteProtect,
    SDCard_MS_Eject,
    SDCard_MS_MiscControl,
} ;

/*-------------------------------------------------------------------------*
 * End of File:  SDCard_MS_driver_SPI.c
 *-------------------------------------------------------------------------*/

