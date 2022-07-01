/*-------------------------------------------------------------------------*
 * File:  AppHTTPServer.c
 *-------------------------------------------------------------------------*
 * Description:
 *      HTTP Server Application
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
#include <uEZ.h>
#include <string.h>
#include <stdio.h>
//#include <stdlib.h>
#include <uEZTimeDate.h>
#include <Source/Library/Web/HTTPServer/HTTPServer.h>
#include "AppHTTPServer.h"
#include "AppDemo.h"

extern int atoi(const char *__nptr);
/*---------------------------------------------------------------------------*
 * Routine:  IMainHTTPServerGetValue
 *---------------------------------------------------------------------------*
 * Description:
 *      Retrieves values for dynamic webpages. This example pulls the current 
 *      time and date information using the uEZTImeDate API.
 * Outputs:
 *      T_uezError                   -- error
 *---------------------------------------------------------------------------*/
static T_uezError IMainHTTPServerGetValue(
            void *aHTTPState,
            const char *aVarName)
{
    T_uezTimeDate TimeDate;
    char line[16];
    // In a real application you should check the clock periodically then just 
    // retrieve the values here. Right now we call the timedate api twice per
    // page load which isn't ideal. The same would apply to other dyamic data.
    if (UEZTimeDateGet(&TimeDate) == UEZ_ERROR_NONE) {
        // success on retriving time from RTC
        // current time value stored in TimeDate        
        if (strcmp(aVarName, "time") == 0) {
          if(TimeDate.iTime.iHour == 0){
            sprintf(line, "12:%02u:%02u AM\r\n",
                TimeDate.iTime.iMinute, TimeDate.iTime.iSecond);
          }else if (TimeDate.iTime.iHour < 12){
            sprintf(line, "%02u:%02u:%02u AM\r\n", TimeDate.iTime.iHour,
                TimeDate.iTime.iMinute, TimeDate.iTime.iSecond);
          }else if (TimeDate.iTime.iHour == 12){
            sprintf(line, "%02u:%02u:%02u PM\r\n", TimeDate.iTime.iHour,
                TimeDate.iTime.iMinute, TimeDate.iTime.iSecond);
          }else{
            sprintf(line, "%02u:%02u:%02u PM\r\n", (TimeDate.iTime.iHour-12),
                TimeDate.iTime.iMinute, TimeDate.iTime.iSecond);
          }
            HTTPServerSetVar(aHTTPState, aVarName, line);
        } else if (strcmp(aVarName, "date") == 0) {
            sprintf(line, "%02u/%02u/%04u\r\n", TimeDate.iDate.iMonth,
                TimeDate.iDate.iDay, TimeDate.iDate.iYear);
            HTTPServerSetVar(aHTTPState, aVarName, line);
        } else {
            HTTPServerSetVar(aHTTPState, aVarName, "InsertVariableHere");
        }
    } else { // timedate failure
        HTTPServerSetVar(aHTTPState, aVarName, "InsertVariableHere");
    }
    
    return UEZ_ERROR_NONE;
}
/*---------------------------------------------------------------------------*
 * Routine:  IMainHTTPServerSetValue
 *---------------------------------------------------------------------------*
 * Description:
 *      Retrieves values from dynamic webpages. 
 * Outputs:
 *      T_uezError                   -- error
 *---------------------------------------------------------------------------*/
static T_uezError IMainHTTPServerSetValue(
            void *aHTTPState,
            const char *aVarName,
            const char *aValue)
{
    static TUInt32 frequency = 1000;

    if (strcmp(aVarName, "freq") == 0) {    // Remember the last know frequency
        frequency = atoi(aValue);
    } else if (strcmp(aVarName, "duration") == 0) {
        PlayAudio(frequency, atoi(aValue) * 1000);
    }
    return UEZ_ERROR_NONE;
}

static T_httpServerParameters G_httpServerParamsWired = {
    IMainHTTPServerGetValue,
    IMainHTTPServerSetValue,
};

/*---------------------------------------------------------------------------*
 * Routine:  App_HTTPServerStart
 *---------------------------------------------------------------------------*
 * Description:
 *      Starts the HTTP server
 * Outputs:
 *      T_uezError                   -- error
 *---------------------------------------------------------------------------*/

T_uezError App_HTTPServerStart(T_uezDevice network)
{
    T_uezError error;
    
    G_httpServerParamsWired.iNetwork = network;
    G_httpServerParamsWired.iDrivePrefix = HTTP_SERVER_DRIVE;
    G_httpServerParamsWired.iPort = HTTP_SERVER_HTTP_PORT;
    
    error = WebServerStart(&G_httpServerParamsWired);
    
    if (error) {
        printf("Problem starting HTTPServer! (Error=%d)\n", error);
    } else {
        printf("HTTPServer started\n");
    }
    
    return error;
}

/*-------------------------------------------------------------------------*
 * End of File:  AppHTTPServer.c
 *-------------------------------------------------------------------------*/
