/*-------------------------------------------------------------------------*
 * File:  LookAndFeel.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
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
#include <uEZ.h>
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "LookAndFeel.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  LAFSetupWindow
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the window properties
 * Inputs:
 *      T_LAFMapping *p -- Mapping of the dialog that holds the
 *                             properties.
 *      WH_HWIN hItem -- handle to the item to be set.
 *---------------------------------------------------------------------------*/
void LAFSetupWindow(const T_LAFMapping *p, WM_HWIN hItem)
{
    WINDOW_SetBkColor(hItem, p->iColor);
}

/*---------------------------------------------------------------------------*
 * Routine:  LAFSetupText
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the properties of the text fields.
 * Inputs:
 *      T_LAFMapping *p -- Mapping of the dialog that holds the
 *                             properties.
 *      WH_HWIN hItem -- handle to the item to be set.
 *---------------------------------------------------------------------------*/
void LAFSetupText(const T_LAFMapping *p, WM_HWIN hItem)
{
    TEXT_SetFont(hItem, p->iFont);
    TEXT_SetTextColor(hItem, p->iFontColor);
    TEXT_SetBkColor(hItem, p->iColor);
}

/*---------------------------------------------------------------------------*
 * Routine:  LAFSetupButton
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the properties of the button fields.
 * Inputs:
 *      T_LAFMapping *p -- Mapping of the dialog that holds the
 *                             properties.
 *      WH_HWIN hItem -- handle to the item to be set.
 *---------------------------------------------------------------------------*/
void LAFSetupButton(const T_LAFMapping *p, WM_HWIN hItem)
{
    BUTTON_SetFont(hItem, p->iFont);
    BUTTON_SetTextColor(hItem, BUTTON_CI_UNPRESSED, p->iFontColor);
    BUTTON_SetTextColor(hItem, BUTTON_CI_PRESSED, p->iFontColor);
    BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED, p->iColor);
    BUTTON_SetBkColor(hItem, BUTTON_CI_PRESSED, GUI_LIGHTGRAY);
    //BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetFocussable(hItem, 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  LAFSetupScrollbar
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the properties of the scroll fields
 * Inputs:
 *      T_LAFMapping *p -- Mapping of the dialog that holds the
 *                             properties.
 *      WH_HWIN hItem -- handle to the item to be set.
 *---------------------------------------------------------------------------*/
void LAFSetupScrollbar(const T_LAFMapping *p, WM_HWIN hItem)
{
    // Nothing currently
}

/*---------------------------------------------------------------------------*
 * Task:  ISetup
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup routine for the dialog items.
 * Inputs:
 *      WM_HWIN hWin -- Handle of window with the items being setup for
 *          this look and feel.
 *      int aWindowID -- ID of the root window
 *      const T_LAFMapping *aMapping -- Array of mappings
 *---------------------------------------------------------------------------*/
void LAFSetup(
        WM_HWIN hWin,
        int aWindowID,
        const T_LAFMapping *aMapping)
{
    const T_LAFMapping *p = aMapping;
    WM_HWIN hItem;

    while (p->iID) {
        // First item MUST be the window
        if (p->iID == aWindowID)
            hItem = hWin;
        else
            hItem = WM_GetDialogItem(hWin, p->iID);
        if ((p->iSetup) && (hItem))
            p->iSetup(p, hItem);
        p++;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:        LAFHandleEvent
 *---------------------------------------------------------------------------*
 * Description:
 *      The standard Look and Feel event handler.  Returns ETrue if the
 *      event was handled, else EFalse.
 * Inputs:
 *      const T_LAFMapping *aMapping -- Mapping to searching
 *      WM_MESSAGE *pMsg -- message structure to handle
 *      int aNCode -- Type of event received to process.
 *      int aID -- ID of window receiving this
 * Outputs:
 *      TBool -- EFalse is not problems, else ETrue.
 *---------------------------------------------------------------------------*/
TBool LAFHandleEvent(
        const T_LAFMapping *aMapping,
        WM_MESSAGE * pMsg,
        int aNCode,
        int aID)
{
    TBool handled = EFalse;
    const T_LAFMapping *p = aMapping;
    extern void ButtonPressed(void);

    /* Find the code in the table */
    while (p->iID) {
        /* Is this the ID we want? */
        if (p->iID == aID) {
            if (aNCode == WM_NOTIFICATION_VALUE_CHANGED) {
                // Scrollbar value has changed
                if (p->iCommand)
                    handled = (p->iCommand)(pMsg, aNCode, aID);
            } else if (aNCode == WM_NOTIFICATION_CLICKED) {
                if (p->iCommand)
                    handled = (p->iCommand)(pMsg, aNCode, aID);
            } else if (aNCode == WM_NOTIFICATION_RELEASED) {
                // Button was released
                /* If this button has a special command, go process it */
                if (p->iCommand) {
                    /* Call the command associated with this ID */
                    handled = (p->iCommand)(pMsg, aNCode, aID);
                } else {
                    /* No command, do the standard code */

                }
            }
            break;
        }
        p++;
    }
    return handled;
}

void TouchscreenClearPresses(void)
{
    GUI_PID_STATE state;
    int i;

    for (i = 0; i < 10; i++) {
        GUI_PID_GetState(&state);
    }
}


/*-------------------------------------------------------------------------*
 * End of File:  LookAndFeel.c
 *-------------------------------------------------------------------------*/
