/*
 FreeRTOS.org V4.7.2 - Copyright (C) 2003-2008 Richard Barry.

 This file is part of the FreeRTOS.org distribution.

 FreeRTOS.org is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 FreeRTOS.org is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with FreeRTOS.org; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 A special exception to the GPL can be applied should you wish to distribute
 a combined work that includes FreeRTOS.org, without being obliged to provide
 the source code for any proprietary components.  See the licensing section
 of http://www.FreeRTOS.org for full details of how and when the exception
 can be applied.

 ***************************************************************************

 Please ensure to read the configuration and relevant port sections of the
 online documentation.

 +++ http://www.FreeRTOS.org +++
 Documentation, latest information, license and contact details.

 +++ http://www.SafeRTOS.com +++
 A version that is certified for use in safety critical systems.

 +++ http://www.OpenRTOS.com +++
 Commercial support, development, porting, licensing and training services.

 ***************************************************************************
 */
/**
 *  @file   BasicWEB.c
 *  @brief  uEZ BasicWEB
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZ License.pdf
 *
 *  uEZ BasicWEB, includes task and start functions
 */
/*
 Implements a simplistic WEB server.  Every time a connection is made and
 data is received a dynamic page that shows the current TCP/IP statistics
 is generated and returned.  The connection is then closed.

 This file was adapted from a FreeRTOS lwIP slip demo supplied by a third
 party.
 */

/*
 Changes from V3.2.2

 + Changed the page returned by the lwIP WEB server demo to display the
 task status table rather than the TCP/IP statistics.
 */

/* Standard includes. */
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include <uEZ.h>
#include <uEZPlatformAPI.h>
#include <uEZNetwork.h>

#if (RTOS==FreeRTOS)
#include <Source/RTOS/FreeRTOS/include/task.h>
#elif ( RTOS == SafeRTOS )
#include "SafeRTOS_API.h"
#endif

#include "BasicWEB.h"

#include "lwip/sockets.h"

/* The size of the buffer in which the dynamic WEB page is created. */
#define webMAX_PAGE_SIZE    1460  // shouldn't go above 1435 with 15 full task lines printed

/* Standard GET response. */
#define webHTTP_OK  "HTTP/1.0 200 OK\r\nContent-type: text/html\r\n\r\n"

/* The port on which we listen. */
#define webHTTP_PORT        ( 81 )

/* Delay on close error. */
#define webSHORT_DELAY      ( 10 )

/* Format of the dynamic page that is returned on each connection. */
#define webHTML_START \
"<html>\
<head>\
</head>\
<BODY onLoad=\"window.setTimeout(&quot;location.href='index.html'&quot;,3000)\"bgcolor=\"#CCCCff\">\
\r\nPage Hits = "

#define webHTML_END \
"\n</pre>\
\r\n</BODY>\
</html>"

extern void FuncTestPageHit(void);

#if (configUSE_TRACE_FACILITY == 1)
#include <task.h>
void UEZGetTaskInfo(char* aBuffer, char* aLineBuffer);
#endif

T_uezTask G_BasicWebTask = NULL;

/*---------------------------------------------------------------------------*
 * Routine:  IBasicWebProcessConnection
 *---------------------------------------------------------------------------*/
/**
 *  Process connection to web server
 *
 *  @param [in] aNetwork		network device to use
 *
 *  @param [in] aSocket			uEZ network socket
 */
/*---------------------------------------------------------------------------*/
static void IBasicWebProcessConnection(
        T_uezDevice aNetwork,
        T_uezNetworkSocket aSocket)
{
    static char cDynamicPage[webMAX_PAGE_SIZE], cPageHits[11];
    static TUInt32 ulPageHits = 0;
    static char receiveBuffer[768];
#if (configUSE_TRACE_FACILITY == 1)
    static char lineBuffer[128]; // only see about 80-90 per line max right now.
#else
#endif
    TUInt32 receiveLength;
    T_uezError error;

    /* We expect to immediately get data. */
    error = UEZNetworkSocketRead(aNetwork, aSocket, receiveBuffer,
            sizeof(receiveBuffer), &receiveLength, 500);

    if ((error == UEZ_ERROR_NONE) || (error == UEZ_ERROR_TIMEOUT)) {
        /* Is this a GET?  We don't handle anything else. */
        if (!strncmp(receiveBuffer, "GET", 3)) {
            /* Update the hit count. */
            ulPageHits++;
            sprintf(cPageHits, "%u", ulPageHits);
            FuncTestPageHit();

            /* Write out the HTTP OK header. */
            UEZNetworkSocketWrite(aNetwork, aSocket, webHTTP_OK, strlen(
                    webHTTP_OK), ETrue, 500);

            /* Generate the dynamic page...

             ... First the page header. */
            strcpy(cDynamicPage, webHTML_START );
            /* ... Then the hit count... */
            strcat(cDynamicPage, cPageHits);

#if (configUSE_TRACE_FACILITY == 1)     
  #if ((FREERTOS_HEAP_SELECTION==4) | (FREERTOS_HEAP_SELECTION==5)) // Add heap free display before task list
        sprintf(lineBuffer, "<p><pre><b>Heap</b><br>Free: %08u, Minimum Ever Free: %08u<br>", (uint32_t) xPortGetFreeHeapSize(), (uint32_t) xPortGetMinimumEverFreeHeapSize());
        strcat(cDynamicPage, lineBuffer);
  #endif
        strcat(cDynamicPage,
#if (configGENERATE_RUN_TIME_STATS == 1)
"<p><pre><b>\
Task Name  State     Priority      Free Stack Remaining\tTimes Run  Run Percentage<br>\
**********************************************************************************<br></b>");
#else
"<p><pre><b>\
Task Name  State     Priority      Free Stack Remaining<br>\
************************************************************<br></b>");
#endif

#else // no trace facility, can only get task name accurately
        strcat(cDynamicPage,
"<p><pre><b>\
Priority    Stack Size    TaskName<br>\
*************************************<br></b>");
#endif
            /* ... Then the list of tasks and their status... */
#if (RTOS == FreeRTOS && configUSE_TRACE_FACILITY == 1)
            UEZGetTaskInfo(cDynamicPage, lineBuffer);
#else
            UEZGetTaskList(cDynamicPage);
#endif            
            /* ... Finally the page footer. */
            strcat(cDynamicPage, webHTML_END );

            /* Write out the dynamically generated page. */
            UEZNetworkSocketWrite(aNetwork, aSocket, cDynamicPage, strlen(
                    cDynamicPage), EFalse, 2000);
        }
    }
    UEZNetworkSocketClose(aNetwork, aSocket);
    UEZNetworkSocketDelete(aNetwork, aSocket);
}
/*------------------------------------------------------------*/
/**
 *	Task for BasicWebServer
 */ 
void BasicWEBServerTask(T_uezTask aMyTask, void *aParameters)
{
    T_uezNetworkSocket socket;
    T_uezNetworkSocket newSocket;
    T_uezDevice aNetwork = (T_uezDevice)aParameters;
    PARAM_NOT_USED(aMyTask);

    lwip_socket_thread_init(); // initialize per thread semaphore if used, creates new semaphore for this thread
    /* Create a new tcp connection handle */
    if (UEZNetworkSocketCreate(aNetwork, UEZ_NETWORK_SOCKET_TYPE_TCP,
            &socket) == UEZ_ERROR_NONE) {
        // Setup the socket to be on the HTTP port
        UEZNetworkSocketBind(aNetwork, socket, 0, webHTTP_PORT);
        // Put the socket into listen mode
        UEZNetworkSocketListen(aNetwork, socket);

        /* Loop forever */
        for (;;) {
            /* Wait for connection. */
            UEZNetworkSocketAccept(aNetwork, socket, &newSocket,
                    UEZ_TIMEOUT_INFINITE);

            /* Service connection. */
            if (newSocket) {
                /* Service connection. */
                IBasicWebProcessConnection(aNetwork, newSocket);
                UEZNetworkSocketDelete(aNetwork, newSocket);
            }
        }
    }
    lwip_socket_thread_cleanup(); // clean up semaphores
    while (1) {
        // Sit here doing nothing
        UEZTaskDelay(1000);
    }
}
/**
 *	The function that implements the WEB server task.
 *
 *	@return		T_uezError
 */ 
T_uezError BasicWebStart(T_uezDevice aNetwork)
{
    return UEZTaskCreate((T_uezTaskFunction)BasicWEBServerTask, "BasicWeb",
            UEZ_TASK_STACK_BYTES(2816), (void *)aNetwork, UEZ_PRIORITY_NORMAL, &G_BasicWebTask);
}

#if (configUSE_TRACE_FACILITY == 1)
#include <task.h>
void UEZGetTaskInfo(char* aBuffer, char* aLineBuffer)
{
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    configRUN_TIME_COUNTER_TYPE ulTotalRunTime;
    
    // Take a snapshot of the number of tasks in case it changes while this function is executing.
    uxArraySize = uxTaskGetNumberOfTasks();

    // Allocate a TaskStatus_t structure for each task. An array could be allocated statically at compile time.
    pxTaskStatusArray = (TaskStatus_t *) pvPortMalloc( uxArraySize * sizeof( TaskStatus_t ) );

    if( pxTaskStatusArray != NULL ) {
      // Generate raw status information about each task.
      uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );

#if (configGENERATE_RUN_TIME_STATS == 1)
      uint32_t  ulStatsAsPercentage;
      ulTotalRunTime /= 100UL; // For percentage calculations.
#endif

      for( x = 0; x < uxArraySize; x++ ) { // For each populated position in the pxTaskStatusArray array
          if(x > 14) { // stop before overflowing the webMAX_PAGE_SIZE, only up to 15 tasks.
            // roughly each task line adds about 65-70 bytes to the total
            printf("Dynamic Page Limit Reached");
            break;
          }
          sprintf(aLineBuffer, "%10s: ", pxTaskStatusArray[x].pcTaskName);
          strcat(aBuffer, aLineBuffer);
          switch(pxTaskStatusArray[x].eCurrentState){
          case 0:
                  strcat(aBuffer, "RUN");                                
                  break;
          case 1:
                  strcat(aBuffer, "RDY");
                  break;
          case 2:
                  strcat(aBuffer, "BLK");
                  break;
          case 3:
                  strcat(aBuffer, "SUS");
                  break;
          case 4:
                  strcat(aBuffer, "DEL");
                  break;
          case 5:
          default:
                  strcat(aBuffer, "INV");
                  break;
          }
          sprintf(aLineBuffer, ", Prio: %02u, Base: %02u, ", (uint32_t) pxTaskStatusArray[x].uxCurrentPriority, (uint32_t) pxTaskStatusArray[x].uxBasePriority);
          strcat(aBuffer, aLineBuffer);
          sprintf(aLineBuffer, "Stack Free: %6u", (uint32_t) pxTaskStatusArray[x].usStackHighWaterMark);
          strcat(aBuffer, aLineBuffer);

#if (configGENERATE_RUN_TIME_STATS == 1)
          if( ulTotalRunTime > 0 ) {
                  // What percentage of the total run time has the task used? This will always be rounded down to the nearest integer.
                  // ulTotalRunTimeDiv100 has already been divided by 100.
                  ulStatsAsPercentage = pxTaskStatusArray[x].ulRunTimeCounter / ulTotalRunTime;

                  if( ulStatsAsPercentage > 0UL ) {
                          sprintf(aLineBuffer,"\t%lu\t\t%lu%%\r", (unsigned long) pxTaskStatusArray[ x ].ulRunTimeCounter, (unsigned long)ulStatsAsPercentage );
                          strcat(aBuffer, aLineBuffer);
                  } else {
                          // If the percentage is zero here then the task has consumed less than 1% of the total run time. Print "<1%"
                          sprintf(aLineBuffer,"\t%lu\t\t<1%%\r", (unsigned long) pxTaskStatusArray[ x ].ulRunTimeCounter );
                          strcat(aBuffer, aLineBuffer);
                  }
          }
#else
          strcat(aBuffer, "\r");
#endif
      }
      vPortFree( pxTaskStatusArray ); // The array is no longer needed, free the memory it consumes.
    } else {
      strcat(aBuffer, "FAIL: Mem Alloc");
    }
}
#endif

/** @} */
