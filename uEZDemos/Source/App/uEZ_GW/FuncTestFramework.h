/*-------------------------------------------------------------------------*
 * File:  FuncTestFramework.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test framework routines
 *-------------------------------------------------------------------------*/
#ifndef _FUNC_TEST_FRAMEWORK_H_
#define _FUNC_TEST_FRAMEWORK_H_

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
#include <uEZ.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define OPT_BUTTON_SKIP             0x0001
#define OPT_BUTTON_YES              0x0002
#define OPT_BUTTON_NO               0x0004
#define OPT_BUTTON_CANCEL           0x0008
#define OPT_BUTTON_OK               0x0010
#define OPT_INIT                    0x8000

#define TEST_RESULT_IN_PROGRESS          0x0000
#define TEST_RESULT_DONE                 0x0001
#define TEST_RESULT_PASS                 0x0002
#define TEST_RESULT_FAIL                 0x0003
#define TEST_RESULT_SKIP                 0x0004
#define TEST_RESULT_IGNORE               0x0005

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct {
    SWIM_WINDOW_T iWin;
    TUInt16 iButtons;
    TUInt16 iResult;
    TUInt8 iData[100];
    TBool iGoNext;
    TUInt16 iLine;
    TBool iAllPass;
    TBool iPauseComplete;
    TUInt32 iParams[10];
    TUInt16 iNumParams;
    char iResultValue[50];
    void *iPrivateData;
} T_testData;

typedef struct {
    void (*iAddButtons)(T_testData *aData, TUInt16 aButtonTypes);
    void (*iRemoveButtons)(T_testData *aData, TUInt16 aButtonTypes);
    void (*iTextLine)(T_testData *aData, TUInt16 aLine, const char *aText);
    void (*iShowResult)(
            T_testData *aData,
            TUInt16 aLine,
            TUInt16 aResult,
            TUInt32 aDelay);
    void (*iSetTestResult)(T_testData *aData, TUInt16 aResult);
    void (*iNextTest)(T_testData *aData);
} T_testAPI;

typedef struct {
    const char *iTitle;
    void (*iMonitorFunction)(
            const T_testAPI *aAPI,
            T_testData *aData,
            TUInt16 aButton);
    TUInt16 iLinesNeeded;
    TBool iPauseOnFail;
    TBool iIgnoreIfExpansionBoard;
    TBool iIgnoreIfNoLoopbackBoard;
} T_testState;

#ifdef __cplusplus
}
#endif

#endif // _FUNC_TEST_FRAMEWORK_H_
/*-------------------------------------------------------------------------*
 * End of File:  FuncTestFramework.h
 *-------------------------------------------------------------------------*/
