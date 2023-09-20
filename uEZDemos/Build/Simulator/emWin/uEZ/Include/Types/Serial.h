/*-------------------------------------------------------------------------*
 * File:  Serial.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Generic serial types used by several systems
 *-------------------------------------------------------------------------*/
#ifndef _SERIAL_TYPES_H_
#define _SERIAL_TYPES_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
/**
 *  @file   /Include/Types/Serial.h
 *  @brief  uEZ Serial Types
 *
 *  The uEZ Serial Types
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

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    SERIAL_STOP_BITS_1,
    SERIAL_STOP_BITS_2,
    SERIAL_STOP_BITS_1_POINT_5,
} T_Serial_StopBit;

typedef enum {
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_ODD,
    SERIAL_PARITY_EVEN,
    SERIAL_PARITY_MARK,
    SERIAL_PARITY_SPACE,
} T_Serial_Parity;

typedef enum {
    SERIAL_FLOW_CONTROL_NONE,
    SERIAL_FLOW_CONTROL_XON_XOFF,
    SERIAL_FLOW_CONTROL_HARDWARE, /** auto RTS and auto CTS (if supported) */
} T_Serial_FlowControl;

typedef struct {
    TUInt32 iBaud;
    TUInt8 iWordLength;
    T_Serial_StopBit iStopBit;
    T_Serial_Parity iParity;
    T_Serial_FlowControl iFlowControl;
} T_Serial_Settings;

/** If supported, these are the STREAM_CONTROL_CONTROL_HANDSHAKING bits
 * (basically the Modem Control Register of a 16550 UART) */
typedef TUInt32 T_serialHandshakingControl;
#define SERIAL_HANDSHAKING_CTS_ENABLE           0x0080
#define SERIAL_HANDSHAKING_RTS_ENABLE           0x0040
#define SERIAL_HANDSHAKING_RTS_HIGH             0x0002
#define SERIAL_HANDSHAKING_DTR_HIGH             0x0001

/** If supported, these are the STREAM_CONTROL_HANDSHAKING_STATUS bits
 * (basically the Modem Status Register of a 16550 UART) */
typedef TUInt32 T_serialHandshakingStatus;
#define SERIAL_HANDSHAKING_DCD                  0x0080
#define SERIAL_HANDSHAKING_RING                 0x0040
#define SERIAL_HANDSHAKING_DSR                  0x0020
#define SERIAL_HANDSHAKING_CTS                  0x0010
#define SERIAL_HANDSHAKING_DELTA_DCD            0x0008
#define SERIAL_HANDSHAKING_TRAILING_EDGE_RI     0x0004
#define SERIAL_HANDSHAKING_DELTA_DSR            0x0002
#define SERIAL_HANDSHAKING_DELTA_CTS            0x0001

#ifdef __cplusplus
}
#endif

#endif // _SERIAL_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Serial.h
 *-------------------------------------------------------------------------*/
