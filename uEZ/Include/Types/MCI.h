/*-------------------------------------------------------------------------*
 * File:  MCI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef _TYPES_MCI_H_
#define _TYPES_MCI_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

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
 
#ifdef __cplusplus
}
#endif

#endif // _TYPES_MCI_H_
/*-------------------------------------------------------------------------*
 * End of File:  MCI.h
 *-------------------------------------------------------------------------*/
