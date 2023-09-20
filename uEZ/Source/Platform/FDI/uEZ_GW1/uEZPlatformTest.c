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
#include <Source/Devices/Timer/Generic/Timer_Generic.h>
#include <Source/Devices/ToneGenerator/Generic/Timer/ToneGenerator_Generic_Timer.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>
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

/* Test pins. See P:\ \uEZGUI-4088-43WQN FCT Loopback Test Debugging Reference.xlsx */
const T_uezGPIOPortPin g_loopback_pins_A[LOOPBACK_TEST_NUM_PINS_A] =
{
    /* J2 Pins */
	GPIO_NONE,	// Pin 7	(0)
};

/* Test pin connectivity (1 = Pins are connected)*/
const uint8_t g_loopback_connected_A[LOOPBACK_TEST_NUM_PINS_A][LOOPBACK_TEST_NUM_PINS_A] =
{
    /* J2 Pins */
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
};

const T_uezGPIOPortPin g_loopback_pins_B[LOOPBACK_TEST_NUM_PINS_B] =
{   
    // Pins 
    GPIO_NONE,	// Pin X	(0)
};

/* Test pin connectivity (1 = Pins are connected)*/
const uint8_t g_loopback_connected_B[LOOPBACK_TEST_NUM_PINS_B][LOOPBACK_TEST_NUM_PINS_B] =
{
    // Pins
    {0},
};

const T_uezGPIOPortPin g_loopback_pins_C[LOOPBACK_TEST_NUM_PINS_C] =
{   
    // Pins 
    GPIO_NONE,	// Pin X	(0)
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
