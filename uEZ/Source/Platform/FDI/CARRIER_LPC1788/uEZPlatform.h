/*-------------------------------------------------------------------------*
* File:  uEZPlatform.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ platform file definitions.
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
#include <uEZNetwork.h>
#include <Types/GPIO.h>
#include <uEZPlatformAPI.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Platform Settings:
 *-------------------------------------------------------------------------*/
#define UEZ_PROCESSOR                       NXP_LPC1788

#ifndef UEZ_NUM_HANDLES
#define UEZ_NUM_HANDLES           150 // more than the average number of handles
#endif

#ifndef PROCESSOR_OSCILLATOR_FREQUENCY
#define PROCESSOR_OSCILLATOR_FREQUENCY      120000000   // clocks per second (when using LCD, must be slower)
#endif

#ifndef SDRAM_CLOCK_FREQUENCY
    #define SDRAM_CLOCK_FREQUENCY               (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif

#ifndef EMC_CLOCK_FREQUENCY
    #define EMC_CLOCK_FREQUENCY                 (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif

#ifndef UEZBSP_SDRAM
    #define UEZBSP_SDRAM                        1
    #define UEZBSP_SDRAM_SIZE                   (8*1024*1024)
    #define UEZBSP_SDRAM_BASE_ADDR              0xA0000000
#endif

/*-------------------------------------------------------------------------*
 * Platform Volume Settings:
 *-------------------------------------------------------------------------*/
#define AUDIO_AMP_NONE                        0   // Unique number per AMP
#define AUDIO_AMP_TDA8551                     1
#define AUDIO_AMP_WOLFSON                     2
#define AUDIO_AMP_LM48110                     3

// Set default audio levels for this platform
#ifndef UEZ_DEFAULT_AUDIO_LEVEL
    #define UEZ_DEFAULT_AUDIO_LEVEL  255 // default master volume level // will lower OB speaker level
#endif

#ifndef UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL
    #define UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL  140 // Limit audio to 0.5W output
#endif

// Max output limit changes based on AMP
#define UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_TDA8551 212 // Limit audio to 0.5W output
#define UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_LM48110 140 // Limit audio to 0.5W output

// Define these for your own speakers or headphones.
#ifndef UEZ_DEFAULT_OFFBOARD_SPEAKER_AUDIO_LEVEL
    #define UEZ_DEFAULT_OFFBOARD_SPEAKER_AUDIO_LEVEL  255
#endif
#ifndef UEZ_DEFAULT_ONBOARD_HEADPHONES_AUDIO_LEVEL
    #define UEZ_DEFAULT_ONBOARD_HEADPHONES_AUDIO_LEVEL  255
#endif
#ifndef UEZ_DEFAULT_OFFBOARD_HEADPHONES_AUDIO_LEVEL
    #define UEZ_DEFAULT_OFFBOARD_HEADPHONES_AUDIO_LEVEL  255
#endif
/*-------------------------------------------------------------------------*/

#ifndef UEZ_CONSOLE_READ_BUFFER_SIZE
    #define UEZ_CONSOLE_READ_BUFFER_SIZE        128 // bytes
#endif
#ifndef UEZ_CONSOLE_WRITE_BUFFER_SIZE
    #define UEZ_CONSOLE_WRITE_BUFFER_SIZE       128 // bytes
#endif

#ifndef FATFS_MAX_MASS_STORAGE_DEVICES
    #define FATFS_MAX_MASS_STORAGE_DEVICES      2
#endif

#define UEZBSP_NOR_FLASH                    UEZ_ENABLE_FLASH
#define UEZBSP_NOR_FLASH_BASE_ADDRESS       0x80000000

#ifndef UEZBSP_EXTERNAL_FLASH_BASE_ADDRESS
    #define UEZBSP_EXTERNAL_FLASH_BASE_ADDRESS  UEZBSP_NOR_FLASH_BASE_ADDRESS
#endif

#include <lwipopts.h>

#if (COMPILER_TYPE==IAR)
    #define NOP() __no_operation()
#endif

#ifndef UEZGUI_7_REV2_BOARD_SWAP_TP_LINES
  #define UEZGUI_7_REV2_BOARD_SWAP_TP_LINES           1
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

 typedef enum {
  HARDWARE_TEST_NORMAL = 0,
  HARDWARE_TEST_CRYSTAL = 1,
  HARDWARE_TEST_SDRAM = 2,
  HARDWARE_TEST_I2C = 3,
  HARDWARE_TEST_AUDIO = 4
} HARDWARE_TEST_Types;

typedef struct {
    uint8_t iTestMode;    
    uint8_t iReserved;
    uint16_t iTestIterations;
} T_BootTestModeSetting;
 
/*-------------------------------------------------------------------------*
 * General Purpose Pin Mappings to CPU:
 *-------------------------------------------------------------------------*/
// HW Reset pin in Rev X PCB
#define PIN_HW_RESET  GPIO_NONE 

// LED pin(s)
#define GPIO_HEARTBEAT_LED  		GPIO_P4_23

// GPIO Loopback Test array for this uEZGUI
#define LOOPBACK_TEST_NUM_PINS_A              (16)
#define LOOPBACK_TEST_NUM_PINS_B              (1)
#define LOOPBACK_TEST_NUM_PINS_C              (1)

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void UEZPlatform_SSP0_Require(void);
void UEZPlatform_SSP1_Require(void);
void UEZPlatform_Accel0_Require(void);
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
void UEZPlatform_AudioMixer_Require(void);
void UEZPlatform_Backlight_Require(void);
void UEZPlatform_Console_Expansion_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_Console_ALT_PWR_COM_Require(
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
void UEZPlatform_Console_HalfDuplex_RS485_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize,
        T_uezGPIOPortPin aDriveEnablePortPin,
        TBool aDriveEnablePolarity,
        TUInt32 aDriveEnableReleaseTime,
        T_uezGPIOPortPin aReceiveEnablePortPin,
        TBool aReceiveEnablePolarity,
        TUInt32 aReceiveEnableReleaseTime);
void UEZPlatform_Console_ISPHeader_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_CRC0_Require(void);
void UEZPlatform_DAC0_Require(void);
void UEZPlatform_EEPROM_I2C_Require(void);
void UEZPlatform_EEPROM_LPC1788_Require(void);
void UEZPlatform_EEPROM0_Require(void);
void UEZPlatform_EMAC_Require(void);
void UEZPlatform_ERTC_Require(void);
void UEZPlatform_FileSystem_Require(void);
void UEZPlatform_I2C0_Require(void);
void UEZPlatform_I2C1_Require(void);
void UEZPlatform_I2C2_Require(void);
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
void UEZPlatform_LCD_Require(void);
void UEZPlatform_MS0_Require(void);
void UEZPlatform_MS1_Require(void);
void UEZPlatform_PWM0_Require(void);
void UEZPlatform_PWM1_Require(void);
void UEZPlatform_RTC_Require(void);
void UEZPlatform_SDCard_Drive_Require(TUInt8 aDriveNum);
void UEZPlatform_Timer0_Require(void);
void UEZPlatform_Timer1_Require(void);
void UEZPlatform_Timer2_Require(void);
void UEZPlatform_Timer3_Require(void);
void UEZPlatform_Speaker_Require(void);
void UEZPlatform_Temp0_Require(void);
void UEZPlatform_Touchscreen_Require(void);
void UEZPlatform_USBDevice_Require(void);
void UEZPlatform_USBFlash_Drive_Require(TUInt8 aDriveNum);
void UEZPlatform_USBHost_Require(void);
void UEZPlatform_USBHost_PortA_Require(void);
TBool UEZPlatform_Host_Port_B_Detect();
void UEZPlatform_USBHost_PortB_Require(void);
void UEZPlatform_Watchdog_Require(void);
void UEZPlatform_WiredNetwork0_Require(void);
void UEZPlatform_WirelessNetwork0_Require(void);

void UEZPlatform_Standard_Require(void);
void UEZPlatform_Full_Require(void);
void UEZPlatform_Minimal_Require(void);
void UEZPlatform_AudioCodec_Require(void);
void UEZPlatform_LED_Require(void);
void UEZPlatform_Button_Require(void);
void UEZPlatform_WiFiProgramMode(TBool runMode);
void UEZPlatform_System_Reset(void);

// Utility function to connect to the Wired Network
T_uezError UEZPlatform_WiredNetwork0_Connect(
        T_uezDevice *aNetwork,
        T_uezNetworkStatus *aStatus);
T_uezError UEZPlatform_WirelessNetwork0_Connect(
        T_uezDevice *aNetwork,
        T_uezNetworkStatus *aStatus);

// Information available about the connected wired network
extern T_uezDevice G_network;
extern T_uezNetworkStatus G_networkStatus;
        
extern TUInt8 *_framesMemoryptr;
#define LCD_DISPLAY_BASE_ADDRESS (TUInt32) _framesMemoryptr //Keep this define for now

// test functions/externs
#if (CONFIG_LOW_LEVEL_TEST_CODE == 1)
extern volatile T_BootTestModeSetting G_hardwareTest;
#endif
void UEZPlatform_INIT_LOW_LEVEL_UART_DEFAULT_CLOCK(void);
void UEZPlatform_INIT_LOW_LEVEL_UART(void);
void COM_Send(char * bytes, uint16_t numBytes);
void UEZBSP_HEARTBEAT_TOGGLE(void);

extern const T_uezGPIOPortPin g_loopback_pins_A[];
extern const uint8_t g_loopback_connected_A[][LOOPBACK_TEST_NUM_PINS_A];

extern const T_uezGPIOPortPin g_loopback_pins_B[];
extern const uint8_t g_loopback_connected_B[][LOOPBACK_TEST_NUM_PINS_B];

extern const T_uezGPIOPortPin g_loopback_pins_C[];
extern const uint8_t g_loopback_connected_C[][LOOPBACK_TEST_NUM_PINS_C];

#ifdef __cplusplus
}
#endif
		
#endif // UEZPLATFORM_SETTINGS_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform_Settings.h
 *-------------------------------------------------------------------------*/
