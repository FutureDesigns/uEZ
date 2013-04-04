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
#include <Config.h>
#include <uEZProcessor.h>
#include <uEZ.h>
#include <HAL/HAL.h>

void uEZProcessorServicesInit(void)
{
    // Setup all clock dividers to be /1 instead of /4 by default
    PCLKSEL0 = 0x11555455;
    PCLKSEL1 = 0x55555555;
}

/*-------------------------------------------------------------------------*
 * End of File:  HALInit.c
 *-------------------------------------------------------------------------*/
