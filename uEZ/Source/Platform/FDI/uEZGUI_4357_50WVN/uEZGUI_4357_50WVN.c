/*-------------------------------------------------------------------------*
 * File:  uEZPlatform.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the uEZGUI-4357-50WVN
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
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

 /**
 *    @addtogroup Platform_uEZGUI_4357_50WVN
 *  @{
 *  @brief     uEZ Platform file for the uEZGUI_4357_50WVN
 *  @see http://goo.gl/UDtTCR/
 *  @see http://goo.gl/UDtTCR/files/uEZLicense.txt
 *
 *    uEZ Platform file for the uEZGUI-435750WVN
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
#include <Source/Processor/NXP/LPC43xx/LPC43xx_USBDeviceController.h>
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
#include <Source/Devices/CRC/Software/CRC_Software.h>
#include <Source/Devices/DAC/Generic/DAC_Generic.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM_Generic_I2C.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM16_Generic_I2C.h>
#include <Source/Devices/EEPROM/NXP/LPC43xx/EEPROM_NXP_LPC43xx.h>
#include <Source/Devices/Flash/NXP/LPC_SPIFI_M4F/Flash_NXP_LPC_SPIFI_M4.h>
#include <Source/Devices/GPDMA/Generic/Generic_GPDMA.h>
#include <Source/Devices/HID/Generic/HID_Generic.h>
#include <Source/Devices/I2C/Generic/Generic_I2C.h>
#include <Source/Devices/I2S/Generic/Generic_I2S.h>
#include <Source/Devices/LCD/Innolux/AT070TN84/Innolux_AT070TN84.h>
#include <Source/Devices/LED/NXP/PCA9551/LED_NXP_PCA9551.h>
#include <Source/Devices/MassStorage/LPCUSBLib/LPCUSBLib_MS.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_SD_MMC.h>
#include <Source/Devices/Stream/LPCUSBLib/SerialHost/Stream_LPCUSBLib_SerialHost.h>
#include <Source/Devices/Network/GainSpan/Network_GainSpan.h>
#include <Source/Devices/Network/lwIP/Network_lwIP.h>
#include <Source/Devices/PWM/Generic/Generic_PWM.h>
#include <Source/Devices/RTC/Generic/Generic_RTC.h>
#include <Source/Devices/RTC/NXP/PCF8563/RTC_PCF8563.h>
#include <Source/Devices/Serial/Generic/Generic_Serial.h>
#include <Source/Devices/Serial/LPCUSBLib/SerialHost_FTDI/Stream_LPCUSBLib_SerialHost_FTDI.h>
#include <Source/Devices/SPI/Generic/Generic_SPI.h>
#include <Source/Devices/Temperature/NXP/LM75A/Temperature_LM75A.h>
#include <Source/Devices/Timer/Generic/Timer_Generic.h>
#include <Source/Devices/ToneGenerator/Generic/Timer/ToneGenerator_Generic_Timer.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>
#include <Source/Devices/Touchscreen/Newhaven/FT5306DE4/FT5306DE4TouchScreen.h>
//#include <Source/Devices/USBDevice/NXP/LPC43xx/LPC43xx_USBDevice.h>
#include <Source/Devices/USBHost/Generic/Generic_USBHost.h>
#include <Source/Devices/Watchdog/Generic/Watchdog_Generic.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_RTC.h>
#include <Source/Library/Web/BasicWeb/BasicWEB.h>
#include <Source/Library/FileSystem/FATFS/uEZFileSystem_FATFS.h>
#include <Source/Library/Graphics/SWIM/lpc_swim.h>
#include <Source/Library/Memory/MemoryTest/MemoryTest.h>
#include <Source/Library/StreamIO/StdInOut/StdInOut.h>
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
#include <uEZNetwork.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZStream.h>
#include <uEZAudioMixer.h>
#include <Source/Devices/GPIO/I2C/PCA9532/GPIO_PCA9532.h>

extern int MainTask(void);

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define NOR_FLASH_BASE_ADDR             0x80000000
#define CONFIG_MEMORY_TEST_ON_SDRAM     0

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static T_uezDevice G_stdout = 0;
static T_uezDevice G_stdin = 0;
T_uezTask G_mainTask;
static TUInt8 G_ms0_driveNum = 0;
static TUInt32 G_USBHostDriveNumber = 0xFF;

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#if(COMPILER_TYPE==Keil4)
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
/* 	Measured to 1.004 microseconds GPIO switch time: (RTOS off)
 *  -   Based on release build with high optimization speed setting, inline
 *      small functions enabled, with GPIO on/off verified with a scope.
 *      Use the following code for direct GPIO switching (example GPIO_P5_3):
 *          LPC_GPIO_PORT->SET[5] = 8; 
 *          UEZBSPDelay1US();        
 *          LPC_GPIO_PORT->CLR[5] = 8; 
 */
#if ( PROCESSOR_OSCILLATOR_FREQUENCY == 204000000)
    nops50();
    nops50();
    nops50();
    nops10();
    nops10();
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
void UEZBSPDelayUS(unsigned int aMicroseconds)
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
    for (TUInt32 i = 0; i < 995; i++)
        UEZBSPDelay1US();
}

void UEZBSPDelayMS(unsigned int aMilliseconds)
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
    static const T_LPC43xx_SDRAM_Configuration sdramConfig_MT48LC2M32B2P = {
            UEZBSP_SDRAM_BASE_ADDR,
            UEZBSP_SDRAM_SIZE,
            SDRAM_CAS_2,
            SDRAM_CAS_3,
            LPC43xx_SDRAM_CLKOUT1,
            SDRAM_CLOCK_FREQUENCY,
            64, // ms
            4096,//8192,//15625,//8192, // cycles
            SDRAM_CYCLES(20),
            SDRAM_CYCLES(45),
            SDRAM_CYCLES(70),
            5,//SDRAM_CYCLES(18),
            5,//SDRAM_CLOCKS(4),
            SDRAM_CLOCKS(14),//(SDRAM_CYCLES(6) + SDRAM_CLOCKS(1)),
            SDRAM_CYCLES(68),
            SDRAM_CYCLES(68),
            SDRAM_CYCLES(70),
            SDRAM_CYCLES(14),
            SDRAM_CLOCKS(2) };
    LPC43xx_SDRAM_Init_32BitBus(&sdramConfig_MT48LC2M32B2P);

#if CONFIG_MEMORY_TEST_ON_SDRAM
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

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_PLLConfigure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the PLL
 *---------------------------------------------------------------------------*/
void UEZBSP_PLLConfigure(void)
{
    const T_LPC43xx_PLL_Frequencies freq = {
            OSCILLATOR_CLOCK_FREQUENCY,

            // Run PLL0 at 48 MHz
            48000000,

            // Run PLL1
            PROCESSOR_OSCILLATOR_FREQUENCY,

            // Use the main oscillator of 12 MHz as a source
            LPC43xx_CLKSRC_SELECT_MAIN_OSCILLATOR,

            // Use PLL0 for the CCLK, PCLK, and EMC Clk source (before dividers)
            LPC43xx_CPU_CLOCK_SELECT_PLL_CLK,

            // Use PPL1 (alt) for the USB
            LPC43xx_USB_CLOCK_SELECT_ALT_PLL_CLK,

            // CPU Clock is PLL0 / 1 or 204 MHz / 1 = 204 MHz
            1,

            // PCLK is PLL0 / 2, or 204 MHz
            1,

            // EMC runs at PLL0 / 2, or 102 MHz
            1,

            // USB Clock = PLL1 / 1 (48 MHz / 1)
            1,

            LPC43xx_CLOCK_OUT_SELECT_CPU,
            1,
            ETrue, };

    LPC43xx_PLL_SetFrequencies(&freq);
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
 *      int aErrorCode -- Blink the LED a number of times equal to
 *          the error code.
 *---------------------------------------------------------------------------*/
void UEZBSP_FatalError(int aErrorCode)
{
    register TUInt32 i;
    register TUInt32 count;
    // Ensure interrupts are turned off
    portDISABLE_INTERRUPTS();

    LPC_SCU->SFSP1_4 = (0x3 << 3) | 0;

    LPC_GPIO_PORT->DIR[0] |= (1<<11);

    LPC_GPIO_PORT->CLR[0] |= 1 << 11; //Failed turn off the LED
    // Blink our status led in a pattern, forever

    count = 0;
    while (1) {
        LPC_GPIO_PORT->SET[0] |= 1 << 11; // on
        for (i = 0; i < 15000000; i++)
            nop();
        //__nop();//asm ( "nop" );
        LPC_GPIO_PORT->CLR[0] |= 1 << 11;// off
        for (i = 0; i < 15000000; i++)
            nop();
        //__nop();//asm ( "nop" );
        count++;
        if (count >= aErrorCode) {
            // Long pause
            for (i = 0; i < 80000000; i++)
                nop();
            //__nop();//asm ( "nop" );
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
    LPC43xx_ADC0_Require();
    ADCBank_Generic_Create("ADC0", "ADC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC1 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC1_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_ADC1_Require();
    ADCBank_Generic_Create("ADC1", "ADC1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_0 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_ADC0_0_Require(GPIO_NONE);//Uses dedicated pin
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_1 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_1_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_GPIO2_Require();

    LPC43xx_ADC0_1_Require(GPIO_P2_1);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_2 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_2_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_GPIO7_Require();

    LPC43xx_ADC0_2_Require(GPIO_P7_22);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_3 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_3_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_GPIO3_Require();

    LPC43xx_ADC0_3_Require(GPIO_P3_13);
    UEZPlatform_ADC0_Require();
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
    LPC43xx_GPIO3_Require();

    LPC43xx_ADC0_4_Require(GPIO_P3_12);
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
    LPC43xx_GPIO5_Require();

    LPC43xx_ADC0_5_Require(GPIO_NONE);//Uses dedicated pin
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_6_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0_6 channel on the ADC0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_6_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_ADC0_6_Require(GPIO_NONE);//Uses dedicated pin
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
    LPC43xx_ADC0_7_Require(GPIO_NONE);//Uses dedicated pin
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC1_0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC1_0 channel on the ADC1 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC1_0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_GPIO7_Require();

    LPC43xx_ADC1_0_Require(GPIO_P7_24);
    UEZPlatform_ADC1_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC1_3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC1_3 channel on the ADC1 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC1_3_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_GPIO7_Require();

    LPC43xx_ADC1_3_Require(GPIO_P7_20);
    UEZPlatform_ADC1_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2C0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C0 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2C0_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC43xx_I2C0_Require(GPIO_NONE, GPIO_NONE);
    I2C_Generic_Create("I2C0", "I2C0", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2S_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2S drivers for talking to the expansion board.
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2S_Require(void)
{
    static const T_LPC43xx_I2S_Settings settings = {
            GPIO_P5_9,      // I2S_RX_SDA
            GPIO_PZ_4_P6_0, // I2S_RX_SCK
            GPIO_NONE,      // I2S_RX_WS
            GPIO_NONE,      // I2S_RX_MCLK
            GPIO_P4_14,     // I2S_TX_SDA
            GPIO_PZ_1_P3_0, // I2S_TX_SCK
            GPIO_P4_13,     // I2S_TX_WS
            GPIO_NONE,      // I2S_TX_MCLK
    };
    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO4_Require();
    LPC43xx_GPIO5_Require();
    LPC43xx_GPIOZ_Require();

    LPC43xx_I2S_Require(&settings);
    Generic_I2S_Create("I2S", "I2S");
}

void UEZPlatform_GPDMA1_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC43xx_GPDMA1_Require();
    GPDMA_Generic_Create("GPDMA1", "GPDMA1");
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
    const T_LPC43xx_SD_MMC_Pins pins = {
        GPIO_P6_3,      // DAT0
        GPIO_P6_4,      // DAT1
        GPIO_P6_5,      // DAT2
        GPIO_P6_6,      // DAT3
        GPIO_PZ_Z_PC_0, // CLK
        GPIO_P6_9,      // CMD
        GPIO_P6_7,      // Card Detect
        GPIO_NONE,      // Write Protect Detect
        GPIO_P6_15,     // Power Output
    };

    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO6_Require();
    LPC43xx_GPIOZ_Require();

    LPC43xx_SD_MMC_Require(&pins);
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

    Software_CRC_Create("CRC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Backlight_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Backlight device driver for the LCD
 *---------------------------------------------------------------------------*/
void UEZPlatform_Backlight_Require(void)
{
    const T_backlightGenericPWMSettings settings = {
            "PWM0",
            0, // master register
            0, // backlight register (PWM0_1)
            UEZ_LCD_BACKLIGHT_FULL_PERIOD,
            UEZ_LCD_BACKLIGHT_FULL_PWR_ON,
            UEZ_LCD_BACKLIGHT_FULL_PWR_OFF,
            GPIO_P3_11,
            EFalse
    };
    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO2_Require();
    LPC43xx_GPIO3_Require();

    LPC43xx_PWM0_A_Require(GPIO_P2_0);
    Backlight_Generic_PWMControlled_Create("Backlight", &settings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_LCD_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LCD device driver and its backlight
 *---------------------------------------------------------------------------*/
void UEZPlatform_LCD_Require(void)
{
    const T_LPC43xx_LCDController_Pins pins = {
            GPIO_NONE,  // LCD_PWR -- GPIO controlled
            GPIO_PZ_3_P4_7,  // LCD_DCLK
            GPIO_P2_6,  // LCD_ENAB_M
            GPIO_P2_5,  // LCD_FP
            GPIO_NONE,  // LCD_LE
            GPIO_P3_14,  // LCD_LP

            {
                GPIO_NONE,  // LCD_VD0
                GPIO_NONE,  // LCD_VD1
                GPIO_P2_3,  // LCD_VD2
                GPIO_P2_2,  // LCD_VD3
                GPIO_P4_7,  // LCD_VD4
                GPIO_P4_6,  // LCD_VD5
                GPIO_P4_5,  // LCD_VD6
                GPIO_P4_4,  // LCD_VD7

                GPIO_NONE,  // LCD_VD8
                GPIO_NONE,  // LCD_VD9
                GPIO_P5_14, // LCD_VD10
                GPIO_P5_13,  // LCD_VD11
                GPIO_P4_3,  // LCD_VD12
                GPIO_P5_26,  // LCD_VD13
                GPIO_P5_25,  // LCD_VD14
                GPIO_P5_24,  // LCD_VD15

                GPIO_NONE,  // LCD_VD16
                GPIO_NONE,  // LCD_VD17
                GPIO_P3_10,  // LCD_VD18
                GPIO_P3_9,  // LCD_VD19
                GPIO_P5_23,  // LCD_VD20
                GPIO_P5_22,  // LCD_VD21
                GPIO_P5_21,  // LCD_VD22
                GPIO_P5_20,  // LCD_VD23
            },

            GPIO_NONE,  // LCD_CLKIN
            GPIO_P3_15, //GPIO Power Control
            EFalse,
            0,

    };
    T_halWorkspace *p_lcdc;
    T_uezDeviceWorkspace *p_lcd;
    T_uezDevice backlight;
    T_uezDeviceWorkspace *p_backlight = 0;
    extern const T_uezDeviceInterface *UEZ_LCD_INTERFACE_ARRAY[];

    LPC43xx_GPIO2_Require();
    LPC43xx_GPIO4_Require();
    LPC43xx_GPIO5_Require();
    LPC43xx_GPIOZ_Require();

    UEZPlatform_Timer0_Require();

    UEZPlatform_Backlight_Require();

    LPC43xx_LCDController_Require(&pins);

    // Need to register LCD device and use existing LCD Controller
    UEZDeviceTableRegister(
            "LCD",
            (T_uezDeviceInterface *)UEZ_LCD_INTERFACE_ARRAY[UEZ_LCD_COLOR_DEPTH],
            0,
            &p_lcd);

    HALInterfaceFind("LCDController", &p_lcdc);
    UEZDeviceTableFind("Backlight", &backlight);
    UEZDeviceTableGetWorkspace(backlight, (T_uezDeviceWorkspace **)&p_backlight);
    ((DEVICE_LCD *)(p_lcd->iInterface))->Configure(
            p_lcd,
            (HAL_LCDController **)p_lcdc,
            LCD_DISPLAY_BASE_ADDRESS,
            (DEVICE_Backlight **)p_backlight);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Touchscreen_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the touchscreen and touch sensitivity for four wire resistive
 *---------------------------------------------------------------------------*/
void UEZPlatform_Touchscreen_Require(char *aName)
{
    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO0_Require();

    UEZPlatform_I2C0_Require();

    //PCAP IRQ, P6_6, GPIO0[5]
    UEZGPIOControl(GPIO_P0_5, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
    UEZGPIOSetMux(GPIO_P0_5, 0);
    UEZGPIOInput(GPIO_P0_5);

    Touchscreen_FT5306DE4_Create(aName, "I2C0", GPIO_P0_5, GPIO_P5_15);
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
    static const T_LPC43xx_Timer_Settings settings = {
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
    LPC43xx_Timer0_Require(&settings);
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
    static const T_LPC43xx_Timer_Settings settings = {
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
    LPC43xx_Timer1_Require(&settings);
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
    static const T_LPC43xx_Timer_Settings settings = {
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
    LPC43xx_Timer2_Require(&settings);
    Timer_Generic_Create("Timer2", "Timer2");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Timer3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Timer3 drivers, but don't set any capture sources or
 *      match registers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Timer3_Require(void)
{
    static const T_LPC43xx_Timer_Settings settings = {
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
    LPC43xx_Timer3_Require(&settings);
    Timer_Generic_Create("Timer3", "Timer3");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the PWM0 drivers
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM0_Require(void)
{
    DEVICE_CREATE_ONCE();

   LPC43xx_PWM0_Require();

   PWM_Generic_Create("PWM0", "PWM0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the PWM1 driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM1_Require(void)
{
    DEVICE_CREATE_ONCE();

   LPC43xx_PWM1_Require();

   PWM_Generic_Create("PWM1", "PWM1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the PWM1 driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM2_Require(void)
{
    DEVICE_CREATE_ONCE();

   LPC43xx_PWM2_Require();

   PWM_Generic_Create("PWM1", "PWM1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_DAC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the DAC0 device driver
 *      NOTE: Goes to
 *---------------------------------------------------------------------------*/
void UEZPlatform_DAC0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_GPIO2_Require();

    LPC43xx_DAC0_Require(GPIO_P2_4);
    DAC_Generic_Create("DAC0", "DAC0");
}

static T_uezDevice G_Amp;
/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_AudioAmp_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Audio Amp driver and set the default level
 *---------------------------------------------------------------------------*/
void UEZPlatform_AudioAmp_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    AudioAmp_LM48110_Create("Amp0", "I2C0", 0x1F);
    UEZAudioAmpOpen("Amp0", &G_Amp);
    UEZAudioAmpSetLevel(G_Amp, UEZ_DEFAULT_AUDIO_LEVEL);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI70WVT_AudioMixerCallback
 *---------------------------------------------------------------------------*
 * Description:
 *
 *---------------------------------------------------------------------------*/
static T_uezError UEZGUI43WQR_AudioMixerCallback(
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
                error = UEZAudioAmpSetLevel(G_Amp, aLevel);
            }
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
    UEZAudioMixerRegister(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, &UEZGUI43WQR_AudioMixerCallback);
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER, 255);
    UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_ONBOARD_SPEAKER);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EEPROM_I2C_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal EEPROM in the LPC43xx
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM_LPC43xx_Require(void)
{
    DEVICE_CREATE_ONCE();
    EEPROM_NXP_LPC43xx_Create("EEPROM0");
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
    LPC43xx_GPIO7_Require();
    LPC43xx_UART0_Require(GPIO_P7_24, GPIO_P7_25, GPIO_NONE, GPIO_NONE);
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
    // Standard Expansion board serial console is on UART0
    UEZPlatform_Console_FullDuplex_UART0_Require(aWriteBufferSize,
            aReadBufferSize);
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
    // UART0 on P7.24/P7.25
    LPC43xx_GPIO7_Require();
    LPC43xx_UART0_Require(GPIO_P7_24, GPIO_P7_25, GPIO_NONE, GPIO_NONE);
    Serial_Generic_FullDuplex_Stream_Create("UART0", "UART0",
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
    LPC43xx_GPIO6_Require();
    LPC43xx_UART1_Require(GPIO_P6_12, GPIO_P6_13, GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE);
    Serial_Generic_FullDuplex_Stream_Create("UART1", "UART1",
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
void UEZPlatform_FullDuplex_UART2_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    LPC43xx_GPIO4_Require();
    LPC43xx_UART2_Require(GPIO_P4_8, GPIO_P4_9, GPIO_NONE, GPIO_NONE);
    Serial_Generic_FullDuplex_Stream_Create("UART2", "UART2",
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
void UEZPlatform_FullDuplex_UART3_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
#if 1
    LPC43xx_GPIO5_Require();
    LPC43xx_UART3_Require(GPIO_P5_3, GPIO_P5_4, GPIO_NONE, GPIO_NONE, GPIO_NONE);
#else
    LPC43xx_GPIO4_Require();
    LPC43xx_GPIO5_Require();
    LPC43xx_UART3_Require(GPIO_P4_15, GPIO_P5_17, GPIO_NONE, GPIO_NONE, GPIO_NONE);
#endif

    Serial_Generic_FullDuplex_Stream_Create("UART3", "UART3",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SSP0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the SSP0 to be a SPI driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_SSP0_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO7_Require();
    LPC43xx_GPIOZ_Require();
    LPC43xx_SSP0_Require(GPIO_PZ_8_PF_0, GPIO_P7_17, GPIO_P7_18, GPIO_NONE);

    SPI_Generic_Create("SSP0", "SSP0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SSP1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the SSP1 to be a SPI driver.
 *      NOTE: On expansion header
 *---------------------------------------------------------------------------*/
void UEZPlatform_SSP1_Require(void)
{
    DEVICE_CREATE_ONCE();

    // SCK1 is P0.7, MISO1 is P0.8, MOSI1 is P0.9, SSEL1 is P0.6
    LPC43xx_GPIO0_Require();
    LPC43xx_GPIO7_Require();
    LPC43xx_GPIOZ_Require();
    LPC43xx_SSP1_Require(GPIO_PZ_0_P1_19, GPIO_P7_20, GPIO_P0_1, GPIO_NONE);

    SPI_Generic_Create("SSP1", "SSP1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Speaker_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Speaker device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_Speaker_Require(void)
{
    const T_ToneGenerator_Generic_Timer_Settings settings = {
            "Timer3",
            3, // MAT3[3]
            GPIO_P5_19,
            4, // 0 mux = GPIO
            1, // 3 mux = T3_MAT0
    };

    DEVICE_CREATE_ONCE();
    UEZPlatform_Timer3_Require();
    ToneGenerator_Generic_Timer_Create("Speaker", &settings);
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

    LPC43xx_RTC_Require(ETrue);
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

    UEZPlatform_I2C0_Require();
    Temp_NXP_LM75A_Create("Temp0", "I2C0", 0x92 >> 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Accel0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Accelerometer 0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Accel0_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();

    ST_Accelo_LIS3DH_I2C_Create("Accel0", "I2C0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EMAC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the EMAC controller for networking.
 *---------------------------------------------------------------------------*/
void UEZPlatform_EMAC_Require(void)
{
    // This EMAC is RMII (less pins)
    const T_LPC43xx_EMAC_Settings emacSettings = {
            GPIO_P0_1,          // ENET_TX_ENn      = PIN_P0_1_ENET_TXEN
            GPIO_NONE,          // ENET_TX_TXD[3]   = not used for RMII
            GPIO_NONE,          // ENET_TX_TXD[2]   = not used for RMII
            GPIO_P0_15,         // ENET_TX_TXD[1]   = PIN_P1_20_ENET_TXD1
            GPIO_P0_13,         // ENET_TX_TXD[0]   = PIN_P1_18_ENET_TXD0
            GPIO_NONE,          // ENET_TX_ER       = not used for RMII
            GPIO_PZ_0_P1_19,    // ENET_TX_CLK      = PIN_P1_19_ENET_REFCLK
            GPIO_P0_3,          // ENET_RX_DV       = ENET_RX_DV
            GPIO_NONE,          // ENET_RXD[3]      = not used for RMII
            GPIO_NONE,          // ENET_RXD[2]      = not used for RMII
            GPIO_P0_0,          // ENET_RXD[1]      = PIN_P0_0_ENET_RXD1
            GPIO_P0_2,          // ENET_RXD[0]      = PIN_P1_15_ENET_RXD0
            GPIO_P6_8,          // ENET_RX_ER       = PIN_P6_8_ENET_RX_ER
            GPIO_NONE,          // ENET_RX_CLK      = not used for RMII
            GPIO_NONE,          // ENET_COL         = not used for RMII
            GPIO_NONE,          // ENET_CRS         = not used for RMII
            GPIO_P6_0,          // ENET_MDC         = PIN_P6_0_ENET_MDC
            GPIO_P0_12,         // ENET_MDIO        = PIN_P1_17_ENET_MDIO
    };
    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO0_Require();
    LPC43xx_GPIO6_Require();
    LPC43xx_GPIOZ_Require();

    LPC43xx_EMAC_RMII_Require(&emacSettings);
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
    Network_lwIP_Create("WiredNetwork0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_PortA_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host on Port on USB1, off board USB
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_PortA_Require(void)
{
    extern void LPCUSBLib_USB1_IRQHandler(void);
    DEVICE_CREATE_ONCE();

    G_USBHostDriveNumber = 1;

    LPC43xx_GPIO5_Require();

    UEZGPIOSetMux(GPIO_P5_18, 2); // Turn on USB0_PPWR USB1
    InterruptRegister(USB1_IRQn, LPCUSBLib_USB1_IRQHandler,
        INTERRUPT_PRIORITY_NORMAL, "USB1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_PortB_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host on Port on USB0, on board USB
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_PortB_Require(void)
{
    extern void LPCUSBLib_USB0_IRQHandler(void);
    DEVICE_CREATE_ONCE();

    G_USBHostDriveNumber = 0;

    LPC43xx_GPIO3_Require();

    UEZGPIOSetMux(GPIO_P3_2, 1); // Turn on USB0_PPWR USB 0
    InterruptRegister(USB0_IRQn, LPCUSBLib_USB0_IRQHandler,
        INTERRUPT_PRIORITY_NORMAL, "USB0");
}

void UEZPlatform_MS0_Connected(void *aWorkspace)
{
    TUInt32 driveNum = G_ms0_driveNum;
    T_uezDevice ms0;
    T_uezDeviceWorkspace *p_ms0;

    printf("UEZGUI MS0 Connected drive %d\n", driveNum);
    UEZDeviceTableFind("MS0", &ms0);
    UEZDeviceTableGetWorkspace(ms0, (T_uezDeviceWorkspace **)&p_ms0);
    // Reregister the device (doing a mount)
    FATFS_RegisterMassStorageDevice(driveNum, (DEVICE_MassStorage **)p_ms0);
}

void UEZPlatform_MS0_Disconnected(void *aWorkspace)
{
    TUInt32 driveNum = G_ms0_driveNum;

    printf("UEZGUI MS0 Disconnected drive %d\n", driveNum);
    // Unregister the device (doing a unmount)
    FATFS_UnregisterMassStorageDevice(driveNum);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MS0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS0 using USBHost flash drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS0_Require(TUInt32 aUSBNumber)
{
    DEVICE_CREATE_ONCE();
    T_MassStorage_LPCUSBLib_Callbacks ms0Callbacks = {
            UEZPlatform_MS0_Connected,
            UEZPlatform_MS0_Disconnected
    };

    MassStorage_LPCUSBLib_Create("MS0", aUSBNumber, &ms0Callbacks, 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBFlash_Drive_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Flash drive using MS0 on the given drive number
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBFlash_Drive_Require(TUInt8 aDriveNum)
{
    T_uezDevice ms0;
    T_uezDeviceWorkspace *p_ms0;

    DEVICE_CREATE_ONCE();

    if(G_USBHostDriveNumber == 0xFF){
        UEZFailureMsg("Must Require Port A or B before the FlashDrive\n");
    }

    // Now put the MS0 in the filesystem using the USB
    UEZPlatform_MS0_Require(G_USBHostDriveNumber);
    UEZPlatform_FileSystem_Require();

    UEZDeviceTableFind("MS0", &ms0);
    UEZDeviceTableGetWorkspace(ms0, (T_uezDeviceWorkspace **)&p_ms0);
    FATFS_RegisterMassStorageDevice(aDriveNum, (DEVICE_MassStorage **)p_ms0);
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
    DEVICE_CREATE_ONCE();

    UEZPlatform_Touchscreen_Require("TouchscreenReal");
    //Touchscreen_Replay_Create("Touchscreen", "TouchscreenReal");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Watchdog_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the internal watchdog.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Watchdog_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC43xx_Watchdog_Require();
    Watchdog_Generic_Create("Watchdog", "Watchdog");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_WirelessNetwork0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wireless connections.
 *---------------------------------------------------------------------------*/
#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_CmdLib.h>
void UEZPlatform_WirelessNetwork0_Require(void)
{
    // These SPI pins work for both the Application Header and the PMOD Header
    // for GainSpan Modules with SPI configured.
    const T_GainSpan_Network_SPISettings spi_settings = {
    // SPI Settings
        "SSP0",// SSP Name
        3000, // 3 MHz
        // Pins.
        GPIO_P7_16, // iSPIChipSelect = WIFI_CSN = P2.23
        GPIO_P5_12, // iDataReadyIO = WIFI_INTR
        GPIO_NONE,  // iProgramMode = P1.6_MCIDAT0_PWM0.4
        GPIO_NONE,  // iSPIMode = WIFI_MODE
        GPIO_NONE,  // iSRSTn = WIFI_SRSTn AND'd with RESET_OUTn
        GPIO_NONE,  // iWIFIFactoryRestore = WIFI_FAC_RST
        };

    DEVICE_CREATE_ONCE();

    LPC43xx_GPIO5_Require();
    LPC43xx_GPIO7_Require();

    //Set CS to GPIO Outpit
    UEZGPIOControl(GPIO_P7_16, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
    UEZGPIOSetMux(GPIO_P7_16, 4);
    UEZGPIOSet(GPIO_P7_16);
    UEZGPIOOutput(GPIO_P7_16);

    //Set int INTR pin to an input.
    UEZGPIOControl(GPIO_P5_12, GPIO_CONTROL_SET_CONFIG_BITS, ((1 << 7) | (1 << 6)));
    UEZGPIOSetMux(GPIO_P5_12, 4);
    UEZGPIOInput(GPIO_P5_12);

    UEZPlatform_SSP0_Require();
    Network_GainSpan_Create("WirelessNetwork0", &spi_settings);
}

void UEZPlatform_USBHost_USB0_Serial_Require(void)
{
    //Stream_LPCUSBLib_SerialHost_Create("SerialHost", 1); // USB0
    UEZGPIOSetMux(GPIO_P3_2, 1); // Turn on USB0_PPWR USB 0
    Stream_LPCUSBLib_SerialHost_FTDI_Create("USB0Serial", 0, 1024, 1024); // USB0
}

void UEZPlatform_USBHost_USB1_Serial_Require(void)
{
    UEZGPIOSetMux(GPIO_P5_18, 2); // Turn on USB0_PPWR USB1
    Stream_LPCUSBLib_SerialHost_FTDI_Create("USB1Serial", 1, 1024, 1024); // USB1
}

void UEZPlatform_Standard_Require(void)
{
    UEZPlatform_I2C0_Require();

    UEZPlatform_LCD_Require();
    UEZPlatform_Touchscreen_Require("Touchscreen");

    //5v Monitor
    UEZPlatform_ADC0_7_Require();

    UEZPlatform_Flash0_Require();
    UEZPlatform_Timer0_Require();
    UEZPlatform_Timer1_Require();
    UEZPlatform_Timer2_Require();
    UEZPlatform_Timer3_Require();
    UEZPlatform_AudioMixer_Require();
    UEZAudioMixerMute(UEZ_AUDIO_MIXER_OUTPUT_MASTER);
    UEZPlatform_Speaker_Require();
    UEZPlatform_DAC0_Require();
    UEZPlatform_EEPROM_LPC43xx_Require();
    UEZPlatform_Temp0_Require();
    UEZPlatform_Accel0_Require();

    UEZPlatform_Console_FullDuplex_UART0_Require(1024, 1024);
    UEZPlatform_IRTC_Require();
}

TBool UEZPlatform_Host_Port_B_Detect(void)
{
    TBool IsDevice;

    UEZGPIOSetMux(GPIO_P3_2, 0);
    UEZGPIOSet(GPIO_P3_2); // disable MIC2025
    UEZGPIOOutput(GPIO_P3_2);

    IsDevice = (LPC_USB0->OTGSC & (1 << 8))? ETrue : EFalse;

    if(!IsDevice){
        UEZGPIOClear(GPIO_P3_2);// enable MIC2025
    }
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

T_pixelColor SUICallbackRGBConvert(int r, int g, int b)
{
    return RGB(r, g, b);
}

TUInt32 UEZPlatform_GetBaseAddress(void)
{
    return 0x28000000;//LCD_DISPLAY_BASE_ADDRESS;
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
    LPC_LCD->UPBASE = (TUInt32)aAddress;
}

TUInt32 SUICallbackGetLCDBase()
{
    return LPC_LCD->UPBASE;
}

void WriteByteInFrameBufferWithAlpha(UNS_32 aAddr, COLOR_T aPixel, T_swimAlpha aAlpha)
{
    static COLOR_T mask = ~((1<<10)|(1<<5)|(1<<0));
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
 *  @return         int
 */
/*---------------------------------------------------------------------------*/
#if (COMPILER_TYPE==Keil4)
__asm void vMainMPUFaultHandler( unsigned long * pulFaultRegisters ) {
    tst lr, #4
    ite eq
    mrseq r0, msp /* The code that generated the exception was using the main stack. */
    mrsne r0, psp /* The code that generated the exception was using the process stack. */
    ldr r0, [r0, #24]   /* Extract the value of the stacked PC. */
    str r0, [sp]    /* Store the value of the stacked PC into ulStacked_pc. */

    /* Inspect ulStacked_pc to locate the offending instruction. */
loopforever
    bl loopforever
}
#else
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
#endif

/*---------------------------------------------------------------------------*
 * Routine:  main
 *---------------------------------------------------------------------------*/
/**
 *  The main() routine in UEZ is only a stub that is used to start
 *      the whole UEZ system.  UEZBSP_Startup() is immediately called.
 *
 *  @return         int
 */
/*---------------------------------------------------------------------------*/
int main(void)
{
    UEZBSP_Startup();

    while (1) {
    } // never should get here
}

/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform.c
 *-------------------------------------------------------------------------*/