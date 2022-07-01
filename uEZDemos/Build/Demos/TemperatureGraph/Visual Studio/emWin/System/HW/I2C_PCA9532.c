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
File    : I2C_PCA9532.c
Purpose : Generic I2C_PCA9532 interface module
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "I2C.h"
#include "I2C_PCA9532.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/

//
// Register addresses
//
#define INPUT0  0
#define INPUT1  1
#define PSC0    2
#define PWM0    3
#define PSC1    4
#define PWM1    5
#define LS0     6
#define LS1     7
#define LS2     8
#define LS3     9

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static U8 _IsInited;

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _SetPinSelect
*
* Function description
*   Sets pin function select for one pin.
*
* Return value
*      0: O.K.
*   != 0: Error
*/
static U8 _SetPinSelect(U32 I2CBaseAddr, U8 Addr, U8 Pin, U8 State) {
  U8 Data[2];
  U8 r;

  if        (Pin < 4) {
    Data[0]  = LS0;
  } else if (Pin < 8) {
    Data[0]  = LS1;
    Pin     -= 4;
  } else if (Pin < 12) {
    Data[0]  = LS2;
    Pin     -= 8;
  } else if (Pin < 16) {
    Data[0]  = LS3;
    Pin     -= 12;
  } else {
    return 1;  // Error, invalid pin
  }
  Pin *= 2;
  r    = I2C_WriteRead(I2CBaseAddr, Addr, &Data[0], 1, &Data[1], 1);
  if (r == 0) {
    Data[1] &= ~(0x3   << Pin);
    Data[1] |=  (State << Pin);
    r        = I2C_Write(I2CBaseAddr, Addr, Data, 2);
  }
  return r;
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       I2C_PCA9532_GetPinState
*
* Function description
*   Retrieves the state for all 16 pins.
*
* Return value
*      0: O.K.
*   != 0: Error
*/
U8 I2C_PCA9532_GetPinState(U32 I2CBaseAddr, U8 Addr, U16 * pState) {
  U8  Register;
  U8  r;

  Register = INPUT0;
  r        = I2C_WriteRead(I2CBaseAddr, Addr, &Register, 1, (U8*)pState, 1);
  if (r == 0) {
    Register++;  // Set to INPUT1
    r = I2C_WriteRead(I2CBaseAddr, Addr, &Register, 1, (U8*)pState + 1, 1);
  }
  return r;
}

/*********************************************************************
*
*       I2C_PCA9532_SetPinSelect
*
* Function description
*   Sets pin function select for one pin.
*
* Return value
*      0: O.K.
*   != 0: Error
*/
U8 I2C_PCA9532_SetPinSelect(U32 I2CBaseAddr, U8 Addr, U8 Pin, U8 State) {
  U8 r;

  r = _SetPinSelect(I2CBaseAddr, Addr, Pin, State);
  return r;
}

/*********************************************************************
*
*       I2C_PCA9532_SetPwm
*
* Function description
*   Sets the timing for PWM0 or PWM1 and assigns the pin to the
*   according PWM.
*
* Return value
*      0: O.K.
*   != 0: Error
*/
U8 I2C_PCA9532_SetPwm(U32 I2CBaseAddr, U8 Addr, U8 Pin, U8 PwmNumber, U8 Pwm, U8 Psc) {
  U8 Data[2];
  U8 r;

  if (PwmNumber > 1) {
    return 1;  // Error, invalid PWM number
  }

  if (PwmNumber) {
    Data[0] = PSC1;
  } else {
    Data[0] = PSC0;
  }
  Data[1] = Psc;
  r       = I2C_Write(I2CBaseAddr, Addr, Data, 2);
  if (r == 0) {
    if (PwmNumber) {
      Data[0] = PWM1;
    } else {
      Data[0] = PWM0;
    }
    Data[1] = Pwm;
    r       = I2C_Write(I2CBaseAddr, Addr, Data, 2);
    if (r == 0) {
      r = _SetPinSelect(I2CBaseAddr, Addr, Pin, I2C_PCA9532_PWM0 + PwmNumber);
    }
  }
  return r;
}

/*********************************************************************
*
*       I2C_PCA9532_Init()
*/
void I2C_PCA9532_Init(U32 I2CBaseAddr, U32 PeripheralClock, U32 ClockRate) {
  if (_IsInited == 0) {
    I2C_Init(I2CBaseAddr, PeripheralClock, ClockRate);
    _IsInited = 1;
  }
}

/*************************** End of file ****************************/
