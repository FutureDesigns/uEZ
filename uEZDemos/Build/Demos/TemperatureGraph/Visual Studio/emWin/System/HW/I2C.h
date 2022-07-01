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
File    : I2C.h
Purpose : Generic I2C module header
--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef I2C_H      // Avoid multiple inclusion
#define I2C_H

#include "GUI.h"   // Definition of U32, U16 and U8

/*********************************************************************
*
*       Functions
*
**********************************************************************
*/
void I2C_Init     (U32 I2CBaseAddr, U32 PeripheralClock, U32 ClockRate);
U8   I2C_Read     (U32 I2CBaseAddr, U8 Addr, U8 * pData, U16 NumBytesToRead);
U8   I2C_Write    (U32 I2CBaseAddr, U8 Addr, U8 * pData, U16 NumBytesToWrite);
U8   I2C_WriteRead(U32 I2CBaseAddr, U8 Addr, U8 * pData, U16 NumBytesToWrite, U8 * pBuf, U16 NumBytesToRead);

#endif                                  // Avoid multiple inclusion

/*************************** End of file ****************************/
