/*-------------------------------------------------------------------------*
* File:  AppDemo.h
*--------------------------------------------------------------------------*
* Description:
*         <DESCRIPTION>
*-------------------------------------------------------------------------*/
#ifndef _APP_DEMO_H_
#define _APP_DEMO_H_

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
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Source/Library/Graphics/SWIM/lpc_swim_font.h>
#include <uEZProcessor.h>
#include <uEZDemoCommon/UDCImages.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ICON_TEXT_COLOR         YELLOW
#define SELECT_ICON_COLOR       YELLOW

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void TestSWIM(void);
const T_choice *IFindChoice(const T_choice *aChoiceList, TUInt16 aX, TUInt16 aY);
void Beep(void);
void CalibrateScreen(void);
void MainMenuScreen(void);
void PlayAudio(TUInt32 aHz, TUInt32 aMS);
void ButtonClick(void);
void BeepError(void);
void PutPointRaw(TUInt8 aPage, int x, int y, TUInt16 pixel);
void PutPointRaw2x2(TUInt8 aPage, int x, int y, TUInt16 pixel);
void DrawIcon(const TUInt8 *p, TUInt16 aXOffset, TUInt16 aYOffset);
void IHidePage0(void);
void IShowPage0(void);
void TitleScreen(void);
void PWMTestAudio(TUInt32 freq, TUInt32 onTime, TUInt32 offTime);
void ChoicesDraw(SWIM_WINDOW_T *aWin, const T_choice *aChoices);
void TimeTempDemoMode(const T_choice *aChoice);
const T_choice *ChoicesUpdate(
        SWIM_WINDOW_T *aWin,
        const T_choice *aChoices,
        T_uezQueue aTouchQueue,
        TUInt32 aTimeout);
void SettingsMenu(const T_choice *aChoice);
void SlideshowMode(const T_choice *p_choice);
void DrawMode(const T_choice *p_choice);
void ConsoleMode(const T_choice *p_choice);
void CalibrateMode(const T_choice *p_choice);
void FunctionalTest(const T_choice *aChoice);
void AccelDemoMode(const T_choice *aChoice);
void MainMenu(void);
void  CANSend8(unsigned int CANTXByte1, unsigned int CANTXByte2);
int CANReceive8(unsigned int *aCANByteA, unsigned int *aCANByteB);
void FunctionalTestLoop(const T_choice *aChoice);
#if APP_DEMO_EMWIN
void emWin(const T_choice *aChoice);
#endif

extern volatile TBool G_mmTestMode;
extern volatile TBool G_mmTestModeRunning;
#define TEST_MODE_END               0
#define TEST_MODE_PING              1
#define TEST_MODE_LCD               2
#define TEST_MODE_TOUCHSCREEN       3
#define TEST_MODE_FILL_COLOR        4
#define TEST_MODE_ALIGNMENT_BORDER  5
extern volatile TBool G_mmTestModeTouchscreenCalibrationBusy;
extern volatile TBool G_mmTestModeTouchscreenCalibrationValid;
void TestMode(void);
TBool TestModeSendCmd(TUInt32 aCmd);
void TestModeGetWindow(SWIM_WINDOW_T *aWin);

#define TP_INSET        50

#ifdef __cplusplus
}
#endif

#endif // _APP_DEMO_H_
