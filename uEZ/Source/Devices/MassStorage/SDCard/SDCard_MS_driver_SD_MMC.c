/*-------------------------------------------------------------------------*
 * File:  SDCard_MS_SD_MMC_driver_SPI.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the SDCard MassStorage Device.
 * This implementation supports MMC, SDC version 1, and SDC version 2(SDHC).
 * This implementation was created from the original SDCard_MS_SD_MMC_driver_SPI.c
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
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include "SDCard_MS_driver_SD_MMC.h"
#include <HAL/Interrupt.h>
#include <HAL/SD_MMC.h>

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef SDCARD_SD_MMC_RATE_FOR_ID_STATE
#define SDCARD_SD_MMC_RATE_FOR_ID_STATE            200000UL   // 400kHz (100-400kHz)
#endif
#ifndef SDCARD_SD_MMC_ACQUIRE_DELAY
#define SDCARD_SD_MMC_ACQUIRE_DELAY      (10)          /*!< inter-command acquire oper condition delay in msec*/
#endif

// Internal settings
#define INIT_OP_RETRIES       50            /*!< initial OP_COND retries */
#define SET_OP_RETRIES        1000          /*!< set OP_COND retries */
#define SDIO_BUS_WIDTH        4             /*!< Max bus width supported */

#define SDCARD_FAILED_READ_RETRY_COUNT              5

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define IGrab()         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()      UEZSemaphoreRelease(p->iSem)

#if SDCARD_DEBUG_OUTPUT
#define dprintf printf
#else
#define dprintf(...)
#endif

#define SDC_SECTOR_SIZE         512 // bytes

/* Card type definitions (iCardType) */
#define CT_MMC              0x01
#define CT_SD1              0x02
#define CT_SD2              0x04
#define CT_SDC              (CT_SD1|CT_SD2)
#define CT_BLOCK            0x08

/**
 * @brief  Possible SDMMC card state types
 */
typedef enum {
    SDMMC_IDLE_ST = 0,  /*!< Idle state */
    SDMMC_READY_ST,     /*!< Ready state */
    SDMMC_IDENT_ST,     /*!< Identification State */
    SDMMC_STBY_ST,      /*!< standby state */
    SDMMC_TRAN_ST,      /*!< transfer state */
    SDMMC_DATA_ST,      /*!< Sending-data State */
    SDMMC_RCV_ST,       /*!< Receive-data State */
    SDMMC_PRG_ST,       /*!< Programming State */
    SDMMC_DIS_ST        /*!< Disconnect State */
} SDMMC_STATE_T;

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    //! Mass Storage device back link
    const DEVICE_MassStorage *iDevice;

    //! Extended CSD
    uint32_t iExtCSD[512 / 4];

//    TUInt8 G_sector[512];
//    TUInt8 iCmd[50];
    TBool iSWWriteProtect;
    TUInt32 iRCA;       // Relative Card Address
//    TUInt32 iOCR;
    TUInt32 iCardType;
//    TUInt8 iCardInfo[16 + 16 + 4]; /* CSD(16), CID(16), OCR(4) */

    //! CID of acquired card
    TUInt8 iCID[16];

    //! CSD of acquired card
    TUInt32 iCSD[4];

    //! Size of a single block in bytes
    TUInt32 iBlockLen;

    //! Number of blocks
    TUInt32 iBlockNum;

    //! Flag if the initial Init call has been performed yet successfully
    TBool iInitPerformed;
    T_uezSemaphore iSem;

    //! General status flags
    TUInt8 iStat;
        #define STA_NOINIT      0x01    /* Drive not initialized */
        #define STA_NODISK      0x02    /* No medium in the drive */
        #define STA_PROTECT     0x04    /* Write protected */

    T_uezSemaphore iIsCompleteRead;
    T_uezSemaphore iIsCompleteWrite;
    HAL_SD_MMC **iSD_MMC;
} T_MassStorage_SDCard_SD_MMC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
/* Get card's current state (idle, transfer, program, etc.) */
static TInt32 ISDMMC_GetState(T_MassStorage_SDCard_SD_MMC_Workspace *p)
{
	uint32_t status;
	TUInt32 response[4];

	/* get current state of the card */
    status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SEND_STATUS, (p->iRCA << 16), 0, response);
	if (status & MCI_INT_RTO)
		return -1;

	/* check card state in response */
	return (TInt32) R1_CURRENT_STATE(response[0]);
}


/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_SDCard_SD_MMC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a MassStorage device's workspace on the SDCard.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 * Outputs:
 *      @return Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_SDCard_SD_MMC_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;

//    p->iCardType = 1;
    p->iInitPerformed = EFalse;
    p->iStat = STA_NOINIT;
    p->iSWWriteProtect = 0;

    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;

    error = UEZSemaphoreCreateCounting(&p->iIsCompleteRead, 255, 0);
    if (error)
        return error;

    error = UEZSemaphoreCreateCounting(&p->iIsCompleteWrite, 255, 0);
    if (error)
        return error;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IIsCardAquired
 *---------------------------------------------------------------------------*
 * Description:
 *      Look at the card status.  Did we acquire a card?
 * Inputs:
 *      @param [in] p -- Workspace
 * Outputs:
 *      @return non-zero if acquired, else 0
 *---------------------------------------------------------------------------*/
static int IIsCardAquired(T_MassStorage_SDCard_SD_MMC_Workspace *p)
{
    return p->iCID[0] != 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  IGetBits
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the bits with ina  multi-word buffer.  Used to get fields
 *      within the CSD and EXT-CSD.
 * Inputs:
 *      @param [in] start -- Start bit location
 *      @param [in] end -- End bit location (inclusive)
 *      @param [in] data -- Pointer to an array of 32-bit data fields
 * Outputs:
 *      @return Resulting value (up to 32 bits)
 *---------------------------------------------------------------------------*/
static TUInt32 IGetBits(TUInt32 start, TUInt32 end, TUInt32 *data)
{
    TUInt32 v;
    TUInt32 i = end >> 5;
    TUInt32 j = start & 0x1f;

    if (i == (start >> 5)) {
        v = (data[i] >> j);
    } else {
        v = ((data[i] << (32 - j)) | (data[start >> 5] >> j));
    }

    return v & ((1 << (end - start + 1)) - 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  IIsCardAquired
 *---------------------------------------------------------------------------*
 * Description:
 *      Process the CSD and if there is a EXT-CSD get it and process it
 *      too.  This determines the size and operating characteristics of the
 *      currently attacked MMC/SDCard.
 * Inputs:
 *      @param [in] p -- Workspace
 *---------------------------------------------------------------------------*/
static void IProcessCSD(T_MassStorage_SDCard_SD_MMC_Workspace *p)
{
    TUInt32 status = 0;
    TUInt32 c_size = 0;
    TUInt32 c_size_mult = 0;
    TUInt32 mult = 0;
    TUInt32 response[4];

    /* compute block length based on CSD response */
    p->iBlockLen = (1 << IGetBits(80, 83, p->iCSD));

    if ((p->iCardType & CARD_TYPE_HC) && (p->iCardType & CARD_TYPE_SD)) {
        /* See section 5.3.3 CSD Register (CSD Version 2.0) of SD2.0 spec  an explanation for the calculation of these values */
        c_size = IGetBits(48, 69, p->iCSD) + 1;
        p->iBlockNum = (c_size << 10);  /* 512 byte blocks assumed */
    }
    else {
        /* See section 5.3 of the 4.1 revision of the MMC specs for  an explanation for the calculation of these values */
        c_size = IGetBits(62, 73, p->iCSD);
        c_size_mult = IGetBits(47, 49, p->iCSD);
        mult = 1 << (c_size_mult + 2);
        p->iBlockNum = (c_size + 1) * mult;

        /* adjust block number to 512/block */
        if (p->iBlockLen > MMC_SECTOR_SIZE) {
            p->iBlockNum = p->iBlockNum * (p->iBlockLen >> 9);
        }

        /* get extended CSD for newer MMC cards CSD spec >= 4.0*/
        if (((p->iCardType & CARD_TYPE_SD) == 0) &&
            (IGetBits(122, 125, p->iCSD) >= 4)) {
            /* put card in trans state */
            status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SELECT_CARD, (p->iRCA << 16), 0, response);

            /* set block size and byte count */
            (*p->iSD_MMC)->SetBlockSize(p->iSD_MMC, MMC_SECTOR_SIZE);

            /* send EXT_CSD command */
            (*p->iSD_MMC)->PrepreExtCSDTransfer(p->iSD_MMC, (TUInt8 *)&p->iExtCSD, MMC_SECTOR_SIZE);
            status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SEND_EXT_CSD, 0, 0 | MCI_INT_DATA_OVER, response);
            if ((status & SD_INT_ERROR) == 0) {
                /* check EXT_CSD_VER is greater than 1.1 */
                if ((p->iExtCSD[48] & 0xFF) > 1) {
                    /* bytes 212:215 represent sec count */
                    p->iBlockNum = p->iExtCSD[53];

                }
                /* switch to 52MHz clock if card type is set to 1 or else set to 26MHz */
                if ((p->iExtCSD[49] & 0xFF) == 1) {
                    /* for type 1 MMC cards high speed is 52MHz */
                    (*p->iSD_MMC)->SetClockRate(p->iSD_MMC, MMC_HIGH_BUS_MAX_CLOCK);
                }
                else {
                    /* for type 0 MMC cards high speed is 26MHz */
                    (*p->iSD_MMC)->SetClockRate(p->iSD_MMC, MMC_LOW_BUS_MAX_CLOCK);
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ISetTransState
 *---------------------------------------------------------------------------*
 * Description:
 *      Put the current MMC/SDCard into the trans state.
 * Inputs:
 *      @param [in] p -- Workspace
 *  Output:
 *      @return Error code.  Zero if ok, else non-zero.
 *---------------------------------------------------------------------------*/
static int ISetTransState(T_MassStorage_SDCard_SD_MMC_Workspace *p)
{
    TUInt32 status;
    TUInt32 response[4];

    // Get current state of the card
    status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SEND_STATUS, (p->iRCA << 16), 0, response);
    if (status & MCI_INT_RTO) {
        // unable to get the card state. So return immediately.
        return -1;
    }

    // Check the card state in response
    status = R1_CURRENT_STATE(response[0]);
    switch (status) {
        case SDMMC_STBY_ST:
            // put into trans state
            status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SELECT_CARD, (p->iRCA << 16), 0, response);
            if (status != 0) {
                // unable to put the card into trans state.  Report error.
                return -1;
            }
            break;

        case SDMMC_TRAN_ST:
            break;

        default:
            // Should not be any other state
            return -1;
    }

    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISetCardParams
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the card's data width and block size for communication
 * Inputs:
 *      @param [in] p -- Workspace
 * Output:
 *      @return Error code.  Zero if ok, else non-zero.
 *---------------------------------------------------------------------------*/
/* Sets card data width and block size */
static int ISetCardParams(T_MassStorage_SDCard_SD_MMC_Workspace *p)
{
    TUInt32 status;
    TUInt32 response[4];

#if SDIO_BUS_WIDTH > 1
    if (p->iCardType & CARD_TYPE_SD) {
        status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SD_SET_WIDTH, 2, 0, response);
        if (status != 0)
            return -1;

        /* if positive response */
        (*p->iSD_MMC)->SetCardType(p->iSD_MMC, MCI_CTYPE_4BIT);
    }
#elif SDIO_BUS_WIDTH > 4
#error 8-bit mode not supported yet!
#endif

    /* set block length */
    (*p->iSD_MMC)->SetBlockSize(p->iSD_MMC, MMC_SECTOR_SIZE);
    status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SET_BLOCKLEN, MMC_SECTOR_SIZE, 0, response);
    if (status != 0)
        return -1;

    return 0;
}

static T_uezError SDCard_MS_SD_MMC_Init(void *aWorkspace, TUInt32 aAddress)
{
    int32_t status;
    int32_t tries = 0;
    uint32_t ocr = OCR_VOLTAGE_RANGE_MSK;
    uint32_t r;
    int32_t state = 0;
    uint32_t command = 0;
    uint32_t response[4];

    // Deference the workspace
    T_MassStorage_SDCard_SD_MMC_Workspace *p = (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;

    // If already initialized, we are done!
    if (p->iInitPerformed)
        return UEZ_ERROR_NONE;

    (*p->iSD_MMC)->PowerOn(p->iSD_MMC); /* Force socket power on */
    (*p->iSD_MMC)->SetClockRate(p->iSD_MMC, SDCARD_SD_MMC_RATE_FOR_ID_STATE);
    UEZTaskDelay(2);

    /* clear card type */
    (*p->iSD_MMC)->SetCardType(p->iSD_MMC, 0);

    /* No card in the socket? */
    if (p->iStat & STA_NODISK)
        return UEZ_ERROR_DEVICE_NOT_FOUND;

    /* set high speed for the card as 20MHz */
    (*p->iSD_MMC)->SetClockRate(p->iSD_MMC, MMC_MAX_CLOCK);
    status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_IDLE, 0, MCI_INT_CMD_DONE, response);

    while (state < 100) {
        switch (state) {
            case 0: /* Setup for SD */
                /* check if it is SDHC card */
                status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SD_SEND_IF_COND, SD_SEND_IF_ARG, 0, response);
                if (!(status & MCI_INT_RTO)) {
                    /* check response has same echo pattern */
                    if ((response[0] & SD_SEND_IF_ECHO_MSK) == SD_SEND_IF_RESP) {
                        ocr |= OCR_HC_CCS;
                    }
                }

                ++state;
                command = CMD_SD_OP_COND;
                tries = INIT_OP_RETRIES;

                /* assume SD card */
                p->iCardType |= CARD_TYPE_SD;
                (*p->iSD_MMC)->SetClockRate(p->iSD_MMC, SD_MAX_CLOCK);
                break;

            case 10: /* Setup for MMC */
                // start fresh for MMC cards
                p->iCardType &= ~CARD_TYPE_SD;
                status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_IDLE, 0, MCI_INT_CMD_DONE, response);
                command = CMD_MMC_OP_COND;
                tries = INIT_OP_RETRIES;
                ocr |= OCR_HC_CCS;
                ++state;

                /* for MMC cards high speed is 20MHz */
                (*p->iSD_MMC)->SetClockRate(p->iSD_MMC, MMC_MAX_CLOCK);
                break;

            case 1:
            case 11:
                status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, command, 0, 0, response);
                if (status & MCI_INT_RTO) {
                    state += 9; /* Mode unavailable */
                } else {
                    ++state;
                }
                break;

            case 2: /* Initial OCR check  */
            case 12:
                ocr = response[0] | (ocr & OCR_HC_CCS);
                if (ocr & OCR_ALL_READY) {
                    ++state;
                } else {
                    state += 2;
                }
                break;

            case 3: // Wait for the OCR to clear
            case 13:
                while ((ocr & OCR_ALL_READY) && --tries > 0) {
                    UEZTaskDelay(SDCARD_SD_MMC_ACQUIRE_DELAY);
                    status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, command, 0, 0, response);
                    ocr = response[0] | (ocr & OCR_HC_CCS);
                }
                if (ocr & OCR_ALL_READY) {
                    state += 7;
                } else {
                    ++state;
                }
                break;

            case 14:
                // Set High Capacity bit
                ocr |= OCR_HC_CCS;
                // fall thru to next switch statement

            case 4: /* Assign OCR */
                tries = SET_OP_RETRIES;
                ocr &= OCR_VOLTAGE_RANGE_MSK | OCR_HC_CCS; /* Mask for the bits we care about */
                do {
                    UEZTaskDelay(SDCARD_SD_MMC_ACQUIRE_DELAY);
                    status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, command, ocr, 0, response);
                    r = response[0];
                } while (!(r & OCR_ALL_READY) && --tries > 0);

                if (r & OCR_ALL_READY) {
                    /* is it high capacity card */
                    p->iCardType |= (r & OCR_HC_CCS);
                    ++state;
                } else {
                    state += 6;
                }
                break;

            case 5: /* CID polling */
            case 15:
                status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_ALL_SEND_CID, 0, 0, response);
                memcpy(p->iCID, &response[0], 16);
                ++state;
                break;

            case 6: /* RCA send, for SD get RCA */
                status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SD_SEND_RCA, 0, 0, response);
                p->iRCA = (response[0] >> 16);
                (*p->iSD_MMC)->SetRelativeCardAddress(p->iSD_MMC, p->iRCA);
                ++state;
                break;

            case 16: /* RCA assignment for MMC set to 1 */
                p->iRCA = 1;
                (*p->iSD_MMC)->SetRelativeCardAddress(p->iSD_MMC, p->iRCA);
                status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_MMC_SET_RCA, p->iRCA << 16, 0, response);
                ++state;
                break;

            case 7:
            case 17:
                status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SEND_CSD, p->iRCA << 16, 0, response);
                memcpy((void *)p->iCSD, response, 16);
                state = 100;
                break;

            default:
                state += 100; /* break from while loop */
                break;
        }
    }

    /* Compute card size, block size and no. of blocks  based on CSD response recived. */
    if (IIsCardAquired(p)) {
        // Get the size of the card and information
        IProcessCSD(p);

        /* Setup card data width and block size (once) */
        if (ISetTransState(p) != 0)
            return UEZ_ERROR_INTERNAL_ERROR;
        if (ISetCardParams(p) != 0)
            return UEZ_ERROR_INTERNAL_ERROR;
    }

    if (IIsCardAquired(p)) {
        /* Initialization succeeded */
        p->iInitPerformed = ETrue;
        p->iStat &= ~STA_NOINIT; /* Clear STA_NOINIT */
        return UEZ_ERROR_NONE;
    }
    return UEZ_ERROR_DEVICE_NOT_FOUND;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_Status
 *---------------------------------------------------------------------------*
 * Description:
 *      Send initialized status.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      T_msStatus *aStatus      -- Status value to send.
 * Outputs:
 *      T_uezError               -- always returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_SD_MMC_Status(void *aWorkspace, T_msStatus *aStatus)
{
    T_msStatus status;
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;

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
 * Routine:  IWaitReady
 *---------------------------------------------------------------------------*
 * Description:
 *      Waits until card is in trans state
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      TUInt32 aTimeout         -- Timeout in ms
 * Outputs:
 *      T_uezError               -- always returns UEZ_ERROR_NONE or
 *                                      UEZ_ERROR_TIMEOUT
 *---------------------------------------------------------------------------*/
static T_uezError IWaitReady(T_MassStorage_SDCard_SD_MMC_Workspace *p, TUInt32 aTimeout)
{
    TUInt32 responseCode[4];
    TUInt32 start;

    start = UEZTickCounterGet();
    responseCode[0] = 0;
    while (1) {
        if (UEZTickCounterGetDelta(start) >= aTimeout)
            return UEZ_ERROR_TIMEOUT;
        if ((*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SEND_STATUS, (TUInt32)p->iRCA << 16UL, 1, responseCode)
            && ((responseCode[0] & 0x01E00) == 0x00800)) {
            break;
        }
        UEZTaskDelay(5);
    }

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISD_MMCCompleteRead
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback routine to release the complete flag.
 *---------------------------------------------------------------------------*/
static void ISD_MMCCompleteRead(void *aWorkspace)
{
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    _isr_UEZSemaphoreRelease(p->iIsCompleteRead);
}

/*---------------------------------------------------------------------------*
 * Routine:  ISD_MMCCompleteWrite
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback routine to release the complete flag.
 *---------------------------------------------------------------------------*/
static void ISD_MMCCompleteWrite(void *aWorkspace)
{
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    _isr_UEZSemaphoreRelease(p->iIsCompleteWrite);
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_Read
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
static T_uezError SDCard_MS_SD_MMC_Read(
                        void *aWorkspace,
                        const TUInt32 aStart,
                        const TUInt32 aNumBlocks,
                        void *aBuffer)
{
    T_MassStorage_SDCard_SD_MMC_Workspace *p = (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    TUInt32 index;
    TUInt32 response[4];
    TUInt32 numTransferring;
    T_uezError error = UEZ_ERROR_NONE;

    IGrab();

    while (1) {
        /* Check parameter, count must be 1-127 */
        /* TODO: We should make this routine do sets of 127 blocks when larger */
        if ((aNumBlocks < 1) || (aNumBlocks > 127)) {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }

        // Has this been init?
        if (!p->iInitPerformed) {
            error = UEZ_ERROR_NOT_READY;
            break;
        }
        /* Check drive status */
        if (p->iStat & STA_NOINIT) {
            error = UEZ_ERROR_NOT_READY;
            break;
        }

        // Is this in range?
        if ((aNumBlocks < 1) || ((aNumBlocks + aNumBlocks) > p->iBlockNum)) {
            error = UEZ_ERROR_OUT_OF_RANGE;
            break;
        }

        // put card in trans state
        if (ISetTransState(p) != 0) {
            error = UEZ_ERROR_INTERNAL_ERROR;
            break;
        }

        // if high capacity card use block indexing
        if (p->iCardType & CARD_TYPE_HC) {
            index = aStart;
        } else {
            // Currently fixed at 512 bytes per block
            index = aStart << 9;
        }

        // set number of bytes to read
        error = (*p->iSD_MMC)->ReadyReception(p->iSD_MMC, aBuffer, aNumBlocks, 512,
            ISD_MMCCompleteRead, p, &numTransferring);
        if (numTransferring < aNumBlocks) {
          // TODO: Maybe come back and do multiple read/writes
            UEZFailureMsg("Transfer too big!");
        }

        /* Select single or multiple read based on number of blocks */
        if (aNumBlocks == 1) {
            (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_READ_SINGLE, index, 0 | MCI_INT_DATA_OVER, response);
        } else {
            (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_READ_MULTIPLE, index, 0 | MCI_INT_DATA_OVER, response);
        }

        /* Wait for card program to finish or timeout (5 seconds is more than enough!) */
        error = UEZSemaphoreGrab(p->iIsCompleteRead, 500 * numTransferring);
        while (ISDMMC_GetState(p) != SDMMC_TRAN_ST)
            UEZTaskDelay(0);

        break;
    }

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_Write
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
static T_uezError SDCard_MS_SD_MMC_Write(
    void *aWorkspace,
    TUInt32 aStart,
    TUInt32 aNumBlocks,
    const void *aBuffer)
{
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 index;
    TUInt32 numWriting;
    TUInt32 response[4];

    IGrab();
    while (1) {
        /* Check parameter */
        if (aNumBlocks < 1 || aNumBlocks > 127) {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }
        /* Check drive status */
        if (p->iStat & STA_NOINIT) {
            error = UEZ_ERROR_NOT_READY;
            break;
        }
        /* Check write protection */
        if ((p->iStat & STA_PROTECT) || (p->iSWWriteProtect)) {
            error = UEZ_ERROR_PROTECTED;
            break;
        }

        /* if card is not acquired return immediately */
        if ((aStart < 1) || ((aStart + aNumBlocks) > p->iBlockNum)) {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }

        /*Wait for card program to finish*/
        while (ISDMMC_GetState(p) != SDMMC_TRAN_ST)
            UEZTaskDelay(0);

        /* put card in trans state */
        if (ISetTransState(p) != 0) {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }

        /* if high capacity card use block indexing */
        if (p->iCardType & CARD_TYPE_HC) {
            index = aStart;
        } else {
            // fixed at 512 bytes
            index = aStart << 9;   // * p->iBlockLen;
        }

        /* set number of bytes to write */
        (*p->iSD_MMC)->ReadyTransmission(p->iSD_MMC, aBuffer, aNumBlocks, 512, &numWriting, ISD_MMCCompleteWrite, p);

        // Single or multiple block write?
        if (aNumBlocks == 1) {
            (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_WRITE_SINGLE, index, 0 | MCI_INT_DATA_OVER, response);
        } else {
            (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_WRITE_MULTIPLE, index, 0 | MCI_INT_DATA_OVER, response);
        }

        // Wait for all blocks to be transferred
        UEZSemaphoreGrab(p->iIsCompleteWrite, 500 * numWriting);
        
        /*Wait for card program to finish*/
        while (ISDMMC_GetState(p) != SDMMC_TRAN_ST)
            UEZTaskDelay(0);

        break;
    }
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_Sync
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
static T_uezError SDCard_MS_SD_MMC_Sync(void *aWorkspace)
{
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();

    // Just wait until we are ready or timeout
    error = IWaitReady(p, 500);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_GetSizeInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Fill in the workspace size variables
 * Inputs:
 *      void *aWorkspace      -- Workspace for this instance.
 *      T_msSizeInfo *aInfo   -- Pointer to a structure to receive info.
 * Outputs:
 *      T_uezError            -- always returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_SD_MMC_GetSizeInfo(
    void *aWorkspace,
    T_msSizeInfo *aInfo)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    TUInt8 *p_csd = (TUInt8 *)p->iCSD;

    IGrab();

    // Get the size of the sectors (just hard code to 512)
    aInfo->iSectorSize = 512;
    aInfo->iNumSectors = p->iBlockNum;

    // Determine the block size
    // SDC or MMC?
    if (p->iCardType & 2) {
        // SDC
        aInfo->iBlockSize = (((p_csd[10] & 63) << 1)
            + ((TUInt16)(p_csd[11] & 128) >> 7) + 1) << ((p_csd[13] >> 16) - 1);
    } else {
        // MMC
        aInfo->iBlockSize = ((TUInt16)((p_csd[10] & 124) >> 2) + 1)
            * (((p_csd[11] & 3) << 3) + ((p_csd[11] & 224) >> 5) + 1);
    }

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_SetPower
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
static T_uezError SDCard_MS_SD_MMC_SetPower(void *aWorkspace, TBool aOn)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_SetLock
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
static T_uezError SDCard_MS_SD_MMC_SetLock(void *aWorkspace, TBool aLock)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_SetSoftwareWriteProtect
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
static T_uezError SDCard_MS_SD_MMC_SetSoftwareWriteProtect(
    void *aWorkspace,
    TBool aSWWriteProtect)
{
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    IGrab();
    p->iSWWriteProtect = aSWWriteProtect;
    IRelease();
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_MiscControl
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
static T_uezError SDCard_MS_SD_MMC_MiscControl(
    void *aWorkspace,
    TUInt32 aControlCode,
    void *aBuffer)
{
    return UEZ_ERROR_ILLEGAL_OPERATION;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_SD_MMC_Eject
 *---------------------------------------------------------------------------*
 * Description:
 *      Unsupported Function.  Just a placeholder in a structure of functions.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 * Outputs:
 *      T_uezError               -- always returns
 *                                  UEZ_ERROR_ILLEGAL_OPERATION.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_SD_MMC_Eject(void *aWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

void MassStorage_SDCard_SD_MMC_Create(const char *aName, const char *aHALSD_MMC)
{
    DEVICE_MassStorage **p_ms;
    T_MassStorage_SDCard_SD_MMC_Workspace *p;

    // Register the MMC card device
    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&MassStorage_SDCard_SD_MMC_Interface, 0,
        (T_uezDeviceWorkspace **)&p_ms);
    p = (T_MassStorage_SDCard_SD_MMC_Workspace *)p_ms;

    HALInterfaceFind(aHALSD_MMC, (T_halWorkspace **)&p->iSD_MMC);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_MassStorage MassStorage_SDCard_SD_MMC_Interface = { {
// Common device interface
    "MS:SD_MMC",
    0x0100,
    MassStorage_SDCard_SD_MMC_InitializeWorkspace,
    sizeof(T_MassStorage_SDCard_SD_MMC_Workspace), },

// Functions
    SDCard_MS_SD_MMC_Init,
    SDCard_MS_SD_MMC_Status,
    SDCard_MS_SD_MMC_Read,
    SDCard_MS_SD_MMC_Write,
    SDCard_MS_SD_MMC_Sync,
    SDCard_MS_SD_MMC_GetSizeInfo,
    SDCard_MS_SD_MMC_SetPower,
    SDCard_MS_SD_MMC_SetLock,
    SDCard_MS_SD_MMC_SetSoftwareWriteProtect,
    SDCard_MS_SD_MMC_Eject,
    SDCard_MS_SD_MMC_MiscControl,
};

/*-------------------------------------------------------------------------*
 * End of File:  SDCard_MS_SD_MMC_driver_SPI.c
 *-------------------------------------------------------------------------*/

