/*-------------------------------------------------------------------------*
 * File:  LPC546xx_UtilityFuncs.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef LPC546xx_UTILITYFUNCS_H_
#define LPC546xx_UTILITYFUNCS_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZGPIO.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define CLK_GATE_REG_OFFSET_SHIFT 8U
#define CLK_GATE_REG_OFFSET_MASK 0xFFFFFF00U
#define CLK_GATE_BIT_SHIFT_SHIFT 0U
#define CLK_GATE_BIT_SHIFT_MASK 0x000000FFU

#define CLK_GATE_DEFINE(reg_offset, bit_shift)                                  \
    ((((reg_offset) << CLK_GATE_REG_OFFSET_SHIFT) & CLK_GATE_REG_OFFSET_MASK) | \
     (((bit_shift) << CLK_GATE_BIT_SHIFT_SHIFT) & CLK_GATE_BIT_SHIFT_MASK))

#define CLK_GATE_ABSTRACT_REG_OFFSET(x) (((TUInt32)(x)&CLK_GATE_REG_OFFSET_MASK) >> CLK_GATE_REG_OFFSET_SHIFT)
#define CLK_GATE_ABSTRACT_BITS_SHIFT(x) (((TUInt32)(x)&CLK_GATE_BIT_SHIFT_MASK) >> CLK_GATE_BIT_SHIFT_SHIFT)

#define AHB_CLK_CTRL0 0
#define AHB_CLK_CTRL1 1
#define AHB_CLK_CTRL2 2
#define ASYNC_CLK_CTRL0 3

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
    T_uezGPIOPortPin iPortPin;
    TUInt32 iSetting;
} T_LPC546xx_ICON_ConfigList;

typedef enum _clock_ip_name
{
    kCLOCK_IpInvalid = 0U,
    kCLOCK_Rom = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 1),
    kCLOCK_Sram1 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 3),
    kCLOCK_Sram2 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 4),
    kCLOCK_Sram3 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 5),
    kCLOCK_Flash = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 7),
    kCLOCK_Fmc = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 8),
    kCLOCK_Eeprom = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 9),
    kCLOCK_Spifi = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 10),
    kCLOCK_InputMux = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 11),
    kCLOCK_Iocon = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 13),
    kCLOCK_Gpio0 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 14),
    kCLOCK_Gpio1 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 15),
    kCLOCK_Gpio2 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 16),
    kCLOCK_Gpio3 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 17),
    kCLOCK_Pint = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 18),
    kCLOCK_Gint = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 19),
    kCLOCK_Dma = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 20),
    kCLOCK_Crc = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 21),
    kCLOCK_Wwdt = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 22),
    kCLOCK_Rtc = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 23),
    kCLOCK_Adc0 = CLK_GATE_DEFINE(AHB_CLK_CTRL0, 27),
    kCLOCK_Mrt = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 0),
    kCLOCK_Rit = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 1),
    kCLOCK_Sct0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 2),
    kCLOCK_Mcan0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 7),
    kCLOCK_Mcan1 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 8),
    kCLOCK_Utick = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 10),
    kCLOCK_FlexComm0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 11),
    kCLOCK_FlexComm1 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 12),
    kCLOCK_FlexComm2 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 13),
    kCLOCK_FlexComm3 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 14),
    kCLOCK_FlexComm4 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 15),
    kCLOCK_FlexComm5 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 16),
    kCLOCK_FlexComm6 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 17),
    kCLOCK_FlexComm7 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 18),
    kCLOCK_MinUart0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 11),
    kCLOCK_MinUart1 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 12),
    kCLOCK_MinUart2 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 13),
    kCLOCK_MinUart3 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 14),
    kCLOCK_MinUart4 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 15),
    kCLOCK_MinUart5 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 16),
    kCLOCK_MinUart6 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 17),
    kCLOCK_MinUart7 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 18),
    kCLOCK_LSpi0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 11),
    kCLOCK_LSpi1 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 12),
    kCLOCK_LSpi2 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 13),
    kCLOCK_LSpi3 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 14),
    kCLOCK_LSpi4 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 15),
    kCLOCK_LSpi5 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 16),
    kCLOCK_LSpi6 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 17),
    kCLOCK_LSpi7 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 18),
    kCLOCK_BI2c0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 11),
    kCLOCK_BI2c1 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 12),
    kCLOCK_BI2c2 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 13),
    kCLOCK_BI2c3 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 14),
    kCLOCK_BI2c4 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 15),
    kCLOCK_BI2c5 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 16),
    kCLOCK_BI2c6 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 17),
    kCLOCK_BI2c7 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 18),
    kCLOCK_FlexI2s0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 11),
    kCLOCK_FlexI2s1 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 12),
    kCLOCK_FlexI2s2 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 13),
    kCLOCK_FlexI2s3 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 14),
    kCLOCK_FlexI2s4 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 15),
    kCLOCK_FlexI2s5 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 16),
    kCLOCK_FlexI2s6 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 17),
    kCLOCK_FlexI2s7 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 18),
    kCLOCK_DMic = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 19),
    kCLOCK_Ct32b2 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 22),
    kCLOCK_Usbd0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 25),
    kCLOCK_Ct32b0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 26),
    kCLOCK_Ct32b1 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 27),
    kCLOCK_BodyBias0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 29),
    kCLOCK_EzhArchB0 = CLK_GATE_DEFINE(AHB_CLK_CTRL1, 31),
    kCLOCK_Lcd = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 2),
    kCLOCK_Sdio = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 3),
    kCLOCK_Usbh1 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 4),
    kCLOCK_Usbd1 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 5),
    kCLOCK_UsbRam1 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 6),
    kCLOCK_Emc = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 7),
    kCLOCK_Eth = CLK_GATE_DEFINE(AHB_CLK_CTRL2,8),
    kCLOCK_Gpio4 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 9),
    kCLOCK_Gpio5 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 10),
    kCLOCK_Aes = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 11),
    kCLOCK_Otp = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 12),
    kCLOCK_Rng = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 13),
    kCLOCK_FlexComm8 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 14),
    kCLOCK_FlexComm9 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 15),
    kCLOCK_MinUart8 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 14),
    kCLOCK_MinUart9 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 15),
    kCLOCK_LSpi8 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 14),
    kCLOCK_LSpi9 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 15),
    kCLOCK_BI2c8 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 14),
    kCLOCK_BI2c9 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 15),
    kCLOCK_FlexI2s8 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 14),
    kCLOCK_FlexI2s9 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 15),
    kCLOCK_Usbhmr0 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 16),
    kCLOCK_Usbhsl0 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 17),
    kCLOCK_Sha0 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 18),
    kCLOCK_SmartCard0 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 19),
    kCLOCK_SmartCard1 = CLK_GATE_DEFINE(AHB_CLK_CTRL2, 20),

    kCLOCK_Ct32b3 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 13),
    kCLOCK_Ct32b4 = CLK_GATE_DEFINE(ASYNC_CLK_CTRL0, 14)
} clock_ip_name_t;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void CPUDisableInterrupts(void);
void CPUEnableInterrupts(void);

void LPC546xxPowerOn(TUInt32 bits);
void LPC546xxPowerOff(TUInt32 bits);

TUInt32 ReadLE32U(volatile TUInt8 *pmem);
void WriteLE32U(volatile TUInt8 *pmem, TUInt32 val);
TUInt32 ReadBE32U(volatile TUInt8 *pmem);
void WriteBE32U(volatile TUInt8 *pmem, TUInt32 val);
TUInt16 ReadLE16U(volatile TUInt8 *pmem);
void WriteLE16U(volatile TUInt8 *pmem, TUInt16 val);
TUInt16  ReadBE16U (volatile  TUInt8  *pmem);
void WriteBE16U(volatile TUInt8 *pmem, TUInt16 val);

void LPC546xx_ICON_ConfigPin(
        T_uezGPIOPortPin aPortPin,
        const T_LPC546xx_ICON_ConfigList *aList,
        TUInt8 aCount);
void LPC546xx_ICON_ConfigPinOrNone(
        T_uezGPIOPortPin aPortPin,
        const T_LPC546xx_ICON_ConfigList *aList,
        TUInt8 aCount);

#endif // LPC546xx_UTILITYFUNCS_H_
/*-------------------------------------------------------------------------*
 * End of File:  LPC546xx_UtilityFuncs.h
 *-------------------------------------------------------------------------*/
