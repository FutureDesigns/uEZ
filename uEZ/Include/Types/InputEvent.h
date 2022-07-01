/*-------------------------------------------------------------------------*
 * File:  Types/InputEvent.h
 *-------------------------------------------------------------------------*
 * Description:
 *     Definition of an input event
 *-------------------------------------------------------------------------*/
#ifndef _INPUT_EVENT_H_
#define _INPUT_EVENT_H_

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
 *  @file   /Include/Types/InputEvent.h
 *  @brief  uEZ Input Event Types
 *
 *  The uEZ Input Event Types
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
 * Constants:
 *-------------------------------------------------------------------------*/
#define INPUT_EVENT_TYPE_BUTTON         1
#define INPUT_EVENT_TYPE_XY             2

#define BUTTON_ACTION_PRESS             0x0001
#define BUTTON_ACTION_RELEASE           0x0000

#define XY_ACTION_PRESS_AND_HOLD        0x0001
#define XY_ACTION_RELEASE               0x0000

#define KEY_NONE                        0xFFFF

#define KEY_F1                          0xFF01
#define KEY_F2                          0xFF02
#define KEY_F3                          0xFF03
#define KEY_F4                          0xFF04
#define KEY_F5                          0xFF05
#define KEY_F6                          0xFF06

#define KEY_ARROW_LEFT                  0xFE00
#define KEY_ARROW_RIGHT                 0xFE01
#define KEY_ARROW_UP                    0xFE02
#define KEY_ARROW_DOWN                  0xFE03

#define KEY_MENU                        0xFD00
#define KEY_END_ON_OFF                  0xFD01
#define KEY_SEND                        0xFD02
#define KEY_OK                          KEY_ENTER
#define KEY_CLEAR                       KEY_BACKSPACE

#define KEY_0                           '0'
#define KEY_1                           '1'
#define KEY_2                           '2'
#define KEY_3                           '3'
#define KEY_4                           '4'
#define KEY_5                           '5'
#define KEY_6                           '6'
#define KEY_7                           '7'
#define KEY_8                           '8'
#define KEY_9                           '9'
#define KEY_ASTERISK                    '*'
#define KEY_POUND                       '#'
#define KEY_PLUS                        '+'
#define KEY_MINUS                       '-'
#define KEY_ENTER                       '\r'
#define KEY_BACKSPACE                   '\b'


/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    TUInt32 iType;
    union {
        struct {
            TUInt32 iKey;
            TUInt32 iAction;
        } iButton;
        struct {
            TUInt32 iX;
            TUInt32 iY;
            TUInt32 iAction;
        } iXY;
    } iEvent;
} T_uezInputEvent;

#ifdef __cplusplus
}
#endif

#endif // _INPUT_EVENT_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/InputEvent.h
 *-------------------------------------------------------------------------*/
