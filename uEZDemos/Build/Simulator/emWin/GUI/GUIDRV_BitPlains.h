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
File        : GUIDRV_Bitplains.h
Purpose     : Interface definition for GUIDRV_Bitplains driver
---------------------------END-OF-HEADER------------------------------
*/

#ifndef GUIDRV_BITPLAINS_H
#define GUIDRV_BITPLAINS_H

/*********************************************************************
*
*       Configuration structure
*/
typedef struct {
  int Mirror;
} CONFIG_BITPLAINS;

typedef struct {
  U8 * apVRAM[8];
} CONFIG_VRAM_BITPLAINS;

/*********************************************************************
*
*       Display drivers
*/
//
// Addresses
//
extern const GUI_DEVICE_API GUIDRV_Win_API;

extern const GUI_DEVICE_API GUIDRV_BitPlains_API;

//
// Macros to be used in configuration files
//
#if defined(WIN32) && !defined(LCD_SIMCONTROLLER)

  #define GUIDRV_BITPLAINS        &GUIDRV_Win_API

#else

  #define GUIDRV_BITPLAINS        &GUIDRV_BitPlains_API

#endif

/*********************************************************************
*
*       Public routines
*/
void GUIDRV_BitPlains_Config(GUI_DEVICE * pDevice, CONFIG_BITPLAINS * pConfig);

#endif /* GUIDRV_BITPLAINS_H */

/*************************** End of file ****************************/
