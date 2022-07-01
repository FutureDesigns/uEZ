/*-------------------------------------------------------------------------*
 * File:  RDK_RX63N.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the RDK_RX63N
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

#include <uEZ.h>
#include <uEZADC.h>
#include <uEZAudioAmp.h>
#include <uEZBSP.h>
#include <uEZDevice.h>
#include <uEZDeviceTable.h>
#include <uEZFile.h>
#include <uEZNetwork.h>
#include <uEZProcessor.h>
#include <uEZStream.h>
#include <UEZPlatformAPI.h>
#include <UEZI2C.h>

#include <Device/SPIBus.h>
#include <Device/ADCBank.h>
#include <Device/AudioAmp.h>
#include <Device/FileSystem.h>
#include <Device/Flash.h>
#include <Device/MassStorage.h>
#include <Device/PWM.h>
#include <Device/Stream.h>
#include <Device/ToneGenerator.h>
#include <Device/Touchscreen.h>
#include <Device/I2CBus.h>

#include <Source/Devices/ADC/Generic/Generic_ADC.h>
#include <Source/Devices/Backlight/Generic/BacklightPWMControlled/BacklightPWM.h>
#include <Source/Devices/Flash/Renesas/RX62N/Flash_Renesas_RX62N.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_SPI.h>
#include <Source/Devices/Network/lwIP/Network_lwIP.h>
#include <Source/Devices/RTC/Generic/Generic_RTC.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>

#include <Source/Devices/Temperature/Analog Devices/ADT7420/Temperature_ADT7420.h>
#include <Source/Devices/Accelerometer/Analog Devices/ADXL345/Accelerometer_ADXL345.h>


#include <Source/Processor/Renesas/RX63N/uEZProcessor_RX63N.h>
#include <Source/Processor/Renesas/RX63N/RX63N_ADCBank_S12AD.h>
#include <Source/Processor/Renesas/RX63N/RX63N_GPIO.h>
#include <Source/Processor/Renesas/RX63N/RX63N_Serial.h>
#include <Source/Processor/Renesas/RX63N/RX63N_UtilityFuncs.h>
#include <Source/Processor/Renesas/RX63N/RX63N_I2C.h>
#include <Source/Processor/Renesas/RX63N/RX63N_RTC.h>
#include <Source/Processor/Renesas/RX63N/RX63N_PWM_TPU.h>
#include <Source/Processor/Renesas/RX63N/RX63N_Flash.h>

/*
#include <Source/Processor/Renesas/RX62N/RX62N_DTC.h>
#include <Source/Processor/Renesas/RX62N/RX62N_EMAC.h>
#include <Source/Processor/Renesas/RX62N/RX62N_ExternalInterrupt.h>
#include <Source/Processor/Renesas/RX62N/RX62N_PWM_TMR.h>
#include <Source/Processor/Renesas/RX62N/RX62N_SPI.h>
*/

#include "UEZPlatform.h"

#ifndef configKERNEL_INTERRUPT_PRIORITY
#define configKERNEL_INTERRUPT_PRIORITY     4
#endif

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
T_uezTask G_mainTask;
static T_uezDevice G_stdout = 0;
static T_uezDevice G_stdin = 0;

/*---------------------------------------------------------------------------*
 * Initialize the Vector Table
 *---------------------------------------------------------------------------*/
#include <RXToolset/vecttbl.c>

/*---------------------------------------------------------------------------*
 * Setting of B,R Section
 *---------------------------------------------------------------------------*/
#include <RXToolset/dbsct.c>

#include <stdio.h>
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE * fp) {
	write(0, ptr, nmemb*size);
	return nmemb;
}

/*---------------------------------------------------------------------------*
 * Routine:  vApplicationSetupTimerInterrupt
 *---------------------------------------------------------------------------*
 * Description:
 *      The RX port uses this callback function to configure its tick   
 *		interrupt. This allows the application to choose the tick interrupt
 *		source.
 *---------------------------------------------------------------------------*/
void vApplicationSetupTimerInterrupt(void)
{
    /* Enable compare match timer 0. */
    MSTP( CMT0) = 0;

    /* Interrupt on compare match. */
    CMT0.CMCR.BIT.CMIE = 1;

    /* Set the compare match value. */
    CMT0.CMCOR = (unsigned short)(((PCLK_FREQUENCY / configTICK_RATE_HZ) - 1) / 8);

    /* Divide the PCLK by 8. */
    CMT0.CMCR.BIT.CKS = 0;

    /* Enable the interrupt... */
    _IEN( _CMT0_CMI0) = 1;

    /* ...and set its priority to the application defined kernel priority. */
    _IPR( _CMT0_CMI0) = configKERNEL_INTERRUPT_PRIORITY;

    /* Start the timer. */
    CMT.CMSTR0.BIT.STR0 = 1;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSPDelayMS
 *---------------------------------------------------------------------------*
 * Description:
 *      Use a delay loop to approximate the time to delay.
 *      Should use UEZTaskDelayMS() when in a task instead.
 *---------------------------------------------------------------------------*/
void UEZBSPDelayMS(unsigned int aMilliseconds)
{
    TUInt32 i;
    // TODO: Calibrate based on the speed of processor
    while (aMilliseconds--) {
        for (i = 0; i < PROCESSOR_OSCILLATOR_FREQUENCY/5000; i++) {
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSPDelayUS
 *---------------------------------------------------------------------------*
 * Description:
 *      Use a delay loop to approximate the time to delay.
 *      Should use UEZTaskDelay when in a task instead.
 *---------------------------------------------------------------------------*/
void UEZBSPDelayUS(unsigned int aMicroseconds)   
{
    TUInt32 i;
    // TODO: Calibrate based on the speed of processor
    while (aMicroseconds--) {
        for (i = 0; i < PROCESSOR_OSCILLATOR_FREQUENCY/5000000; i++) {
        }
    }
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_PLLConfigure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the PLL
 *---------------------------------------------------------------------------*/
void UEZBSP_PLLConfigure(void)
{
    /* 
    Clock Description              Frequency
    ----------------------------------------
    Input Clock Frequency............  12 MHz
    PLL frequency (x16).............. 192 MHz
    Internal Clock Frequency.........  96 MHz    
    Peripheral Clock Frequency.......  96 MHz
    USB Clock Frequency..............  48 MHz
    External Bus Clock Frequency.....  24 MHz */

	unsigned int i;

	// Unlock protection bits
    SYSTEM.PRCR.WORD = 0xA50B;			/* Protect off */
    MPC.PWPR.BIT.B0WI = 0 ;     		/* Unlock protection register */
    MPC.PWPR.BIT.PFSWE = 1 ;    		/* Unlock MPC registers */
	
    /* Uncomment if not using sub-clock */
	//SYSTEM.SOSCCR.BYTE = 0x01;			/* stop sub-clock */
    SYSTEM.SOSCCR.BYTE = 0x00;			/* Enable sub-clock for RTC */
	SYSTEM.MOSCWTCR.BYTE = 0x0D;		/* Wait 131,072 cycles * 12 MHz = 10.9 ms */
	SYSTEM.PLLWTCR.BYTE = 0x04;			/* PLL wait is 4,194,304 cycles (default) * 192 MHz (12 MHz * 16) = 20.1 ms*/
	SYSTEM.PLLCR.WORD = 0x0F00;			/* x16 @PLL */
	SYSTEM.MOSCCR.BYTE = 0x00;			/* EXTAL ON */
	SYSTEM.PLLCR2.BYTE = 0x00;			/* PLL ON */

	for(i = 0;i< 0x168;i++)             /* wait over 12ms */
    {
        nop() ;
	}

    /* SCKCR - System Clock Control Register
    FCK[3:0]    0x2 = Flash clock: PLL/4 = (192 / 4) = 48 MHz
    ICK[3:0]    0x1 = System clock: PLL/2 = (192 / 2) = 96 MHz
    PSTOP1      0x0 = BCLK pin output is enabled
    BCK[3:0]    0x3 = BCLK: PLL/8 = 24 MHz
	PCKA[3:0]   0x1 = Peripheral clock A: PLLx2
    PCKB[3:0]   0x2 = Peripheral clock B: PLL/4, PCK = (PLLx2)/4 = 96 MHz
    */
	SYSTEM.SCKCR.LONG = 0x21031222;		/* ICK=PLL/2,BCK,FCK,PCK=PLL/2 */

    /* SCKCR2 - System Clock Control Register 2 
    UCK[3:0]    0x03 = USB clock is PLL/4 = 48 MHz
    IEBCK[3:0]  0x01 = IE Bus clock is PLL/2 = 96 MHz
    */
    SYSTEM.SCKCR2.WORD = 0x0031;

	SYSTEM.SCKCR3.WORD = 0x0400;		/* ICLK, PCLKB, FCLK, BCLK, IECLK, and USBCLK all come from PLL circuit */

    //SYSTEM.PRCR.WORD = 0xA500;			/* protect on */
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
 * Routine:  UEZBSP_RAMInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the external RAM.
 *---------------------------------------------------------------------------*/
void UEZBSP_RAMInit(void)
{
	// No External RAM
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZBSP_ROMInit
 *---------------------------------------------------------------------------*
 * Description:
 *      Initialize the external ROM.
 *---------------------------------------------------------------------------*/
void UEZBSP_ROMInit(void)
{
    // No External ROM
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

    // Stop running any other tasks
	UEZTaskSchedulerSuspend();

    // Port D bit 5 is LED4 on the YRDKRX63N
    // Set it for output and blink it
    PORTD.PDR.BIT.B5 = 1;

    // Blink our status led in a pattern, forever
    count = 0;
    while (1) {
        PORTD.PODR.BIT.B5 = 0;
        for (i=0; i<2000000; i++)
            ;
        PORTD.PODR.BIT.B5 = 1;
        for (i=0; i<2000000; i++)
            ;
        count++;
        if (count >= aErrorCode) {
            // Long pause
            for (i=0; i<10000000; i++)
                ;
            count = 0;
        }
    }
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
void UEZPlatform_Console_FullDuplex_SCI_Require(
        const char *aHALSerialName,
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    DEVICE_CREATE_ONCE();

    Serial_Generic_FullDuplex_Stream_Create("Console", aHALSerialName,
            aWriteBufferSize, aReadBufferSize);

    // Set standard output to console
    UEZStreamOpen("Console", &G_stdout);
    G_stdin = G_stdout;
    StdinRedirect(G_stdin);
    StdoutRedirect(G_stdout);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Console_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the console to be a full duplex UART on the expansion header
 *      NOTE: On expansion header
 * Inputs:
 *      TUInt32 aWriteBufferSize -- Size in bytes of outgoing buffer
 *      TUInt32 aReadBufferSize -- Size in bytes of incoming buffer
 *---------------------------------------------------------------------------*/
void UEZPlatform_Console_Require(
        TUInt32 aWriteBufferSize,
        TUInt32 aReadBufferSize)
{
    // Standard Expansion board serial console is on SCI6-A
	RX63N_SCI2_Require(GPIO_P50, GPIO_P52);
    UEZPlatform_Console_FullDuplex_SCI_Require("SCI2", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Backlight_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Backlight device driver for the LCD
 *---------------------------------------------------------------------------*/
/*
void UEZPlatform_Backlight_Require(void)
{
    const T_backlightGenericPWMSettings settings = {
            "PWM_TMR0",
            1, // master register
            0, // backlight register (TMO0)
            UEZ_LCD_BACKLIGHT_FULL_PERIOD,
            UEZ_LCD_BACKLIGHT_FULL_PWR_OFF,
			UEZ_LCD_BACKLIGHT_FULL_PWR_ON,
            GPIO_P21,
            EFalse
    };
    DEVICE_CREATE_ONCE();
    RX62N_PWM_TMR0_Require();
	UEZGPIOClear(GPIO_P21);
	UEZGPIOOutput(GPIO_P21);
    Backlight_Generic_PWMControlled_Create("Backlight", &settings);
}
*/

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Speaker_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Speaker device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM_TPU0_Require(void)
{
    DEVICE_CREATE_ONCE();

	RX63N_PWM_TPU0_Require(GPIO_NONE);
	
	PWM_Generic_Create("PWM_TPU0", "PWM_TPU0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SpeakerMasterCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Master MTU10 callback function for clearing the GPIO PWM Speaker pin
 *---------------------------------------------------------------------------*/
void UEZPlatform_SpeakerMasterCallback(void *aWorkspace)
{
	UEZGPIOClear(GPIO_P17);	// AUD_R
	UEZGPIOClear(GPIO_PA0);	// AUD_L
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SpeakerMatchCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Slave MTU10 callback function for setting the GPIO PWM Speaker pin
 *---------------------------------------------------------------------------*/
void UEZPlatform_SpeakerMatchCallback(void *aWorkspace)
{
	UEZGPIOSet(GPIO_P17);	// AUD_R
	UEZGPIOSet(GPIO_PA0);	// AUD_L
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
            "PWM_TPU0",
            1,
            GPIO_NONE,
            0,
            0,
    };
	T_uezDevice pwmDevice;
	
    DEVICE_CREATE_ONCE();

	UEZPlatform_PWM_TPU0_Require();
	UEZDeviceTableFind("PWM_TPU0", &pwmDevice);
	
	// The PWM Speaker is output by setting and clearing a GPIO pin
	// through software by using Callback routines.
	UEZGPIOClear(GPIO_P17);	// AUD_R
	UEZGPIOOutput(GPIO_P17);
	
	UEZGPIOClear(GPIO_PA0);	// AUD_L
	UEZGPIOOutput(GPIO_PA0);
	
	UEZGPIOSet(GPIO_PA2);	// AMP
	UEZGPIOOutput(GPIO_PA2);
	
	UEZPWMSetMatchCallback(pwmDevice, 0, UEZPlatform_SpeakerMasterCallback, 0);
	UEZPWMSetMatchCallback(pwmDevice, 1, UEZPlatform_SpeakerMatchCallback, 0);

    ToneGenerator_Generic_PWM_Create("Speaker", &settings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Flash0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Flash0 device driver for the external NOR Flash
 *---------------------------------------------------------------------------*/
void UEZPlatform_DataFlash_Require(void)
{
    DEVICE_CREATE_ONCE();
    Flash_Renesas_RX63N_Create("Flash0");
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
 * Routine:  UEZPlatform_USBHost_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USBHost.
 *---------------------------------------------------------------------------*/
void UEZPlatform_USBHost_Require(void)
{
    // Not Implemented
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MS0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS0 using USBHost flash drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS0_Require(void)
{
    // Not Implemented
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_USBFlash_Drive_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the USB Flash drive using MS0 on the given drive number
 *---------------------------------------------------------------------------*/
/*
void UEZPlatform_USBFlash_Drive_Require(TUInt8 aDriveNum)
{
    // Not Implemented
}
*/

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SPI0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the SSP1 to be a SPI driver.
 *      NOTE: On expansion header
 *---------------------------------------------------------------------------*/
void UEZPlatform_SPI0_Require(void)
{
    DEVICE_CREATE_ONCE();
    RX63N_RSPI0_Require(GPIO_PC5, GPIO_PC6, GPIO_PC7);
    SPI_Generic_Create("SPI0", "RSPI0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_MS1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS1 using SDCard drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_MS1_Require(void)
{
    DEVICE_CREATE_ONCE();
    UEZPlatform_SPI0_Require();
    MassStorage_SDCard_Create("MS1", "SPI0", GPIO_PC4);	// SD-CS = PC4
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SDCard_Drive_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the SDCard drive using MS1 on the given drive number
 *---------------------------------------------------------------------------*/
void UEZPlatform_SDCard_Drive_Require(TUInt8 aDriveNum)
{
    T_uezDevice ms1;
    T_uezDeviceWorkspace *p_ms1;

    DEVICE_CREATE_ONCE();

    UEZPlatform_MS1_Require();
    UEZPlatform_FileSystem_Require();

    UEZDeviceTableFind("MS1", &ms1);
    UEZDeviceTableGetWorkspace(ms1, (T_uezDeviceWorkspace **)&p_ms1);
    FATFS_RegisterMassStorageDevice(aDriveNum, (DEVICE_MassStorage **)p_ms1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_S12AD_Require(void)
{
	TUInt32 channels;
	
    DEVICE_CREATE_ONCE();
	
	channels = (1<<2); 	// AN002 = Potentiometer
	channels |= (1<<5); // AN005 = Microphone
	channels |= (1<<6); // AN006 = Right Audio Feedback
    RX63N_S12AD_Require(channels);
    ADCBank_Generic_Create("ADC_S12AD", "ADC_S12AD");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2C2_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C1 device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_I2C0_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Ensure the I2C0 exists in the HAL level
    RX63N_RIIC0_Require();
    I2C_Generic_Create("I2C0", "RIIC0", 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_RTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RTC device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_RTC_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Ensure the RTC exists in the HAL level
    RX63N_RTC_Require();
    RTC_Generic_Create("RTC", "RTC");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Temp_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Temperature device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Temp_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    Temp_AnalogDevices_ADT7420_Create("Temp0", "I2C0", 0x90 >> 1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Accel_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Accelerometer device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Accel_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    ST_Accelerometer_ADXL345_Create("Accel0", "I2C0");
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

	RX63N_UnlockProtectionBits();

    // Wired network needs an EMAC
    RX63N_100_EMAC_RMII_Require();

    // Create the network driver for talking to lwIP on a wired
    // network.
    Network_lwIP_Create("WiredNetwork0");
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
    // Do any initialiation necessary before the RTOS is started
}

void UEZPlatform_Standard_Require(void)
{
	
}

void UEZPlatform_Full_Require(void)
{
    // Setup console immediately
    UEZPlatform_Console_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
    
	UEZPlatform_S12AD_Require();
	UEZPlatform_SPI0_Require();
	UEZPlatform_Temp_Require();
	UEZPlatform_Accel_Require();
	UEZPlatform_Speaker_Require();
	UEZPlatform_DataFlash_Require();
	UEZPlatform_RTC_Require();
}

void UEZPlatform_Minimal_Require(void)
{
    // Setup console immediately
    UEZPlatform_Console_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
}

TUInt16 UEZPlatform_LCDGetHeight(void)
{
	// Needed by the library but not used
    return 0;
}

TUInt16 UEZPlatform_LCDGetWidth(void)
{
	// Needed by the library but not used
    return 0;
}

TUInt32 UEZPlatform_SerialGetDefaultBaud(void)
{
    return 115200;
}

TUInt32 UEZPlatform_ProcessorGetFrequency(void)
{
    return PROCESSOR_OSCILLATOR_FREQUENCY;
}

TUInt32 UEZPlatform_GetEMACPHYAddress(void)
{
    return 1; // National DP83640
}

void UEZPlatformWriteChar(char aChar)
{
    putchar(aChar);
}
/*
extern void WriteByteInFrameBufferWithAlpha(
        TUInt32 aAddr,
        COLOR_T aPixel,
        T_swimAlpha aAlpha)
{
    // Needed by the library but not used
}
*/
void ForceCodeNotToBeRemovedDuringOptimizedLink(void)
{
	extern void vSoftwareInterruptEntry(void);
	
	// This code is a trick to the HEW compiler.  It ensures
	// the the list of functions are definitely compiled into
	// the code instead of being optimized out.  Without this
	// function, the uEZ code will not work with HEW Renesas RX
	// compiler! -- 3/19/2012
	vSoftwareInterruptEntry();
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
	TUInt32 i;
	
	PORTC.PDR.BIT.B3 = 1; // RESET_IO
	PORTC.PODR.BIT.B3 = 1; // RESET_IO
    for (i=0;i<1000000;i++){}
    PORTC.PODR.BIT.B3 = 0; // RESET_IO
    for (i=0;i<1000000;i++){}
	
	UEZBSP_Startup();
    while (1) {
		// never should get here
		ForceCodeNotToBeRemovedDuringOptimizedLink();
    } 
}

/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform.c
 *-------------------------------------------------------------------------*/
