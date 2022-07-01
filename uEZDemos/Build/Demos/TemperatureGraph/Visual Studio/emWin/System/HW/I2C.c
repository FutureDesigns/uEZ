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
File    : I2C.c
Purpose : Generic I2C interface module
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "I2C.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
//
// I2C return codes
//
#define I2C_CODE_OK       (0)
#define I2C_CODE_DATA     (1)
#define I2C_CODE_RTR      (2)
#define I2C_CODE_ERROR    (3)
#define I2C_CODE_FULL     (4)
#define I2C_CODE_EMPTY    (5)
#define I2C_CODE_BUSY     (6)

//
// I2C modes
//
#define I2C_MODE_ACK0     (0)
#define I2C_MODE_ACK1     (1)
#define I2C_MODE_READ     (2)

/*********************************************************************
*
*       Defines, sfrs
*
**********************************************************************
*/
#define I2C_CONSET         (*(volatile unsigned long *)(_I2CBaseAddr + 0x0000))
#define I2C_STAT           (*(volatile unsigned long *)(_I2CBaseAddr + 0x0004))
#define I2C_DAT            (*(volatile unsigned long *)(_I2CBaseAddr + 0x0008))
#define I2C_ADR            (*(volatile unsigned long *)(_I2CBaseAddr + 0x000C))
#define I2C_SCLH           (*(volatile unsigned long *)(_I2CBaseAddr + 0x0010))
#define I2C_SCLL           (*(volatile unsigned long *)(_I2CBaseAddr + 0x0014))
#define I2C_CONCLR         (*(volatile unsigned long *)(_I2CBaseAddr + 0x0018))

/*********************************************************************
*
*       Macros
*
**********************************************************************
*/
#define SET_BASE_ADDR(Addr)  (_I2CBaseAddr = Addr)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
static U8  _IsInited;
static U32 _I2CBaseAddr;

/*********************************************************************
*
*       Static functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _I2C_CheckStatus()
*
* Function description
*   Checks the I2C status
*
* Return values
*   0x00 Bus error
*   0x08 START condition transmitted
*   0x10 Repeated START condition transmitted
*   0x18 SLA + W transmitted, ACK received
*   0x20 SLA + W transmitted, ACK not received
*   0x28 Data byte transmitted, ACK received
*   0x30 Data byte transmitted, ACK not received
*   0x38 Arbitration lost
*   0x40 SLA + R transmitted, ACK received
*   0x48 SLA + R transmitted, ACK not received
*   0x50 Data byte received in master mode, ACK transmitted
*   0x58 Data byte received in master mode, ACK not transmitted
*   0x60 SLA + W received, ACK transmitted
*   0x68 Arbitration lost, SLA + W received, ACK transmitted
*   0x70 General call address received, ACK transmitted
*   0x78 Arbitration lost, general call addr received, ACK transmitted
*   0x80 Data byte received with own SLA, ACK transmitted
*   0x88 Data byte received with own SLA, ACK not transmitted
*   0x90 Data byte received after general call, ACK transmitted
*   0x98 Data byte received after general call, ACK not transmitted
*   0xA0 STOP or repeated START condition received in slave mode
*   0xA8 SLA + R received, ACK transmitted
*   0xB0 Arbitration lost, SLA + R received, ACK transmitted
*   0xB8 Data byte transmitted in slave mode, ACK received
*   0xC0 Data byte transmitted in slave mode, ACK not received
*   0xC8 Last byte transmitted in slave mode, ACK received
*   0xF8 No relevant status information, SI=0
*   0xFF Channel error
*
* Note
*   SI flag is not cleared!
*/
static unsigned char _I2C_CheckStatus(void) {
  unsigned char r = 0;

  //
  // Wait for I2C status to change
  //
  while ((I2C_CONSET & (1 << 3)) == 0) {  // SI == 0
  }
  //
  // Read I2C state
  //
  r = I2C_STAT;

  return r;
}

/*********************************************************************
*
*       _I2C_Start()
*/
static char _I2C_Start(void) {
  unsigned char Status = 0;
  unsigned char r      = 0;

  //
  // Issue start condition
  //
  I2C_CONSET |= (1 << 5);  // STA = 1, set start flag
  //
  // Wait until START transmitted
  //
  while(1) {
    Status = _I2C_CheckStatus();
    //
    // Check if start transmitted
    //
    if ((Status == 0x08) || (Status == 0x10)) {
      r = I2C_CODE_OK;
      break;
    } else if (Status != 0xF8) {  // Error
      r = Status;
      break;
    } else {
      I2C_CONCLR = (1 << 3);      // Clear SI flag
    }
  }
  I2C_CONCLR = (1 << 5);          // Clear start flag

  return r;
}

/*********************************************************************
*
*       _I2C_PutChar()
*
* Function description
*   Sends a character on the I2C network
*
* Return value
*   I2C_CODE_OK   - successful
*   I2C_CODE_BUSY - data register is not ready -> byte was not sent
*/
static unsigned  char _I2C_PutChar(unsigned char Data) {
  unsigned char r;

  r = 0;
  //
  // Check if I2C Data register can be accessed
  //
  if ((I2C_CONSET & (1 << 3)) != 0) {  // SI = 1
    //
    // Send data
    //
    I2C_DAT    = Data;
    I2C_CONCLR = (1 << 3);       // Clear SI flag
    r          = I2C_CODE_OK;
  } else {
    r          = I2C_CODE_BUSY;  // Data register not ready
  }
  return r;
}

/*********************************************************************
*
*       _I2C_WaitTransmit()
*/
static unsigned char _I2C_WaitTransmit(void) {
  unsigned char Status = 0;

  //
  // Wait till data transmitted
  //
  while(1) {
    Status = _I2C_CheckStatus();
    //
    // SLA+W transmitted, ACK received or
    // data byte transmitted, ACK received
    //
    if ((Status == 0x18) || (Status == 0x28)) {
      return I2C_CODE_OK;  // Data transmitted and ACK received
    } else if (Status != 0xF8) {
      return I2C_CODE_ERROR;  // Error, no relevant status information
    }
  }
}

/*********************************************************************
*
*       _I2C_WriteWithWait()
*/
static unsigned char _I2C_WriteWithWait(unsigned char Data) {
  unsigned char r = 0;

  r = _I2C_PutChar(Data);
  while (r == I2C_CODE_BUSY) {
    r = _I2C_PutChar(Data);
  }
  if (r == I2C_CODE_OK) {
    r = _I2C_WaitTransmit();
  }
  return r;
}

/*********************************************************************
*
*       _I2C_Stop()
*
* Function description
*   Generates a stop condition in master mode or recovers from an
*   error condition in slave mode.
*
* Note
*   After this function is run, you may need a bus free time before
*   you can generate a new start condition.
*/
static void _I2C_Stop(void) {
  I2C_CONSET |= (1 << 4);  // STO = 1, set stop flag
  I2C_CONCLR  = (1 << 3);  // Clear SI flag
  while (I2C_CONSET & (1 << 4)) { } // Wait till STOP detected (while STO = 1)
}

/*********************************************************************
*
*       _I2C_RepeatStart()
*
* Function description
*   Generates a start condition on I2C when bus is free.
*   Master mode will also automatically be entered.
*
* Note
*   After a stop condition, you may need a bus free time before you
*   can generate a new start condition.
*/
static unsigned char _I2C_RepeatStart(void) {
  unsigned char Status = 0;
  unsigned char r      = 0;

  //
  // Issue a start condition
  //
  I2C_CONSET |= (1 << 5);  // STA = 1, set start flag
  I2C_CONCLR  = (1 << 3);  // Clear SI flag
  //
  // Wait until START transmitted
  //
  while (1) {
    Status = _I2C_CheckStatus();
    if ((Status == 0x08) || (Status == 0x10)) {
      r = I2C_CODE_OK;  // Start transmitted
      break;
    } else if (Status != 0xF8) {
      r = Status;  // Error
      break;
    } else {
      I2C_CONCLR = (1 << 3);  // Clear SI flag
    }
  }
  I2C_CONCLR = (1 << 5);  // Clear start flag
  return r;
}

/*********************************************************************
*
*       _I2C_GetChar()
*
* Function description
*   Read a character. I2C master mode is used.
*   This function is also used to prepare if the master shall generate
*   acknowledge or not acknowledge.
*
* Parameter
*   Mode: I2C_MODE_ACK0 Set ACK=0. Slave sends next byte
*         I2C_MODE_ACK1 Set ACK=1. Slave sends last byte
*         I2C_MODE_READ Read data from data register
*/
static unsigned char _I2C_GetChar(unsigned char Mode, unsigned char * pBuf) {
  unsigned char r = I2C_CODE_OK;

  if (Mode == I2C_MODE_ACK0) {
    //
    // Operation mode is changed from master transmit to master receive
    // Set ACK = 0 (informs slave to send next byte)
    //
    I2C_CONSET |= (1 << 2);  // AA = 1
    I2C_CONCLR  = (1 << 3);  // Clear SI flag
  } else if (Mode == I2C_MODE_ACK1) {
    I2C_CONCLR = (1 << 2);   // Set ACK = 1 (informs slave to send last byte)
    I2C_CONCLR = (1 << 3);   // Clear SI flag
  } else if (Mode == I2C_MODE_READ) {
    //
    // Check if I2C Data register can be accessed
    //
    if (I2C_CONSET & (1 << 3)) {       // SI = 1
      *pBuf = (unsigned char)I2C_DAT;  // Read data
    } else {
      r = I2C_CODE_EMPTY;  // No data available
    }
  }
  return r;
}

/*********************************************************************
*
*       _Read
*
* Function description
*   Reads from a device on I2C.
*/
static U8 _Read(U8 Addr, U8 * pData, U16 NumBytesToRead, U8 PartOfWriteRead) {
  U16 i;
  U8  Status;
  U8  r;

  //
  // Generate start condition
  //
  if (PartOfWriteRead) {
    r = _I2C_RepeatStart();  // Generate Start condition
  } else {
    r = _I2C_Start();  // Generate Start condition
  }
  //
  // Transmit device address
  //
  if (r == I2C_CODE_OK) {
    r = _I2C_PutChar(Addr + 0x01);  // Write address (SLA+R)
    while (r == I2C_CODE_BUSY) {
      r = _I2C_PutChar(Addr + 0x01);
    }
  }
  //
  // Wait until SLA+R transmitted
  //
  while (1) {
    Status = _I2C_CheckStatus();
    if (Status == 0x40) {         // Data transmitted and ACK received
      break;
    } else if (Status != 0xF8) {  // Error
      r = I2C_CODE_ERROR;
      break;
    }
  }
  if (r == I2C_CODE_OK) {
    //
    // Wait until address transmitted and receive data
    //
    for (i = 1; i <= NumBytesToRead; i++) {
      //
      // Wait until data transmitted
      //
      while (1) {
        Status = _I2C_CheckStatus();
        if ((Status == 0x40) || (Status == 0x48) || (Status == 0x50)) {  // Data received
          //
          // Set generate NACK
          //
          if (i == NumBytesToRead) {
            r = _I2C_GetChar(I2C_MODE_ACK1, pData);
          } else {
            r = _I2C_GetChar(I2C_MODE_ACK0, pData);
          }
          r = _I2C_GetChar(I2C_MODE_READ, pData);  // Read data
          while (r == I2C_CODE_EMPTY) {
            r = _I2C_GetChar(I2C_MODE_READ, pData);
          }
          pData++;
          break;
        } else if (Status != 0xF8) {  // Error
          i = NumBytesToRead;
          r = I2C_CODE_ERROR;
          break;
        }
      }
    }
  }
  _I2C_Stop();  // Generate Stop condition
  return r;
}

/*********************************************************************
*
*       _Write
*
* Function description
*   Writes to a device on I2C.
*/
static U8 _Write(U8 Addr, U8 * pData, U16 NumBytesToWrite, U8 PartOfWriteRead) {
  U8  r;
  U16 i;

  r = _I2C_Start();  // Generate Start condition
  if(r == I2C_CODE_OK) {
    r = _I2C_WriteWithWait(Addr);  // Write address (SLA+W)
    if (r == I2C_CODE_OK) {
      //
      // Write data
      //
      for (i = 0; i < NumBytesToWrite; i++) {
        r = _I2C_WriteWithWait(*pData);
        if (r != I2C_CODE_OK) {
          PartOfWriteRead = 0;  // Override to generate stop condition on error
          break;
        }
        pData++;
      }
    }
  }
  if (PartOfWriteRead == 0) {
    _I2C_Stop();  // Generate Stop condition
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
*       I2C_Read
*
* Function description
*   Reads from a device on I2C.
*/
U8 I2C_Read(U32 I2CBaseAddr, U8 Addr, U8 * pData, U16 NumBytesToRead) {
  U8 r;

  SET_BASE_ADDR(I2CBaseAddr);
  r = _Read(Addr << 1, pData, NumBytesToRead, 0);
  return r;
}

/*********************************************************************
*
*       I2C_Write
*
* Function description
*   Writes to a device on I2C.
*/
U8 I2C_Write(U32 I2CBaseAddr, U8 Addr, U8 * pData, U16 NumBytesToWrite) {
  U8 r;

  SET_BASE_ADDR(I2CBaseAddr);
  r = _Write(Addr << 1, pData, NumBytesToWrite, 0);
  return r;
}

/*********************************************************************
*
*       I2C_WriteRead
*
* Function description:
*   Writes and reads from a device on I2C.
*/
U8 I2C_WriteRead(U32 I2CBaseAddr, U8 Addr, U8 * pData, U16 NumBytesToWrite, U8 * pBuf, U16 NumBytesToRead) {
  U8 r;

  SET_BASE_ADDR(I2CBaseAddr);
  r = _Write(Addr << 1, pData, NumBytesToWrite, 1);
  if (r == I2C_CODE_OK) {
    r = _Read(Addr << 1, pBuf, NumBytesToRead, 1);
  }
  return r;
}

/*********************************************************************
*
*       I2C_Init()
*/
void I2C_Init(U32 I2CBaseAddr, U32 PeripheralClock, U32 ClockRate) {
  U32 Div;

  if (_I2CBaseAddr == I2CBaseAddr) {
    return;  // We have used this I2C before, nothing changed
  }
  if (_IsInited == 0) {
    _IsInited = 1;
  }
  SET_BASE_ADDR(I2CBaseAddr);
  //
  // Clear flags and disable
  //
  I2C_CONCLR = 0
               | (1 << 2)  // AAC,   Assert acknowledge clear bit
               | (1 << 3)  // SIC,   I2C interrupt clear bit
               | (1 << 5)  // STAC,  START flag clear bit
               | (1 << 6)  // I2ENC, I2C interface disable bit
               ;
  //
  // Set clock rate
  //
  Div      = PeripheralClock / ClockRate;
  I2C_SCLH = Div / 2;            // SCL Duty Cycle high register
  I2C_SCLL = (Div - (Div / 2));  // SCL Duty Cycle low register
  //
  // Enable I2C
  //
  I2C_CONSET |= (1 << 6);
}

/*************************** End of file ****************************/
