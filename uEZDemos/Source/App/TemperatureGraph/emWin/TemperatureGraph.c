/*-------------------------------------------------------------------------*
 * File: TemperatureGraph.c
 *-------------------------------------------------------------------------*
 * Description:
 *
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
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "WM.h"
#include "GRAPH.h"
#include "string.h"
#include <uEZ.h>
#include "LookAndFeel.h"
#include <stdio.h>
#include <uEZFile.h>
#include <UEZTemperature.h>
#include "TemperatureGraph.h"
#if RENAME_INI
#include <UEZWatchdog.h>
#endif
#include <UEZLCD.h>

#include <UEZTimeDate.h>

//#include <uEZTemperature.h>

extern GUI_CONST_STORAGE GUI_FONT GUI_FontOpenSans49;
extern GUI_CONST_STORAGE GUI_FONT GUI_FontOpenSans24;
/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define     ID_WINDOW               (GUI_ID_USER + 0x00)
#define     ID_TIME_DATE_TEXT       (GUI_ID_USER + 0x01)
#define     ID_TEMP_TEXT            (GUI_ID_USER + 0x02)
#define     ID_HUMIDITY_TEXT        (GUI_ID_USER + 0x03)
#define     ID_GRAPH                (GUI_ID_USER + 0x04)


#define     ID_RTCUPDATE_TIMER      (GUI_ID_USER + 0x05)
#define     ID_READINGUPDATE_TIMER  (GUI_ID_USER + 0x06)
#define     ID_GRAPHUPDATE_TIMER    (GUI_ID_USER + 0x07)
#define     ID_LOG_DATA_TIMER       (GUI_ID_USER + 0x08)
#define     ID_SETTINGS_BUTTON      (GUI_ID_USER + 0x09)
#define     ID_ALERT_TEXT           (GUI_ID_USER + 0x0A)
#define     ID_BACK_BUTTON          (GUI_ID_USER + 0x0B)
#define     ID_GRAPH_KEY_TEXT       (GUI_ID_USER + 0x0C)
#define     ID_GRAPH_KEY_TEXT2      (GUI_ID_USER + 0x0D)

#define WINDOW_XSIZE                (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE                (UEZ_LCD_DISPLAY_HEIGHT)

#define     SPACING                 10

#define     TIME_DATE_XPOS          -50
#define     TIME_DATE_YPOS          0
#define     TIME_DATE_XSIZE         UEZ_LCD_DISPLAY_WIDTH
#define     TIME_DATE_YSIZE         (UEZ_LCD_DISPLAY_HEIGHT/8)

#define     TEMP_HUMIDITY_XSIZE     (UEZ_LCD_DISPLAY_WIDTH/6)
#define     TEMP_HUMIDITY_YSIZE     (UEZ_LCD_DISPLAY_HEIGHT/8)
#define     TEMP_HUMIDITY_XPOS      10
#define     TEMP_HUMIDITY_YPOS(n)   ((UEZ_LCD_DISPLAY_HEIGHT/4) + SPACING) + (n * ((UEZ_LCD_DISPLAY_HEIGHT/4) + SPACING))

#define     GRAPH_XPOS              (TEMP_HUMIDITY_XPOS + (UEZ_LCD_DISPLAY_HEIGHT/8) + (2 * SPACING))
#define     GRAPH_YPOS              (UEZ_LCD_DISPLAY_HEIGHT/8)
#define     GRAPH_XSIZE             (UEZ_LCD_DISPLAY_WIDTH - GRAPH_XPOS - SPACING)
#define     GRAPH_YSIZE             (UEZ_LCD_DISPLAY_HEIGHT - TIME_DATE_YSIZE - SPACING)

 /*   Code Removed by IMM - Exit button was removed and replaced with the ID_BACK_BUTTON
#define     EXIT_XSIZE              (75)
#define     EXIT_YSIZE              (50)
#define     EXIT_XPOS               (3)
#define     EXIT_YPOS               (3)
*/

#define BACK_BUTTON_XSIZE       ((WINDOW_XSIZE/6))
#define BACK_BUTTON_YSIZE       ((WINDOW_YSIZE/8))
#define BACK_BUTTON_XPOS        (SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - SPACING - BACK_BUTTON_YSIZE)   

#define     GRAPH_BORDERL           25
#define     GRAPH_BORDERT           10
#define     GRAPH_BORDERR           0
#define     GRAPH_BORDERB           0

#define     GRAPH_BK_COLOR          GUI_BLACK
#define     GRAPH_BORDER_COLOR      GUI_GRAY
#define     GRAPH_FRAME_COLOR       GUI_BLACK
#define     GRAPH_GRID_COLOR        GUI_YELLOW

#define     GRAPH_TEMP_COLOR        GUI_BLUE
#define     GRAPH_HUMIDITY_COLOR    GUI_RED

#define     GRAPH_XNUMPIXELS        (GRAPH_XSIZE - GRAPH_BORDERL - GRAPH_BORDERR)
#define     GRAPH_YNUMPIXELS        (GRAPH_YSIZE - GRAPH_BORDERT - GRAPH_BORDERB)

#define     GRAPH_H_TICK_SIZE       60
#define     GRAPH_V_TICK_SIZE       5

#define     NUM_TEMP_SAPMPLES       GRAPH_XNUMPIXELS

#define     FONT                    GUI_FontOpenSans24
#define     FONT_LARGE              GUI_FontOpenSans49
#define     BACKGROUND_COLOR        GUI_BLACK
#define     FONT_COLOR              GUI_WHITE

#define     RTC_UPDATE_TIME_MS      1000
#define     READING_UPDATE_TIME_MS  100
#define     GRAPH_UPDATE_TIME_MS    100 //(1000 * 60)
#define     ID_LOG_DATA_TIME_MS     (1000 * 60)

#define     ALERT_X_POS             (3)    //Added by IMM - Added to give the alert a better and dedicated location
#define     ALERT_Y_POS(n)         ((UEZ_LCD_DISPLAY_HEIGHT/4) + SPACING) + (n * ((UEZ_LCD_DISPLAY_HEIGHT/4) + SPACING))+30    //Added by IMM - Added to give the alert a better and dedicated location

#define     GRAPH_KEY_X_POS        560 //Added by IMM - Added to give the labels a better and dedicated location
#define     GRAPH_KEY_Y_POS        10 //Added by IMM - Added to give the labels a better and dedicated location

#define     TEMP_HIGH              32
#define     TEMP_LOW               10

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleUpdateReadings(WM_MESSAGE * pMsg, int aNCode);
static TBool IHandleUpdateTime(WM_MESSAGE * pMsg, int aNCode);
static TBool IHandleUpdateGraph(WM_MESSAGE * pMsg, int aNCode);
static TBool IHandleLogData(WM_MESSAGE * pMsg, int aNCode);
static TBool IHandleSettings(WM_MESSAGE * pMsg, int aNCode, int aID);
#if RENAME_INI
static TBool IHandleExit(WM_MESSAGE * pMsg, int aNCode, int aID);
#endif


/*---------------------------------------------------------------------------*
 * Global Data:
 *---------------------------------------------------------------------------*/
T_TemperatureGraphSetting G_AppSettings;

/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
static const GUI_WIDGET_CREATE_INFO _iTemperatureGraphDialog[] = {
  //Function                 Text           ID                       XP                     YP                      XS                             YS
  {WINDOW_CreateIndirect    , ""          , ID_WINDOW              , 0                    , 0                     , UEZ_LCD_DISPLAY_WIDTH        , UEZ_LCD_DISPLAY_HEIGHT    , 0, 0, 0},
  {TEXT_CreateIndirect      , ""          , ID_TIME_DATE_TEXT      , TIME_DATE_XPOS       , TIME_DATE_YPOS        , TIME_DATE_XSIZE              , TIME_DATE_YSIZE+20        , GUI_TA_HCENTER, 0, 0},
  {TEXT_CreateIndirect      , ""          , ID_TEMP_TEXT           , TEMP_HUMIDITY_XPOS   , TEMP_HUMIDITY_YPOS(0) , TEMP_HUMIDITY_XSIZE+30       , TEMP_HUMIDITY_YSIZE       , 0, 0, 0},
  {TEXT_CreateIndirect      , ""          , ID_HUMIDITY_TEXT       , TEMP_HUMIDITY_XPOS   , TEMP_HUMIDITY_YPOS(1) , TEMP_HUMIDITY_XSIZE+20       , TEMP_HUMIDITY_YSIZE       , 0, 0, 0},
  {BUTTON_CreateIndirect    , "Settings"  , ID_SETTINGS_BUTTON     , 0                    , (3)                   , TEMP_HUMIDITY_XSIZE-SPACING  , TEMP_HUMIDITY_YSIZE-10       , 0, 0, 0},
  {TEXT_CreateIndirect      , ""          , ID_ALERT_TEXT          , ALERT_X_POS          , ALERT_Y_POS(0)        , TEMP_HUMIDITY_XSIZE-30       , TEMP_HUMIDITY_YSIZE       , 0, 0, 0},
  {GRAPH_CreateIndirect     , ""          , ID_GRAPH, GRAPH_XPOS   , GRAPH_YPOS           , GRAPH_XSIZE           , GRAPH_YSIZE                  , WM_CF_SHOW | WM_CF_CONST_OUTLINE, 0, 0},
  {TEXT_CreateIndirect      , ""          , ID_GRAPH_KEY_TEXT      , GRAPH_KEY_X_POS      , GRAPH_KEY_Y_POS       , TIME_DATE_XSIZE              , TIME_DATE_YSIZE/2           , 0, 0, 0},//Code added by IMM - Add label to tell the user what each color represents
  {TEXT_CreateIndirect      , ""          , ID_GRAPH_KEY_TEXT2     , GRAPH_KEY_X_POS+110  , GRAPH_KEY_Y_POS       , TIME_DATE_XSIZE              , TIME_DATE_YSIZE/2           , 0, 0, 0},//Code added by IMM - Add label to tell the user what each color represents
#if RENAME_INI
  {BUTTON_CreateIndirect    , "Back"     , ID_BACK_BUTTON          , BACK_BUTTON_XPOS-10  , BACK_BUTTON_YPOS      , BACK_BUTTON_XSIZE-45         , BACK_BUTTON_YSIZE           , 0, 0, 0},   //Code Modified by IMM - Changed ID_EXIT_BUTTON to ID_BACK_BUTTON and changed size and position code    
#endif
};

static T_LAFMapping TemperatureGraphLayout[] = {
  { ID_WINDOW           ,""             , BACKGROUND_COLOR     , FONT_COLOR, &FONT_LARGE , LAFSetupWindow      , 0},
  { ID_TIME_DATE_TEXT   ,""             , BACKGROUND_COLOR     , FONT_COLOR, &FONT_LARGE , LAFSetupText        , 0},
  { ID_TEMP_TEXT        ,""             , BACKGROUND_COLOR     , GRAPH_TEMP_COLOR        , &FONT , LAFSetupText, 0},
  { ID_GRAPH_KEY_TEXT   ,"BLUE: Temp"   , BACKGROUND_COLOR     , GUI_BLUE, &FONT         , LAFSetupText        , 0},
  { ID_GRAPH_KEY_TEXT2  ,"RED: Humidity", BACKGROUND_COLOR     , GUI_RED, &FONT          , LAFSetupText        , 0},
  { ID_HUMIDITY_TEXT    ,""             , BACKGROUND_COLOR     , GRAPH_HUMIDITY_COLOR    , &FONT, LAFSetupText , 0},
  { ID_SETTINGS_BUTTON  ,"Settings"     , GUI_GRAY             , GUI_BLACK, &FONT        , LAFSetupButton      , IHandleSettings},
  { ID_ALERT_TEXT       ,""             , BACKGROUND_COLOR     , FONT_COLOR              , GUI_FONT_13B_ASCII  , LAFSetupText   , 0},
#if RENAME_INI
  { ID_BACK_BUTTON      , "Back"        , GUI_GRAY             , GUI_BLACK     , &FONT    ,LAFSetupButton      , IHandleExit},   //Code Modified by IMM - Changed ID_EXIT_BUTTON to ID_BACK_BUTTON and changed the font and color code
#endif
  {0},
};

static GRAPH_Handle G_GraphHandle;
static WM_HTIMER G_RTCUpdate;
static WM_HTIMER G_ReadingUpdate;
static WM_HTIMER G_GraphUpdate;
static WM_HTIMER G_LogData;
static GRAPH_SCALE_Handle G_Graph_H_Scale;
static GRAPH_SCALE_Handle G_Graph_V_Scale;
static GRAPH_DATA_Handle G_Temp_Data;
static GRAPH_DATA_Handle G_Humidity_Data;
static T_uezDevice _hTouchScreen;
static T_uezQueue _hTSQueue;
static U8 _RequestExit;
static T_uezFile G_FileHandle;
static T_uezDevice G_TempSensor;
//static T_uezTimeDate G_TimeDate;
T_uezTimeDate G_TimeDate;

/*---------------------------------------------------------------------------*
 * Functions:
 *---------------------------------------------------------------------------*/
extern WM_HWIN CreateTemperatureGraph_Setting(void);
static TBool IHandleSettings(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        GUI_ExecCreatedDialog(CreateTemperatureGraph_Setting());
    }
    return EFalse;
}

#if RENAME_INI
static TBool IHandleExit(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    T_uezDevice watchdog;
    
    if (aNCode == WM_NOTIFICATION_RELEASED) {
      if(UEZWatchdogOpen("Watchdog", &watchdog) == UEZ_ERROR_NONE){
        UEZWatchdogSetMaxTime(watchdog, 100);
        UEZWatchdogSetMinTime(watchdog, 1);
        UEZWatchdogStart(watchdog);
        UEZWatchdogFeed(watchdog);
        UEZWatchdogTrip(watchdog);
      }
    }
    return EFalse;
}
#endif

static TBool IHandleUpdateReadings(WM_MESSAGE * pMsg, int aNCode)
{
    TInt32 tempReading;
    WM_HWIN hItem;
    char text[100];
    TUInt32 i, f;
    TInt32 tempInF;
    static TUInt8 timesThrough;

    if(UEZTemperatureRead(G_TempSensor, &tempReading) == UEZ_ERROR_NONE){
        //Display the temp
        tempInF = tempReading;
        i = tempReading>>16;
        f = ((((TUInt32)tempReading)&0xFFFF)*10)>>16;  // Convert to 1 digit decimal
        hItem = WM_GetDialogItem(pMsg->hWin, ID_HUMIDITY_TEXT);
        //fake the humidity reading
        i = i - 1;
        sprintf(text, "%3d.%01d%c", i, f, 0x25);
        TEXT_SetText(hItem, text);

        tempInF *= 9;
        tempInF /=5;
        tempInF += (32<<16);
        if(G_AppSettings.iTempIsFahrenheit){
            i = tempInF>>16;
            f = ((((TUInt32)tempInF)&0xFFFF)*10)>>16;
            sprintf(text, "%3d.%01d F", i, f); //change to deg f
        }else{
            i = tempReading>>16;
            f = ((((TUInt32)tempReading)&0xFFFF)*10)>>16;  // Convert to 1 digit decimal
            sprintf(text, "%3d.%01d C", i, f);
			
        }
        hItem = WM_GetDialogItem(pMsg->hWin, ID_TEMP_TEXT);
        TEXT_SetText(hItem, text);

        timesThrough++;
        hItem = WM_GetDialogItem(pMsg->hWin,ID_ALERT_TEXT);
        if(G_AppSettings.iTempIsFahrenheit){  //If statement added by IMM - Original code did not chech here for what temp setting the user is in
          if(((tempInF>>16) > G_AppSettings.iHighAlarmTemp || (tempInF>>16) < G_AppSettings.iLowAlarmTemp)){
              timesThrough++;
              if(timesThrough > 5){
                  if((tempInF>>16) > G_AppSettings.iHighAlarmTemp){
                      TEXT_SetTextColor(hItem, GUI_RED);
                      TEXT_SetText(hItem, "Temp HIGH");
                  }else if ((tempInF>>16) < G_AppSettings.iLowAlarmTemp){
                      TEXT_SetTextColor(hItem, GUI_BLUE);
                      TEXT_SetText(hItem, "Temp Low");
                  }
              }
          }  
        }else{  //Code added by IMM - Will check High and Low temp in Celsius setting
        //******************************************************************************************
          if(((tempReading>>16) > G_AppSettings.iHighAlarmTemp || (tempReading>>16) < G_AppSettings.iLowAlarmTemp)){
              timesThrough++;
              if(timesThrough > 5){
                  if((tempReading>>16) > G_AppSettings.iHighAlarmTemp){
                      TEXT_SetTextColor(hItem, GUI_RED);
                      TEXT_SetText(hItem, "Temp HIGH");
                  }else if ((tempReading>>16) < G_AppSettings.iLowAlarmTemp){
                      TEXT_SetTextColor(hItem, GUI_BLUE);
                      TEXT_SetText(hItem, "Temp Low");
                  }
              }
          }
        }
        //******************************************************************************************
        if(timesThrough > 9){
            timesThrough = 0;
            TEXT_SetText(hItem, "");
        }
    }
    return EFalse;
}

static TBool IHandleUpdateTime(WM_MESSAGE * pMsg, int aNCode)
{
    WM_HWIN hItem;
    TBool isPM;
    TUInt32 hours;
    TUInt32 minutes;
    char text[220];

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TIME_DATE_TEXT);
    if (UEZTimeDateGet(&G_TimeDate) == UEZ_ERROR_NONE) {
        // Convert 24 hour time to 12 hour time
        hours = G_TimeDate.iTime.iHour;
        minutes = G_TimeDate.iTime.iMinute;
        if(G_AppSettings.iTimeIsAMPM){
            if (hours >= 12) {
                hours -= 12;
                isPM = ETrue;
            } else {
                isPM = EFalse;
            }
            if (hours == 0)
                hours = 12;
            // Put up the 12 hour format with an AM/PM tail
			

            sprintf(text, "%02d:%02d:%02d %s %02d/%02d/%04d\0", hours, minutes, G_TimeDate.iTime.iSecond, (isPM)?"PM":"AM", G_TimeDate.iDate.iMonth, G_TimeDate.iDate.iDay, G_TimeDate.iDate. iYear);
        }else{
            sprintf(text, "%02d:%02d:%02d %02d/%02d/%04d\0", hours, minutes, G_TimeDate.iTime.iSecond, G_TimeDate.iDate.iMonth, G_TimeDate.iDate.iDay, G_TimeDate.iDate. iYear);
        }
    } else {
        // Error getting the RTC, put up something funny
        strcpy(text, "??:??:?? PM ??/??/??");
    }
    TEXT_SetText(hItem, text);

	return EFalse;
}

static TBool IHandleUpdateGraph(WM_MESSAGE * pMsg, int aNCode)
{
    TInt32 tempReading;
    
    if(UEZTemperatureRead(G_TempSensor, &tempReading) == UEZ_ERROR_NONE){
        
        tempReading = tempReading>>16;
        GRAPH_DATA_YT_AddValue(G_Humidity_Data, (tempReading - 1) * 4);//times 4 to match graph .25

        tempReading *= 9;
        tempReading /=5;
        tempReading += 32;
        GRAPH_DATA_YT_AddValue(G_Temp_Data, tempReading * 4);
    }
    return EFalse;
}

static TBool IHandleLogData(WM_MESSAGE * pMsg, int aNCode)
{
    char dataOut[250];
    TUInt32 BytesWritten;
    TInt32 tempReading;
    char tempInF[70];
    char humidity[70];
    TUInt32 i, f;

    if(UEZFileOpen("1:/data.csv", FILE_FLAG_APPEND, &G_FileHandle) == UEZ_ERROR_NONE){
        if (UEZTimeDateGet(&G_TimeDate) == UEZ_ERROR_NONE) {
            if(UEZTemperatureRead(G_TempSensor, &tempReading) == UEZ_ERROR_NONE){

                i = tempReading>>16;
                f = ((((TUInt32)tempReading)&0xFFFF)*10)>>16;
                i -= 1;
                sprintf(humidity, "%02d.%d%c\0", i, f, 0x25);

                //Convert to Deg F
                tempReading *= 9;
                tempReading /= 5;
                tempReading += (32<<16);
                i = tempReading>>16;
                f = ((((TUInt32)tempReading)&0xFFFF)*10)>>16;
                sprintf(tempInF, "%03d.%dF\0", i, f);
                sprintf(dataOut, "%02d:%02d,%s,%s\r\n", G_TimeDate.iTime.iHour, G_TimeDate.iTime.iMinute, tempInF, humidity);
                UEZFileWrite(G_FileHandle, dataOut, strlen(dataOut), &BytesWritten);
            }
        }        
        UEZFileClose(G_FileHandle);
    }

    return EFalse;
}

static void iInitGraph(WM_MESSAGE *pMsg)
{
    //G_GraphHandle = GRAPH_CreateEx(GRAPH_XPOS, GRAPH_YPOS, GRAPH_XSIZE, GRAPH_YSIZE, WM_HBKWIN, WM_CF_SHOW | WM_CF_CONST_OUTLINE, 0, 0);
    G_GraphHandle = WM_GetDialogItem(pMsg->hWin, ID_GRAPH);
    GRAPH_SetBorder(G_GraphHandle, GRAPH_BORDERL, GRAPH_BORDERT, GRAPH_BORDERR, GRAPH_BORDERB);
    GRAPH_SetColor(G_GraphHandle, GRAPH_BK_COLOR,     GRAPH_CI_BK);
    GRAPH_SetColor(G_GraphHandle, GRAPH_BORDER_COLOR, GRAPH_CI_BORDER);
    GRAPH_SetColor(G_GraphHandle, GRAPH_FRAME_COLOR,  GRAPH_CI_FRAME);
    GRAPH_SetColor(G_GraphHandle, GRAPH_GRID_COLOR,   GRAPH_CI_GRID);

    GRAPH_SetGridDistX(G_GraphHandle, GRAPH_XNUMPIXELS);
    GRAPH_SetGridDistY(G_GraphHandle, GRAPH_YNUMPIXELS);
    GRAPH_SetGridFixedX(G_GraphHandle, 0);//0 X-axis is moving
    GRAPH_SetGridVis(G_GraphHandle, 0);//0 is set the grid to not be visible

    GRAPH_SetLineStyleH(G_GraphHandle, GUI_LS_DOT);
    GRAPH_SetLineStyleV(G_GraphHandle, GUI_LS_DOT);

    G_Graph_H_Scale = GRAPH_SCALE_Create(GRAPH_BORDERT / 2, GUI_TA_RIGHT, GRAPH_SCALE_CF_HORIZONTAL, GRAPH_H_TICK_SIZE);
    G_Graph_V_Scale = GRAPH_SCALE_Create(GRAPH_BORDERL / 2, GUI_TA_HCENTER, GRAPH_SCALE_CF_VERTICAL, GRAPH_V_TICK_SIZE);

    GRAPH_SCALE_SetTextColor(G_Graph_V_Scale, GRAPH_GRID_COLOR);
    GRAPH_AttachScale(G_GraphHandle, G_Graph_V_Scale);

    GRAPH_SCALE_SetTextColor(G_Graph_H_Scale, GRAPH_GRID_COLOR);
    GRAPH_AttachScale(G_GraphHandle, G_Graph_H_Scale);

    //This code handles the Vertical number scale
    GRAPH_SCALE_SetTickDist(G_Graph_V_Scale, (GRAPH_YNUMPIXELS / 100) * 5);
    GRAPH_SCALE_SetTickDist(G_Graph_H_Scale, (GRAPH_XNUMPIXELS / 50) * 5);

    //change scale back to 0 to 100
    
    //GRAPH_SCALE_SetFactor(G_Graph_V_Scale, .25);
    GRAPH_SCALE_SetFactor(G_Graph_V_Scale, .25);  //Code Modified by IMM, Original code commented out above
    GRAPH_SCALE_SetFactor(G_Graph_H_Scale, .01538);
    
    GRAPH_SetVSizeY(G_GraphHandle, 580);//Code added by IMM - Creates a Scroll Bar and allows the graph's range to go up to 120 degrees Fahrenheight
    GRAPH_SCALE_SetOff(G_Graph_V_Scale, 90);//Code added by IMM - Makes the minimum value on the graph -20
    
    G_Temp_Data = GRAPH_DATA_YT_Create(GRAPH_TEMP_COLOR, NUM_TEMP_SAPMPLES, 0, 0);
    
    GRAPH_DATA_YT_SetOffY(G_Temp_Data,90);//Code added by IMM to adjust the rescaling of the graph
    
    GRAPH_AttachData(G_GraphHandle, G_Temp_Data);
   

    G_Humidity_Data = GRAPH_DATA_YT_Create(GRAPH_HUMIDITY_COLOR, NUM_TEMP_SAPMPLES, 0, 0);
    
    GRAPH_DATA_YT_SetOffY(G_Humidity_Data,90);//Code added by IMM to adjust the rescaling of the graph
    
    GRAPH_AttachData(G_GraphHandle, G_Humidity_Data);

    GRAPH_DATA_YT_SetAlign(G_Temp_Data, GRAPH_ALIGN_LEFT);
    GRAPH_DATA_YT_SetAlign(G_Humidity_Data, GRAPH_ALIGN_LEFT);
}

/*---------------------------------------------------------------------------*
 * Task:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/
static void _TemperatureGraphDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        LAFSetup(pMsg->hWin, ID_WINDOW, TemperatureGraphLayout);
        
        //Code added by IMM - Initialize G_AppSettings.iLowAlarmTemp and G_AppSettings.iHighAlarmTemp
        G_AppSettings.iLowAlarmTemp =  TEMP_LOW;
        G_AppSettings.iHighAlarmTemp =  TEMP_HIGH;

        //Create timers
        G_RTCUpdate = WM_CreateTimer(pMsg->hWin, ID_RTCUPDATE_TIMER, 0, 0);
        G_ReadingUpdate = WM_CreateTimer(pMsg->hWin, ID_READINGUPDATE_TIMER, 0, 0);
        G_GraphUpdate = WM_CreateTimer(pMsg->hWin, ID_GRAPHUPDATE_TIMER, 0 , 0);
        G_LogData = WM_CreateTimer(pMsg->hWin, ID_LOG_DATA_TIMER, 0, 0);

        //Create Graph
        iInitGraph(pMsg);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(TemperatureGraphLayout, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
    case WM_PRE_PAINT:
        //Display logo in bottom right corner
        break;
    case WM_POST_PAINT:
      //IHandleUpdateGraph(pMsg, 0);
        break;
    case WM_TIMER:
        NCode = pMsg->Data.v;
        if(NCode == G_RTCUpdate){
            IHandleUpdateTime(pMsg, NCode);
            WM_RestartTimer(G_RTCUpdate, RTC_UPDATE_TIME_MS);
        }else if (NCode == G_ReadingUpdate){
            IHandleUpdateReadings(pMsg, NCode);
            WM_RestartTimer(G_ReadingUpdate, READING_UPDATE_TIME_MS);
        }else if (NCode == G_GraphUpdate){
            IHandleUpdateGraph(pMsg, NCode);
            WM_RestartTimer(G_GraphUpdate, GRAPH_UPDATE_TIME_MS);
        }else if (NCode == G_LogData){
            IHandleLogData(pMsg, NCode);
            WM_RestartTimer(G_LogData, ID_LOG_DATA_TIME_MS);
        }
        break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Task:
 *---------------------------------------------------------------------------*
 * Description:
 *
 * Inputs:
 *
 * Outputs:
 *
 *---------------------------------------------------------------------------*/


WM_HWIN Temperature_Create()
{
    UEZTemperatureOpen("Temp0",&G_TempSensor);
    return GUI_CreateDialogBox(_iTemperatureGraphDialog, GUI_COUNTOF(_iTemperatureGraphDialog), &_TemperatureGraphDialog, 0,0,0);
}

/*-------------------------------------------------------------------------*
 * End of File:  TemperatureGraph.c
 *-------------------------------------------------------------------------*/
