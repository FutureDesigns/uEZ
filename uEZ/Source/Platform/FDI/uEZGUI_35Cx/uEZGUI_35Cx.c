/*-------------------------------------------------------------------------*
 * File:  uEZPlatform.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the uEZGUI-35Cx
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2017 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

 /**
 *    @addtogroup Platform_uEZGUI_35Cx
 *  @{
 *  @brief     uEZ Platform file for the uEZGUI_35Cx
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZLicense.txt
 *
 *    uEZ Platform file for the uEZGUI-35Cx
 *
 * @par Example code:
 * // example description
 * @par
 * @code
 *
 * @endcode
*/


#include <Config.h>
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <Device/LCD.h>
#include <HAL/HAL.h>
#include <HAL/GPIO.h>
#include <HAL/EMAC.h>
#include <HAL/Interrupt.h>
#include <Source/Devices/Accelerometer/Freescale/MMA7455/Freescale_MMA7455.h>
#include <Source/Devices/Accelerometer/ST/LIS3DH/ST_LIS3DH_I2C.h>
#include <Source/Devices/ADC/Generic/Generic_ADC.h>
#include <Source/Devices/AudioAmp/NXP/TDA8551_T/AudioAmp_TDA8551T.h>
#include <Source/Devices/AudioAmp/Wolfson/WM8731/AudioAmp_WM8731.h>
#include <Source/Devices/AudioAmp/TI/LM48100/AudioAmp_LM48100.h>
#if UEZ_ENABLE_AUDIO_CODEC
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>
#endif
#include <Source/Devices/Backlight/Generic/BacklightPWMControlled/BacklightPWM.h>
#include <Source/Devices/Button/NXP/PCA9551/Button_PCA9551.h>
#include <Source/Devices/CRC/Generic/CRC_Generic.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM_Generic_I2C.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM16_Generic_I2C.h>
#include <Source/Devices/EEPROM/NXP/LPC546xx/EEPROM_NXP_LPC546xx.h>
#include <Source/Devices/Flash/NXP/LPC_SPIFI_M4F/Flash_NXP_LPC_SPIFI_M4.h>
#include <Source/Devices/GPDMA/Generic/Generic_GPDMA.h>
#include <Source/Devices/HID/Generic/HID_Generic.h>
#include <Source/Devices/I2C/Generic/Generic_I2C.h>
#include <Source/Devices/I2S/Generic/Generic_I2S.h>
#include <Source/Devices/LCD/Tianma/TM035KBH02-09/Tianma_TM035KBH02_09.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_SD_MMC.h>
#include <Source/Devices/Network/GainSpan/Network_GainSpan.h>
#include <Source/Devices/Network/lwIP/Network_lwIP.h>
#include <Source/Devices/PWM/Generic/Generic_PWM.h>
#include <Source/Devices/RTC/Generic/Generic_RTC.h>
#include <Source/Devices/RTC/NXP/PCF8563/RTC_PCF8563.h>
#include <Source/Devices/Serial/Generic/Generic_Serial.h>
#include <Source/Devices/Serial/RS485/Generic/Generic_RS485.h>
#include <Source/Devices/SPI/Generic/Generic_SPI.h>
#include <Source/Devices/Temperature/NXP/LM75A/Temperature_LM75A.h>
#include <Source/Devices/Timer/Generic/Timer_Generic.h>
#include <Source/Devices/ToneGenerator/Generic/Timer/ToneGenerator_Generic_Timer.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>
#include <Source/Devices/Touchscreen/Himax/HX8526-A/HX8526-A_Touchscreen.h>
#include <Source/Devices/Touchscreen/Generic/FourWireTouchResist/FourWireTouchResist_TS.h>
#include <Source/Devices/Watchdog/Generic/Watchdog_Generic.h>
#include <Source/Library/Web/BasicWeb/BasicWEB.h>
#include <Source/Library/FileSystem/FATFS/uEZFileSystem_FATFS.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Source/Library/Memory/MemoryTest/MemoryTest.h>
#include <Source/Library/StreamIO/StdInOut/StdInOut.h>
#include <Source/Processor/NXP/LPC546xx/uEZProcessor_LPC546xx.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_ADCBank.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_CRC.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_EMAC.h>
//#include <Source/Processor/NXP/LPC546xx/LPC546xx_EMC_Static.h>
//#include <Source/Processor/NXP/LPC546xx/LPC546xx_GPDMA.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_GPIO.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_I2C.h>
//#include <Source/Processor/NXP/LPC546xx/LPC546xx_I2S.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_LCDController.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_SD_MMC.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_PWM.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_PLL.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_RTC.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_SDRAM.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_Serial.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_SPI.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_Timer.h>
//#include <Source/Processor/NXP/LPC546xx/LPC546xx_USBDeviceController.h>
//#include <Source/Processor/NXP/LPC546xx/LPC546xx_USBHost.h>
#include <Source/Processor/NXP/LPC546xx/LPC546xx_Watchdog.h>
#include <uEZAudioAmp.h>
#include <uEZBSP.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>
#include <uEZFile.h>
#include <uEZI2C.h>
#include <uEZNetwork.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZStream.h>
#include <uEZAudioMixer.h>

extern int32_t MainTask(void);

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define CONFIG_MEMORY_TEST_ON_SDRAM     0

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_uezTask G_mainTask;
//static TUInt8 G_ms0_driveNum = 0;
//static TUInt32 G_USBHostDriveNumber = 0xFF;

/*---------------------------------------------------------------------------*
 * Memory placement section:
 *---------------------------------------------------------------------------*/
//Allocate general purpose frames memory
UEZ_PUT_SECTION(".frames", static TUInt8 _framesMemory [LCD_FRAMES_SIZE]);
TUInt8 *_framesMemoryptr = _framesMemory;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#if(COMPILER_TYPE==KEIL_UV)
#define nop()      __nop()
#else
#define nop()     NOP()//asm("nop")
#endif
#define nops5()    nop();nop();nop();nop();nop()
#define nops10()   nops5();nops5()
#define nops50()   nops10();nops10();nops10();nops10();nops10()

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSPDelayMS
 *---------------------------------------------------------------------------*
 * Description:
 *      Use a delay loop to approximate the time to delay.
 *      Should use UEZTaskDelayMS() when in a task instead.
 *---------------------------------------------------------------------------*/
void UEZBSPDelay1US(void)
{	
#if ( PROCESSOR_OSCILLATOR_FREQUENCY == 180000000) 
  nops50();
  nops50();
  nops10();
  nop();
  nop();
  nop(); // BF - Verified that UEZBSPDelayUS(50); ~51us debug and release build
#elif ( PROCESSOR_OSCILLATOR_FREQUENCY == 96000000)
  nops10(); //setup for FRO
  nops10();
  nops10();
  nop();
  nop();
  nop();
  nop();
#elif ( PROCESSOR_OSCILLATOR_FREQUENCY == 24000000)
  nops10();
  nops10();
  nop();
  nop();
  nop();
  nop();
#else
#error "1 microSecond delay not defined for CPU speed"
#endif
}
void UEZBSPDelayUS(uint32_t aMicroseconds)
{
    while (aMicroseconds--)
        UEZBSPDelay1US();
}

void UEZBSPDelay1MS(void)
{    
/* 	Measured to 1.000 milliseconds GPIO switch time: (RTOS off)
 *  -   Based on release build with high optimization speed setting, inline
 *      small functions enabled, with GPIO on/off verified with a scope.
 *      Use the following code for direct GPIO switching (example GPIO_P5_3):
 *          LPC_GPIO_PORT->SET[5] = 8; 
 *          UEZBSPDelay1MS();        
 *          LPC_GPIO_PORT->CLR[5] = 8; 
 */  
    TUInt32 i;
    for (i = 0; i < 995; i++)
        UEZBSPDelay1US();
}

void UEZBSPDelayMS(uint32_t aMilliseconds)
{
    while (aMilliseconds--) {
        UEZBSPDelay1MS();
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSPSDRAMInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the external SDRAM.
 *---------------------------------------------------------------------------*/
void UEZBSP_RAMInit(void)
{
  //Rev 2 has Alliance AS4C8M16SA-6 part
  /*static const T_LPC546xx_SDRAM_Configuration sdramConfig_8Mx16_6 = {
    UEZBSP_SDRAM_BASE_ADDR,
    UEZBSP_SDRAM_SIZE,
    SDRAM_CAS_2,
    SDRAM_CAS_2,
    LPC546xx_SDRAM_CLKOUT0,
    SDRAM_CLOCK_FREQUENCY,
    64,                     // ms
    4096,                   // cycles
    SDRAM_CYCLES(18),       // tRP min
    SDRAM_CYCLES(42),       // tRAS min
    SDRAM_CYCLES(67),       // tSREX min
    SDRAM_CYCLES(18),       // tAPR/tRCD
    SDRAM_CYCLES(30),       // tDAL
    SDRAM_CYCLES(24),       // tWR 
    SDRAM_CYCLES(60),       // tRC
    SDRAM_CYCLES(60),       // tRFC // match tRC
    SDRAM_CYCLES(67),       // tXSR
    SDRAM_CYCLES(12),       // tRRD
    SDRAM_CLOCKS(12) };     // tMRD clocks
      
  LPC546xx_SDRAM_Init_16BitBus(&sdramConfig_8Mx16_6);*/  
  
  //Rev 1 has ISSI IS42S16800F-7 part
  static const T_LPC546xx_SDRAM_Configuration sdramConfig_8Mx16_7 = {
    UEZBSP_SDRAM_BASE_ADDR,
    UEZBSP_SDRAM_SIZE,
    SDRAM_CAS_2,
    SDRAM_CAS_2,
    LPC546xx_SDRAM_CLKOUT0,
    SDRAM_CLOCK_FREQUENCY,
    64,                     // ms
    4096,                   // cycles
    SDRAM_CYCLES(15),       // tRP min - 2 clocks for CAS2
    SDRAM_CLOCKS(5),//SDRAM_CYCLES(37),       // tRAS min
    SDRAM_CYCLES(67),       // tSREX 
    SDRAM_CYCLES(15),       // tAPR/tRCD - 2 cycles
    (SDRAM_CYCLES(15)+SDRAM_CYCLES(22)),  // tDAL  = TWR+TRP rounded
    SDRAM_CYCLES(22),       // tWR 
    SDRAM_CLOCKS(8),//SDRAM_CYCLES(60),  // tRC 8 cycles from DS
    SDRAM_CLOCKS(8),//SDRAM_CYCLES(64),  // tRFC match tRC
    SDRAM_CYCLES(67),       // tXSR
    SDRAM_CYCLES(14),       // tRRD
    SDRAM_CYCLES(14) };     // tMRD clocks 
  LPC546xx_SDRAM_Init_16BitBus(&sdramConfig_8Mx16_7);
  
#if (CONFIG_MEMORY_TEST_ON_SDRAM == 1)
  MemoryTest(UEZBSP_SDRAM_BASE_ADDR, UEZBSP_SDRAM_SIZE);
#endif  
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_ROMInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the NOR Flash
 *---------------------------------------------------------------------------*/
void UEZBSP_ROMInit(void)
{

}

// Earliest platform init function
/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_Pre_PLL_SystemInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Earliest platform init function
 *      Can call before PLL comes on. For example to set LED initial state.
 *---------------------------------------------------------------------------*/
void UEZBSP_Pre_PLL_SystemInit(void) {
  // PIO3_14 initial state is pull up mode.
  // Turn off LED before init clocks. 
  // Then it will only start blinking after RTOS
  
  SYSCON->AHBCLKCTRLSET[0] = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(17));
  IOCON->PIO[3][14] = ((0 <<3) | (0x03 << 8) | (1 << 0x07) | (0x00));
  GPIO->DIR[3] |= (1 << 14);
  GPIO->CLR[3] |= (1<<14);
}

// Low level function for controlling LED after Pre-Pll init and before system starts up.
void UEZBSP_HeartBeat_Off(void) {  // PIO3_14 clear
  GPIO->CLR[3] |= (1<<14);
}
void UEZBSP_HeartBeat_On(void) {  // PIO3_14 set
  GPIO->SET[3] |= (1<<14);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_PLLConfigure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the PLL
 *---------------------------------------------------------------------------*/
void UEZBSP_PLLConfigure(void)
{
    const T_LPC546xx_PLL_Frequencies freq = {
            OSCILLATOR_CLOCK_FREQUENCY,

            //PLL1
            PROCESSOR_OSCILLATOR_FREQUENCY,

            // Run PLL1, USB
            48000000,

            // Use the main oscillator of 12 MHz as a source
            LPC546xx_CLKSRC_SELECT_MAIN_OSCILLATOR,

            // Use PLL0 for the CCLK, PCLK, and EMC Clk source (before dividers)
            LPC546xx_CPU_CLOCK_SELECT_PLL_CLK,

            // Use PPL1 (alt) for the USB
            LPC546xx_USB_CLOCK_SELECT_ALT_PLL_CLK,

            // CPU Clock is PLL0 / 1 or 180 MHz / 1 = 180 MHz
            1,

            // PCLK is PLL0 / 2, or 180 MHz
            1,

            // EMC runs at PLL0 / 2, or 90 MHz
            1,

            // USB Clock = PLL1 / 1 (48 MHz / 1)
            1,

            LPC546xx_CLOCK_OUT_SELECT_CPU,
            1,
            ETrue, };

    LPC546xx_PLL_SetFrequencies(&freq);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_InterruptsReset
 *---------------------------------------------------------------------------*
 * Description:
 *      Do the first initialization of the interrupts.
 *---------------------------------------------------------------------------*/
void UEZBSP_InterruptsReset(void)
{
    //InterruptsReset();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_CPU_PinConfigInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Immediately configure the port pins
 *---------------------------------------------------------------------------*/
void UEZBSP_CPU_PinConfigInit(void)
{
    // Place any pin configuration that MUST be initially here (at power up
    // but before even SDRAM is initialized)
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_FatalError
 *---------------------------------------------------------------------------*
 * Description:
 *      A fatal error in uEZ will come here last.  We must turn off
 *      interrupts and put the platform in a halted state with a blinking
 *      LED.
 * Inputs:
 *      int32_t aErrorCode -- Blink the LED a number of times equal to
 *          the error code.
 *---------------------------------------------------------------------------*/
void UEZBSP_FatalError(int32_t aErrorCode)
{
    register TUInt32 i;
    register TUInt32 count;
    // Ensure interrupts are turned off
    portDISABLE_INTERRUPTS();

    SYSCON->AHBCLKCTRLSET[0] = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(17));
    IOCON->PIO[3][14] = ((0 <<3) | (0x03 << 8) | (1 << 0x07) | (0x00));
    GPIO->DIR[3] |= (1 << 14);

    count = 0;
    while (1) {
        GPIO->SET[3] |= (1<<14);
        for (i = 0; i < 15000000; i++)
            nop();
        GPIO->CLR[3] |= (1<<14);
        for (i = 0; i < 15000000; i++)
            nop();
        count++;
        if (count >= aErrorCode) {
            // Long pause
            for (i = 0; i < 80000000; i++)
                nop();
            count = 0;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC546xx_ADC0_Require();
    ADCBank_Generic_Create("ADC0", "ADC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_4_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_4 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_4_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC546xx_GPIO3_Require();

    LPC546xx_ADC0_4_Require(GPIO_P0_16);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_5_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_5 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_5_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC546xx_GPIO5_Require();

    LPC546xx_ADC0_5_Require(GPIO_P0_31);//Uses dedicated pin
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_7_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_7 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_7_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC546xx_ADC0_7_Require(GPIO_P2_0);//Uses dedicated pin
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_8_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_8 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_8_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC546xx_ADC0_8_Require(GPIO_P2_1);//Uses dedicated pin
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_9_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_9 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_9_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC546xx_ADC0_9_Require(GPIO_P3_21);//Uses dedicated pin
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_10_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_10 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_10_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC546xx_ADC0_10_Require(GPIO_P3_22);//Uses dedicated pin
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2C2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C2 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2C2_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC546xx_GPIO4_Require();
    LPC546xx_I2C2_Require(GPIO_P4_20, GPIO_P4_21);
    I2C_Generic_Create("I2C2", "I2C2", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2S_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2S drivers for talking to the expansion board.
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2S_Require(void)
{
//    static const T_LPC546xx_I2S_Settings settings = {
//            GPIO_P5_9,      // I2S_RX_SDA
//            GPIO_PZ_4_P6_0, // I2S_RX_SCK
//            GPIO_NONE,      // I2S_RX_WS
//            GPIO_NONE,      // I2S_RX_MCLK
//            GPIO_P4_14,     // I2S_TX_SDA
//            GPIO_PZ_1_P3_0, // I2S_TX_SCK
//            GPIO_P4_13,     // I2S_TX_WS
//            GPIO_NONE,      // I2S_TX_MCLK
//    };
//    DEVICE_CREATE_ONCE();
//
//    LPC546xx_GPIO4_Require();
//    LPC546xx_GPIO5_Require();
//    LPC546xx_GPIOZ_Require();
//
//    LPC546xx_I2S_Require(&settings);
//    Generic_I2S_Create("I2S", "I2S");
}

void UEZPlatform_GPDMA1_Require(void)
{
//    DEVICE_CREATE_ONCE();
//
//    LPC546xx_GPDMA1_Require();
//    GPDMA_Generic_Create("GPDMA1", "GPDMA1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FileSystem_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the File System using FATFS
 *---------------------------------------------------------------------------*/
void UEZPlatform_FileSystem_Require(void)
{
    T_uezDevice dev_fs;

    DEVICE_CREATE_ONCE();

    FileSystem_FATFS_Create("FATFS");
    UEZDeviceTableFind("FATFS", &dev_fs);
    UEZFileSystemInit();
    UEZFileSystemMount(dev_fs, "/");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SD_MMC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the SD MMC Peripheral
 *---------------------------------------------------------------------------*/
void UEZPlatform_SD_MMC_Require(void)
{
    // Using the defaults for now
    const T_LPC546xx_SD_MMC_Pins pins = {
        GPIO_P2_6,      // DAT0
        GPIO_P2_7,      // DAT1
        GPIO_P2_8,      // DAT2
        GPIO_P2_9,      // DAT3
        GPIO_P2_3,      // CLK
        GPIO_P2_4,      // CMD
        GPIO_P4_22,     // Card Detect
        GPIO_NONE,      // Write Protect Detect
        GPIO_P2_5,      // Power Output
    };

    DEVICE_CREATE_ONCE();

    LPC546xx_GPIO2_Require();
    LPC546xx_GPIO4_Require();

    //All 8 data pins need to be configured for 4-bit mode to work
    LPC546xx_GPIO3_Require();

#define SD_4        GPIO_P3_16 //EXP UART
#define SD_5        GPIO_P3_17 //EXP UART
#define SD_6        GPIO_P3_18 //Unused
#define SD_7        GPIO_P3_19 //Unused

    UEZGPIOLock(SD_4);
    UEZGPIOControl(SD_4, GPIO_CONTROL_SET_CONFIG_BITS,
            IOCON_D_DEFAULT(2) | IOCON_SLEW_FAST);

    UEZGPIOLock(SD_5);
    UEZGPIOControl(SD_5, GPIO_CONTROL_SET_CONFIG_BITS,
            IOCON_D_DEFAULT(2) | IOCON_SLEW_FAST);

    UEZGPIOLock(SD_6);
    UEZGPIOControl(SD_6, GPIO_CONTROL_SET_CONFIG_BITS,
            IOCON_D_DEFAULT(2) | IOCON_SLEW_FAST);

    UEZGPIOLock(SD_7);
    UEZGPIOControl(SD_7, GPIO_CONTROL_SET_CONFIG_BITS,
            IOCON_D_DEFAULT(2) | IOCON_SLEW_FAST);

    LPC546xx_SD_MMC_Require(&pins);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_CRC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the CRC0 drivers for doing CRC calculations
 *---------------------------------------------------------------------------*/
void UEZPlatform_CRC0_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC546xx_CRC0_Require();
    CRC_Generic_Create("CRC0", "CRC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Backlight_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Backlight device driver for the LCD
 *---------------------------------------------------------------------------*/
void UEZPlatform_Backlight_Require(void)
{
//    const T_backlightGenericPWMSettings settings = {
//            "PWM0",
//            0, // master register
//            0, // backlight register (PWM0_1)
//            UEZ_LCD_BACKLIGHT_FULL_PERIOD,
//            UEZ_LCD_BACKLIGHT_FULL_PWR_ON,
//            UEZ_LCD_BACKLIGHT_FULL_PWR_OFF,
//            GPIO_P3_11,
//            EFalse
//    };
//    DEVICE_CREATE_ONCE();
//
//    LPC546xx_GPIO2_Require();
//    LPC546xx_GPIO3_Require();
//
//    LPC546xx_PWM0_A_Require(GPIO_P2_0);
//    Backlight_Generic_PWMControlled_Create("Backlight", &settings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_LCD_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD device driver and its backlight
 *---------------------------------------------------------------------------*/
void UEZPlatform_LCD_Require(void)
{
    const T_LPC546xx_LCDController_Pins pins = {
            GPIO_NONE,      // LCD_PWR -- GPIO controlled
            GPIO_P2_13,     // LCD_DCLK
            GPIO_NONE,//GPIO_P2_15,     // LCD_ENAB_M
            GPIO_P2_14,     // LCD_FP
            GPIO_NONE,      // LCD_LE
            GPIO_P2_16,     // LCD_LP

            {
                GPIO_NONE,  // LCD_VD0
                GPIO_NONE,  // LCD_VD1
                GPIO_NONE,  // LCD_VD2
                GPIO_P2_21, // LCD_VD3
                GPIO_P2_22, // LCD_VD4
                GPIO_P2_23, // LCD_VD5
                GPIO_P2_24, // LCD_VD6
                GPIO_P2_25, // LCD_VD7

                GPIO_NONE,  // LCD_VD8
                GPIO_NONE,  // LCD_VD9
                GPIO_P2_28, // LCD_VD10
                GPIO_P2_29, // LCD_VD11
                GPIO_P2_30, // LCD_VD12
                GPIO_P2_31, // LCD_VD13
                GPIO_P3_0,  // LCD_VD14
                GPIO_P3_1,  // LCD_VD15

                GPIO_NONE,  // LCD_VD16
                GPIO_NONE,  // LCD_VD17
                GPIO_NONE,  // LCD_VD18
                GPIO_P3_5,  // LCD_VD19
                GPIO_P3_6,  // LCD_VD20
                GPIO_P3_7,  // LCD_VD21
                GPIO_P3_8,  // LCD_VD22
                GPIO_P3_9,  // LCD_VD23
            },

            GPIO_NONE,      // LCD_CLKIN
            GPIO_P2_11,     //GPIO Power Control
            EFalse,
            0,

    };
    T_halWorkspace *p_lcdc;
    T_uezDeviceWorkspace *p_lcd;
    //T_uezDevice backlight;
    //T_uezDeviceWorkspace *p_backlight = 0;
    extern const T_uezDeviceInterface *UEZ_LCD_INTERFACE_ARRAY[];

    LPC546xx_GPIO2_Require();
    LPC546xx_GPIO3_Require();

    //TODO: Add BL driver when PWM is implemented.
    //Testing, Turn BL on 100%
    LPC546xx_GPIO4_Require();
    UEZGPIOSetMux(GPIO_P4_30, 0); //BL_PWM
    UEZGPIOSet(GPIO_P4_30);
    UEZGPIOOutput(GPIO_P4_30);

    UEZGPIOSetMux(GPIO_P3_28, 0); //BL_PWRSW
    UEZGPIOClear(GPIO_P3_28);
    UEZGPIOOutput(GPIO_P3_28);

    UEZGPIOSetMux(GPIO_P2_15, 0); //BL_PWRSW
    UEZGPIOSet(GPIO_P2_15);
    UEZGPIOOutput(GPIO_P2_15);

    //UEZPlatform_Timer0_Require();

    //UEZPlatform_Backlight_Require();

    LPC546xx_LCDController_Require(&pins);

    // Need to register LCD device and use existing LCD Controller
    UEZDeviceTableRegister(
            "LCD",
            (T_uezDeviceInterface *)UEZ_LCD_INTERFACE_ARRAY[UEZ_LCD_COLOR_DEPTH],
            0,
            &p_lcd);

    //SPI driver for the display
    UEZPlatform_SPI6_Require();

    HALInterfaceFind("LCDController", &p_lcdc);
    //UEZDeviceTableFind("Backlight", &backlight);
    //UEZDeviceTableGetWorkspace(backlight, (T_uezDeviceWorkspace **)&p_backlight);
    ((DEVICE_LCD *)(p_lcd->iInterface))->Configure(
            p_lcd,
            (HAL_LCDController **)p_lcdc,
            LCD_DISPLAY_BASE_ADDRESS,
            NULL);//(DEVICE_Backlight **)p_backlight);
    LCD_Tianma_TM035NKH02_09_Create("LCD", "SPI6", GPIO_P4_0);
}
/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Touchscreen_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the touchscreen and touch sensitivity for four wire resistive
 *---------------------------------------------------------------------------*/
void UEZPlatform_Touchscreen_Require(char *aName)
{
    // Select Touchscreen
    UEZPlatform_TouchscreenRT_Require(aName);
    //UEZPlatform_TouchscreenPCAP_Require(aName);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_TouchscreenRT_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the touchscreen and touch sensitivity for four wire resistive
 *---------------------------------------------------------------------------*/
void UEZPlatform_TouchscreenRT_Require(char * aName)
{
    T_uezDevice ts;

    const TS_FourWireTouchResist_Configuration tsConfig = {
            // XPlus ADC = ADC0.8 / P2.01
            { "ADC0",   8,  GPIO_P2_1,     0,      0 },
            // XMinus GPIO = P3.22
            { GPIO_P3_22 },
            // YPlus ADC = ADC0.7 / P2.00
            { "ADC0",   7,  GPIO_P2_0,     0,      0 },
            // YMinus GPIO = P3.21
            { GPIO_P3_21 },
            // Use IRQs?
            EFalse,
            // Need input buffer disabled when doing A/Ds?
            EFalse,
    };

    DEVICE_CREATE_ONCE();

    LPC546xx_GPIO2_Require();
    LPC546xx_GPIO3_Require();
    UEZPlatform_ADC0_7_Require();
    UEZPlatform_ADC0_8_Require();

    Touchscreen_FourWireTouchResist_Create(aName, &tsConfig);
    UEZDeviceTableFind("Touchscreen", &ts);
    UEZTSSetTouchDetectSensitivity(ts, 0x1000, 0x5000); // testing new numbers for touch detection
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Touchscreen_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the touchscreen and touch sensitivity for four wire resistive
 *---------------------------------------------------------------------------*/
void UEZPlatform_TouchscreenPCAP_Require(char *aName)
{
    DEVICE_CREATE_ONCE();

    LPC546xx_GPIO2_Require();

    UEZPlatform_I2C2_Require();

    //PCAP IRQ, P2_26
    UEZGPIOControl(GPIO_P2_26, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
    UEZGPIOSetMux(GPIO_P2_26, 0);
    UEZGPIOInput(GPIO_P2_26);

    Touchscreen_HX8526A_Create(aName, "I2C2", GPIO_P2_26, GPIO_P2_27);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Flash0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Flash0 device driver for the external QSPI Flash
 *---------------------------------------------------------------------------*/
void UEZPlatform_Flash0_Require(void)
{
    DEVICE_CREATE_ONCE();
    Flash_NXP_LPC_SPIFI_M4_Create("Flash0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer0 drivers, but don't set any capture sources or
 *      match registers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer0_Require(void)
{
    static const T_LPC546xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            GPIO_NONE,      // T0_CAP[2]
            GPIO_NONE,      // T0_CAP[3]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC546xx_Timer0_Require(&settings);
    Timer_Generic_Create("Timer0", "Timer0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer1 drivers, but don't set any capture sources or
 *      match registers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer1_Require(void)
{
    /* Not tested */
    static const T_LPC546xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T1_CAP[0]
            GPIO_NONE,      // T1_CAP[1]
            GPIO_NONE,      // T1_CAP[2]
            GPIO_NONE,      // T1_CAP[3]
            },
            {
            GPIO_NONE,      // T1_MAT[0]
            GPIO_NONE,      // T1_MAT[1]
            GPIO_NONE,      // T1_MAT[2]
            GPIO_NONE,      // T1_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC546xx_Timer1_Require(&settings);
    Timer_Generic_Create("Timer1", "Timer1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer2 drivers, but don't set any capture sources or
 *      match registers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer2_Require(void)
{
    /* Not tested */
    static const T_LPC546xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T2_CAP[0]
            GPIO_NONE,      // T2_CAP[1]
            GPIO_NONE,      // T2_CAP[2]
            GPIO_NONE,      // T2_CAP[3]
            },
            {
            GPIO_NONE,      // T2_MAT[0]
            GPIO_NONE,      // T2_MAT[1]
            GPIO_NONE,      // T2_MAT[2]
            GPIO_NONE,      // T2_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC546xx_Timer2_Require(&settings);
    Timer_Generic_Create("Timer2", "Timer2");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer3 drivers, but don't set any capture sources or
 *      match registers.
 *      Do not use when using PWM Backlight
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer3_Require(void)
{
    /* Not tested */
    static const T_LPC546xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T3_CAP[0]
            GPIO_NONE,      // T3_CAP[1]
            GPIO_NONE,      // T3_CAP[2]
            GPIO_NONE,      // T3_CAP[3]
            },
            {
            GPIO_NONE,      // T3_MAT[0]
            GPIO_NONE,      // T3_MAT[1]
            GPIO_NONE,      // T3_MAT[2]
            GPIO_NONE,      // T3_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC546xx_Timer3_Require(&settings);
    Timer_Generic_Create("Timer3", "Timer3");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer4_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer4 drivers, but don't set any capture sources or
 *      match registers.
 *      Do not use when using PWM Backlight
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer4_Require(void)
{
    /* Not tested */
    static const T_LPC546xx_Timer_Settings settings = {
            {
            GPIO_NONE,      // T3_CAP[0]
            GPIO_NONE,      // T3_CAP[1]
            GPIO_NONE,      // T3_CAP[2]
            GPIO_NONE,      // T3_CAP[3]
            },
            {
            GPIO_NONE,      // T3_MAT[0]
            GPIO_NONE,      // T3_MAT[1]
            GPIO_NONE,      // T3_MAT[2]
            GPIO_NONE,      // T3_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC546xx_Timer4_Require(&settings);
    Timer_Generic_Create("Timer4", "Timer4");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the PWM0 drivers
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM0_Require(void)
{
//    DEVICE_CREATE_ONCE();
//
//   LPC546xx_PWM0_Require();
//
//   PWM_Generic_Create("PWM0", "PWM0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the PWM1 driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM1_Require(void)
{
//    DEVICE_CREATE_ONCE();
//
//   LPC546xx_PWM1_Require();
//
//   PWM_Generic_Create("PWM1", "PWM1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the PWM1 driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM2_Require(void)
{
//    DEVICE_CREATE_ONCE();
//
//   LPC546xx_PWM2_Require();
//
//   PWM_Generic_Create("PWM1", "PWM1");
}

//static T_uezDevice G_Amp;
static TUInt8 G_Amp_In_Use = AUDIO_AMP_NONE;
/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_AudioAmp_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Audio Amp driver and set the default level
 *---------------------------------------------------------------------------*/
void UEZPlatform_AudioAmp_Require(void)
{
//    DEVICE_CREATE_ONCE();
//
//    UEZPlatform_I2C0_Require();
//    AudioAmp_LM48110_Create("Amp0", "I2C0", 0x1F);
//    UEZAudioAmpOpen("Amp0", &G_Amp);
//    UEZAudioAmpSetLevel(G_Amp, UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_LM48110);
      G_Amp_In_Use = AUDIO_AMP_LM48110;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI35CX_AudioMixerCallback
 *---------------------------------------------------------------------------*
 * Description:
 *
 *---------------------------------------------------------------------------*/
static T_uezError UEZGUI35CX_AudioMixerCallback(
        T_uezAudioMixerOutput aChangedOutput,
            TBool aMute,
            TUInt8 aLevel)
{
    T_uezError error = UEZ_ERROR_NONE;

    switch(aChangedOutput){
        case  UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER:
            if(G_Amp){
                if(aMute){
                    error = UEZAudioAmpMute(G_Amp);
                } else {
                    error = UEZAudioAmpUnMute(G_Amp);
                } 
                // This insures we scale the 0-255 input volume to the correct maximum volume for the onboard speaker.
                if(G_Amp_In_Use == AUDIO_AMP_TDA8551) { // enforce maximum volume based on AMP
                  aLevel = (aLevel * UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_TDA8551 /255);
                } else if(G_Amp_In_Use == AUDIO_AMP_WOLFSON) {
                  aLevel = (aLevel * UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL /255);
                } else if(G_Amp_In_Use == AUDIO_AMP_LM48110) {
                  aLevel = (aLevel * UEZ_DEFAULT_ONBOARD_SPEAKER_AUDIO_LEVEL_LM48110 /255);
                } else {
                    aLevel = 0; // NEED TO DEFINE AMP AND MAX SAFE VOLUME LEVEL!
                }
                error = UEZAudioAmpSetLevel(G_Amp, aLevel);
            }
        // Example Offboard speaker callback case
        /*case  UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER:
            if(G_Amp){
                if(aMute){
                    error = UEZAudioAmpMute(G_Amp);
                } else {
                    error = UEZAudioAmpUnMute(G_Amp);
                }
                error = UEZAudioAmpSetLevel(G_Amp, aLevel);
            }*/
            break;
        default:
            break;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_AudioMixer_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Audio Mixer driver and callback function
 *---------------------------------------------------------------------------*/
void UEZPlatform_AudioMixer_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_AudioAmp_Require();
    UEZAudioMixerRegister(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, &UEZGUI35CX_AudioMixerCallback);
    
    // To seperately control offboard speaker jack uncomment this line.
    //UEZAudioMixerRegister(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER, &UEZGUI70WVT_AudioMixerCallback);
    
    UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER); // mute here to avoid the "beep" sound from turning on too fast.

    // Set all 5 volume levels from platform file
    // First set master volume
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_MASTER, UEZ_DEFAULT_AUDIO_LEVEL); // master volume
    // Next set onboard speaker based on AMP that is loaded. Callback will handle this automatically.
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, UEZ_DEFAULT_AUDIO_LEVEL);
    
    // Uncomment these lines to set the other 3 volumes if needed.
    //UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER, UEZ_DEFAULT_OFFBOARD_SPEAKER_AUDIO_LEVEL); // We call this AFTER onboard speaker, so it will set the current volume.
    //UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_HEADPHONES, UEZ_DEFAULT_ONBOARD_HEADPHONES_AUDIO_LEVEL);
    //UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_HEADPHONES, UEZ_DEFAULT_OFFBOARD_HEADPHONES_AUDIO_LEVEL);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EEPROM_I2C_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal EEPROM in the LPC546xx
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM_LPC546xx_Require(void)
{
    DEVICE_CREATE_ONCE();
    EEPROM_NXP_LPC546xx_Create("EEPROM0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART.
 * Inputs:
 *      const char *aHALSerialName -- Name of UART serial driver to use
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    DEVICE_CREATE_ONCE();
    // NOTE: This require routine does NOT require the HAL driver
    // automatically!
    Serial_Generic_FullDuplex_Stream_Create("Console", aHALSerialName,
            aWriteBufferSize, aReadBufferSize);
    // Set standard output to console
    UEZStreamOpen("Console", &G_stdout);
    G_stdin = G_stdout;
    StdinRedirect(G_stdin);
    StdoutRedirect(G_stdout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART8_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART8.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART8_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART8 on P3.17/P3.16
    LPC546xx_GPIO3_Require();
    LPC546xx_UART8_Require(GPIO_P3_17, GPIO_P3_16);
    UEZPlatform_Console_FullDuplex_UART_Require("UART8", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
* Routine:  UEZPlatform_Console_FullDuplex_UART6_Require
*---------------------------------------------------------------------------*
* Description:
*      Setup the console using a full duplex UART using UART6.
* Inputs:
*      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
*      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
*---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART6_Require(
                                                  TUInt32 aWriteBufferSize,
                                                  TUInt32 aReadBufferSize)
{
  // UART8 on P4.3/P4.2
  LPC546xx_GPIO4_Require();
  LPC546xx_UART6_Require(GPIO_P4_3, GPIO_P4_2);
  UEZPlatform_Console_FullDuplex_UART_Require("UART6", aWriteBufferSize,
                                              aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART0.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART0_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART0 on P0.29/P0.30
    LPC546xx_GPIO0_Require();
    LPC546xx_UART0_Require(GPIO_P0_30, GPIO_P0_29);
    UEZPlatform_Console_FullDuplex_UART_Require("UART0", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_Expansion_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console to be a full duplex UART on the expansion header
 *      NOTE: On expansion header
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_Expansion_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // Standard Expansion board serial console is on UART8
    UEZPlatform_Console_FullDuplex_UART8_Require(aWriteBufferSize,
            aReadBufferSize);
}

void UEZPlatform_Console_HalfDuplex_UART0_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART0 on P0.29/P0.30
    LPC546xx_GPIO0_Require();
    LPC546xx_UART0_Require(GPIO_P0_30, GPIO_P0_29);
    UEZPlatform_HalfDuplex_RS485_UART0_Require("UART0", aWriteBufferSize,
            aReadBufferSize,
        GPIO_P0_31, ETrue,   1, // if one pin don't set it twice, otherwise we switch/delay twice in driver
        GPIO_NONE, EFalse,  1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_HalfDuplex_RS485_UART0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the half-duplex RS485 driver for UART5 as the uEZGUI Console.
 * Inputs:
 *      char *aHALSerialName, -- Serial Port Name (
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *      T_uezGPIOPortPin aDriveEnablePortPin -- drive enable GPIO pin
 *      TBool aDriveEnablePolarity -- Drive enable polarity (ETrue = HIGH true)
 *      TUInt32 aDriveEnableReleaseTime -- Drive enable release time (in ms)
 *      T_uezGPIOPortPin aReceiveEnablePortPin -- Receive enable GPIO pin
 *      TBool aReceiveEnablePolarity -- Receive enable polarity (EFalse = LOW true)
 *      TUInt32 aReceiveEnableReleaseTime -- Receive enable release time (in ms)
 *---------------------------------------------------------------------------*/
void UEZPlatform_HalfDuplex_RS485_UART0_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize,
        T_uezGPIOPortPin aDriveEnablePortPin,
        TBool aDriveEnablePolarity,
        TUInt32 aDriveEnableReleaseTime,
        T_uezGPIOPortPin aReceiveEnablePortPin,
        TBool aReceiveEnablePolarity,
        TUInt32 aReceiveEnableReleaseTime)
{
    T_RS485_GenericHalfDuplex_Settings aSettings;
    DEVICE_CREATE_ONCE();

    aSettings.iSerialName = aHALSerialName;
    aSettings.iQueueSendSize = aWriteBufferSize;
    aSettings.iQueueReceiveSize = aReadBufferSize;
    aSettings.iDriveEnable = aDriveEnablePortPin;
    aSettings.iDriveEnablePolarity = aDriveEnablePolarity;
    aSettings.iDriveEnableReleaseTime = aDriveEnableReleaseTime;
    aSettings.iReceiveEnable = aReceiveEnablePortPin;
    aSettings.iReceiveEnablePolarity = aReceiveEnablePolarity;
    aSettings.aReceiveEnableReleaseTime = aReceiveEnableReleaseTime;

    RS485_GenericHalfDuplex_Create("Console", &aSettings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_HalfDuplex_RS485_UART5_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the half-duplex RS485 driver for UART5 as the uEZGUI Console.
 * Inputs:
 *      char *aHALSerialName, -- Serial Port Name (
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *      T_uezGPIOPortPin aDriveEnablePortPin -- drive enable GPIO pin
 *      TBool aDriveEnablePolarity -- Drive enable polarity (ETrue = HIGH true)
 *      TUInt32 aDriveEnableReleaseTime -- Drive enable release time (in ms)
 *      T_uezGPIOPortPin aReceiveEnablePortPin -- Receive enable GPIO pin
 *      TBool aReceiveEnablePolarity -- Receive enable polarity (EFalse = LOW true)
 *      TUInt32 aReceiveEnableReleaseTime -- Receive enable release time (in ms)
 *---------------------------------------------------------------------------*/
void UEZPlatform_HalfDuplex_RS485_UART5_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize,
        T_uezGPIOPortPin aDriveEnablePortPin,
        TBool aDriveEnablePolarity,
        TUInt32 aDriveEnableReleaseTime,
        T_uezGPIOPortPin aReceiveEnablePortPin,
        TBool aReceiveEnablePolarity,
        TUInt32 aReceiveEnableReleaseTime)
{
    T_RS485_GenericHalfDuplex_Settings aSettings;
    DEVICE_CREATE_ONCE();
    // UART5 on P5.8/P5.7
    LPC546xx_GPIO5_Require();
    LPC546xx_UART5_Require(GPIO_P5_8, GPIO_P5_7);

    //GPIO_P5_9 Receive Enable
    //GPIO_P5_10 Drive enable

    aSettings.iSerialName = aHALSerialName;
    aSettings.iQueueSendSize = aWriteBufferSize;
    aSettings.iQueueReceiveSize = aReadBufferSize;
    aSettings.iDriveEnable = aDriveEnablePortPin;
    aSettings.iDriveEnablePolarity = aDriveEnablePolarity;
    aSettings.iDriveEnableReleaseTime = aDriveEnableReleaseTime;
    aSettings.iReceiveEnable = aReceiveEnablePortPin;
    aSettings.iReceiveEnablePolarity = aReceiveEnablePolarity;
    aSettings.aReceiveEnableReleaseTime = aReceiveEnableReleaseTime;

    RS485_GenericHalfDuplex_Create("RS485", &aSettings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART8_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART8.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART8_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART8 on P3.17/P3.16
    LPC546xx_GPIO3_Require();
    LPC546xx_UART8_Require(GPIO_P3_17, GPIO_P3_16);
    Serial_Generic_FullDuplex_Stream_Create("UART8", "UART8",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART0.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART0_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART0 on P0.29/P0.30
    LPC546xx_GPIO0_Require();
    LPC546xx_UART0_Require(GPIO_P0_30, GPIO_P0_29);
    Serial_Generic_FullDuplex_Stream_Create("UART0", "UART0",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART5_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART5.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART5_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART5 on P5.8/P5.7
    LPC546xx_GPIO5_Require();
    LPC546xx_UART5_Require(GPIO_P5_8, GPIO_P5_7);
    Serial_Generic_FullDuplex_Stream_Create("UART5", "UART5",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART6_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART5.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART6_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART6 on P4.3/P4.2
    LPC546xx_GPIO4_Require();
    LPC546xx_UART6_Require(GPIO_P4_3, GPIO_P4_2);
    Serial_Generic_FullDuplex_Stream_Create("UART6", "UART6",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART0.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART1_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
//    LPC546xx_GPIO6_Require();
//    LPC546xx_UART1_Require(GPIO_P6_12, GPIO_P6_13, GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE);
//    Serial_Generic_FullDuplex_Stream_Create("UART1", "UART1",
//            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART0.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART2_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
//    LPC546xx_GPIO4_Require();
//    LPC546xx_UART2_Require(GPIO_P4_8, GPIO_P4_9, GPIO_NONE, GPIO_NONE);
//    Serial_Generic_FullDuplex_Stream_Create("UART2", "UART2",
//            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART0.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART3_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
//#if 1
//    LPC546xx_GPIO5_Require();
//    LPC546xx_UART3_Require(GPIO_P5_3, GPIO_P5_4, GPIO_NONE, GPIO_NONE, GPIO_NONE);
//#else
//    LPC546xx_GPIO4_Require();
//    LPC546xx_GPIO5_Require();
//    LPC546xx_UART3_Require(GPIO_P4_15, GPIO_P5_17, GPIO_NONE, GPIO_NONE, GPIO_NONE);
//#endif
//
//    Serial_Generic_FullDuplex_Stream_Create("UART3", "UART3",
//            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SPI0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the SSP0 to be a SPI driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_SPI6_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC546xx_GPIO4_Require();
    LPC546xx_SPI6_Require(GPIO_P4_1, GPIO_P4_3, GPIO_P4_2, GPIO_NONE);

    SPI_Generic_Create("SPI6", "SPI6");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Speaker_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Speaker device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_Speaker_Require(void)
{
//    const T_ToneGenerator_Generic_Timer_Settings settings = {
//            "Timer3",
//            3, // MAT3[3]
//            GPIO_P5_19,
//            4, // 0 mux = GPIO
//            1, // 3 mux = T3_MAT0
//    };
//
//    DEVICE_CREATE_ONCE();
//    UEZPlatform_Timer3_Require();
//    ToneGenerator_Generic_Timer_Create("Speaker", &settings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_IRTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal RTC device driver.  Not compatible with the
 *      external RTC.
 *---------------------------------------------------------------------------*/
void UEZPlatform_IRTC_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC546xx_RTC_Require(ETrue);
    RTC_Generic_Create("RTC", "RTC");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_IRTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal RTC device driver.  Not compatible with the
 *      external RTC.
 *---------------------------------------------------------------------------*/
void UEZPlatform_RTC_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_IRTC_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Temp0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Temperature0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Temp0_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C2_Require();
    Temp_NXP_LM75A_Create("Temp0", "I2C2", 0x92 >> 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Accel0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Accelerometer 0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Accel0_Require(void)
{
//    DEVICE_CREATE_ONCE();
//
//    UEZPlatform_I2C0_Require();
//
//    ST_Accelo_LIS3DH_I2C_Create("Accel0", "I2C0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EMAC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the EMAC controller for networking.
 *---------------------------------------------------------------------------*/
void UEZPlatform_EMAC_Require(void)
{
//    // This EMAC is RMII (less pins)
//    const T_LPC546xx_EMAC_Settings emacSettings = {
//            GPIO_P0_1,          // ENET_TX_ENn      = PIN_P0_1_ENET_TXEN
//            GPIO_NONE,          // ENET_TX_TXD[3]   = not used for RMII
//            GPIO_NONE,          // ENET_TX_TXD[2]   = not used for RMII
//            GPIO_P0_15,         // ENET_TX_TXD[1]   = PIN_P1_20_ENET_TXD1
//            GPIO_P0_13,         // ENET_TX_TXD[0]   = PIN_P1_18_ENET_TXD0
//            GPIO_NONE,          // ENET_TX_ER       = not used for RMII
//            GPIO_PZ_0_P1_19,    // ENET_TX_CLK      = PIN_P1_19_ENET_REFCLK
//            GPIO_P0_3,          // ENET_RX_DV       = ENET_RX_DV
//            GPIO_NONE,          // ENET_RXD[3]      = not used for RMII
//            GPIO_NONE,          // ENET_RXD[2]      = not used for RMII
//            GPIO_P0_0,          // ENET_RXD[1]      = PIN_P0_0_ENET_RXD1
//            GPIO_P0_2,          // ENET_RXD[0]      = PIN_P1_15_ENET_RXD0
//            GPIO_P6_8,          // ENET_RX_ER       = PIN_P6_8_ENET_RX_ER
//            GPIO_NONE,          // ENET_RX_CLK      = not used for RMII
//            GPIO_NONE,          // ENET_COL         = not used for RMII
//            GPIO_NONE,          // ENET_CRS         = not used for RMII
//            GPIO_P6_0,          // ENET_MDC         = PIN_P6_0_ENET_MDC
//            GPIO_P0_12,         // ENET_MDIO        = PIN_P1_17_ENET_MDIO
//    };
//    DEVICE_CREATE_ONCE();
//
//    LPC546xx_GPIO0_Require();
//    LPC546xx_GPIO6_Require();
//    LPC546xx_GPIOZ_Require();
//
//    LPC546xx_EMAC_RMII_Require(&emacSettings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_WiredNetwork0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wired connections.
 *---------------------------------------------------------------------------*/
void UEZPlatform_WiredNetwork0_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Wired network needs an EMAC
    UEZPlatform_EMAC_Require();

    // Create the network driver for talking to lwIP on a wired
    // network.
#if (UEZ_ENABLE_TCPIP_STACK == 1)
    Network_lwIP_Create("WiredNetwork0");
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_PortA_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host on Port on USB1, off board USB
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_PortA_Require(void)
{
//#if UEZ_ENABLE_USB_HOST_STACK
//    extern void LPCUSBLib_USB1_IRQHandler(void);
//    DEVICE_CREATE_ONCE();
//
//    G_USBHostDriveNumber = 1;
//
//    LPC546xx_GPIO5_Require();
//
//    UEZGPIOSetMux(GPIO_P5_18, 2); // Turn on USB0_PPWR USB1
//    InterruptRegister(USB1_IRQn, LPCUSBLib_USB1_IRQHandler,
//        INTERRUPT_PRIORITY_NORMAL, "USB1");
//#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_PortB_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host on Port on USB0, on board USB
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_PortB_Require(void)
{
//#if UEZ_ENABLE_USB_HOST_STACK
//    extern void LPCUSBLib_USB0_IRQHandler(void);
//    DEVICE_CREATE_ONCE();
//
//    G_USBHostDriveNumber = 0;
//
//    LPC546xx_GPIO3_Require();
//
//    UEZGPIOSetMux(GPIO_P3_2, 1); // Turn on USB0_PPWR USB 0
//    InterruptRegister(USB0_IRQn, LPCUSBLib_USB0_IRQHandler,
//        INTERRUPT_PRIORITY_NORMAL, "USB0");
//#endif
}

void UEZPlatform_MS0_Connected(void *aWorkspace)
{
//    TUInt32 driveNum = G_ms0_driveNum;
//    T_uezDevice ms0;
//    T_uezDeviceWorkspace *p_ms0;
//
//    //printf("UEZGUI MS0 Connected drive %d\n", driveNum);
//    UEZDeviceTableFind("MS0", &ms0);
//    UEZDeviceTableGetWorkspace(ms0, (T_uezDeviceWorkspace **)&p_ms0);
//    // Reregister the device (doing a mount)
//    FATFS_RegisterMassStorageDevice(driveNum, (DEVICE_MassStorage **)p_ms0);
}

void UEZPlatform_MS0_Disconnected(void *aWorkspace)
{
//    TUInt32 driveNum = G_ms0_driveNum;
//
//    //printf("UEZGUI MS0 Disconnected drive %d\n", driveNum);
//    // Unregister the device (doing a unmount)
//    FATFS_UnregisterMassStorageDevice(driveNum);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MS0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS0 using USBHost flash drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS0_Require(TUInt32 aUSBNumber)
{
//#if UEZ_ENABLE_USB_HOST_STACK
//    T_MassStorage_LPCUSBLib_Callbacks ms0Callbacks = {
//            UEZPlatform_MS0_Connected,
//            UEZPlatform_MS0_Disconnected
//    };
//    DEVICE_CREATE_ONCE();
//
//    MassStorage_LPCUSBLib_Create("MS0", aUSBNumber, &ms0Callbacks, 0);
//#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBFlash_Drive_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Flash drive using MS0 on the given drive number
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBFlash_Drive_Require(TUInt8 aDriveNum)
{
//    T_uezDevice ms0;
//    T_uezDeviceWorkspace *p_ms0;
//
//    DEVICE_CREATE_ONCE();
//
//    if(G_USBHostDriveNumber == 0xFF){
//        UEZFailureMsg("Must Require Port A or B before the FlashDrive\n");
//    }
//
//    // Now put the MS0 in the filesystem using the USB
//    UEZPlatform_MS0_Require(G_USBHostDriveNumber);
//    UEZPlatform_FileSystem_Require();
//
//    UEZDeviceTableFind("MS0", &ms0);
//    UEZDeviceTableGetWorkspace(ms0, (T_uezDeviceWorkspace **)&p_ms0);
//    FATFS_RegisterMassStorageDevice(aDriveNum, (DEVICE_MassStorage **)p_ms0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MCI_DefaultFreq
 *---------------------------------------------------------------------------*
 * Description:
 *      Set the bootup max frequency for SD/MCI/SPI mode data transfer.
 *      Existing drivers will divide down till <= this frequency.
 *---------------------------------------------------------------------------*/
TUInt32 UEZPlatform_MCI_DefaultFreq(void)
{
  return 52000000UL;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MCI_TransferMode
 *---------------------------------------------------------------------------*
 * Description:
 *      For SD/MCI mode select 1-bit or 4-bit mode data transfer.
 *---------------------------------------------------------------------------*/
TUInt32 UEZPlatform_MCI_TransferMode(void)
{
  return UEZ_MCI_BUS_4BIT_WIDE;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MS1_SD_MMC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS1 using MMC SDCard drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS1_SD_MMC_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_SD_MMC_Require();
    MassStorage_SDCard_SD_MMC_Create("MS1", "SD_MMC");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SDCard_Drive_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the high speed SDCard drive using MS1 on the given drive number
 *---------------------------------------------------------------------------*/
void UEZPlatform_SDCard_Drive_Require(TUInt8 aDriveNum)
{
    T_uezDevice ms1;
    T_uezDeviceWorkspace *p_ms1;

    DEVICE_CREATE_ONCE();

    UEZPlatform_MS1_SD_MMC_Require();
    UEZPlatform_FileSystem_Require();

    UEZDeviceTableFind("MS1", &ms1);
    UEZDeviceTableGetWorkspace(ms1, (T_uezDeviceWorkspace **)&p_ms1);
    FATFS_RegisterMassStorageDevice(aDriveNum, (DEVICE_MassStorage **)p_ms1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_System_Reset
 *---------------------------------------------------------------------------*
 * Description:
 *      Do a board specific system reset. In some cases we have a pin that
 *      can trigger POR as if you pushed a physical reset button.
 *      This is necessary to insure a full hardware reset across all lines
 *      with minimum reset hold timing.
 *---------------------------------------------------------------------------*/

void UEZPlatform_System_Reset(void) {
  CPUDisableInterrupts(); // Per ARM should disable ISR when resetting, but it isn't done in reset function and CPSID is even missing in intrinsics.h
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dai0321a/BIHDHCGJ.html  
  // By default use HW reset pin on this board so that we can trigger POR.
  // Use low level code to make sure we can call this anytime.  
  SYSCON->AHBCLKCTRLSET[2] = (1U << CLK_GATE_ABSTRACT_BITS_SHIFT(9)); // enable GPIO4 clock  
  IOCON->PIO[UEZ_GPIO_PORT_FROM_PORT_PIN(PIN_HW_RESET)][UEZ_GPIO_PIN_FROM_PORT_PIN(PIN_HW_RESET)] = ((0 <<3) | (0x03 << 8) | (1 << 0x07) | (0x00)); //UEZGPIOControl(PIN_HW_RESET, GPIO_CONTROL_SET_CONFIG_BITS, IOCON_D_DEFAULT(0));
  // Reset any HW that might cause bootup issues on warm boot.
    
  // Finally switch the GPIO pin which will trigger POR and hold reset for 150ms.
  GPIO->CLR[UEZ_GPIO_PORT_FROM_PORT_PIN(PIN_HW_RESET)] |= (1 << UEZ_GPIO_PIN_FROM_PORT_PIN(PIN_HW_RESET)); //UEZGPIOClear(PIN_HW_RESET);
  GPIO->DIR[UEZ_GPIO_PORT_FROM_PORT_PIN(PIN_HW_RESET)] |= (1 << UEZ_GPIO_PIN_FROM_PORT_PIN(PIN_HW_RESET)); //UEZGPIOOutput(PIN_HW_RESET);
  //NVIC_SystemReset(); // Cannot use this with SDRAM or you will encounter bootup issues!
}

/*---------------------------------------------------------------------------*
 * Routine:  uEZPlatformInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the board with all the proper settings.
 *      Registers all peripherals specific to this board.
 *---------------------------------------------------------------------------*/
void uEZPlatformInit(void)
{
    // Do any initialization necessary before the RTOS is started
}

void UEZPlatform_TouchscreenReplay_Require(void)
{
//    DEVICE_CREATE_ONCE();
//
//    UEZPlatform_Touchscreen_Require("TouchscreenReal");
//    //Touchscreen_Replay_Create("Touchscreen", "TouchscreenReal");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Watchdog_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal watchdog.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Watchdog_Require(void)
{
//    DEVICE_CREATE_ONCE();
//    LPC546xx_Watchdog_Require();
//    Watchdog_Generic_Create("Watchdog", "Watchdog");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_WirelessNetwork0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wireless connections.
 *---------------------------------------------------------------------------*/
//#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_CmdLib.h>
void UEZPlatform_WirelessNetwork0_Require(void)
{
//    // These SPI pins work for both the Application Header and the PMOD Header
//    // for GainSpan Modules with SPI configured.
//    const T_GainSpan_Network_SPISettings spi_settings = {
//    // SPI Settings
//        "SSP0",// SSP Name
//        3000, // 3 MHz
//        // Pins.
//        GPIO_P7_16, // iSPIChipSelect = WIFI_CSN = P2.23
//        GPIO_P5_12, // iDataReadyIO = WIFI_INTR
//        GPIO_NONE,  // iProgramMode = P1.6_MCIDAT0_PWM0.4
//        GPIO_NONE,  // iSPIMode = WIFI_MODE
//        GPIO_NONE,  // iSRSTn = WIFI_SRSTn AND'd with RESET_OUTn
//        GPIO_NONE,  // iWIFIFactoryRestore = WIFI_FAC_RST
//        };
//
//    DEVICE_CREATE_ONCE();
//
//    LPC546xx_GPIO5_Require();
//    LPC546xx_GPIO7_Require();
//
//    //Set CS to GPIO Outpit
//    UEZGPIOControl(GPIO_P7_16, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
//    UEZGPIOSetMux(GPIO_P7_16, 4);
//    UEZGPIOSet(GPIO_P7_16);
//    UEZGPIOOutput(GPIO_P7_16);
//
//    //Set int32_t INTR pin to an input.
//    UEZGPIOControl(GPIO_P5_12, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
//    UEZGPIOSetMux(GPIO_P5_12, 4);
//    UEZGPIOInput(GPIO_P5_12);
//
//    UEZPlatform_SSP0_Require();
//    Network_GainSpan_Create("WirelessNetwork0", &spi_settings);
}

void UEZPlatform_USBHost_USB0_Serial_Require(void)
{
//#if UEZ_ENABLE_USB_HOST_STACK
//    UEZGPIOSetMux(GPIO_P3_2, 1); // Turn on USB0_PPWR USB 0
//    Stream_LPCUSBLib_SerialHost_Create("SerialHost", 1); // USB0
//    Stream_LPCUSBLib_SerialHost_FTDI_Create("USB0Serial", 0, 1024, 1024); // USB0
//#endif
}

void UEZPlatform_USBHost_USB1_Serial_Require(void)
{
//#if UEZ_ENABLE_USB_HOST_STACK
//    UEZGPIOSetMux(GPIO_P5_18, 2); // Turn on USB0_PPWR USB1
//    Stream_LPCUSBLib_SerialHost_FTDI_Create("USB1Serial", 1, 1024, 1024); // USB1
//#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ButtonBoard_Require
 *---------------------------------------------------------------------------*/
/**
 *  Setup the I2C GPIO drivers for talking to the button board
 */
/*---------------------------------------------------------------------------*/
/*void UEZPlatform_ButtonBoard_Require(void)
{
    static const T_GPIOKeypadAssignment keyAssignment[] = {
        { 0, KEY_ENTER },
        { 1, KEY_ARROW_LEFT },
        { 2, KEY_ARROW_RIGHT },
        { 3, KEY_ARROW_DOWN },
        { 4, KEY_ARROW_UP },
        { 0, 0 },
    };

    DEVICE_CREATE_ONCE();
    UEZPlatform_I2C0_Require();
    GPIO_PCF8574T_Create("GPIO:PCF8574T", UEZ_GPIO_PORT_EXT1, "I2C0", 0x48>>1);
    Keypad_Generic_GPIO_Create("BBKeypad", UEZ_GPIO_PORT_EXT1, keyAssignment, 5,
        KEYPAD_LOW_TRUE_SIGNALS, 
        UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 7), 
        UEZ_GPIO_PORT_PIN(UEZ_GPIO_PORT_EXT1, 6));
}*/

// Don't enable console anymore in minimal requires, so that we can use this for GUI only bootloader, etc.
void UEZPlatform_Minimal_Require(void)
{
    LPC546xx_GPIO2_Require();
    LPC546xx_GPIO3_Require();
}

void UEZPlatform_Standard_Require(void)
{
    UEZPlatform_Minimal_Require();

    //Testing
    UEZPlatform_I2C2_Require();
//
    UEZPlatform_LCD_Require();
    UEZPlatform_Touchscreen_Require("Touchscreen");
//
    //Testing
    UEZPlatform_ADC0_7_Require();
//
    UEZPlatform_Flash0_Require();
//    UEZPlatform_Timer0_Require();
//    UEZPlatform_Timer1_Require();
//    UEZPlatform_Timer2_Require();
//    UEZPlatform_Timer3_Require();
//    UEZPlatform_AudioMixer_Require();
//    UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
//    UEZPlatform_Speaker_Require();
    UEZPlatform_EEPROM_LPC546xx_Require();
    UEZPlatform_Temp0_Require();
//    UEZPlatform_Accel0_Require();
//
//    UEZPlatform_Console_FullDuplex_UART0_Require(1024, 1024);
    UEZPlatform_IRTC_Require();
    
    UEZPlatform_AudioMixer_Require(); // UEZPlatform_AudioAmp_Require();
    UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
    UEZPlatform_Speaker_Require();
    //UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
}

TBool UEZPlatform_Host_Port_B_Detect(void)
{
    TBool IsDevice = ETrue;
//
//    UEZGPIOSetMux(GPIO_P3_2, 0);
//    UEZGPIOSet(GPIO_P3_2); // disable MIC2025
//    UEZGPIOOutput(GPIO_P3_2);
//
//    IsDevice = (LPC_USB0->OTGSC & (1 << 8))? ETrue : EFalse;
//
//    if(!IsDevice){
//        UEZGPIOClear(GPIO_P3_2);// enable MIC2025
//    }
    return IsDevice;
}

TUInt16 UEZPlatform_LCDGetHeight(void)
{
    return UEZ_LCD_DISPLAY_HEIGHT;
}
TUInt16 UEZPlatform_LCDGetWidth(void)
{
    return UEZ_LCD_DISPLAY_WIDTH;
}
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>
TUInt32 UEZPlatform_LCDGetFrame(TUInt16 aFrameNum)
{
    return (TUInt32)FRAME(aFrameNum);
}
TUInt32 UEZPlatform_LCDGetFrameBuffer(void)
{
    return (TUInt32)LCD_FRAME_BUFFER;
}
TUInt32 UEZPlatform_LCDGetFrameSize(void)
{
    return FRAME_SIZE;
}
TUInt32 UEZPlatform_SerialGetDefaultBaud(void)
{
    return SERIAL_PORTS_DEFAULT_BAUD;
}
TUInt32 UEZPlatform_ProcessorGetFrequency(void)
{
    return PROCESSOR_OSCILLATOR_FREQUENCY;
}
TUInt32 UEZPlatform_GetPCLKFrequency(void)
{
    return PROCESSOR_OSCILLATOR_FREQUENCY;
}

T_pixelColor SUICallbackRGBConvert(int32_t r, int32_t g, int32_t b)
{
    return RGB(r, g, b);
}

TUInt32 UEZPlatform_GetBaseAddress(void)
{
    return LCD_DISPLAY_BASE_ADDRESS;
}
#if INCLUDE_EMWIN
#include <Source/Library/GUI/SEGGER/emWin/LCD.h>
#include <Source/Library/GUI/SEGGER/emWin/GUIDRV_Lin.h>
const void* UEZPlatform_GUIColorConversion(void)
{    
    return GUICC_M555;
}

const void *UEZPlatform_GUIDisplayDriver()
{
    return &GUIDRV_Lin_16_API;
}
#else
const void* UEZPlatform_GUIColorConversion(void)
{
    return 0;
}

const void *UEZPlatform_GUIDisplayDriver()
{
    return 0;
}
#endif

void SUICallbackSetLCDBase(void *aAddress)
{
    LCD->UPBASE = (TUInt32)aAddress;
}

TUInt32 SUICallbackGetLCDBase()
{
    return LCD->UPBASE;
}

void WriteByteInFrameBufferWithAlpha(UNS_32 aAddr, COLOR_T aPixel, T_swimAlpha aAlpha)
{
    static COLOR_T mask = (COLOR_T)(~((1<<10)|(1<<5)|(1<<0)));
    COLOR_T *p = (COLOR_T *)aAddr;

    switch (aAlpha) {
        case SWIM_ALPHA_50:
            *p = (((*p & mask)>>1) + ((aPixel & mask)>>1));
            break;
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  vMainMPUFaultHandler
 *---------------------------------------------------------------------------*/
/**
 *  Determine which stack was in use when the MPU fault occurred and extract
    the stacked PC.
 *
 *  @return         int32_t
 */
/*---------------------------------------------------------------------------*/
void vMainMPUFaultHandler( unsigned long * pulFaultRegisters ) {
unsigned long ulStacked_pc = 0UL;
    ( void ) ulStacked_pc;
    __asm
    (
        "   tst lr, #4          \n"
        "   ite eq              \n"
        "   mrseq r0, msp       \n" /* The code that generated the exception was using the main stack. */
        "   mrsne r0, psp       \n" /* The code that generated the exception was using the process stack. */
        "   ldr r0, [r0, #24]   \n" /* Extract the value of the stacked PC. */
        "   str r0, [sp]        \n" /* Store the value of the stacked PC into ulStacked_pc. */
    );

    for( ;; );     /* Inspect ulStacked_pc to locate the offending instruction. */
}

/*---------------------------------------------------------------------------*
 * Routine:  main
 *---------------------------------------------------------------------------*/
/**
 *  The main() routine in UEZ is only a stub that is used to start
 *      the whole UEZ system.  UEZBSP_Startup() is immediately called.
 *
 *  @return         int32_t
 */
/*---------------------------------------------------------------------------*/
int32_t main(void)
{
    UEZBSP_Startup();

    while (1) {
    } // never should get here
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform.c
 *-------------------------------------------------------------------------*/
