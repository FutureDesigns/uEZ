/*-------------------------------------------------------------------------*
 * File:  TimeDate_Callbacks.c
 *-------------------------------------------------------------------------*
 * Description:
 *     The callbacks are designed to make a disconnect from the GUI
 *     and allow for more flexibility in how the data is updated.
 *     
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <TimeDate_Callbacks.h>
#include <stdio.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_uezSemaphore G_Sem;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

static void IGrab(void)
{
    UEZSemaphoreGrab(G_Sem, UEZ_TIMEOUT_INFINITE);
}

static void IRelease(void)
{
    UEZSemaphoreRelease(G_Sem);
}

char G_DateMessage[25] = {0}; // Global Variable for uC/Probe Demo
char G_TimeMessage[25] = {0}; // Global Variable for uC/Probe Demo
/*-------------------------------------------------------------------------*
 * Gets the time and date from the internal RTC
 *-------------------------------------------------------------------------*/
void TimeDate_GetSettings(T_TimeDateSettings *aSettings)
{
    IGrab();
    UEZTimeDateGet(&aSettings->iTimeDate);
    IRelease();
    
	// For uC/Probe Demo
    sprintf(G_TimeMessage, "%02d:%02d:%02d",  aSettings->iTimeDate.iTime.iHour,
                                            aSettings->iTimeDate.iTime.iMinute,
                                            aSettings->iTimeDate.iTime.iSecond);
    // For uC/Probe Demo               
    sprintf(G_DateMessage, "%02d/%02d/%04d",  aSettings->iTimeDate.iDate.iMonth,
                                            aSettings->iTimeDate.iDate.iDay,
                                            aSettings->iTimeDate.iDate.iYear);    
}

/*-------------------------------------------------------------------------*
 * Sets the time and date for the Internal RTC
 *-------------------------------------------------------------------------*/
void TimeDate_SetSettings(T_TimeDateSettings *aSettings)
{
    IGrab();
    UEZTimeDateSet(&aSettings->iTimeDate);
    IRelease();
}

//Initialize
void TimeDate_Initialize()
{
    static TBool haveRun = EFalse;

    if(!haveRun)
        UEZSemaphoreCreateBinary(&G_Sem);
}

/*-------------------------------------------------------------------------*
 * End of File:  TimeDate_Callbacks.c
 *-------------------------------------------------------------------------*/
