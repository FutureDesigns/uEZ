 /**
 *  @file   Platform\EA\OEM511_LPC4357\uEZPlatform.h
 *  @brief  uEZ Platform File Definitions
 *
 * The uEZ platform file definitions.
 */

#ifndef UEZPLATFORM_SETTINGS_H_
#define UEZPLATFORM_SETTINGS_H_

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Config.h> //includes Config_Build.h
#include <uEZNetwork.h>
#include <Types/GPIO.h>
#include <uEZPlatformAPI.h>
#include <Source/Library/LPCOpen/LPCUSBLib/LPCUSBLib_uEZ.h>

/*-------------------------------------------------------------------------*
 * Platform Settings:
 *-------------------------------------------------------------------------*/
#ifndef UEZGUI_4357_50WVN_REV
    #define UEZGUI_4357_50WVN_REV  2 // Rev 2 board has new LCD and 32MB SDRAM different timings.
#endif

#define UEZ_PROCESSOR                       NXP_LPC4357

#ifndef LPC43XX_ENABLE_M0_CORES
    #define LPC43XX_ENABLE_M0_CORES  0 // Must set to 1 to enable cores at bootup. If second core project is missing it will fault.
#endif

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
#define PROCESSOR_OSCILLATOR_FREQUENCY      204000000   // clocks per second 
#endif

#ifndef SDRAM_CLOCK_FREQUENCY
    #define SDRAM_CLOCK_FREQUENCY               (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif

#ifndef EMC_CLOCK_FREQUENCY
    #define EMC_CLOCK_FREQUENCY                 (PROCESSOR_OSCILLATOR_FREQUENCY/2)
#endif

#ifndef UEZBSP_SDRAM
    #define UEZBSP_SDRAM                        1

#if (UEZGUI_4357_50WVN_REV == 2)
    #define UEZBSP_SDRAM_SIZE                   (32*1024*1024)
#else
    #define UEZBSP_SDRAM_SIZE                   (16*1024*1024)
#endif

    #define UEZBSP_SDRAM_BASE_ADDR              0x28000000
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

#ifndef UEZBSP_EXTERNAL_FLASH_BASE_ADDRESS
    #define UEZBSP_EXTERNAL_FLASH_BASE_ADDRESS  0x14000000
#endif

// TODO Can these defines be removed on 4357 builds?
#define UEZBSP_NOR_FLASH                    UEZ_ENABLE_FLASH
#define UEZBSP_NOR_FLASH_BASE_ADDRESS       UEZBSP_EXTERNAL_FLASH_BASE_ADDRESS

#include <lwipopts.h>

#if (COMPILER_TYPE==IAR)
    #define NOP() __no_operation()
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
// HW Reset pin in Rev 1,2 PCB
#define PIN_HW_RESET                GPIO_P7_14

// LED pin(s)
#define GPIO_HEARTBEAT_LED          GPIO_P0_11

#define GPIO_PERIPHERAL_RESET       GPIO_P7_0

// TODO add GPIOs on ALT PWR COM, PMOD here
 
 // LCD Pins
#define LCD_GPIO_LCD_3V3_EN         UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 0)
#define LCD_GPIO_LCD_5V_EN          UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 1)
#define LCD_GPIO_LCD_2              UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 2)
#define LCD_GPIO_LCD_3              UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 3)
#define LCD_GPIO_LCD_DISP_EN        UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 4)
#define LCD_GPIO_LCD_3V3_IN         UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 5)
#define LCD_GPIO_LCD_RESET          UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 6)
#define LCD_GPIO_LCD_BL_CON2        UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 7)
#define LCD_GPIO_LCD_BL_CON         UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 8)
#define LCD_GPIO_LCD_9              UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 9)
#define LCD_GPIO_LCD_10             UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 10)
#define LCD_GPIO_LCD_11             UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 11)
#define LCD_GPIO_LCD_12             UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 12)
#define LCD_GPIO_LCD_13             UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 13)
#define LCD_GPIO_LCD_14             UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 14)
#define LCD_GPIO_LCD_15             UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 15)

// GPIO Loopback Test array for this uEZGUI
#define LOOPBACK_TEST_NUM_PINS_A              (36)
#define LOOPBACK_TEST_NUM_PINS_B              (4)
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
void UEZPlatform_ADC1_0_Require(void);
void UEZPlatform_ADC1_1_Require(void);
void UEZPlatform_ADC1_2_Require(void);
void UEZPlatform_ADC1_3_Require(void);
void UEZPlatform_ADC1_4_Require(void);
void UEZPlatform_ADC1_5_Require(void);
void UEZPlatform_ADC1_6_Require(void);
void UEZPlatform_ADC1_7_Require(void);
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
void UEZPlatform_EEPROM_LPC43xx_Require(void);
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
void UEZPlatform_MS0_Require(TUInt32 aUSBNumber);
void UEZPlatform_MS1_Require(void);
void UEZPlatform_PWM0_Require(void);
void UEZPlatform_PWM1_Require(void);
void UEZPlatform_RTC_Require(void);
void UEZPlatform_SDCard_Drive_Require(TUInt8 aDriveNum);
void UEZPlatform_IAP_Require(void);
void UEZPlatform_Timer0_Require(void);
void UEZPlatform_Timer1_Require(void);
void UEZPlatform_Timer2_Require(void);
void UEZPlatform_Timer3_Require(void);
void UEZPlatform_Speaker_Require(void);
void UEZPlatform_Temp0_Require(void);
void UEZPlatform_Touchscreen_Require(char* aName);
void UEZPlatform_USBDevice_Require(void);
void UEZPlatform_USBFlash_Drive_Require(TUInt8 aDriveNum);
void UEZPlatform_USBHost_PortA_Require(void);
void UEZPlatform_USBHost_PortB_Require(void);
void UEZPlatform_USBHost_Serial_Require(void);
void UEZPlatform_USBHost_USB0_Serial_Require(void);
void UEZPlatform_USBHost_USB1_Serial_Require(void);
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
void uEZPlatform_Start_Additonal_Cores(void); // Don't need to add to platform API if we only call from app projects
void uEZPlatform_Register_InterCore_WS(void);

TBool UEZPlatform_Host_Port_B_Detect(void);

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

void UEZPlatform_Prepare_GPIOs_For_Loopback(void);

extern const T_uezGPIOPortPin g_loopback_pins_A[];
extern const uint8_t g_loopback_connected_A[][LOOPBACK_TEST_NUM_PINS_A];

extern const T_uezGPIOPortPin g_loopback_pins_B[];
extern const uint8_t g_loopback_connected_B[][LOOPBACK_TEST_NUM_PINS_B];

extern const T_uezGPIOPortPin g_loopback_pins_C[];
extern const uint8_t g_loopback_connected_C[][LOOPBACK_TEST_NUM_PINS_C];

#endif // UEZPLATFORM_SETTINGS_H_
/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform_Settings.h
 *-------------------------------------------------------------------------*/
