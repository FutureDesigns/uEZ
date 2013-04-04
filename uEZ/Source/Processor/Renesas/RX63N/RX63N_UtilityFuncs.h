/*-------------------------------------------------------------------------*
 * File:  RX63N_UtilityFuncs.h
 *-------------------------------------------------------------------------*
 * Description:
 *      
 *-------------------------------------------------------------------------*/
#ifndef _RX63N_UTILTY_FUNCS_H_
#define _RX63N_UTILTY_FUNCS_H_

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
#include <uEZ.h>
#include <uEZGPIO.h>

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        T_uezGPIOPortPin iPortPin;
        TUInt32 iPFSSetting;
} T_RX63N_MPC_ConfigList;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void RX63N_PinsLock(const T_uezGPIOPortPin *aPins, TUInt8 aCount);
void RX63N_UnlockProtectionBits();
void RX63N_LockProtectionBits();

void RX63N_MPC_ConfigAllPins(
        const T_RX63N_MPC_ConfigList *aList,
        TUInt8 aCount);
void RX63N_MPC_ConfigPin(
        T_uezGPIOPortPin aPortPin,
        const T_RX63N_MPC_ConfigList *aList,
        TUInt8 aCount);
void RX63N_MPC_ConfigPinOrNone(
        T_uezGPIOPortPin aPortPin,
        const T_RX63N_MPC_ConfigList *aList,
        TUInt8 aCount);

#endif // _RX63N_UTILTY_FUNCS_H_
 /*-------------------------------------------------------------------------*
 * End of File:  RX63N_UtilityFuncs.h
 *-------------------------------------------------------------------------*/