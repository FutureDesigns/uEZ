/*-------------------------------------------------------------------------*
 * File:  uEZPlatform_Settings.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef UEZPLATFORM_SETTINGS_H_
#define UEZPLATFORM_SETTINGS_H_

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
#include <Config.h>
#include <UEZNetwork.h>
#include <Types/GPIO.h>
#include <UEZPlatformAPI.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Platform Settings:
 *-------------------------------------------------------------------------*/
#define UEZ_PROCESSOR                           NXP_LPC1768

#ifndef UEZ_NUM_HANDLES
#define UEZ_NUM_HANDLES                         150 // more than the average number of handles
#endif

#ifndef PROCESSOR_OSCILLATOR_FREQUENCY
#define PROCESSOR_OSCILLATOR_FREQUENCY          120000000   // clocks per second (when using LCD, must be slower)
#endif

#ifndef SDRAM_CLOCK_FREQUENCY
    #define SDRAM_CLOCK_FREQUENCY               (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif

#ifndef EMC_CLOCK_FREQUENCY
    #define EMC_CLOCK_FREQUENCY                 (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif
/*
#ifndef UEZBSP_SDRAM
    #define UEZBSP_SDRAM                        0
    #define UEZBSP_SDRAM_SIZE                   (8*1024*1024)
    #define UEZBSP_SDRAM_BASE_ADDR              0xA0000000
#endif*/

#ifndef UEZ_CONSOLE_READ_BUFFER_SIZE
    #define UEZ_CONSOLE_READ_BUFFER_SIZE        128 // bytes
#endif
#ifndef UEZ_CONSOLE_WRITE_BUFFER_SIZE
    #define UEZ_CONSOLE_WRITE_BUFFER_SIZE       128 // bytes
#endif

#ifndef FATFS_MAX_MASS_STORAGE_DEVICES
    #define FATFS_MAX_MASS_STORAGE_DEVICES      2
#endif

#include <lwipopts.h>

//#if (COMPILER_TYPE==IAR)
//    #define NOP() __no_operation()
//#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void UEZPlatform_ADC0_Require(void);
void UEZPlatform_ADC0_0_Require(void);
void UEZPlatform_ADC0_1_Require(void);
void UEZPlatform_ADC0_2_Require(void);
void UEZPlatform_ADC0_3_Require(void);
void UEZPlatform_ADC0_4_Require(void);
void UEZPlatform_ADC0_5_Require(void);
void UEZPlatform_ADC0_6_Require(void);
void UEZPlatform_ADC0_7_Require(void);
void UEZPlatform_AudioAmp_Require(void);
void UEZPlatform_Backlight_Require(void);
void UEZPlatform_Button_Require(void);
void UEZPlatform_CharDisplay_Require(void);
void UEZPlatform_Console_Expansion_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_Console_FullDuplex_UART_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_Console_FullDuplex_UART0_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_Console_FullDuplex_UART1_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_Console_FullDuplex_UART2_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_Console_FullDuplex_UART3_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_Console_HalfDuplex_UART_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize,
        T_uezGPIOPortPin aDriveEnablePortPin,
        TBool aDriveEnablePolarity,
        TUInt32 aDriveEnableReleaseTime);
void UEZPlatform_Console_ISPHeader_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_DAC0_Require(void);
void UEZPlatform_EEPROM_I2C_Require(void);
void UEZPlatform_EEPROM_LPC1788_Require(void);
void UEZPlatform_EEPROM0_Require(void);
void UEZPlatform_EMAC_Require(void);
void UEZPlatform_ERTC_Require(void);
void UEZPlatform_ExternalInterrupts_Require(void);
void UEZPlatform_FileSystem_Require(void);
void UEZPlatform_I2C0_Require(void);
//void UEZPlatform_I2C1_Require(void);
//void UEZPlatform_I2C2_Require(void);
void UEZPlatform_IAP_Require(void);
void UEZPlatform_IRTC_Require(void);
void UEZPlatform_Flash0_Require(void);
void UEZPlatform_GPDMA0_Require(void);
void UEZPlatform_GPDMA1_Require(void);
void UEZPlatform_GPDMA2_Require(void);
void UEZPlatform_GPDMA3_Require(void);
void UEZPlatform_GPDMA4_Require(void);
void UEZPlatform_GPDMA5_Require(void);
void UEZPlatform_GPDMA6_Require(void);
void UEZPlatform_GPDMA7_Require(void);
void UEZPlatform_Keypad_Require(void);
void UEZPlatform_LCD_Require(void);
void UEZPlatform_MS0_Require(void);
void UEZPlatform_MS1_Require(void);
void UEZPlatform_PWM0_Require(void);
void UEZPlatform_PWM1_Require(void);
void UEZPlatform_RTC_Require(void);
void UEZPlatform_SDCard_Drive_Require(TUInt8 aDriveNum);
void UEZPlatform_Speaker_Require(void);
void UEZPlatform_TempLocal_Require(void);
void UEZPlatform_TempRemote_Require(void);
void UEZPlatform_Timer0_Require(void);
void UEZPlatform_Timer1_Require(void);
void UEZPlatform_Timer2_Require(void);
void UEZPlatform_Timer3_Require(void);
void UEZPlatform_Touchscreen_Require(void);
void UEZPlatform_USBDevice_Require(void);
void UEZPlatform_USBFlash_Drive_Require(TUInt8 aDriveNum);
void UEZPlatform_USBHost_Require(void);
void UEZPlatform_USBHost_PortA_Require(void);
void UEZPlatform_USBHost_PortB_Require(void);
void UEZPlatform_Watchdog_Require(void);
void UEZPlatform_WiredNetwork0_Require(void);

void UEZPlatform_Require(void);
void UEZPlatform_Standard_Require(void);
void UEZPlatform_Full_Require(void);
void UEZPlatform_Minimal_Require(void);

// Utility function to connect to the Wired Network
T_uezError UEZPlatform_WiredNetwork0_Connect(
        T_uezDevice *aNetwork,
        T_uezNetworkStatus *aStatus);

#ifdef __cplusplus
}
#endif
	
#endif // UEZPLATFORM_SETTINGS_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform_Settings.h
 *-------------------------------------------------------------------------*/
