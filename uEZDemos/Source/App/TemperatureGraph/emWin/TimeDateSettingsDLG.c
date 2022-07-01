/*-------------------------------------------------------------------------*
 * File:  TimeDateSettingsDLG.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Implements Layout and functionality of TimeDateSettingsDLG.c.
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

//Includes needed by emWin
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "DIALOG.h"
#include "TEXT.h"
#include "WM.h"
#include <string.h>
#include <stdio.h>

//uEZ and Application Includes
#include <uEZ.h>
#include "LookAndFeel.h"
#include "WindowManager.h"
#include "Graphics/Graphics.h"
//#include "Fonts/Fonts.h"

#include "TimeDateSettingsDLG.h"
#include <UEZTimeDate.h>
#include "TemperatureGraph.h"


/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define ID_WINDOW               (GUI_ID_USER + 0x00)
#define ID_BUTTON_CLOSE         (GUI_ID_USER + 0x01)

#define ID_UPDATE_TIMER         (GUI_ID_USER + 0x03)
#define ID_BUTTON_UP		(GUI_ID_USER + 0x04)
#define ID_BUTTON_DOWN		(GUI_ID_USER + 0x05)
#define ID_TIMELABEL		(GUI_ID_USER + 0x06)
#define ID_DATELABEL		(GUI_ID_USER + 0x07)
#define ID_COLON_ONE		(GUI_ID_USER + 0x08)
#define ID_COLON_TWO		(GUI_ID_USER + 0x09)
#define ID_SLASH_ONE		(GUI_ID_USER + 0x0A)
#define ID_SLASH_TWO		(GUI_ID_USER + 0x0B)

#define ID_BUTTON_HOUR		(GUI_ID_USER + 0x0C)
#define ID_BUTTON_MINUTE	(GUI_ID_USER + 0x0D)
#define ID_BUTTON_SECOND	(GUI_ID_USER + 0x0E)
#define ID_BUTTON_MONTH		(GUI_ID_USER + 0x0F)
#define ID_BUTTON_DAY		(GUI_ID_USER + 0x10)
#define ID_BUTTON_YEAR		(GUI_ID_USER + 0x11)

#define ID_TEXT_AM_PM		(GUI_ID_USER + 0x12)
#define ID_SHOW_FOR_TIMER	(GUI_ID_USER + 0x13)




#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define DIVIDER                 (10)
#else
#define DIVIDER                 (5)
#endif

#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING                 (10)
#else
#define SPACING                 (5)
#endif

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH/2)//previously defined as (UEZ_LCD_DISPLAY_WIDTH / 2), the new method works better for scaling
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT/2)//previously defined as (UEZ_LCD_DISPLAY_HEIGHT / 2), the new method works better for scaling
#define WINDOW_XPOS             ((UEZ_LCD_DISPLAY_WIDTH / 2) - (WINDOW_XSIZE / 2))
#define WINDOW_YPOS             ((UEZ_LCD_DISPLAY_HEIGHT / 2) - (WINDOW_YSIZE / 2))

#define TITLE_XPOS		(WINDOW_XSIZE/5.5)
#define TITLE_YPOS		(10)
#define TITLE_XSIZE		(100)
#define TITLE_YSIZE		(25)

#define LABEL_SMALL_XSIZE	(30)
#define LABEL_SMALL_YSIZE       (30)

#define CLOSE_XSIZE             ((WINDOW_XSIZE/6))           //Code removed by IMM - Changing the back button location
#define CLOSE_YSIZE             ((WINDOW_YSIZE/8))           //Code removed by IMM - Changing the back button location
#define CLOSE_XPOS              (SPACING)                    //Code removed by IMM - Changing the back button location
#define CLOSE_YPOS              (WINDOW_YSIZE - SPACING - CLOSE_YSIZE)  //(WINDOW_YSIZE - CLOSE_YSIZE - SPACING)  //Code removed by IMM - Changing the back button location

#define UP_XPOS			(WINDOW_XSIZE/4)
#define UP_YPOS			(WINDOW_YSIZE/1.3)
	
#define DOWN_XPOS		(WINDOW_XSIZE/1.6)
#define DOWN_YPOS		(WINDOW_YSIZE/1.3)

#define BUTTON_XSIZE		(WINDOW_XSIZE/DIVIDER)/1.2
#define BUTTON_YSIZE		(WINDOW_YSIZE/DIVIDER)/1.5

#define TIMEDATE_BUTTON_XSIZE	(50)//originally 30
#define TIMEDATE_BUTTON_YSIZE	(50)//originally 30

#define UPDATE_TIMER_MS         (1000)
#define SHOW_FOR_TIME_MS        (2000)  

static WM_HWIN hItem;//Assign this as global as well?
static int Id, NCode;//Assign them as global?

TBool G_AM_PM_Flag;
TBool G_IsPM_Flag = EFalse;
extern T_TemperatureGraphSetting G_AppSettings;

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
static TBool IHandleClose(WM_MESSAGE * pMsg, int aNCode, int aID);
static void IUpdateFields(WM_MESSAGE * pMsg);
static TBool IHandleTimeDate(WM_MESSAGE *pMsg, int aNCode, int aID);
static TBool IHandleUP(WM_MESSAGE * pMsg, int aNCode, int aID);
static TBool IHandleDOWN(WM_MESSAGE * pMsg, int aNCode, int aID);


/*---------------------------------------------------------------------------*
 * Local Data:
 *---------------------------------------------------------------------------*/
/** Structure to hold all of the widgets used in this dialog*/
static const GUI_WIDGET_CREATE_INFO _iTimeDateSettingsDLGDialog[] = {
    //Function			          Name	                  ID                     XP				YP                      XS                   YS
    { WINDOW_CreateIndirect		, ""			, ID_WINDOW              ,WINDOW_XPOS			,WINDOW_YPOS		,WINDOW_XSIZE		,WINDOW_YSIZE	,0, 0, 0},
    { BUTTON_CreateIndirect             , ""			, ID_BUTTON_CLOSE        ,CLOSE_XPOS			,CLOSE_YPOS		,CLOSE_XSIZE		,CLOSE_YSIZE    ,0, 0, 0},
    { BUTTON_CreateIndirect		, "UP"			, ID_BUTTON_UP		 ,UP_XPOS			,UP_YPOS		,BUTTON_XSIZE+50        ,BUTTON_YSIZE+30,0,	0,	0 },
    { BUTTON_CreateIndirect		, "DOWN"		, ID_BUTTON_DOWN	 ,DOWN_XPOS			,DOWN_YPOS		,BUTTON_XSIZE+50        ,BUTTON_YSIZE+30,0,	0,	0 },
    { TEXT_CreateIndirect		,"Date:"		, ID_DATELABEL		 ,TITLE_XPOS			,TITLE_YPOS+75		,TITLE_XSIZE		,TITLE_YSIZE,0 ,0 ,0 },
    { TEXT_CreateIndirect		,":"			, ID_COLON_ONE		 ,TITLE_XPOS*1.75		,TITLE_YPOS+30		,LABEL_SMALL_XSIZE	,LABEL_SMALL_YSIZE,0 ,0 ,0 },
    { TEXT_CreateIndirect		,":"			, ID_COLON_TWO		 ,TITLE_XPOS*2.75		,TITLE_YPOS+30		,LABEL_SMALL_XSIZE	,LABEL_SMALL_YSIZE,0 ,0 ,0 },
    { TEXT_CreateIndirect		,"/"			, ID_SLASH_ONE		 ,TITLE_XPOS*1.75		,TITLE_YPOS+105		,LABEL_SMALL_XSIZE	,LABEL_SMALL_YSIZE,0 ,0 ,0 },
    { TEXT_CreateIndirect		,"/"			, ID_SLASH_TWO		 ,TITLE_XPOS*2.75		,TITLE_YPOS+105		,LABEL_SMALL_XSIZE	,LABEL_SMALL_YSIZE,0 ,0 ,0 },
    { BUTTON_CreateIndirect		, ""			, ID_BUTTON_HOUR	 ,TITLE_XPOS			,TITLE_YPOS+30		,TIMEDATE_BUTTON_XSIZE	,TIMEDATE_BUTTON_YSIZE,0,	0,	0 },
    { BUTTON_CreateIndirect		, ""			, ID_BUTTON_MINUTE	 ,TITLE_XPOS*2                  ,TITLE_YPOS+30		,TIMEDATE_BUTTON_XSIZE	,TIMEDATE_BUTTON_YSIZE,0,	0,	0 },
    { BUTTON_CreateIndirect		, ""			, ID_BUTTON_SECOND	 ,TITLE_XPOS*3			,TITLE_YPOS+30		,TIMEDATE_BUTTON_XSIZE	,TIMEDATE_BUTTON_YSIZE,0,	0,	0 },
    { TEXT_CreateIndirect		, ""			, ID_TEXT_AM_PM		 ,TITLE_XPOS*4  		,TITLE_YPOS+35		,TIMEDATE_BUTTON_XSIZE	,TIMEDATE_BUTTON_YSIZE,0,	0,	0 },
    { BUTTON_CreateIndirect		, ""			, ID_BUTTON_MONTH	 ,TITLE_XPOS			,TITLE_YPOS+105		,TIMEDATE_BUTTON_XSIZE	,TIMEDATE_BUTTON_YSIZE,0,	0,	0 },
    { BUTTON_CreateIndirect		, ""			, ID_BUTTON_DAY		 ,TITLE_XPOS*2		        ,TITLE_YPOS+105		,TIMEDATE_BUTTON_XSIZE	,TIMEDATE_BUTTON_YSIZE,0,	0,	0 },
    { BUTTON_CreateIndirect		, ""			, ID_BUTTON_YEAR	 ,TITLE_XPOS*2.95               ,TITLE_YPOS+105		,TIMEDATE_BUTTON_XSIZE*2,TIMEDATE_BUTTON_YSIZE,0,	0,	0 },
    { TEXT_CreateIndirect	        , ""			, ID_TIMELABEL		 ,TITLE_XPOS			,TITLE_YPOS	        ,TITLE_XSIZE*2		,TITLE_YSIZE,0 ,0 ,0 },
};

/** Generic Mapping of Screen Layout*/
static T_LAFMapping TimeDateSettingsDLGMapping[] = {
    { ID_WINDOW         ,""       , GUI_DARKGRAY      , GUI_BLACK         , &GUI_Font13_ASCII  , LAFSetupWindow  , 0},
    { ID_BUTTON_CLOSE   , "Back"  , GUI_GRAY          , GUI_BLACK         , &GUI_Font13_ASCII  , LAFSetupButton  , IHandleClose},
    { ID_BUTTON_UP	, "UP"	  , GUI_WHITE	      , GUI_BLACK	  , &GUI_Font24_ASCII  , LAFSetupButton  , IHandleUP},
    { ID_BUTTON_DOWN	, "DOWN"  , GUI_WHITE	      , GUI_BLACK	  , &GUI_Font24_ASCII  , LAFSetupButton  , IHandleDOWN},
    { ID_DATELABEL	, "Date:" , GUI_DARKGRAY      , GUI_BLACK	  , &GUI_Font32B_ASCII , LAFSetupText    , 0},
    { ID_COLON_ONE	, ":"	  , GUI_DARKGRAY      , GUI_BLACK	  , &GUI_Font32B_ASCII , LAFSetupText	 , 0},
    { ID_COLON_TWO	, ":"	  , GUI_DARKGRAY      , GUI_BLACK	  , &GUI_Font32B_ASCII , LAFSetupText	 , 0},
    { ID_SLASH_ONE	, "/"	  , GUI_DARKGRAY      , GUI_BLACK	  , &GUI_Font32B_ASCII , LAFSetupText	 , 0},
    { ID_SLASH_TWO	, "/"	  , GUI_DARKGRAY      , GUI_BLACK	  , &GUI_Font32B_ASCII , LAFSetupText	 , 0},
    { ID_BUTTON_HOUR	, ""	  , GUI_WHITE	      , GUI_BLACK	  , &GUI_Font24_ASCII  , LAFSetupButton  , IHandleTimeDate},
    { ID_BUTTON_MINUTE	, ""	  , GUI_WHITE	      , GUI_BLACK	  , &GUI_Font24_ASCII  , LAFSetupButton  , IHandleTimeDate},
    { ID_BUTTON_SECOND	, ""	  , GUI_WHITE	      , GUI_BLACK	  , &GUI_Font24_ASCII  , LAFSetupButton  , IHandleTimeDate},
    { ID_TEXT_AM_PM	, ""	  , GUI_DARKGRAY      , GUI_BLACK	  , &GUI_Font24_ASCII  , LAFSetupText    , 0},
    { ID_BUTTON_MONTH	, ""	  , GUI_WHITE	      , GUI_BLACK	  , &GUI_Font24_ASCII  , LAFSetupButton  , IHandleTimeDate},
    { ID_BUTTON_DAY     , ""      , GUI_WHITE	      , GUI_BLACK	  , &GUI_Font24_ASCII  , LAFSetupButton  , IHandleTimeDate},
    { ID_BUTTON_YEAR	, ""	  , GUI_WHITE	      , GUI_BLACK	  , &GUI_Font24_ASCII  , LAFSetupButton	 , IHandleTimeDate},
    { ID_TIMELABEL	, ""	  , GUI_DARKGRAY      , GUI_BLACK	  , &GUI_Font32B_ASCII , LAFSetupText    , 0},
    {0},
};

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;
static WM_HTIMER G_UpdateTimer;
static T_uezTimeDate G_TimeDate;
static WM_HTIMER G_ShowForTimer;
static TUInt32 G_SelectedButton=0;

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*/
/** 
 *      
 *  @param [in] pMsg	    WM_MESSAGE structure used by emWin to
 *							to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID			ID of widget that caused the event.
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleClose(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    if (aNCode == WM_NOTIFICATION_RELEASED) {
        GUI_EndDialog(pMsg->hWin, 1);
    }
    return EFalse;
}


//IDisableButton: used to freeze a clicked button in it's pressed state
static void IDisableButton(WM_MESSAGE *pMsg, TUInt32 aButtonID)
{
	WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CLOSE));//If the close button is hit while a timer is in use, the program crashes
									 //This insures that the user is unable to exit while a timer is used
    switch(aButtonID){
        case ID_BUTTON_HOUR:		//Disables the button in it's pressed state and then allows up and down to be manipulated
            WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR));
	    G_SelectedButton=1;
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UP));
	    WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DOWN));
            break;
        case ID_BUTTON_MINUTE:		//Disables the button in it's pressed state and then allows up and down to be manipulated
            WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MINUTE));
	    G_SelectedButton=2;
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UP));
	    WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DOWN));
            break;
        case ID_BUTTON_SECOND:		//Disables the button in it's pressed state and then allows up and down to be manipulated
	    WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SECOND));
	    G_SelectedButton=3;
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UP));
	    WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DOWN));
            break;
       case ID_BUTTON_MONTH:		//Disables the button in it's pressed state and then allows up and down to be manipulated
	    WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MONTH));
	    G_SelectedButton=4;
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UP));
	    WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DOWN));
            break;

       case ID_BUTTON_DAY:		//Disables the button in it's pressed state and then allows up and down to be manipulated
	    WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DAY));
	    G_SelectedButton=5;
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UP));
	    WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DOWN));
            break;

       case ID_BUTTON_YEAR:		//Disables the button in it's pressed state and then allows up and down to be manipulated
	    WM_DisableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_YEAR));
	    G_SelectedButton=6;
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UP));
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DOWN));
            break;

        default:					//For when anything else is sent, we want to ensure all buttons are enabled

            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_CLOSE));
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR));
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MINUTE));
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SECOND));
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MONTH));
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DAY));
            WM_EnableWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_YEAR));

            break;
    }
}



/*---------------------------------------------------------------------------*
 * Routine: IHandleUP
 *---------------------------------------------------------------------------*/
/** Show the temperature screen
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID            ID of widget that caused the event.
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleUP(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    int monthMax;
    T_uezTimeDate aTimeDate;
	T_uezDate aDate;
	T_uezTime aTime;

	UEZTimeDateGet(&aTimeDate);

          aDate.iDay=aTimeDate.iDate.iDay;
          aDate.iMonth=aTimeDate.iDate.iMonth;
          aDate.iYear=aTimeDate.iDate.iYear;

          aTime.iHour=aTimeDate.iTime.iHour;
          aTime.iMinute=aTimeDate.iTime.iMinute;
          aTime.iSecond=aTimeDate.iTime.iSecond;

	if (aNCode == WM_NOTIFICATION_RELEASED) {
		switch(G_SelectedButton){
		case 1:
                        aTime.iHour++;
			//UEZTimeNextHour(&aTime);//Increment the current number
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;
		case 2:
                        aTime.iMinute++;
			//UEZTimeNextMinute(&aTime);//Increment the current number
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;
		case 3:
                        aTime.iSecond++;
			//UEZTimeNextSecond(&aTime);//Increment the current number
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;
		case 4:
			aDate.iMonth++;//Increment the current number
			//UEZDateNextMonth(&aDate);
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;
		case 5:
			aDate.iDay++;//Increment the current number
			//UEZDateNextDay(&aDate);
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;
		case 6:
			aDate.iYear++;//Increment the current number
			//UEZDateNextYear(&aDate);
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;
		default:
			break;
			}
      //***************************************************************************************          
      //Code by IMM - This switch statement allows the program to keep track of the last day of
      //              a given month so that when the user goes past it, the day will reset to 1
      //              and increment the month.
      //***************************************************************************************          
                   switch(aDate.iMonth){
                          case 4:
                          case 6:
                          case 9:
                          case 11:
                          monthMax = 30;
                                  break;       
                          case 1:
                          case 3:
                          case 5:
                          case 7:
                          case 8:
                          case 10:
                          case 12:  
                          monthMax = 31;
                                  break;
                          case 2:
                          if (atoi(aDate.iYear)%400 == 0){
                               monthMax=29;
                            }
                            else if (atoi(aDate.iYear)%100 == 0){
                               monthMax=28;
                            }
                            else if (atoi(aDate.iYear)%4 == 0){
                               monthMax=29;
                            }
                            else{
                               monthMax=28;
                            }
                                  break;
                          default:  
                                  break;
                  }
                
      //***************************************************************************************          
      //Code by IMM - Error hanldling for Maximum and minimum dates and times
      //***************************************************************************************
                if(aTime.iHour > 23){
                    aTime.iHour = 0;
                    aDate.iDay++;
                }               
                if(aTime.iMinute > 59){
                    aTime.iMinute = 0;
                    aTime.iHour++;
                }
                if(aTime.iSecond > 59){
                    aTime.iSecond = 0;
                    aTime.iMinute++;
                }
                if(aDate.iDay > monthMax){
                  aDate.iDay=1;
                  aDate.iMonth++;
                }
                if(aDate.iMonth > 12){
                  aDate.iMonth = 1;
                  aDate.iYear++;
                }
                if(aDate.iYear >= 3000){
                  aDate.iYear=3000;
                }
                if(aDate.iYear < 2016){
                  aDate.iYear=2016;
                }
                
		//Puts the newly modified Time and Date back into TimeDate
		aTimeDate.iDate.iDay=aDate.iDay;
		aTimeDate.iDate.iMonth=aDate.iMonth;
		aTimeDate.iDate.iYear=aDate.iYear;

		aTimeDate.iTime.iHour=aTime.iHour;
		aTimeDate.iTime.iMinute=aTime.iMinute;
		aTimeDate.iTime.iSecond=aTime.iSecond;
		//********************************************************

		UEZTimeDateSet(&aTimeDate);

		 IUpdateFields(pMsg);
	}

    return EFalse;	
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleDOWN
 *---------------------------------------------------------------------------*/
/** Show the temperature screen
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID            ID of widget that caused the event.
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
static TBool IHandleDOWN(WM_MESSAGE * pMsg, int aNCode, int aID)
{
    int monthMax;
    T_uezTimeDate aTimeDate;
	T_uezDate aDate;
	T_uezTime aTime;

	UEZTimeDateGet(&aTimeDate);

		aDate.iDay=aTimeDate.iDate.iDay;
		aDate.iMonth=aTimeDate.iDate.iMonth;
		aDate.iYear=aTimeDate.iDate.iYear;

		aTime.iHour=aTimeDate.iTime.iHour;
		aTime.iMinute=aTimeDate.iTime.iMinute;
		aTime.iSecond=aTimeDate.iTime.iSecond;

	if (aNCode == WM_NOTIFICATION_RELEASED) {
		switch(G_SelectedButton){
		case 1:
			//UEZTimePreviousHour(&aTime);//Deincrement the current number
                        aTime.iHour--;
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;
		case 2:
			//UEZTimePreviousMinute(&aTime);//Deincrement the current number
                        aTime.iMinute--;
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;

		case 3:
			//UEZTimePreviousSecond(&aTime);//Deincrement the current number
                        aTime.iSecond--;
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;

		case 4:
			//UEZDatePreviousMonth(&aDate);
                        aDate.iMonth--;
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;

		case 5:
			//UEZDatePreviousDay(&aDate);
                        aDate.iDay--;
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;

		case 6:
			//UEZDatePreviousYear(&aDate);
                        aDate.iYear--;
			WM_RestartTimer(G_ShowForTimer, SHOW_FOR_TIME_MS);
			break;

		default:
			
			break;
			}

             if(aTime.iHour > 23){
                    aTime.iHour = 23;
                    aDate.iDay--;
             }
             if(aTime.iMinute > 59){
                    aTime.iMinute = 59;
                    aTime.iHour--;
             }
             if(aTime.iSecond > 59){
                    aTime.iSecond = 59;
                    aTime.iMinute--;
             }
                
             if(aDate.iDay==0)
             {     
                  aDate.iMonth--;  
                  //***************************************************************************************          
                  //Code by IMM - This switch statement allows the program to keep track of the last day of
                  //              a given month so that when the user goes past it, the day will set to the 
                  //              last day of the month and deincrement the month.
                  //***************************************************************************************  
                   switch(aDate.iMonth){
                          case 4:
                          case 6:
                          case 9:
                          case 11:
                          monthMax = 30;
                                  break;       
                  
                          case 1:
                          case 3:
                          case 5:
                          case 7:
                          case 8:
                          case 10:
                          case 0:  
                          monthMax = 31;
                                  break;
                          
                          case 2:
                            if (atoi(aDate.iYear)%400 == 0){
                               monthMax=29;
                            }
                            else if (atoi(aDate.iYear)%100 == 0){
                               monthMax=28;
                            }
                            else if (atoi(aDate.iYear)%4 == 0){
                               monthMax=29;
                            }
                            else{
                               monthMax=28;
                            }
                                  break;
                   
                          default:
                                  
                                  break;
                                  }
                  aDate.iDay = monthMax;
             }
                
             if(aDate.iMonth < 1){
                  aDate.iMonth = 12;
                  aDate.iYear--;
             }   
             if(aDate.iYear<2016){
                  aDate.iYear=2016;
             }
                
		//Puts the newly modified Time and Date back into TimeDate
		aTimeDate.iDate.iDay=aDate.iDay;
		aTimeDate.iDate.iMonth=aDate.iMonth;
		aTimeDate.iDate.iYear=aDate.iYear;

		aTimeDate.iTime.iHour=aTime.iHour;
		aTimeDate.iTime.iMinute=aTime.iMinute;
		aTimeDate.iTime.iSecond=aTime.iSecond;
		//********************************************************

		UEZTimeDateSet(&aTimeDate);
		 IUpdateFields(pMsg);
	}

    return EFalse;
}


/*---------------------------------------------------------------------------*
 * Routine: IUpdateFields
 *---------------------------------------------------------------------------*/
/** Update all the fields on the screen.
 *      
 *  @param [in] pMsg	    WM_MESSAGE structure used by emWin to
 *							to handle screen information.
 */
 /*---------------------------------------------------------------------------*/
static void IUpdateFields(WM_MESSAGE * pMsg)
{
	T_uezTimeDate TimeDate;
	char message[12];

	UEZTimeDateGet(&TimeDate);
        
	//Sets the Title of the Dialog box based on what state AM/PM is in
	if(G_AM_PM_Flag==ETrue){
		TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TIMELABEL), "Time: AM/PM");

			if(TimeDate.iTime.iHour>12){
				sprintf(message, "%02d", TimeDate.iTime.iHour-12);
				BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), message);//Displays the active time minus 12 to accomodate AM/PM change
				G_IsPM_Flag=ETrue;
		        }else{
				TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "AM");
				G_IsPM_Flag=EFalse;
			}
	}else {
	TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TIMELABEL), "Time: 24-Hour");
	}
	
	UEZTimeDateSet(&TimeDate);//Once changes are implemented, then set the time and date

	sprintf(message, "%02d", TimeDate.iTime.iHour);
	BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), message);	

        sprintf(message, "%02d", TimeDate.iTime.iMinute);
	BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MINUTE), message);

	sprintf(message, "%02d", TimeDate.iTime.iSecond);
	BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_SECOND), message);
	
	sprintf(message, "%02d", TimeDate.iDate.iMonth);
	BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_MONTH), message);
	
	sprintf(message, "%02d", TimeDate.iDate.iDay);
	BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DAY), message);
	
	sprintf(message, "%02d", TimeDate.iDate.iYear);
	BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_YEAR), message);

	if(G_AM_PM_Flag==ETrue){
		switch(TimeDate.iTime.iHour){	//This switch is to display the proper text on the button during AM/PM
										//The actual conversions are already done elsewhere in the program
		case 0:						   
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "12");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "AM");
			break;
		case 12:
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;

		case 13:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "1");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;
		case 14:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "2");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;

		case 15:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "3");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;

		case 16:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "4");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;

		case 17:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "5");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;

		case 18:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "6");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;
		
		case 19:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "7");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;

		case 20:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "8");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;

		case 21:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "9");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;

		case 22:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "10");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;
			
		case 23:
			BUTTON_SetText(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_HOUR), "11");
			TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_AM_PM), "PM");
			break;

		default:
			
			break;
			}
		}
}

/*---------------------------------------------------------------------------*
 * Routine: IHandleTimeDate
 *---------------------------------------------------------------------------*/
/** Show the temperature screen
 *
 *  @param [in] pMsg        WM_MESSAGE structure used by emWin to
 *                            to handle screen information.
 *  @param [in] aNCode      Notification code of event.
 *  @param [in] aID            ID of widget that caused the event.
 *  @return                    The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/

static TBool IHandleTimeDate(WM_MESSAGE * pMsg, int aNCode, int aID)
{
	if (aNCode == WM_NOTIFICATION_RELEASED) {
        WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UP));
	WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DOWN));
        if(!G_ShowForTimer){
            G_ShowForTimer = WM_CreateTimer(pMsg->hWin, ID_SHOW_FOR_TIMER, SHOW_FOR_TIME_MS, 0);
            IDisableButton(pMsg, aID);
        }
    }
    return EFalse;
}


/*---------------------------------------------------------------------------*
 * Routine:	_TimeDateSettingsDLG.cDialog
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback function used by emWin to process events.
 * Inputs:
 *      WM_MESSAGE *pMsg -- message structure for current dialog.
 *---------------------------------------------------------------------------*/
static void _TimeDateSettingsDLGDialog(WM_MESSAGE *pMsg)
{
    int Id, NCode;

    switch (pMsg->MsgId){
    case WM_INIT_DIALOG:
        Id    = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
		WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UP));
		WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DOWN));
		IUpdateFields(pMsg);
        LAFSetup(pMsg->hWin, ID_WINDOW, TimeDateSettingsDLGMapping);
	
        IUpdateFields(pMsg);
        G_UpdateTimer = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER, UPDATE_TIMER_MS, 0);
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);
        NCode = pMsg->Data.v;
        if( !LAFHandleEvent(TimeDateSettingsDLGMapping, pMsg, NCode, Id)){
            //Handle special cases here
        }
        break;
    case WM_TIMER:
        NCode = pMsg->Data.v;
        if(NCode == G_UpdateTimer){
            IUpdateFields(pMsg);
            WM_RestartTimer(G_UpdateTimer, UPDATE_TIMER_MS);
        }
		if(NCode == G_ShowForTimer)
		{
			if(G_Active){
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_UP));
			WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_BUTTON_DOWN));
                        WM_DeleteTimer(G_ShowForTimer);
                        G_ShowForTimer = 0;
                        IDisableButton(pMsg, 0);
			G_SelectedButton=0;
			}
		}
		break;
	case WM_PRE_PAINT:

        break;
    case WM_POST_PAINT:
        break;
    case WM_APP_GAINED_FOCUS:
    	G_Active = ETrue;
    	break;
    case WM_APP_LOST_FOCUS:
    	G_Active = EFalse;
    	break;
    default:
      WM_DefaultProc(pMsg);
    break;
  }
}

/*---------------------------------------------------------------------------*
 * Routine:	TimeDateSettingsDLG
 *---------------------------------------------------------------------------*/
/** Create the TimeDateSettingsDLG. This function should only be called by the
 *  window manager and never by user code.
 *      
 *  @param [in] p_choice    void
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
void TimeDateSettingsDLG()
{
	G_Active=ETrue;
    GUI_ExecDialogBox(_iTimeDateSettingsDLGDialog, GUI_COUNTOF(_iTimeDateSettingsDLGDialog), &_TimeDateSettingsDLGDialog, 0,0,0);
	G_Active=EFalse;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:    TimeDateSettingsDLG.c
 *-------------------------------------------------------------------------*/
