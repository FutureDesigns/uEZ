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
#include <uEZTimeDate.h>
//#include <stdlib.h>
#include "AppDemo.h"
#include <Config_Build.h>

#if (UEZ_ENABLE_TCPIP_STACK == 1)
#include <Source/Library/Web/HTTPServer/HTTPServer.h>
#include "AppHTTPServer.h"

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
    char line[18];
    // In a real application you should check the clock periodically then just 
    // retrieve the values here. Right now we call the timedate api twice per
    // page load which isn't ideal. The same would apply to other dyamic data.
    if (UEZTimeDateGet(&TimeDate) == UEZ_ERROR_NONE) {
        // success on retriving time from RTC
        // current time value stored in TimeDate        
        if (strcmp(aVarName, "time") == 0) {
          if(TimeDate.iTime.iHour == 0){
            sprintf(line, "12:%02u:%02u AM\r\n",
                (uint32_t) TimeDate.iTime.iMinute, (uint32_t) TimeDate.iTime.iSecond);
          }else if (TimeDate.iTime.iHour < 12){
            sprintf(line, "%02u:%02u:%02u AM\r\n", (uint32_t) TimeDate.iTime.iHour,
               (uint32_t) TimeDate.iTime.iMinute, (uint32_t) TimeDate.iTime.iSecond);
          }else if (TimeDate.iTime.iHour == 12){
            sprintf(line, "%02u:%02u:%02u PM\r\n", (uint32_t) TimeDate.iTime.iHour,
                (uint32_t) TimeDate.iTime.iMinute, (uint32_t) TimeDate.iTime.iSecond);
          }else{
            sprintf(line, "%02u:%02u:%02u PM\r\n", (uint32_t) (TimeDate.iTime.iHour-12),
                (uint32_t) TimeDate.iTime.iMinute, (uint32_t) TimeDate.iTime.iSecond);
          }
          return HTTPServerSetVar(aHTTPState, aVarName, line);
        } else if (strcmp(aVarName, "date") == 0) {
          sprintf(line, "%02u/%02u/%04u\r\n", (uint32_t) TimeDate.iDate.iMonth,
                (uint32_t) TimeDate.iDate.iDay, (uint32_t) TimeDate.iDate.iYear);
          return HTTPServerSetVar(aHTTPState, aVarName, line);
        } else {
          return HTTPServerSetVar(aHTTPState, aVarName, "InsertVariableHere");
        }
    } else { // timedate failure
        return HTTPServerSetVar(aHTTPState, aVarName, "InsertVariableHere");
    }    
    //return UEZ_ERROR_NONE;
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
    PARAM_NOT_USED(aHTTPState);

    if (strcmp(aVarName, "freq") == 0) {    // Remember the last know frequency
        frequency = atoi(aValue);
        //printf("Freq\n");
        return UEZ_ERROR_NONE;
    } else if (strcmp(aVarName, "duration") == 0) {
        //printf("Duration\n");
        PlayAudio(frequency, atoi(aValue) * 1000);
        return UEZ_ERROR_NONE;
    } else { // TODO can add more variables here
        //printf("Variable %s\n", aVarName);
        return UEZ_ERROR_NONE;
    }
}

static T_httpServerParameters G_httpServerParamsWired = {
    .iGet = IMainHTTPServerGetValue,
    .iSet = IMainHTTPServerSetValue,
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
#endif
/*-------------------------------------------------------------------------*
 * End of File:  AppHTTPServer.c
 *-------------------------------------------------------------------------*/
