/*-------------------------------------------------------------------------*
 * File:  HALInit.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the HAL processor specific interfaces.
 *-------------------------------------------------------------------------*/

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
#include <uEZ.h>
#include <uEZProcessor.h>

unsigned int G_lpc1788_powerSetting;

void uEZProcessorServicesInit(void)
{
    // Read the default PCONP setting, but from this point
    // on, use LPC1788PowerOn() and LPC1788PowerOff()
    G_lpc1788_powerSetting = LPC_SC->PCONP;
}

/*-------------------------------------------------------------------------*
 * End of File:  HALInit.c
 *-------------------------------------------------------------------------*/
