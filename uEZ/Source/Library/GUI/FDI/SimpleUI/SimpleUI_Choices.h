/*-------------------------------------------------------------------------*
 * File:  SimpleUI_Choices.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Core code for handling the selection of choices (aka buttons) on
 *      the screen.
 *-------------------------------------------------------------------------*/
#ifndef _SIMPLEUI_CHOICES_H_
#define _SIMPLEUI_CHOICES_H_

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
#include <uEZ.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Types/InputEvent.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef TUInt16 T_choiceFlags;
#define SUI_FLAG_REPEAT         0x0001

typedef struct tag_choice {
    TUInt16 iLeft;
    TUInt16 iTop;
    TUInt16 iRight;
    TUInt16 iBottom;
    const char *iText;
    void (*iAction)(const struct tag_choice *p_choice);
    const TUInt8 *iIcon;
    T_choiceFlags iFlags;
    void *iData;
    void (*iDraw)(const struct tag_choice *p_choice);
} T_choice;

typedef struct {
    T_pixelColor iSelectColor;
    T_pixelColor iUnselectColor;
} T_choicesSettings;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void ChoicesDraw(SWIM_WINDOW_T *aWin, const T_choice *aChoices);
const T_choice *ChoicesUpdate(
        SWIM_WINDOW_T *aWin,
        const T_choice *aChoices,
        T_uezQueue aTouchQueue,
        TUInt32 aTimeout);
const T_choice *ChoicesUpdateByReading(
        SWIM_WINDOW_T *aWin,
        const T_choice *aChoices,
        T_uezInputEvent *p_inputEvent);
const T_choice *ChoicesUpdateContinuously(
        SWIM_WINDOW_T *aWin,
        const T_choice *aChoices,
        T_uezQueue aInputEventQueue,
        TUInt32 aTimeout);
void ChoicesSetSettings(T_choicesSettings *aSettings);
void ChoicesGetSettings(T_choicesSettings *aSettings);

#ifdef __cplusplus
}
#endif

#endif // _SIMPLEUI_CHOICES_H_
/*-------------------------------------------------------------------------*
 * End of File:  SimpleUI_Choices.h
 *-------------------------------------------------------------------------*/
