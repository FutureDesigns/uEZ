/*-------------------------------------------------------------------------*
 * File:  SDCard_MS_MCI_driver_SPI.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the SDCard MassStorage Device.
 * This implementation supports MMC, SDC version 1, and SDC version 2(SDHC).
 * This implementation was created from the original SDCard_MS_MCI_driver_SPI.c
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
#include <string.h>
#include "SDCard_MS_driver_MCI.h"
//#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC1788/LPC1788_GPIO.h>
#include <HAL/MCI.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*
 * Options:
 *---------------------------------------------------------------------------*/
#ifndef SDCARD_INIT_TIMEOUT
#define SDCARD_INIT_TIMEOUT                 4000    // ms, approximate
#endif
#ifndef SDCARD_DEBUG_OUTPUT
#define SDCARD_DEBUG_OUTPUT 0
#endif

#ifndef SDCARD_MCI_RATE_FOR_ID_STATE
#define SDCARD_MCI_RATE_FOR_ID_STATE            200000UL   // 400kHz (100-400kHz)
#endif
#ifndef SDCARD_MCI_RATE_FOR_RW_STATE
#define SDCARD_MCI_RATE_FOR_RW_STATE            20000000UL // 0000UL // 18000000UL // 18 MHz
#endif

#define SDCARD_FAILED_READ_RETRY_COUNT              5

#define STA_NOINIT      0x01    /* Drive not initialized */
#define STA_NODISK      0x02    /* No medium in the drive */
#define STA_PROTECT     0x04    /* Write protected */

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

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    const DEVICE_MassStorage *iDevice;
    TUInt8 G_sector[512];
    TUInt8 iCmd[50];
    TBool iSWWriteProtect;
    TUInt32 iRCA;       // Relative Card Address
    TUInt32 iOCR;
    TUInt16 iCardType;
    TUInt8 iCardInfo[16 + 16 + 4]; /* CSD(16), CID(16), OCR(4) */
    TBool iInitPerformed;
    T_uezSemaphore iSem;
    TUInt8 iStat;
    T_uezSemaphore iIsComplete;
    HAL_MCI **iMCI;
} T_MassStorage_SDCard_MCI_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static T_uezError ISDCard_MS_MCI_GetSizeInfo(
    void *aWorkspace,
    T_msSizeInfo *aInfo);

/*---------------------------------------------------------------------------*
 * Routine:  MassStorage_SDCard_MCI_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a MassStorage device's workspace on the SDCard.
 * Inputs:
 *      void *aWorkspace             -- Workspace
 * Outputs:
 *      T_uezError                   -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError MassStorage_SDCard_MCI_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error = UEZ_ERROR_NONE;
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;

    p->iCardType = 1;
    p->iInitPerformed = EFalse;
    p->iStat = STA_NOINIT;

    error = UEZSemaphoreCreateBinary(&p->iSem);
    if (error)
        return error;
    
    //error = UEZSemaphoreCreateCounting(&p->iIsComplete, 20, 0);
    error = UEZSemaphoreCreateCounting(&p->iIsComplete, 255, 0);
    if (error)
        return error;

    return error;
}

static void bswap_cp(TUInt8 *dst, const TUInt32 *src)
{
    TUInt32 d32;

    d32 = *src;
    *dst++ = (TUInt8)(d32 >> 24);
    *dst++ = (TUInt8)(d32 >> 16);
    *dst++ = (TUInt8)(d32 >> 8);
    *dst++ = (TUInt8)(d32 >> 0);
}

static T_uezError SDCard_MS_MCI_Init(void *aWorkspace, TUInt32 aAddress)
{
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;
    TUInt16 cmd;
    TUInt32 n;
    TUInt32 response[4];
    TUInt8 type;
    TUInt32 timeStart;
    T_msSizeInfo si;
    T_uezError error;

    /* No card in the socket? */
    if (p->iStat & STA_NODISK)
        return UEZ_ERROR_DEVICE_NOT_FOUND;

    // If already initialized, we are done!
    if (p->iInitPerformed)
        return UEZ_ERROR_NONE;

    (*p->iMCI)->PowerOn(p->iMCI); /* Force socket power on */
    (*p->iMCI)->SetClockRate(p->iMCI, SDCARD_MCI_RATE_FOR_ID_STATE, EFalse);
    UEZTaskDelay(2);
    (*p->iMCI)->SendCommand(p->iMCI, MCI_CMD0, 0, 0, NULL); /* Put the card into idle state */
    p->iRCA = 0;

    /*---- Card is 'idle' state ----*/

    /* Initialization timeout of SDCARD_INIT_TIMEOUT msec */
//    Timer[0] = 1000;
    timeStart = UEZTickCounterGet();

    if ((*p->iMCI)->SendCommand(p->iMCI, MCI_CMD8, 0x1AA, 1, response)
        && (response[0] & 0xFFF) == 0x1AA) {
        /* Card is SDC Ver2 */
        /* The card can work at vdd range of 2.7-3.6V */

        /* Wait while card is busy state (use MCI_ACMD41 with HCS bit) */
        do {
            /* This loop will take a time. Insert task rotation here for multitask envilonment. */
            UEZTaskDelay(1);
            if (UEZTickCounterGetDelta(timeStart) >= SDCARD_INIT_TIMEOUT)
                goto di_fail;
        } while (!(*p->iMCI)->SendCommand(p->iMCI, MCI_ACMD41, 0x40FF8000, 1,
            response) || !(response[0] & 0x80000000));
        type = (response[0] & 0x40000000) ? CT_SD2 | CT_BLOCK : CT_SD2; /* Check CCS bit in the OCR */
    } else {
        /* Card is SDC Ver1 or MMC */
        if ((*p->iMCI)->SendCommand(p->iMCI, MCI_ACMD41, 0x00FF8000, 1,
            response)) {
            type = CT_SD1;
            cmd = MCI_ACMD41; /* MCI_ACMD41 is accepted -> SDC Ver1 */
        } else {
            type = CT_MMC;
            cmd = MCI_CMD1; /* MCI_ACMD41 is rejected -> MMC */
        }

        /* Wait while card is busy state (use MCI_ACMD41 or MCI_CMD1) */
        do {
            /* This loop will take a time. Insert task rotation here for multitask envilonment. */
            UEZTaskDelay(1);
            if (UEZTickCounterGetDelta(timeStart) >= SDCARD_INIT_TIMEOUT)
                goto di_fail;
        } while (!(*p->iMCI)->SendCommand(p->iMCI, cmd, 0x00FF8000, 1, response)
            || !(response[0] & 0x80000000));
    }

    /* Save card type */
    p->iCardType = type;
    bswap_cp(&p->iCardInfo[32], response); /* Save OCR */

    /*---- Card is 'ready' state ----*/
    if (!(*p->iMCI)->SendCommand(p->iMCI, MCI_CMD2, 0, 2, response))
        goto di_fail;

    /* Enter ident state */
    for (n = 0; n < 4; n++)
        bswap_cp(&p->iCardInfo[n * 4 + 16], &response[n]); /* Save CID */

    /*---- Card is 'ident' state ----*/
    if (type & CT_SDC) {
        /* SDC: Get generated RCA and save it */
        if (!(*p->iMCI)->SendCommand(p->iMCI, MCI_CMD3, 0, 1, response))
            goto di_fail;
        p->iRCA = (TUInt16)(response[0] >> 16);
    } else {
        /* MMC: Assign RCA to the card */
        if (!(*p->iMCI)->SendCommand(p->iMCI, MCI_CMD3, 1 << 16, 1, response))
            goto di_fail;
        p->iRCA = 1;
    }
    (*p->iMCI)->SetRelativeCardAddress(p->iMCI, p->iRCA);

    /*---- Card is 'stby' standby state ----*/
    /* Get CSD and save it */
    if (!(*p->iMCI)->SendCommand(p->iMCI, MCI_CMD9, (TUInt32)p->iRCA << 16, 2,
        response))
        goto di_fail;
    for (n = 0; n < 4; n++)
        bswap_cp(&p->iCardInfo[n * 4], &response[n]);

    /* Select card */
    if (!(*p->iMCI)->SendCommand(p->iMCI, MCI_CMD7, (TUInt32)p->iRCA << 16, 1,
        response))
        goto di_fail;

    /*---- Card is 'tran' state ----*/

    /* Set data block length to 512 (for byte addressing cards) */
    if (!(type & CT_BLOCK)) {
        if (!(*p->iMCI)->SendCommand(p->iMCI, MCI_CMD16, 512, 1, response)
            || (response[0] & 0xFDF90000))
            goto di_fail;
    }

    /* Set wide bus mode (for SDCs) */
    if (type & CT_SDC) {
        /* Set wide bus mode of SDC */
        if (!(*p->iMCI)->SendCommand(p->iMCI, MCI_ACMD6, 2, 1, response)
            || (response[0] & 0xFDF90000))
            goto di_fail;

        /* Set wide bus mode of MCI */
        (*p->iMCI)->SetDataBus(p->iMCI, UEZ_MCI_BUS_4BIT_WIDE);
    }

    (*p->iMCI)->SetClockRate(p->iMCI, SDCARD_MCI_RATE_FOR_RW_STATE, ETrue);

    error = ISDCard_MS_MCI_GetSizeInfo(aWorkspace, &si);
    if (error)
        goto di_fail;

    /* Initialization succeeded */
    p->iInitPerformed = ETrue;
    p->iStat &= ~STA_NOINIT; /* Clear STA_NOINIT */
    return UEZ_ERROR_NONE;

di_fail:
    /* Initialization failed */
    p->iInitPerformed = EFalse;
    (*p->iMCI)->PowerOff(p->iMCI);
    p->iStat |= STA_NOINIT; /* Set STA_NOINIT */
    return UEZ_ERROR_INTERNAL_ERROR;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_Status
 *---------------------------------------------------------------------------*
 * Description:
 *      Send initialized status.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 *      T_msStatus *aStatus      -- Status value to send.
 * Outputs:
 *      T_uezError               -- always returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_MCI_Status(void *aWorkspace, T_msStatus *aStatus)
{
    T_msStatus status;
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;

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

/* Returns 1 when card is tran state, otherwise returns 0 */
/* tmr = Timeout in unit of 1ms */
static T_uezError IWaitReady(T_MassStorage_SDCard_MCI_Workspace *p, TUInt32 tmr)
{
    TUInt32 responseCode;
    TUInt32 start;

    start = UEZTickCounterGet();
    while (1) {
        if (UEZTickCounterGetDelta(start) >= tmr) {
            return UEZ_ERROR_TIMEOUT;
        }
        if ((*p->iMCI)->SendCommand(p->iMCI, MCI_CMD13, (TUInt32)p->iRCA << 16UL, 1, &responseCode)
            && ((responseCode & 0x01E00) == 0x00800)) {
            break;
        }

        UEZTaskDelay(5);
    }

    return UEZ_ERROR_NONE;
}

static void IMCIComplete(void *aWorkspace)
{
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;
    _isr_UEZSemaphoreRelease(p->iIsComplete);
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_Read
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
static T_uezError SDCard_MS_MCI_Read(
    void *aWorkspace,
    const TUInt32 aStart,
    const TUInt32 aNumBlocks,
    void *aBuffer)
{
    T_uezError error;
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;
    TUInt32 response;
    TUInt16 cmd;
    TUInt32 count = aNumBlocks;
    TUInt32 sector = aStart;
    TUInt8 *buff = aBuffer;
    TUInt8 *buffStart = aBuffer;
    TUInt8 retry = SDCARD_FAILED_READ_RETRY_COUNT;
    TUInt32 numTransferring;
    TUInt32 countLeft = 0;

    IGrab();
    dprintf("SDCard Read: %d %d\n", aStart, aNumBlocks);

    while (1) {
        /* Check parameter, count must be 1-127 */
        /* TODO: We should make this routine do sets of 127 blocks when larger */
        if (count < 1 || count > 127) {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }

        /* Check drive status */
        if (p->iStat & STA_NOINIT) {
            error = UEZ_ERROR_NOT_READY;
            break;
        }
        if (!p->iInitPerformed) {
            error = UEZ_ERROR_NOT_READY;
            break;
        }

        /* Convert LBA to byte address if needed */
        if (!(p->iCardType & CT_BLOCK))
            sector *= 512;

        /* Make sure that card is tran state */
        if (IWaitReady(p, 500) != UEZ_ERROR_NONE) {
            error = UEZ_ERROR_TIMEOUT;
            break;
        }

        do {
            while (retry) {
                /* Ready to receive data blocks up to count in size */
                /* numTransferring tells how many the MCI driver can handle in
                 * one stream. */
                dprintf("Ready: %d ", count);
                error = (*p->iMCI)->ReadyReception(p->iMCI, count, 512,
                    IMCIComplete, p, &numTransferring);
                dprintf("%d\n", numTransferring);
                if (error)
                    break;

                /* Transfer type: Single block or Multiple block */
                cmd = (numTransferring > 1) ? MCI_CMD18 : MCI_CMD17;
                /* Start the reading */
                if ((*p->iMCI)->SendCommand(p->iMCI, cmd, sector, 1, &response)
                    && !(response & 0xC0580000)) {
                    dprintf("{");
                    countLeft = numTransferring;
                    buff = buffStart;
                    do {
                        while (1) {
                            // Wait for a block or timeout
                            dprintf("?");
                            UEZSemaphoreGrab(p->iIsComplete, 500);
                            error = (*p->iMCI)->Read(p->iMCI, buff, 512);
                            if (error == UEZ_ERROR_OUT_OF_DATA) {
                                dprintf("-");
                                continue;
                            }
                            if (error) {
                                dprintf("!");
                                error = UEZ_ERROR_READ_WRITE_ERROR;
                                break;
                            }
                            if (error == UEZ_ERROR_NONE) {
                                dprintf("+");
                                break;
                            }
                            dprintf(".");
                        }
                        if (error)
                            break;

                        /* Next user buffer address */
                        buff += 512;
                    } while (--countLeft);
                }
                dprintf("}");
                (*p->iMCI)->StopTransfer(p->iMCI); /* Close data path */
                if (countLeft || cmd == MCI_CMD18) /* Terminate to read if needed */
                    (*p->iMCI)->SendCommand(p->iMCI, MCI_CMD12, 0, 1, &response);

                if (error) {
                    dprintf("Retry\n");
                    retry--;
                } else {
                    dprintf("Done\n");
                    break;
                }
            }
            if (error)
                break;

            // We have now done a number of blocks
            // If there are no blocks, stop looping
            // Otherwise, setup for another transfer
            count -= numTransferring;
            if (count == 0)
                break;
            if (!(p->iCardType & CT_BLOCK))
                sector += numTransferring * 512;
            else
                sector += numTransferring;
            buffStart += numTransferring * 512;
        } while (1);

        error = count ? UEZ_ERROR_READ_WRITE_ERROR : UEZ_ERROR_NONE;
        break;
    }

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_Write
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
static T_uezError SDCard_MS_MCI_Write(
    void *aWorkspace,
    TUInt32 aStart,
    TUInt32 aNumBlocks,
    const void *aBuffer)
{
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 num = aNumBlocks;
    TUInt16 command;
    TUInt32 response;
    TUInt32 numWriting;
    TUInt32 sector = aStart;
    const TUInt8 *buffStart = aBuffer;

    IGrab();

    while (1) {
        /* Check parameter */
        if (num < 1 || num > 127) {
            error = UEZ_ERROR_INVALID_PARAMETER;
            break;
        }
        /* Check drive status */
        if (p->iStat & STA_NOINIT) {
            error = UEZ_ERROR_NOT_READY;
            break;
        }
        /* Check write protection */
        if (p->iStat & STA_PROTECT) {
            error = UEZ_ERROR_PROTECTED;
            break;
        }

        /* Convert LBA to byte address if needed */
        if (!(p->iCardType & CT_BLOCK))
            sector *= 512;

        // Write in multiple runs
        while (aNumBlocks) {
            /* Make sure that card is tran state */
            if (IWaitReady(p, 500) != UEZ_ERROR_NONE) {
                error = UEZ_ERROR_TIMEOUT;
                break;
            }

            // Prepare the number of blocks we are going to transfer
// TODO: Multi-block writes are not working dependably yet! -- LES 3/15/2013
            (*p->iMCI)->ReadyTransmission(p->iMCI, 1 /* aNumBlocks */, 512, &numWriting);

            /* Single or multiple block write? */
            if (numWriting == 1) {
                /* Single block write */
                command = MCI_CMD24;
            } else {
                /* Multiple block write */
                command = (p->iCardType & CT_SDC) ? MCI_ACMD23 : MCI_CMD23;

                /* Preset number of blocks to write */
                if (!(*p->iMCI)->SendCommand(p->iMCI, command, numWriting, 1, &response)
                    || (response & 0xC0580000)) {
                    error = UEZ_ERROR_READ_WRITE_ERROR;
                    break;
                }
                command = MCI_CMD25;
            }

            /* Send a write command */
            if (!(*p->iMCI)->SendCommand(p->iMCI, command, sector, 1, &response)
                || (response & 0xC0580000)) {
                error = UEZ_ERROR_READ_WRITE_ERROR;
                break;
            }

            // Prepare a number of blocks for writing
            num = numWriting;
            while (num && ((*p->iMCI)->IsWriteAvailable(p->iMCI))) {
                // Write out a block into the waiting MCI commands
                (*p->iMCI)->Write(p->iMCI, buffStart, 512);
                buffStart += 512;
                num--;
            };

            // Start transmitting, but report when we get a transmission complete
            (*p->iMCI)->StartTransmission(p->iMCI, IMCIComplete, p);

            // Wait for all blocks to be transferred
            UEZSemaphoreGrab(p->iIsComplete, 500 * numWriting);

            /* Close data path */
            (*p->iMCI)->StopTransfer(p->iMCI);

            /* Terminate to write if needed */
            if ((command == MCI_CMD25) && (p->iCardType & CT_SDC))
                (*p->iMCI)->SendCommand(p->iMCI, MCI_CMD12, 0, 1, &response);

            // We have a few less blocks to transfer
            aNumBlocks -= numWriting;

            // Determine next sector
            if (!(p->iCardType & CT_BLOCK))
                sector += 512*numWriting;
            else
                sector += numWriting;
        };

        break;
    }

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_Sync
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
static T_uezError SDCard_MS_MCI_Sync(void *aWorkspace)
{
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;
    T_uezError error;

    IGrab();

    // Just wait until we are ready or timeout
    error = IWaitReady(p, 500);

    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  ISDCard_MS_MCI_GetSizeInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Fill in the workspace size variables
 * Inputs:
 *      void *aWorkspace      -- Workspace for this instance.
 *      T_msSizeInfo *aInfo   -- Pointer to a structure to receive info.
 * Outputs:
 *      T_uezError            -- always returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError ISDCard_MS_MCI_GetSizeInfo(
    void *aWorkspace,
    T_msSizeInfo *aInfo)
{
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;
    TUInt8 *p_csd = (TUInt8 *)p->iCardInfo;
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
        n = (p_csd[5] & 15) + ((p_csd[10] & 128) >> 7) + ((p_csd[9] & 3) << 1)
            + 2;
        v = (p_csd[8] >> 6) + ((TUInt16)p_csd[7] << 2)
            + ((TUInt16)(p_csd[6] & 3) << 10) + 1;
        aInfo->iNumSectors = (TUInt32)v << (n - 9);
    }

    // Get the size of the sectors (just hard code to 512)
    aInfo->iSectorSize = 512;

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

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_GetSizeInfo
 *---------------------------------------------------------------------------*
 * Description:
 *      Fill in the workspace size variables
 * Inputs:
 *      void *aWorkspace      -- Workspace for this instance.
 *      T_msSizeInfo *aInfo   -- Pointer to a structure to receive info.
 * Outputs:
 *      T_uezError            -- always returns UEZ_ERROR_NONE.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_MCI_GetSizeInfo(
    void *aWorkspace,
    T_msSizeInfo *aInfo)
{
    T_uezError error;
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;

    IGrab();
    error = ISDCard_MS_MCI_GetSizeInfo(aWorkspace, aInfo);
    IRelease();

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_SetPower
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
static T_uezError SDCard_MS_MCI_SetPower(void *aWorkspace, TBool aOn)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_SetLock
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
static T_uezError SDCard_MS_MCI_SetLock(void *aWorkspace, TBool aLock)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_SetSoftwareWriteProtect
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
static T_uezError SDCard_MS_MCI_SetSoftwareWriteProtect(
    void *aWorkspace,
    TBool aSWWriteProtect)
{
    T_MassStorage_SDCard_MCI_Workspace *p =
        (T_MassStorage_SDCard_MCI_Workspace *)aWorkspace;
    IGrab();
    p->iSWWriteProtect = aSWWriteProtect;
    IRelease();
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_Eject
 *---------------------------------------------------------------------------*
 * Description:
 *      Unsupported Function.  Just a placeholder in a structure of functions.
 * Inputs:
 *      void *aWorkspace         -- Workspace for this instance.
 * Outputs:
 *      T_uezError               -- always returns
 *                                  UEZ_ERROR_ILLEGAL_OPERATION.
 *---------------------------------------------------------------------------*/
static T_uezError SDCard_MS_MCI_Eject(void *aWorkspace)
{
    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Routine:  SDCard_MS_MCI_MiscControl
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
static T_uezError SDCard_MS_MCI_MiscControl(
    void *aWorkspace,
    TUInt32 aControlCode,
    void *aBuffer)
{
    return UEZ_ERROR_ILLEGAL_OPERATION;
}

void MassStorage_SDCard_MCI_Create(const char *aName, const char *aHALMCI, const char *aDMA)
{
    DEVICE_MassStorage **p_ms;
    T_MassStorage_SDCard_MCI_Workspace *p;

    // Register the MMC card device
    UEZDeviceTableRegister(aName,
        (T_uezDeviceInterface *)&MassStorage_SDCard_MCI_Interface, 0,
        (T_uezDeviceWorkspace **)&p_ms);
    p = (T_MassStorage_SDCard_MCI_Workspace *)p_ms;


    HALInterfaceFind(aHALMCI, (T_halWorkspace **)&p->iMCI);
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_MassStorage MassStorage_SDCard_MCI_Interface = { {
// Common device interface
    "MS:USB",
    0x0100,
    MassStorage_SDCard_MCI_InitializeWorkspace,
    sizeof(T_MassStorage_SDCard_MCI_Workspace), },

// Functions
    SDCard_MS_MCI_Init,
    SDCard_MS_MCI_Status,
    SDCard_MS_MCI_Read,
    SDCard_MS_MCI_Write,
    SDCard_MS_MCI_Sync,
    SDCard_MS_MCI_GetSizeInfo,
    SDCard_MS_MCI_SetPower,
    SDCard_MS_MCI_SetLock,
    SDCard_MS_MCI_SetSoftwareWriteProtect,
    SDCard_MS_MCI_Eject,
    SDCard_MS_MCI_MiscControl, };

/*-------------------------------------------------------------------------*
 * End of File:  SDCard_MS_MCI_driver_SPI.c
 *-------------------------------------------------------------------------*/

