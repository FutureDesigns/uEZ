/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.12                          *
*        Compiled Jun 29 2011, 15:28:47                              *
*        (c) 2011 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
#include "TemperatureGraph.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "CHECKBOX.h"
#include <uEZTimeDate.h>
#include "Graphics/Graphics.h"
#include <DIALOG.h>

// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

#define ID_WINDOW_0             (GUI_ID_USER + 0x00)

#define ID_BUTTON_0		(GUI_ID_USER + 0x01)
#define ID_TEXT_0		(GUI_ID_USER + 0x02)


#define ID_CHECKBOX_0		(GUI_ID_USER + 0x03)
#define ID_CHECKBOX_1		(GUI_ID_USER + 0x04)
#define ID_CHECKBOX_2		(GUI_ID_USER + 0x10)
#define ID_CHECKBOX_3		(GUI_ID_USER + 0x11)

#define ID_TEXT_1		(GUI_ID_USER + 0x05)
#define ID_TEXT_2		(GUI_ID_USER + 0x06)
#define ID_TEXT_3		(GUI_ID_USER + 0x07)
#define ID_TEXT_4		(GUI_ID_USER + 0x08)
#define ID_TEXT_5		(GUI_ID_USER + 0x09)
#define ID_BUTTON_1		(GUI_ID_USER + 0x0A)
#define ID_BUTTON_2	        (GUI_ID_USER + 0x0B)
#define ID_BUTTON_3	        (GUI_ID_USER + 0x0C)
#define ID_BUTTON_4	        (GUI_ID_USER + 0x0D)
#define ID_TEXT_6		(GUI_ID_USER + 0x0E)
#define ID_TEXT_7		(GUI_ID_USER + 0x0F)
#define ID_BUTTON_5		(GUI_ID_USER + 0x12)

#define ID_BUTTON_6		(GUI_ID_USER + 0x14)
#define ID_BUTTON_7		(GUI_ID_USER + 0x15)

#define ID_TEXT_8               (GUI_ID_USER + 0x16)
#define ID_BANNER_BITMAP        (GUI_ID_USER + 0x17)


//**************************************************
//New #define macros for Position
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

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define WINDOW_XPOS             (0)
#define WINDOW_YPOS             (0)

#define WINDOW_XCENTER		(UEZ_LCD_DISPLAY_WIDTH/7)
#define WINDOW_YCENTER		(UEZ_LCD_DISPLAY_HEIGHT/7)

#define TITLE_XSIZE             (WINDOW_XSIZE-4)
#define TITLE_YSIZE             (WINDOW_YSIZE / 10)

#define TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define TITLE_TEXT_YSIZE        (((WINDOW_YSIZE-80)/10))
#define TITLE_TEXT_XPOS         (0)
#define TITLE_TEXT_YPOS         (0)

#define DIALOG_LOCATION_Y       (WINDOW_XSIZE/3.5)
#define DIALOG_LOCATION_X	(WINDOW_YSIZE/2)

/*  Code removed by IMM - Changed spacing and size perameters
#define BACK_BUTTON_XSIZE       75
#define BACK_BUTTON_YSIZE       50
#define BACK_BUTTON_XPOS        3//((WINDOW_XSIZE-BACK_BUTTON_XSIZE)-SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - BACK_BUTTON_YSIZE - SPACING)
*/

// Code added by IMM - Changed spacing perameters to consistent format
//***********************************************
#define BACK_BUTTON_XSIZE       ((WINDOW_XSIZE/6))
#define BACK_BUTTON_YSIZE       ((WINDOW_YSIZE/8))
#define BACK_BUTTON_XPOS        (SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - SPACING - BACK_BUTTON_YSIZE)
//***********************************************

#define TEXT_XSIZE              ((WINDOW_XSIZE - (8 * SPACING))/5)//currently =144
#define TEXT_YSIZE              ((WINDOW_YSIZE - TITLE_TEXT_YSIZE - (5 * SPACING))/13)//currently=30
#define LARGE_TEXT_XSIZE        (4*((WINDOW_XSIZE - (8 * SPACING))/5))

#define TEXT_XPOS(n)		(WINDOW_XSIZE/2)*n
#define TEXT_YPOS(n)		(((WINDOW_YSIZE-TITLE_TEXT_YSIZE)/2)*n)

#define CHECKBOX_XSIZE		100
#define CHECKBOX_YSIZE		20

#define BUTTON_XSIZE		(TUInt32)((WINDOW_XSIZE/DIVIDER)/1.2)
#define BUTTON_YSIZE		(TUInt32)((WINDOW_YSIZE/DIVIDER)/1.5)

#define BUTTON_XPOS(n)		(SPACING + (n * (SPACING + TEXT_XSIZE)))
#define BUTTON_YPOS(n)		(((WINDOW_YSIZE - TITLE_TEXT_YSIZE)/2 - (TEXT_YSIZE/2)) + ( n * (TEXT_YSIZE + SPACING)))

#define     TEMP_HIGH           32
#define     TEMP_LOW            10

//**************************************************

WM_HWIN hItem;//Assign this as global as well?
int Id, NCode;//Assign them as global?


T_uezTimeDate TimeDate;

extern TBool G_AM_PM_Flag;      //Code added by IMM - Used to manage AM/PM and 24 hour Checkbox activation
extern TBool FIRST_EXEC_Flag = ETrue;   //Code added by IMM - Used to set the Low and High Temperature defaults upon first execution of the window.

// USER START (Optionally insert additional defines)
extern T_TemperatureGraphSetting G_AppSettings;

// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static void ISetButtonIcons(WM_MESSAGE * pMsg);

// USER START (Optionally insert additional static data)
//static T_uezTimeDate G_TimeDate;
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/	
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  //Function****************** |Text********************************|ID**************|XP************************|YP**************************|XS*********************|YS**************************|Flags***|Perameters**|Extra Bytes								
  { WINDOW_CreateIndirect      , "TemperatureGraph_Setting"         , ID_WINDOW_0    , WINDOW_XPOS	        , WINDOW_YPOS	             , WINDOW_XSIZE          , WINDOW_YSIZE               , 0      , 0          , 0 },
  { BUTTON_CreateIndirect      , "Back"                             , ID_BUTTON_0    , BACK_BUTTON_XPOS         , BACK_BUTTON_YPOS           , BACK_BUTTON_XSIZE     , BACK_BUTTON_YSIZE          , 0      , 0          , 0 },
  { TEXT_CreateIndirect        , "Application Settings"             , ID_TEXT_0      , TITLE_TEXT_XPOS-10	, TITLE_TEXT_YPOS+10         , TITLE_XSIZE      , TITLE_YSIZE        , 0      , 0          , 0 },
  { TEXT_CreateIndirect        , "Time Format"                      , ID_TEXT_1      , TEXT_XPOS(.100)          , TEXT_YPOS(0.30)            , TEXT_XSIZE+50         , TEXT_YSIZE+50              , 0      , 0          , 0 },
  { TEXT_CreateIndirect        , "Temperature Scale"                , ID_TEXT_2      , TEXT_XPOS(.100)          , TEXT_YPOS(1.30)            , TEXT_XSIZE+120        , TEXT_YSIZE+50              , 0      , 0          , 0 },
  { TEXT_CreateIndirect        , "Temperature Alarm Settings"       , ID_TEXT_3      , TEXT_XPOS(1.00)          , TEXT_YPOS(0.50)            , LARGE_TEXT_XSIZE      , TEXT_YSIZE+50              , 0      , 0          , 0 },
  { TEXT_CreateIndirect        , ""                                 , ID_TEXT_8      , TEXT_XPOS(1.70)          , TEXT_YPOS(0.50)            , 100                   , 20                         , 0      , 0          , 0 },
  { TEXT_CreateIndirect        , "Low Temperature"                  , ID_TEXT_4      , TEXT_XPOS(1.00)          , TEXT_YPOS(0.75)            , TEXT_XSIZE+13         , TEXT_YSIZE+20              , 0      , 0          , 0 },
  { TEXT_CreateIndirect        , "High Temperature"                 , ID_TEXT_5      , TEXT_XPOS(1.00)          , TEXT_YPOS(1.00)            , TEXT_XSIZE+15         , TEXT_YSIZE+20              , 0      , 0          , 0 },
  { BUTTON_CreateIndirect      , "Up"                               , ID_BUTTON_1    , TEXT_XPOS(1.80)          , TEXT_YPOS(0.75)            , BUTTON_XSIZE          , BUTTON_YSIZE               , 0      , 0          , 0 },
  { BUTTON_CreateIndirect      , "UP"                               , ID_BUTTON_2    , TEXT_XPOS(1.80)          , TEXT_YPOS(1.00)            , BUTTON_XSIZE          , BUTTON_YSIZE               , 0      , 0          , 0 },
  { BUTTON_CreateIndirect      , "Down"                             , ID_BUTTON_3    , TEXT_XPOS(1.63)          , TEXT_YPOS(0.75)            , BUTTON_XSIZE          , BUTTON_YSIZE               , 0      , 0          , 0 },
  { BUTTON_CreateIndirect      , "Down"                             , ID_BUTTON_4    , TEXT_XPOS(1.63)          , TEXT_YPOS(1.00)            , BUTTON_XSIZE          , BUTTON_YSIZE               , 0      , 0          , 0 },
  { TEXT_CreateIndirect        , "30"                               , ID_TEXT_6      , TEXT_XPOS(1.37)          , TEXT_YPOS(0.75)            , TEXT_XSIZE            , TEXT_YSIZE                 , 0      , 0          , 0 },
  { TEXT_CreateIndirect        , "90"                               , ID_TEXT_7      , TEXT_XPOS(1.37)          , TEXT_YPOS(1.00)            , TEXT_XSIZE            , TEXT_YSIZE                 , 0      , 0          , 0 },
  { CHECKBOX_CreateIndirect    , "Time Format AM/PM"                , ID_CHECKBOX_0  , TEXT_XPOS(.100)          , (TUInt32)TEXT_YPOS(.95)    , CHECKBOX_XSIZE        , CHECKBOX_YSIZE             , 0      , 8962       , 0 },
  { CHECKBOX_CreateIndirect    , "Temp Scale Celsius"               , ID_CHECKBOX_1  , TEXT_XPOS(.100)          , (TUInt32)TEXT_YPOS(1.5)    , CHECKBOX_XSIZE        , CHECKBOX_YSIZE             , 0      , 8962       , 0 },
  { CHECKBOX_CreateIndirect    , "Time Format 24 Hour"              , ID_CHECKBOX_2  , TEXT_XPOS(.100)          , (TUInt32)TEXT_YPOS(.80)    , CHECKBOX_XSIZE        , CHECKBOX_YSIZE             , 0      , 8962       , 0 },
  { CHECKBOX_CreateIndirect    , "Temp Scale Fahrenheit"            , ID_CHECKBOX_3  , TEXT_XPOS(.100)          , (TUInt32)TEXT_YPOS(1.65)   , CHECKBOX_XSIZE+25     , CHECKBOX_YSIZE             , 0      , 8962       , 0 },
  { BUTTON_CreateIndirect      , "Time/Date Setup"                  , ID_BUTTON_5    , (TUInt32)TEXT_XPOS(0.500), (TUInt32)TEXT_YPOS(0.87)   , BUTTON_XSIZE+60       , BUTTON_YSIZE               , 0      , 0          , 0 },
  { BUTTON_CreateIndirect      , ""                                 , ID_BANNER_BITMAP       , BACK_BUTTON_XPOS+BACK_BUTTON_XSIZE+SPACING   , BACK_BUTTON_YPOS-10    ,(TITLE_XSIZE*2)-40          ,BACK_BUTTON_YSIZE+60, 0, 0, 0},

  // USER START (Optionally insert additional widgets)	
  // USER END
};


/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

static void IUpdateTeps(WM_MESSAGE * pMsg)
{
    char text[10];
    
    if(G_AppSettings.iTempIsFahrenheit==ETrue){
      if(G_AppSettings.iLowAlarmTemp <= -20){ //Code added by IMM - Set a Minimum temperature for which the iLowAlarmTemp can be set
          G_AppSettings.iLowAlarmTemp =  -20;  //Code added by IMM - Set a Minimum temperature for which the iLowAlarmTemp can be set
      }
      if(G_AppSettings.iHighAlarmTemp >= 120){ //Code added by IMM - Set a Maximum temperature for which the iHighAlarmTemp can be set
          G_AppSettings.iHighAlarmTemp =  120;        //Code added by IMM - Set a Maximum temperature for which the iHighAlarmTemp can be set
      }
    }
    if(G_AppSettings.iTempIsFahrenheit==EFalse){
      if(G_AppSettings.iLowAlarmTemp <= -28){ //Code added by IMM - Set a Minimum temperature for which the iLowAlarmTemp can be set
          G_AppSettings.iLowAlarmTemp =  -28;  //Code added by IMM - Set a Minimum temperature for which the iLowAlarmTemp can be set
      }
      if(G_AppSettings.iHighAlarmTemp >= 48){ //Code added by IMM - Set a Maximum temperature for which the iHighAlarmTemp can be set
          G_AppSettings.iHighAlarmTemp =  48;        //Code added by IMM - Set a Maximum temperature for which the iHighAlarmTemp can be set
      }
    }
    
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    sprintf(text, "%02d", G_AppSettings.iLowAlarmTemp);
    TEXT_SetText(hItem, text);

    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
    sprintf(text, "%02d", G_AppSettings.iHighAlarmTemp);
    TEXT_SetText(hItem, text);
}

//Code added by IMM - Added banner to the bottom of the page
//**********************************************************
static void ISetButtonIcons(WM_MESSAGE *pMsg)
{
    WM_HWIN hItem;
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BANNER_BITMAP);
    
    BUTTON_SetBkColor(hItem, BUTTON_CI_UNPRESSED, GUI_BLACK);
    //BUTTON_SetSkin(hItem, BUTTON_SKIN_FLEX);
    BUTTON_SetBitmapEx(hItem, BUTTON_CI_UNPRESSED, &bmBannerAdd, 0, 0);
    //BUTTON_SetBitmapEx(hItem, BUTTON_CI_PRESSED, &bmBannerAdd, 1, 1);
    //BUTTON_SetBitmapEx(hItem, BUTTON_CI_DISABLED, &bmBannerAdd, 0, 0);

}
//**********************************************************

//Used for updating the Celsius Checkbox-IMM
//**********************************************************
static void IUpdateTempFormatCel(WM_MESSAGE * pMsg)  
{
    WM_HWIN hItem;

    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
	if(CHECKBOX_GetState(hItem)==1){
		CHECKBOX_SetState(hItem,1);
	        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
		CHECKBOX_SetState(hItem,0);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
		G_AppSettings.iTempIsFahrenheit=EFalse;   
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
                TEXT_SetText(hItem,"(in Deg C)");
                
                //Imm to add code to update temperature to Celsius
            G_AppSettings.iLowAlarmTemp=((G_AppSettings.iLowAlarmTemp-32)*(.555555));
            G_AppSettings.iHighAlarmTemp=((G_AppSettings.iHighAlarmTemp-32)*(.555555));

                IUpdateTeps(pMsg);
                
        }else if(CHECKBOX_GetState(hItem)==0){
		CHECKBOX_SetState(hItem,0);										
	        hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
		CHECKBOX_SetState(hItem,1);
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
		G_AppSettings.iTempIsFahrenheit=ETrue;
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
                TEXT_SetText(hItem,"(in Deg F)");
                G_AppSettings.iLowAlarmTemp=((G_AppSettings.iLowAlarmTemp*(1.8))+32);
                G_AppSettings.iHighAlarmTemp=((G_AppSettings.iHighAlarmTemp*(1.8))+32);

                IUpdateTeps(pMsg);
                //Update the temperature to Fahrenheit
	}
}
//**********************************************************

//Used for updating Fahrenheit Checkbox
//**********************************************************
static void IUpdateTempFormatFah(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;
    
    //Update the temperature to Fahrenheit

    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
	if(CHECKBOX_GetState(hItem)==1){
	    CHECKBOX_SetState(hItem,1);									
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
	    CHECKBOX_SetState(hItem,0);
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
	    G_AppSettings.iTempIsFahrenheit=ETrue;
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
            TEXT_SetText(hItem,"(in Deg F)");
            G_AppSettings.iLowAlarmTemp=((G_AppSettings.iLowAlarmTemp*(1.8))+32);//Will Result in loss of data due to Integer/Float Conversion
            G_AppSettings.iHighAlarmTemp=((G_AppSettings.iHighAlarmTemp*(1.8))+32);
            IUpdateTeps(pMsg);
            //Imm to add code to update temperature to fahrenheit
	}

	else if(CHECKBOX_GetState(hItem)==0){
	    CHECKBOX_SetState(hItem,0);										
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
	    CHECKBOX_SetState(hItem,1);
            G_AppSettings.iTempIsFahrenheit=EFalse;
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
            TEXT_SetText(hItem,"(in Deg C)");
            G_AppSettings.iLowAlarmTemp=((G_AppSettings.iLowAlarmTemp-32)*(.555555));
            G_AppSettings.iHighAlarmTemp=((G_AppSettings.iHighAlarmTemp-32)*(.555555));


            IUpdateTeps(pMsg);
            //Imm to add code to update temperature to Celsius
	}
}
//**********************************************************

//Used for updating AM/PM Checkbox
//**********************************************************
static void IUpdateTimeFormatAMPM(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;

    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
	if(CHECKBOX_GetState(hItem)==1){
	    CHECKBOX_SetState(hItem,1);									
            hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);//24-Hour
	    CHECKBOX_SetState(hItem,0);
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);//AM/PM
	    G_AppSettings.iTimeIsAMPM=ETrue;
	    G_AM_PM_Flag=ETrue;	
        }
	else if(CHECKBOX_GetState(hItem)==0){
	    CHECKBOX_SetState(hItem,0);									
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);//24-Hour
	    CHECKBOX_SetState(hItem,1);
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);//AM/PM
	    G_AppSettings.iTimeIsAMPM=EFalse;
	    G_AM_PM_Flag=EFalse;
	}	
}
//**********************************************************

//Used for updating 24-Hour Checkbox
static void IUpdateTimeFormat24_Hour(WM_MESSAGE * pMsg)
{
    WM_HWIN hItem;

    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
	if(CHECKBOX_GetState(hItem)==1){
	    CHECKBOX_SetState(hItem,1);									
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);//AM/PM
	    CHECKBOX_SetState(hItem,0);
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);//24-Hour
            G_AppSettings.iTimeIsAMPM=EFalse;
	    G_AM_PM_Flag=EFalse;
        }
	else if(CHECKBOX_GetState(hItem)==0){
	    CHECKBOX_SetState(hItem,0);									
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);//AM/PM
	    CHECKBOX_SetState(hItem,1);
	    hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);//24-Hour
	    G_AppSettings.iTimeIsAMPM=ETrue;
	    G_AM_PM_Flag=ETrue;
	}
}

// USER START (Optionally insert additional static code)


static void _cbDialog(WM_MESSAGE * pMsg) {
  
  char text[10];  //Code added by IMM - Used to apply values for Low and High temperature upon initializiation of the program
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
     ISetButtonIcons(pMsg);  //Code added by IMM - Create FDI Banner on bottom of screen to show advertizing potential
    //
    // Initialization of 'TemperatureGraph_Setting'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_BLACK);

	//
    // Initialization of 'X'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
    //
    // Initialization of 'Application Settings'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetFont(hItem, GUI_FONT_32_ASCII );
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_WHITE);
    //
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_BOTTOM);
    TEXT_SetTextColor(hItem, GUI_WHITE);
    //
    // Initialization of 'Temperature Scale'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
    TEXT_SetTextColor(hItem, GUI_WHITE);
    //
    // Initialization of 'Temperature Alarm Settings (in Deg F)'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetTextColor(hItem, GUI_WHITE);
    TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
    //
    // Initialization of 'Low Temperature'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetTextColor(hItem, GUI_BLUE);
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);
    //
    // Initialization of 'High Temperature'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetTextColor(hItem, GUI_RED);
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);
    //
    // Initialization of 'Up'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
    BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
    //
    // Initialization of 'UP'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_2);
    BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
    //
    // Initialization of 'Down'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_3);
    BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
    //
    // Initialization of 'Down'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_4);
    BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
    //
    // Initialization of '30'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetTextColor(hItem, GUI_WHITE);
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);

    //
    // Initialization of '90'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_WHITE);
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);    
    
    //
    // Initialization of '(in Dec C)'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_WHITE);
    TEXT_SetFont(hItem, GUI_FONT_20B_ASCII);


    // USER START (Optionally insert additional code for further widget initialization)

	//Initialization of 'Time Checkboxes'-IM
	//**************************************************
	hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
	CHECKBOX_SetBkColor(hItem,GUI_BLACK);  
	CHECKBOX_SetText(hItem,"AM/PM");
	CHECKBOX_SetFont(hItem, GUI_FONT_20B_ASCII);
        CHECKBOX_SetTextColor(hItem, GUI_WHITE);
	


	hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
	CHECKBOX_SetBkColor(hItem,GUI_BLACK);
	CHECKBOX_SetText(hItem,"24-Hour");
	CHECKBOX_SetFont(hItem, GUI_FONT_20B_ASCII);
        CHECKBOX_SetTextColor(hItem, GUI_WHITE);
	//**************************************************

	//Initialization of 'Temp Checkboxes'-IM
	//**************************************************
	hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
	CHECKBOX_SetBkColor(hItem,GUI_BLACK);  
	CHECKBOX_SetText(hItem,"Celcius");
	CHECKBOX_SetFont(hItem, GUI_FONT_20B_ASCII);
        CHECKBOX_SetTextColor(hItem, GUI_WHITE);
 	

	hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
	CHECKBOX_SetBkColor(hItem,GUI_BLACK);
	CHECKBOX_SetText(hItem,"Fahrenheit");
	CHECKBOX_SetFont(hItem, GUI_FONT_20B_ASCII);
        CHECKBOX_SetTextColor(hItem, GUI_WHITE);
	//**************************************************

	//Date and Time Button Initialization
	//**************************************************
	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_5);
        BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_6);
	BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
	BUTTON_SetBkColor(hItem,BUTTON_CI_DISABLED,GUI_BLACK);

	hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_7);
	BUTTON_SetFont(hItem, GUI_FONT_13B_ASCII);
	BUTTON_SetBkColor(hItem,BUTTON_CI_DISABLED,GUI_BLACK);
	//**************************************************
        
    IUpdateTeps(pMsg);

	//This code checks the Checkbox of the statements that were last true
	//Insures that after Settings is left, if returned it is set to the previous set state.-IM
	//*******************************************************************

        if(G_AppSettings.iTimeIsAMPM==ETrue){					//Is time set to AM/PM?
		G_AM_PM_Flag=ETrue;
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_0);
		CHECKBOX_SetState(hItem,1);						
	}

	if(G_AppSettings.iTimeIsAMPM==EFalse){					//Is time set to 24-Hour?
		G_AM_PM_Flag=EFalse;
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_2);
		CHECKBOX_SetState(hItem,1);
	}

	if(G_AppSettings.iTempIsFahrenheit==ETrue){				//Is temperature set to Fahrenheit?
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_3);
		CHECKBOX_SetState(hItem,1);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
                TEXT_SetText(hItem,"(in Deg F)");                               //Convert the temperatures to Celsius
	}

	if(G_AppSettings.iTempIsFahrenheit==EFalse){				//Is temperature set to Celsius?
		hItem = WM_GetDialogItem(pMsg->hWin, ID_CHECKBOX_1);
		CHECKBOX_SetState(hItem,1);
                hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
                TEXT_SetText(hItem,"(in Deg C)");                               //Convert the temperatures to Celsius 
        }
	//*********************************************************************

        
        if(FIRST_EXEC_Flag == ETrue){  //Code added by IMM - Populates the Low and High temperature default values upon first execution of the program ONLY!
            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
            G_AppSettings.iLowAlarmTemp = TEMP_LOW;
            sprintf(text, "%02d", G_AppSettings.iLowAlarmTemp);
            TEXT_SetText(hItem, text);

            hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
            G_AppSettings.iHighAlarmTemp = TEMP_HIGH;
            sprintf(text, "%02d", G_AppSettings.iHighAlarmTemp);
            TEXT_SetText(hItem, text);
            FIRST_EXEC_Flag = EFalse; 
        }

    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'X'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)

          GUI_EndDialog(pMsg->hWin, 0);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_1: // Notifications sent by 'Up'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
          G_AppSettings.iLowAlarmTemp++;
          IUpdateTeps(pMsg);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_2: // Notifications sent by 'UP'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
          G_AppSettings.iHighAlarmTemp++;
          IUpdateTeps(pMsg);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_3: // Notifications sent by 'Down'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
          G_AppSettings.iLowAlarmTemp--;
          IUpdateTeps(pMsg);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    case ID_BUTTON_4: // Notifications sent by 'Down'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
          G_AppSettings.iHighAlarmTemp--;
          IUpdateTeps(pMsg);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END

       //THIS IS MY ADDED CODE TO TRY TO HANDLE THE NEW CHECKBOXES!*************************
      case ID_CHECKBOX_0: // Notifications sent by 'AM/PM'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
		  IUpdateTimeFormatAMPM(pMsg);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
         
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END

	  case ID_CHECKBOX_1: // Notifications sent by 'Farenheight'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
		  IUpdateTempFormatCel(pMsg);

        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
          
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END

	  case ID_CHECKBOX_2: // Notifications sent by '24 hour'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
		  IUpdateTimeFormat24_Hour(pMsg);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
		  
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END

	  case ID_CHECKBOX_3: // Notifications sent by 'Celsius'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
		  IUpdateTempFormatFah(pMsg);
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
          
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;

	  case ID_BUTTON_5: //Want this to activate the opening of a new window
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)

          TimeDateSettingsDLG();  //Sends the program to TimeDateSettingDLG.c
     			 
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)

      // USER END
      }
    // USER START (Optionally insert additional code for further Ids)

    // USER END

	  //************************************************************************************

    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       CreateTemperatureGraph_Setting
*/
WM_HWIN CreateTemperatureGraph_Setting(void);

WM_HWIN CreateTemperatureGraph_Setting(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)

// USER END

/*************************** End of file ****************************/