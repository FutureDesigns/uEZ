/*-------------------------------------------------------------------------*
 * File:  SD_MMC.h
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/
#ifndef _TYPES_SD_MMC_H_
#define _TYPES_SD_MMC_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define DEBUG_SD_CRC_COUNT         0 // for 4357 SD card testing

/* ----- MMC/SDC command ----- */
//#define SD_MMC_CMD0    (0)             /* GO_IDLE_STATE */
//#define SD_MMC_CMD1    (1)             /* SEND_OP_COND (MMC) */
//#define SD_MMC_CMD2    (2)             /* ALL_SEND_CID */
//#define SD_MMC_CMD3    (3)             /* SEND_RELATIVE_ADDR */
//#define SD_MMC_ACMD6   (6|0x80)        /* SET_BUS_WIDTH (SDC) */
//#define SD_MMC_CMD7    (7)             /* SELECT_CARD */
//#define SD_MMC_CMD8    (8)             /* SEND_IF_COND */
//#define SD_MMC_CMD9    (9)             /* SEND_CSD */
//#define SD_MMC_CMD10   (10)            /* SEND_CID */
//#define SD_MMC_CMD12   (12)            /* STOP_TRANSMISSION */
//#define SD_MMC_CMD13   (13)            /* SEND_STATUS */
//#define SD_MMC_ACMD13  (13|0x80)       /* SD_STATUS (SDC) */
//#define SD_MMC_CMD16   (16)            /* SET_BLOCKLEN */
//#define SD_MMC_CMD17   (17)            /* READ_SINGLE_BLOCK */
//#define SD_MMC_CMD18   (18)            /* READ_MULTIPLE_BLOCK */
//#define SD_MMC_CMD23   (23)            /* SET_BLK_COUNT (MMC) */
//#define SD_MMC_ACMD23  (23|0x80)       /* SET_WR_BLK_ERASE_COUNT (SDC) */
//#define SD_MMC_CMD24   (24)            /* WRITE_BLOCK */
//#define SD_MMC_CMD25   (25)            /* WRITE_MULTIPLE_BLOCK */
//#define SD_MMC_CMD32   (32)            /* ERASE_ER_BLK_START */
//#define SD_MMC_CMD33   (33)            /* ERASE_ER_BLK_END */
//#define SD_MMC_CMD38   (38)            /* ERASE */
//#define SD_MMC_ACMD41  (41|0x80)       /* SEND_OP_COND (SDC) */
//#define SD_MMC_CMD55   (55)            /* APP_SD_MMC_CMD */

/** @brief SDIO card-type register defines
 */
#define MCI_CTYPE_8BIT          (1 << 16)       /*!< Enable 4-bit mode */
#define MCI_CTYPE_4BIT          (1 << 0)        /*!< Enable 8-bit mode */

/**
 * @brief SD/MMC commands, arguments and responses
 * Standard SD/MMC commands (3.1)       type  argument     response
 */
/* class 1 */
#define MMC_GO_IDLE_STATE         0     /* bc                          */
#define MMC_SEND_OP_COND          1     /* bcr  [31:0]  OCR        R3  */
#define MMC_ALL_SEND_CID          2     /* bcr                     R2  */
#define MMC_SET_RELATIVE_ADDR     3     /* ac   [31:16] RCA        R1  */
#define MMC_SET_DSR               4     /* bc   [31:16] RCA            */
#define MMC_SELECT_CARD           7     /* ac   [31:16] RCA        R1  */
#define MMC_SEND_EXT_CSD          8     /* bc                      R1  */
#define MMC_SEND_CSD              9     /* ac   [31:16] RCA        R2  */
#define MMC_SEND_CID             10     /* ac   [31:16] RCA        R2  */
#define MMC_STOP_TRANSMISSION    12     /* ac                      R1b */
#define MMC_SEND_STATUS          13     /* ac   [31:16] RCA        R1  */
#define MMC_GO_INACTIVE_STATE    15     /* ac   [31:16] RCA            */

/* class 2 */
#define MMC_SET_BLOCKLEN         16     /* ac   [31:0]  block len  R1  */
#define MMC_READ_SINGLE_BLOCK    17     /* adtc [31:0]  data addr  R1  */
#define MMC_READ_MULTIPLE_BLOCK  18     /* adtc [31:0]  data addr  R1  */

/* class 3 */
#define MMC_WRITE_DAT_UNTIL_STOP 20     /* adtc [31:0]  data addr  R1  */

/* class 4 */
#define MMC_SET_BLOCK_COUNT      23     /* adtc [31:0]  data addr  R1  */
#define MMC_WRITE_BLOCK          24     /* adtc [31:0]  data addr  R1  */
#define MMC_WRITE_MULTIPLE_BLOCK 25     /* adtc                    R1  */
#define MMC_PROGRAM_CID          26     /* adtc                    R1  */
#define MMC_PROGRAM_CSD          27     /* adtc                    R1  */

/* class 6 */
#define MMC_SET_WRITE_PROT       28     /* ac   [31:0]  data addr  R1b */
#define MMC_CLR_WRITE_PROT       29     /* ac   [31:0]  data addr  R1b */
#define MMC_SEND_WRITE_PROT      30     /* adtc [31:0]  wpdata addr R1  */

/* class 5 */
#define MMC_ERASE_GROUP_START    35     /* ac   [31:0]  data addr  R1  */
#define MMC_ERASE_GROUP_END      36     /* ac   [31:0]  data addr  R1  */
#define MMC_ERASE                37     /* ac                      R1b */
#define SD_ERASE_WR_BLK_START    32     /* ac   [31:0]  data addr  R1  */
#define SD_ERASE_WR_BLK_END      33     /* ac   [31:0]  data addr  R1  */
#define SD_ERASE                 38     /* ac                      R1b */

/* class 9 */
#define MMC_FAST_IO              39     /* ac   <Complex>          R4  */
#define MMC_GO_IRQ_STATE         40     /* bcr                     R5  */

/* class 7 */
#define MMC_LOCK_UNLOCK          42     /* adtc                    R1b */

/* class 8 */
#define MMC_APP_CMD              55     /* ac   [31:16] RCA        R1  */
#define MMC_GEN_CMD              56     /* adtc [0]     RD/WR      R1b */

/* SD commands                           type  argument     response */
/* class 8 */
/* This is basically the same command as for MMC with some quirks. */
#define SD_SEND_RELATIVE_ADDR     3     /* ac                      R6  */
#define SD_CMD8                   8     /* bcr  [31:0]  OCR        R3  */

/* Application commands */
#define SD_CMD6                   6     /* bcr  [31:0]  OCR        R1  */ // SwitchMode0, Access Mode-1, CMD Sys 2, drive str....
#define SD_APP_SET_BUS_WIDTH      6     /* ac   [1:0]   bus width  R1   */ // ACMD6
#define SD_APP_OP_COND           41     /* bcr  [31:0]  OCR        R1 (R4)  */
#define SD_APP_SEND_SCR          51     /* adtc                    R1   */

/**
 * @brief MMC status in R1<br>
 * Type<br>
 *   e : error bit<br>
 *   s : status bit<br>
 *   r : detected and set for the actual command response<br>
 *   x : detected and set during command execution. the host must poll
 *       the card by sending status command in order to read these bits.
 * Clear condition<br>
 *   a : according to the card state<br>
 *   b : always related to the previous command. Reception of
 *       a valid command will clear it (with a delay of one command)<br>
 *   c : clear by read<br>
 */

#define R1_OUT_OF_RANGE         (1UL << 31) /* er, c */
#define R1_ADDRESS_ERROR        (1 << 30)   /* erx, c */
#define R1_BLOCK_LEN_ERROR      (1 << 29)   /* er, c */
#define R1_ERASE_SEQ_ERROR      (1 << 28)   /* er, c */
#define R1_ERASE_PARAM          (1 << 27)   /* ex, c */
#define R1_WP_VIOLATION         (1 << 26)   /* erx, c */
#define R1_CARD_IS_LOCKED       (1 << 25)   /* sx, a */
#define R1_LOCK_UNLOCK_FAILED   (1 << 24)   /* erx, c */
#define R1_COM_CRC_ERROR        (1 << 23)   /* er, b */
#define R1_ILLEGAL_COMMAND      (1 << 22)   /* er, b */
#define R1_CARD_ECC_FAILED      (1 << 21)   /* ex, c */
#define R1_CC_ERROR             (1 << 20)   /* erx, c */
#define R1_ERROR                (1 << 19)   /* erx, c */
#define R1_UNDERRUN             (1 << 18)   /* ex, c */
#define R1_OVERRUN              (1 << 17)   /* ex, c */
#define R1_CID_CSD_OVERWRITE    (1 << 16)   /* erx, c, CID/CSD overwrite */
#define R1_WP_ERASE_SKIP        (1 << 15)   /* sx, c */
#define R1_CARD_ECC_DISABLED    (1 << 14)   /* sx, a */
#define R1_ERASE_RESET          (1 << 13)   /* sr, c */
#define R1_STATUS(x)            (x & 0xFFFFE000)
#define R1_CURRENT_STATE(x)     ((x & 0x00001E00) >> 9) /* sx, b (4 bits) */
#define R1_READY_FOR_DATA       (1 << 8)    /* sx, a */
#define R1_APP_CMD              (1 << 5)    /* sr, c */

/**
 * @brief SD/MMC card OCR register bits
 */
#define OCR_ALL_READY           (1UL << 31) /* Card Power up status bit */
#define OCR_HC_CCS              (1 << 30)   /* High capacity card */
#define OCR_VOLTAGE_RANGE_MSK   (0x00FF8000)

#define SD_SEND_IF_ARG          0x000001AA
#define SD_SEND_IF_ECHO_MSK     0x000000FF
#define SD_SEND_IF_RESP         0x000000AA

/**
 * @brief R3 response definitions
 */
#define CMDRESP_R3_OCR_VAL(n)           (((uint32_t) n) & 0xFFFFFF)
#define CMDRESP_R3_S18A                 (((uint32_t) 1 ) << 24)
#define CMDRESP_R3_HC_CCS               (((uint32_t) 1 ) << 30)
#define CMDRESP_R3_INIT_COMPLETE        (((uint32_t) 1 ) << 31)

/**
 * @brief R6 response definitions
 */
#define CMDRESP_R6_RCA_VAL(n)           (((uint32_t) (n >> 16)) & 0xFFFF)
#define CMDRESP_R6_CARD_STATUS(n)       (((uint32_t) (n & 0x1FFF)) | \
                                         ((n & (1 << 13)) ? (1 << 19) : 0) | \
                                         ((n & (1 << 14)) ? (1 << 22) : 0) | \
                                         ((n & (1 << 15)) ? (1 << 23) : 0))

/**
 * @brief R7 response definitions
 */
/** Echo-back of check-pattern */
#define CMDRESP_R7_CHECK_PATTERN(n)     (((uint32_t) n ) & 0xFF)
/** Voltage accepted */
#define CMDRESP_R7_VOLTAGE_ACCEPTED     (((uint32_t) 1 ) << 8)

/**
 * @brief CMD3 command definitions
 */
/** Card Address */
#define CMD3_RCA(n)         (((uint32_t) (n & 0xFFFF) ) << 16)

/**
 * @brief CMD7 command definitions
 */
/** Card Address */
#define CMD7_RCA(n)         (((uint32_t) (n & 0xFFFF) ) << 16)

/**
 * @brief CMD8 command definitions
 */
/** Check pattern */
#define CMD8_CHECKPATTERN(n)            (((uint32_t) (n & 0xFF) ) << 0)
/** Recommended pattern */
#define CMD8_DEF_PATTERN                    (0xAA)
/** Voltage supplied.*/
#define CMD8_VOLTAGESUPPLIED_27_36     (((uint32_t) 1 ) << 8)

/**
 * @brief CMD9 command definitions
 */
#define CMD9_RCA(n)         (((uint32_t) (n & 0xFFFF) ) << 16)

/**
 * @brief CMD13 command definitions
 */
#define CMD13_RCA(n)            (((uint32_t) (n & 0xFFFF) ) << 16)

/**
 * @brief APP_CMD command definitions
 */
#define CMD55_RCA(n)            (((uint32_t) (n & 0xFFFF) ) << 16)

/**
 * @brief ACMD41 command definitions
 */
#define ACMD41_OCR(n)                   (((uint32_t) n) & 0xFFFFFF)
#define ACMD41_S18R                     (((uint32_t) 1 ) << 24)
#define ACMD41_XPC                      (((uint32_t) 1 ) << 28)
#define ACMD41_HCS                      (((uint32_t) 1 ) << 30)

/**
 * @brief ACMD6 command definitions
 */
#define ACMD6_BUS_WIDTH(n)              ((uint32_t) n & 0x03)
#define ACMD6_BUS_WIDTH_1               (0)
#define ACMD6_BUS_WIDTH_4               (2)

/** @brief Card type defines
 */
#define CARD_TYPE_SD    (1 << 0)
#define CARD_TYPE_4BIT  (1 << 1)
#define CARD_TYPE_8BIT  (1 << 2)
#define CARD_TYPE_HC    (OCR_HC_CCS)/*!< high capacity card > 2GB */

/**
 * @brief SD/MMC sector size in bytes
 */
#define MMC_SECTOR_SIZE     512

/**
 * @brief Typical enumeration clock rate
 */
#define SD_MMC_ENUM_CLOCK       400000

/**
 * @brief Max MMC clock rate
 */
#if(UEZ_PROCESSOR == NXP_LPC4357)
// This combination passes with uEZ > 2.13 on 50WVN R1.4, 50WVN R2, 70WVN R1 boards
// This only satisfies MMC spec, not SD spec. Need to lower MCU clock to get <=50MHz for SD spec.
#define MMC_MAX_READ_CLOCK           51000000
#define MMC_MAX_WRITE_CLOCK          51000000

/* // This combination (single block writes)  passes with uEZ > 2.13 on 50WVN R1.4, 50WVN R2, 70WVN R1 boards
#define MMC_MAX_READ_CLOCK           51000000
#define MMC_MAX_WRITE_CLOCK          25500000 */

/* // This combination has issues with some SD card switchers/repeaters
#define MMC_MAX_READ_CLOCK           34000000
#define MMC_MAX_WRITE_CLOCK          25500000*/

/* // This combination at passes file write test using single block writes, but only if input glitch filter is disabled on the 6 signal pins.
#define MMC_MAX_READ_CLOCK           25500000
#define MMC_MAX_WRITE_CLOCK          25500000 */

/* // Force slow write speed
#define MMC_MAX_READ_CLOCK           25500000
#define MMC_MAX_WRITE_CLOCK          12750000*/

#else
#define MMC_MAX_READ_CLOCK           20000000
#define MMC_MAX_WRITE_CLOCK          20000000
#endif

/**
 * @brief Type 0 MMC card max clock rate
 */
#define MMC_LOW_BUS_MAX_CLOCK   25500000

/**
 * @brief Type 1 MMC card max clock rate
 */
#define MMC_HIGH_BUS_MAX_CLOCK  51000000

/**
 * @brief Max SD clock rate
 */
#if(UEZ_PROCESSOR != NXP_LPC4357)
#define SD_MAX_CLOCK            25000000
#else
#define SD_MAX_CLOCK            25500000//34000000
#endif

#define CMD_MASK_RESP       (0x3UL << 28)
#define CMD_RESP(r)         (((r) & 0x3) << 28)
#define CMD_RESP_R0         (0 << 28)
#define CMD_RESP_R1         (1 << 28)
#define CMD_RESP_R2         (2 << 28)
#define CMD_RESP_R3         (3 << 28)
#define CMD_BIT_AUTO_STOP   (1 << 24)
#define CMD_BIT_APP         (1 << 23)
#define CMD_BIT_INIT        (1 << 22)
#define CMD_BIT_BUSY        (1 << 21)
#define CMD_BIT_LS          (1 << 20)   /* Low speed, used during acquire */
#define CMD_BIT_DATA        (1 << 19)
#define CMD_BIT_WRITE       (1 << 18)
#define CMD_BIT_STREAM      (1 << 17)
#define CMD_MASK_CMD        (0xff)
#define CMD_SHIFT_CMD       (0)

#define CMD(c, r)        ( ((c) &  CMD_MASK_CMD) | CMD_RESP((r)) )

#define CMD_IDLE            CMD(MMC_GO_IDLE_STATE, 0) | CMD_BIT_LS    | CMD_BIT_INIT
#define CMD_SD_SEND_SWITCH  CMD(SD_CMD6, 1)      | CMD_BIT_LS | CMD_BIT_APP
#define CMD_SD_OP_COND      CMD(SD_APP_OP_COND, 1)      | CMD_BIT_LS | CMD_BIT_APP
#define CMD_SD_SEND_IF_COND CMD(SD_CMD8, 1)      | CMD_BIT_LS
#define CMD_MMC_OP_COND     CMD(MMC_SEND_OP_COND, 3)    | CMD_BIT_LS | CMD_BIT_INIT
#define CMD_ALL_SEND_CID    CMD(MMC_ALL_SEND_CID, 2)    | CMD_BIT_LS
#define CMD_MMC_SET_RCA     CMD(MMC_SET_RELATIVE_ADDR, 1) | CMD_BIT_LS
#define CMD_SD_SEND_RCA     CMD(SD_SEND_RELATIVE_ADDR, 1) | CMD_BIT_LS
#define CMD_SEND_CSD        CMD(MMC_SEND_CSD, 2) | CMD_BIT_LS
#define CMD_SEND_EXT_CSD    CMD(MMC_SEND_EXT_CSD, 1) | CMD_BIT_LS | CMD_BIT_DATA
#define CMD_DESELECT_CARD   CMD(MMC_SELECT_CARD, 0)
#define CMD_SELECT_CARD     CMD(MMC_SELECT_CARD, 1)
#define CMD_SET_BLOCKLEN    CMD(MMC_SET_BLOCKLEN, 1)
#define CMD_SEND_STATUS     CMD(MMC_SEND_STATUS, 1)
#define CMD_READ_SINGLE     CMD(MMC_READ_SINGLE_BLOCK, 1) | CMD_BIT_DATA
#define CMD_READ_MULTIPLE   CMD(MMC_READ_MULTIPLE_BLOCK, 1) | CMD_BIT_DATA | CMD_BIT_AUTO_STOP
#define CMD_SD_SET_WIDTH    CMD(SD_APP_SET_BUS_WIDTH, 1) | CMD_BIT_APP
#define CMD_STOP            CMD(MMC_STOP_TRANSMISSION, 1) | CMD_BIT_BUSY
#define CMD_WRITE_SINGLE    CMD(MMC_WRITE_BLOCK, 1) | CMD_BIT_DATA | CMD_BIT_WRITE
#define CMD_WRITE_MULTIPLE  CMD(MMC_WRITE_MULTIPLE_BLOCK, 1) | CMD_BIT_DATA | CMD_BIT_WRITE | CMD_BIT_AUTO_STOP


/** @brief 4357 MMC/SD/SDIO Interrupt status & mask register defines
 */
#define MCI_INT_SDIO            (1 << 16)       /*!< SDIO interrupt */
#define MCI_INT_EBE             (1 << 15)       /*!< End-bit error */
#define MCI_INT_ACD             (1 << 14)       /*!< Auto command done */
#define MCI_INT_SBE             (1 << 13)       /*!< Start bit error */
#define MCI_INT_HLE             (1 << 12)       /*!< Hardware locked error */
#define MCI_INT_FRUN            (1 << 11)       /*!< FIFO overrun/underrun error */
#define MCI_INT_HTO             (1 << 10)       /*!< Host data starvation error */
#define MCI_INT_DTO             (1 << 9)        /*!< Data read timeout (DRTO_BDS) */
#define MCI_INT_RTO             (1 << 8)        /*!< Response timeout error (RTO_BAR) */
#define MCI_INT_DCRC            (1 << 7)        /*!< Data CRC error */
#define MCI_INT_RCRC            (1 << 6)        /*!< Response CRC error */
#define MCI_INT_RXDR            (1 << 5)        /*!< RX data ready */
#define MCI_INT_TXDR            (1 << 4)        /*!< TX data needed */
#define MCI_INT_DATA_OVER       (1 << 3)        /*!< Data transfer over */
#define MCI_INT_CMD_DONE        (1 << 2)        /*!< Command done */
#define MCI_INT_RESP_ERR        (1 << 1)        /*!< Command response error */
#define MCI_INT_CD              (1 << 0)        /*!< Card detect */

/* Helper definition: all SD error conditions in the status word */
#define SD_INT_ERROR (MCI_INT_RESP_ERR | MCI_INT_RCRC | MCI_INT_DCRC | \
                      MCI_INT_RTO | MCI_INT_DTO | MCI_INT_HTO | MCI_INT_FRUN | MCI_INT_HLE | \
                      MCI_INT_SBE | MCI_INT_EBE)


/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef TUInt8 T_uezSD_MMCResponseType;
#define UEZ_SD_MMC_RESPONSE_TYPE_NONE                      0   /* 0 bytes */
#define UEZ_SD_MMC_RESPONSE_TYPE_SHORT                     1   /* 4 bytes */
#define UEZ_SD_MMC_RESPONSE_TYPE_RESPONSE_LONG             2   /* 16 bytes */

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

#endif // _TYPES_SD_MMC_H_
/*-------------------------------------------------------------------------*
 * End of File:  SD_MMC.h
 *-------------------------------------------------------------------------*/
