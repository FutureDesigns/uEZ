/*********************************************************************
*                SEGGER Microcontroller GmbH                         *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The software has been licensed to  NXP Semiconductors USA, Inc.  whose
registered  office  is  situated  at 411 E. Plumeria Drive, San  Jose,
CA 95134, USA  solely for  the  purposes  of  creating  libraries  for
NXPs M0, M3/M4 and  ARM7/9 processor-based  devices,  sublicensed  and
distributed under the terms and conditions of the NXP End User License
Agreement.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
Licensing information
Licensor:                 SEGGER Microcontroller Systems LLC
Licensed to:              NXP Semiconductors, 1109 McKay Dr, M/S 76, San Jose, CA 95131, USA
Licensed SEGGER software: emWin
License number:           GUI-00186
License model:            emWin License Agreement, dated August 20th 2011 and Amendment, dated October 19th 2017
Licensed platform:        NXP's ARM 7/9, Cortex-M0, M3, M4, M7, A7
----------------------------------------------------------------------
Support and Update Agreement (SUA)
SUA period:               2011-08-19 - 2018-09-02
Contact to extend SUA:    sales@segger.com
----------------------------------------------------------------------
File        : LCD_SIM.h
Purpose     : Declares LCD interface functions
----------------------------------------------------------------------
*/

#ifndef LCDSIM_H
#define LCDSIM_H

#include "LCD.h"

/************************************************************
*
*       Defines
*
*************************************************************
*/
#define LCDSIM_MAX_DISPLAYS GUI_NUM_LAYERS

#define GUI_TRANSMODE_PIXELALPHA 0
#define GUI_TRANSMODE_CHROMA     1
#define GUI_TRANSMODE_ZERO       2

/************************************************************
*
*       LCDSIM_      General declarations
*
*************************************************************
*/
void  LCDSIM_PreInit(void);
char* LCDSIM_Init(void);
void  LCDSIM_Exit(void);
int32_t   LCDSIM_GetMouseState(LCD_tMouseState *pState);
void  LCDSIM_SetMouseState(int32_t x, int32_t y, int32_t KeyStat, int32_t LayerIndex);
void  LCDSIM_CheckMouseState(int32_t LayerIndex);
int32_t   LCDSIM_SaveSBMP  (const char * sFileName);
int32_t   LCDSIM_SaveSBMPEx(const char * sFileName, int32_t x0, int32_t y0, int32_t xSize, int32_t ySize);
void  LCDSIM_SetRGBOrder(unsigned RGBOrder);

/************************************************************
*
*       LCDSIM_     Functions for each display
*
*************************************************************
*/
void LCDSIM_FillRect(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t Index, int32_t LayerIndex);
int32_t  LCDSIM_GetModifyCnt(int32_t LayerIndex);
int32_t  LCDSIM_GetModifyCntInfo(int32_t LayerIndex);
int32_t  LCDSIM_GetPixelColor(int32_t x, int32_t y, int32_t LayerIndex);
int32_t  LCDSIM_GetPixelIndex(int32_t x, int32_t y, int32_t LayerIndex);
int32_t  LCDSIM_Index2Color(int32_t Index, int32_t LayerIndex);
int32_t  LCDSIM_RLUT_GetPixelIndex(int32_t x, int32_t y, int32_t LayerIndex);
void LCDSIM_RLUT_SetPixelIndex(int32_t x, int32_t y, int32_t Index, int32_t LayerIndex);
void LCDSIM_SetLUTEntry(U8 Pos, LCD_COLOR color, int32_t LayerIndex);
void LCDSIM_SetPixelIndex(int32_t x, int32_t y, int32_t Index, int32_t LayerIndex);
void LCDSIM_SetPixelColor(int32_t x, int32_t y, LCD_COLOR PixelColor, int32_t LayerIndex);
void LCDSIM_SetSubPixel(int32_t x, int32_t y, U8 Value, int32_t LayerIndex);
void LCDSIM_SetPixelPhys(int32_t x, int32_t y, int32_t Index, int32_t LayerIndex);
int32_t  LCDSIM_GetPixelPhys(int32_t xPhys, int32_t yPhys, int32_t LayerIndex);
void LCDSIM_FillRectPhys(int32_t x0Phys, int32_t y0Phys, int32_t x1Phys, int32_t y1Phys, int32_t Index, int32_t LayerIndex);
void LCDSIM_SetOrg(int32_t x, int32_t y, int32_t LayerIndex);
void LCDSIM_SetAlpha(int32_t Alpha, int32_t LayerIndex);
int32_t  LCDSIM_GetAlpha(int32_t LayerIndex);
void LCDSIM_SetLayerPos(int32_t xPos, int32_t yPos, int32_t LayerIndex);
void LCDSIM_SetLayerVis(int32_t OnOff, int32_t LayerIndex);
void LCDSIM_SetSize(int32_t LayerIndex, int32_t xSize, int32_t ySize);
void LCDSIM_SetTransMode(int32_t LayerIndex, int32_t TransMode);
void LCDSIM_SetChroma(int32_t LayerIndex, LCD_COLOR ChromaMin, LCD_COLOR ChromaMax);
void LCDSIM_SetCompositeColor(U32 Color);
void LCDSIM_SetCompositeSize(int32_t xSize, int32_t ySize);
void LCDSIM_CopyBuffer(int32_t LayerIndex, int32_t IndexSrc, int32_t IndexDst);
void LCDSIM_Invalidate(int32_t LayerIndex);

/********************************************************************
*
*       Publics for LCD
*
*********************************************************************
*/
void SIM_GUI_SetCompositeSize(int32_t xSize, int32_t ySize);
void SIM_GUI_SetCompositeColor(U32 Color);
U32  SIM_GUI_GetCompositeColor(void);
void SIM_GUI_SetLCDPos(int32_t xPos, int32_t yPos);
int32_t  SIM_GUI_SaveBMP(const char * sFileName);
int32_t  SIM_GUI_SaveBMPEx(const char * sFileName, int32_t x0, int32_t y0, int32_t xSize, int32_t ySize);
int32_t  SIM_GUI_SaveCompositeBMP(const char * sFileName);
int32_t  SIM_GUI_SetTransColor(int32_t Color);
int32_t  SIM_GUI_SetLCDColorBlack (uint32_t Index, int32_t Color);
int32_t  SIM_GUI_SetLCDColorWhite (uint32_t Index, int32_t Color);
void SIM_GUI_SetMag(int32_t MagX, int32_t MagY);
int32_t  SIM_GUI_GetMagX(void);
int32_t  SIM_GUI_GetMagY(void);
int32_t  SIM_GUI_GetForwardRButton(void);
void SIM_GUI_SetForwardRButton(int32_t OnOff);
void SIM_GUI_SetTransMode(int32_t LayerIndex, int32_t TransMode);
void SIM_GUI_SetChroma(int32_t LayerIndex, unsigned long ChromaMin, unsigned long ChromaMax);
void SIM_GUI_UseCustomBitmaps(void);
void SIM_GUI_SetAccellerator(int32_t Accellerator);
void SIM_GUI_SetMainScreenOffset(int32_t x, int32_t y);
void SIM_GUI_SetCompositeTouch(int32_t LayerIndex);
int32_t  SIM_GUI_GetCompositeTouch(void);

/********************************************************************
*
*       Routine(s) in user application
*
*********************************************************************
*/
void SIM_X_Config(void);   /* Allow init before application starts ... Use it to set LCD offset etc */

/********************************************************************
*
*       Publics used by GUI_X module
*
*********************************************************************
*/
void SIM_GUI_Delay (int32_t ms);
void SIM_GUI_ExecIdle(void);
int32_t  SIM_GUI_GetTime(void);
int32_t  SIM_GUI_GetKey(void);
int32_t  SIM_GUI_WaitKey(void);
void SIM_GUI_StoreKey(int32_t);

/********************************************************************
*
*       Publics for logging, warning, errorout
*
*********************************************************************
*/
void SIM_GUI_Log(const char *s);
void SIM_GUI_Log1(const char *s, int32_t p0);
void SIM_GUI_Log2(const char *s, int32_t p0, int32_t p1);
void SIM_GUI_Log3(const char *s, int32_t p0, int32_t p1, int32_t p2);
void SIM_GUI_Log4(const char *s, int32_t p0, int32_t p1, int32_t p2,int32_t p3);
void SIM_GUI_Warn(const char *s);
void SIM_GUI_Warn1(const char *s, int32_t p0);
void SIM_GUI_Warn2(const char *s, int32_t p0, int32_t p1);
void SIM_GUI_Warn3(const char *s, int32_t p0, int32_t p1, int32_t p2);
void SIM_GUI_Warn4(const char *s, int32_t p0, int32_t p1, int32_t p2, int32_t p3);
void SIM_GUI_ErrorOut(const char *s);
void SIM_GUI_ErrorOut1(const char *s, int32_t p0);
void SIM_GUI_ErrorOut2(const char *s, int32_t p0, int32_t p1);
void SIM_GUI_ErrorOut3(const char *s, int32_t p0, int32_t p1, int32_t p2);
void SIM_GUI_ErrorOut4(const char *s, int32_t p0, int32_t p1, int32_t p2, int32_t p3);
void SIM_GUI_EnableMessageBoxOnError(int32_t Status);

/********************************************************************
*
*       Commandline support
*
*********************************************************************
*/
const char *SIM_GUI_GetCmdLine(void);

/********************************************************************
*
*       Multitasking support
*
*********************************************************************
*/
void SIM_GUI_CreateTask(char * pName, void * pFunc);
void SIM_GUI_Start(void);
unsigned long SIM_GUI_GetTaskID(void);
void SIM_GUI_Lock(void);
void SIM_GUI_Unlock(void);
void SIM_GUI_InitOS(void);

#endif /* LCD_H */




