/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2012  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.16 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_TemplateI.h
Purpose     : Interface definition for GUIDRV_TemplateI driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_TEMPLATE_I_H
#define GUIDRV_TEMPLATE_I_H

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  //
  // Driver specific configuration items
  //
  int FirstSEG;
  int FirstCOM;
  int UseCache;
} CONFIG_TEMPLATE_I;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_TEMPLATE_I_16_API;
extern const GUI_DEVICE_API GUIDRV_TEMPLATE_I_OY_16_API;
extern const GUI_DEVICE_API GUIDRV_TEMPLATE_I_OX_16_API;
extern const GUI_DEVICE_API GUIDRV_TEMPLATE_I_OXY_16_API;
extern const GUI_DEVICE_API GUIDRV_TEMPLATE_I_OS_16_API;
extern const GUI_DEVICE_API GUIDRV_TEMPLATE_I_OSY_16_API;
extern const GUI_DEVICE_API GUIDRV_TEMPLATE_I_OSX_16_API;
extern const GUI_DEVICE_API GUIDRV_TEMPLATE_I_OSXY_16_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_TEMPLATE_I_16       &GUIDRV_Win_API
  #define GUIDRV_TEMPLATE_I_OY_16    &GUIDRV_Win_API
  #define GUIDRV_TEMPLATE_I_OX_16    &GUIDRV_Win_API
  #define GUIDRV_TEMPLATE_I_OXY_16   &GUIDRV_Win_API
  #define GUIDRV_TEMPLATE_I_OS_16    &GUIDRV_Win_API
  #define GUIDRV_TEMPLATE_I_OSY_16   &GUIDRV_Win_API
  #define GUIDRV_TEMPLATE_I_OSX_16   &GUIDRV_Win_API
  #define GUIDRV_TEMPLATE_I_OSXY_16  &GUIDRV_Win_API

#else

  #define GUIDRV_TEMPLATE_I_16       &GUIDRV_TEMPLATE_I_16_API
  #define GUIDRV_TEMPLATE_I_OY_16    &GUIDRV_TEMPLATE_I_OY_16_API
  #define GUIDRV_TEMPLATE_I_OX_16    &GUIDRV_TEMPLATE_I_OX_16_API
  #define GUIDRV_TEMPLATE_I_OXY_16   &GUIDRV_TEMPLATE_I_OXY_16_API
  #define GUIDRV_TEMPLATE_I_OS_16    &GUIDRV_TEMPLATE_I_OS_16_API
  #define GUIDRV_TEMPLATE_I_OSY_16   &GUIDRV_TEMPLATE_I_OSY_16_API
  #define GUIDRV_TEMPLATE_I_OSX_16   &GUIDRV_TEMPLATE_I_OSX_16_API
  #define GUIDRV_TEMPLATE_I_OSXY_16  &GUIDRV_TEMPLATE_I_OSXY_16_API

#endif

/*********************************************************************
*
*       Public routines
*/
void GUIDRV_TemplateI_Config    (GUI_DEVICE * pDevice, CONFIG_TEMPLATE_I * pConfig);
void GUIDRV_TemplateI_SetBus_XXX(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);

#endif

/*************************** End of file ****************************/
