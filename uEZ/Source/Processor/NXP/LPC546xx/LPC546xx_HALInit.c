/*-------------------------------------------------------------------------*
 * File:  HALInit.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the HAL processor specific interfaces.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZProcessor.h>

unsigned int G_LPC546xx_powerSetting[4];

void uEZProcessorServicesInit(void)
{
    // Read the default PCONP setting, but from this point
    // on, use LPC546xxPowerOn() and LPC546xxPowerOff()
    G_LPC546xx_powerSetting[0] = SYSCON->AHBCLKCTRLSET[0];
    G_LPC546xx_powerSetting[1] = SYSCON->AHBCLKCTRLSET[1];
    G_LPC546xx_powerSetting[2] = SYSCON->AHBCLKCTRLSET[2];
    SYSCON->ASYNCAPBCTRL = SYSCON_ASYNCAPBCTRL_ENABLE(1);
    G_LPC546xx_powerSetting[3] = ASYNC_SYSCON->ASYNCAPBCLKCTRLSET;
}

/*-------------------------------------------------------------------------*
 * End of File:  HALInit.c
 *-------------------------------------------------------------------------*/
