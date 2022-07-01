/*-------------------------------------------------------------------------*
 * File:  Button_Generic_EINT.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef BUTTON_GENERIC_EINT_H_
#define BUTTON_GENERIC_EINT_H_

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
#include <Types/EINT.h>
#include <Device/ButtonBank.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#ifndef UEZ_BUTTON_GENERIC_EINT_MAX_NUM_BUTTONS
#define UEZ_BUTTON_GENERIC_EINT_MAX_NUM_BUTTONS     32
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        const char *const iEINTName;
        TUInt32 iEINTChannel;
        T_EINTTrigger aTrigger;
        T_EINTPriority aPriority;
} T_Button_EINT_Source;

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_ButtonBank ButtonBank_Generic_EINT_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void ButtonBank_Generic_EINT_Configure(
    void *aWorkspace,
    const T_Button_EINT_Source *aSourceArray,
    const TUInt32 aNumSources);
	
#ifdef __cplusplus
}
#endif

#endif // BUTTON_GENERIC_EINT_H_
/*-------------------------------------------------------------------------*
 * End of File:  Button_Generic_EINT.h
 *-------------------------------------------------------------------------*/
