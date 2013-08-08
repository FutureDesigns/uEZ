/*-------------------------------------------------------------------------*
 * File:  SimpleUI_Choices.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Core code for handling the selection of choices (aka buttons) on
 *      the screen.
 *-------------------------------------------------------------------------*/

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
#ifndef _SIMPLEUI_CHOICES_H_
#define _SIMPLEUI_CHOICES_H_

#include <uEZ.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Types/InputEvent.h>

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
void ChoicesSetSettings(T_choicesSettings *aSettings);
void ChoicesGetSettings(T_choicesSettings *aSettings);

#endif // _SIMPLEUI_CHOICES_H_
/*-------------------------------------------------------------------------*
 * End of File:  SimpleUI_Choices.h
 *-------------------------------------------------------------------------*/
