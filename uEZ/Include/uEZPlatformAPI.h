/*-------------------------------------------------------------------------*
 * File:  UEZPlatformAPI.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZPLATFORMAPI_H_
#define UEZPLATFORMAPI_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

TUInt16 UEZPlatform_LCDGetHeight(void);
TUInt16 UEZPlatform_LCDGetWidth(void);
TUInt32 UEZPlatform_LCDGetFrame(TUInt16 aFrameNum);
TUInt32 UEZPlatform_LCDGetFrameBuffer(void);
TUInt32 UEZPlatform_LCDGetFrameSize(void);
TUInt32 UEZPlatform_SerialGetDefaultBaud(void);
TUInt32 UEZPlatform_ProcessorGetFrequency(void);
TUInt32 UEZPlatform_GetPCLKFrequency(void);
TUInt32 UEZPlatform_GetBaseAddress(void);
const void* UEZPlatform_GUIColorConversion(void);

TBool UEZPlatform_ExpansionBoardIsConnected(void);


#endif // UEZPLATFORMAPI_H_
/*-------------------------------------------------------------------------*
 * End of File:  UEZPlatformAPI.h
 *-------------------------------------------------------------------------*/
