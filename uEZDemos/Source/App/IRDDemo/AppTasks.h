/*-------------------------------------------------------------------------*
 * File:  AppTasks.h
 *-------------------------------------------------------------------------*
 * Description:
 *      Various tasks to possibly run.  Examples if nothing else.
 *-------------------------------------------------------------------------*/
#ifndef _APP_TASKS_H_
#define _APP_TASKS_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/
#include <uEZ.h>

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // _APP_TASKS_H_

/*-------------------------------------------------------------------------*
 * End of File:  AppTasks.h
 *-------------------------------------------------------------------------*/
