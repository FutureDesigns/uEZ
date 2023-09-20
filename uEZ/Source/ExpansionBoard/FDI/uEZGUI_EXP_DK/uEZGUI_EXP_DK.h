/*-------------------------------------------------------------------------*
* File:  uEZGUI_EXP_DK.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZGUI-EXP-DK Expansion board platform file definitions.
*-------------------------------------------------------------------------*/
#ifndef uEZGUI_EXP_DK_H_
#define uEZGUI_EXP_DK_H_

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
#include <uEZPlatform.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EXP_DK_DEFINED                  1


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
// I2CMux Device Names:
#define UEZGUI_EXP_DK_I2CMUX_BUS_0_NAME     "EXPDK_I2C0"
#define UEZGUI_EXP_DK_I2CMUX_BUS_1_NAME     "EXPDK_I2C1"
#define UEZGUI_EXP_DK_I2CMUX_BUS_2_NAME     "EXPDK_I2C2"
#define UEZGUI_EXP_DK_I2CMUX_BUS_3_NAME     "EXPDK_I2C3"

// ----------------------------------------------------------------------------
// GPIO Mapping table for Primary expansion
// ----------------------------------------------------------------------------
//#define --GND--                         GPIO_PRIMARY_EXPANSION_PIN_1
#define GPIO_EXPDK_RX62CRX_SCL          GPIO_PRIMARY_EXPANSION_PIN_2
#define GPIO_EXPDK_RX62CTX_SDA          GPIO_PRIMARY_EXPANSION_PIN_3
#define GPIO_EXPDK_RX62SCL_DALIRXD      GPIO_PRIMARY_EXPANSION_PIN_4
#define GPIO_EXPDK_RX62SDA_MCICLK       GPIO_PRIMARY_EXPANSION_PIN_5
#define GPIO_EXPDK_485DE                GPIO_PRIMARY_EXPANSION_PIN_6
#define GPIO_EXPDK_485RE                GPIO_PRIMARY_EXPANSION_PIN_7
#define GPIO_EXPDK_485RXD               GPIO_PRIMARY_EXPANSION_PIN_8
#define GPIO_EXPDK_485TXD               GPIO_PRIMARY_EXPANSION_PIN_9
//#define --GND--                         GPIO_PRIMARY_EXPANSION_PIN_10
#define GPIO_EXPDK_USBHDM               GPIO_PRIMARY_EXPANSION_PIN_11
#define GPIO_EXPDK_USBHDP               GPIO_PRIMARY_EXPANSION_PIN_12
#define GPIO_EXPDK_USBHPWRD             GPIO_PRIMARY_EXPANSION_PIN_13
#define GPIO_EXPDK_USBHOVC              GPIO_PRIMARY_EXPANSION_PIN_14
#define GPIO_EXPDK_USBHPPWR             GPIO_PRIMARY_EXPANSION_PIN_15
#define GPIO_EXPDK_I2STXSDA_MOSI        GPIO_PRIMARY_EXPANSION_PIN_16
#define GPIO_EXPDK_I2STXWS_MISO         GPIO_PRIMARY_EXPANSION_PIN_17
#define GPIO_EXPDK_I2STXCLK_SCK         GPIO_PRIMARY_EXPANSION_PIN_18
#define GPIO_EXPDK_I2SRXSDA_SSEL        GPIO_PRIMARY_EXPANSION_PIN_19
#define GPIO_EXPDK_I2SRXWS              GPIO_PRIMARY_EXPANSION_PIN_20
#define GPIO_EXPDK_I2CIRQ               GPIO_PRIMARY_EXPANSION_PIN_21
//#define --GND--                         GPIO_PRIMARY_EXPANSION_PIN_22
#define GPIO_EXPDK_RESETIN              GPIO_PRIMARY_EXPANSION_PIN_23
#define GPIO_EXPDK_RESETOUT             GPIO_PRIMARY_EXPANSION_PIN_24
#define GPIO_EXPDK_AOUT                 GPIO_PRIMARY_EXPANSION_PIN_25
//#define --UNUSED--                      GPIO_PRIMARY_EXPANSION_PIN_26
#define GPIO_EXPDK_ENETMDC              GPIO_PRIMARY_EXPANSION_PIN_27
#define GPIO_EXPDK_ENETMDIO             GPIO_PRIMARY_EXPANSION_PIN_28
//#define --GND--                         GPIO_PRIMARY_EXPANSION_PIN_29
#define GPIO_EXPDK_ENETREFCLK           GPIO_PRIMARY_EXPANSION_PIN_30
#define GPIO_EXPDK_ENETRXER             GPIO_PRIMARY_EXPANSION_PIN_31
//#define --3V3--                         GPIO_PRIMARY_EXPANSION_PIN_32
#define GPIO_EXPDK_ENETRXD1             GPIO_PRIMARY_EXPANSION_PIN_33
#define GPIO_EXPDK_ENETRXD0             GPIO_PRIMARY_EXPANSION_PIN_34
#define GPIO_EXPDK_ENETCRSDV            GPIO_PRIMARY_EXPANSION_PIN_35
#define GPIO_EXPDK_ENETTXEN             GPIO_PRIMARY_EXPANSION_PIN_36
#define GPIO_EXPDK_ENETTXD1             GPIO_PRIMARY_EXPANSION_PIN_37
#define GPIO_EXPDK_ENETTXD0             GPIO_PRIMARY_EXPANSION_PIN_38
//#define --GND--                         GPIO_PRIMARY_EXPANSION_PIN_39
#define GPIO_EXPDK_IRQ_ISPENTRY         GPIO_PRIMARY_EXPANSION_PIN_40
#define GPIO_EXPDK_RS232RXD             GPIO_PRIMARY_EXPANSION_PIN_41
#define GPIO_EXPDK_RS232TXD             GPIO_PRIMARY_EXPANSION_PIN_42
#define GPIO_EXPDK_USBDDP               GPIO_PRIMARY_EXPANSION_PIN_43
#define GPIO_EXPDK_USBDDM               GPIO_PRIMARY_EXPANSION_PIN_44
#define GPIO_EXPDK_USBDVBUS             GPIO_PRIMARY_EXPANSION_PIN_45
//#define --5V0--                         GPIO_PRIMARY_EXPANSION_PIN_46
//#define --5V0--                         GPIO_PRIMARY_EXPANSION_PIN_47
//#define --5V0--                         GPIO_PRIMARY_EXPANSION_PIN_48
//#define --3V3--                         GPIO_PRIMARY_EXPANSION_PIN_49
//#define --3V3--                         GPIO_PRIMARY_EXPANSION_PIN_50

// ----------------------------------------------------------------------------
// GPIO Mapping table for Secondary expansion
// ----------------------------------------------------------------------------
//#define --GND--                         GPIO_SECONDARY_EXPANSION_PIN_1
#define GPIO_EXPDK_USBDID               GPIO_SECONDARY_EXPANSION_PIN_2
//#define --UNUSED--                      GPIO_SECONDARY_EXPANSION_PIN_3
#define GPIO_EXPDK_DALITXD              GPIO_SECONDARY_EXPANSION_PIN_4
#define GPIO_EXPDK_MCIDAT3              GPIO_SECONDARY_EXPANSION_PIN_5
#define GPIO_EXPDK_MCIDAT2              GPIO_SECONDARY_EXPANSION_PIN_6
#define GPIO_EXPDK_MCIDAT1              GPIO_SECONDARY_EXPANSION_PIN_7
#define GPIO_EXPDK_MCIDAT0              GPIO_SECONDARY_EXPANSION_PIN_8
#define GPIO_EXPDK_FETPWM               GPIO_SECONDARY_EXPANSION_PIN_9
#define GPIO_EXPDK_MCICMD               GPIO_SECONDARY_EXPANSION_PIN_10
#define GPIO_EXPDK_NXPCANTD             GPIO_SECONDARY_EXPANSION_PIN_11
#define GPIO_EXPDK_NXPCANRD             GPIO_SECONDARY_EXPANSION_PIN_12
//#define --5V0--                         GPIO_SECONDARY_EXPANSION_PIN_13
//#define --GND--                         GPIO_SECONDARY_EXPANSION_PIN_14
//#define --UNUSED--                      GPIO_SECONDARY_EXPANSION_PIN_15
#define GPIO_USB_HOST_ENABLE            GPIO_SECONDARY_EXPANSION_PIN_16
#define GPIO_EXPDK_ADC                  GPIO_SECONDARY_EXPANSION_PIN_17
//#define --UNUSED--                      GPIO_SECONDARY_EXPANSION_PIN_18
//#define --UNUSED--                      GPIO_SECONDARY_EXPANSION_PIN_19
//#define --GND--                         GPIO_SECONDARY_EXPANSION_PIN_20


// Audio Amp configuration:
#define EXPDK_AUDIO_I2C                     PRIMARY_EXPANSION_I2C_A
#define UEZGUI_EXP_DK_AudioI2C_Require      UEZPlatform_ExpansionPrimary_I2C_A_Require
#define EXPDK_AUDIO_AMP_DEFAULT_LEVEL       UEZ_DEFAULT_AUDIO_LEVEL

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_AudioAmp0_Require(void);
void UEZGUI_EXP_DK_AudioAmp1_Require(void);
void UEZGUI_EXP_DK_AudioCodec_Require(void);
void UEZGUI_EXP_DK_AudioMixer_Require(void);
void UEZGUI_EXP_DK_CAN_Require(void);
void UEZGUI_EXP_DK_DALI_Require(void);
void UEZGUI_EXP_DK_EMAC_Require(void);
void UEZGUI_EXP_DK_I2CMux_Require(void);
void UEZGUI_EXP_DK_I2S_Require(void);
void UEZGUI_EXP_DK_Button_Require(void);
void UEZGUI_EXP_DK_LED_Require(void);
void UEZGUI_EXP_DK_LightSensor_Require(void);
void UEZGUI_EXP_DK_ProximitySensor_Require(void);
void UEZGUI_EXP_DK_RS232_Require(
    TUInt32 aWriteBufferSize,
    TUInt32 aReadBufferSize);
void UEZGUI_EXP_DK_FullDuplex_RS485_Require(
    TUInt32 aWriteBufferSize,
    TUInt32 aReadBufferSize);
void UEZGUI_EXP_DK_SDCard_MCI_Require(TUInt32 aDriveNumber);
void UEZGUI_EXP_DK_USBHost_Require(void);
void UEZGUI_EXP_DK_USBDevice_Require(void);
void UEZGUI_EXP_DK_GainSpan_PMOD_Require(void);
TBool UEZGUI_EXP_DK_USB_Host_Port_B_Detect(void);

TBool UEZGUI_EXP_DK_Detect(void);

#ifdef __cplusplus
}
#endif

#endif // uEZGUI_EXP_DK_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZExpansionBoard.h
 *-------------------------------------------------------------------------*/
