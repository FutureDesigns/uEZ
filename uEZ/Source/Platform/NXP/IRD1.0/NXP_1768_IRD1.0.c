/*-------------------------------------------------------------------------*
 * File:  uEZPlatform.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the uEZGUI-1788-43WQS
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <Config.h>
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <HAL/HAL.h>
#include <HAL/GPIO.h>
#include <HAL/EMAC.h>
#include <HAL/Interrupt.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_USBDeviceController.h>
#include <Source/Devices/Accelerometer/Freescale/MMA7455/Freescale_MMA7455.h>
#include <Source/Devices/ADC/Generic/Generic_ADC.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>
#include <Source/Devices/Backlight/Generic/BacklightPWMControlled/BacklightPWM.h>
#include <Source/Devices/Button/NXP/PCA9551/Button_PCA9551.h>
#include <Source/Devices/CharDisplay/Lumex/SO2004D/Lumex_SO2004DSR.h>
#include <Source/Devices/DAC/Generic/DAC_Generic.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM_Generic_I2C.h>
#include <Source/Devices/EEPROM/Generic/I2C/EEPROM16_Generic_I2C.h>
#include <Source/Devices/ExternalInterrupt/NXP/LPC1768/ExternalInterrupt_NXP_LPC1768.h>
#include <Source/Devices/Flash/Spansion/S29GL/Flash_S29GL064N90_16bit.h>
#include <Source/Devices/GPDMA/Generic/Generic_GPDMA.h>
#include <Source/Devices/HID/Generic/HID_Generic.h>
#include <Source/Devices/I2C/Generic/Generic_I2C.h>
#include <Source/Devices/I2S/Generic/Generic_I2S.h>
#include <Source/Devices/Keypad/NXP/I2C_Keypad/Keypad_NXP_I2C_PCA9555.h>
#include <Source/Devices/LED/NXP/PCA9551/LED_NXP_PCA9551.h>
#include <Source/Devices/MassStorage/USB_MS/USB_MS.h>
#include <Source/Devices/Network/lwIP/Network_lwIP.h>
#include <Source/Devices/PWM/Generic/Generic_PWM.h>
#include <Source/Devices/RTC/Generic/Generic_RTC.h>
#include <Source/Devices/RTC/NXP/PCF8563/RTC_PCF8563.h>
#include <Source/Devices/Serial/Generic/Generic_Serial.h>
#include <Source/Devices/SPI/Generic/Generic_SPI.h>
#include <Source/Devices/Temperature/NXP/SA56004X/Temperature_SA56004X.h>
#include <Source/Devices/Timer/Generic/Timer_Generic.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>
#include <Source/Devices/USBDevice/NXP/LPC1768/LPC1768_USBDevice.h>
#include <Source/Devices/USBHost/Generic/Generic_USBHost.h>
#include <Source/Devices/Watchdog/Generic/Watchdog_Generic.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_RTC.h>
#include <Source/Library/Web/BasicWeb/BasicWEB.h>
#include <Source/Library/FileSystem/FATFS/uEZFileSystem_FATFS.h>
#include <Source/Library/Memory/MemoryTest/MemoryTest.h>
#include <Source/Library/StreamIO/StdInOut/StdInOut.h>
#include <Source/Processor/NXP/LPC1768/uEZProcessor_LPC1768.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_ADCBank.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_DAC.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_EMAC.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_GPDMA.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_GPIO.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_I2C.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_I2S.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_PWM.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_PLL.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_Serial.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_SSP.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_Timer.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_USBDeviceController.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_USBHost.h>
#include <Source/Processor/NXP/LPC1768/LPC1768_Watchdog.h>
#include <uEZBSP.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>
#include <uEZFile.h>
#include <uEZI2C.h>
#include <uEZNetwork.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>
#include <uEZStream.h>

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

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#if(COMPILER_TYPE==Keil4)
#define nop()      __nop()
#elif(COMPILER_TYPE==RowleyARM)
#include <intrinsics.h>
#define nop()     __asm__("nop")
#else
#define nop()     NOP()
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
#if ( PROCESSOR_OSCILLATOR_FREQUENCY == 100000000)
    // 100 nops -- estimate of 1 US delay
    nops50();
    nops50();
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
    TUInt32 i;

    // Approximate delays here
    for (i = 0; i < 1000; i++)
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
    // None on this memory configuration
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
    // NOTE: The current implementation of the LPC1768 is hardcoded to 100MHz
    const T_LPC1768_PLL_Frequencies freq_CPU100MHz_Peripheral100Mhz_USB48MHz = {
            10000000,

            // Run PLL0 at 100 MHz
            100000000,

            // Run PLL1 at 48 MHz (for USB)
            48000000,

            // Use the main oscillator of 12 MHz as a source
            LPC1768_CLKSRC_SELECT_MAIN_OSCILLATOR,

            // Use PLL0 for the CCLK, PCLK, and EMC Clk source (before dividers)
            LPC1768_CPU_CLOCK_SELECT_PLL_CLK,

            // Use PPL1 (alt) for the USB
            LPC1768_USB_CLOCK_SELECT_ALT_PLL_CLK,

            // CPU Clock is PLL0 / 1 or 100 MHz / 1 = 100 MHz
            1,

            // PCLK is PLL0 / 2, or 50 MHz
            1,

            // EMC runs at PLL0 / 2, or 50 MHz (not used)
            2,

            // USB Clock = PLL1 / 1 (48 MHz / 1)
            1,

            // CLKOUT is on the EMC CLK and at 50 MHz (50 MHz / 1)
            LPC1768_CLOCK_OUT_SELECT_CPU,
            1,
            ETrue, };
    LPC1768_PLL_SetFrequencies(&freq_CPU100MHz_Peripheral100Mhz_USB48MHz);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_InterruptsReset
 *---------------------------------------------------------------------------*
 * Description:
 *      Do the first initialization of the interrupts.
 *---------------------------------------------------------------------------*/
void UEZBSP_InterruptsReset(void)
{
    InterruptsReset();
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
 * Routine:  UEZPlatform_LED_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the LED Bank device driver to go to I2C address 0xC0
 *---------------------------------------------------------------------------*/
void UEZPlatform_LED_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    LED_NXP_PCA9551_Create("LEDBank0", "I2C0", 0xC0>>1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Button_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Button Bank device driver to go to I2C address 0xC0
 *---------------------------------------------------------------------------*/
void UEZPlatform_Button_Require(void)
{
    DEVICE_CREATE_ONCE();

    ButtonBank_NXP_PCA9551_Create("ButtonBank0", "I2C0", 0xC0 >> 1);
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

    // Ensure the I2C0 exists in the HAL level
    LPC1768_GPIO0_Require();
    LPC1768_I2C0_Require(GPIO_P0_27, GPIO_P0_28);
    I2C_Generic_Create("I2C0", "I2C0", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2C1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C1 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2C1_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Ensure the I2C0 exists in the HAL level
    LPC1768_GPIO2_Require();
    LPC1768_I2C1_Require(GPIO_P0_19, GPIO_P0_20);
    I2C_Generic_Create("I2C1", "I2C1", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_TempLocal_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Temperature (local) device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_TempLocal_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    Temp_NXP_SA56004X_Local_Create("TempLocal", "I2C0", 0x96 >> 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_TempRemote_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Temperature (remote) device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_TempRemote_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    Temp_NXP_SA56004X_Remote_Create("TempRemote", "I2C0", 0x96 >> 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_GPDMAx_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the General Purpose DMA drivers.
 *---------------------------------------------------------------------------*/
void UEZPlatform_GPDMA0_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC1768_GPDMA0_Require();
    GPDMA_Generic_Create("GPDMA0", "GPDMA0");
}

void UEZPlatform_GPDMA1_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC1768_GPDMA1_Require();
    GPDMA_Generic_Create("GPDMA1", "GPDMA1");
}

void UEZPlatform_GPDMA2_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC1768_GPDMA2_Require();
    GPDMA_Generic_Create("GPDMA2", "GPDMA2");
}

void UEZPlatform_GPDMA3_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC1768_GPDMA3_Require();
    GPDMA_Generic_Create("GPDMA3", "GPDMA3");
}

void UEZPlatform_GPDMA4_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC1768_GPDMA4_Require();
    GPDMA_Generic_Create("GPDMA4", "GPDMA4");
}

void UEZPlatform_GPDMA5_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC1768_GPDMA5_Require();
    GPDMA_Generic_Create("GPDMA5", "GPDMA5");
}

void UEZPlatform_GPDMA6_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC1768_GPDMA6_Require();
    GPDMA_Generic_Create("GPDMA6", "GPDMA6");
}

void UEZPlatform_GPDMA7_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC1768_GPDMA7_Require();
    GPDMA_Generic_Create("GPDMA7", "GPDMA7");
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
 * Routine:  UEZPlatform_Console_HalfDuplex_UART_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a half duplex UART.
 * Inputs:
 *      const char *aHALSerialName -- Name of UART serial driver to use
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *      T_uezGPIOPortPin aDriveEnablePortPin -- GPIO for drive enable
 *      TBool aDriveEnablePolarity -- ETrue for high true, else EFalse
 *      TUInt32 aDriveEnableReleaseTime -- Milliseconds to wait before
 *          changing drive enable direction.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_HalfDuplex_UART_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize,
        T_uezGPIOPortPin aDriveEnablePortPin,
        TBool aDriveEnablePolarity,
        TUInt32 aDriveEnableReleaseTime)
{
    DEVICE_CREATE_ONCE();
    // NOTE: This require routine does NOT require the HAL driver
    // automatically!
    // NOTE: This require routine does NOT require the GPIO driver
    // needed for the GPIO drive enable port pin!
    Serial_Generic_HalfDuplex_Stream_Create("Console", aHALSerialName,
            aWriteBufferSize, aReadBufferSize, aDriveEnablePortPin,
            aDriveEnablePolarity, aDriveEnableReleaseTime);
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
    // UART0 on P0.2/P0.3
    LPC1768_GPIO0_Require();
    LPC1768_UART0_Require(GPIO_P0_2, GPIO_P0_3);
    UEZPlatform_Console_FullDuplex_UART_Require("UART0", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART1.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART1_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART1 on P0.15/P0.16
    LPC1768_GPIO0_Require();
    LPC1768_UART1_Require(GPIO_P0_15, GPIO_P0_16, GPIO_NONE, GPIO_NONE,
            GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE);
    UEZPlatform_Console_FullDuplex_UART_Require("UART1", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_FullDuplex_UART1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART1.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_FullDuplex_UART1_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART1 on P0.15/P0.16
    LPC1768_GPIO0_Require();
    LPC1768_UART1_Require(GPIO_P0_15, GPIO_P0_16, GPIO_NONE, GPIO_NONE,
            GPIO_NONE, GPIO_NONE, GPIO_NONE, GPIO_NONE);
    Serial_Generic_FullDuplex_Stream_Create("UART1", "UART1",
            aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART2.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART2_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART2 on P0.10/P0.11
    LPC1768_GPIO0_Require();
    LPC1768_UART2_Require(GPIO_P0_10, GPIO_P0_11);
    UEZPlatform_Console_FullDuplex_UART_Require("UART2", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_UART3_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console using a full duplex UART using UART3.
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_FullDuplex_UART3_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // UART3 on P0.0/P0.1
    LPC1768_GPIO0_Require();
    LPC1768_UART3_Require(GPIO_P0_0, GPIO_P0_1);
    UEZPlatform_Console_FullDuplex_UART_Require("UART3", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_FullDuplex_ISPHeader_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console to be a full duplex UART on the ISP header
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_ISPHeader_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // Standard ISP header is on UART0
    UEZPlatform_Console_FullDuplex_UART0_Require(aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_Expansion_ISPHeader_Require
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
 * Routine:  UEZPlatform_ADC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_ADC0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC1768_ADC0_Require();
    ADCBank_Generic_Create("ADC0", "ADC0");
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
    LPC1768_ADC0_0_Require(GPIO_P0_23);
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
    LPC1768_ADC0_1_Require(GPIO_P0_24);
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
    LPC1768_ADC0_2_Require(GPIO_P0_25);
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
    LPC1768_ADC0_3_Require(GPIO_P0_26);
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
    LPC1768_ADC0_4_Require(GPIO_P1_30);
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
    LPC1768_ADC0_5_Require(GPIO_P1_31);
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
    LPC1768_ADC0_6_Require(GPIO_P0_3);
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
    LPC1768_ADC0_7_Require(GPIO_P0_2);
    UEZPlatform_ADC0_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_DAC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the DAC0 device driver
 *      NOTE: Goes to P0.26_AD03_AOUT_RD3
 *---------------------------------------------------------------------------*/
void UEZPlatform_DAC0_Require(void)
{
    DEVICE_CREATE_ONCE();
    LPC1768_DAC0_Require(GPIO_P0_26);
    DAC_Generic_Create("DAC0", "DAC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Flash0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Flash0 device driver for the external NOR Flash
 *---------------------------------------------------------------------------*/
void UEZPlatform_Flash0_Require(void)
{
    DEVICE_CREATE_ONCE();
    Flash_S29GL064N90_16Bit_Create("Flash0", NOR_FLASH_BASE_ADDR);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Speaker_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Speaker device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_Speaker_Require(void)
{
    const T_ToneGenerator_Generic_PWM_Settings settings = {
            "PWM1",
            6, // PWM1[6]
            GPIO_P1_26,
            0, // 0 mux = GPIO
            2, // 2 mux = PWM1_6
    };
    DEVICE_CREATE_ONCE();

    LPC1768_GPIO1_Require();
    LPC1768_PWM1_6_Require(GPIO_P1_26);

    // Speaker is on PWM1[2]
    ToneGenerator_Generic_PWM_Create("Speaker", &settings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_IRTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the external RTC device driver.  Not compatible with the
 *      internal RTC.
 *---------------------------------------------------------------------------*/
void UEZPlatform_ERTC_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    RTC_PCF8563_Create("RTC", "I2C0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_RTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RTC device driver and check it for validity
 *---------------------------------------------------------------------------*/
void UEZPlatform_RTC_Require(void)
{
    T_uezDevice rtcDev;
    DEVICE_RTC **p_rtcDev;
    const T_uezTimeDate resetTD = {
        {   0, 0, 0}, // 12:00 midnight
        {   1, 1, 2009}, // Jan 1, 2009
    };
    T_uezError error;

    // Choose one:
    //UEZPlatform_IRTC_Require();
    UEZPlatform_ERTC_Require();

    // Make sure the RTC has valid data
    printf("Checking RTC ...\n");
    UEZDeviceTableFind("RTC", &rtcDev);
    UEZDeviceTableGetWorkspace(rtcDev, (T_uezDeviceWorkspace **)&p_rtcDev);
    error = (*p_rtcDev)->Validate(p_rtcDev, &resetTD);
    if (error == UEZ_ERROR_INVALID) {
        printf("RTC was invalid, reset to 01/01/2009 12:00:00\n");
    } else if (error != UEZ_ERROR_NONE) {
        printf("RTC error #%d!  Could not check validity!\n", error);
    } else {
        printf("RTC OK\n");
    }

#if 0 // Test code for doing Clock out
    // Make sure the clock out is disabled
    error = (*p_rtcDev)->SetClockOutHz(p_rtcDev, 32768);
    if (error == UEZ_ERROR_NONE) {
        printf("RTC Clock out disabled\n");
    } else {
        printf("RTC Clock out error #%d!\n", error);
    }
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EEPROM_I2C_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the external EEPROM over I2C.
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM_I2C_Require(void)
{
    const T_EEPROMConfig eeprom_config = {EEPROM_CONFIG_PCF8582C_2};

    DEVICE_CREATE_ONCE();
    UEZPlatform_I2C0_Require();
    EEPROM16_Generic_I2C_Create("EEPROM0", "I2C0", 0xA0>>1, &eeprom_config);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_EEPROM0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the default EEPROM0 driver (usually internal LPC1768)
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM0_Require(void)
{
    // Pick one of these two for the type of EEPROM
    UEZPlatform_EEPROM_I2C_Require();
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
    LPC1768_Watchdog_Require();
    Watchdog_Generic_Create("Watchdog", "Watchdog");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_PortA_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host on Port A (expansion board).
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_PortA_Require(void)
{
    const T_LPC1768_USBHost_Settings portASettings = {
            GPIO_P0_29,     // USB_D+1          = P0.29_USB1_DP_EINT0
            GPIO_P0_30,     // USB_D-1          = P0.30_USB1_DM_EINT1
            GPIO_NONE,      // USB_CONNECT1n
            GPIO_NONE,      // USB_UP_LED1
            GPIO_NONE,      // USB_INT1n
            GPIO_NONE,      // USB_SCL1
            GPIO_NONE,      // USB_SDA1
            GPIO_NONE,      // USB_TX_E
            GPIO_NONE,      // USB_TX_DP
            GPIO_NONE,      // USB_TX_DM
            GPIO_NONE,      // USB_RCV
            GPIO_NONE,      // USB_RX_DP
            GPIO_NONE,      // USB_RX_DM
            GPIO_NONE,      // USB_LS1
            GPIO_NONE,      // USB_SSPND1
            GPIO_P1_19,     // USB_PPWR1        = USB1H_PPWR
            GPIO_NONE,      // USB_PWRD1        = USB1H_PWRD
            GPIO_NONE,      // USB_OVRCR1       = USB1H_OVC
            GPIO_NONE,      // USB_HSTEN1
    };
    DEVICE_CREATE_ONCE();

    LPC1768_USBHost_PortA_Require(&portASettings);
    USBHost_Generic_Create("USBHost", "USBHost:PortA");
    // NOTE: USB1H_PWRD is on GPIO_P4_26 and is manually controlled
    // NOTE: USB1H_OVR  is on GPIO_P4_27 and is manually controlled
    LPC1768_GPIO4_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBHost_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Host.
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_Require(void)
{
    // If expansion board, do only Port A (expansion board).
    UEZPlatform_USBHost_PortA_Require();
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
 * Routine:  UEZPlatform_MS0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS0 using USBHost flash drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS0_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_USBHost_Require();
    MassStorage_USB_Create("MS0", "USBHost");
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

    // Now put the MS0 in the filesystem using the USB
    UEZPlatform_USBHost_Require();
    UEZPlatform_MS0_Require();
    UEZPlatform_FileSystem_Require();

    UEZDeviceTableFind("MS0", &ms0);
    UEZDeviceTableGetWorkspace(ms0, (T_uezDeviceWorkspace **)&p_ms0);
    FATFS_RegisterMassStorageDevice(aDriveNum, (DEVICE_MassStorage **)p_ms0);
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

   LPC1768_PWM0_Require();
   // Current UEZGUI does not use these, but may need one later
   //LPC1768_PWM0_1_Require(pin?);
   //LPC1768_PWM0_2_Require(pin?);
   //LPC1768_PWM0_3_Require(pin?);
   //LPC1768_PWM0_4_Require(pin?);
   //LPC1768_PWM0_5_Require(pin?);
   //LPC1768_PWM0_6_Require(pin?);
   //LPC1768_PWM0_CAP0_Require(pin?);
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

   LPC1768_PWM1_Require();
   // Current UEZGUI does not use these, but may need one later
   //LPC1768_PWM1_1_Require(pin?);
   //LPC1768_PWM1_2_Require(pin?);
   //LPC1768_PWM1_3_Require(pin?);
   //LPC1768_PWM1_4_Require(pin?);
   //LPC1768_PWM1_5_Require(pin?);
   //LPC1768_PWM1_6_Require(pin?);
   //LPC1768_PWM1_CAP0_Require(pin?);
   //LPC1768_PWM1_CAP1_Require(pin?);
   PWM_Generic_Create("PWM1", "PWM1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBDevice_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USBDevice controller.
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBDevice_Require(void)
{
    const T_LPC1768_USBDevice_Settings settings = {
            GPIO_P1_30,     // USB_VBUS        = P1.30_VBUS
            GPIO_P0_29,     // USB_D+          = P0.29_USB_D+
            GPIO_P0_30,     // USB_D-          = P0.30_USB_D-
            GPIO_P2_9,      // USB_CONNECT     = P2.9_USB_CONNECT
            GPIO_P1_18,     // USB_UP_LED      = P1_18-USB_UP_LED
    };
    DEVICE_CREATE_ONCE();

    LPC1768_GPIO0_Require();
    LPC1768_GPIO1_Require();
    LPC1768_GPIO2_Require();

    // NOTE: There is a conflict of the USBDevice and the LCD display
    // We'll override it here
    UEZGPIOUnlock(GPIO_P2_9);
    
    LPC1768_USBDevice_Require(&settings);
    USBDevice_LPC1768_Create("USBDevice", "USBDeviceController");
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
    const T_LPC1768_EMAC_Settings emacSettings = {
            GPIO_P1_4,      // ENET_TX_ENn      = P1.4_ENET_TXEN
            GPIO_NONE,      // ENET_TX_TXD[3]   = not used for RMII
            GPIO_NONE,      // ENET_TX_TXD[2]   = not used for RMII
            GPIO_P1_1,      // ENET_TX_TXD[1]   = P1.1_ENET_TXD1
            GPIO_P1_0,      // ENET_TX_TXD[0]   = P1.0_ENET_TXD0
            GPIO_NONE,      // ENET_TX_ER       = not used for RMII
            GPIO_NONE,      // ENET_TX_CLK      = not used for RMII
            GPIO_NONE,      // ENET_RX_DV       = not used for RMII
            GPIO_NONE,      // ENET_RXD[3]      = not used for RMII
            GPIO_NONE,      // ENET_RXD[2]      = not used for RMII
            GPIO_P1_10,     // ENET_RXD[1]      = P1.10_ENET_RXD1
            GPIO_P1_9,      // ENET_RXD[0]      = P1.9_ENET_RXD0
            GPIO_P1_14,     // ENET_RX_ER       = P1.14_ENET_RX_ER
            GPIO_P1_15,     // ENET_REFCLK      = P1.15_ENET_REFCLK
            GPIO_NONE,      // ENET_COL         = not used for RMII
            GPIO_P1_8,      // ENET_CRS         = P1.8_ENET_CRSDV
            GPIO_P1_16,     // ENET_MDC         = P1.16_ENET_MDC
            GPIO_P1_17,     // ENET_MDIO        = P1.17_ENET_MDIO
    };
    DEVICE_CREATE_ONCE();

    LPC1768_EMAC_RMII_Require(&emacSettings);
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
    static const T_LPC1768_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            },
    };
    DEVICE_CREATE_ONCE();
    LPC1768_Timer0_Require(&settings);
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
    static const T_LPC1768_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC1768_Timer1_Require(&settings);
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
    static const T_LPC1768_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC1768_Timer2_Require(&settings);
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
    static const T_LPC1768_Timer_Settings settings = {
            {
            GPIO_NONE,      // T0_CAP[0]
            GPIO_NONE,      // T0_CAP[1]
            },
            {
            GPIO_NONE,      // T0_MAT[0]
            GPIO_NONE,      // T0_MAT[1]
            GPIO_NONE,      // T0_MAT[2]
            GPIO_NONE,      // T0_MAT[3]
            }
    };
    DEVICE_CREATE_ONCE();
    LPC1768_Timer3_Require(&settings);
    Timer_Generic_Create("Timer3", "Timer3");
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
 * Routine:  UEZPlatform_I2S_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2S drivers for talking to the expansion board.
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2S_Require(void)
{
    static const T_LPC1768_I2S_Settings settings = {
            GPIO_P0_6,  // I2S_RX_SDA   = P0.6_I2SRX_SDA_SSEL1_MAT2.0
            GPIO_P0_4,  // I2S_RX_SCK   = P0.4_I2SRX_CLK_RD2_CAP2.0
            GPIO_P0_5,  // I2S_RX_WS    = P0.5_I2SRX_WS_TD2_CAP2.1
            GPIO_NONE,  // I2S_RX_MCLK  = not used

            GPIO_P0_9,  // I2S_TX_SDA   = P0.9_I2STX_SDA_MOSI1_MAT2.3
            GPIO_P0_7,  // I2S_TX_SCK   = P0.7_I2STX_CLK_SCK1_MAT2.1
            GPIO_P0_8,  // I2S_TX_WS    = P0.8_I2STX_WS_MISO1_MAT2.2
            GPIO_NONE,  // I2S_TX_MCLK  = not used
    };
    DEVICE_CREATE_ONCE();
    LPC1768_I2S_Require(&settings);
    Generic_I2S_Create("I2S", "I2S");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Keypad_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Keypad driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_Keypad_Require(void)
{
    DEVICE_CREATE_ONCE();
    UEZPlatform_I2C0_Require();
    //UEZPlatform_ExternalInterrupts_Require();
    Keypad_NXP_PCA9555_Create("Keypad", "I2C0", 0x40 >> 1, "ExternalInterrupts",
        2);
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

    // There are several options:
    // SCK0 can be one of: P1.20
    // MISO0 can be one of: P0.17, P1.23
    // MOSI0 can be one of: P0.18, P1.24
    LPC1768_GPIO0_Require();
    LPC1768_GPIO1_Require();
    LPC1768_GPIO2_Require();
    LPC1768_SSP0_Require(GPIO_P1_20, GPIO_P0_17, GPIO_P0_18, GPIO_NONE);

    SPI_Generic_Create("SSP0", "SSP0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_CharDisplay_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the CharDisplay
 *---------------------------------------------------------------------------*/
void UEZPlatform_CharDisplay_Require(void)
{
    DEVICE_CREATE_ONCE();

    LPC1768_GPIO0_Require();
    LPC1768_GPIO2_Require();
    Lumex_SO2004DSR_Create("CharDisplay", GPIO_P2_10, GPIO_P0_22, GPIO_P2_8,
        GPIO_P2_0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ExternalInterrupts_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the external interrupts driver
 *---------------------------------------------------------------------------*/
//void UEZPlatform_ExternalInterrupts_Require(void)
//{
//    DEVICE_CREATE_ONCE();
//
//    LPC1768_ExternalInterrupts_Require();
//    ExternalInterrupt_Generic_Create("ExternalInterrupts");
//}

/*---------------------------------------------------------------------------*
 * Routine:  uEZPlatformInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the board with all the proper settings.
 *      Registers all peripherals specific to this board.
 *---------------------------------------------------------------------------*/
void uEZPlatformInit(void)
{
    // Do any initialiation necessary before the RTOS is started
}

void UEZPlatform_Standard_Require(void)
{
    // Setup console immediately
    UEZPlatform_Console_Expansion_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
    UEZPlatform_CharDisplay_Require();
    UEZPlatform_LED_Require();
    UEZPlatform_Keypad_Require();
    UEZPlatform_FullDuplex_UART1_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
    UEZPlatform_I2C0_Require();
    UEZPlatform_I2C1_Require();
    UEZPlatform_TempLocal_Require();
    UEZPlatform_TempRemote_Require();
    UEZPlatform_GPDMA0_Require();
    UEZPlatform_GPDMA1_Require();
    UEZPlatform_ADC0_0_Require();
    UEZPlatform_ADC0_1_Require();
    UEZPlatform_ADC0_2_Require();
    UEZPlatform_DAC0_Require();
    UEZPlatform_Flash0_Require();
    UEZPlatform_Speaker_Require();
    UEZPlatform_EEPROM0_Require();
    UEZPlatform_Button_Require();

    UEZPlatform_RTC_Require();
}

void UEZPlatform_Full_Require(void)
{
    // Setup console immediately
    UEZPlatform_Console_Expansion_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
    UEZPlatform_CharDisplay_Require();
    UEZPlatform_LED_Require();
    UEZPlatform_Keypad_Require();
    UEZPlatform_FullDuplex_UART1_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);

    LPC1768_GPIO0_Require();
    LPC1768_GPIO1_Require();
    LPC1768_GPIO2_Require();
    LPC1768_GPIO3_Require();
    LPC1768_GPIO4_Require();
    UEZPlatform_I2C0_Require();
    UEZPlatform_I2C1_Require();
    UEZPlatform_TempLocal_Require();
    UEZPlatform_TempRemote_Require();
    UEZPlatform_GPDMA0_Require();
    UEZPlatform_GPDMA1_Require();
    UEZPlatform_GPDMA2_Require();
    UEZPlatform_GPDMA3_Require();
    UEZPlatform_GPDMA4_Require();
    UEZPlatform_GPDMA5_Require();
    UEZPlatform_GPDMA6_Require();
    UEZPlatform_GPDMA7_Require();
    UEZPlatform_Timer0_Require();
    UEZPlatform_Timer1_Require();
    UEZPlatform_Timer2_Require();
    UEZPlatform_Timer3_Require();
    UEZPlatform_ADC0_0_Require();
    UEZPlatform_ADC0_1_Require();
    UEZPlatform_ADC0_2_Require();
    UEZPlatform_DAC0_Require();
    UEZPlatform_Flash0_Require();
    UEZPlatform_Speaker_Require();
    UEZPlatform_EEPROM0_Require();
    UEZPlatform_USBHost_Require();
    UEZPlatform_USBFlash_Drive_Require(0);
    UEZPlatform_Watchdog_Require();
    UEZPlatform_USBDevice_Require();
    UEZPlatform_WiredNetwork0_Require();
    UEZPlatform_Button_Require();

    UEZPlatform_RTC_Require();
}

void UEZPlatform_Minimal_Require(void)
{
    // Setup console immediately
    UEZPlatform_Console_Expansion_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
}

void UEZPlatform_Require(void)
{
    // Choose one:
    //UEZPlatform_Minimal_Require();
    UEZPlatform_Standard_Require();
    //UEZPlatform_Full_Require();
}

TUInt32 UEZPlatform_ProcessorGetFrequency(void)
{
    return PROCESSOR_OSCILLATOR_FREQUENCY;
}
TUInt32 UEZPlatform_GetPCLKFrequency(void)
{
    return 100000000;
}
TUInt32 UEZPlatform_SerialGetDefaultBaud(void)
{
    return SERIAL_PORTS_DEFAULT_BAUD;
}
#if 0
TUInt16 UEZPlatform_LCDGetHeight(void)
{
  return 0;
}
TUInt16 UEZPlatform_LCDGetWidth(void)
{
  return 0;
}
TUInt32 UEZPlatform_LCDGetFrame(TUInt16 aFrameNum)
{
  return 0;
}
TUInt32 UEZPlatform_LCDGetFrameBuffer(void)
{
  return 0;
}
TUInt32 UEZPlatform_LCDGetFrameSize(void)
{
  return 0;
}
TUInt32 UEZPlatform_GetBaseAddress(void)
{
    return 0;
}

void *SUICallbackGetLCDBase(void)
{
    return 0;
}
void SUICallbackSetLCDBase(void *aAddress)
{
    return;
}
#endif

#if (COMPILER_TYPE==Keil4)
__asm void vMainMPUFaultHandler( unsigned long * pulFaultRegisters )
{
    /* Determine which stack was in use when the MPU fault occurred and extract
    the stacked PC. */
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
void vMainMPUFaultHandler( unsigned long * pulFaultRegisters )
{
unsigned long ulStacked_pc = 0UL;

    ( void ) ulStacked_pc;

    /* Determine which stack was in use when the MPU fault occurred and extract
    the stacked PC. */
    __asm
    (
        "   tst lr, #4          \n"
        "   ite eq              \n"
        "   mrseq r0, msp       \n" /* The code that generated the exception was using the main stack. */
        "   mrsne r0, psp       \n" /* The code that generated the exception was using the process stack. */
        "   ldr r0, [r0, #24]   \n" /* Extract the value of the stacked PC. */
        "   str r0, [sp]        \n" /* Store the value of the stacked PC into ulStacked_pc. */
    );

    /* Inspect ulStacked_pc to locate the offending instruction. */
    for( ;; );
}
#endif

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
#if (COMPILER_TYPE==IAR)
void UEZBSP_FatalError(int aNumBlinks)
#else
void UEZBSP_FatalError(TUInt32 aNumBlinks)
#endif
{
    register TUInt32 count;
    // Ensure interrupts are turned off
    portDISABLE_INTERRUPTS();

    // Configure status led to be fully in our control
    // Make P0.7 be a GPIO pin
    PINCON->PINSEL0 &= ~(3<<14);
    // and an output pin
    GPIO0->FIODIR |= (1<<7);

    // Blink our status led in a pattern, forever
    count = 0;
    while (1) {
        GPIO0->FIOSET |= (1<<7); // off
        UEZBSPDelayMS(100);
        GPIO0->FIOCLR |= (1<<7); // on
        UEZBSPDelayMS(100);
        count++;
        if (count >= aNumBlinks) {
            // Long pause
            UEZBSPDelayMS(1250);
            count = 0;
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  main
 *---------------------------------------------------------------------------*
 * Description:
 *      The main() routine in UEZ is only a stub that is used to start
 *      the whole UEZ system.  UEZBSP_Startup() is immediately called.
 * Outputs:
 *      int -- not used, 0
 *---------------------------------------------------------------------------*/
int main(void)
{
// Turn off piezo immediately!
GPIO1->FIODIR |= (1<<26);
GPIO1->FIOCLR = (1<<26);

//GPIO2->FIODIR |= (1<<10);
//GPIO2->FIOCLR = (1<<10);
//while (1) {
//    GPIO2->FIOSET = (1<<10);
//    UEZBSPDelayMS(10);
//    GPIO2->FIOCLR = (1<<10);
//    UEZBSPDelayMS(10);
//}

    UEZBSP_Startup();
    while (1) {
    } // never should get here
}

/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform.c
 *-------------------------------------------------------------------------*/
