/*-------------------------------------------------------------------------*
 * File:  AppTasks.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Various tasks to possibly run.  Examples if nothing else.
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

#ifndef _APP_TASKS_H_
#define _APP_TASKS_H_

#include <uEZ.h>

T_uezError Main_ShowDirectory(void);
T_uezError Main_ShowHTTPRootDirectory(void);
TUInt32 MonDir(T_uezTask aMyTask, void *aParams);
TUInt32 MonAccl(T_uezTask aMyTask, void *aParams);
TUInt32 MonTemp(T_uezTask aMyTask, void *aParams);
TUInt32 MonLEDs(T_uezTask aMyTask, void *aParams);
void MonLEDsStart(void);
TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams);
TUInt32 WatchGenericBulk(T_uezTask aMyTask, void *aParams);
T_uezError ReadButtons(TUInt32 *aButtons);

typedef struct {
    TUInt8 iEvent;
        #define MON_LED_EVENT_OFF           0
        #define MON_LED_EVENT_ON            1
    TUInt8 iLEDIndex;
} T_monLedsMsg;
T_uezError MonLEDEvent(TUInt8 aEvent, TUInt8 aLEDIndex);

#endif // _APP_TASKS_H_

/*-------------------------------------------------------------------------*
 * End of File:  AppTasks.h
 *-------------------------------------------------------------------------*/
