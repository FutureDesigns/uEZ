/*-------------------------------------------------------------------------*
 * File:  uEZBSP.h
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Device Interface
 *-------------------------------------------------------------------------*/
#ifndef _UEZ_BSP_H_
#define _UEZ_BSP_H_

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
/**
 *  @file   uEZBSP.h
 *  @brief  uEZ BSP
 *
 *  uEZ BSP
 */
#include "Config.h"
#include "uEZTypes.h"

void UEZBSP_Startup(void);
void UEZBSP_PLLConfigure(void);
void UEZBSP_CPU_PinConfigInit(void);
void UEZBSP_InterruptsReset(void);
void UEZBSP_RAMInit(void);
void UEZBSP_ROMInit(void);
void UEZBSP_FatalError(int aNumBlinks);

/** Utility routine to delay a number of ms (approximately). */
void UEZBSPDelayMS(unsigned int aMilliseconds);
void UEZBSPDelayUS(unsigned int aMicroseconds);

void uEZProcessorServicesInit(void);
void uEZPlatformInit(void);

#endif //_UEZ_BSP_H_

/*-------------------------------------------------------------------------*
 * File:  uEZBSP.h
 *-------------------------------------------------------------------------*/

