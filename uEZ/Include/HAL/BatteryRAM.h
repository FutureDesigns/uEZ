/*-------------------------------------------------------------------------*
* File:  HAL/BatteryRAM.h
*-------------------------------------------------------------------------*
* Description:
*     Battery RAM HAL interface
*-------------------------------------------------------------------------*/
#ifndef BATTERYRAM_H_
#define BATTERYRAM_H_

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
 *  @file   /Include/HAL/BatteryRAM.h
 *  @brief  uEZ Battery RAM HAL Interface
 *
 *  The uEZ Battery RAM HAL Interface
 *
 *  Example:
 *  @code
 *      TODO: Write code here
 *  @endcode
 */
/*-------------------------------------------------------------------------*
* Includes:
*-------------------------------------------------------------------------*/
#include <uEZTypes.h>
#include <HAL/HAL.h>

/*-------------------------------------------------------------------------*
* Types:
*-------------------------------------------------------------------------*/
typedef struct {
    // Header
    T_halInterface iInterface;

    // Functions
    T_uezError (*Read)(void *aWorkspace, const TUInt32 aOffset, TUInt8 *aData);
    T_uezError (*Write)(void *aWorkspace, const TUInt32 aOffset, const TUInt8 aData);
    TBool (*IsValid)(void *aWorkspace);
    TUInt32 (*GetSize)(void *aWorkspace);
} HAL_BatteryRAM;

#endif /* BATTERYRAM_H_ */
