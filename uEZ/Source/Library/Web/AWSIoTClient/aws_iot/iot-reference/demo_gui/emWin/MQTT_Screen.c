/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 6.32                          *
*        Compiled Feb 16 2023, 09:25:51                              *
*        (c) 2023 SEGGER Microcontroller GmbH                        *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
#include "GUI.h"
#include "BUTTON.h"
#include "FRAMEWIN.h"
#include "IMAGE.h"
#include "TEXT.h"
#include "WM.h"
#include <string.h>
#include <stdio.h>

#include <uEZ.h>
//#include "LookAndFeel.h" // not setup yet in this demo screen
#include <emWin/WindowManager.h>
#include "Graphics/Graphics.h"
//#include "emWin/Fonts/Fonts.h" // not using the 3 font constants yet
#include "../MQTT_Callbacks.h"

#include <uEZPlatform.h>
#include <uEZDeviceTable.h>
#include "NetworkStartup.h"
#include <NVSettings.h>
#include <Config_Build.h>
#include <sys/socket.h>

#include "Source/Library/Web/AWSIoTClient/AWSIoTClient.h"
#include "Source/Library/Encryption/mbedtls/port/uez_mbedtls.h"
#include "Source/Library/Web/AWSIoTClient/aws_dev_mode_key_provisioning.h"
#include "Source/Library/Web/AWSIoTClient/aws_iot/iot-reference/examples/common/kvstore/kvstore.h"
#include "Source/Library/Web/AWSIoTClient/aws_iot/iot-reference/examples/common/mqtt_agent/mqtt_agent_task.h"
#include "Source/Library/Web/AWSIoTClient/mflash/mflash_file.h"

#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
#include <uEZTimeDate.h>
// USER END

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0     (GUI_ID_USER + 0x00)
#define ID_BUTTON_0     (GUI_ID_USER + 0x01)
#define ID_TEXT_0     (GUI_ID_USER + 0x02)
#define ID_GRAPH_0     (GUI_ID_USER + 0x03)
#define ID_TEXT_1     (GUI_ID_USER + 0x04)
#define ID_TEXT_2     (GUI_ID_USER + 0x05)
#define ID_TEXT_3     (GUI_ID_USER + 0x06)
#define ID_TEXT_4     (GUI_ID_USER + 0x07)
#define ID_TEXT_5     (GUI_ID_USER + 0x08)
#define ID_TEXT_6     (GUI_ID_USER + 0x09)
#define ID_TEXT_7     (GUI_ID_USER + 0x0A)
#define ID_TEXT_8     (GUI_ID_USER + 0x0B)
#define ID_TEXT_9     (GUI_ID_USER + 0x0C)
#define ID_TEXT_10     (GUI_ID_USER + 0x0D)
#define ID_TEXT_11     (GUI_ID_USER + 0x0E)
#define ID_TEXT_12     (GUI_ID_USER + 0x0F)
#define ID_TEXT_13     (GUI_ID_USER + 0x10)
#define ID_TEXT_14     (GUI_ID_USER + 0x11)
#define ID_TEXT_15     (GUI_ID_USER + 0x12)
#define ID_TEXT_16     (GUI_ID_USER + 0x13)
#define ID_TEXT_17     (GUI_ID_USER + 0x14)
#define ID_TEXT_18     (GUI_ID_USER + 0x15)
#define ID_TEXT_19     (GUI_ID_USER + 0x16)
#define ID_TEXT_20     (GUI_ID_USER + 0x17)
#define ID_TEXT_21     (GUI_ID_USER + 0x18)
#define ID_TEXT_22     (GUI_ID_USER + 0x19)
#define ID_IMAGE_0     (GUI_ID_USER + 0x1A)


// USER START (Optionally insert additional defines)
#define ID_TEXT_CONNECTED   ID_TEXT_2
#define ID_TEXT_TEMP   ID_TEXT_8
#define ID_TEXT_BMON   ID_TEXT_9
#define ID_TEXT_IPADDR   ID_TEXT_13
#define ID_TEXT_TC   ID_TEXT_14
#define ID_TEXT_ACC   ID_TEXT_15
#define ID_TEXT_VER   ID_TEXT_16
#define ID_TEXT_CLIENT   ID_TEXT_17
#define ID_TEXT_BUZZ   ID_TEXT_18
#define ID_TEXT_IPCURR   ID_TEXT_19
#define ID_TEXT_CLOCK   ID_TEXT_20
#define ID_TEXT_CLIENT_ID   ID_TEXT_21
#define ID_TEXT_CLIENT_REC    ID_TEXT_22

#define ID_UPDATE_TIMER         (GUI_ID_USER + 0x1B)
#define ID_UPDATE_TIMER_SLOW    (GUI_ID_USER + 0x1C)
#define ID_UPDATE_TIMER_FAST    (GUI_ID_USER + 0x1D)

#if(UEZ_DEFAULT_LCD == LCD_RES_WVGA)
#define SPACING                 (10)
#define DIVIDER                 (10)
#else
#define SPACING                 (5)
#define DIVIDER                 (5)
#endif

#define WINDOW_XSIZE            (UEZ_LCD_DISPLAY_WIDTH)
#define WINDOW_YSIZE            (UEZ_LCD_DISPLAY_HEIGHT)
#define WINDOW_XPOS             (0)
#define WINDOW_YPOS             (0)

#define TITLE_TEXT_XSIZE        (WINDOW_XSIZE)
#define TITLE_TEXT_YSIZE        ((WINDOW_YSIZE/10))
#define TITLE_TEXT_XPOS         (0)
#define TITLE_TEXT_YPOS         (0)

#define BACK_BUTTON_XSIZE       (WINDOW_XSIZE/DIVIDER)
#define BACK_BUTTON_YSIZE       (WINDOW_YSIZE/DIVIDER)
#define BACK_BUTTON_XPOS        (SPACING)
#define BACK_BUTTON_YPOS        (WINDOW_YSIZE - BACK_BUTTON_YSIZE - SPACING)

#define TEXT_XSIZE              ((WINDOW_XSIZE - (8 * SPACING))/7)
#define TEXT_YSIZE              ((WINDOW_YSIZE - TITLE_TEXT_YSIZE - (5 * SPACING))/4)
#define TEXT_XPOS(n)            (SPACING + (n * (SPACING + TEXT_XSIZE)))
#define TEXT_YPOS(n)            (((WINDOW_YSIZE - TITLE_TEXT_YSIZE)/2 - (TEXT_YSIZE/2)) + ( n * (TEXT_YSIZE + SPACING)))

#define UPDATE_TIME_MS          (250)
#define UPDATE_TIME_MS_SLOW     (10000)
#define UPDATE_TIME_MS_FAST     (50)

#define GRAPHA_MAX_NUM_DATA_OBJ  3
#define GRAPHB_MAX_NUM_DATA_OBJ  3

#define     GRAPH_XPOS              446//(TEMP_HUMIDITY_XPOS + (UEZ_LCD_DISPLAY_HEIGHT/8) + (2 * SPACING))
#define     GRAPH_YPOS              75//(UEZ_LCD_DISPLAY_HEIGHT/8)
#define     GRAPH_XSIZE             350//(UEZ_LCD_DISPLAY_WIDTH - GRAPH_XPOS - SPACING)
#define     GRAPH_YSIZE             400//(UEZ_LCD_DISPLAY_HEIGHT - TIME_DATE_YSIZE - SPACING)

#define     GRAPH_BORDERL           2//25 if using a scale
#define     GRAPH_BORDERT           2//10
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

#define     GRAPH_H_TICK_SIZE       5
#define     GRAPH_V_TICK_SIZE       10

#define     NUM_TEMP_SAPMPLES       GRAPH_XNUMPIXELS

#define     FONT                    GUI_FontOpenSans24
//#define     FONT_LARGE              GUI_FontOpenSans49
#define     BACKGROUND_COLOR        GUI_BLACK
#define     FONT_COLOR              GUI_WHITE

//#define     RTC_UPDATE_TIME_MS      1000
//#define     READING_UPDATE_TIME_MS  100
//#define     GRAPH_UPDATE_TIME_MS    100 //(1000 * 60)
#define     ID_LOG_DATA_TIME_MS     (1000 * 60)

#define     ALERT_X_POS             (3)    
#define     ALERT_Y_POS(n)         ((UEZ_LCD_DISPLAY_HEIGHT/4) + SPACING) + (n * ((UEZ_LCD_DISPLAY_HEIGHT/4) + SPACING))+30    //Added by IMM - Added to give the alert a better and dedicated location

#define     GRAPH_KEY_X_POS        560 
#define     GRAPH_KEY_Y_POS        10 

#define     TEMP_HIGH              32
#define     TEMP_LOW               10

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
// timer callbacks
static void IUpdateFields(WM_MESSAGE * pMsg);
static void IUpdateFields_Slow(WM_MESSAGE * pMsg);
static void IUpdateFields_Fast(WM_MESSAGE * pMsg);
// update GUI elements
static void IPrintIpConfiguration(WM_MESSAGE *pMsg);
static void IPrintIpConnecting(WM_MESSAGE *pMsg);
// button callbacks
//static TBool IHandleBack(WM_MESSAGE * pMsg, int aNCode, int aID);

static WM_HTIMER G_UpdateTimer = 0;
static WM_HTIMER G_UpdateTimer_Slow = 0;
static WM_HTIMER G_UpdateTimer_Fast = 0;
static TUInt8 G_CurrentImage = 0;

// read these text strings into the GUI to update labels
extern char G_CurrentShadowReceivedBuzzer;
extern char G_CurrentShadowReceivedClientT;
extern char G_CurrentShadowReceivedVersion;

extern char G_CurrentShadowReportTemp;
extern char G_CurrentShadowReportAcc;
extern char G_CurrentShadowReport5V;
extern char G_CurrentShadowReportTouch;
extern char G_CurrentShadowReportClientT;

extern T_uezTask G_tsMonitorTask;
extern TBool G_DHCP_Task_Has_IP;
static TBool G_GUI_Has_Displayed_IP = EFalse;

static GRAPH_Handle  G_GraphHandle[2];
static GRAPH_DATA_Handle G_GraphDataHandleA[GRAPHA_MAX_NUM_DATA_OBJ];

//static GRAPH_SCALE_Handle G_Graph_H_Scale;
//static GRAPH_SCALE_Handle G_Graph_V_Scale;

//static GRAPH_DATA_Handle G_GraphDataHandleB[GRAPHB_MAX_NUM_DATA_OBJ];

//static GRAPH_SCALE_Handle G_Graph_H_ScaleB;
//static GRAPH_SCALE_Handle G_Graph_V_ScaleB;

// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 800, 480, 0, 0x0, 0 },
  { BUTTON_CreateIndirect, "Back", ID_BUTTON_0, 5, 415, 80, 60, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "AWS MQTT Shadow Demo", ID_TEXT_0, 0, 5, 800, 24, 0, 0x0, 0 },
  { GRAPH_CreateIndirect, "Graph", ID_GRAPH_0, 446, 75, 350, 400, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Reported to AWS", ID_TEXT_1, 10, 91, 280, 32, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Connecting...", ID_TEXT_2, 5, 5, 200, 24, 0, 0x64, 0 },
  { TEXT_CreateIndirect, "Accelerometer:", ID_TEXT_3, 10, 135, 185, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Temperature:", ID_TEXT_4, 10, 169, 185, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Board 5V Monitor:", ID_TEXT_5, 10, 203, 185, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Touch Coordinates:", ID_TEXT_6, 10, 237, 185, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Received From AWS", ID_TEXT_7, 10, 328, 282, 32, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_8, 200, 169, 160, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_9, 200, 203, 200, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Client Token:", ID_TEXT_10, 62, 373, 133, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Version:", ID_TEXT_11, 102, 407, 93, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Buzzer:", ID_TEXT_12, 116, 441, 79, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "IP Address:", ID_TEXT_13, 5, 34, 110, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_14, 200, 237, 190, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_15, 200, 135, 200, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_16, 200, 407, 160, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_17, 200, 373, 160, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_18, 200, 441, 150, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "   .   .   .   ", ID_TEXT_19, 120, 34, 165, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "00/00/2024 00:00:00 PM GMT", ID_TEXT_20, 553, 5, 245, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "Client ID:", ID_TEXT_21, 10, 271, 185, 24, 0, 0x0, 0 },
  { TEXT_CreateIndirect, "", ID_TEXT_22, 200, 272, 160, 24, 0, 0x0, 0 },
  { IMAGE_CreateIndirect, "LogoQR", ID_IMAGE_0, 302, 340, 131, 130, 0, 0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/

// USER START (Optionally insert additional static code)

/** Active Flag, tell the dialog when it receives messages that the screen is in the foreground*/
static TBool G_Active = EFalse;
T_uezTimeDate G_CurrentTime;
static char G_widget_buf[36];    

static void IPrintIpConfiguration(WM_MESSAGE *pMsg)
{
    WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_IPADDR));
    WM_HideWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_IPCURR));

    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONNECTED), "Getting IP Address...");
    // TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_IPCURR), "");
    TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONNECTED), GUI_MAKE_COLOR(0x00FFFFFF));
}

static void IPrintIpConnecting(WM_MESSAGE *pMsg)
{
    TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONNECTED), "Connecting to AWS...");
    TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONNECTED), GUI_MAKE_COLOR(0x00FFFF00));
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
static void IUpdateFields(WM_MESSAGE *pMsg) {
  // T_MQTTSettings MQTTSettings;
  // MQTT_GetSettings(&MQTTSettings);

  if (UEZDeviceTableIsRegistered("RTC") == ETrue) {

    if (UEZTimeDateGet(&G_CurrentTime) == UEZ_ERROR_NONE) {
      if (G_CurrentTime.iTime.iHour > 12) {
        snprintf(G_widget_buf, 36, "%2u/%02u/%4u %u:%u:%02u PM GMT\n",
            G_CurrentTime.iDate.iMonth, G_CurrentTime.iDate.iDay, G_CurrentTime.iDate.iYear + 1900,
            G_CurrentTime.iTime.iHour - 12, G_CurrentTime.iTime.iMinute, G_CurrentTime.iTime.iSecond);
      } else {
        snprintf(G_widget_buf, 36, "%2u/%02u/%4u %u:%u:%02u AM GMT\n",
            G_CurrentTime.iDate.iMonth, G_CurrentTime.iDate.iDay, G_CurrentTime.iDate.iYear + 1900,
            G_CurrentTime.iTime.iHour, G_CurrentTime.iTime.iMinute, G_CurrentTime.iTime.iSecond);
      }
      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CLOCK), G_widget_buf);
    }
  }

  if (G_GUI_Has_Displayed_IP == ETrue) {
    if (xGetMQTTAgentState() == MQTT_AGENT_STATE_CONNECTED) {
      TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONNECTED), "Connected - TLS 1.2");
      TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CONNECTED), GUI_MAKE_COLOR(0x0000FF00));
    } else {
      IPrintIpConnecting(pMsg);
    }
    if (G_DHCP_Task_Has_IP == EFalse) {
      IPrintIpConfiguration(pMsg);
      G_GUI_Has_Displayed_IP = EFalse;
    }
  } else { // not displaed yet
    if (G_DHCP_Task_Has_IP == ETrue) {
      T_uezNetworkStatus status = {0};
      T_uezDevice activeDevice = NetworkGetActiveDevice(1); // query second device first
      if (activeDevice != 0) {
        UEZNetworkGetStatus(activeDevice, &status);
      } else { // check first device
        activeDevice = NetworkGetActiveDevice(0);
        if (activeDevice != 0) {
          UEZNetworkGetStatus(activeDevice, &status);
        }
      }

      if (status.iIPAddr.v4[0] != 0) {
        snprintf(G_widget_buf, 32, "%d.%d.%d.%d\n",
            status.iIPAddr.v4[0], status.iIPAddr.v4[1],
            status.iIPAddr.v4[2], status.iIPAddr.v4[3]);
        TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_IPCURR), G_widget_buf);
        // TEXT_SetTextColor(WM_GetDialogItem(pMsg->hWin, ID_TEXT_IPCURR), GUI_MAKE_COLOR(0x00FFFF00));

        WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_IPADDR));
        WM_ShowWindow(WM_GetDialogItem(pMsg->hWin, ID_TEXT_IPCURR));
        G_GUI_Has_Displayed_IP = ETrue;
        
        IPrintIpConnecting(pMsg);

      } else { // no IP address
        IPrintIpConfiguration(pMsg);
      }
    } else { // no ip address was set    
      IPrintIpConfiguration(pMsg);
    }
  }

  TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_ACC), &G_CurrentShadowReportAcc);
  TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_TEMP), &G_CurrentShadowReportTemp);
  TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_BMON), &G_CurrentShadowReport5V);
  TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_TC), &G_CurrentShadowReportTouch);
  TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CLIENT_REC), &G_CurrentShadowReportClientT);

  TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_CLIENT), &G_CurrentShadowReceivedClientT);
  TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_VER), &G_CurrentShadowReceivedVersion);
  TEXT_SetText(WM_GetDialogItem(pMsg->hWin, ID_TEXT_BUZZ), &G_CurrentShadowReceivedBuzzer);

#if 0
  GRAPH_DATA_YT_AddValue(hData, (_HeartBeat[Index]) + _DataAdjust);
  if (++Index == GUI_COUNTOF(_HeartBeat)) {
    Index = 0;
  }
#endif
}

static void IUpdateFields_Fast(WM_MESSAGE * pMsg)
{
    GUI_PID_STATE aTouchState;
    WindowManager_Get_Touch(&aTouchState);
    if(aTouchState.Pressed == 1) {
      GRAPH_DATA_YT_AddValue( G_GraphDataHandleA[0], aTouchState.x/2);
      GRAPH_DATA_YT_AddValue( G_GraphDataHandleA[1], aTouchState.y/2);
    }
}

/*---------------------------------------------------------------------------*
 * Routine: IUpdateFields_Slow
 *---------------------------------------------------------------------------*/
/** Update slow fields on the scren
 *      
 *  @param [in] pMsg	    WM_MESSAGE structure used by emWin to
 *							to handle screen information.
 */
 /*---------------------------------------------------------------------------*/
static void IUpdateFields_Slow(WM_MESSAGE * pMsg)
{
    if(G_CurrentImage == 0) {
      IMAGE_SetBitmap(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0), &bmiotQrCode);
      G_CurrentImage = 1;
    } else {
      IMAGE_SetBitmap(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0), &bmmqttLogo128);
      G_CurrentImage = 0;
    }
}

static void iInitGraph(WM_MESSAGE *pMsg)
{

#if 0 // TODO may update Graph demo later and create second graph.
  GRAPH_Handle          hGraph;
  unsigned              i;
  int                   Graph_xSize;
  int                   Graph_ySize;
  int                   Graph_xPos;
  int                   Graph_yPos;
  int                   Data_ySize;
  int                   xSize;
  int                   ySize;
#endif

    // Init first graph for touch coordinates only
    //G_GraphHandle = GRAPH_CreateEx(GRAPH_XPOS, GRAPH_YPOS, GRAPH_XSIZE, GRAPH_YSIZE, WM_HBKWIN, WM_CF_SHOW | WM_CF_CONST_OUTLINE, 0, 0);
    G_GraphHandle[0] = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
    GRAPH_SetBorder(G_GraphHandle[0], GRAPH_BORDERL, GRAPH_BORDERT, GRAPH_BORDERR, GRAPH_BORDERB);
    GRAPH_SetColor(G_GraphHandle[0], GRAPH_BK_COLOR,     GRAPH_CI_BK);
    GRAPH_SetColor(G_GraphHandle[0], GRAPH_BORDER_COLOR, GRAPH_CI_BORDER);
    GRAPH_SetColor(G_GraphHandle[0], GRAPH_FRAME_COLOR,  GRAPH_CI_FRAME);
    GRAPH_SetColor(G_GraphHandle[0], GRAPH_GRID_COLOR,   GRAPH_CI_GRID);

    GRAPH_SetGridDistX(G_GraphHandle[0], GRAPH_XNUMPIXELS);
    GRAPH_SetGridDistY(G_GraphHandle[0], GRAPH_YNUMPIXELS);
    GRAPH_SetGridFixedX(G_GraphHandle[0], 0);//0 X-axis is moving
    GRAPH_SetGridVis(G_GraphHandle[0], 0);//0 is set the grid to not be visible

    GRAPH_SetLineStyleH(G_GraphHandle[0], GUI_LS_DOT);
    GRAPH_SetLineStyleV(G_GraphHandle[0], GUI_LS_DOT);

    //G_Graph_H_Scale = GRAPH_SCALE_Create(GRAPH_BORDERT / 2, GUI_TA_RIGHT, GRAPH_SCALE_CF_HORIZONTAL, GRAPH_H_TICK_SIZE);
    //G_Graph_V_Scale = GRAPH_SCALE_Create(GRAPH_BORDERL / 2, GUI_TA_HCENTER, GRAPH_SCALE_CF_VERTICAL, GRAPH_V_TICK_SIZE);

    //GRAPH_SCALE_SetTextColor(G_Graph_V_Scale, GRAPH_GRID_COLOR);
    //GRAPH_AttachScale(G_GraphHandle[0], G_Graph_V_Scale);

    //GRAPH_SCALE_SetTextColor(G_Graph_H_Scale, GRAPH_GRID_COLOR);
    //GRAPH_AttachScale(G_GraphHandle[0], G_Graph_H_Scale);

    //This code handles the Vertical number scale
    //GRAPH_SCALE_SetTickDist(G_Graph_V_Scale, (GRAPH_YNUMPIXELS / 100) * 5);
    //GRAPH_SCALE_SetTickDist(G_Graph_H_Scale, (GRAPH_XNUMPIXELS / 50) * 5);

    //change scale back to 0 to 100
    
    //GRAPH_SCALE_SetFactor(G_Graph_V_Scale, .25);
    //GRAPH_SCALE_SetFactor(G_Graph_V_Scale, .25);  //Code Modified by IMM, Original code commented out above
    //GRAPH_SCALE_SetFactor(G_Graph_H_Scale, .01538);
    
    //  GRAPH_SetVSizeY(G_GraphHandle[0], 440);//Code added by BF - Creates a Scroll Bar and allows the graph's range to go up to 85 degrees Celcius
    //  GRAPH_SCALE_SetOff(G_Graph_V_Scale, 90);//Code added by IMM - Makes the minimum value on the graph -20
    
    
    G_GraphDataHandleA[0] = GRAPH_DATA_YT_Create(GRAPH_TEMP_COLOR, NUM_TEMP_SAPMPLES/2, 0, 0);    
    GRAPH_DATA_YT_SetOffY(G_GraphDataHandleA[0],0); // Sets a vertical offset used to draw the object data. 
    GRAPH_AttachData(G_GraphHandle[0], G_GraphDataHandleA[0]);
   

    G_GraphDataHandleA[1] = GRAPH_DATA_YT_Create(GRAPH_HUMIDITY_COLOR, NUM_TEMP_SAPMPLES/2, 0, 0);    
    GRAPH_DATA_YT_SetOffY(G_GraphDataHandleA[1],0);
    GRAPH_AttachData(G_GraphHandle[0], G_GraphDataHandleA[1]);

    GRAPH_DATA_YT_SetAlign(G_GraphDataHandleA[0], GRAPH_ALIGN_LEFT);
    GRAPH_DATA_YT_SetAlign(G_GraphDataHandleA[1], GRAPH_ALIGN_LEFT);
    // TODO second graph init
}

// USER END

/*********************************************************************
*
*       _cbDialog
*/
static void _cbDialog(WM_MESSAGE * pMsg) {
  WM_HWIN hItem;
  int     NCode;
  int     Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
    //
    // Initialization of 'Window'
    //
    hItem = pMsg->hWin;
    WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00000000));
    //
    // Initialization of 'Back'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
    BUTTON_SetFont(hItem, GUI_FONT_24_ASCII);
    //
    // Initialization of 'AWS MQTT Shadow Demo'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_0);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00521AD5));
    TEXT_SetFont(hItem, GUI_FONT_24B_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
    //
    // Initialization of 'Graph'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_GRAPH_0);
    GRAPH_SetBorder(hItem, 0, 0, 0, 0);
    //
    // Initialization of 'Reported to AWS'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_1);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    //
    // Initialization of 'Connecting...'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_2);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    //
    // Initialization of 'Accelerometer:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_3);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    //
    // Initialization of 'Temperature:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_4);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    //
    // Initialization of 'Board 5V Monitor:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_5);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    //
    // Initialization of 'Touch Coordinates:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_6);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of 'Received From AWS'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_7);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_32B_ASCII);
    //
    // Initialization of ''
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_8);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    //
    // Initialization of ''
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_9);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    //
    // Initialization of 'Client Token:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_10);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of 'Version:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_11);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of 'Buzzer:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_12);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of 'IP Address:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_13);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of ''
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_14);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    //
    // Initialization of ''
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_15);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    //
    // Initialization of ''
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_16);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    //
    // Initialization of ''
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_17);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    //
    // Initialization of ''
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_18);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    //
    // Initialization of '   .   .   .   '
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_19);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    //
    // Initialization of '00/00/2024 00:00:00 PM GMT'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_20);
    TEXT_SetFont(hItem, GUI_FONT_20_ASCII);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of 'Client ID:'
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_21);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    TEXT_SetTextAlign(hItem, GUI_TA_RIGHT | GUI_TA_VCENTER);
    //
    // Initialization of ''
    //
    hItem = WM_GetDialogItem(pMsg->hWin, ID_TEXT_22);
    TEXT_SetTextColor(hItem, GUI_MAKE_COLOR(0x00FFFFFF));
    TEXT_SetTextAlign(hItem, GUI_TA_LEFT | GUI_TA_VCENTER);
    TEXT_SetFont(hItem, GUI_FONT_24_ASCII);
    // USER START (Optionally insert additional code for further widget initialization)
    
    IMAGE_SetBitmap(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0), &bmiotQrCode); // start with QR code
    //IMAGE_SetBitmap(WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0), &bmmqttLogo128); // start with MQTT logo

    iInitGraph(pMsg);
    
    G_UpdateTimer = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER, 0, 0);
    G_UpdateTimer_Slow = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER_SLOW, 0, 0);
    G_UpdateTimer_Fast = WM_CreateTimer(pMsg->hWin, ID_UPDATE_TIMER_FAST, 0, 0);
    
    UEZTaskPrioritySet(G_tsMonitorTask, UEZ_PRIORITY_NORMAL);
    

    // USER END
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_BUTTON_0: // Notifications sent by 'Back'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        WindowManager_Show_Window(HOME_SCREEN);
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  
    case WM_PRE_PAINT:
        IUpdateFields(pMsg);
        break;
    case WM_POST_PAINT:
        break;

    case WM_TIMER:
        NCode = pMsg->Data.v;
        if (NCode == G_UpdateTimer) {
            WM_RestartTimer(G_UpdateTimer, UPDATE_TIME_MS);
            if(G_Active){
                IUpdateFields(pMsg);
            }
        }
        if (NCode == G_UpdateTimer_Slow) {
            WM_RestartTimer(G_UpdateTimer_Slow, UPDATE_TIME_MS_SLOW);
            if(G_Active){
                IUpdateFields_Slow(pMsg);
            }
        }
        if (NCode == G_UpdateTimer_Fast) {
            WM_RestartTimer(G_UpdateTimer_Fast, UPDATE_TIME_MS_FAST);
            if(G_Active){
                IUpdateFields_Fast(pMsg);
            }
        }
        break;

    case WM_APP_GAINED_FOCUS:
    	G_Active = ETrue;
    	break;
    case WM_APP_LOST_FOCUS:
    	G_Active = EFalse;
    	break;

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
*       CreateWindow
*/
WM_HWIN CreateWindow(void);
WM_HWIN CreateWindow(void) {
  WM_HWIN hWin;

  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)

/*---------------------------------------------------------------------------*
 * Routine:	MQTT_Create
 *---------------------------------------------------------------------------*/
/** Create the Window. This function should only be called by the
 *  window manager and never by user code.
 *      
 *  @param [in] p_choice    void
 *  @return					The emWin Handle to this window
 */
 /*---------------------------------------------------------------------------*/
WM_HWIN MQTT_Create(void)
{   /* Note that the last 3 parameters may be changed from the generated version which 
     * is why we create our own named function. */
    G_GUI_Has_Displayed_IP = EFalse;
    G_UpdateTimer = 0;
    G_UpdateTimer_Slow = 0;
    G_UpdateTimer_Fast = 0;
    G_CurrentImage = 0;
    return GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN,0,0);
}
// USER END

/*************************** End of file ****************************/
