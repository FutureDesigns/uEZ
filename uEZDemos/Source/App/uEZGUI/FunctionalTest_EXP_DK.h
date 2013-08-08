/*-------------------------------------------------------------------------*
 * File:  FunctionalTest_EXP_DK.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test procedure for uEZGUI-EXP-DK
 *
 * Implementation:
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
#ifndef _FUNCTIONAL_TEST_EXP_DK_H_
#define _FUNCTIONAL_TEST_EXP_DK_H_

#include <uEZ.h>
#include "FuncTestFramework.h"
#include <HAL/GPIO.h>

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void EXP_DK_FuncTestRS232(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestRS485(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestPotentiometer(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestUSBHostA(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestUSBHostB(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestEthernet(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestFET(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestCAN(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestPMOD(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestMicroSDCard4bit(const T_testAPI *aAPI,T_testData *aData,TUInt16 aButton);
void EXP_DK_FuncTestI2C_C_D(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestALS(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestButtonLED(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestAudio(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestHeadphone(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestSpeaker(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestDALI(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void EXP_DK_FuncTestComplete(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);

HAL_GPIOPort **PortNumToPort(TUInt8 aPort);

#endif // _FUNCTIONAL_TEST_EXP_DK_H_
/*-------------------------------------------------------------------------*
 * End of File:  FunctionalTest_EXP_DK.h
 *-------------------------------------------------------------------------*/
