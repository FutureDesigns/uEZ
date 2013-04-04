/*-------------------------------------------------------------------------*
 * File:  RX62N_UtilityFuncs.h
 *-------------------------------------------------------------------------*
 * Description:
 *      
 *-------------------------------------------------------------------------*/
 
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
#include <uEZGPIO.h>

void UEZFailureMsg(const char *aLine);
void RX62N_PinsLock(const T_uezGPIOPortPin *aPins, TUInt8 aCount);
  
 /*-------------------------------------------------------------------------*
 * End of File:  RX62N_UtilityFuncs.h
 *-------------------------------------------------------------------------*/