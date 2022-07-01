/*-------------------------------------------------------------------------*
 * File:  uEZRTOS.c
 *-------------------------------------------------------------------------*
 * Description:
 *     uEZ Operation System Abstraction Layer
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include "Config.h"
#include "uEZRTOS.h"

#if (RTOS==FreeRTOS)
#include "uEZRTOS_FreeRTOS.c"
#elif (RTOS==SafeRTOS)
#include "uEZRTOS_SafeRTOS.c"
#endif

#include <string.h>
#include <stdio.h>

#define MAX_UEZ_TASK_LIST   10

#ifndef configMAX_TASK_NAME_LEN
#define configMAX_TASK_NAME_LEN   16
#endif

typedef struct {
        char iName[configMAX_TASK_NAME_LEN+1];
        T_uezPriority iPriority;
        TUInt32 iStackSize;
}T_uezTaskList;

static T_uezTaskList G_uezTaskList[MAX_UEZ_TASK_LIST];
static T_uezSemaphore G_TaskListSem = 0;

void UEZTaskRegister(const char * const aName, T_uezPriority aPriority, TUInt32 aStackSize)
{
    static TUInt32 task = 0;

    if ( G_TaskListSem == 0){
        UEZSemaphoreCreateBinary(&G_TaskListSem);
    }

    UEZSemaphoreGrab(G_TaskListSem, UEZ_TIMEOUT_INFINITE);
    if ( task == 0 ){
        memset((void*)G_uezTaskList, 0, sizeof(T_uezTaskList) * MAX_UEZ_TASK_LIST);
    }

    if( task < MAX_UEZ_TASK_LIST){
        memcpy((void*)G_uezTaskList[task].iName, (void*)aName, configMAX_TASK_NAME_LEN);
        G_uezTaskList[task].iStackSize = aStackSize;
        G_uezTaskList[task].iPriority = aPriority;
        task++;
    }
    UEZSemaphoreRelease(G_TaskListSem);
}

#define MAX_TASK_LINE   75
void UEZGetTaskList(char* aBuffer)
{
    TUInt8 i = 0;
    char task[MAX_TASK_LINE];

    UEZSemaphoreGrab(G_TaskListSem, UEZ_TIMEOUT_INFINITE);
    for ( i = 0; i < MAX_UEZ_TASK_LIST; i++){
        if ( G_uezTaskList[i].iPriority == 0){
            break;
        }
        sprintf(task, "   %01d        %07d       %s\n",
                G_uezTaskList[i].iPriority,
                G_uezTaskList[i].iStackSize,
                G_uezTaskList[i].iName );
        strcat(aBuffer, task);
    }
    UEZSemaphoreRelease(G_TaskListSem);
}
/*-------------------------------------------------------------------------*
 * End of File:  uEZRTOS.c
 *-------------------------------------------------------------------------*/
