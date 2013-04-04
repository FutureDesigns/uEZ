/*-------------------------------------------------------------------------*
 * File:  MCI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef _TYPES_MCI_H_
#define _TYPES_MCI_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
/* ----- MMC/SDC command ----- */
#define MCI_CMD0    (0)             /* GO_IDLE_STATE */
#define MCI_CMD1    (1)             /* SEND_OP_COND (MMC) */
#define MCI_CMD2    (2)             /* ALL_SEND_CID */
#define MCI_CMD3    (3)             /* SEND_RELATIVE_ADDR */
#define MCI_ACMD6   (6|0x80)        /* SET_BUS_WIDTH (SDC) */
#define MCI_CMD7    (7)             /* SELECT_CARD */
#define MCI_CMD8    (8)             /* SEND_IF_COND */
#define MCI_CMD9    (9)             /* SEND_CSD */
#define MCI_CMD10   (10)            /* SEND_CID */
#define MCI_CMD12   (12)            /* STOP_TRANSMISSION */
#define MCI_CMD13   (13)            /* SEND_STATUS */
#define MCI_ACMD13  (13|0x80)       /* SD_STATUS (SDC) */
#define MCI_CMD16   (16)            /* SET_BLOCKLEN */
#define MCI_CMD17   (17)            /* READ_SINGLE_BLOCK */
#define MCI_CMD18   (18)            /* READ_MULTIPLE_BLOCK */
#define MCI_CMD23   (23)            /* SET_BLK_COUNT (MMC) */
#define MCI_ACMD23  (23|0x80)       /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define MCI_CMD24   (24)            /* WRITE_BLOCK */
#define MCI_CMD25   (25)            /* WRITE_MULTIPLE_BLOCK */
#define MCI_CMD32   (32)            /* ERASE_ER_BLK_START */
#define MCI_CMD33   (33)            /* ERASE_ER_BLK_END */
#define MCI_CMD38   (38)            /* ERASE */
#define MCI_ACMD41  (41|0x80)       /* SEND_OP_COND (SDC) */
#define MCI_CMD55   (55)            /* APP_MCI_CMD */

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef TUInt8 T_uezMCIResponseType;
#define UEZ_MCI_RESPONSE_TYPE_NONE                      0   /* 0 bytes */
#define UEZ_MCI_RESPONSE_TYPE_SHORT                     1   /* 4 bytes */
#define UEZ_MCI_RESPONSE_TYPE_RESPONSE_LONG             2   /* 16 bytes */


/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

#endif // _TYPES_MCI_H_
/*-------------------------------------------------------------------------*
 * End of File:  MCI.h
 *-------------------------------------------------------------------------*/
