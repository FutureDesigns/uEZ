/*-------------------------------------------------------------------------*
 * File:  FuncTestFramework.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test framework routines
 *-------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
 * This software and associated documentation files (the "Software")
 * are copyright 2008 Future Designs, Inc. All Rights Reserved.
 *
 * A copyright license is hereby granted for redistribution and use of
 * the Software in source and binary forms, with or without modification,
 * provided that the following conditions are met:
 * 	-   Redistributions of source code must retain the above copyright
 *      notice, this copyright license and the following disclaimer.
 * 	-   Redistributions in binary form must reproduce the above copyright
 *      notice, this copyright license and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 * 	-   Neither the name of Future Designs, Inc. nor the names of its
 *      subsidiaries may be used to endorse or promote products
 *      derived from the Software without specific prior written permission.
 *
 * 	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * 	CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * 	INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * 	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * 	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * 	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * 	SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * 	NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * 	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * 	HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * 	CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * 	OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * 	EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *-------------------------------------------------------------------------*/
#ifndef _FUNC_TEST_FRAMEWORK_H_
#define _FUNC_TEST_FRAMEWORK_H_

#include <uEZ.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>

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

#endif // _FUNC_TEST_FRAMEWORK_H_
/*-------------------------------------------------------------------------*
 * End of File:  FuncTestFramework.h
 *-------------------------------------------------------------------------*/
