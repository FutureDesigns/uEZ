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
File    : I2C_PCA9532.h
Purpose : Generic I2C_PCA9532 module header
--------  END-OF-HEADER  ---------------------------------------------
*/

#ifndef I2C_PCA9532_H                       /* avoid multiple inclusion */
#define I2C_PCA9532_H

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define I2C_PCA9532_PWM0   2

/*********************************************************************
*
*       Functions
*
**********************************************************************
*/
void I2C_PCA9532_Init        (U32 I2CBaseAddr, U32 PeripheralClock, U32 ClockRate);
U8   I2C_PCA9532_GetPinState (U32 I2CBaseAddr, U8 Addr, U16 * pState);
U8   I2C_PCA9532_SetPinSelect(U32 I2CBaseAddr, U8 Addr, U8 Pin, U8 State);
U8   I2C_PCA9532_SetPwm      (U32 I2CBaseAddr, U8 Addr, U8 Pin, U8 PwmNumber, U8 Pwm, U8 Psc);

#endif                                  /* avoid multiple inclusion */

/*************************** End of file ****************************/
