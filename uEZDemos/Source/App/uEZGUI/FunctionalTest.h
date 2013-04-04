/*-------------------------------------------------------------------------*
 * File:  FunctionalTest.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Functional test procedure for ARM Carrier board with ARM7DIMM
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
#ifndef _FUNCTIONAL_TEST_H_
#define _FUNCTIONAL_TEST_H_

#include <uEZ.h>
#include "FuncTestFramework.h"
#include <HAL/GPIO.h>

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

HAL_GPIOPort **PortNumToPort(TUInt8 aPort);

#endif // _FUNCTIONAL_TEST_H_
/*-------------------------------------------------------------------------*
 * End of File:  FunctionalTest.h
 *-------------------------------------------------------------------------*/
