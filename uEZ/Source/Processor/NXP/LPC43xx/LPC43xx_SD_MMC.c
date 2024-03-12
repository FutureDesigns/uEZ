/*-------------------------------------------------------------------------*
 * File:  LPC43xx_SD_MMC.c
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
#include "LPC43xx_SD_MMC.h"
#include "LPC43xx_GPIO.h"
#include "LPC43xx_UtilityFuncs.h"
#include "CMSIS/LPC43xx.h"
#include <Source/Library/SEGGER/SystemView/SEGGER_SYSVIEW.h>
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef NUM_MAX_BLOCKS                   // Currently 32 seems to be minimum size
    #define NUM_MAX_BLOCKS               128         /* Block transfer FIFO depth (>= 2), 20+ recommended */
#endif

#define DEBUG_PRINTF_FUNCTION(...) 
//#define DEBUG_PRINTF_FUNCTION(...) DEBUG_RTT_WriteString(0, __VA_ARGS__)
//#define DEBUG_PRINTF_FUNCTION(...) DEBUG_SV_PrintfE(__VA_ARGS__)

#if(DEBUG_SD_CRC_COUNT==1)
TUInt32 G_SD_CRC_error;
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
#define MCI_CLKEN_LOW_PWR       (1 << 16)       /*!< Enable clock idle for slot */ // Only for SD or MMC, NOT for SDIO
#define MCI_CLKEN_ENABLE        (1 << 0)        /*!< Enable slot clock */


/** @brief  SDIO chained DMA descriptor
 */
typedef struct {
    volatile uint32_t des0;                     /*!< Control and status */
    volatile uint32_t des1;                     /*!< Buffer size(s) */
    volatile uint32_t des2;                     /*!< Buffer address pointer 1 */
    volatile uint32_t des3;                     /*!< Buffer address pointer 2 */
} LPC43xx_SD_MMC_DMADescriptor;

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
      
#define MCI_RINTSTS_ERROR_BITS_TO_IGNORE  (MCI_INT_CMD_DONE | MCI_INT_ACD | MCI_INT_CD | MCI_INT_SDIO)

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    /* MCI_IRQ_CB_FUNC_T irq_cb; */
        LPC43xx_SD_MMC_DMADescriptor mci_dma_dd[1 + (0x10000 / MCI_DMADES1_MAXTR)];
    /* uint32_t sdio_clk_rate; */
    /* uint32_t sdif_slot_clk_rate; */
    /* int32_t clock_enabled; */
} LPC43xx_SD_MMC_DMAArea;

/*-------------------------------------------------------------------------*
 * Workspace:
 *-------------------------------------------------------------------------*/
typedef struct {
    const HAL_SD_MMC *iHAL;

    T_uezGPIOPortPin iCardDetect;
    T_uezGPIOPortPin iWriteProtect;

    LPC43xx_SD_MMC_DMAArea iDMAArea;

    T_SD_MMCReceptionComplete iReceptionCompleteCallback;
    void *iReceptionCompleteCallbackWorkspace;

    T_SD_MMCTransmissionComplete iTransmissionCompleteCallback;
    void *iTransmissionCompleteCallbackWorkspace;
    
    T_SD_MMCError iErrorCallback;
    void *iErrorCallbackWorkspace;

    TUInt32 iRCA;       // Relative Card Address

    TUInt32 iSpeed;     // Speed of clock rate in Hz

    volatile TUInt8 sdio_wait_exit;
    
    T_LPC43xx_SD_MMC_Pins iSdMmcPins;
} T_LPC43xx_SD_MMC_Workspace;

T_LPC43xx_SD_MMC_Workspace *G_SD_MMCWorkspace;
static TUInt32 G_SourceClkFrequency;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LPC43xx_SD_MMC_ProcessInterrupt(T_LPC43xx_SD_MMC_Workspace *p);

/*---------------------------------------------------------------------------*
 * Interrupt:  LPC43xx_SD_MMC_Interrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      A SD_MMC Interrupt has come in.  Process the one workspace attached
 *      to this interrupt.
 *---------------------------------------------------------------------------*/
IRQ_ROUTINE(LPC43xx_SD_MMC_Interrupt)
{
    IRQ_START();
    LPC43xx_SD_MMC_ProcessInterrupt(G_SD_MMCWorkspace);
    IRQ_END();
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
 * Routine:  LPC43xx_SD_MMC_ProcessInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      Process an interrupt for this workspace
 * Inputs:
 *      void *aWorkspace          -- SD_MMC Workspace
 * Outputs:
 *      void
 *---------------------------------------------------------------------------*/
void LPC43xx_SD_MMC_ProcessInterrupt(T_LPC43xx_SD_MMC_Workspace *p)
{
#ifdef CORE_M4
    InterruptDisable(SDIO_IRQn);
#endif
#ifdef CORE_M0
    InterruptDisable(M0_SDIO_IRQn);
#endif
#ifdef CORE_M0SUB
    // None
#endif

    LPC_SDMMC->RINTSTS = MCI_INT_TXDR; // Don't use currently, clear it
    LPC_SDMMC->RINTSTS = MCI_INT_RXDR; // Don't use currently, clear it
    uint32_t temp = LPC_SDMMC->RINTSTS;
        
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
        //LPC_SDMMC->RINTSTS = MCI_INT_CMD_DONE; // Don't clear the flag so we can check it later
        if (p->sdio_wait_exit != 0) { // only 0 when init workspace, 1 when int cleared or set to 2 here
            // Set this interrupt for SDIO commands
            p->sdio_wait_exit = 2;
        }
    }
    
    temp &=  ~(MCI_RINTSTS_ERROR_BITS_TO_IGNORE);
      
    if(temp > 0x0) {
      // errors
      if (LPC_SDMMC->RINTSTS & MCI_INT_RESP_ERR) { // Command response error
          DEBUG_PRINTF_FUNCTION("RSE");
      }
      
      // 6-8 indicate that the received data may have errors. If there was a response time-out, then no data transfer occurred.
      if (LPC_SDMMC->RINTSTS & MCI_INT_RCRC) { // Response CRC error
          DEBUG_PRINTF_FUNCTION("RCR");
      }
      
  /* From ES_LPC435x/3x/2x/1x Rev 6.8:    Problem:
  * The CMD6 returned status block always gets a data CRC error although the status data is
  * correct. The data CRC error prevents the switching of the SD memory card from the
  * default mode to high-speed mode.

  * Work-around:
  * Capture the 64 bits of CRC data that follow the 512 bits of data allowing the CRC data to
  * be calculated in software. The DMA buffer length and SD/MMC BYTCNT must be set to
  * 72 (versus 64). The CRC data consists of four interleaved 16-bit words, one for each of
  * the four serialized SD data bits. If all four of the calculated CRCs match the captured
  * CRCs, the software can clear the data CRC error flag bit.*/

      if (LPC_SDMMC->RINTSTS & MCI_INT_DCRC) { // Data CRC error - CRC error occurred during data reception.
          LPC_SDMMC->RINTSTS = MCI_INT_DCRC; // clear error
          DEBUG_PRINTF_FUNCTION("DCR"); // We get this with wrong pin settings or too high frequency and it is a real CRC error and we fail file comparison read back.
          #if(DEBUG_SD_CRC_COUNT==1)
          G_SD_CRC_error++;
          #endif
      }
      
      if (LPC_SDMMC->RINTSTS & MCI_INT_RTO) { // Response timeout error (RTO_BAR)
          DEBUG_PRINTF_FUNCTION("RTO");
      }
      
      if (LPC_SDMMC->RINTSTS & MCI_INT_DTO) { // Data read timeout (DRTO_BDS) - Card has not sent data within the time-out period.
          DEBUG_PRINTF_FUNCTION("DTO");
      }
      
      if (LPC_SDMMC->RINTSTS & MCI_INT_HTO) { // Host data starvation error
          DEBUG_PRINTF_FUNCTION("HDS");
      }
      
      if (LPC_SDMMC->RINTSTS & MCI_INT_FRUN) { //  FIFO overrun/underrun error
          DEBUG_PRINTF_FUNCTION("FOV");
      }
      
      if (LPC_SDMMC->RINTSTS & MCI_INT_HLE) { //  Hardware locked write error
          DEBUG_PRINTF_FUNCTION("HLE");
      }
      
      if (LPC_SDMMC->RINTSTS & MCI_INT_SBE) { // Start bit error - Start bit was not received during data reception.
          DEBUG_PRINTF_FUNCTION("SBE");
      }
      
      if (LPC_SDMMC->RINTSTS & MCI_INT_EBE) { // End-bit error - End bit was not received during data reception or for a write operation; a CRC error is indicated by the card.
          DEBUG_PRINTF_FUNCTION("EBE");
      }
      
      if (LPC_SDMMC->IDSTS & (1 << 10)) { // 
          DEBUG_PRINTF_FUNCTION("HOT");
      }
      if (LPC_SDMMC->IDSTS & (1 << 11)) { // 
          DEBUG_PRINTF_FUNCTION("HOR");
      }
      // Note that non-errors aren't listed here, such as RTO/DTO/ACD, etc.
      
      if (temp & MCI_INT_DATA_OVER) { 
          // This possibly indicates command finished as the bit is never set on a real timeout
      } else {      
        if (p->iErrorCallback) {
            p->iErrorCallback(p->iErrorCallbackWorkspace);
            // can't clear FIFO here using normal clear command, not sure yet if that helps
            //p->iErrorCallback = 0;
        }
      }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SD_MMC_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the workspace for this Controller
 * Inputs:
 *      void *aWorkspace          -- Memory allocated for workspace area.
 * Outputs:
 *      T_uezError                 -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_SD_MMC_InitializeWorkspace(void *aWorkspace)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;
    p->iHAL = &LPC43xx_SD_MMC_Interface;

    p->iCardDetect = GPIO_NONE;
    p->iWriteProtect = GPIO_NONE;
    p->iReceptionCompleteCallback = 0;
    p->iReceptionCompleteCallbackWorkspace = 0;
    p->iTransmissionCompleteCallback = 0;
    p->iTransmissionCompleteCallbackWorkspace = 0;
    p->iErrorCallback = 0;
    p->iErrorCallbackWorkspace = 0;
    p->iRCA = 0;
    p->sdio_wait_exit = 0;
    //p->iSpeed = UEZPlatform_MCI_DefaultFreq();

    G_SD_MMCWorkspace = p;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SD_MMC_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Reset the SD_MMC Controller
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_SD_MMC_Reset(void *aWorkspace)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;
    
    p->iCardDetect = GPIO_NONE;
    p->iWriteProtect = GPIO_NONE;
    p->iReceptionCompleteCallback = 0;
    p->iTransmissionCompleteCallback = 0;
    p->iRCA = 0;
    p->sdio_wait_exit = 0;

    /* Software reset */
    LPC_SDMMC->BMOD = MCI_BMOD_SWR;

    /* reset all blocks */
    LPC_SDMMC->CTRL = MCI_CTRL_RESET | MCI_CTRL_FIFO_RESET | MCI_CTRL_DMA_RESET;
    while (LPC_SDMMC->CTRL & (MCI_CTRL_RESET | MCI_CTRL_FIFO_RESET | MCI_CTRL_DMA_RESET))
        {}

    LPC_SDMMC->CTYPE = 0; // 1-bit mode to start
    LPC_SDMMC->DBADDR = 0; // 
    LPC_SDMMC->CLKDIV = 0; // 
    LPC_SDMMC->BYTCNT = 0x200; //
    LPC_SDMMC->CMDARG = 0; //
    LPC_SDMMC->CMD = 0; //

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

    #if(DEBUG_SD_CRC_COUNT==1)
    G_SD_CRC_error = 0;
    #endif

    // Enable interrupts
#ifdef CORE_M4
    InterruptEnable(SDIO_IRQn);
#endif
#ifdef CORE_M0
    InterruptEnable(M0_SDIO_IRQn);
#endif
#ifdef CORE_M0SUB
    //None
#endif
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SD_MMC_PowerOn
 *---------------------------------------------------------------------------*
 * Description:
 *      Have the SD_MMC power on the target
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_SD_MMC_PowerOn(void *aWorkspace)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;

#if 0 // not implemented features so don't include the unused consts.
    static const T_LPC43xx_SCU_ConfigList sd_pow[] = {
            {GPIO_P1_8     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_POW   SD/MMC power monitor output.
            {GPIO_P6_8     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_POW   SD/MMC power monitor output.
            {GPIO_P6_15    , SCU_NORMAL_DRIVE_DEFAULT(5)}, //SD_POW   SD/MMC power monitor output.
    };

    static const T_LPC43xx_SCU_ConfigList sd_cd[] = {
            {GPIO_P1_6     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_CD    SD/MMC card detect input.
            {GPIO_P6_7     , SCU_NORMAL_DRIVE_DEFAULT(7)}, //SD_CD    SD/MMC card detect input.
    };

    static const T_LPC43xx_SCU_ConfigList sd_wp[] = {
            {GPIO_P6_29    , SCU_NORMAL_DRIVE_DEFAULT(5)}, //SD_WP    SD/MMC card write protect input.
            {GPIO_P7_24    , SCU_NORMAL_DRIVE_DEFAULT(5)}, //SD_WP    SD/MMC card write protect input.
    };
#endif

    // Recommend settings for EHS=0 for CMD/DAT and EHS=1 for clock from LPC435x/3x/2x/1x 11.20 SD/MMC.
    static const T_LPC43xx_SCU_ConfigList sd_clk[] = {
            {GPIO_PZ_Z_PC_0, SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_FAST, SCU_EZI_DISABLE,SCU_ZIF_DISABLE)}, //SD_CLK   SD/MMC card clock.
    };

    static const T_LPC43xx_SCU_ConfigList sd_cmd[] = {
            {GPIO_P1_9     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_CMD   SD/MMC command signal.
            {GPIO_P6_9     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_CMD   SD/MMC command signal.
    };

    static const T_LPC43xx_SCU_ConfigList sd_dat0[] = {
            {GPIO_P1_2     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_DAT0  SD/MMC data bus line 0.
            {GPIO_P6_3     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_DAT0  SD/MMC data bus line 0.
    };

    static const T_LPC43xx_SCU_ConfigList sd_dat1[] = {
            {GPIO_P1_3     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_DAT1  SD/MMC data bus line 1.
            {GPIO_P6_4     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_DAT1  SD/MMC data bus line 1.
    };

    static const T_LPC43xx_SCU_ConfigList sd_dat2[] = {
            {GPIO_P1_4     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_DAT2  SD/MMC data bus line 2.
            {GPIO_P6_5     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_DAT2  SD/MMC data bus line 2.
    };

    static const T_LPC43xx_SCU_ConfigList sd_dat3[] = {
            {GPIO_P1_5     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_DAT3  SD/MMC data bus line 3.
            {GPIO_P6_6     , SCU_NORMAL_DRIVE(7, SCU_EPD_DISABLE, SCU_EPUN_DISABLE,SCU_EHS_SLOW, SCU_EZI_ENABLE, SCU_ZIF_DISABLE)}, //SD_DAT3  SD/MMC data bus line 3.
    };
    
    // Disable interrupts
#ifdef CORE_M4
    InterruptDisable(SDIO_IRQn);
#endif
#ifdef CORE_M0
    InterruptDisable(M0_SDIO_IRQn);
#endif
#ifdef CORE_M0SUB
    //None
#endif

    UEZGPIOUnlock(p->iSdMmcPins.iCMD);
    UEZGPIOUnlock(p->iSdMmcPins.iDAT0);
    UEZGPIOUnlock(p->iSdMmcPins.iDAT1);
    UEZGPIOUnlock(p->iSdMmcPins.iDAT2);
    UEZGPIOUnlock(p->iSdMmcPins.iDAT3);
    UEZGPIOUnlock(p->iSdMmcPins.iCLK);
    UEZGPIOUnlock(p->iSdMmcPins.iPOW);
    //UEZGPIOUnlock(p->iSdMmcPins.iCardDetect);
    //UEZGPIOUnlock(p->iSdMmcPins.iWriteProtect);

    // Power is enabled before we switch the pins to peripheral mode.
    // Manually turn SDcard power on (if not already on)
    UEZGPIOSet(p->iSdMmcPins.iPOW);
    //LPC_SDMMC->PWREN = 0x01; // not using function pin, using GPIO set below.

    // SD spec says that ramp to 0.5V is up to 250ms, then 35ms ramp to 3.3V.
    UEZTaskDelay(50); // MMC spec said to wait 35ms after power on before CMD
    // We ramp on these uEZGUIs almost instantly, less than 20uS on this switch.

    LPC43xx_SCU_ConfigPinOrNone(p->iSdMmcPins.iCMD, sd_cmd, ARRAY_COUNT(sd_cmd));
    LPC43xx_SCU_ConfigPinOrNone(p->iSdMmcPins.iDAT0, sd_dat0, ARRAY_COUNT(sd_dat0));
    LPC43xx_SCU_ConfigPinOrNone(p->iSdMmcPins.iDAT1, sd_dat1, ARRAY_COUNT(sd_dat1));
    LPC43xx_SCU_ConfigPinOrNone(p->iSdMmcPins.iDAT2, sd_dat2, ARRAY_COUNT(sd_dat2));
    LPC43xx_SCU_ConfigPinOrNone(p->iSdMmcPins.iDAT3, sd_dat3, ARRAY_COUNT(sd_dat3));
    LPC43xx_SCU_ConfigPinOrNone(p->iSdMmcPins.iCLK, sd_clk, ARRAY_COUNT(sd_clk));
    //LPC43xx_SCU_ConfigPinOrNone(p->iSdMmcPins.iPOW, sd_pow, ARRAY_COUNT(sd_pow));
    //LPC43xx_SCU_ConfigPinOrNone(p->iSdMmcPins.iCardDetect, sd_cd, ARRAY_COUNT(sd_cd));
    //LPC43xx_SCU_ConfigPinOrNone(p->iSdMmcPins.iWriteProtect, sd_wp, ARRAY_COUNT(sd_wp));

    LPC43xx_SD_MMC_Reset((void *)p);
    
    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SD_MMC_PowerOff
 *---------------------------------------------------------------------------*
 * Description:
 *      Have the SD_MMC power off the target
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_SD_MMC_PowerOff(void *aWorkspace)
{
    LPC_SDMMC->INTMASK = 0;
    LPC_SDMMC->CTRL   &= ~(MCI_CTRL_INT_ENABLE); // disables interrupts
    //LPC_SDMMC->CTRL = 1; // would reset DMA if uncommented
    LPC_SDMMC->RINTSTS = 0xFFFFFFFF;

    LPC_SDMMC->PWREN = 0x00;
    LPC_SDMMC->CLKENA = 0;

    // Disable interrupts
#ifdef CORE_M4
    InterruptDisable(SDIO_IRQn);
#endif
#ifdef CORE_M0
    InterruptDisable(M0_SDIO_IRQn);
#endif
#ifdef CORE_M0SUB
    //None
#endif
    
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;
    TUInt32 value;
    //dprintf("Reboot SD card\n");
    // Set GPIOs low and hold power off to allow for proper power down then power up sequence.

    UEZGPIOUnlock(p->iSdMmcPins.iCMD);
    UEZGPIOUnlock(p->iSdMmcPins.iDAT0);
    UEZGPIOUnlock(p->iSdMmcPins.iDAT1);
    UEZGPIOUnlock(p->iSdMmcPins.iDAT2);
    UEZGPIOUnlock(p->iSdMmcPins.iDAT3);
    UEZGPIOUnlock(p->iSdMmcPins.iCLK);
    UEZGPIOUnlock(p->iSdMmcPins.iPOW);
    UEZGPIOUnlock(p->iSdMmcPins.iCardDetect);
    //UEZGPIOUnlock(p->iSdMmcPins.iWriteProtect);
    
    value = ((p->iSdMmcPins.iCardDetect >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOSetMux(p->iSdMmcPins.iCardDetect, (p->iSdMmcPins.iCardDetect >> 8) >= 5 ? 4 : 0);
    UEZGPIOControl(p->iSdMmcPins.iCardDetect, GPIO_CONTROL_SET_CONFIG_BITS, value);
    UEZGPIOInput(p->iSdMmcPins.iCardDetect);
    UEZGPIOControl(p->iSdMmcPins.iCardDetect, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0);

    // Make sure that DAT0 does not draw power from MCU by setting low.
    UEZGPIOOutput(p->iSdMmcPins.iDAT0); 
    UEZGPIOSetMux(p->iSdMmcPins.iDAT0, (p->iSdMmcPins.iDAT0 >> 8) >= 5 ? 4 : 0);
    value = ((p->iSdMmcPins.iDAT0 >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(p->iSdMmcPins.iDAT0, GPIO_CONTROL_SET_CONFIG_BITS, value);    
    UEZGPIOClear(p->iSdMmcPins.iDAT0);
    // Make sure that DAT1 does not draw power from MCU by setting low.
    UEZGPIOOutput(p->iSdMmcPins.iDAT1); 
    UEZGPIOSetMux(p->iSdMmcPins.iDAT1, (p->iSdMmcPins.iDAT1 >> 8) >= 5 ? 4 : 0);
    value = ((p->iSdMmcPins.iDAT1 >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(p->iSdMmcPins.iDAT1, GPIO_CONTROL_SET_CONFIG_BITS, value);    
    UEZGPIOClear(p->iSdMmcPins.iDAT1);
    // Make sure that DAT2 does not draw power from MCU by setting low.
    UEZGPIOOutput(p->iSdMmcPins.iDAT2); 
    UEZGPIOSetMux(p->iSdMmcPins.iDAT2, (p->iSdMmcPins.iDAT2 >> 8) >= 5 ? 4 : 0);
    value = ((p->iSdMmcPins.iDAT2 >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(p->iSdMmcPins.iDAT2, GPIO_CONTROL_SET_CONFIG_BITS, value);    
    UEZGPIOClear(p->iSdMmcPins.iDAT2);
    // Make sure that DAT3 does not draw power from MCU by setting low.
    UEZGPIOOutput(p->iSdMmcPins.iDAT3); 
    UEZGPIOSetMux(p->iSdMmcPins.iDAT3, (p->iSdMmcPins.iDAT3 >> 8) >= 5 ? 4 : 0);
    value = ((p->iSdMmcPins.iDAT3 >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(p->iSdMmcPins.iDAT3, GPIO_CONTROL_SET_CONFIG_BITS, value);    
    UEZGPIOClear(p->iSdMmcPins.iDAT3);
    // Make sure that CMD does not draw power from MCU by setting low.
    UEZGPIOOutput(p->iSdMmcPins.iCMD); 
    UEZGPIOSetMux(p->iSdMmcPins.iCMD, (p->iSdMmcPins.iCMD >> 8) >= 5 ? 4 : 0);
    value = ((p->iSdMmcPins.iCMD >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(p->iSdMmcPins.iCMD, GPIO_CONTROL_SET_CONFIG_BITS, value);    
    UEZGPIOClear(p->iSdMmcPins.iCMD);
    // Make sure that CLK does not draw power from MCU by setting low.
    // PC_0 does not have GPIO functionality. It has USB_ULPI_CLK, ENET_RX_CLK, LCD_CLK, SD_CLK, and ADC1_1
    // This Pin DOES contribute to current draw even though no GPIO as pull-up resistor is still enabled at reset.
    value = (1 << 3) | (1 << 4); // enable pull-down, disable pull-up, leave at reservered function
    UEZGPIOControl(p->iSdMmcPins.iCLK, GPIO_CONTROL_SET_CONFIG_BITS, value); // verified that we later correctly disable both pull in require.

    // First hold SD card power low for 200 ms to allow for full power down.
    UEZGPIOOutput(p->iSdMmcPins.iPOW); 
    UEZGPIOSetMux(p->iSdMmcPins.iPOW, (p->iSdMmcPins.iPOW >> 8) >= 5 ? 4 : 0);
    value = ((p->iSdMmcPins.iPOW >> 8) & 0x7) >= 5 ? 4 : 0;
    UEZGPIOControl(p->iSdMmcPins.iPOW, GPIO_CONTROL_SET_CONFIG_BITS, value);    
    UEZGPIOClear(p->iSdMmcPins.iPOW);
    UEZTaskDelay(200); // Power off delay, currently much longer than spec requires

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SD_MMC_IsCardInserted
 *---------------------------------------------------------------------------*
 * Description:
 *      Check Card detect line of socket
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 * Outputs:
 *      TBool              -- ETrue if card present
 *---------------------------------------------------------------------------*/
TBool LPC43xx_SD_MMC_IsCardInserted(void * aWorkspace)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;    
    PARAM_NOT_USED(p);
    if(p->iSdMmcPins.iCardDetect != GPIO_NONE) {
      TBool read = UEZGPIORead(p->iSdMmcPins.iCardDetect);
      UEZBSPDelayUS(20);
      if (read == UEZGPIORead(p->iSdMmcPins.iCardDetect)){
        if(read == ETrue) {
          return ETrue; // card present both reads
        }
      }
      // If we read 2 different values or card disconnected then card was removed at least momentarily.
      return EFalse;
    } else {
      return ETrue;
    }
}

static int32_t ISendCommand(uint32_t aCommand, uint32_t aArgument)
{
    volatile int32_t timeout = 50;
    volatile int32_t delay; // very fast delays

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
void ISetupDMA(LPC43xx_SD_MMC_DMAArea *aDMAArea, TUInt32 addr, TUInt32 size)
{
    int32_t i = 0;
    TUInt32 ctrl, maxs;

    /* // Enable this code to check for unaligned Read/Write buffer pointers
       // so that you can remove them from the application.
    if (addr & 0x03) {
       UEZFailureMsg("LPC43xx_SD_MMC.c:ISetupDMA requires 32-bit aligned address!");
    }*/
    
    // Reset DMA
    LPC_SDMMC->CTRL |= MCI_CTRL_DMA_RESET | MCI_CTRL_FIFO_RESET;
    while (LPC_SDMMC->CTRL & MCI_CTRL_DMA_RESET) {
    }

    // Build a descriptor list using the chained DMA method
    while (size > 0) {
        // Limit size of the transfer to maximum buffer size
        maxs = size;
        if (maxs > MCI_DMADES1_MAXTR) {
            maxs = MCI_DMADES1_MAXTR; // cannot lower by 1
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

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SD_MMC_ReadyTransmission
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
T_uezError LPC43xx_SD_MMC_ReadyTransmission(
    void *aWorkspace,
    const TUInt8 *aBuffer,
    TUInt32 aNumBlocks,
    TUInt32 aBlockSize,
    TUInt32 *aNumWriting,
    T_SD_MMCTransmissionComplete aTransmissionCallback,
    void *aTransmissionCallbackWorkspace)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;
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
 * Routine:  LPC43xx_SD_MMC_SetRelativeCardAddress
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the active RCA (Relative Card Address).
 * Inputs:
 *      void *aWorkspace        -- SD_MMC Workspace
 *      TUInt32 aRCA            -- Relative Card Address (RCA)
 * Outputs:
 *      T_uezError              -- Error code
 *---------------------------------------------------------------------------*/
T_uezError LPC43xx_SD_MMC_SetRelativeCardAddress(void *aWorkspace, TUInt32 aRCA)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;
    p->iRCA = aRCA;

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SD_MMC_SetClockRate
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
        uint32_t div_rem;

        div_rem = ((clk_rate % aHz)); // This doesn't handle less than 400KHz since that is MAX divider!
        div = ((clk_rate / aHz)) >> 1;
        if(div_rem != 0) {
          div = div +1;
        }
        if ((div == LPC_SDMMC->CLKDIV) && (LPC_SDMMC->CLKENA & SDMMC_CLKENA_CCLK_ENABLE_Msk)) {
            return UEZ_ERROR_NONE; /* Closest speed is already set */

        }
        // disable clock - clock must be disabled for minimum time before changing it.
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
        if(aHz <=400000UL) {
          LPC_SDMMC->CLKENA = MCI_CLKEN_ENABLE; // run clock constantly
        } else {
          LPC_SDMMC->CLKENA = MCI_CLKEN_ENABLE | MCI_CLKEN_LOW_PWR; // only run clock when not idling.
        }

        /* inform CIU */
        ISendCommand(MCI_CMD_UPD_CLK | MCI_CMD_PRV_DAT_WAIT, 0);
    }

    return UEZ_ERROR_NONE;
}

T_uezError LPC43xx_SD_MMC_SetClockRate(
    void *aWorkspace,
    TUInt32 aHz)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;

    // Remember speed for later
    p->iSpeed = aHz;

    if(aHz <=400000UL) { // force clock to come on even if we haven't sent a command yet.
      ISetClockRate(aWorkspace, p->iSpeed);
    } else { // otherwise set clock back into low power mode (will take affect before first real command is sent).      
    }

    return UEZ_ERROR_NONE;
}

static void IEventSetup(T_LPC43xx_SD_MMC_Workspace *p, void *bits)
{
    uint32_t bit_mask = *((uint32_t *)bits);

    /* Wait for IRQ - for an RTOS, you would pend on an event here with a IRQ based wakeup. */

#ifdef CORE_M4
    NVIC_ClearPendingIRQ(SDIO_IRQn);
    p->sdio_wait_exit = 1;
    LPC_SDMMC->INTMASK = bit_mask;
    InterruptEnable(SDIO_IRQn);
#endif
#ifdef CORE_M0
    NVIC_ClearPendingIRQ(M0_SDIO_IRQn);
    p->sdio_wait_exit = 1;
    LPC_SDMMC->INTMASK = bit_mask;
    InterruptEnable(M0_SDIO_IRQn);
#endif
#ifdef CORE_M0SUB
    //NVIC_ClearPendingIRQ(SDIO_IRQn);
    p->sdio_wait_exit = 1;
    LPC_SDMMC->INTMASK = bit_mask;
    //InterruptEnable(SDIO_IRQn);
#endif
}

static uint32_t IIRQWait(T_LPC43xx_SD_MMC_Workspace *p)
{
    uint32_t status;
    TUInt32 start;

    start = UEZTickCounterGet();

    /* Wait for event, with timeout*/
    while (p->sdio_wait_exit != 2) { // we only call from one place and interrupt can't reset this number.
        // So if we already hit interrupt before reaching this function, we will just continue to status return below without going into loop. (this is what we want)
        // Spinning.... Very fast wait time in microsecond range
        if (UEZTickCounterGetDelta(start) >= 500) { // hit > 25 on samsung cards in video player. Our timing isn't very accurate and from SD spec 
            break; // for CMD ACK response (say that we are "going to" do multiple block read) it is 64 cycles or 1.28us at 51MHz or 163.2us at 400Khz.
            // If we had a separate accurate timer that doesn't get stopped by RTOS we could check 1ms timeout for initial command response.
            // Per SD spec 3.01 table 4-51 SDHC/SDXC is max read access time of 100ms (for some large multiple block read).
            // So we will always set above that here to account for our innacuracies. If we timeout on large delay we need to mark SD card non-inserted and try to re-init, as it should't be this long.
        }
        UEZTaskDelay(0); // force reschedule to allow quick background tasks to run, but don't delay.
        // Any non-zero delay here would slowdown something like video player noticeably.
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
int32_t G_numCmds = 0;

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

TUInt32 LPC43xx_SD_MMC_ExecuteCommand(
            void *aWorkspace,
            TUInt32 aCommand,
            TUInt32 aArgument,
            TUInt32 aWaitStatus,
            TUInt32 *aResponse)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;
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
        status = IIRQWait(p); // TODO wait earlier for interrupt as we might get it earlier

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

void LPC43xx_SD_MMC_SetCardType(void *aWorkspace, TUInt32 aType)
{
    PARAM_NOT_USED(aWorkspace);
    LPC_SDMMC->CTYPE = aType;
}
void LPC43xx_SD_MMC_SetBlockSize(void *aWorkspace, TUInt32 bytes)
{
    PARAM_NOT_USED(aWorkspace);
    LPC_SDMMC->BLKSIZ = bytes;
}

void LPC43xx_SD_MMC_PrepreExtCSDTransfer(void *aWorkspace, TUInt8 *aAddress, TUInt32 aTransferSize)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;

    // Prepare the size
    LPC_SDMMC->BLKSIZ = aTransferSize;
    LPC_SDMMC->BYTCNT = aTransferSize;

    // Setup the DMA
    ISetupDMA(&p->iDMAArea, (TUInt32)aAddress, aTransferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  LPC43xx_SD_MMC_ReadyReception
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
T_uezError LPC43xx_SD_MMC_ReadyReception(
    void *aWorkspace,
    TUInt8 *aBuffer,
    TUInt32 aNumBlocks,
    TUInt32 aBlockSize,
    T_SD_MMCReceptionComplete aReceptionCallback,
    void *aCallbackWorkspace,
    TUInt32 *aNumBlocksReady)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;
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

void LPC43xx_SD_MMC_SetupErrorCallback (void *aWorkspace, T_SD_MMCError aErrorCallback,
     void *aErrorCallbackWorkspace)
{
    T_LPC43xx_SD_MMC_Workspace *p = (T_LPC43xx_SD_MMC_Workspace *)aWorkspace;  
    p->iErrorCallback = aErrorCallback;
    p->iErrorCallbackWorkspace = aErrorCallbackWorkspace;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const HAL_SD_MMC LPC43xx_SD_MMC_Interface = { {
    "LPC43xx:SD_MMC",
    0x0204,
    LPC43xx_SD_MMC_InitializeWorkspace,
    sizeof(T_LPC43xx_SD_MMC_Workspace), },

    // v2.04
    LPC43xx_SD_MMC_PowerOn,
    LPC43xx_SD_MMC_PowerOff,
    LPC43xx_SD_MMC_Reset,
    LPC43xx_SD_MMC_ExecuteCommand,
    LPC43xx_SD_MMC_ReadyReception,
    LPC43xx_SD_MMC_SetRelativeCardAddress,
    LPC43xx_SD_MMC_SetClockRate,
    LPC43xx_SD_MMC_ReadyTransmission,
    LPC43xx_SD_MMC_SetCardType,
    LPC43xx_SD_MMC_SetBlockSize,
    LPC43xx_SD_MMC_PrepreExtCSDTransfer,
    LPC43xx_SD_MMC_SetupErrorCallback,
    // uEZ 2.14
    LPC43xx_SD_MMC_IsCardInserted
};

/*---------------------------------------------------------------------------*
 * Requirement routines:
 *---------------------------------------------------------------------------*/
void LPC43xx_SD_MMC_Require(const T_LPC43xx_SD_MMC_Pins *aPins)
{
    T_halWorkspace *p_SD_MMC;
    T_LPC43xx_SD_MMC_Workspace *p;

    HAL_DEVICE_REQUIRE_ONCE();

    // Register SD_MMC Controller
    HALInterfaceRegister("SD_MMC",
            (T_halInterface *)&LPC43xx_SD_MMC_Interface, 0, &p_SD_MMC);
    p = (T_LPC43xx_SD_MMC_Workspace *)p_SD_MMC;

    // set the pins from the platform assignment.
    memcpy((void *)&p->iSdMmcPins, (void *)aPins, sizeof(p->iSdMmcPins)); 
    // Currently card detect and write protect aren't implemented.
    // Currently power control is just GPIO and could be probably.
    // Currently we only support the non-GPIO CLK pin.
    LPC43xx_SD_MMC_PowerOff(p); // set pins to GPIO mode and hold power off

    // Turn on the SD_MMC Controller
    G_SourceClkFrequency = UEZPlatform_GetPCLKFrequency(); //clock based on PCLK
    LPC_CGU->BASE_SDIO_CLK = (9<<24) | (1<<11); //Use PLL1 clock
    LPC_CCU1->CLK_M4_SDIO_CFG = 3;

    /* 17.4.10 SD/MMC delay register
     * This register provides a programmable delay for the SD/MMC sample and drive inputs
     * and outputs. See the LPC43xx/LPC43Sxx data sheets for recommended settings.
     * Typical setting for SD cards are SAMPLE_DELAY = 0x8 and DRV_DELAY = 0xF.
     * Remark: The values DRV_DELAY = 0 and DRV_DELAY = 1 are not allowed*/
    
    // The numbers from both the datasheet and user's manual for SDDElay are too large and lead to immediate failure of SD card at all tested frequencies.
    // Values of 0 not only aren't allowed but cause issues, so we need small numbers for each setting.
    // Note that other newer LPC MCU families with this same register appear to have a different preferred number even for the same frequencies. (different range)

    // Drive delay minimum of 2 is required.    
    LPC_SCU->SDDELAY = (2 << SCU_SDDELAY_SAMPLE_DELAY_Pos) | (3 << SCU_SDDELAY_DRV_DELAY_Pos); // This passes test with 34MHz read, 48MHz read/write, 51MHz read/write, and 25.5MHz read/write.


    // Setup interrupt for the SD_MMC, but don't enable it yet until we init everything.
#ifdef CORE_M4
    InterruptRegister(SDIO_IRQn, LPC43xx_SD_MMC_Interrupt,
        INTERRUPT_PRIORITY_HIGH, "SD_MMC");
#endif
#ifdef CORE_M0
    InterruptRegister(M0_SDIO_IRQn, LPC43xx_SD_MMC_Interrupt,
        INTERRUPT_PRIORITY_HIGH, "SD_MMC");
#endif
#ifdef CORE_M0SUB
    //None
#endif
    // Registers are reset and power is enabled on each init of SD card.
}

/*-------------------------------------------------------------------------*
 * End of File:  LPC43xx_SD_MMC.c
 *-------------------------------------------------------------------------*/
