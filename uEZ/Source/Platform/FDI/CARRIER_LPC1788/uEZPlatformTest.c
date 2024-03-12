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
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_USBDeviceController.h>
#include <Source/Devices/Accelerometer/Freescale/MMA7455/Freescale_MMA7455.h>
#include <Source/Devices/Accelerometer/ST/LIS3DH/ST_LIS3DH_I2C.h>
#include <Source/Devices/ADC/Generic/Generic_ADC.h>
#include <Source/Devices/Backlight/Generic/BacklightPWMControlled/BacklightPWM.h>
#include <Source/Devices/Button/NXP/PCA9551/Button_PCA9551.h>
#include <Source/Devices/CRC/Generic/CRC_Generic.h>
#include <Source/Devices/DAC/Generic/DAC_Generic.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM_Generic_I2C.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM16_Generic_I2C.h>
#include <Source/Devices/EEPROM/NXP/LPC17xx_40xx/EEPROM_NXP_LPC17xx_40xx.h>
#include <Source/Devices/Flash/Spansion/S29GL/Flash_S29GL064N90_16bit.h>
#include <Source/Devices/Flash/NXP/LPC17xx_40xx/LPC17xx_40xx_IAP.h>
#include <Source/Devices/GPDMA/Generic/Generic_GPDMA.h>
#include <Source/Devices/GPIO/I2C/PCF8574T/GPIO_PCF8574T.h>
#include <Source/Devices/HID/Generic/HID_Generic.h>
#include <Source/Devices/I2C/Generic/Generic_I2C.h>
#include <Source/Devices/LED/NXP/PCA9551/LED_NXP_PCA9551.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_SPI.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_MCI.h>
#include <Source/Devices/MassStorage/USB_MS/USB_MS.h>
#include <Source/Devices/PWM/Generic/Generic_PWM.h>
#include <Source/Devices/RTC/Generic/Generic_RTC.h>
#include <Source/Devices/RTC/NXP/PCF8563/RTC_PCF8563.h>
#include <Source/Devices/Serial/Generic/Generic_Serial.h>
#include <Source/Devices/Serial/RS485/Generic/Generic_RS485.h>
#include <Source/Devices/SPI/Generic/Generic_SPI.h>
#include <Source/Devices/Temperature/NXP/LM75A/Temperature_LM75A.h>
#include <Source/Devices/Timer/Generic/Timer_Generic.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>
#include <Source/Devices/Touchscreen/Generic/FourWireTouchResist/FourWireTouchResist_TS.h>
#include <Source/Devices/Touchscreen/Newhaven/FT5306DE4/FT5306DE4TouchScreen.h>
#include <Source/Devices/USBDevice/NXP/LPC17xx_40xx/LPC17xx_40xx_USBDevice.h>
#include <Source/Processor/NXP/LPC17xx_40xx/uEZProcessor_LPC17xx_40xx.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_ADCBank.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_CRC.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_DAC.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_EMAC.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_EMC_Static.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_GPDMA.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_GPIO.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_I2C.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_I2S.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_LCDController.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_PWM.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_PLL.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_SDRAM.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_Serial.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_SSP.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_Timer.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_USBDeviceController.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_USBHost.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_Watchdog.h>
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_MCI.h>
#include <uEZAudioAmp.h>
#include <uEZBSP.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>
#include <uEZFile.h>
#include <uEZI2C.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZStream.h>
#include <uEZPlatform.h>
#include <uEZAudioMixer.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
// GPIO Loopback Test array for this uEZGUI
// Make sure that the first pin array is the largest array.

// Note: Dont add pins here that have diodes, LEDs, pull resistors, capacitors or transistors on the lines as even if
// they can be looped back the timing will be different and the test may depend on a GPIO state outside of the array.
// Also any pin combination with an extra long length cable (over 10") should use a different test with a longer timeout.
// Any signal with long unterminated branches will also require a longer time for pins to settle than typical.

/* Test pins. See P:\uEZGUI\uEZGUI-4088-43WQN\FCT\V2.07\DOC\uEZGUI-4088-43WQN FCT Loopback Test Debugging Reference.xlsx */
const T_uezGPIOPortPin g_loopback_pins_A[LOOPBACK_TEST_NUM_PINS_A] =
{
    /* J2 Pins */
	GPIO_P4_20,	// Pin 7	(0)
	GPIO_P4_23,	// Pin 8	(1)
	GPIO_P4_22,	// Pin 9	(2)
	GPIO_P4_26,	// Pin 13	(3)
	GPIO_P4_27,	// Pin 14	(4)
	GPIO_P1_19,	// Pin 15	(5)
	GPIO_P0_8,	// Pin 17	(6)
	GPIO_P0_7,	// Pin 18	(7)
	GPIO_P0_5,	// Pin 20	(8)
	GPIO_P0_4,	// Pin 21	(9)
	GPIO_P1_31,	// Pin 26	(10)
	GPIO_P1_17,	// Pin 27	(11)
	GPIO_P1_16,	// Pin 28	(12)
	GPIO_P1_10,	// Pin 33	(13)
	GPIO_P1_9,	// Pin 34	(14)
	GPIO_P1_8,	// Pin 35	(15)
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

#define IOCON_FUNC(n)               ((n)&7) // FUNC: Pin function
#define IOCON_NO_PULL               (0<<3)  // MODE: No Pull-down/pull-up resistors
#define IOCON_PULL_DOWN             (1<<3)  // MODE: Pull-down resistor enabled
#define IOCON_PULL_UP               (2<<3)  // MODE: Pull-up resistor enabled
#define IOCON_REPEATER              (3<<3)  // MODE: Repeater mode
#define IOCON_HYS_DISABLE           (0<<5)  // HYS: Hysteresis disabled
#define IOCON_HYS_ENABLE            (1<<5)  // HYS: Hysteresis enabled
#define IOCON_INVERT_OFF            (0<<6)  // INV: Input is NOT inverted
#define IOCON_INVERT_ON             (1<<6)  // INV: Input is inverted
#define IOCON_ANALOG                (0<<7)  // ADMODE: Analog mode
#define IOCON_DIGITAL               (1<<7)  // ADMODE: Digital mode
#define IOCON_FILTER_ON             (0<<8)  // FILTER: For I2C: I2C 50 ns glitch filter and slew rate control enabled
                                            // For non-I2C: Noise pulses below approx 10 ns are filtered out
#define IOCON_FILTER_OFF            (1<<8)  // FILTER: For I2C: I2C 50 ns glitch filter and slew rate control disabled
                                            // For non-I2C: No input filtering is done
#define IOCON_SLEW_STANDARD         (0<<9)  // SLEW: For non-I2C: Standard mode, output slew rate control is enabled
#define IOCON_SLEW_FAST             (1<<9)  // SLEW: For non-I2C: Fast mode, slew rate control is disabled.
#define IOCON_SINK_4MA              (0<<9)  // HIDRIVE: For I2C: Output drive sink is 4 mA.  This is sufficient for standard and fast mode I2C
#define IOCON_SINK_20MA             (1<<9)  // HIDRIVE: For I2C: Output drive sink is 20 mA.  This is needed for Fast Mode Plus I2C
#define IOCON_PUSH_PULL             (0<<10) // OD: Normal push-pull output
#define IOCON_OPEN_DRAIN            (1<<10) // OD: Simulated open-drain output (high drive disabled)
#define IOCON_DAC_DISABLE           (0<<16) // DACEN: DAC is disabled
#define IOCON_DAC_ENABLE            (1<<16) // DACEN: DAC is enabled

#define BAUD_DIVIDER(PCLK, BAUD)         ((PCLK/1)/((BAUD)*16))

// Init UART without changinge ANY clock source or PCLK from power on default
void UEZPlatform_INIT_LOW_LEVEL_UART_DEFAULT_CLOCK(void){
    LPC_IOCON->P0_2 = IOCON_FUNC(1)|IOCON_PULL_UP|IOCON_HYS_ENABLE|IOCON_INVERT_OFF|IOCON_SLEW_STANDARD|IOCON_PUSH_PULL;
    LPC_IOCON->P0_3 = IOCON_FUNC(1)|IOCON_PULL_UP|IOCON_HYS_ENABLE|IOCON_INVERT_OFF|IOCON_SLEW_STANDARD|IOCON_PUSH_PULL;
    
    TUInt32 temp = LPC_SC->PCONP; // Dont' use LPC17xx_40xxPowerOn(1UL << 3); as not initialized yet
    temp |= (1UL << 3);  // defualt index 3 is already on
    LPC_SC->PCONP = temp;

    // Set the FIFO enable bit in the FCR register. This bit must be set for
    // proper UART operation.
    LPC_UART0->FCR = 7; // FCRFE|RFR|TFR

    // Internal RC oscillator is 12MHz, default PCLK is divide by 4, default SYSCLK is divided  by 1
    // Set baudrate (3000000)/(16*(256*0+1)*(1+(5/8))) = 115384.61538461538461538462
    
    TUInt32 divider = BAUD_DIVIDER(3000000, 115200);
    TUInt8 divAddVal = 5;
    TUInt8 mulVal = 8;

    LPC_UART0->LCR |= 0x80;
    LPC_UART0->DLL = divider & 0x00ff;
    LPC_UART0->DLM = (divider >> 8) & 0x00ff;
    LPC_UART0->FDR = ((mulVal<<4) & 0x00f0) | (divAddVal & 0x000f); // fractional dividers
    LPC_UART0->LCR &= 0x00;

    LPC_UART0->LCR |= ((0 << 3) | (0 << 4)); // SERIAL_PARITY_NONE:
    LPC_UART0->LCR |= (0 << 2); // SERIAL_STOP_BITS_1:
    LPC_UART0->LCR |= (3 << 0); // 8 bits
    // No flow control on this UART - SERIAL_FLOW_CONTROL_NONE
    
    //LPC_UART0->THR = 'H'; // Test single character
}

void UEZPlatform_INIT_LOW_LEVEL_UART(void){
    LPC_IOCON->P0_2 = IOCON_FUNC(1)|IOCON_PULL_UP|IOCON_HYS_ENABLE|IOCON_INVERT_OFF|IOCON_SLEW_STANDARD|IOCON_PUSH_PULL;
    LPC_IOCON->P0_3 = IOCON_FUNC(1)|IOCON_PULL_UP|IOCON_HYS_ENABLE|IOCON_INVERT_OFF|IOCON_SLEW_STANDARD|IOCON_PUSH_PULL;
    
    TUInt32 temp = LPC_SC->PCONP; // Dont' use LPC17xx_40xxPowerOn(1UL << 3); as not initialized yet
    temp |= (1UL << 3);  // defualt index 3 is already on
    LPC_SC->PCONP = temp;

    // Set the FIFO enable bit in the FCR register. This bit must be set for
    // proper UART operation.
    LPC_UART0->FCR = 7; // FCRFE|RFR|TFR

    // Set baudrate, at this stage we have proper PCLK
    TUInt32 divider = BAUD_DIVIDER(PCLK_FREQUENCY, 115200);
    TUInt8 divAddVal = 0;
    TUInt8 mulVal = 1;

    LPC_UART0->LCR |= 0x80;
    LPC_UART0->DLL = divider & 0x00ff;
    LPC_UART0->DLM = (divider >> 8) & 0x00ff;
    LPC_UART0->FDR = ((mulVal<<4) & 0x00f0) | (divAddVal & 0x000f); // fractional dividers
    LPC_UART0->LCR &= 0x00;

    LPC_UART0->LCR |= ((0 << 3) | (0 << 4)); // SERIAL_PARITY_NONE:
    LPC_UART0->LCR |= (0 << 2); // SERIAL_STOP_BITS_1:
    LPC_UART0->LCR |= (3 << 0); // 8 bits
    // No SERIAL_FLOW_CONTROL_NONE

    //Enable UART0 interrupts // will crash if not defined, can't run registered interruptsd here yet due to uEZ code included.
    //LPC_UART0->IER = 0x03; // Interrupts and TX and RX

    //LPC_UART0->THR = 'H'; // Test single character
}

// Note that BSPDelayUS is not the same before we init the crystal oscillators so this isn't consistent....
void COM_Send(char * bytes, uint16_t numBytes) {
#if (CONSOLE_USE_RTT0 == 1)
    SEGGER_RTT_Write(0, c, numBytes);
#else
    char c;
    uint16_t bytesToSend = numBytes;

    //G_UART_SendingData = 1;
    // Place the MCU specific send function here. Prefer ISR.
    while(bytesToSend > 0) {
      c=*bytes++;
      LPC_UART0->THR = c;
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