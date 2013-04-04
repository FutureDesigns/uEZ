/*-------------------------------------------------------------------------*
 * File:  Types/EINT.h
 *-------------------------------------------------------------------------*
 * Description:
 *     EINT types (non-processor specific)
 *-------------------------------------------------------------------------*/
#ifndef _EINT_TYPES_H_
#define _EINT_TYPES_H_

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

#endif // _EINT_TYPES_H_
/*-------------------------------------------------------------------------*
 * End of File:  Types/EINT.h
 *-------------------------------------------------------------------------*/
