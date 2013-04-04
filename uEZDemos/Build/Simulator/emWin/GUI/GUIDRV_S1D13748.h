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
File        : GUIDRV_S1D13748.h
Purpose     : Interface definition for GUIDRV_S1D13748 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_S1D13748_H
#define GUIDRV_S1D13748_H

#define GUIDRV_S1D13748_USE_MAIN 0
#define GUIDRV_S1D13748_USE_PIP1 1
#define GUIDRV_S1D13748_USE_PIP2 2

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  //
  // Driver specific configuration items
  //
  U32 BufferOffset;
  int UseLayer;
  int WriteBufferSize;
} CONFIG_S1D13748;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_S1D13748_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OY_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OX_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OXY_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OS_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OSY_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OSX_16_API;
extern const GUI_DEVICE_API GUIDRV_S1D13748_OSXY_16_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_S1D13748_16       &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OY_16    &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OX_16    &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OXY_16   &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OS_16    &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OSY_16   &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OSX_16   &GUIDRV_Win_API
  #define GUIDRV_S1D13748_OSXY_16  &GUIDRV_Win_API

#else

  #define GUIDRV_S1D13748_16       &GUIDRV_S1D13748_16_API
  #define GUIDRV_S1D13748_OY_16    &GUIDRV_S1D13748_OY_16_API
  #define GUIDRV_S1D13748_OX_16    &GUIDRV_S1D13748_OX_16_API
  #define GUIDRV_S1D13748_OXY_16   &GUIDRV_S1D13748_OXY_16_API
  #define GUIDRV_S1D13748_OS_16    &GUIDRV_S1D13748_OS_16_API
  #define GUIDRV_S1D13748_OSY_16   &GUIDRV_S1D13748_OSY_16_API
  #define GUIDRV_S1D13748_OSX_16   &GUIDRV_S1D13748_OSX_16_API
  #define GUIDRV_S1D13748_OSXY_16  &GUIDRV_S1D13748_OSXY_16_API

#endif

/*********************************************************************
*
*       Public routines
*/
void GUIDRV_S1D13748_Config  (GUI_DEVICE * pDevice, CONFIG_S1D13748 * pConfig);
void GUIDRV_S1D13748_SetBus16(GUI_DEVICE * pDevice, GUI_PORT_API * pHW_API);

#endif

/*************************** End of file ****************************/
