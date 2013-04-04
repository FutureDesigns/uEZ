/*-------------------------------------------------------------------------*
 * File:  Types/EMAC.h
 *-------------------------------------------------------------------------*
 * Description:
 *     EMAC types for configuration and control
 *-------------------------------------------------------------------------*/
#ifndef _EMAC_TYPES_H_
#define _EMAC_TYPES_H_

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
/**
 *  @file   /Include/Types/EMAC.h
 *  @brief  uEZ EMAC Types
 *
 *  The uEZ EMAC Types
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZTypes.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iPHYType;   	/** 0=unknown, use detect */ 
    TUInt32 iPHYAddr;

    TUInt8 iMACAddress[6]; 	/** 6 byte Ethernet MAC address */ 
} T_EMACSettings;

#endif // _EMAC_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/EMAC.h
 *-------------------------------------------------------------------------*/
