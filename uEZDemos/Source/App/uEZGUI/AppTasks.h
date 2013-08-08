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

T_uezError SetupTasks(void);
TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams);

#endif // _APP_TASKS_H_
/*-------------------------------------------------------------------------*
 * End of File:  AppTasks.h
 *-------------------------------------------------------------------------*/
