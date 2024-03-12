/*-------------------------------------------------------------------------*
 * File:  uEZPlatformAPI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZPLATFORMAPI_H_
#define UEZPLATFORMAPI_H_

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
/**
 *  @file   uEZPlatformAPI.h
 *  @brief  uEZ Platform API
 *
 *  uEZ Platform API
 */
/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZNetwork.h>
#include <uEZRTOS.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
/** Initialize any part of the platform before the RTOS starts */
void uEZPlatformInit(void);

/** Startup the platform (first main task).  Use this to require other parts */
TUInt32 uEZPlatformStartup(T_uezTask aMyTask, void *aParameters);

// Delays
void UEZBSPDelay1MS(void);
void UEZBSPDelayMS(TUInt32 aMilliseconds);
void UEZBSPDelay1US(void);
void UEZBSPDelayUS(TUInt32 aMicroseconds);

// CPU
TUInt32 UEZPlatform_ProcessorGetFrequency(void);
TUInt32 UEZPlatform_GetPCLKFrequency(void);

// LCD
TUInt16 UEZPlatform_LCDGetHeight(void);
TUInt16 UEZPlatform_LCDGetWidth(void);
TUInt32 UEZPlatform_LCDGetFrame(TUInt16 aFrameNum);
TUInt32 UEZPlatform_LCDGetFrameBuffer(void);
TUInt32 UEZPlatform_LCDGetFrameSize(void);
TUInt32 UEZPlatform_GetBaseAddress(void);
const void* UEZPlatform_GUIColorConversion(void);
const void* UEZPlatform_GUIDisplayDriver(void);

// Serial
TUInt32 UEZPlatform_SerialGetDefaultBaud(void);

// MCI/SD card
TUInt32 UEZPlatform_MCI_DefaultFreq(void);
TUInt32 UEZPlatform_MCI_TransferMode(void);

// Monitoring
TUInt32 UEZPlatform_5V_Monitor_Get_Raw_Reading(void);

// Expansion
TBool UEZPlatform_ExpansionBoardIsConnected(void);

// Memory Test step callbacks
void UEZPlatform_MemTest_StepA(void);
void UEZPlatform_MemTest_StepA_Pass(void);
void UEZPlatform_MemTest_StepB(void);
void UEZPlatform_MemTest_StepB_Pass(void);
void UEZPlatform_MemTest_StepC(void);
void UEZPlatform_MemTest_StepC_Pass(void);
void UEZPlatform_MemTest_StepD(void);
void UEZPlatform_MemTest_StepD_Pass(void);
void UEZPlatform_MemTest_StepE(void);
void UEZPlatform_MemTest_StepE_Pass(void);
void UEZPlatform_MemTest_StepF(void);
void UEZPlatform_MemTest_StepF_Pass(void);

// Flash programming
void UEZPlatform_IAP_Require(void);

// Reset
void UEZPlatform_System_Reset(void);

#ifdef __cplusplus
}
#endif

#endif // UEZPLATFORMAPI_H_
/*-------------------------------------------------------------------------*
 * End of File:  UEZPlatformAPI.h
 *-------------------------------------------------------------------------*/
