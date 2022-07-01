/*-------------------------------------------------------------------------*
 * File:  porttimer.c
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>

#include "port.h"
#include "mb.h"
#include "mbport.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static TUInt32 dwTimeOut;
static TBool bTimeoutEnable;
static TUInt32 dwTimeLast;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern BOOL xMBPortSerialSetTimeout( TUInt32 dwTimeoutMs );

BOOL xMBPortTimersInit(USHORT usTim1Timerout50us)
{
    return xMBPortSerialSetTimeout(usTim1Timerout50us);
}

void xMBPortTimersClose()
{
    /* Does not use any hardware resources. */
}

void vMBPortTimerPoll()
{
//  Timers are called from the serial layer
    if (bTimeoutEnable) {
        TUInt32 dwTimeCurrent = UEZTickCounterGet();

        if ((dwTimeCurrent - dwTimeLast) > dwTimeOut) {
            bTimeoutEnable = FALSE;
            (void)pxMBPortCBTimerExpired();
        }
    }
}

void vMBPortTimersEnable()
{
    bTimeoutEnable = TRUE;
    dwTimeLast = UEZTickCounterGet();
}

void vMBPortTimersDisable()
{
    bTimeoutEnable = FALSE;
}

/*-------------------------------------------------------------------------*
 * End of File:  porttimer.c
 *-------------------------------------------------------------------------*/
