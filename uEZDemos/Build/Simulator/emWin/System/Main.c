/*********************************************************************
*                SEGGER MICROCONTROLLER GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 2003-2012     SEGGER Microcontroller GmbH & Co KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

----------------------------------------------------------------------
File    : Main.c
Purpose : Calls hardware initialization and application.
--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef _WINDOWS
#include "HWConf.h"
#endif

#ifdef __CROSSWORKS_ARM
extern void __low_level_init(); // hwconf.c
#endif

void MainTask(void);  // Defined in SEGGERDEMO.c

/*********************************************************************
*
*       main
*/
void main(void) {
  #ifdef __CROSSWORKS_ARM
  __low_level_init();
  #endif
  #ifndef _WINDOWS
  HW_X_Config();      // Initialization of Hardware
  #endif
  MainTask();         // emWin application
}
