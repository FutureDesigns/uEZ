/*-------------------------------------------------------------------------*
 * File:  LPC546xx_SD_MMC.c
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <string.h>
#include <uEZ.h>
#include <uEZProcessor.h>
#include <uEZPlatformAPI.h>
#include <HAL/Interrupt.h>
#include "LPC546xx_SD_MMC.h"
#include "LPC546xx_GPIO.h"
#include "IAR/include/CMSIS/LPC546xx.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef NUM_MAX_BLOCKS
    #define NUM_MAX_BLOCKS               128         /* Block transfer FIFO depth (>= 2), 20+ recommended */
#endif

/** @brief SDIO FIFO threshold defines
 */
#define MCI_FIFOTH_TX_WM(x)     ((x) & 0xFFF)
#define MCI_FIFOTH_RX_WM(x)     (((x) & 0xFFF) << 16)
#define MCI_FIFOTH_DMA_MTS_1    (0UL << 28)
#define MCI_FIFOTH_DMA_MTS_4    (1UL << 28)
#define MCI_FIFOTH_DMA_MTS_8    (2UL << 28)
#define MCI_FIFOTH_DMA_MTS_16   (3UL << 28)
#define MCI_FIFOTH_DMA_MTS_32   (4UL << 28)
#define MCI_FIFOTH_DMA_MTS_64   (5UL << 28)
#define MCI_FIFOTH_DMA_MTS_128  (6UL << 28)
#define MCI_FIFOTH_DMA_MTS_256  (7UL << 28)

/** @brief Bus mode register defines
 */
#define MCI_BMOD_PBL1           (0 << 8)        /*!< Burst length = 1 */
#define MCI_BMOD_PBL4           (1 << 8)        /*!< Burst length = 4 */
#define MCI_BMOD_PBL8           (2 << 8)        /*!< Burst length = 8 */
#define MCI_BMOD_PBL16          (3 << 8)        /*!< Burst length = 16 */
#define MCI_BMOD_PBL32          (4 << 8)        /*!< Burst length = 32 */
#define MCI_BMOD_PBL64          (5 << 8)        /*!< Burst length = 64 */
#define MCI_BMOD_PBL128         (6 << 8)        /*!< Burst length = 128 */
#define MCI_BMOD_PBL256         (7 << 8)        /*!< Burst length = 256 */
#define MCI_BMOD_DE             (1 << 7)        /*!< Enable internal DMAC */
#define MCI_BMOD_DSL(len)       ((len) << 2)    /*!< Descriptor skip length */
#define MCI_BMOD_FB             (1 << 1)        /*!< Fixed bursts */
#define MCI_BMOD_SWR            (1 << 0)        /*!< Software reset of internal registers */

/** @brief  SDIO control register defines
 */
#define MCI_CTRL_USE_INT_DMAC   (1 << 25)       /*!< Use internal DMA */
#define MCI_CTRL_CARDV_MASK     (0x7 << 16)     /*!< SD_VOLT[2:0} pins output state mask */
#define MCI_CTRL_CEATA_INT_EN   (1 << 11)       /*!< Enable CE-ATA interrupts */
#define MCI_CTRL_SEND_AS_CCSD   (1 << 10)       /*!< Send auto-stop */
#define MCI_CTRL_SEND_CCSD      (1 << 9)        /*!< Send CCSD */
#define MCI_CTRL_ABRT_READ_DATA (1 << 8)        /*!< Abort read data */
#define MCI_CTRL_SEND_IRQ_RESP  (1 << 7)        /*!< Send auto-IRQ response */
#define MCI_CTRL_READ_WAIT      (1 << 6)        /*!< Assert read-wait for SDIO */
#define MCI_CTRL_INT_ENABLE     (1 << 4)        /*!< Global interrupt enable */
#define MCI_CTRL_DMA_RESET      (1 << 2)        /*!< Reset internal DMA */
#define MCI_CTRL_FIFO_RESET     (1 << 1)        /*!< Reset data FIFO pointers */
#define MCI_CTRL_RESET          (1 << 0)        /*!< Reset controller */

/** @brief SDIO Power Enable register defines
 */
#define MCI_POWER_ENABLE        0x1             /*!< Enable slot power signal (SD_POW) */

/** @brief SDIO Clock source register defines
 */
#define MCI_CLKSRC_CLKDIV0      0
#define MCI_CLKSRC_CLKDIV1      1
#define MCI_CLKSRC_CLKDIV2      2
#define MCI_CLKSRC_CLKDIV3      3
#define MCI_CLK_SOURCE(clksrc)  (clksrc)        /*!< Set cklock divider source */

/** @brief Commonly used definitions
 */
#define SD_FIFO_SZ              32              /*!< Size of SDIO FIFOs (32-bit wide) */

/** @brief SDIO Command register defines
 */
#define MCI_CMD_START           (1UL << 31)     /*!< Start command */
#define MCI_CMD_VOLT_SWITCH     (1 << 28)       /*!< Voltage switch bit */
#define MCI_CMD_BOOT_MODE       (1 << 27)       /*!< Boot mode */
#define MCI_CMD_DISABLE_BOOT    (1 << 26)       /*!< Disable boot */
#define MCI_CMD_EXPECT_BOOT_ACK (1 << 25)       /*!< Expect boot ack */
#define MCI_CMD_ENABLE_BOOT     (1 << 24)       /*!< Enable boot */
#define MCI_CMD_CCS_EXP         (1 << 23)       /*!< CCS expected */
#define MCI_CMD_CEATA_RD        (1 << 22)       /*!< CE-ATA read in progress */
#define MCI_CMD_UPD_CLK         (1 << 21)       /*!< Update clock register only */
#define MCI_CMD_INIT            (1 << 15)       /*!< Send init sequence */
#define MCI_CMD_STOP            (1 << 14)       /*!< Stop/abort command */
#define MCI_CMD_PRV_DAT_WAIT    (1 << 13)       /*!< Wait before send */
#define MCI_CMD_SEND_STOP       (1 << 12)       /*!< Send auto-stop */
#define MCI_CMD_STRM_MODE       (1 << 11)       /*!< Stream transfer mode */
#define MCI_CMD_DAT_WR          (1 << 10)       /*!< Read(0)/Write(1) selection */
#define MCI_CMD_DAT_EXP         (1 << 9)        /*!< Data expected */
#define MCI_CMD_RESP_CRC        (1 << 8)        /*!< Check response CRC */
#define MCI_CMD_RESP_LONG       (1 << 7)        /*!< Response length */
#define MCI_CMD_RESP_EXP        (1 << 6)        /*!< Response expected */
#define MCI_CMD_INDX(n)         ((n) & 0x1F)

/** @brief SDIO Clock Enable register defines
 */
#define MCI_CLKEN_LOW_PWR       (1 << 16)       /*!< Enable clock idle for slot */
#define MCI_CLKEN_ENABLE        (1 << 0)        /*!< Enable slot clock */


/** @brief  SDIO chained DMA descriptor
 */
typedef struct {
    volatile uint32_t des0;                     /*!< Control and status */
    volatile uint32_t des1;                     /*!< Buffer size(s) */
    volatile uint32_t des2;                     /*!< Buffer address pointer 1 */
    volatile uint32_t des3;                     /*!< Buffer address pointer 2 */
} LPC546xx_SD_MMC_DMADescriptor;

/** @brief  SDIO DMA descriptor control (des0) register defines
 */
#define MCI_DMADES0_OWN         (1UL << 31)     /*!< DMA owns descriptor bit */
#define MCI_DMADES0_CES         (1 << 30)       /*!< Card Error Summary bit */
#define MCI_DMADES0_ER          (1 << 5)        /*!< End of descriptopr ring bit */
#define MCI_DMADES0_CH          (1 << 4)        /*!< Second address chained bit */
#define MCI_DMADES0_FS          (1 << 3)        /*!< First descriptor bit */
#define MCI_DMADES0_LD          (1 << 2)        /*!< Last descriptor bit */
#define MCI_DMADES0_DIC         (1 << 1)        /*!< Disable interrupt on completion bit */

/** @brief  SDIO DMA descriptor size (des1) register defines
 */
#define MCI_DMADES1_BS1(x)      (x)             /*!< Size of buffer 1 */
#define MCI_DMADES1_BS2(x)      ((x) << 13)     /*!< Size of buffer 2 */
#define MCI_DMADES1_MAXTR       4096            /*!< Max transfer size per buffer */

/** @brief SDIO Clock divider register defines
 */
#define MCI_CLOCK_DIVIDER(dn, d2) ((d2) << ((dn) * 8))  /*!< Set cklock divider */

/** @brief  SDIO device type
 */
#define TRANSFER_STATUS_READING             0x01
#define TRANSFER_STATUS_WRITING             0x02
#define TRANSFER_STATUS_BUFFER_OVERRUN      0x04
#define TRANSFER_STATUS_SD_MMC_ERROR           0x08
#define TRANSFER_STATUS_DONE                0x10
#define TRANSFER_STATUS_ANY_ERROR   \
    (TRANSFER_STATUS_BUFFER_OVERRUN|TRANSFER_STATUS_SD_MMC_ERROR)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    /* MCI_IRQ_CB_FUNC_T irq_cb; */
        LPC546xx_SD_MMC_DMADescriptor mci_dma_dd[1 + (0x10000 / MCI_DMADES1_MAXTR)];
    /* uint32_t sdio_clk_rate; */
    /* uint32_t sdif_slot_clk_rate; */
    /* int32_t clock_enabled; */
} LPC546xx_SD_MMC_DMAArea;

/*-------------------------------------------------------------------------*
 * Workspace:
 *-------------------------------------------------------------------------*/
typedef struct {
    const HAL_SD_MMC *iHAL;

    T_uezGPIOPortPin iCardDetect;
    T_uezGPIOPortPin iWriteProtect;

    LPC546xx_SD_MMC_DMAArea iDMAArea;

    T_SD_MMCReceptionComplete iReceptionCompleteCallback;
    void *iReceptionCompleteCallbackWorkspace;

    T_SD_MMCTransmissionComplete iTransmissionCompleteCallback;
    void *iTransmissionCompleteCallbackWorkspace;

    TUInt32 iRCA;       // Relative Card Address

    TUInt32 iSpeed;     // Speed of clock rate in Hz

    volatile TUInt8 sdio_wait_exit;
} T_LPC546xx_SD_MMC_Workspace;

T_LPC546xx_SD_MMC_Workspace *G_SD_MMCWorkspace;
static TUInt32 G_SourceClkFrequency;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC546xx_SD_MMC_ProcessInterrupt(T_LPC546xx_SD_MMC_Workspace *p);

/*---------------------------------------------------------------------------*
 * Interrupt:  LPC546xx_SD_MMC_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      A SD_MMC Interrupt has come in.  Process the one workspace attached
 *      to this interrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(LPC546xx_SD_MMC_Interrupt)
{
    IRQ_START();
    LPC546xx_SD_MMC_ProcessInterrupt(G_SD_MMCWorkspace);
    IRQ_END();
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SD_MMC_ProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Process an interrupt for this workspace
 * Inputs:
 *      void *aWorkspace          -- SD_MMC Workspace
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void LPC546xx_SD_MMC_ProcessInterrupt(T_LPC546xx_SD_MMC_Workspace *p)
{
    InterruptDisable(SDIO_IRQn);

    // Did we finish a DMA?
    if (LPC_SDMMC->RINTSTS & MCI_INT_DATA_OVER) {
        // Clear the data transfer complete flag
        LPC_SDMMC->RINTSTS = MCI_INT_DATA_OVER;
        // We doing a reception callback?  You only get one
        if (p->iReceptionCompleteCallback) {
            p->iReceptionCompleteCallback(p->iReceptionCompleteCallbackWorkspace);
            p->iReceptionCompleteCallback = 0;
        }

        // We doing a transmission callback?  You only get one.
        if (p->iTransmissionCompleteCallback) {
            p->iTransmissionCompleteCallback(p->iTransmissionCompleteCallbackWorkspace);
            p->iTransmissionCompleteCallback = 0;
        }
    }
    if (LPC_SDMMC->RINTSTS & MCI_INT_CMD_DONE) {
        //LPC_SDMMC->RINTSTS = MCI_INT_CMD_DONE;
        if (p->sdio_wait_exit != 0) {
            // Set this interrupt for SDIO commands
            p->sdio_wait_exit = 2;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SD_MMC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_SD_MMC_InitializeWorkspace(void *aWorkspace)
{
    T_LPC546xx_SD_MMC_Workspace *p = (T_LPC546xx_SD_MMC_Workspace *)aWorkspace;
    p->iHAL = &LPC546xx_SD_MMC_Interface;

    p->iCardDetect = GPIO_NONE;
    p->iWriteProtect = GPIO_NONE;
    p->iReceptionCompleteCallback = 0;
    p->iReceptionCompleteCallbackWorkspace = 0;
    p->iTransmissionCompleteCallback = 0;
    p->iTransmissionCompleteCallbackWorkspace = 0;
    p->iRCA = 0;
    p->sdio_wait_exit = 0;

    G_SD_MMCWorkspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SD_MMC_PowerOn
 *---------------------------------------------------------------------------*
 * Description:
 *      Have the SD_MMC power on the target
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_SD_MMC_PowerOn(void *aWorkspace)
{
    // Turn on the power (if not already on), wait 10 ms minimum,
    // and then enable the signals
    LPC_SDMMC->PWREN = 0x01;
    UEZTaskDelay(10);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SD_MMC_PowerOff
 *---------------------------------------------------------------------------*
 * Description:
 *      Have the SD_MMC power off the target
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_SD_MMC_PowerOff(void *aWorkspace)
{
    LPC_SDMMC->INTMASK = 0;
    LPC_SDMMC->CTRL   &= ~(1<<4);
    LPC_SDMMC->CTRL = 1;
    LPC_SDMMC->RINTSTS = 0xFFFFFFFF;

    LPC_SDMMC->PWREN = 0x00;
    LPC_SDMMC->CLKENA = 0;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SD_MMC_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Reset the SD_MMC Controller
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_SD_MMC_Reset(void *aWorkspace)
{
    /* Software reset */
    LPC_SDMMC->BMOD = MCI_BMOD_SWR;

    /* reset all blocks */
    LPC_SDMMC->CTRL = MCI_CTRL_RESET | MCI_CTRL_FIFO_RESET | MCI_CTRL_DMA_RESET;
    while (LPC_SDMMC->CTRL & (MCI_CTRL_RESET | MCI_CTRL_FIFO_RESET | MCI_CTRL_DMA_RESET))
        {}

    /* Internal DMA setup for control register */
    LPC_SDMMC->CTRL = MCI_CTRL_USE_INT_DMAC | MCI_CTRL_INT_ENABLE;
    LPC_SDMMC->INTMASK = 0;

    /* Clear the interrupts for the host controller */
    LPC_SDMMC->RINTSTS = 0xFFFFFFFF;

    /* Put in max timeout */
    LPC_SDMMC->TMOUT = 0xFFFFFFFF;

    /* FIFO threshold settings for DMA, DMA burst of 4,   FIFO watermark at 16 */
    LPC_SDMMC->FIFOTH = MCI_FIFOTH_DMA_MTS_4 | MCI_FIFOTH_RX_WM((SD_FIFO_SZ / 2) - 1) | MCI_FIFOTH_TX_WM(SD_FIFO_SZ / 2);

    /* Enable internal DMA, burst size of 4, fixed burst */
    LPC_SDMMC->BMOD = MCI_BMOD_DE | MCI_BMOD_PBL4 | MCI_BMOD_DSL(4);

    /* disable clock to CIU (needs latch) */
    LPC_SDMMC->CLKENA = 0;
    LPC_SDMMC->CLKSRC = 0;

    return UEZ_ERROR_NONE;
}

static int ISendCommand(unsigned int aCommand, unsigned int aArgument)
{
    volatile int32_t timeout = 50;
    volatile int delay; // very fast delays

    // Set command argument
    LPC_SDMMC->CMDARG = aArgument;
    LPC_SDMMC->CMD = MCI_CMD_START | aCommand;

    // Poll until accepted
    while (--timeout && (LPC_SDMMC->CMD & MCI_CMD_START)) {
        if (timeout & 1) {
            delay = 50;
        }
        else {
            delay = 18000;
        }

        while (--delay > 1) {}
    }

    return (timeout < 1) ? 1 : 0;
}

/* Read the response from the last command */
static void IGetResponse(uint32_t *resp)
{
    /* on this chip response is not a fifo so read all 4 regs */
    resp[0] = LPC_SDMMC->RESP0;
    resp[1] = LPC_SDMMC->RESP1;
    resp[2] = LPC_SDMMC->RESP2;
    resp[3] = LPC_SDMMC->RESP3;
}

/* Setup DMA descriptors */
void ISetupDMA(LPC546xx_SD_MMC_DMAArea *aDMAArea, TUInt32 addr, TUInt32 size)
{
    int i = 0;
    TUInt32 ctrl, maxs;

    // Reset DMA
    LPC_SDMMC->CTRL |= MCI_CTRL_DMA_RESET | MCI_CTRL_FIFO_RESET;
    while (LPC_SDMMC->CTRL & MCI_CTRL_DMA_RESET) {
    }

    // Build a descriptor list using the chained DMA method
    while (size > 0) {
        // Limit size of the transfer to maximum buffer size
        maxs = size;
        if (maxs > MCI_DMADES1_MAXTR) {
            maxs = MCI_DMADES1_MAXTR;
        }
        size -= maxs;

        /* Set buffer size */
        aDMAArea->mci_dma_dd[i].des1 = MCI_DMADES1_BS1(maxs);

        // Setup buffer address (chained)
        aDMAArea->mci_dma_dd[i].des2 = addr + (i * MCI_DMADES1_MAXTR);

        // Setup basic control
        ctrl = MCI_DMADES0_OWN | MCI_DMADES0_CH;
        if (i == 0)
            ctrl |= MCI_DMADES0_FS; // First DMA buffer

        // No more data? Then this is the last descriptor
        if (!size) {
            ctrl |= MCI_DMADES0_LD;
        } else {
            ctrl |= MCI_DMADES0_DIC;
        }

        // Another descriptor is needed
        aDMAArea->mci_dma_dd[i].des3 = (uint32_t)&aDMAArea->mci_dma_dd[i + 1];
        aDMAArea->mci_dma_dd[i].des0 = ctrl;

        i++;
    }

    // Set DMA descriptor base address
    LPC_SDMMC->DBADDR = (uint32_t)&aDMAArea->mci_dma_dd[0];
}

/* Function to clear interrupt & FIFOs */
static void ISetClearIntFifo(void)
{
    /* reset all blocks */
    LPC_SDMMC->CTRL |= MCI_CTRL_FIFO_RESET;

    /* wait till resets clear */
    while (LPC_SDMMC->CTRL & MCI_CTRL_FIFO_RESET) {}

    /* Clear interrupt status */
    LPC_SDMMC->RINTSTS = 0xFFFFFFFF;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SD_MMC_ReadyTransmission
 *---------------------------------------------------------------------------*
 * Description:
 *      Prepare for a number of write block transactions.
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 *      TUInt32 aNumBlocks      -- Number of total blocks to write
 *      TUInt32 aBlockSize      -- Size of each block (only 512 supported)
 *      T_SD_MMCReceptionComplete aTranmissionCallback -- Routine to call
 *                                  when complete from interrupt.
 *      void *aTransmissionCallbackWorkspace -- Workspace for callback.
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_SD_MMC_ReadyTransmission(
    void *aWorkspace,
    const TUInt8 *aBuffer,
    TUInt32 aNumBlocks,
    TUInt32 aBlockSize,
    TUInt32 *aNumWriting,
    T_SD_MMCTransmissionComplete aTransmissionCallback,
    void *aTransmissionCallbackWorkspace)
{
    T_LPC546xx_SD_MMC_Workspace *p = (T_LPC546xx_SD_MMC_Workspace *)aWorkspace;
    TUInt32 numBytes;

    // This implementation only supports 512 byte blocks!
    if (aBlockSize != 512)
        return UEZ_ERROR_INVALID_PARAMETER;

    // Determine how many blocks we are going to write (based on the
    // number requested to send and how much room we got).
    *aNumWriting = aNumBlocks;
    if (*aNumWriting >= NUM_MAX_BLOCKS)
        *aNumWriting = NUM_MAX_BLOCKS-1;
    //p->iNumWriting = *aNumWriting;

    numBytes = aBlockSize * aNumBlocks;
    LPC_SDMMC->BYTCNT = numBytes;

    // Set the callback routine (if any)
    p->iTransmissionCompleteCallback = aTransmissionCallback;
    p->iTransmissionCompleteCallbackWorkspace = aTransmissionCallbackWorkspace;

    ISetupDMA(&p->iDMAArea, (TUInt32) aBuffer, aNumBlocks * aBlockSize);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SD_MMC_SetRelativeCardAddress
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the active RCA (Relative Card Address).
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 *      TUInt32 aRCA            -- Relative Card Address (RCA)
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_SD_MMC_SetRelativeCardAddress(void *aWorkspace, TUInt32 aRCA)
{
    T_LPC546xx_SD_MMC_Workspace *p = (T_LPC546xx_SD_MMC_Workspace *)aWorkspace;
    p->iRCA = aRCA;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SD_MMC_SetClockRate
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the clocking rate of the SD_MMC bus clock in Hz.  If power save
 *      mode is set, the clock is only run when communications is active,
 *      otherwise it is run continuously.
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 *      TUInt32 aHz             -- Clock rate in Hz.
 *      TBool aPowerSave        -- ETrue to save power by running the clock
 *                                  only when active, else EFalse to run
 *                                  clock continuously.
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError ISetClockRate(
    void *aWorkspace,
    TUInt32 aHz)
{
    PARAM_NOT_USED(aWorkspace);
    //PARAM_NOT_USED(aPowerSave);

    TUInt32 clk_rate = G_SourceClkFrequency;

    // Power save means the clock ONLY runs when needed, otherwise the SD CLK
    // runs continuously.
    if (aHz == 0) {

    } else {
        /* compute SD/MCI clock dividers */
        uint32_t div;

        div = ((clk_rate / aHz)) >> 1;
        if ((div == LPC_SDMMC->CLKDIV) && LPC_SDMMC->CLKENA) {
            return UEZ_ERROR_NONE; /* Closest speed is already set */

        }
        /* disable clock */
        LPC_SDMMC->CLKENA = 0;

        /* User divider 0 */
        LPC_SDMMC->CLKSRC = MCI_CLKSRC_CLKDIV0;

        /* inform CIU */
        ISendCommand(MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);

        /* set divider 0 to desired value */
        LPC_SDMMC->CLKDIV = MCI_CLOCK_DIVIDER(0, div);

        /* inform CIU */
        ISendCommand(MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);

        /* enable clock */
        LPC_SDMMC->CLKENA = MCI_CLKEN_ENABLE;

        /* inform CIU */
        ISendCommand(MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);
    }

    return UEZ_ERROR_NONE;
}

T_uezError LPC546xx_SD_MMC_SetClockRate(
    void *aWorkspace,
    TUInt32 aHz)
{
    T_LPC546xx_SD_MMC_Workspace *p = (T_LPC546xx_SD_MMC_Workspace *)aWorkspace;

    // Remember speed for later
    p->iSpeed = aHz;

    return UEZ_ERROR_NONE;
}

static void IEventSetup(T_LPC546xx_SD_MMC_Workspace *p, void *bits)
{
    uint32_t bit_mask = *((uint32_t *)bits);

    /* Wait for IRQ - for an RTOS, you would pend on an event here with a IRQ based wakeup. */
    NVIC_ClearPendingIRQ(SDIO_IRQn);
    p->sdio_wait_exit = 1;
    LPC_SDMMC->INTMASK = bit_mask;
    InterruptEnable(SDIO_IRQn);
}

static uint32_t IIRQWait(T_LPC546xx_SD_MMC_Workspace *p)
{
    uint32_t status;

    /* Wait for event, would be nice to have a timeout, but keep it  simple */
    while (p->sdio_wait_exit != 2) {
        // Very fast wait time?
        // Spinning....
    }

    /* Get status and clear interrupts */
    status = LPC_SDMMC->RINTSTS;
    LPC_SDMMC->RINTSTS = status;
    LPC_SDMMC->INTMASK = 0;

    return status;
}

#ifdef DEBUG_CMD
typedef struct {
      uint32_t cmd;
      uint32_t arg;
      uint32_t wait_status;
      uint32_t response[4];
} T_mmcCmd;
T_mmcCmd G_cmds[100];
int G_numCmds = 0;

static T_mmcCmd *SDMMCLog(uint32_t cmd, uint32_t arg, uint32_t wait_status)
{
  if (G_numCmds < 100) {
    T_mmcCmd *p = G_cmds + G_numCmds++;
    p->cmd = cmd;
    p->arg = arg;
    p->wait_status = wait_status;
    p->response[0] = 0;
    p->response[1] = 0;
    p->response[2] = 0;
    p->response[3] = 0;
    return p;
  }
  return 0;
}
#endif

TUInt32 LPC546xx_SD_MMC_ExecuteCommand(
            void *aWorkspace,
            TUInt32 aCommand,
            TUInt32 aArgument,
            TUInt32 aWaitStatus,
            TUInt32 *aResponse)
{
    T_LPC546xx_SD_MMC_Workspace *p = (T_LPC546xx_SD_MMC_Workspace *)aWorkspace;
    int32_t step = (aCommand & CMD_BIT_APP) ? 2 : 1;
    int32_t status = 0;
    uint32_t cmd_reg = 0;

    if (!aWaitStatus) {
        aWaitStatus = (aCommand & CMD_MASK_RESP) ? MCI_INT_CMD_DONE : MCI_INT_DATA_OVER;
    }

    // Zero out the response
    aResponse[0] = 0;
    aResponse[1] = 0;
    aResponse[2] = 0;
    aResponse[3] = 0;
#ifdef DEBUG_CMD
    T_mmcCmd *p_cmd = SDMMCLog(aCommand, aArgument, aWaitStatus);
#endif

    /* Clear the interrupts & FIFOs*/
    if (aCommand & CMD_BIT_DATA)
        ISetClearIntFifo();

    /* also check error conditions */
    aWaitStatus |= MCI_INT_EBE | MCI_INT_SBE | MCI_INT_HLE | MCI_INT_RTO | MCI_INT_RCRC | MCI_INT_RESP_ERR;
    if (aWaitStatus & MCI_INT_DATA_OVER)
        aWaitStatus |= MCI_INT_FRUN | MCI_INT_HTO | MCI_INT_DTO | MCI_INT_DCRC;

    while (step) {
        ISetClockRate(aWorkspace, p->iSpeed);

        /* Clear the interrupts */
        LPC_SDMMC->RINTSTS = 0xFFFFFFFF;

        IEventSetup(p, (void *)&aWaitStatus);

        switch (step) {
            case 1: /* Execute command */
                cmd_reg = ((aCommand & CMD_MASK_CMD) >> CMD_SHIFT_CMD)
                        | ((aCommand & CMD_BIT_INIT) ? MCI_CMD_INIT : 0)
                        | ((aCommand & CMD_BIT_DATA) ? (MCI_CMD_DAT_EXP | MCI_CMD_PRV_DAT_WAIT) : 0)
                        | (((aCommand & CMD_MASK_RESP) == CMD_RESP_R2) ? MCI_CMD_RESP_LONG : 0)
                        | ((aCommand & CMD_MASK_RESP) ? MCI_CMD_RESP_EXP : 0)
                        | ((aCommand & CMD_BIT_WRITE) ? MCI_CMD_DAT_WR : 0)
                        | ((aCommand & CMD_BIT_STREAM) ? MCI_CMD_STRM_MODE : 0)
                        | ((aCommand & CMD_BIT_BUSY) ? MCI_CMD_STOP : 0)
                        | ((aCommand & CMD_BIT_AUTO_STOP) ? MCI_CMD_SEND_STOP : 0)
                        | MCI_CMD_START;

                /* wait for previos data finsh for select/deselect commands */
                if (((aCommand & CMD_MASK_CMD) >> CMD_SHIFT_CMD) == MMC_SELECT_CARD) {
                    cmd_reg |= MCI_CMD_PRV_DAT_WAIT;
                }

                /* wait for command to be accepted by CIU */
                if (ISendCommand(cmd_reg, aArgument) == 0)
                    --step;
                break;

            case 0:
                return 0;

            case 2: /* APP prefix */
                cmd_reg = MMC_APP_CMD | MCI_CMD_RESP_EXP | ((aCommand & CMD_BIT_INIT) ? MCI_CMD_INIT : 0) |
                MCI_CMD_START;

                if (ISendCommand(cmd_reg, (p->iRCA << 16)) == 0)
                    --step;
                break;
        }

        // wait for command response (does not use semaphores?)
        // Only used for fast interrupts
        status = IIRQWait(p);

        /* We return an error if there is a timeout, even if we've fetched  a response */
        if (status & SD_INT_ERROR) {
            return status;
        }

        if (status & MCI_INT_CMD_DONE) {
            switch (aCommand & CMD_MASK_RESP) {
                case 0:
                    break;

                case CMD_RESP_R1:
                case CMD_RESP_R3:
                case CMD_RESP_R2:
                    IGetResponse(aResponse);
                    break;
            }
        }
#ifdef DEBUG_CMD
        p_cmd->response[0] = aResponse[0];
        p_cmd->response[1] = aResponse[1];
        p_cmd->response[2] = aResponse[2];
        p_cmd->response[3] = aResponse[3];
#endif
    }

    return 0;
}

void LPC546xx_SD_MMC_SetCardType(void *aWorkspace, TUInt32 aType)
{
    PARAM_NOT_USED(aWorkspace);
    LPC_SDMMC->CTYPE = aType;
}
void LPC546xx_SD_MMC_SetBlockSize(void *aWorkspace, TUInt32 bytes)
{
    PARAM_NOT_USED(aWorkspace);
    LPC_SDMMC->BLKSIZ = bytes;
}

void LPC546xx_SD_MMC_PrepreExtCSDTransfer(void *aWorkspace, TUInt8 *aAddress, TUInt32 aTransferSize)
{
    T_LPC546xx_SD_MMC_Workspace *p = (T_LPC546xx_SD_MMC_Workspace *)aWorkspace;

    // Prepare the size
    LPC_SDMMC->BLKSIZ = aTransferSize;
    LPC_SDMMC->BYTCNT = aTransferSize;

    // Setup the DMA
    ISetupDMA(&p->iDMAArea, (TUInt32)aAddress, aTransferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC546xx_SD_MMC_ReadyReception
 *---------------------------------------------------------------------------*
 * Description:
 *      Prepare the SD_MMC Controller for receiving blocks of data of the
 *      given size (usually 512 bytes).  When each block is received, call
 *      the given callback routine.
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 *      TUInt32 aNumBlocks      -- Number of blocks to receive
 *      TUInt32 aBlockSize      -- Block size.  Should be multiple of 4 and
 *                                  a power of 2.  Currently only 512 supported.
 *      T_SD_MMCReceptionComplete aReceptionCallback - Callback routine when
 *                                  received block.
 *      void *aReceptionCallbackWorkspace -- Workspace for callback.
 *      TUInt32 aNumBlocksReady -- Number of blocks readied equal or or less
 *                                  than aNumBlocks.  Can be zero.
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC546xx_SD_MMC_ReadyReception(
    void *aWorkspace,
    TUInt8 *aBuffer,
    TUInt32 aNumBlocks,
    TUInt32 aBlockSize,
    T_SD_MMCReceptionComplete aReceptionCallback,
    void *aCallbackWorkspace,
    TUInt32 *aNumBlocksReady)
{
    T_LPC546xx_SD_MMC_Workspace *p = (T_LPC546xx_SD_MMC_Workspace *)aWorkspace;
    TUInt32 byteCount;

    if (aNumBlocks > NUM_MAX_BLOCKS)
        aNumBlocks = NUM_MAX_BLOCKS;
    *aNumBlocksReady = aNumBlocks;

    p->iReceptionCompleteCallback = aReceptionCallback;
    p->iReceptionCompleteCallbackWorkspace = aCallbackWorkspace;

    byteCount = aNumBlocks * aBlockSize;

    // Set the number of bytes to read
    LPC_SDMMC->BYTCNT = byteCount;

    ISetupDMA(&p->iDMAArea, (TUInt32) aBuffer, aNumBlocks * aBlockSize);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const HAL_SD_MMC LPC546xx_SD_MMC_Interface = { {
    "LPC546xx:SD_MMC",
    0x0204,
    LPC546xx_SD_MMC_InitializeWorkspace,
    sizeof(T_LPC546xx_SD_MMC_Workspace), },

// v2.04
    LPC546xx_SD_MMC_PowerOn,
    LPC546xx_SD_MMC_PowerOff,
    LPC546xx_SD_MMC_Reset,
    LPC546xx_SD_MMC_ExecuteCommand,
    LPC546xx_SD_MMC_ReadyReception,
    LPC546xx_SD_MMC_SetRelativeCardAddress,
    LPC546xx_SD_MMC_SetClockRate,
    LPC546xx_SD_MMC_ReadyTransmission,
    LPC546xx_SD_MMC_SetCardType,
    LPC546xx_SD_MMC_SetBlockSize,
    LPC546xx_SD_MMC_PrepreExtCSDTransfer
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC546xx_SD_MMC_Require(const T_LPC546xx_SD_MMC_Pins *aPins)
{
    T_halWorkspace *p_SD_MMC;
    T_LPC546xx_SD_MMC_Workspace *p;

    static const T_LPC546xx_SCU_ConfigList sd_cd[] = {
            {GPIO_P1_6     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_CD    SD/MMC card detect input.
            {GPIO_P6_7     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_CD    SD/MMC card detect input.
    };

    static const T_LPC546xx_SCU_ConfigList sd_cmd[] = {
            {GPIO_P1_9     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_CMD   SD/MMC command signal.
            {GPIO_P6_9     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_CMD   SD/MMC command signal.
    };

    static const T_LPC546xx_SCU_ConfigList sd_dat0[] = {
            {GPIO_P1_2     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_DAT0  SD/MMC data bus line 0.
            {GPIO_P6_3     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_DAT0  SD/MMC data bus line 0.
    };

    static const T_LPC546xx_SCU_ConfigList sd_dat1[] = {
            {GPIO_P1_3     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_DAT1  SD/MMC data bus line 1.
            {GPIO_P6_4     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_DAT1  SD/MMC data bus line 1.
    };

    static const T_LPC546xx_SCU_ConfigList sd_dat2[] = {
            {GPIO_P1_4     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_DAT2  SD/MMC data bus line 2.
            {GPIO_P6_5     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_DAT2  SD/MMC data bus line 2.
    };

    static const T_LPC546xx_SCU_ConfigList sd_dat3[] = {
            {GPIO_P1_5     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_DAT3  SD/MMC data bus line 3.
            {GPIO_P6_6     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_DAT3  SD/MMC data bus line 3.
    };

    static const T_LPC546xx_SCU_ConfigList sd_pow[] = {
            {GPIO_P1_8     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_POW   SD/MMC power monitor output.
            {GPIO_P6_8     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_POW   SD/MMC power monitor output.
            {GPIO_P6_15    , SCU_NORMAL_DRIVE_DEFAULT(5)}, //SD_POW   SD/MMC power monitor output.
    };

    static const T_LPC546xx_SCU_ConfigList sd_wp[] = {
            {GPIO_P6_29    , SCU_NORMAL_DRIVE_DEFAULT(5)}, //SD_WP    SD/MMC card write protect input.
            {GPIO_P7_24    , SCU_NORMAL_DRIVE_DEFAULT(5)}, //SD_WP    SD/MMC card write protect input.
    };

    static const T_LPC546xx_SCU_ConfigList sd_clk[] = {
            {GPIO_PZ_Z_PC_0, SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_FAST, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_CLK   SD/MMC card clock.
    };

    HAL_DEVICE_REQUIRE_ONCE();

    // Register SD_MMC Controller
    HALInterfaceRegister("SD_MMC",
            (T_halInterface *)&LPC546xx_SD_MMC_Interface, 0, &p_SD_MMC);
    p = (T_LPC546xx_SD_MMC_Workspace *)p_SD_MMC;

    // Turn on the SD_MMC Controller
    //clock based on PCLK
    G_SourceClkFrequency = UEZPlatform_GetPCLKFrequency();
    LPC_CGU->BASE_SDIO_CLK = (9<<24) | (1<<11); //Use PLL1 clock
    LPC_CCU1->CLK_M4_SDIO_CFG = 3;

    LPC546xx_SCU_ConfigPinOrNone(aPins->iCardDetect, sd_cd, ARRAY_COUNT(sd_cd));
    LPC546xx_SCU_ConfigPinOrNone(aPins->iCMD, sd_cmd, ARRAY_COUNT(sd_cmd));
    LPC546xx_SCU_ConfigPinOrNone(aPins->iDAT0, sd_dat0, ARRAY_COUNT(sd_dat0));
    LPC546xx_SCU_ConfigPinOrNone(aPins->iDAT1, sd_dat1, ARRAY_COUNT(sd_dat1));
    LPC546xx_SCU_ConfigPinOrNone(aPins->iDAT2, sd_dat2, ARRAY_COUNT(sd_dat2));
    LPC546xx_SCU_ConfigPinOrNone(aPins->iDAT3, sd_dat3, ARRAY_COUNT(sd_dat3));
    LPC546xx_SCU_ConfigPinOrNone(aPins->iPOW, sd_pow, ARRAY_COUNT(sd_pow));
    LPC546xx_SCU_ConfigPinOrNone(aPins->iWriteProtect, sd_wp, ARRAY_COUNT(sd_wp));
    LPC546xx_SCU_ConfigPinOrNone(aPins->iCLK, sd_clk, ARRAY_COUNT(sd_clk));

    // Reset SD_MMC Registers
    LPC546xx_SD_MMC_Reset((void *)p);

    // Setup interrupt for the SD_MMC
    InterruptRegister(SDIO_IRQn, LPC546xx_SD_MMC_Interrupt,
        INTERRUPT_PRIORITY_NORMAL, "SD_MMC");
    InterruptEnable(SDIO_IRQn);
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC546xx_SD_MMC.c
 *-------------------------------------------------------------------------*/
