/*-------------------------------------------------------------------------*
 * File:  LookAndFeel.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LOOKANDFEEL_H_
#define LOOKANDFEEL_H_

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

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct _T_LAFMapping {
    // ID in window, or 0 if last entry in mapping list
    int iID;

    // Optional default text, or 0 for no changes
    const char *iText;

    // General font and colors
    TUInt32 iColor;
    TUInt32 iFontColor;
    const GUI_FONT *iFont;

    // Setup routine to call when starting up the dialog
    void (*iSetup)(const struct _T_LAFMapping *p, WM_HWIN hItem);

    // Command to call on appropriate message types
    TBool (*iCommand)(WM_MESSAGE * pMsg, int aID, int aNCode);

    // possible additional data, or 0 for none
    void *iData;
} T_LAFMapping;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void LAFSetupWindow(const T_LAFMapping *p, WM_HWIN hItem);
void LAFSetupText(const T_LAFMapping *p, WM_HWIN hItem);
void LAFSetupRadioText(const T_LAFMapping *p, WM_HWIN hItem);
void LAFSetupButton(const T_LAFMapping *p, WM_HWIN hItem);
void LAFSetupScrollbar(const T_LAFMapping *p, WM_HWIN hItem);
TBool LAFHandleEvent(
        const T_LAFMapping *aMapping,
        WM_MESSAGE * pMsg,
        int aNCode,
        int aID);
void LAFSetup(
        WM_HWIN hWin,
        int aWindowID,
        const T_LAFMapping *aMapping);

void TouchscreenClearPresses(void);

#endif // LOOKANDFEEL_H_
/*-------------------------------------------------------------------------*
 * End of File:  LookAndFeel.h
 *-------------------------------------------------------------------------*/
