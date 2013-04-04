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

** emWin V5.14 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : GUIDRV_GW7630_1.h
Purpose     : Interface definition for GUIDRV_GW7630_1 driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_GW7630_1_H
#define GUIDRV_GW7630_1_H

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_Win_API;

extern const GUI_DEVICE_API GUIDRV_GW7630_1_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_GW7630_1            &GUIDRV_Win_API

#else

  #define GUIDRV_GW7630_1            &GUIDRV_GW7630_1_API

#endif

#endif

void GUIDRV_GW7630_SetBus8(GUI_DEVICE * pDevice, GUI_PORT_API * pPortAPI);

/*************************** End of file ****************************/
