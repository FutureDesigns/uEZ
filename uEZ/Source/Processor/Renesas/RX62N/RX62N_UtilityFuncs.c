/*-------------------------------------------------------------------------*
 * File:  RX62N_UtilityFuncs.c
 *-------------------------------------------------------------------------*
 * Description:
 *      
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include "RX62N_UtilityFuncs.h"

void RX62N_PinsLock(
        const T_uezGPIOPortPin *aPins,
        TUInt8 aCount)
{
    while (aCount--) {
        UEZGPIOLock(aPins[aCount]);
    }
}
 
 /*-------------------------------------------------------------------------*
 * End of File:  RX62N_UtilityFuncs.c
 *-------------------------------------------------------------------------*/