/*-------------------------------------------------------------------------*
 * File:  DK_TS_RX63N.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Bring up the DK_TS_RX63N
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
#include <UEZPlatformAPI.h>
#include <UEZI2C.h>

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
#include <Device/I2CBus.h>

#include <Source/Devices/ADC/Generic/Generic_ADC.h>
#include <Source/Devices/AudioAmp/NXP/TDA8551_T/AudioAmp_TDA8551T.h>
#include <Source/Devices/Backlight/Generic/BacklightPWMControlled/BacklightPWM.h>
#include <Source/Devices/Flash/Renesas/RX62N/Flash_Renesas_RX62N.h>
#include <Source/Devices/MassStorage/SDCard/SDCard_MS_driver_SPI.h>
#include <Source/Devices/Network/lwIP/Network_lwIP.h>
#include <Source/Devices/ToneGenerator/Generic/PWM/ToneGenerator_Generic_PWM.h>
#include <Source/Devices/Touchscreen/Generic/DirectDrive/DirectDrive_TS.h>
#include <Source/Devices/RTC/Generic/Generic_RTC.h>
#include <Source/Devices/RTC/NXP/PCF8563/RTC_PCF8563.h>
#include <Source/Devices/Temperature/NXP/LM75A/Temperature_LM75A.h>
#include <Source/Devices/Accelerometer/Freescale/MMA7455/Freescale_MMA7455.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>
#include <Source/Devices/Button/NXP/PCA9551/Button_PCA9551.h>
#include <Source/Devices/LED/NXP/PCA9551/LED_NXP_PCA9551.h>

#include <Source/Processor/Renesas/RX63N/uEZProcessor_RX63N.h>
#include <Source/Processor/Renesas/RX63N/RX63N_ADCBank_S12AD.h>
#include <Source/Processor/Renesas/RX63N/RX63N_GPIO.h>
#include <Source/Processor/Renesas/RX63N/RX63N_Serial.h>
#include <Source/Processor/Renesas/RX63N/RX63N_UtilityFuncs.h>
#include <Source/Processor/Renesas/RX63N/RX63N_I2C.h>
#include <Source/Processor/Renesas/RX63N/RX63N_PWM_TPU.h>
#include <Source/Processor/Renesas/RX63N/RX63N_Flash.h>
#include <Source/Processor/Renesas/RX63N/RX63N_RTC.h>

#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI_Types.h>
#include <Source/Library/GUI/FDI/SimpleUI/SimpleUI.h> // Needs to go away!

#include "DirectDrive/CARRIER_RX63N_DirectDrive.h"


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
TBool G_TriLED = EFalse;

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
 * Task:  Heartbeat
 *---------------------------------------------------------------------------*
 * Description:
 *      Blink heartbeat LED
 * Inputs:
 *      T_uezTask aMyTask            -- Handle to this task
 *      void *aParams               -- Parameters.  Not used.
 * Outputs:
 *      TUInt32                     -- Never returns.
 *---------------------------------------------------------------------------*/
TUInt32 Heartbeat(T_uezTask aMyTask, void *aParams)
{
	TUInt8 circle = 0;
	
	UEZGPIOOutput(GPIO_PG7);
	UEZGPIOSet(GPIO_PG7);
	UEZGPIOOutput(GPIO_PG6);
	UEZGPIOSet(GPIO_PG6);
	UEZGPIOOutput(GPIO_PG5);
	UEZGPIOSet(GPIO_PG7);
    // Blink
    for (;;) {
		if(G_TriLED) {
			if (circle==0) {
				UEZGPIOSet(GPIO_PG7);
				UEZGPIOClear(GPIO_PG5);
				circle++;
			} else if(circle==1) {
				UEZGPIOSet(GPIO_PG5);
				UEZGPIOClear(GPIO_PG6);
				circle++;
			} else if(circle==2) {
				UEZGPIOSet(GPIO_PG6);
				UEZGPIOClear(GPIO_PG7);
				circle=0;
			}
			UEZTaskDelay(1000);
		}	
		else {
	        UEZGPIOSet(GPIO_PG6);
			UEZGPIOSet(GPIO_PG7);
			UEZGPIOSet(GPIO_PG5);
	        UEZTaskDelay(250);
			UEZGPIOClear(GPIO_PG6);
	        UEZTaskDelay(250);
		}
    }                    
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
    External Bus Clock Frequency.....  48 MHz */

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
    BCK[3:0]    0x2 = BCLK: PLL/4 = 48 MHz
	PCKA[3:0]   0x1 = Peripheral clock A: PLLx2
    PCKB[3:0]   0x2 = Peripheral clock B: PLL/4, PCK = (PLLx2)/4 = 96 MHz
    */
	SYSTEM.SCKCR.LONG = 0x21021222;		/* ICK=PLL/2,BCK,FCK,PCK=PLL/2 */
	// 0x21021222

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
	TUInt32 delay;
	
	RX63N_UnlockProtectionBits();	
	
    // Configure port pins --------------
    // Enable On Chip ROM & External BUS
    SYSTEM.SYSCR0.WORD = 0x5A03;

    // Port A should be A1-A7 and Port B should be A8 - A14
	MPC.PFBCR0.BIT.ADRLE=1;
	MPC.PFAOE0.BIT.A8E=1;
	MPC.PFAOE0.BIT.A9E=1;
	MPC.PFAOE0.BIT.A10E=1;
	MPC.PFAOE0.BIT.A11E=1;
	MPC.PFAOE0.BIT.A12E=1;
	MPC.PFAOE0.BIT.A13E=1;
	MPC.PFAOE0.BIT.A14E=1;
	
    // Port D is D0-D7 and Port E is D8-D15
    //IOPORT.PF5BUS.BIT.DHE = 1;
	MPC.PFBCR0.BIT.DHE=1;

    // Configure P61 for SDRAM SDCS#
    // and P62 for RAS#
    // and P63 for CAS#
    // and P64 for WE#
    // and P65 for CKE
    // and P66 for DQM0
//    IOPORT.PF6BUS.BIT.MDSDE = 1;
    // and P67 for DQM1
    //IOPORT.PF6BUS.BIT.DQM1E = 1;
	MPC.PFBCR1.BIT.MDSDE=1;
	
    // and enable SDCLK port pin
    //;IOPORT.PF6BUS.BIT.SDCLKE = 1;
	MPC.PFBCR1.BIT.SDCLKE=1;
	MPC.PFBCR1.BIT.DQM1E=1;
	
// Set MDR on io pins
	

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
    //         010        CAS Latency (010 = 2, 011 = 3, 001 = 1)
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
#if (SDRAM_CAS_LATENCY==3)
    BSC.SDMOD.WORD = 0x230;
#elif (SDRAM_CAS_LATENCY==2)
    BSC.SDMOD.WORD = 0x220;
#elif (SDRAM_CAS_LATENCY==1)
    BSC.SDMOD.WORD = 0x210;
#else
    #error "SDRAM_CAS_LATENCY MUST BE SET!"
#endif

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
    BSC.SDTR.BIT.CL = SDRAM_CAS_LATENCY;

    // Write recovery interval = 1 cycle
    //   This bit specifies the interval that must elapse between the SDRAM
    //   write command (WRIT) and deactivation (PALL).
    BSC.SDTR.BIT.WR = 0;

    // Row precharge interval = 1-8 cycles
    //   These bits specify the minimum number of cycles that must elapse
    //   between the SDRAM deactivation command (PALL) and the next valid
    //   command.
    BSC.SDTR.BIT.RP = SDRAM_ROW_PRECHARGE-1;     

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
	
	PORTD.DSCR.BYTE = 0xFF;
	PORTE.DSCR.BYTE = 0xFF;
	
#if 0
        MemoryTest(0x08000000, DISPLAY_HEIGHT*DISPLAY_WIDTH*2);
#endif
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
    PORTG.PDR.BIT.B7 = 1;

    // Blink our status led in a pattern, forever
    count = 0;
    while (1) {
        PORTG.PODR.BIT.B7 = 0;
        for (i=0; i<2000000; i++)
            ;
        PORTG.PODR.BIT.B7 = 1;
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
void UEZPlatform_Console_FullDuplex_UART_Require(
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
	RX63N_SCI6_Require(GPIO_P00, GPIO_P01);
    UEZPlatform_Console_FullDuplex_UART_Require("SCI6", aWriteBufferSize,
            aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM_TPU0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup TPU0
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM_TPU0_Require(T_uezGPIOPortPin aOutputPin)
{
    DEVICE_CREATE_ONCE();

	RX63N_PWM_TPU0_Require(aOutputPin);
	
	PWM_Generic_Create("PWM_TPU0", "PWM_TPU0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SpeakerMasterCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Master TPU0 callback function for clearing the GPIO PWM Speaker pin
 *---------------------------------------------------------------------------*/
void UEZPlatform_SpeakerMasterCallback(void *aWorkspace)
{
	UEZGPIOClear(GPIO_P05);	// SPKR
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_SpeakerMatchCallback
 *---------------------------------------------------------------------------*
 * Description:
 *      Slave TPU0 callback function for setting the GPIO PWM Speaker pin
 *---------------------------------------------------------------------------*/
void UEZPlatform_SpeakerMatchCallback(void *aWorkspace)
{
	UEZGPIOSet(GPIO_P05);	// SPKR
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

	UEZPlatform_PWM_TPU0_Require(GPIO_NONE);
	UEZDeviceTableFind("PWM_TPU0", &pwmDevice);
	
	// The PWM Speaker is output by setting and clearing a GPIO pin
	// through software by using Callback routines.
	UEZGPIOClear(GPIO_P05);	// SPKR  
	UEZGPIOOutput(GPIO_P05);
	
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

    // P97 = VOL_UD
    // P96 = AMP_MODE  
    AudioAmp_8551T_Create("AMP0", GPIO_P97, GPIO_P96, GPIO_NONE);
    UEZAudioAmpOpen("AMP0", &amp);
    UEZAudioAmpSetLevel(amp, UEZ_DEFAULT_AUDIO_LEVEL);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_PWM_TPU1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the TPU1
 *---------------------------------------------------------------------------*/
void UEZPlatform_PWM_TPU1_Require(T_uezGPIOPortPin aOutputPin)
{
    DEVICE_CREATE_ONCE();

	RX63N_PWM_TPU1_Require(aOutputPin);
	
	PWM_Generic_Create("PWM_TPU1", "PWM_TPU1");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Backlight_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the backlight device driver
 *---------------------------------------------------------------------------*/
void UEZPlatform_Backlight_Require(void)
{
    const T_backlightGenericPWMSettings settings = {
            "PWM_TPU1",
            0, // master register
            1, // backlight register (TIOCB1)
            UEZ_LCD_BACKLIGHT_FULL_PERIOD,
            UEZ_LCD_BACKLIGHT_FULL_PWR_OFF,
			UEZ_LCD_BACKLIGHT_FULL_PWR_ON,
            GPIO_NONE,
            EFalse
    };  
	T_uezDevice pwmDevice;
	
    DEVICE_CREATE_ONCE();

	UEZPlatform_PWM_TPU1_Require(GPIO_P16);
	UEZDeviceTableFind("PWM_TPU1", &pwmDevice);

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
			GPIO_P56,	// LCD DOTCLK
			GPIO_P20,  	// LCD_HSYNC
			GPIO_P83,  	// LCD_VSYNC
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
	RX63N_DirectDrive_LCDController_Require(&pins);
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
 * Routine:  UEZPlatform_EEPROM_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Mass Storage device MS1 using SDCard drive.
 *---------------------------------------------------------------------------*/
void UEZPlatform_EEPROM_Require(void)
{
    DEVICE_CREATE_ONCE();
    UEZPlatform_SPI0_Require();
	UEZGPIOSet(GPIO_PC4);
  	UEZGPIOOutput(GPIO_PC4);
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
    MassStorage_SDCard_Create("MS1", "SPI0", GPIO_PC0);	// MICROSD_CS = PC0
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
	
	channels = (1<<7); 	// AN007 = Y_INPUT2
	channels |= (1<<6); // AN006 = X_INPUT2
	channels |= (1<<5); // AN005 = Y_INPUT1
	channels |= (1<<4); // AN004 = X_INPUT1
	
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
    I2C_Generic_Create("I2C0", "RIIC0");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_AudioCodec_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Audio Codec driver and set the default level
 *---------------------------------------------------------------------------*/
void UEZPlatform_AudioCodec_Require(void)
{
    T_uezDevice p_ac;
    DEVICE_AudioCodec **ac;

    DEVICE_CREATE_ONCE();

	UEZPlatform_I2C0_Require();
    AudioCodec_WM8731_ADXL345_Create("AudioCodec0", "I2C0");
    UEZDeviceTableFind("AudioCodec0", &p_ac);
    UEZDeviceTableGetWorkspace(p_ac, (T_uezDeviceWorkspace **)&ac);

    (*ac)->UseConfiguration((void*)ac, 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_RTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the external RTC device driver.  Not compatible with the
 *      internal RTC.
 *---------------------------------------------------------------------------*/
void UEZPlatform_RTC_PCF8563_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    RTC_PCF8563_Create("RTC", "I2C0");     
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_ERTC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RTC device driver and check it for validity
 *---------------------------------------------------------------------------*/
void UEZPlatform_ERTC_Require(void)
{
    T_uezDevice rtcDev;
    DEVICE_RTC **p_rtcDev;
    const T_uezTimeDate resetTD = {
        {   0, 0, 0}, // 12:00 midnight
        {   1, 1, 2009}, // Jan 1, 2009
    };
    T_uezError error;

    UEZPlatform_RTC_PCF8563_Require();

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
    RX63N_RTC_Require();
    RTC_Generic_Create("IRTC", "RTC");    
}     

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_Temp0_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the Temperature device driver.
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
 *      Setup the Accelerometer device driver.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Accel0_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZPlatform_I2C0_Require();
    Accelerometer_Freescale_MMA7455_I2C_Create("Accel0", "I2C0",
            MMA7455_I2C_ADDR);
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
        // XDrive is P84
		{GPIO_P84},
        // YDrive is P85
		{GPIO_P85},
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

	RX63N_UnlockProtectionBits();

    // Wired network needs an EMAC
    RX63N_176_EMAC_RMII_Require();

    // Create the network driver for talking to lwIP on a wired
    // network.
    Network_lwIP_Create("WiredNetwork0");
}


/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wired connections.
 *---------------------------------------------------------------------------*/
void UEZPlatform_Button_Require(void)
{
    T_uezDeviceWorkspace *p_button0;
    T_uezDevice i2c0;
    T_uezDeviceWorkspace *p_i2c0;

    DEVICE_CREATE_ONCE();

    UEZDeviceTableFind("I2C0", &i2c0);
    UEZDeviceTableGetWorkspace(i2c0, (T_uezDeviceWorkspace **)&p_i2c0);

    UEZDeviceTableRegister(
        "ButtonBank0",
        (T_uezDeviceInterface *)&ButtonBank_NXP_PCA9551_Interface,
        0,
        &p_button0);
    ButtonBank_NXP_PCA9551_Configure(p_button0, (DEVICE_I2C_BUS **)p_i2c0, 0xC0>>1);
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wired connections.
 *---------------------------------------------------------------------------*/
void UEZPlatform_LED_Require(void)
{
    T_uezDeviceWorkspace *p_led0;
    T_uezDevice i2c0;
    T_uezDeviceWorkspace *p_i2c0;

    DEVICE_CREATE_ONCE();

    UEZDeviceTableFind("I2C0", &i2c0);
    UEZDeviceTableGetWorkspace(i2c0, (T_uezDeviceWorkspace **)&p_i2c0);

    UEZDeviceTableRegister(
        "LEDBank0",
        (T_uezDeviceInterface *)&LEDBank_NXP_PCA9551_Interface,
        0,
        &p_led0);
    LED_NXP_PCA9551_Configure(p_led0, (DEVICE_I2C_BUS **)p_i2c0, 0xC0>>1);
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
	
    UEZPlatform_Console_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
    
	UEZPlatform_Touchscreen_Require();
	UEZPlatform_S12AD_Require();
	UEZPlatform_SPI0_Require();
	UEZPlatform_Temp0_Require();
	UEZPlatform_Accel0_Require();
	UEZPlatform_Speaker_Require();
	UEZPlatform_AudioAmp_Require();
	UEZPlatform_DataFlash_Require();
	UEZPlatform_ERTC_Require();
	UEZPlatform_IRTC_Require();
	UEZPlatform_LED_Require();
	UEZPlatform_Button_Require();
	UEZPlatform_AudioCodec_Require();
	UEZPlatform_EEPROM_Require();  
}

void UEZPlatform_Full_Require(void)
{
    // Setup console immediately
	UEZPlatform_LCD_Require();
	
    UEZPlatform_Console_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
    
	UEZPlatform_Touchscreen_Require();
	UEZPlatform_S12AD_Require();
	UEZPlatform_SPI0_Require();
	UEZPlatform_Temp0_Require();
	UEZPlatform_Accel0_Require();
	UEZPlatform_Speaker_Require();
	UEZPlatform_AudioAmp_Require();
	UEZPlatform_DataFlash_Require();
	UEZPlatform_ERTC_Require();
	UEZPlatform_IRTC_Require();
	UEZPlatform_LED_Require();
	UEZPlatform_Button_Require();
	UEZPlatform_AudioCodec_Require();
	UEZPlatform_EEPROM_Require();
}

void UEZPlatform_Minimal_Require(void)
{
    // Setup console immediately
    UEZPlatform_Console_Require(UEZ_CONSOLE_WRITE_BUFFER_SIZE,
            UEZ_CONSOLE_READ_BUFFER_SIZE);
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

void UEZPlatformWriteChar(char aChar)
{
    putchar(aChar);
}

#include "LCD.h"
const void* UEZPlatform_GUIColorConversion(void)
{
    return GUICC_M565;
}

T_pixelColor SUICallbackRGBConvert(int r, int g, int b)
{
    return RGB(r, g, b);
}

void *SUICallbackGetLCDBase(void)
{
    return (void *)(LCD_FRAME_BUFFER);
}
    
void SUICallbackSetLCDBase(void *aBaseAddress)
{
    extern TUInt16 * LCDSetActiveRaster(TUInt32 frame_request);
    extern void HandleFail(void);

    //LCDSetActiveRaster((TUInt32)aBaseAddress);
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
