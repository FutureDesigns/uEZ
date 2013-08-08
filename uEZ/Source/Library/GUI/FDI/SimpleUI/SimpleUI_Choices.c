/*-------------------------------------------------------------------------*
 * File:  SimpleUI_Choices.c
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
#include <uEZ.h>
#include "SimpleUI.h"
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <UEZPlatform.h>

#if HAPTIC_FEEDBACK              
extern void hapticFeedback(TUInt8 aTime);
#endif
static TBool G_repeatOn = ETrue;
static TBool G_repeating = EFalse;
static TUInt32 G_repeatTime;
static TUInt32 G_repeatLastTouch;
static T_choicesSettings G_settings = { YELLOW,     // iSelectColor
    BLACK,      // iUnselectColor
    };

#define REPEAT_TIME_1     500
#define REPEAT_TIME_2     250
#define REPEAT_TIME_3     75

/*---------------------------------------------------------------------------*
 * External Hooks:
 *---------------------------------------------------------------------------*/
extern void ButtonClick(void);

/*---------------------------------------------------------------------------*
 * Routine:  IFindChoice
 *---------------------------------------------------------------------------*
 * Description:
 *      Find a choice in the choice list at the given x, y location.
 * Inputs:
 *      const T_choice *aChoiceList -- List of choices to search
 *      TUInt16 aX, aY              -- Coordinate to search
 * Outputs:
 *      const T_choice *            -- Choice to found or 0 if not found.
 *---------------------------------------------------------------------------*/
const T_choice *IFindChoice(const T_choice *aChoiceList, TUInt16 aX, TUInt16 aY)
{
    const T_choice *p;

    for (p = aChoiceList; p->iText; p++) {
        if ((aX >= p->iLeft) && (aX <= p->iRight)) {
            if ((aY >= p->iTop) && (aY <= p->iBottom)) {
                return p;
            }
        }
    }
    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  ChoicesDraw
 *---------------------------------------------------------------------------*
 * Description:
 *      Draw an array of choices on the window.
 * Inputs:
 *      SWIM_WINDOW_T *aWin       -- Window to draw upon
 *      const T_choice *aChoices  -- Choices to draw
 *---------------------------------------------------------------------------*/
void ChoicesDraw(SWIM_WINDOW_T *aWin, const T_choice *aChoices)
{
    INT_32 imageX, imageY;
    int c;
    const char *p;
    const T_choice *p_choice;
    TUInt32 width;

    SUISetDrawWindow(aWin);
    
    for (c = 0; aChoices[c].iText; c++) {
        p_choice = aChoices + c;
        if (p_choice->iDraw) {
            // Custom drawing algorithm
            p_choice->iDraw(p_choice);
        } else {
            // Draw the choices and texxt
            if (p_choice->iIcon) {
                imageX = p_choice->iLeft;
                imageY = p_choice->iTop;
                swim_get_physical_xy(aWin, &imageX, &imageY);
                SUIDrawIcon(p_choice->iIcon, imageX, imageY);
            } else {
#if 0
                swim_put_box(aWin,
                    p_choice->iLeft,
                    p_choice->iTop,
                    p_choice->iRight,
                    p_choice->iBottom);
#endif
            }

            // Calculate width of text
            width = 0;
            for (p = aChoices[c].iText; *p; p++)
                width += swim_get_font_char_width(aWin, *p);

            // Cut width in half
            width >>= 1;

            // Now draw centered below.
            swim_put_text_xy(aWin, aChoices[c].iText,
                ((aChoices[c].iLeft + aChoices[c].iRight) >> 1) - width,
                aChoices[c].iBottom + 2);
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  ChoicesUpdate
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for a touchscreen event or timeout.  Check the event and change
 *      the state of the choices.
 * Inputs:
 *      SWIM_WINDOW_T *aWin         -- Window to draw within
 *      const T_choice *aChoices    -- Choices to use
 *      T_uezQueue aTouchQueue      -- Touchscreen queue
 *      TUInt32 aTimeout            -- Time to wait for touchscreen event
 * Outputs:
 *      const T_choice *            -- Last choice selected or 0 for none.
 *---------------------------------------------------------------------------*/
const T_choice *ChoicesUpdate(
    SWIM_WINDOW_T *aWin,
    const T_choice *aChoices,
    T_uezQueue aInputEventQueue,
    TUInt32 aTimeout)
{
    T_uezInputEvent inputEvent;

    if (UEZQueueReceive(aInputEventQueue, &inputEvent, aTimeout) == UEZ_ERROR_NONE) {
        return ChoicesUpdateByReading(aWin, aChoices, &inputEvent);
    } else {
        return 0;
    }
}

static TUInt32 xMid(const T_choice *p)
{
    return (p->iLeft + p->iRight)/2;
}

static TUInt32 yMid(const T_choice *p)
{
    return (p->iTop + p->iBottom)/2;
}

static TUInt32 absDiff(TUInt32 x, TUInt32 y)
{
    if(x > y)
        return x-y;
    else
        return y-x;
}

static const T_choice *IFindChoiceTopLeft(const T_choice *aChoiceList)
{
    const T_choice *p;
    TUInt32 xySmallestSum = 800+480;
    TUInt32 xySum;
    const T_choice *p_topLeftChoice = 0;
    for (p = aChoiceList; p->iText; p++) {
        xySum = p->iTop + p->iLeft;
        if(xySum < xySmallestSum) {
            xySmallestSum = xySum;
            p_topLeftChoice = p;
        }  
    }
    return p_topLeftChoice;
}

static const T_choice *IFindChoiceRight(const T_choice *aChoiceList, const T_choice *aSelectedChoice, TUInt32 aSearchRange)
{
    const T_choice *p;
    TUInt32 xSmallestDiff = UEZPlatform_LCDGetWidth();
    TUInt32 xDiff, yDiff;
    const T_choice *p_nextChoice = aSelectedChoice;
    for (p = aChoiceList; p->iText; p++) {
        if(p != aSelectedChoice) {
            xDiff = xMid(p) - xMid(aSelectedChoice);
            yDiff = absDiff(yMid(p), yMid(aSelectedChoice));
            if((xDiff < xSmallestDiff) && (yDiff<(xDiff/aSearchRange))) {
                xSmallestDiff = xDiff;
                p_nextChoice = p;
            }
        }
    }
    
    // If we didn't find a new, choice. Lets wrap around our search
    if(p_nextChoice == aSelectedChoice) {
        xSmallestDiff = UEZPlatform_LCDGetWidth();
        for (p = aChoiceList; p->iText; p++) {
            if(p != aSelectedChoice) {
                xDiff = xMid(p);
                yDiff = absDiff(yMid(p), yMid(aSelectedChoice));
                if((xDiff < xSmallestDiff) && (yDiff<(xDiff/aSearchRange))) {
                    xSmallestDiff = xDiff;
                    p_nextChoice = p;
                }
            }
        }
    }
    
    return p_nextChoice;
}

static const T_choice *IFindChoiceLeft(const T_choice *aChoiceList, const T_choice *aSelectedChoice, TUInt32 aSearchRange)
{
    const T_choice *p;
    TUInt32 xSmallestDiff = UEZPlatform_LCDGetWidth();
    TUInt32 xDiff, yDiff;
    const T_choice *p_nextChoice = aSelectedChoice;
    for (p = aChoiceList; p->iText; p++) {
        if(p != aSelectedChoice) {
            xDiff = xMid(aSelectedChoice) - xMid(p);
            yDiff = absDiff(yMid(aSelectedChoice), yMid(p));
            if((xDiff < xSmallestDiff) && (yDiff<(xDiff/aSearchRange))) {
                xSmallestDiff = xDiff;
                p_nextChoice = p;
            }
        }
    }
    
    // If we didn't find a new, choice. Lets wrap around our search
    if(p_nextChoice == aSelectedChoice) {
        xSmallestDiff = UEZPlatform_LCDGetWidth();
        for (p = aChoiceList; p->iText; p++) {
            if(p != aSelectedChoice) {
                xDiff = UEZPlatform_LCDGetWidth() - xMid(p);
                yDiff = absDiff(yMid(p), yMid(aSelectedChoice));
                if((xDiff < xSmallestDiff) && (yDiff<(xDiff/aSearchRange))) {
                    xSmallestDiff = xDiff;
                    p_nextChoice = p;
                }
            }
        }
    }
    
    return p_nextChoice;
}

static const T_choice *IFindChoiceUp(const T_choice *aChoiceList, const T_choice *aSelectedChoice, TUInt32 aSearchRange)
{
    const T_choice *p;
    TUInt32 ySmallestDiff = UEZPlatform_LCDGetHeight();
    TUInt32 xDiff, yDiff;
    const T_choice *p_nextChoice = aSelectedChoice;
    for (p = aChoiceList; p->iText; p++) {
        if(p != aSelectedChoice) {
            yDiff = yMid(aSelectedChoice) - yMid(p);
            xDiff = absDiff(xMid(aSelectedChoice), xMid(p));
            if((yDiff < ySmallestDiff) && (xDiff<(yDiff/aSearchRange))) {
                ySmallestDiff = yDiff;
                p_nextChoice = p;
            }
        }
    }
    
    // If we didn't find a new, choice. Lets wrap around our search
    if(p_nextChoice == aSelectedChoice) {
        ySmallestDiff = UEZPlatform_LCDGetHeight();
        for (p = aChoiceList; p->iText; p++) {
            if(p != aSelectedChoice) {
                yDiff = UEZPlatform_LCDGetHeight() - yMid(p);
                xDiff = absDiff(xMid(aSelectedChoice), xMid(p));
                if((yDiff < ySmallestDiff) && (xDiff<(yDiff/aSearchRange))) {
                    ySmallestDiff = yDiff;
                    p_nextChoice = p;
                }
            }
        }
    }
    
    return p_nextChoice;
}

static const T_choice *IFindChoiceDown(const T_choice *aChoiceList, const T_choice *aSelectedChoice, TUInt32 aSearchRange)
{
    const T_choice *p;
    TUInt32 ySmallestDiff = UEZPlatform_LCDGetHeight();
    TUInt32 xDiff, yDiff;
    const T_choice *p_nextChoice = aSelectedChoice;
    for (p = aChoiceList; p->iText; p++) {
        if(p != aSelectedChoice) {
            yDiff = yMid(p) - yMid(aSelectedChoice);
            xDiff = absDiff(xMid(aSelectedChoice), xMid(p));
            if((yDiff < ySmallestDiff) && (xDiff<(yDiff/aSearchRange))) {
                ySmallestDiff = yDiff;
                p_nextChoice = p;
            }
        }
    }
    
    // If we didn't find a new, choice. Lets wrap around our search
    if(p_nextChoice == aSelectedChoice) {
        ySmallestDiff = UEZPlatform_LCDGetHeight();
        for (p = aChoiceList; p->iText; p++) {
            if(p != aSelectedChoice) {
                yDiff = yMid(aSelectedChoice);
                xDiff = absDiff(xMid(aSelectedChoice), xMid(p));
                if((yDiff < ySmallestDiff) && (xDiff<(yDiff/aSearchRange))) {
                    ySmallestDiff = yDiff;
                    p_nextChoice = p;
                }
            }
        }
    }
    
    return p_nextChoice;
}

/*---------------------------------------------------------------------------*
 * Routine:  ChoicesUpdateByReading
 *---------------------------------------------------------------------------*
 * Description:
 *      An input event has been received.  Check the event and change
 *      the state of the choices.
 * Inputs:
 *      SWIM_WINDOW_T *aWin         -- Window to draw within
 *      const T_choice *aChoices    -- Choices to use
 *      T_uezQueue aTouchQueue      -- Touchscreen queue
 *      TUInt32 aTimeout            -- Time to wait for touchscreen event
 * Outputs:
 *      const T_choice *            -- Last choice selected or 0 for none.
 *---------------------------------------------------------------------------*/
const T_choice *ChoicesUpdateByReading(
    SWIM_WINDOW_T *aWin,
    const T_choice *aChoices,
    T_uezInputEvent *p_inputEvent)
{
    INT_32 winX, winY;
    static TUInt16 lastX = 0, lastY = 0;
    static const T_choice *p_lastChoice = 0;
    static const T_choice *p_buttonChoice = 0;
    static const T_choice *p_nextButtonChoice = 0;
    const T_choice *p_choice = 0;
    const T_choice *p_choiceCalled = 0;

    winX = p_inputEvent->iEvent.iXY.iX;
    winY = p_inputEvent->iEvent.iXY.iY;
    swim_get_virtual_xy(aWin, &winX, &winY);

    // Is this a touching event?
    if(p_inputEvent->iType == INPUT_EVENT_TYPE_XY) {
        // Is this a press or hold event?
        if (p_inputEvent->iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD) {
            // We are touching the screen.
            // Is this a different position than before?
            if ((p_inputEvent->iEvent.iXY.iX != lastX) || (p_inputEvent->iEvent.iXY.iY != lastY)) {
                // Determine which choice we are in
                p_choice = IFindChoice(aChoices, winX, winY);
                if (p_choice != p_lastChoice) {
    #if HAPTIC_FEEDBACK              
                    hapticFeedback(DEFAULT_HAPTIC_TIME_MS);
    #endif                
                    if (p_lastChoice) {
                        // Un-invert the last choice 
                        swim_set_fill_color(aWin, G_settings.iUnselectColor);
                        swim_set_pen_color(aWin, G_settings.iUnselectColor);
                        swim_set_fill_transparent(aWin, 1);
                        swim_put_box(aWin, p_lastChoice->iLeft, p_lastChoice->iTop,
                            p_lastChoice->iRight, p_lastChoice->iBottom);
                        swim_set_fill_transparent(aWin, 0);
                    }
    
                    // Turn off button repeating when we leave focus
                    G_repeatOn = EFalse;
                    if (p_choice) {
                        // Invert the new choice
                        swim_set_pen_color(aWin, G_settings.iSelectColor);
                        swim_set_fill_transparent(aWin, 1);
                        swim_put_box(aWin, p_choice->iLeft, p_choice->iTop,
                            p_choice->iRight, p_choice->iBottom);
                        swim_set_fill_transparent(aWin, 0);
    
                        // If the option we are over is repeating,
                        // turn on repeating.
                        if (p_choice->iFlags & SUI_FLAG_REPEAT)
                            G_repeatOn = ETrue;
                    }
                    p_lastChoice = p_choice;
                    p_choice = 0;
                }
            }
    
            // Are we allowed to repeat?
            if (G_repeatOn) {
                if (!G_repeating) {
                    G_repeating = ETrue;
                    G_repeatLastTouch = UEZTickCounterGet();
                    G_repeatTime = REPEAT_TIME_1; // first repeat is slow
                } else {
                    // Time to repeat?
                    if (UEZTickCounterGetDelta(G_repeatLastTouch) >= G_repeatTime) {
                        if (G_repeatTime == REPEAT_TIME_1)
                            G_repeatTime = REPEAT_TIME_2;
                        else if (G_repeatTime == REPEAT_TIME_2)
                            G_repeatTime = REPEAT_TIME_3;
                        G_repeatLastTouch = UEZTickCounterGet();
    
                        // Cause a repeat by signalling a non-touch event below
                        //p_reading->iFlags &= ~TSFLAG_PEN_DOWN;
                    }
                }
            }
        } else {
            // Really not touching?
            // Are we repeating?
            if (G_repeating) {
                // Stop that.
                G_repeating = EFalse;
            }
        }
    
        // Release event (real for faked above?)
        if (p_inputEvent->iEvent.iXY.iAction == XY_ACTION_RELEASE) {
            // The screen is no longer being touched.
            // Determine which choice we are in
            p_choice = p_lastChoice;
            if (p_choice) {
                // Same as when we pressed
                // Do the action
                if (p_choice->iAction) {
                    const T_choice *p_prevChoice = p_lastChoice;
                    p_lastChoice = 0;
                    p_choiceCalled = p_choice;
                    // Un-invert the last choice
                    swim_set_fill_transparent(aWin, 1);
                    swim_set_pen_color(aWin, G_settings.iUnselectColor);
                    swim_put_box(aWin, p_prevChoice->iLeft, p_prevChoice->iTop,
                        p_prevChoice->iRight, p_prevChoice->iBottom);
                    swim_set_fill_transparent(aWin, 0);
    
                    p_choice->iAction(p_choice);
    
                    p_lastChoice = 0;
                }
            }
        }
    } else {
        
        
        // This is a button event
        if(p_inputEvent->iEvent.iButton.iAction == BUTTON_ACTION_PRESS) {
            
            if(p_buttonChoice == 0){
                p_buttonChoice = IFindChoiceTopLeft(aChoices);
            } else {
                // Un-invert the last choice 
                swim_set_fill_color(aWin, G_settings.iUnselectColor);
                swim_set_pen_color(aWin, G_settings.iUnselectColor);
                swim_set_fill_transparent(aWin, 1);
                swim_put_box(aWin, p_buttonChoice->iLeft, p_buttonChoice->iTop,
                    p_buttonChoice->iRight, p_buttonChoice->iBottom);
                swim_set_fill_transparent(aWin, 0);
                
                if(p_inputEvent->iEvent.iButton.iKey == KEY_ARROW_RIGHT) {
                    p_nextButtonChoice = IFindChoiceRight(aChoices, p_buttonChoice, 2);
                    if(p_nextButtonChoice == p_buttonChoice)
                        p_buttonChoice = IFindChoiceRight(aChoices, p_buttonChoice, 1);
                    else
                        p_buttonChoice = p_nextButtonChoice;
                } else if(p_inputEvent->iEvent.iButton.iKey == KEY_ARROW_LEFT) {
                    p_nextButtonChoice = IFindChoiceLeft(aChoices, p_buttonChoice, 2);
                    if(p_nextButtonChoice == p_buttonChoice)
                        p_buttonChoice = IFindChoiceLeft(aChoices, p_buttonChoice, 1);
                    else
                        p_buttonChoice = p_nextButtonChoice;
                } else if(p_inputEvent->iEvent.iButton.iKey == KEY_ARROW_UP) {
                    p_nextButtonChoice = IFindChoiceUp(aChoices, p_buttonChoice, 2);
                    if(p_nextButtonChoice == p_buttonChoice)
                        p_buttonChoice = IFindChoiceUp(aChoices, p_buttonChoice, 1);
                    else
                        p_buttonChoice = p_nextButtonChoice;
                } else if(p_inputEvent->iEvent.iButton.iKey == KEY_ARROW_DOWN) {
                    p_nextButtonChoice = IFindChoiceDown(aChoices, p_buttonChoice, 2);
                    if(p_nextButtonChoice == p_buttonChoice)
                        p_buttonChoice = IFindChoiceDown(aChoices, p_buttonChoice, 1);
                    else
                        p_buttonChoice = p_nextButtonChoice;
                } else if(p_inputEvent->iEvent.iButton.iKey == KEY_ENTER) {
                    p_choice = p_buttonChoice;
                    p_buttonChoice = 0;
                    p_choiceCalled = p_choice;
                    p_choice->iAction(p_choice);
                }

            }
            
            // Invert the new choice
            swim_set_pen_color(aWin, G_settings.iSelectColor);
            swim_set_fill_transparent(aWin, 1);
            swim_put_box(aWin, p_buttonChoice->iLeft, p_buttonChoice->iTop,
                p_buttonChoice->iRight, p_buttonChoice->iBottom);
            swim_set_fill_transparent(aWin, 0);
            
        } else /* BUTTON_ACTION_RELEASE */{
        }
    }
    return p_choiceCalled;
}

/*---------------------------------------------------------------------------*
 * Routine:  ChoicesUpdateContinuously
 *---------------------------------------------------------------------------*
 * Description:
 *      Wait for a touchscreen event or timeout.  Check the event and change
 *      the state of the choices.  Choices are chosen continuously as they
 *      are pressed reporting events every call.
 * Inputs:
 *      SWIM_WINDOW_T *aWin         -- Window to draw within
 *      const T_choice *aChoices    -- Choices to use
 *      T_uezQueue aTouchQueue      -- Touchscreen queue
 *      TUInt32 aTimeout            -- Time to wait for touchscreen event
 * Outputs:
 *      const T_choice *            -- Last choice selected or 0 for none.
 *---------------------------------------------------------------------------*/
const T_choice *ChoicesUpdateContinuously(
    SWIM_WINDOW_T *aWin,
    const T_choice *aChoices,
    T_uezQueue aInputEventQueue,
    TUInt32 aTimeout)
{
    T_uezInputEvent inputEvent;
    INT_32 winX, winY;
    static const T_choice *p_lastChoice = 0;
    const T_choice *p_choice = 0;
    const T_choice *p_choiceCalled = 0;

    if (UEZQueueReceive(aInputEventQueue, &inputEvent, aTimeout) == UEZ_ERROR_NONE) {
        winX = inputEvent.iEvent.iXY.iX;
        winY = inputEvent.iEvent.iXY.iY;
        swim_get_virtual_xy(aWin, &winX, &winY);

        // Is this a touching event?
        if (inputEvent.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD) {
            // We are touching the screen.
            // Is this a different position than before?
            // Determine which choice we are in
            p_choice = IFindChoice(aChoices, winX, winY);
            if (p_choice != p_lastChoice) {
                if (p_lastChoice) {
                    // Un-invert the last choice (draw black square)
                    swim_set_fill_color(aWin, G_settings.iUnselectColor);
                    swim_set_pen_color(aWin, G_settings.iUnselectColor);
                    swim_set_fill_transparent(aWin, 1);
                    swim_put_box(aWin, p_lastChoice->iLeft, p_lastChoice->iTop,
                        p_lastChoice->iRight, p_lastChoice->iBottom);
                    swim_set_fill_transparent(aWin, 0);
                }
                if (p_choice) {
                    // Invert the new choice
                    swim_set_pen_color(aWin, G_settings.iSelectColor);
                    swim_set_fill_transparent(aWin, 1);
                    swim_put_box(aWin, p_choice->iLeft, p_choice->iTop,
                        p_choice->iRight, p_choice->iBottom);
                    swim_set_fill_transparent(aWin, 0);
                }
                p_lastChoice = p_choice;
                p_choice = 0;
            }
            // Act like we just released the same spot
            // Determine which choice we are in
            p_choice = p_lastChoice;
            if (p_choice) {
                // Same as when we pressed
                // Do the action
                if (p_choice->iAction) {
                    const T_choice *p_prevChoice = p_lastChoice;
                    p_lastChoice = 0;
//                    ButtonClick();
                    p_choiceCalled = p_choice;
                    p_choice->iAction(p_choice);

                    // Un-invert the last choice
                    swim_set_fill_transparent(aWin, 1);
                    swim_set_pen_color(aWin, G_settings.iUnselectColor);
                    swim_put_box(aWin, p_prevChoice->iLeft, p_prevChoice->iTop,
                        p_prevChoice->iRight, p_prevChoice->iBottom);
                    swim_set_fill_transparent(aWin, 0);

                    p_lastChoice = 0;
                }
            }
        } else {
            // The screen is no longer being touched.
            // Nothing being touched
            p_lastChoice = 0;
            return 0;
        }
    } else {
        return 0;
    }
    return p_choiceCalled;
}

/*---------------------------------------------------------------------------*
 * Routine:  ChoicesSetSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Change the settings of the Choices system.  This change takes
 *      affect immediately and is global.
 * Inputs:
 *      T_choiceSettings *aSettings -- New settings
 *---------------------------------------------------------------------------*/
void ChoicesSetSettings(T_choicesSettings *aSettings)
{
    G_settings = *aSettings;
}

/*---------------------------------------------------------------------------*
 * Routine:  ChoicesGetSettings
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the global settings of the Choices system.
 * Inputs:
 *      T_choiceSettings *aSettings -- Place to store current settings.
 *---------------------------------------------------------------------------*/
void ChoicesGetSettings(T_choicesSettings *aSettings)
{
    *aSettings = G_settings;
}

/*-------------------------------------------------------------------------*
 * End of File:  SimpleUI_Choices.c
 *-------------------------------------------------------------------------*/
