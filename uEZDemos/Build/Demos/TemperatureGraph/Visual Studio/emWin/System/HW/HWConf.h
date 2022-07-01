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
File    : HWConf.h
Purpose : Function prototypes for hardware configuration and
          initialization.
---------------------------END-OF-HEADER------------------------------
*/

#ifndef HWCONF_H            // Avoid multiple inclusion
  #define HWCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "GUI.h"            // Definition of U32, U16 and U8

/*********************************************************************
*
*       Prototypes
*
**********************************************************************
*/
void HW_X_Config(void);
void ExecTouch(void);    // Defined in LCDConf.c

#ifdef __cplusplus
}
#endif

#endif                      // Avoid multiple inclusion

/*************************** End of file ****************************/
