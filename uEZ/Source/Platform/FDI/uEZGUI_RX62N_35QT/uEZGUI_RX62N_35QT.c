/*-------------------------------------------------------------------------*
 * File:  uEZGUI_RX62N_35QT.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the uEZGUI_RX62N_35QT
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
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
#include <uEZPlatformAPI.h>

#include <Device/SPIBus.h>
#include <Device/ADCBank.h>
#include <Device/AudioAmp.h>
#include <Device/FileSystem.h>
#include <Device/Flash.h>
#include <Device/LCD.h>
#include <Device/MassStorage.h>
#include <Device/PWM.h>
#include <Device/Stream.h>
#include <Device/ToneGenerator.h>
#include <Device/Touchscreen.h>

#include <Source/Devices/ADC/Generic/Generic_ADC.h>
#include <Source/Devices/AudioAmp/NXP/TDA8551_T/AudioAmp_TDA8551T.h>
#include <Source/Devices/Backlight/Generic/BacklightPWMControlled/BacklightPWM.h>
#include <Source/Devices/Flash/Renesas/RX62N/Flash_Renesas_RX62N.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_SPI.h>
#include <Source/Devices/Network/lwIP/Network_lwIP.h>
#include <Source/Devices/Touchscreen/Generic/DirectDrive/DirectDrive_TS.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>

#include <Source/Processor/Renesas/RX62N/uEZProcessor_RX62N.h>
#include <Source/Processor/Renesas/RX62N/RX62N_ADCBank_S12AD.h>
#include <Source/Processor/Renesas/RX62N/RX62N_DTC.h>
#include <Source/Processor/Renesas/RX62N/RX62N_EMAC.h>
#include <Source/Processor/Renesas/RX62N/RX62N_ExternalInterrupt.h>
#include <Source/Processor/Renesas/RX62N/RX62N_Flash.h>
#include <Source/Processor/Renesas/RX62N/RX62N_GPIO.h>
#include <Source/Processor/Renesas/RX62N/RX62N_I2C.h>
#include <Source/Processor/Renesas/RX62N/RX62N_PWM_MTU.h>
#include <Source/Processor/Renesas/RX62N/RX62N_PWM_TMR.h>
#include <Source/Processor/Renesas/RX62N/RX62N_Serial.h>
#include <Source/Processor/Renesas/RX62N/RX62N_SPI.h>
#include <Source/Processor/Renesas/RX62N/RX62N_UtilityFuncs.h>

#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h> // Needs to go away!

#include "UEZPlatform.h"

#define configKERNEL_INTERRUPT_PRIORITY     4

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

/*---------------------------------------------------------------------------*
 * Provide low level access to STD In/Out calls
 *---------------------------------------------------------------------------*/
#include <RXToolset/lowsrc.c>

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
    for (i = 0; i < 96000; i++) {
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
	TUInt32 sckcr = 0;
	
    /* Configure system clocks based on header */
    sckcr += (ICLK_MUL==8) ? (0ul << 24) : (ICLK_MUL==4) ? (1ul << 24) : (ICLK_MUL==2) ? (2ul << 24) : (3ul << 24);
    sckcr += (BCLK_MUL==8) ? (0ul << 16) : (BCLK_MUL==4) ? (1ul << 16) : (BCLK_MUL==2) ? (2ul << 16) : (3ul << 16);
    sckcr += (PCLK_MUL==8) ? (0ul <<  8) : (PCLK_MUL==4) ? (1ul <<  8) : (PCLK_MUL==2) ? (2ul <<  8) : (3ul <<  8);
    SYSTEM.SCKCR.LONG = sckcr;
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
	TUInt32 delay;
	
    // Configure port pins --------------
    // Enable On Chip ROM & External BUS
    SYSTEM.SYSCR0.WORD = 0x5A03;

    // Port A should be A1-A7 and Port B should be A8 - A14
    IOPORT.PF4BUS.BYTE = 0x7F;
    // Port D is D0-D7 and Port E is D8-D15
    IOPORT.PF5BUS.BIT.DHE = 1;

    // Configure P61 for SDRAM SDCS#
    // and P62 for RAS#
    // and P63 for CAS#
    // and P64 for WE#
    // and P65 for CKE
    // and P66 for DQM0
    IOPORT.PF6BUS.BIT.MDSDE = 1;
    // and P67 for DQM1
    IOPORT.PF6BUS.BIT.DQM1E = 1;
    // and enable SDCLK port pin
    IOPORT.PF6BUS.BIT.SDCLKE = 1;

    // Wait at least 100 uS (100 counts of about 100 MHz)
    for (delay=0; delay<100*100; delay++)
        {}

    // SDRAM Initialization
    // Set the interval at which auto-refresh commands are issued in the
    // SDRAM initialization sequence
    BSC.SDIR.BIT.ARFI = 3-3;
    // Set the number of times auto-refresh is to be performed in the
    // SDRAM initialization sequence
    BSC.SDIR.BIT.ARFC = 15;
    // Set the number of precharge cycles in the SDRAM initialization sequence.
    BSC.SDIR.BIT.PRC = 3-3;

    // Start the SDRAM initialization
    BSC.SDICR.BIT.INIRQ = 1;
    // Wait for it to complete
    while (BSC.SDSR.BIT.INIST)
        {}

    // Configure more the SDRAM
    // Set little endian
    BSC.SDCMOD.BIT.EMODE = 0;
    // Set 16-bit databus
    BSC.SDCCR.BIT.BSIZE = 0;
    // Set Mode Register Definition
    //   1100 0000 0000
    //   1098 7654 3210
    //   ---- ---- ----
    //   00               Reserved
    //     1              WB
    //                      1 for Single Location Address
    //                      0 for Programmed Burst Length
    //      0 0           Op Mode
    //                      00 for Standard Operation
    //         010        CAS Latency
    //                      000, 001, 100, 101, 110, 111 = Reserved
    //                      010 = CAS 2
    //                      011 = CAS 3
    //             0      Burst Type
    //                      0 = Sequential
    //                      1 = Interleaved
    //              000   Burst Length
    //                      000 = 1
    //                      001 = 2
    //                      010 = 4
    //                      011 = 8
    //                      100, 101, 110 = Reserved
    //                      111 = Reserved if BT=0, else Full Page
    //   ---- ---- ----
    //   0010 0010 0000    <-- 0x220
    BSC.SDMOD.WORD = 0x220;

    // Set the refresh cycle count to 5
    BSC.SDRFCR.BIT.REFW = 5-1;

    // Set the refresh period
    BSC.SDRFCR.BIT.RFC =
        (((BCLK_FREQUENCY/1000) * SDRAM_REFRESH_TIME)
            / SDRAM_NUM_ROWS)-1;

    // Set CAS to 2 cycles
    //   These bits specify the column latency of the SDRAM controller. This
    //   setting only affects the latency setting on the SDRAM controller side.
    //   To specify the column latency for externally connected SDRAM, use the
    //   separate SDRAM mode register (SDMOD), which is described below.
    BSC.SDTR.BIT.CL = 2;

    // Write recovery interval = 1 cycle
    //   This bit specifies the interval that must elapse between the SDRAM
    //   write command (WRIT) and deactivation (PALL).
    BSC.SDTR.BIT.WR = 0;

    // Row precharge interval = 1 cycle
    //   These bits specify the minimum number of cycles that must elapse
    //   between the SDRAM deactivation command (PALL) and the next valid
    //   command.
    BSC.SDTR.BIT.RP = 0;

    // Row column latency = 1 cycle
    //   These bits specify the SDRAM row column latency.
    BSC.SDTR.BIT.RCD = 0;

    // Row active interval = 1 cycle
    //   These bits specify the minimum interval that must elapse between the
    //   SDRAM row activation command (ACTV) and deactivation (PALL).
    BSC.SDTR.BIT.RAS = 0;

    // Set Address Multiplex Select to 8 bits
    //   These bits select the size of the shift toward the lower half of the
    //   row address in row address/column address multiplexing. These bits
    //   also select the row address bits to be used for comparison in the
    //   SDRAMC continuous access operation.
    BSC.SDADR.BIT.MXC = 8-8;

    // Auto-refresh operation is enabled
    BSC.SDRFEN.BIT.RFEN = 1;

    //Enables the operation of the SDRAM address space
    BSC.SDCCR.BIT.EXENB = 1;

#if 0
    MemoryTest();
#endif

    /* connect MTU channel 3 to B pins */
    IOPORT.PFCMTU.BYTE = 0x0C;
    /* Connect DMAREQ0-C to P55 and DMAREQ0-A to P54 */
    IOPORT.PF7DMA.BIT.EDMA0S = 2;

    /* Enable hardware as necessary */
    MSTP(DTC) = 0;    /* enable DTC */
    MSTP(EXDMAC) = 0; /* enable ExDMA */
    MSTP(MTU0) = 0;   /* enable TPU 0-5 */
    MSTP(CRC) = 0;    /* enable CRC 0-5 */

    MTUA.TOER.BYTE = 0xFF; /* turn on all OE based outputs */
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

    // Port G bit 7 is the LED on the SOMDIMM-RX62N
    // Set it for output and blink it
    PORTG.DDR.BIT.B7 = 1;

    // Blink our status led in a pattern, forever
    count = 0;
    while (1) {
        PORT1.DR.BIT.B5 = 0;
        for (i=0; i<2000000; i++)
            ;
        PORT1.DR.BIT.B5 = 1;
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
    // Standard Expansion board serial console is on SCI6-A
	RX62N_SCI6_A_Require();
    UEZPlatform_Console_FullDuplex_SCI_Require("SCI6", aWriteBufferSize,
            aReadBufferSize);
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
            "PWM_TMR0",
            1, // master register
            0, // backlight register (TMO0)
            UEZ_LCD_BACKLIGHT_FULL_PERIOD,
            UEZ_LCD_BACKLIGHT_FULL_PWR_OFF,
			UEZ_LCD_BACKLIGHT_FULL_PWR_ON,
            GPIO_NONE,
            EFalse
    };
    DEVICE_CREATE_ONCE();
    RX62N_PWM_TMR0_Require();
	PORT2.DDR.BIT.B2 = 1;
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
	static const T_uezGPIOPortPin pins[] = {
			GPIO_P73,  	// YDRIVE
			GPIO_P93,  	// XDRIVE
			GPIO_P20,  	// LCD_HSYNC
			GPIO_P24,  	// LCD_VSYNC
			GPIO_PD0,	// LCD_DATA0
			GPIO_PD1,	// LCD_DATA1
			GPIO_PD2,	// LCD_DATA2
			GPIO_PD3,	// LCD_DATA3
			GPIO_PD4,	// LCD_DATA4
			GPIO_PD5,	// LCD_DATA5
			GPIO_PD6,	// LCD_DATA6
			GPIO_PD7,	// LCD_DATA7
			GPIO_PE0,	// LCD_DATA8
			GPIO_PE1,	// LCD_DATA9
			GPIO_PE2,	// LCD_DATA10
			GPIO_PE3,	// LCD_DATA11
			GPIO_PE4,	// LCD_DATA12
			GPIO_PE5,	// LCD_DATA13
			GPIO_PE6,	// LCD_DATA14
			GPIO_PE7,	// LCD_DATA15
    };
	T_halWorkspace *p_lcdc;
    T_uezDeviceWorkspace *p_lcd;
    T_uezDevice backlight;
    T_uezDeviceWorkspace *p_backlight;
    extern const T_uezDeviceInterface *UEZ_LCD_INTERFACE_ARRAY[];
	
	DEVICE_CREATE_ONCE();
	RX62N_DirectDrive_LCDController_Require(&pins);
	UEZPlatform_Backlight_Require();
	
    // Register LCD device
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
 * Routine:  UEZPlatform_Speaker_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Speaker device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM_MTU10_Require(void)
{
    DEVICE_CREATE_ONCE();

	RX62N_PWM_MTU10_Require();
	
	PWM_Generic_Create("PWM_MTU10", "PWM_MTU10");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SpeakerMasterCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Master MTU10 callback function for clearing the GPIO PWM Speaker pin
 *---------------------------------------------------------------------------*/
void UEZPlatform_SpeakerMasterCallback(void *aWorkspace)
{
	UEZGPIOClear(GPIO_P17);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SpeakerMatchCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Slave MTU10 callback function for setting the GPIO PWM Speaker pin
 *---------------------------------------------------------------------------*/
void UEZPlatform_SpeakerMatchCallback(void *aWorkspace)
{
	UEZGPIOSet(GPIO_P17);
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
            "PWM_MTU10",
            1,
            GPIO_NONE,
            0,
            0,
    };
	T_uezDevice pwmDevice;
	
    DEVICE_CREATE_ONCE();

	UEZPlatform_PWM_MTU10_Require();
	UEZDeviceTableFind("PWM_MTU10", &pwmDevice);
	
	// The PWM Speaker is output by setting and clearing a GPIO pin
	// through software by using Callback routines.
	UEZGPIOClear(GPIO_P17);
	UEZGPIOOutput(GPIO_P17);
	UEZPWMSetMatchCallback(pwmDevice, 0, UEZPlatform_SpeakerMasterCallback, 0);
	UEZPWMSetMatchCallback(pwmDevice, 1, UEZPlatform_SpeakerMatchCallback, 0);

    ToneGenerator_Generic_PWM_Create("Speaker", &settings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_AudioAmp_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Audio Amp driver and set the default level
 *---------------------------------------------------------------------------*/
void UEZPlatform_AudioAmp_Require(void)
{
    T_uezDevice amp;

    DEVICE_CREATE_ONCE();

    // P90 = VOL_UD
    // P91 = AMP_MODE
    AudioAmp_8551T_Create("AMP0", GPIO_P90, GPIO_P91, GPIO_NONE);
    UEZAudioAmpOpen("AMP0", &amp);
    UEZAudioAmpSetLevel(amp, UEZ_DEFAULT_AUDIO_LEVEL);
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
    Flash_Renesas_RX62N_Create("Flash0");
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
void UEZPlatform_USBFlash_Drive_Require(TUInt8 aDriveNum)
{
    // Not Implemented
}

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
    RX62N_RSPI0_A_Require();
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
    MassStorage_SDCard_Create("MS1", "SPI0", GPIO_P23);	// USD-CS = P23
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
 * Routine:  UEZPlatform_IRTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RTC device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_IRTC_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Ensure the RTC exists in the HAL level
    RX62N_RTC_Require();
    RTC_Generic_Create("RTC", "RTC");    
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ADC0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the ADC0 device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_S12AD_Require(void)
{
    TUInt8 allChannels = 0xFF;
    DEVICE_CREATE_ONCE();
    RX62N_S12AD_Require(allChannels);
    ADCBank_Generic_Create("ADC_S12AD", "ADC_S12AD");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Touchscreen_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the touchscreen and touch sensitivity for four wire resistive
 *---------------------------------------------------------------------------*/
void UEZPlatform_Touchscreen_Require(void)
{
   const TS_DirectDrive_Configuration tsConfig = {
        // XDrive is P93
		{GPIO_P93},
        // YDrive is P73
		{GPIO_P73},
		// iInputXL is AN6
		{ "ADC_S12AD",   6},
		// iInputXR is AN4
		{ "ADC_S12AD",   4},
		// iInputYU is AN5
		{ "ADC_S12AD",   5},
		// iInputYD is AN7
		{ "ADC_S12AD",   7},
    };
	T_uezDevice ts;

	DEVICE_CREATE_ONCE();
	UEZPlatform_S12AD_Require();
	
	Touchscreen_DirectDrive_Create("Touchscreen", &tsConfig);
    UEZDeviceTableFind("Touchscreen", &ts);
    UEZTSSetTouchDetectSensitivity(ts, 0x2000, 0x6000);
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
    RX62N_176_EMAC_RMII_Require();

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
	UEZPlatform_LCD_Require();
	
    // Setup console immediately
    UEZPlatform_Console_Expansion_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
    
	UEZPlatform_Touchscreen_Require();
	UEZPlatform_DataFlash_Require();
	UEZPlatform_Speaker_Require();
	UEZPlatform_AudioAmp_Require();
	UEZPlatform_SDCard_Drive_Require(1);
	UEZPlatform_WiredNetwork0_Require();
	UEZPlatform_IRTC_Require();
} 

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUIIsLoopbackBoardConnected
 *---------------------------------------------------------------------------*
 * Description:
 *      Determines if a loopback board for testing is connected
 * Outputs:
 *      TBool -- ETrue if connected, else EFalse
 *---------------------------------------------------------------------------*/
TBool UEZGUIIsLoopbackBoardConnected(void)
{
    TBool readSet;
    TBool readClear;

	// When the uEZGUI RX 3.5 loopback board is connected, P41 and P16 are shorted
	// Run a quick GPIO loopback test on these pins.

	UEZGPIOSetMux(GPIO_P41, 0); // GPIO mode
	UEZGPIOOutput(GPIO_P41);
	UEZGPIOSetMux(GPIO_P16, 0); // GPIO mode
	UEZGPIOInput(GPIO_P16);
	
	UEZGPIOSet(GPIO_P41);
	UEZTaskDelay(1);
	readSet = UEZGPIORead(GPIO_P16);
	UEZTaskDelay(1);
	UEZGPIOClear(GPIO_P41);
	UEZTaskDelay(1);
	readClear = UEZGPIORead(GPIO_P16);
	
	UEZGPIOInput(GPIO_P41);

    // If the pin wiggled, then the loop back is in place
    if (readSet && !readClear)
        return ETrue;

    // Otherwise it is not set.
    return EFalse;
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
TUInt32 UEZPlatform_GetEMACPHYAddress(void)
{
    return 1; // National DP83640
}
TUInt32 UEZPlatform_GetBaseAddress(void)
{
    return LCD_DISPLAY_BASE_ADDRESS;
}

#if INCLUDE_EMWIN
#include <Source/Library/GUI/SEGGER/emWin/LCD.h>
const void* UEZPlatform_GUIColorConversion(void)
{
    return GUICC_M565;
}
#endif

T_pixelColor SUICallbackRGBConvert(int r, int g, int b)
{
    return RGB(r, g, b);
}

void SUICallbackSetLCDBase(void *aBaseAddress)
{
    extern TUInt16 * LCDSetActiveRaster(TUInt32 frame_request);
    extern void HandleFail(void);

    LCDSetActiveRaster((TUInt32)aBaseAddress);
}

extern void WriteByteInFrameBufferWithAlpha(
        UNS_32 aAddr,
        COLOR_T aPixel,
        T_swimAlpha aAlpha)
{
    static COLOR_T mask = ~((1 << 10) | (1 << 5) | (1 << 0));
    COLOR_T *p = (COLOR_T *)aAddr;

    switch (aAlpha) {
        case SWIM_ALPHA_50:
            *p = (((*p & mask) >> 1) + ((aPixel & mask) >> 1));
            break;
    }
}

void UEZPlatformWriteChar(char aChar)
{
    putchar(aChar);
}

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
    UEZBSP_Startup();
    while (1) {
		// never should get here
		ForceCodeNotToBeRemovedDuringOptimizedLink();
    } 
}

/*-------------------------------------------------------------------------*
 * End of File:  uEZPlatform.c
 *-------------------------------------------------------------------------*/
