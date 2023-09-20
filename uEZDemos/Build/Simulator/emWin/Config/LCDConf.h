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
File        : LCDConf.h
Purpose     : Display driver configuration file
----------------------------------------------------------------------
*/

#ifndef LCDCONF_H
#define LCDCONF_H

// Define this term if running on LPC4088 OEM module. Comment out if on a LPC1788 OEM module
//#define LPC4088

#ifdef LPC4088
#define LPC_SSP LPC_SSP2
#else
#define LPC_SSP LPC_SSP0
#endif

#endif /* LCDCONF_H */

/*************************** End of file ****************************/
