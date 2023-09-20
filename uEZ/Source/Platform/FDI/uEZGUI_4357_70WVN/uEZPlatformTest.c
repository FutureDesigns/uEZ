/*-------------------------------------------------------------------------*
 * File:  uEZPlatformTest.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Test related platform functions
 *-------------------------------------------------------------------------*/
 
/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

#include <Config.h>
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <Device/LCD.h>
#include <HAL/HAL.h>
#include <HAL/GPIO.h>
#include <HAL/EMAC.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_USBDeviceController.h>
#include <Source/Devices/Accelerometer/Freescale/MMA7455/Freescale_MMA7455.h>
#include <Source/Devices/Accelerometer/ST/LIS3DH/ST_LIS3DH_I2C.h>
#include <Source/Devices/ADC/Generic/Generic_ADC.h>
#include <Source/Devices/CRC/Generic/CRC_Generic.h>
#include <Source/Devices/Backlight/Generic/BacklightPWMControlled/BacklightPWM.h>
#include <Source/Devices/Button/NXP/PCA9551/Button_PCA9551.h>
#include <Source/Devices/CRC/Software/CRC_Software.h>
#include <Source/Devices/DAC/Generic/DAC_Generic.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM_Generic_I2C.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM16_Generic_I2C.h>
#include <Source/Devices/EEPROM/NXP/LPC43xx/EEPROM_NXP_LPC43xx.h>
#include <Source/Devices/Flash/NXP/LPC43xx/LPC43xx_IAP.h>
#include <Source/Devices/Flash/NXP/LPC_SPIFI_M4F/Flash_NXP_LPC_SPIFI_M4.h>
#include <Source/Devices/GPDMA/Generic/Generic_GPDMA.h>
#include <Source/Devices/HID/Generic/HID_Generic.h>
#include <Source/Devices/I2C/Generic/Generic_I2C.h>
#include <Source/Devices/LED/NXP/PCA9551/LED_NXP_PCA9551.h>
#include <Source/Devices/MassStorage/LPCUSBLib/LPCUSBLib_MS.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_SD_MMC.h>
#include <Source/Devices/Stream/LPCUSBLib/SerialHost/Stream_LPCUSBLib_SerialHost.h>
#include <Source/Devices/PWM/Generic/Generic_PWM.h>
#include <Source/Devices/RTC/Generic/Generic_RTC.h>
#include <Source/Devices/RTC/NXP/PCF8563/RTC_PCF8563.h>
#include <Source/Devices/Serial/Generic/Generic_Serial.h>
#include <Source/Devices/Stream/SWO/Stream_SWO_CortexM.h>
#include <Source/Devices/Serial/LPCUSBLib/SerialHost_FTDI/Stream_LPCUSBLib_SerialHost_FTDI.h>
#include <Source/Devices/SPI/Generic/Generic_SPI.h>
#include <Source/Devices/Temperature/NXP/LM75A/Temperature_LM75A.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>
#include <Source/Devices/Touchscreen/Newhaven/FT5306DE4/FT5306DE4TouchScreen.h>
#include <Source/Processor/NXP/LPC43xx/uEZProcessor_LPC43xx.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_ADCBank.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_DAC.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_EMAC.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_EMC_Static.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_GPDMA.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_GPIO.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_I2C.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_I2S.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_LCDController.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_SD_MMC.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_PWM.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_PLL.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_SDRAM.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Serial.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_SSP.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Timer.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_USBDeviceController.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_USBHost.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Watchdog.h>
#include <uEZAudioAmp.h>
#include <uEZBSP.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>
#include <uEZFile.h>
#include <uEZI2C.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZStream.h>
#include <uEZAudioMixer.h>
#include <Source/Devices/GPIO/I2C/PCA9532/GPIO_PCA9532.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
// GPIO Loopback Test array for this uEZGUI
// Make sure that the first pin array is the largest array.
// TODO Must change for this HW!

// Note: Dont add pins here that have diodes, LEDs, pull resistors, capacitors or transistors on the lines as even if
// they can be looped back the timing will be different and the test may depend on a GPIO state outside of the array.
// Also any pin combination with an extra long length cable (over 10") should use a different test with a longer timeout.
// Any signal with long unterminated branches will also require a longer time for pins to settle than typical.

/* Test pins. See P:\uEZGUI\uEZGUI-4357-50WVN\FCT\...\DOC\uEZGUI-4357-50WVN FCT Loopback Test Debugging Reference.xlsx */
const T_uezGPIOPortPin g_loopback_pins_A[LOOPBACK_TEST_NUM_PINS_A] =
{
   GPIO_P5_3,
   GPIO_P4_12,
   GPIO_P3_8,
   GPIO_P6_2,
   GPIO_P6_1,
   GPIO_P6_13,
   GPIO_P6_12,
   GPIO_P5_5,
   GPIO_P4_11,
   GPIO_P5_18,
   GPIO_P4_14,
   GPIO_P5_9,
   GPIO_P2_4,
   GPIO_P2_1,
   GPIO_P0_12,
   GPIO_P6_0,
   GPIO_P0_0,
   GPIO_P0_2,
   GPIO_P0_3,
   GPIO_P0_1,
   GPIO_P0_15,
   GPIO_P0_13,
   GPIO_P6_30,
   GPIO_P4_10,
   GPIO_P6_28,
   GPIO_P6_27,
   GPIO_P6_26,
   GPIO_P6_25,
   GPIO_P6_24,
   GPIO_P5_17,
   GPIO_P4_15,
   GPIO_P4_9,
   GPIO_P4_8,
   GPIO_P3_13,
   GPIO_P3_12,
   GPIO_P7_22
};

/* Test pin connectivity (1 = Pins are connected)*/
const uint8_t g_loopback_connected_A[LOOPBACK_TEST_NUM_PINS_A][LOOPBACK_TEST_NUM_PINS_A] =
{
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
};

const T_uezGPIOPortPin g_loopback_pins_B[LOOPBACK_TEST_NUM_PINS_B] =
{
    /* J11 Pins */
    GPIO_P7_16,     // Pin 1    (0)
    GPIO_P7_18,     // Pin 2    (1)
    GPIO_P7_17,     // Pin 3    (2)
    // GPIO_NONE,   // Pin 4            // Not a GPIO
    GPIO_P5_12,     // Pin 7    (3)
    // GPIO_NONE,   // Pin 8            // Not a GPIO, controlled by output of reset circuit, connects to other peripherals
    // GPIO_NONE,   // Pin 9            // Connected to DNL resistors (Not connected to MCU)
    // GPIO_NONE,   // Pin 10           // Connected to DNL resistors (Not connected to MCU)
};

/* Test pin connectivity (1 = Pins are connected)*/
const uint8_t g_loopback_connected_B[LOOPBACK_TEST_NUM_PINS_B][LOOPBACK_TEST_NUM_PINS_B] =
{
    /* J19 Pins */
    {1, 1, 0, /*0,*/ 0, /*0, 0, 0*/},
    {1, 1, 0, /*0,*/ 0, /*0, 0, 0*/},
    {0, 0, 1, /*1,*/ 1, /*0, 0, 0*/},
  /*{0, 0, 1,   1,   1,   0, 0, 0  },*/
    {0, 0, 1, /*1,*/ 1, /*0, 0, 0*/},
  /*{0, 0, 0,   0,   0,   1, 0, 0  },*/
  /*{0, 0, 0,   0,   0,   0, 1, 0  },*/
  /*{0, 0, 0,   0,   0,   0, 0, 1  },*/
};

const T_uezGPIOPortPin g_loopback_pins_C[LOOPBACK_TEST_NUM_PINS_C] =
{   
    // Pins 
    GPIO_NONE,  // Pin X    (0)
};

/* Test pin connectivity (1 = Pins are connected)*/
const uint8_t g_loopback_connected_C[LOOPBACK_TEST_NUM_PINS_C][LOOPBACK_TEST_NUM_PINS_C] =
{
    // Pins
    {0},
};

// TODO We should be able to loopback some pins on J6 connector now.

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*
 * Memory placement section:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/

// These GPIOs are only for Rev 2 PCBs
// By default all pull-up resistors are enabled at reset on all normal GPIOs.
 static void Prepare_GPIOs_J5(void)
 {
    //TUInt32 value; // 16 total
    T_uezGPIOPortPin currentPortPin;

    currentPortPin = GPIO_P7_19; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P7_20; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P7_22; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P3_12; // P_ func 0
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P3_13; // P_ func 0
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P4_8; // P_ func 0 // different rev 2
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P4_9; // P_ func 0 // different rev 2
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P4_15; // P_ func 0
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P5_17; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P6_24; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P6_25; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P6_26; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P6_27; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P6_28; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.

    currentPortPin = GPIO_P4_10; // P_ func 0 // different rev 2
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P6_30; // P_ func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
 }
 static void Prepare_GPIOs_J6(void)
 {
  //TUInt32 value; // 27 pins total plus extra USB, UART, I2S pins
    T_uezGPIOPortPin currentPortPin;

    // Skip USB_VBUS, DM, DP not GPIOs
    // Skip UART pins for console

    T_uezGPIOPortPin currentPorrtPinArr[] = {
        GPIO_P7_4, //0  - J6 pin 40 - P_ func 4
        GPIO_P0_13,//1  - J6 pin 38 - P_ func 0
        GPIO_P0_15,//2  - J6 pin 37 - P_ func 0
        GPIO_P0_1, //3  - J6 pin 36 - P_ func 0
        GPIO_P0_3, //4  - J6 pin 35 - P_ func 0
        GPIO_P0_2, //5  - J6 pin 34 - P_ func 0
        GPIO_P0_0, //6  - J6 pin 33 - P_ func 0
        GPIO_P6_8, //7  - J6 pin 31 - P_ func 4
        GPIO_P6_0, //8  - J6 pin 28 - P_ func 4
        GPIO_P0_12,//9  - J6 pin 27 - P_ func 0
        GPIO_P2_1, //10 - J6 pin 26 - P_ func 4
        GPIO_P2_4, //11 - J6 pin 25 - P_ func 0
        GPIO_P5_8, //12 - J6 pin 20 - P_ func 4
        GPIO_P5_9, //13 - J6 pin 19 - P_ func 4
        GPIO_P4_13,//14 - J6 pin 17 - P_ func 0
        GPIO_P4_14,//15 - J6 pin 16 - P_ func 0
        GPIO_P5_18,//16 - J6 pin 15 - P_ func 4
        GPIO_P4_11,//17 - J6 pin 14 - P_ func 0
        GPIO_P5_5, //18 - J6 pin 13 - P_ func 4
        GPIO_P6_12,//19 - J6 pin 9 -- P_ func 4
        GPIO_P6_13,//20 - J6 pin 8 -- P_ func 4
        GPIO_P6_1, //21 - J6 pin 7 -- P_ func 4
        GPIO_P6_2, //22 - J6 pin 6 -- P_ func 4
        GPIO_P3_8, //23 - J6 pin 5 -- P_ func 0
        GPIO_P4_12,//24 - J6 pin 4 -- P_ func 0
        GPIO_P5_3, //25 - J6 pin 3 -- P_ func 4
        GPIO_P5_4  //26 - J6 pin 2 -- P_ func 4
    };
    
    //P1_19 not a GPIO
    // skip reset lines
    // P6_0 not a GPIO
    // P3_0 not a GPIO
    // Skip USB1_DP USB1_DM not GPIOs

    for (int i = 0; i < 27; i++)
    {
        // Placed a breakpoint here to take initial pin reading
        currentPortPin = currentPorrtPinArr[i]; // P_ func 4
        UEZGPIOInput(currentPortPin);
        UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
        // Can use to test a pin if it got set to GPIO correctly
        UEZGPIOSet(currentPortPin); 
        UEZGPIOOutput(currentPortPin); 
        UEZGPIOClear(currentPortPin);

        // Placed a breakpoint here to take pin reading after toggling it off
        //printf("Breakpoint");

        UEZGPIOInput(currentPortPin);
    	UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
        UEZGPIOSetPull(currentPortPin, GPIO_PULL_UP);

        // Placed a breakpoint here to take reading after attempting to pull high
        //printf("Breakpoint");
    }
 }

 static void Prepare_GPIOs_J11(void) // except for reset line the other PMOD pins are same between rev 1 and rev 2
 {
    //TUInt32 value; // 4 pins total
    T_uezGPIOPortPin currentPortPin;
    // TODO put in correct numbers
    currentPortPin = GPIO_P7_16; // PF_1 func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    // cut off upper bytes for ports over port 7
    //value = ((currentPortPin >> 8) & 0x7) >= 5 ? 4 : 0; // port 5-7 set func 4, port 0-4 set func 0
    //UEZGPIOControl(currentPortPin, GPIO_CONTROL_SET_CONFIG_BITS, value); // This sets only SCU to value
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
#if 0
  UEZGPIOSet(currentPortPin); // can use to test a pin if it got set to GPIO correctly
  UEZGPIOOutput(currentPortPin); 
  UEZGPIOClear(currentPortPin);
#endif
    currentPortPin = GPIO_P7_17; // PF_2 func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    // cut off upper bytes for ports over port 7
    //value = ((currentPortPin >> 8) & 0x7) >= 5 ? 4 : 0; // port 5-7 set func 4, port 0-4 set func 0
    //UEZGPIOControl(currentPortPin, GPIO_CONTROL_SET_CONFIG_BITS, value); // This sets only SCU to value
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.

    currentPortPin = GPIO_P7_18; // PF_3 func 4
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    // cut off upper bytes for ports over port 7
    //value = ((currentPortPin >> 8) & 0x7) >= 5 ? 4 : 0; // port 5-7 set func 4, port 0-4 set func 0
    //UEZGPIOControl(currentPortPin, GPIO_CONTROL_SET_CONFIG_BITS, value); // This sets only SCU to value
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
    
    currentPortPin = GPIO_P5_12; // P4_8 funct 4 (PMOD IRQ)
    UEZGPIOInput(currentPortPin);
    UEZGPIOSetMux(currentPortPin, (currentPortPin >> 8) >= 5 ? 4 : 0); // This will set SCU to 4 or 0
    // cut off upper bytes for ports over port 7
    //value = ((currentPortPin >> 8) & 0x7) >= 5 ? 4 : 0; // port 5-7 set func 4, port 0-4 set func 0
    //UEZGPIOControl(currentPortPin, GPIO_CONTROL_SET_CONFIG_BITS, value); // This sets only SCU to value
    UEZGPIOControl(currentPortPin, GPIO_CONTROL_ENABLE_INPUT_BUFFER, 0); // This enables input buffer, which is required for input mode operation.
 }


 void UEZPlatform_Prepare_GPIOs_For_Loopback(void)
 {
   Prepare_GPIOs_J5();  // 20 pin
   Prepare_GPIOs_J6();  // 50 pin
   Prepare_GPIOs_J11(); // Pmod
 }

// Init UART without changinge ANY clock source or PCLK from power on default
void UEZPlatform_INIT_LOW_LEVEL_UART_DEFAULT_CLOCK(void){

}

void UEZPlatform_INIT_LOW_LEVEL_UART(void){

}

// Note that BSPDelayUS is not the same before we init the crystal oscillators so this isn't consistent....
void COM_Send(char * bytes, uint16_t numBytes) {
#if (CONSOLE_USE_RTT0 == 1)
    //SEGGER_RTT_Write(0, c, numBytes);
#else
    char c;
    (void) c;
    uint16_t bytesToSend = numBytes;

    //G_UART_SendingData = 1;
    // Place the MCU specific send function here. Prefer ISR.
    while(bytesToSend > 0) {
      c=*bytes++;
//      LPC_UART0->THR = c;
      bytesToSend--;
      UEZBSPDelayUS(50);
    }
    UEZBSPDelayUS(100);

    // If blocking send function used clear flag here.
    //G_UART_SendingData = 0;

    // If not blocking thread sleep or block until we receive.
    /*while(G_UART_SendingData){
      osDelay(1);//NOP();
    }*/
#endif
}

void UEZPlatform_MemTest_StepA(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
  // Currently we can't run this check before we reach main().
  /*if(UEZIsRtosRunning() == RTOS_SCHEDULER_RUNNING){
    UEZTaskDelay(1);
  }*/
}

void UEZPlatform_MemTest_StepA_Pass(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepB(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepB_Pass(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepC(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepC_Pass(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepD(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepD_Pass(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepE(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepE_Pass(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepF(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

void UEZPlatform_MemTest_StepF_Pass(void)
{
  UEZBSP_HEARTBEAT_TOGGLE();
  UEZBSPDelay1MS();
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform.c
 *-------------------------------------------------------------------------*/
