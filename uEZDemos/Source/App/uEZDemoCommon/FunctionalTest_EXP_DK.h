/*-------------------------------------------------------------------------*
 * File:  FunctionalTest_EXP_DK.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test procedure for uEZGUI-EXP-DK
 *
 * Implementation:
 *-------------------------------------------------------------------------*/
#ifndef _FUNCTIONAL_TEST_EXP_DK_H_
#define _FUNCTIONAL_TEST_EXP_DK_H_

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
#include "FuncTestFramework.h"
#include <HAL/GPIO.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _FUNCTIONAL_TEST_EXP_DK_H_
/*-------------------------------------------------------------------------*
 * End of File:  FunctionalTest_EXP_DK.h
 *-------------------------------------------------------------------------*/
