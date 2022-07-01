/*-------------------------------------------------------------------------*
 * File:  Types/EINT.h
 *-------------------------------------------------------------------------*
 * Description:
 *     EINT types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _EINT_TYPES_H_
#define _EINT_TYPES_H_

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
 *  @file   /Include/Types/EINT.h
 *  @brief  uEZ EINT Types
 *
 *  The uEZ EINT Types
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
#include <HAL/Interrupt.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef enum {
    EINT_TRIGGER_EDGE_FALLING   =0,
    EINT_TRIGGER_EDGE_RISING    =1,
    EINT_TRIGGER_LEVEL_LOW      =3,
    EINT_TRIGGER_LEVEL_HIGH     =4,
    EINT_TRIGGER_EDGE_BOTH      =5
} T_EINTTrigger;

typedef enum {
    EINT_PRIORITY_HIGHEST   =INTERRUPT_PRIORITY_HIGHEST,
    EINT_PRIORITY_HIGH      =INTERRUPT_PRIORITY_HIGH,
    EINT_PRIORITY_NORMAL    =INTERRUPT_PRIORITY_NORMAL,
    EINT_PRIORITY_LOW       =INTERRUPT_PRIORITY_LOW,
    EINT_PRIORITY_LOWEST    =INTERRUPT_PRIORITY_LOWEST
} T_EINTPriority;


typedef TBool (*EINT_Callback)(void *aCallbackWorkspace, TUInt32 aChannel); /**<
 * This callback is called when the interrupt fires off.
 * Processing occurs inside the ISR, so be careful.
 * Return ETrue clears the EINT and lets it run again.  Using EFalse
 * requires calling Clear (below) to let the interrupt occur again. */

#ifdef __cplusplus
}
#endif

#endif // _EINT_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/EINT.h
 *-------------------------------------------------------------------------*/
