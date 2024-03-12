/*-------------------------------------------------------------------------*
* File:  uEZPlatform.h
*--------------------------------------------------------------------------*
* Description:
*         The uEZ platform file definitions
*-------------------------------------------------------------------------*/
/**
 *  @file   Platform\FDI\uEZGUI_4088_43WQN\uEZPlatform.h
 *  @brief  uEZ Platform File Definitions
 *
 * The uEZ platform file definitions
 */
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
#define UEZ_PROCESSOR                       NXP_LPC4088

#ifndef UEZ_ENABLE_TOUCHSCREEN_CALIBRATION
  #if (USE_RESISTIVE_TOUCH == 1)
    #define UEZ_ENABLE_TOUCHSCREEN_CALIBRATION 1
  #else
    #define UEZ_ENABLE_TOUCHSCREEN_CALIBRATION 0
  #endif
#endif

#ifndef UEZ_NUM_HANDLES
#define UEZ_NUM_HANDLES           150 // more than the average number of handles
#endif

#ifndef PROCESSOR_OSCILLATOR_FREQUENCY
#define PROCESSOR_OSCILLATOR_FREQUENCY      72000000   // clocks per second (when using LCD, must be slower)
#endif

#if (PROCESSOR_OSCILLATOR_FREQUENCY == 72000000)

#ifndef SDRAM_CLOCK_FREQUENCY
    #define SDRAM_CLOCK_FREQUENCY               (PROCESSOR_OSCILLATOR_FREQUENCY/1)
#endif

#ifndef EMC_CLOCK_FREQUENCY
    #define EMC_CLOCK_FREQUENCY                 (PROCESSOR_OSCILLATOR_FREQUENCY/1)
#endif

#else // 120000000 MHz

#ifndef SDRAM_CLOCK_FREQUENCY
    #define SDRAM_CLOCK_FREQUENCY               (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif

#ifndef EMC_CLOCK_FREQUENCY
    #define EMC_CLOCK_FREQUENCY                 (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif

#endif

#ifndef UEZBSP_SDRAM
    #define UEZBSP_SDRAM                        1
    #define UEZBSP_SDRAM_SIZE                   (8*1024*1024)
    #define UEZBSP_SDRAM_BASE_ADDR              0xA0000000
#endif

/*-------------------------------------------------------------------------*
 * Platform Volume Settings:
 *-------------------------------------------------------------------------*/
// AMP constants
#define AUDIO_AMP_NONE                        0   // Unique number per AMP
#define AUDIO_AMP_TDA8551                     1
#define AUDIO_AMP_WOLFSON                     2
#define AUDIO_AMP_LM48110                     3
// Max output limit changes based on AMP
#define UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_TDA8551 212 // Limit audio to 0.5W output
#define UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_LM48110 140 // Limit audio to 0.5W output for SMS1515-08H04 LF Speaker (8 OHM,87db,0.5W)

#ifndef UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL
    #define UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL  UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_LM48110 // Limit audio to 0.5W output
#endif

// Set default audio levels for this platform
#ifndef UEZ_DEFAULT_AUDIO_LEVEL
    #define UEZ_DEFAULT_AUDIO_LEVEL  192 // default master volume level in range of 0-255. Will be scaled to allowed AMP volume.
#endif

// Define these for your own speakers or headphones.
#ifndef UEZ_DEFAULT_OFFBOARD_SPEAKER_AUDIO_LEVEL
    #define UEZ_DEFAULT_OFFBOARD_SPEAKER_AUDIO_LEVEL  255
#endif
#ifndef UEZ_DEFAULT_ONBOARD_HEADPHONES_AUDIO_LEVEL // Onboard headphones use the built-in headphone jack.
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
    #define UEZBSP_EXTERNAL_FLASH_BASE_ADDRESS  0x28000000 // QSPI Address
#endif

#if (COMPILER_TYPE==IAR)
    #define NOP() __no_operation()
#endif

// default these to 0 for IAR warnings
#ifndef CONFIG_LOW_LEVEL_TEST_CODE
    #define CONFIG_LOW_LEVEL_TEST_CODE       0
#endif
#ifndef LIGHT_SENSOR_ENABLED
    #define LIGHT_SENSOR_ENABLED       0
#endif
#ifndef SEC_TO_ENABLED
    #define SEC_TO_ENABLED       0
#endif
#ifndef SEC_MC_ENABLED
    #define SEC_MC_ENABLED       0
#endif
#ifndef UEZ_ENABLE_VIRTUAL_COM_PORT
    #define UEZ_ENABLE_VIRTUAL_COM_PORT       0
#endif
#ifndef DO_NOT_INCLUDE_LPC17XX40XX_CODE_READ_PROTECTION_1
    #define DO_NOT_INCLUDE_LPC17XX40XX_CODE_READ_PROTECTION_1       0
#endif
#ifndef INCLUDE_EMWIN
    #define INCLUDE_EMWIN       0
#endif
#ifndef UEZGUI_EXP_BRK_OUT
    #define UEZGUI_EXP_BRK_OUT       0
#endif
#ifndef CONSOLE_USE_RTT0
    #define CONSOLE_USE_RTT0       0
#endif
#ifndef DISABLE_BACKLIGHT_PWM_UEZ_API
    #define DISABLE_BACKLIGHT_PWM_UEZ_API       0
#endif
#ifndef UEZ_ENABLE_AUDIO_CODEC
    #define UEZ_ENABLE_AUDIO_CODEC       0
#endif
#ifndef UEZ_ENABLE_LOOPBACK_TEST
    #define UEZ_ENABLE_LOOPBACK_TEST       0
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
#define PIN_HW_RESET                 GPIO_NONE 

// LED pin(s)
#define GPIO_HEARTBEAT_LED                       GPIO_P1_13

#define GPIO_PSU_3VERR              GPIO_P2_25

// GPIO Loopback Test array for this uEZGUI
#define LOOPBACK_TEST_NUM_PINS_A       (16)
#define LOOPBACK_TEST_NUM_PINS_B       (2)
#define LOOPBACK_TEST_NUM_PINS_C       (3)
#define LOOPBACK_TEST_NUM_PINS_D       (1)

// TODO add GPIOs on ALT PWR COM, PMOD here 
 
/*-------------------------------------------------------------------------*
 * Expansion Connector Header Pin Mappings to CPU:
 *-------------------------------------------------------------------------*/
// Primary Expansion Header mapping from connector (J2) to CPU
#define GPIO_PRIMARY_EXPANSION_PIN_1             GPIO_NONE // GND
#define GPIO_PRIMARY_EXPANSION_PIN_2             GPIO_P0_11
#define GPIO_PRIMARY_EXPANSION_PIN_3             GPIO_P0_10
#define GPIO_PRIMARY_EXPANSION_PIN_4             GPIO_P0_20
#define GPIO_PRIMARY_EXPANSION_PIN_5             GPIO_P0_19
#define GPIO_PRIMARY_EXPANSION_PIN_6             GPIO_P0_22
#define GPIO_PRIMARY_EXPANSION_PIN_7             GPIO_P0_17
#define GPIO_PRIMARY_EXPANSION_PIN_8             GPIO_P0_16
#define GPIO_PRIMARY_EXPANSION_PIN_9             GPIO_P0_15
#define GPIO_PRIMARY_EXPANSION_PIN_10            GPIO_NONE // GND
#define GPIO_PRIMARY_EXPANSION_PIN_11            GPIO_P0_30
#define GPIO_PRIMARY_EXPANSION_PIN_12            GPIO_P0_29
#define GPIO_PRIMARY_EXPANSION_PIN_13            GPIO_P4_26 // USB1H_PWRD
#define GPIO_PRIMARY_EXPANSION_PIN_14            GPIO_P4_27 // USB1H_OVC
#define GPIO_PRIMARY_EXPANSION_PIN_15            GPIO_P1_19 // USB1H_PPWR
#define GPIO_PRIMARY_EXPANSION_PIN_16            GPIO_P0_9
#define GPIO_PRIMARY_EXPANSION_PIN_17            GPIO_P0_8
#define GPIO_PRIMARY_EXPANSION_PIN_18            GPIO_P0_7
#define GPIO_PRIMARY_EXPANSION_PIN_19            GPIO_P0_6
#define GPIO_PRIMARY_EXPANSION_PIN_20            GPIO_P0_5
#define GPIO_PRIMARY_EXPANSION_PIN_21            GPIO_P0_4
#define GPIO_PRIMARY_EXPANSION_PIN_22            GPIO_NONE // GND
#define GPIO_PRIMARY_EXPANSION_PIN_23            GPIO_NONE // RESET_INn
#define GPIO_PRIMARY_EXPANSION_PIN_24            GPIO_NONE // RESET_OUTn
#define GPIO_PRIMARY_EXPANSION_PIN_25            GPIO_P0_26
#define GPIO_PRIMARY_EXPANSION_PIN_26            GPIO_P1_31
#define GPIO_PRIMARY_EXPANSION_PIN_27            GPIO_P1_17
#define GPIO_PRIMARY_EXPANSION_PIN_28            GPIO_P1_16
#define GPIO_PRIMARY_EXPANSION_PIN_29            GPIO_NONE // GND
#define GPIO_PRIMARY_EXPANSION_PIN_30            GPIO_P1_15
#define GPIO_PRIMARY_EXPANSION_PIN_31            GPIO_P1_14
#define GPIO_PRIMARY_EXPANSION_PIN_32            GPIO_NONE // 3V3
#define GPIO_PRIMARY_EXPANSION_PIN_33            GPIO_P1_10
#define GPIO_PRIMARY_EXPANSION_PIN_34            GPIO_P1_9
#define GPIO_PRIMARY_EXPANSION_PIN_35            GPIO_P1_8
#define GPIO_PRIMARY_EXPANSION_PIN_36            GPIO_P1_4
#define GPIO_PRIMARY_EXPANSION_PIN_37            GPIO_P1_1
#define GPIO_PRIMARY_EXPANSION_PIN_38            GPIO_P1_0
#define GPIO_PRIMARY_EXPANSION_PIN_39            GPIO_NONE // GND
#define GPIO_PRIMARY_EXPANSION_PIN_40            GPIO_P2_10
#define GPIO_PRIMARY_EXPANSION_PIN_41            GPIO_P0_3
#define GPIO_PRIMARY_EXPANSION_PIN_42            GPIO_P0_2
#define GPIO_PRIMARY_EXPANSION_PIN_43            GPIO_P0_11 // USBD_DP
#define GPIO_PRIMARY_EXPANSION_PIN_44            GPIO_P0_11 // USBD_DM
#define GPIO_PRIMARY_EXPANSION_PIN_45            GPIO_P0_11 // USBD_VBUS
#define GPIO_PRIMARY_EXPANSION_PIN_46            GPIO_NONE // 5V0
#define GPIO_PRIMARY_EXPANSION_PIN_47            GPIO_NONE // 5V0
#define GPIO_PRIMARY_EXPANSION_PIN_48            GPIO_NONE // 5V0
#define GPIO_PRIMARY_EXPANSION_PIN_49            GPIO_NONE // 3V3
#define GPIO_PRIMARY_EXPANSION_PIN_50            GPIO_NONE // 3V3

// Secondary Expansion Connector (J1) to CPU
#define GPIO_SECONDARY_EXPANSION_PIN_1           GPIO_NONE // GND
#define GPIO_SECONDARY_EXPANSION_PIN_2           GPIO_P5_4
#define GPIO_SECONDARY_EXPANSION_PIN_3           GPIO_P5_3
#define GPIO_SECONDARY_EXPANSION_PIN_4           GPIO_P5_2
#define GPIO_SECONDARY_EXPANSION_PIN_5           GPIO_P1_12
#define GPIO_SECONDARY_EXPANSION_PIN_6           GPIO_P1_11
#define GPIO_SECONDARY_EXPANSION_PIN_7           GPIO_P1_7
#define GPIO_SECONDARY_EXPANSION_PIN_8           GPIO_P1_6
#define GPIO_SECONDARY_EXPANSION_PIN_9           GPIO_P1_5
#define GPIO_SECONDARY_EXPANSION_PIN_10          GPIO_P1_3
#define GPIO_SECONDARY_EXPANSION_PIN_11          GPIO_P0_1
#define GPIO_SECONDARY_EXPANSION_PIN_12          GPIO_P0_0
#define GPIO_SECONDARY_EXPANSION_PIN_13          GPIO_NONE // 5V0
#define GPIO_SECONDARY_EXPANSION_PIN_14          GPIO_NONE // GND
#define GPIO_SECONDARY_EXPANSION_PIN_15          GPIO_P0_13
#define GPIO_SECONDARY_EXPANSION_PIN_16          GPIO_P0_12
#define GPIO_SECONDARY_EXPANSION_PIN_17          GPIO_P0_25
#define GPIO_SECONDARY_EXPANSION_PIN_18          GPIO_P0_24
#define GPIO_SECONDARY_EXPANSION_PIN_19          GPIO_P0_23
#define GPIO_SECONDARY_EXPANSION_PIN_20          GPIO_NONE // GND

/*-------------------------------------------------------------------------*
 * Expansion Connector Devices:
 *-------------------------------------------------------------------------*/
// Device names:
#define PRIMARY_EXPANSION_UART_A            "UART1"     // Pins 6-9
#define PRIMARY_EXPANSION_UART_B            "UART0"     // Pins 41-42
#define PRIMARY_EXPANSION_I2C_A             "I2C2"      // Pins 2-3
#define PRIMARY_EXPANSION_I2C_B             "I2C1"      // Pins 4-5
#define PRIMARY_EXPANSION_EMAC              "EMAC"      // Pins 25-27, 30, 31, 33-37
#define PRIMARY_EXPANSION_I2S               "I2S"       // Pins 16-21
#define PRIMARY_EXPANSION_USB_DEVICE        "USBDevice" // Pins 11-12, 43-45
#define PRIMARY_EXPANSION_USB_HOST          "USBHost"   // Pins 11-15
#define PRIMARY_EXPANSION_DAC               "DAC0"      // Pins 25
#define PRIMARY_EXPANSION_SPI               "SSP1"      // Pins 16-19
#define SECONDARY_EXPANSION_UART_C          "UART4"     // Pins 2-3
#define SECONDARY_EXPANSION_UART_D          "UART3"     // Pins 11-12
#define SECONDARY_EXPANSION_MCI             "MCI"       // Pins 5-10

// Mapping the require routines
#define UEZPlatform_ExpansionPrimary_I2C_A_Require              UEZPlatform_I2C2_Require
#define UEZPlatform_ExpansionPrimary_I2C_B_Require              UEZPlatform_I2C1_Require
#define UEZPlatform_ExpansionPrimary_EMAC_Require               UEZPlatform_EMAC_Require
#define UEZPlatform_ExpansionPrimary_UART_A_Require             UEZPlatform_UART1_Require
#define UEZPlatform_ExpansionPrimary_SPI_A_Require              UEZPlatform_SSP1_Require
#define UEZPlatform_ExpansionPrimary_UART_B_Require(w, r)       UEZPlatform_FullDuplex_UART0_Require(w, r)
#define UEZPlatform_ExpansionPrimary_USBHost_Require            UEZPlatform_USBHost_PortA_Require
#define UEZPlatform_ExpansionPrimary_USBDevice_Require          UEZPlatform_USBDevice_Require

#define UEZPlatform_ExpansionSecondary_SDCard_MCI_Require       UEZPlatform_SDCard_MCI_Drive_Require


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
void UEZPlatform_AudioCodec_Require(void);
void UEZPlatform_AudioAmp_Require(void);
void UEZPlatform_AudioMixer_Require(void);
void UEZPlatform_Audio_Require(void);
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
void UEZPlatform_I2S_Require(void);
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
void UEZPlatform_LCD_Require(void);
void UEZPlatform_MS0_Require(void);
void UEZPlatform_MS1_Require(void);
void UEZPlatform_PWM0_Require(void);
void UEZPlatform_PWM1_Require(void);
void UEZPlatform_RTC_Require(void);
void UEZPlatform_SDCard_Drive_Require(TUInt8 aDriveNum);
void UEZPlatform_SDCard_MCI_Drive_Require(TUInt8 aDriveNum);
void UEZPlatform_Speaker_Require(void);
void UEZPlatform_Temp0_Require(void);
void UEZPlatform_Timer0_Require(void);
void UEZPlatform_Timer1_Require(void);
void UEZPlatform_Timer2_Require(void);
void UEZPlatform_Timer3_Require(void);
void UEZPlatform_Touchscreen_Require(void);
void UEZPlatform_FullDuplex_UART0_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_FullDuplex_UART1_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_FullDuplex_UART2_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_FullDuplex_UART3_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize);
void UEZPlatform_UART1_Require(void);
void UEZPlatform_USBDevice_Require(void);
void UEZPlatform_USBFlash_Drive_Require(TUInt8 aDriveNum);
void UEZPlatform_USBHost_Require(void);
void UEZPlatform_USBHost_PortA_Require(void);
void UEZPlatform_USBHost_PortB_Require(void);
void UEZPlatform_Watchdog_Require(void);
void UEZPlatform_WiredNetwork0_Require(void);
void UEZPlatform_WirelessNetwork0_Require(void);
TBool UEZPlatform_Host_Port_B_Detect(void);
void UEZPlatform_ButtonBoard_Require(void);
void UEZPlatform_WiFiProgramMode(TBool runMode);

void UEZPlatform_Require(void);
void UEZPlatform_Standard_Require(void);
void UEZPlatform_Full_Require(void);
void UEZPlatform_Minimal_Require(void);
void UEZPlatform_System_Reset(void);

// Utility function to connect to the Wired Network
T_uezError UEZPlatform_WiredNetwork0_Connect(
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
