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
#include <uEZPlatformAPI.h>
#include "SDCard_MS_driver_SD_MMC.h"
#include <HAL/Interrupt.h>
#include <HAL/SD_MMC.h>
#include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h>
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>

// Currently this is only used on LPC4357 and maybe for LPC54XX and LPC55XX MCUs.

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef SDCARD_SD_MMC_RATE_FOR_ID_STATE
#define SDCARD_SD_MMC_RATE_FOR_ID_STATE            400000UL // 400kHz (100-400kHz) // 4357 can't do less than 400KHz without clock tree change
#endif
#ifndef SDCARD_SD_MMC_ACQUIRE_DELAY
#define SDCARD_SD_MMC_ACQUIRE_DELAY      (10)          /*!< inter-command acquire oper condition delay in msec*/
#endif

// Internal settings
#define INIT_OP_RETRIES       50            /*!< initial OP_COND retries */
#define SET_OP_RETRIES        1000          /*!< set OP_COND retries */
#define SDIO_BUS_WIDTH        4             /*!< Max bus width supported */

#define SDCARD_FAILED_READ_RETRY_COUNT              3
#define SDCARD_FAILED_WRITE_RETRY_COUNT             3

#define MULTIBLOCK_READ_SUPPORTED                   1 // set to 0 to turn off multiblock reads
#define MULTIBLOCK_WRITE_SUPPORTED                  0 // set to 0 to turn off multiblock writes

#ifndef SDCARD_DEBUG_OUTPUT
    #define SDCARD_DEBUG_OUTPUT 0
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define IGrab()         UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE)
#define IRelease()      UEZSemaphoreRelease(p->iSem)

#if SDCARD_DEBUG_OUTPUT
//#define dprintf printf
#define dprintf(...) DEBUG_RTT_WriteString(0, __VA_ARGS__)
//#define dprintf(...) DEBUG_SV_PrintfE(__VA_ARGS__)
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

    //T_uezSemaphore iIsCompleteRead;
    //T_uezSemaphore iIsCompleteWrite;
    TBool iIsCompleteReadFlag;
    TBool iIsCompleteWriteFlag;
    
    HAL_SD_MMC **iSD_MMC;
    TBool i4bitModeEnabled;
    TUInt32 iError;
} T_MassStorage_SDCard_SD_MMC_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
 static TBool G_MMC_HighSpeedModeAllowed = ETrue;

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
 * Routine:  ISD_MMCerror
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback routine for errors
 *---------------------------------------------------------------------------*/
static void ISD_MMCError(void *aWorkspace)
{
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    
    if (LPC_SDMMC->RINTSTS & MCI_INT_SBE) { // Start bit error - Start bit was not received during data reception.
       p->iError = 2;
    }      
    if (LPC_SDMMC->RINTSTS & MCI_INT_EBE) { // End-bit error - End bit was not received during data reception or for a write operation; a CRC error is indicated by the card.
       p->iError = 2;
    }      
    if (LPC_SDMMC->RINTSTS & MCI_INT_RTO) { // Response timeout error (RTO_BAR)
       p->iError = 2;
    }    
    if (LPC_SDMMC->RINTSTS & MCI_INT_DTO) { // Data read timeout (DRTO_BDS) - Card has not sent data within the time-out period.
       p->iError = 2;
    }

    if (p->iError == 2) { // force re-int.
      p->iInitPerformed = EFalse;
    } else {
      p->iError = 1;
    }
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
    p->iCardType = 0;
    p->iError = 0;
    if(UEZPlatform_MCI_TransferMode() == UEZ_MCI_BUS_4BIT_WIDE) {
      p->i4bitModeEnabled = ETrue;
    } else {
      p->i4bitModeEnabled = EFalse;
    }
    
    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;
/*
    error = UEZSemaphoreCreateCounting(&p->iIsCompleteRead, 255, 0;
    if (error)
        return error;

    error = UEZSemaphoreCreateCounting(&p->iIsCompleteWrite, 255, 0);
    if (error)
        return error;*/

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
static int32_t IIsCardAquired(T_MassStorage_SDCard_SD_MMC_Workspace *p)
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
static int32_t ISetTransState(T_MassStorage_SDCard_SD_MMC_Workspace *p)
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
static int32_t ISetCardParams(T_MassStorage_SDCard_SD_MMC_Workspace *p, 
      TBool ahighSpeedModeAllowed)
{
    TUInt32 status;
    TUInt32 response[4];

#if SDIO_BUS_WIDTH > 1
    if(p->i4bitModeEnabled == ETrue) {
      if (p->iCardType & CARD_TYPE_SD) {
          status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SD_SET_WIDTH, 2, 0, response);
          if (status != 0) {
              return -1;
          }

          /* if positive response */
          (*p->iSD_MMC)->SetCardType(p->iSD_MMC, MCI_CTYPE_4BIT);
      }    
    }
#elif SDIO_BUS_WIDTH > 4
#error 8-bit mode not supported yet! // MMC only
#endif

    if ((p->iCardType & CARD_TYPE_SD) == 0) { // not SD card, CMD6 not supported?
    } else { // SD card
        // Only SD V1.10 and above supports CMD6 (2004). Maybe SD V1.10 could be supported, but we won't test and would need to hunt down old full size sd card to test it.
        // For eMMC modules need to check if type 1 MMC
        // Set mode switch to 50MHz (high-speed mode) here, can change clock after 8 clock cycles after end of status data. (Switch Function Command)
        status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SD_SEND_SWITCH, 1, 0, response); // CMD6
        if (status != 0) {
            return -1;
        }
    }
    // Now run some other command first before switching speed to high speed mode.

    /* set block length */
    (*p->iSD_MMC)->SetBlockSize(p->iSD_MMC, MMC_SECTOR_SIZE);
    status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SET_BLOCKLEN, MMC_SECTOR_SIZE, 0, response);
    if (status != 0) {
        return -1;
    }
    // After the above command has finished, the SD card will have had time to adjust modes to allow for high-speed mode frequencies to be used.

    if ((p->iCardType & CARD_TYPE_SD) == 0) { // not SD card
        // switch to 51MHz clock if card type is set to 1 or else set to 25.5MHz (actual default uEZGUI speeds on 4357 204MHz source clock)
        if ((p->iExtCSD[49] & 0xFF) == 1) { // for type 1 MMC cards high speed is 52MHz 
        } else { // for type 0 MMC cards high speed is 26MHz
            return 0;
        }
    }

    if(ahighSpeedModeAllowed == ETrue) {
      // Per SD spec SD V2.00 and above (all microSD cards, all SDHC+ cards) will suppor the 50Mhz high speed mode. In this driver normally it is slightly less than 51MHz. (20ns clock time)
      status = (*p->iSD_MMC)->SetClockRate(p->iSD_MMC, MMC_MAX_READ_CLOCK); // Now full speed is set//UEZPlatform_MCI_DefaultFreq());
      if (status != 0) {
          return -1;
      }
    }

    return 0;
}

static T_uezError SDCard_MS_SD_MMC_Init(void *aWorkspace, TUInt32 aAddress)
{
    int32_t status = 0;
    int32_t tries = 0;
    uint32_t ocr = OCR_VOLTAGE_RANGE_MSK;
    uint32_t resp2 = 0;
    int32_t state = 0;
    uint32_t command = 0;
    uint32_t response[4];

    // Deference the workspace
    T_MassStorage_SDCard_SD_MMC_Workspace *p = (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;

    if (p->iError == 2) { // We got a start bit or timeout error
      // Check card detect, if card is not present we can preserve full speed on init.
      if((*p->iSD_MMC)->IsCardInserted(p->iSD_MMC) == EFalse) { // failed start bits or timeout and card was removed
          //p->iError = 1;
          G_MMC_HighSpeedModeAllowed = ETrue;
          (*p->iSD_MMC)->PowerOff(p->iSD_MMC);
          return UEZ_ERROR_DEVICE_NOT_FOUND;
      } else { // failed start bits or timeout and card remained inserted
        G_MMC_HighSpeedModeAllowed = EFalse;
        (*p->iSD_MMC)->PowerOff(p->iSD_MMC);
      }   
    } else if (p->iError == 1) {
    } else { // cold bootup initialization, no errors yet
      //G_MMC_HighSpeedModeAllowed = ETrue;
    }

    // If already initialized, we are done!
    if (p->iInitPerformed)
        return UEZ_ERROR_NONE;
    

    p->iStat = STA_NOINIT;
    p->iSWWriteProtect = 0;
    p->iCardType = 0;
    p->iError = 0;
    p->iRCA = 0;
    p->iBlockLen = 0;
    memset(p->iCID, 0, 16);
    memset(p->iCSD, 0, 16);

    p->iBlockNum = 0;
    p->iStat = 1;
    p->iIsCompleteReadFlag = EFalse;
    p->iIsCompleteWriteFlag = EFalse;
    memset(response, 0, 16);

    (*p->iSD_MMC)->SetupErrorCallback(p->iSD_MMC, ISD_MMCError, p);
    // Executing a power cycle or issuing CMD0 will reset card to idle state.
    (*p->iSD_MMC)->PowerOn(p->iSD_MMC); // Force socket power on, partially reset peripherals and workspace variables where needed.
    
    (*p->iSD_MMC)->SetClockRate(p->iSD_MMC, SDCARD_SD_MMC_RATE_FOR_ID_STATE); // Will force clock to be on even when not sending commands.

    UEZTaskDelay(2);

    /* clear card type */
    (*p->iSD_MMC)->SetCardType(p->iSD_MMC, p->iCardType);

    /* No card in the socket? */
    if (p->iStat & STA_NODISK)
        return UEZ_ERROR_DEVICE_NOT_FOUND;

    status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_IDLE, 0, MCI_INT_CMD_DONE, response); // CMD0. Since clock is 400KHz mode, clock will run after this command.
    // Must clock 8 signals signals before starting init after CMD0.
    UEZTaskDelay(1); // allow for 8 signals before changing clock rate
    /* set 25.5MHz default speed mode now */ //UEZPlatform_MCI_DefaultFreq());
    (*p->iSD_MMC)->SetClockRate(p->iSD_MMC, MMC_LOW_BUS_MAX_CLOCK); // This will turn clock into normal power mode, off when not sending.

    while (state < 100) {
        switch (state) {
            case 0: /* Setup for SD */
                /* check if it is SDHC card */ // CMD8
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
                p->iCardType |= CARD_TYPE_SD; // After first command, set normal speed mode
                break;

            case 10: // CMD8 failed, not an SD card, restart init for MMC card. (NOT TESTED!)
                p->iCardType &= ~CARD_TYPE_SD;
                status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_IDLE, 0, MCI_INT_CMD_DONE, response);
                command = CMD_MMC_OP_COND;
                tries = INIT_OP_RETRIES;
                ocr |= OCR_HC_CCS;
                ++state;
                break;

            case 1:
            case 11:
                status = (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, command, 0, 0, response);
                if (status & MCI_INT_RTO) {
                    state += 9; /* NOT SD card */
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
                    resp2 = response[0];
                } while (!(resp2 & OCR_ALL_READY) && --tries > 0);

                if (resp2 & OCR_ALL_READY) {
                    /* is it high capacity card */
                    p->iCardType |= (resp2 & OCR_HC_CCS);
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

            case 20: // failed to init
                return UEZ_ERROR_DEVICE_NOT_FOUND;
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
        if (ISetCardParams(p, G_MMC_HighSpeedModeAllowed) != 0)
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
        if (UEZTickCounterGetDelta(start) >= aTimeout) {
            return UEZ_ERROR_TIMEOUT;
        }
        if ((*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_SEND_STATUS, (TUInt32)p->iRCA << 16UL, 1, responseCode)
            && ((responseCode[0] & 0x01E00) == 0x00800)) {
            break;
        }
        UEZTaskDelay(1);
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
    /*T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    _isr_UEZSemaphoreRelease(p->iIsCompleteRead);*/  
  
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    p->iIsCompleteReadFlag = ETrue;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISD_MMCCompleteWrite
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback routine to release the complete flag.
 *---------------------------------------------------------------------------*/
static void ISD_MMCCompleteWrite(void *aWorkspace)
{
    /*T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    _isr_UEZSemaphoreRelease(p->iIsCompleteWrite);*/
    
    T_MassStorage_SDCard_SD_MMC_Workspace *p =
        (T_MassStorage_SDCard_SD_MMC_Workspace *)aWorkspace;
    p->iIsCompleteWriteFlag = ETrue;
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
  TUInt32 start;
  T_uezError error = UEZ_ERROR_NONE;
  
  IGrab();
  
  // Clock is set to high speed in ISetCardParams after init and in order to allow for required minimum clocking out delay before switch.
  
  while (1) {
    for (uint8_t i = 0; i < SDCARD_FAILED_READ_RETRY_COUNT; i++) {
      error = UEZ_ERROR_NONE;
      /* Check parameter, count must be 1-127 */
      /* TODO: We should make this routine do sets of 127 blocks when larger */
      // Note that in real world use with video player you don't usually see large consecutive blocks
      // as files get scattered over the drives. We haven't tested 127 blocks yet.
      if ((aNumBlocks < 1) || (aNumBlocks > 127)) {
        error = UEZ_ERROR_INVALID_PARAMETER;
        break;
      }
      
      // Has this been init or did we fail and require re-init?
      if (!p->iInitPerformed) { // try re-init first
        SDCard_MS_SD_MMC_Init(p,0);
      }
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
        start = UEZTickCounterGet();
        while (ISDMMC_GetState(p) != SDMMC_TRAN_ST) {        
          UEZTaskDelay(0);
          if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
              break;
          }
          if (UEZTickCounterGetDelta(start) >= 25) {
            UEZTaskDelay(1);
            if (UEZTickCounterGetDelta(start) >= 1000) { // not hitting this timeout
              error = UEZ_ERROR_TIMEOUT;
              dprintf("RST");
              break; // 
            }
          }
        }
        if (ISetTransState(p) != 0) {          
          error = UEZ_ERROR_INTERNAL_ERROR;
          dprintf("RSS");
          break;
        }
      }
      
      if (p->iCardType & CARD_TYPE_HC) { // if high capacity card use block indexing
        index = aStart; 
      } else {            
        index = aStart << 9; // Currently fixed at 512 bytes per block
      }       
      
      p->iError = 0;
      
#if (MULTIBLOCK_READ_SUPPORTED==1)
      // set number of bytes to read
      //dprintf("R");
      error = (*p->iSD_MMC)->ReadyReception(p->iSD_MMC, aBuffer, aNumBlocks, 512,
                                            ISD_MMCCompleteRead, p, &numTransferring);
      if (numTransferring < aNumBlocks) {
        // TODO: Maybe come back and do multiple read/writes
        UEZFailureMsg("Transfer too big!");
      }
      
      p->iIsCompleteReadFlag = EFalse;
      
      //dprintf("E");
      // Select single or multiple read based on number of blocks 
      if (aNumBlocks == 1) {
        (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_READ_SINGLE, index, 0 | MCI_INT_DATA_OVER, response);
      } else {
        (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_READ_MULTIPLE, index, 0 | MCI_INT_DATA_OVER, response);
        // Don't do an actual task delay here. Only delay below if we start to stall. Always doing a delay will kill video player performance.
      }
#else
      /* // Disabled till further testing or if needed.
      // set number of bytes to read
      error = (*p->iSD_MMC)->ReadyReception(p->iSD_MMC, aBuffer, 1, 512,
      ISD_MMCCompleteRead, p, &numTransferring);
      
      // Select single or multiple read based on number of blocks 
      if (aNumBlocks == 1) {
      (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_READ_SINGLE, index, 0 | MCI_INT_DATA_OVER, response);
    } else {
      numTransferring = aNumBlocks;
      while(numTransferring > 0) {
      numTransferring--;
      TUInt32 dum = 0;
      (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_READ_SINGLE, index, 0 | MCI_INT_DATA_OVER, response);             
      if (p->iCardType & CARD_TYPE_HC) { // if high capacity card use block indexing
      index++;
    } else { // Currently fixed at 512 bytes per block
      index += 512;
    }
      aBuffer = (TUInt8 *) aBuffer +512;
      error = (*p->iSD_MMC)->ReadyReception(p->iSD_MMC, aBuffer, 1, 512,
      ISD_MMCCompleteRead, p, &dum);
    }
      // (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_READ_MULTIPLE, index, 0 | MCI_INT_DATA_OVER, response);
    }*/
#endif
      
      /* // TODO this isn't working correctly, but we check timeout on card state below which is sufficient.
      // Wait for card program to finish or timeout (5 seconds is more than enough!)
      error = UEZSemaphoreGrab(p->iIsCompleteRead, 1000*numTransferring);
    }*/
      
      start = UEZTickCounterGet();
      /*Wait for card program to finish*/
      while (p->iIsCompleteReadFlag == EFalse) {
        UEZTaskDelay(0);
        if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
          break;
        }
        if (UEZTickCounterGetDelta(start) >= 150) {
          UEZTaskDelay(1);
          if (UEZTickCounterGetDelta(start) >= 500) { //1000 * numTransferring) {  // TODO this may need to be higher? we freq timeout after doing write....
            error = UEZ_ERROR_TIMEOUT;
            dprintf("RTO");
            break; // 
          }
        }
        if(p->iError != 0) {
          error = UEZ_ERROR_TIMEOUT; // force retry
          //dprintf("RE");
          break; // don't wait on timeout
        }
      }
      
      start = UEZTickCounterGet();
      //Wait for card to get back into transfer starte, should be very quick.
      while (ISDMMC_GetState(p) != SDMMC_TRAN_ST) {
        UEZTaskDelay(0);
        if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
          break;
        }
        if (UEZTickCounterGetDelta(start) >= 100) {
            UEZTaskDelay(1);
            if (UEZTickCounterGetDelta(start) >=  500) { //( * numTransferring) {
              error = UEZ_ERROR_TIMEOUT;
              dprintf("RTE");
              break; // from testing so far we basically never hit this
            }
         }
      }
      
      if(error == UEZ_ERROR_NONE) {
        break;
      }
      
      if(error == UEZ_ERROR_TIMEOUT) {
        ISetTransState(p);
        //dprintf("RRT");
      }
    } // end for loop
    
    break;
  } // end while loop
  
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
    TUInt32 start;

    IGrab();

    // Clock is set to high speed in ISetCardParams after init and in order to allow for required minimum clocking out delay before switch.

    while (1) {
    for (uint8_t i = 0; i < SDCARD_FAILED_WRITE_RETRY_COUNT; i++) {
      error = UEZ_ERROR_NONE;
        /* Check parameter */
        if (aNumBlocks < 1 || aNumBlocks > 127) {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }

        // Has this been init or did we fail and require re-init?
        if (!p->iInitPerformed) { // try re-init first
          SDCard_MS_SD_MMC_Init(p,0);
        }
        if (!p->iInitPerformed) {
          error = UEZ_ERROR_NOT_READY;
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
        if ((aStart + aNumBlocks) > p->iBlockNum) {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }

        /*Wait for card program to finish*/
        //volatile TInt32 state = ISDMMC_GetState(p);
        start = UEZTickCounterGet();
        while (ISDMMC_GetState(p) != SDMMC_TRAN_ST) {        
            UEZTaskDelay(0);
            if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
              break;
            }
            if (UEZTickCounterGetDelta(start) >= 25) {
                UEZTaskDelay(1);
                if (UEZTickCounterGetDelta(start) >= 1000) {
                    error = UEZ_ERROR_TIMEOUT;
                    dprintf("WST");
                    break; // 
                }
            }
        }
        
        /* put card in trans state */
        if (ISetTransState(p) != 0) {
            error = UEZ_ERROR_INVALID_PARAMETER;
                dprintf("WSS");
            break;
        }

        /* if high capacity card use block indexing */
        if (p->iCardType & CARD_TYPE_HC) {
            index = aStart;
        } else {
            // fixed at 512 bytes
            index = aStart << 9;   // * p->iBlockLen; // same as x512
        }
        
        p->iError = 0;

#if (MULTIBLOCK_WRITE_SUPPORTED==1) // TODO this isn't fully implemented/working yet.
        /* set number of bytes to write */
        (*p->iSD_MMC)->ReadyTransmission(p->iSD_MMC, aBuffer, aNumBlocks, 512, &numWriting, ISD_MMCCompleteWrite, p);

        p->iIsCompleteWriteFlag = EFalse;
        
        // Single or multiple block write?
        if (aNumBlocks == 1) {
            (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_WRITE_SINGLE, index, 0 | MCI_INT_DATA_OVER, response);
        } else {
            (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_WRITE_MULTIPLE, index, 0 | MCI_INT_DATA_OVER, response);
        }
        
        start = UEZTickCounterGet();
        /*Wait for card program to finish*/
        while (p->iIsCompleteWriteFlag == EFalse) {
            UEZTaskDelay(0);
            if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
              break;
            }
            if (UEZTickCounterGetDelta(start) >= 50) {
                UEZTaskDelay(1);
                if (UEZTickCounterGetDelta(start) >= 5000 * numWriting) { // TODO this may need to be higher?
                    error = UEZ_ERROR_TIMEOUT;
                    dprintf("WTO");
                    break; // 
                }
            }
            if(p->iError != 0) {
                dprintf("WE");
              error = UEZ_ERROR_TIMEOUT; // force retry
              break; // don't wait on timeout
            }
        }

        // Wait for all blocks to be transferred
        /* //error = UEZSemaphoreGrab(p->iIsCompleteWrite, 1000 * numWriting); // for 32 blocks wait up to 960 ms

        if(error == UEZ_ERROR_TIMEOUT) {
            error = UEZ_ERROR_TIMEOUT;
            //break;
        }*/
        
        start = UEZTickCounterGet();
        //Wait for card to get back into transfer starte, should be very quick.
        while (ISDMMC_GetState(p) != SDMMC_TRAN_ST) {
            UEZTaskDelay(0);
            if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
              break;
            }
            if (UEZTickCounterGetDelta(start) >= 100) {
                UEZTaskDelay(1);
                if (UEZTickCounterGetDelta(start) >= 1000) {
                    error = UEZ_ERROR_TIMEOUT;
                    dprintf("WTE");
                    break; // from testing so far we basically never hit this
                }
            }
        }

#else
        TUInt32 dum = 0;
        /* set number of bytes to write */
        (*p->iSD_MMC)->ReadyTransmission(p->iSD_MMC, aBuffer, 1, 512, &dum, ISD_MMCCompleteWrite, p);
        p->iIsCompleteWriteFlag = EFalse;
    
        // Select single or multiple write based on number of blocks 
        if (aNumBlocks == 1) {
          (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_WRITE_SINGLE, index, 0 | MCI_INT_DATA_OVER, response);

          start = UEZTickCounterGet();
          /*Wait for card program to finish*/
          while (p->iIsCompleteWriteFlag == EFalse) {
              UEZTaskDelay(0);
              if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
                break;
              }
              if (UEZTickCounterGetDelta(start) >= 50) {
                UEZTaskDelay(1);
                if (UEZTickCounterGetDelta(start) >= 5000 * 1) { // TODO this may need to be higher?
                    error = UEZ_ERROR_TIMEOUT;
                    dprintf("WTO");
                    break; // 
                }
              }
              if(p->iError != 0) {
                  dprintf("WE");
                error = UEZ_ERROR_TIMEOUT; // force retry
                break; // don't wait on timeout
              }
          }
        
          start = UEZTickCounterGet();
          //Wait for card to get back into transfer starte, should be very quick.
          while (ISDMMC_GetState(p) != SDMMC_TRAN_ST) {
              UEZTaskDelay(0);
              if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
                break;
              }
              if (UEZTickCounterGetDelta(start) >= 100) {
                UEZTaskDelay(1);
                if (UEZTickCounterGetDelta(start) >= 1000) {
                    error = UEZ_ERROR_TIMEOUT;
                    dprintf("WTE");
                    break; // from testing so far we basically never hit this
                }
              }
          }

        } else {
          numWriting = aNumBlocks;
          while(numWriting > 0) {
            numWriting--;
            (*p->iSD_MMC)->ExecuteCommand(p->iSD_MMC, CMD_WRITE_SINGLE, index, 0 | MCI_INT_DATA_OVER, response);
            if (p->iCardType & CARD_TYPE_HC) { // if high capacity card use block indexing
              index++;
            } else { // Currently fixed at 512 bytes per block
              index += 512;
            }
            aBuffer = (TUInt8 *) aBuffer +512;

            start = UEZTickCounterGet();
            /*Wait for card program to finish*/
            while (p->iIsCompleteWriteFlag == EFalse) {
                UEZTaskDelay(0);
                if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
                  break;
                }
                if (UEZTickCounterGetDelta(start) >= 50) {
                  UEZTaskDelay(1);
                  if (UEZTickCounterGetDelta(start) >= 5000 * 1) { // TODO this may need to be higher?
                      error = UEZ_ERROR_TIMEOUT;
                      dprintf("WTO");
                      break; // 
                  }
                }
                if(p->iError != 0) {
                    dprintf("WE");
                  error = UEZ_ERROR_TIMEOUT; // force retry
                  break; // don't wait on timeout
                }
            }            
        
            start = UEZTickCounterGet();
            //Wait for card to get back into transfer starte, should be very quick.
            while (ISDMMC_GetState(p) != SDMMC_TRAN_ST) {
                UEZTaskDelay(0);
                if (p->iInitPerformed == EFalse) { // if we failed we need to re-init and retry from for loop start
                  break;
                }
                if (UEZTickCounterGetDelta(start) >= 100) {
                  UEZTaskDelay(1);
                  if (UEZTickCounterGetDelta(start) >= 1000) {
                      error = UEZ_ERROR_TIMEOUT;
                      dprintf("WTE");
                      break; // from testing so far we basically never hit this
                  }
                }
            }
            
            if(error == UEZ_ERROR_TIMEOUT) { // reset state and try again up to X times
              ISetTransState(p);
            }

            p->iError = 0;
            (*p->iSD_MMC)->ReadyTransmission(p->iSD_MMC, aBuffer, 1, 512, &dum, ISD_MMCCompleteWrite, p);
            p->iIsCompleteWriteFlag = EFalse;
          }     
        }
#endif
      if(error == UEZ_ERROR_NONE) {
        break;
      }
      
      if(error == UEZ_ERROR_TIMEOUT) { // reset state and try again up to X times
        ISetTransState(p);
        //dprintf("WRT");
      }
    } // end for loop
      break;
    } // end while loop
    
    if(error == UEZ_ERROR_TIMEOUT) {
       ISetTransState(p);
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
    error = IWaitReady(p, 1000); // same as LPC4088

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
 *      T_uezError            -- Error code
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

    if (!p->iInitPerformed) { // drive not initialized
        // Don't force perform init here
        // When we try to ready any file from SD card it will attempt init if not init
    }

    if (!p->iInitPerformed) { // card not initialized
      aInfo->iSectorSize = 512;
      aInfo->iNumSectors = 0;
      aInfo->iBlockSize = 0;
      error = UEZ_ERROR_NAK;
    } else {
      // Get the size of the sectors (just hard code to 512)
      aInfo->iSectorSize = 512;
      aInfo->iNumSectors = p->iBlockNum;

      // Determine the block size
      // SDC or MMC?
      
      if ((p->iCardType & CARD_TYPE_SD) == CARD_TYPE_SD) {//(p->iCardType & 2) {
          // SDC // TODO Currently wrong so report 512
          /*aInfo->iBlockSize = (((p_csd[10] & 63) << 1)
              + ((TUInt16)(p_csd[11] & 128) >> 7) + 1) << ((p_csd[13] >> 16) - 1); */          
          aInfo->iBlockSize = 512;
      } else {
          // MMC
          aInfo->iBlockSize = ((TUInt16)((p_csd[10] & 124) >> 2) + 1)
              * (((p_csd[11] & 3) << 3) + ((p_csd[11] & 224) >> 5) + 1);
      }
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
    //(*p->iMCI)->SetErrorCallback(p->iMCI, IMCIError, p);
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


