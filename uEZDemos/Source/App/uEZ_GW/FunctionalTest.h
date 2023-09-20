/*-------------------------------------------------------------------------*
 * File:  FunctionalTest.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test procedure for ARM Carrier board with ARM7DIMM
 *
 * Implementation:
 *-------------------------------------------------------------------------*/
#ifndef _FUNCTIONAL_TEST_H_
#define _FUNCTIONAL_TEST_H_

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
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>

#ifdef __cplusplus
extern "C" {
#endif
  
typedef unsigned long long TUInt64;

typedef struct {
    TUInt64 iPinsDrive;
    TUInt64 iPinsChange;
    const char *iMsg;
} T_gpioTestEntry;

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
void FuncTestSDRAM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestInternalRTC(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
void FuncTestEEPROM(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestTemperature(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
void FuncTestSerial(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestLCD(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestUSBHost1(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestUSBHost2(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestComplete(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestFlash0(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestFullFlash0(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
void FuncTestBacklightMonitor(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
void FuncTestBacklightPWM(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
void FuncTestVoltageMonitor(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
void FuncTestGPIOs(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestLoopbacks(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton, const T_gpioTestEntry *testArray, TUInt16 testArraySize);
void FuncTestXBEE(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestPMOD(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestEXP(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestMicroSDCard(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
#if LIGHT_SENSOR_ENABLED
void FuncTestLightSensor(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
#endif
void FuncTestAccelerometer(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
void FuncTestAccelerometerFast(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
void FuncTestSpeaker(const T_testAPI *aAPI, T_testData *aData, TUInt16 aButton);
void FuncTestExternalRTC(
        const T_testAPI *aAPI,
        T_testData *aData,
        TUInt16 aButton);
void FuncTestEXP1(void);

// common functions
void TestTextLine(T_testData *aData, TUInt16 aLine, const char *aText);
void TestShowResult(T_testData *aData, TUInt16 aLine, TUInt16 aResult, TUInt32 aDelay);
void TestSetTestResult(T_testData *aData, TUInt16 aPass);
void TestNext(T_testData *aData);
void TestRemoveButtons(T_testData *aData, TUInt16 aButtonTypes);
void TestAddButtons(T_testData *aData, TUInt16 aButtonTypes);
void TestButtonClick(T_testData *aData, TUInt16 aButton);
void TestDrawButtons(T_testData *aData, TUInt16 aButtons, T_pixelColor aBackground);
HAL_GPIOPort **PortNumToPort(TUInt8 aPort);

#ifdef __cplusplus
}
#endif

#endif // _FUNCTIONAL_TEST_H_
/*-------------------------------------------------------------------------*
 * End of File:  FunctionalTest.h
 *-------------------------------------------------------------------------*/
