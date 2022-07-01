/*-------------------------------------------------------------------------*
 * File:  uEZGUI_EXP_DK.c
 *-------------------------------------------------------------------------*
 * Description:
 *     Bring up the EXP-DK
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
/**
 *    @addtogroup uEZGUI-EXP-DK
 *  @{
 *  @brief     uEZGUI-EXP-DK platform
 *  @see http://www.teamfdi.com/uez/
 *  @see http://www.teamfdi.com/uez/files/uEZ License.pdf
 *
 *    The uEZGUI-EXP-DK platform interface.
*/
#include <Config.h>
#include <stdio.h>
#include <string.h>
#include "uEZGUI_EXP_DK.h"
#include <Source/Devices/I2C/Mux/PCA9546/I2C_Mux_PCA9546.h>
#include <Source/Devices/I2S/Generic/Generic_I2S.h>
#include <Source/Devices/AudioAmp/NXP/TDA8551_T/AudioAmp_TDA8551T.h>
#include <Source/Devices/AudioAmp/TI/LM48100/AudioAmp_LM48100.h>
#include <Source/Devices/AudioAmp/Wolfson/WM8731/AudioAmp_WM8731.h>
#include <Source/Devices/Audio Codec/Wolfson/WM8731/AudioCodec_WM8731.h>
#include <Source/Devices/Serial/Generic/Generic_Serial.h>
#include <Source/Devices/LED/NXP/PCA9551/LED_NXP_PCA9551.h>
#include <uEZ.h>
#include <uEZI2C.h>
#include <uEZAudioAmp.h>
#include <uEZAudioMixer.h>
#include <uEZDeviceTable.h>
#include <uEZGPIO.h>
#include <uEZPlatform.h>
#include <uEZProcessor.h>


#include <Device/ButtonBank.h>
#include <Device/LEDBank.h>
#include <Source/Devices/LED/NXP/PCA9551/LED_NXP_PCA9551.h>
#include <Source/Devices/Button/NXP/PCA9551/Button_PCA9551.h>
#include <Include/uEZDeviceTable.h>
#include <Source/Devices/I2C/Mux/PCA9546/I2C_Mux_PCA9546.h>
#include <Source/Library/Tests/ALS/Vishay/VCNL4010/TestVCNL4010.h>


/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define UEZGUI_EXP_DK_I2CMUX_I2C_ADDRESS        0x70

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static T_uezDevice G_Amp0 = 0;
static T_uezDevice G_Amp1 = 0;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_AudioAmp_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup EXP-DK AudioAmp, a LM48100QMH at U10.  It uses AUDIOSDA/AUDIOSCL
 *      to control it.  Set it up as "EXP_AMP0".
 *
 *      NOTE: To enable I2C to this audio amp, JP16 must connect I2CSCL
 *          to AUDIOSCL and I2CSDA to AUDIOSDA.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_AudioAmp0_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZGUI_EXP_DK_AudioI2C_Require();
    AudioAmp_LM48110_Create("EXPDK_AMP0", EXPDK_AUDIO_I2C, 0x1F);
    UEZAudioAmpOpen("EXPDK_AMP0", &G_Amp0);
    UEZAudioAmpSetLevel(G_Amp0, UEZ_DEFAULT_AUDIO_LEVEL);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_AudioAmp1_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      The WM8731 AudioCodec has an amp for controlling output to
 *      the headphones.  Register this amp as EXPDK_AMP1.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_AudioAmp1_Require(void)
{
    DEVICE_CREATE_ONCE();

    UEZGUI_EXP_DK_AudioI2C_Require();
    AudioAmp_WM8731_Create("EXPDK_AMP1", EXPDK_AUDIO_I2C, 0x7F);
    UEZAudioAmpOpen("EXPDK_AMP1", &G_Amp1);
    UEZAudioAmpSetLevel(G_Amp1, UEZ_DEFAULT_AUDIO_LEVEL);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_AudioCodec_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the WM8731 AudioCodec for directing audio in and out.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_AudioCodec_Require(void)
{
    T_uezDevice p_ac;
    DEVICE_AudioCodec **ac;

    DEVICE_CREATE_ONCE();

    AudioCodec_WM8731_ADXL345_Create("EXP_AudioCodec0", EXPDK_AUDIO_I2C);
    UEZDeviceTableFind("EXP_AudioCodec0", &p_ac);
    UEZDeviceTableGetWorkspace(p_ac, (T_uezDeviceWorkspace **)&ac);

    // Configure so that both audio inputs are output to the headphones.
    (*ac)->UseConfiguration((void*)ac, 0);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_AudioMixerCallback
 *---------------------------------------------------------------------------*
 * Description:
 *    Process a callback even for changing volumes to the EXP_DK board, or
 *    in this case, for all offboard speaker and headphones.
 *---------------------------------------------------------------------------*/
static T_uezError UEZGUI_EXP_DK_AudioMixerCallback(
    T_uezAudioMixerOutput aChangedOutput,
    TBool aMute,
    TUInt8 aLevel)
{
    T_uezError error = UEZ_ERROR_NONE;

    switch (aChangedOutput) {
        case UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER:
            if (G_Amp0) {
                if (aMute) {
                    error = UEZAudioAmpMute(G_Amp0);
                } else {
                    error = UEZAudioAmpUnMute(G_Amp0);
                }
                error = UEZAudioAmpSetLevel(G_Amp0, aLevel);
            }
            break;
        case UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_HEADPHONES:
            if (G_Amp1) {
                if (aMute) {
                    error = UEZAudioAmpMute(G_Amp1);
                } else {
                    error = UEZAudioAmpUnMute(G_Amp1);
                }
                error = UEZAudioAmpSetLevel(G_Amp1, aLevel);
            }
            break;
        default:
            break;
    }
    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_AudioMixer_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup all the audio components needed for the audio mixer
 *      and register a callback function for controlling audio to those
 *      units.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_AudioMixer_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Make sure all the audio components exist
    UEZGUI_EXP_DK_AudioCodec_Require();
    UEZGUI_EXP_DK_AudioAmp0_Require(); // LM48100 I2C Amp
    UEZGUI_EXP_DK_AudioAmp1_Require(); // Wolfson Audio amp

    // Register an off board speaker
    UEZAudioMixerRegister(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER,
        &UEZGUI_EXP_DK_AudioMixerCallback);
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER, 250);
    UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_SPEAKER);

    // Register an off board set of headphones
    UEZAudioMixerRegister(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_HEADPHONES,
        &UEZGUI_EXP_DK_AudioMixerCallback);
    UEZAudioMixerSetLevel(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_HEADPHONES, 250);
    UEZAudioMixerUnmute(UEZ_AUDIO_MIXER_OUTPUT_OFFBOARD_HEADPHONES);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_CAN_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup CAN to the EXP DK (currently not supported as a driver yet)
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_CAN_Require(void)
{
    DEVICE_CREATE_ONCE();

    // The EXP-DK has a CAN option when JP23 is set to connect CANTD and CANRD
    // to NXPCANTD/NXPCANRD (meaning this is currently a NXP processor only
    // option).
    // NXPCANTD/NXPCANRD are on the secondary expansion connector at J1-11/12.
    // This is UART_D on the expansion board.

    // TBD: Currently, uEZ does NOT have a CAN driver, so there is nothing else
    // to do here!
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_DALI_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup DALI on the EXP_DK.  Uses the serial lines DALITXD and DALIRXD.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_DALI_Require(void)
{
    // TBD: No driver exists currently for DALI
    // GPIO_EXPDK_DALITXD & GPIO_EXPDK_RX62SCL_DALIRXD used here.
}

/*---------------------------------------------------------------------------*
f * Routine:  UEZGUI_EXP_DK_EMAC_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup Ethernet to work on the EXP_DK.
 *      Since the EXP_DK is just a header and PHY, no setup is needed other
 *      than to enable it on the connected platform/board.  The platform
 *      will do the rest.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_EMAC_Require(void)
{
    //DEVICE_CREATE_ONCE();
    UEZPlatform_ExpansionPrimary_EMAC_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_I2CMux_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup I2CMux on the expansion board as EXP_I2C-A - EXP_I2C-D
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_I2CMux_Require(void)
{
    void *p_group = 0;
    DEVICE_CREATE_ONCE();

    UEZPlatform_ExpansionPrimary_I2C_A_Require();
    I2C_Mux_PCA9546_Create("EXPDK_I2C-A", PRIMARY_EXPANSION_I2C_A,
        UEZGUI_EXP_DK_I2CMUX_I2C_ADDRESS, 0, &p_group);
    I2C_Mux_PCA9546_Create("EXPDK_I2C-B", PRIMARY_EXPANSION_I2C_A,
        UEZGUI_EXP_DK_I2CMUX_I2C_ADDRESS, 1, &p_group);
    I2C_Mux_PCA9546_Create("EXPDK_I2C-C", PRIMARY_EXPANSION_I2C_A,
        UEZGUI_EXP_DK_I2CMUX_I2C_ADDRESS, 2, &p_group);
    I2C_Mux_PCA9546_Create("EXPDK_I2C-D", PRIMARY_EXPANSION_I2C_A,
        UEZGUI_EXP_DK_I2CMUX_I2C_ADDRESS, 3, &p_group);
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C GPIO Buttons on EXPDK_I2C-A
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_Button_Require(void)
{
    T_uezDeviceWorkspace *p_button0;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    DEVICE_CREATE_ONCE();
    UEZGUI_EXP_DK_I2CMux_Require();
    // In order to use this, it must use EXPDK_I2C-A provided by the I2C Mux
    UEZDeviceTableFind("EXPDK_I2C-A", &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);

    UEZDeviceTableRegister(
        "ButtonBank0",
        (T_uezDeviceInterface *)&ButtonBank_NXP_PCA9551_Interface,
        0,
        &p_button0);
    ButtonBank_NXP_PCA9551_Configure(p_button0, (DEVICE_I2C_BUS **)p_i2c, 0xC0>>1);
}

/*---------------------------------------------------------------------------*
 * Routine:
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2C GPIO LEDs on EXPDK_I2C-A
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_LED_Require(void)
{
    T_uezDeviceWorkspace *p_led0;
    T_uezDevice i2c;
    T_uezDeviceWorkspace *p_i2c;

    DEVICE_CREATE_ONCE();
    UEZGUI_EXP_DK_I2CMux_Require();
    // In order to use this, it must use EXPDK_I2C-A provided by the I2C Mux
    UEZDeviceTableFind("EXPDK_I2C-A", &i2c);
    UEZDeviceTableGetWorkspace(i2c, (T_uezDeviceWorkspace **)&p_i2c);

    UEZDeviceTableRegister(
        "LEDBank0",
        (T_uezDeviceInterface *)&LEDBank_NXP_PCA9551_Interface,
        0,
        &p_led0);
    LED_NXP_PCA9551_Configure(p_led0, (DEVICE_I2C_BUS **)p_i2c, 0xC0>>1);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_LightSensor_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable the EXP_DK Light Sensor -- a VCNL4010-GS08.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_LightSensor_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Currently, there is no driver for the VCNL4010-GS08.
    // Manual readings of I2C have been performed, but nothing more.

    // In order to use this, it must use EXPDK_I2C-B provided by the I2C Mux
    UEZGUI_EXP_DK_I2CMux_Require();

    // TODO: Make a light sensor driver appear on this!
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_ProximitySensor_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Enable the EXP_DK Proximity Sensor -- a VCNL4010-GS08.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_ProximitySensor_Require(void)
{
    DEVICE_CREATE_ONCE();

    // Currently, there is no driver for the VCNL4010-GS08.
    // Manual readings of I2C have been performed, but nothing more.

    // In order to use this, it must use EXPDK_I2C-B provided by the I2C Mux
    UEZGUI_EXP_DK_I2CMux_Require();

    // TODO: Make a proximity driver appear on this!
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_RS232_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the RS232 connection here.  This just links to a UART
 *      on the platform.  Let's setup an alias to whatever that name is.
 *      UART_B on J2-41/42 is what is used for the RS232
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_RS232_Require(
    TUInt32 aWriteBufferSize,
    TUInt32 aReadBufferSize)
{
    DEVICE_CREATE_ONCE()
    ;

    // Setup UARTB into a full duplex RS232 serial port
    UEZPlatform_ExpansionPrimary_UART_B_Require(aWriteBufferSize,
        aReadBufferSize);

    // Setup an alias for this name that is common.
    // NOTE: This alias is a convenience, but the port may already be
    // in use elsewhere as "Console", "UART0", or some other name.
    UEZDeviceTableRegisterAlias(PRIMARY_EXPANSION_UART_B, "EXPDK_RS232");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_FullDuplex_RS485_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a full duplex RS485 connection.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_FullDuplex_RS485_Require(
    TUInt32 aWriteBufferSize,
    TUInt32 aReadBufferSize)
{
    // Need UART A without a stream
    UEZPlatform_ExpansionPrimary_UART_A_Require();

    // Ensure the drive enable pin is always HIGH
    if (GPIO_EXPDK_485DE != GPIO_NONE) {
        UEZGPIOSetMux(GPIO_EXPDK_485DE, 0);
        UEZGPIOSet(GPIO_EXPDK_485DE);
        UEZGPIOOutput(GPIO_EXPDK_485DE);
        UEZGPIOLock(GPIO_EXPDK_485DE);
    }

    // Ensure the receive enable pin is always low
    if (GPIO_EXPDK_485RE != GPIO_NONE) {
        // Drive enable pin must be LOW
        UEZGPIOSetMux(GPIO_EXPDK_485RE, 0);
        UEZGPIOClear(GPIO_EXPDK_485RE);
        UEZGPIOOutput(GPIO_EXPDK_485RE);
        UEZGPIOLock(GPIO_EXPDK_485RE);
    }

    Serial_Generic_FullDuplex_Stream_Create("EXP_RS485",
        PRIMARY_EXPANSION_UART_A, aWriteBufferSize, aReadBufferSize);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_USBHost_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      The USB Host on the EXP_DK is really just a connector (with power
 *      chip).  We just need to make sure it is enabled on the platform.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_USBHost_Require(void)
{
    //DEVICE_CREATE_ONCE();
    UEZPlatform_ExpansionPrimary_USBHost_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_USBDevice_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      The USB Device on the EXP_DK is really just a connector.  We just
 *      need to make sure it is enabled on the platform.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_USBDevice_Require(void)
{
    //DEVICE_CREATE_ONCE();
    UEZPlatform_ExpansionPrimary_USBDevice_Require();
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_SDCard_MCI_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      The SDCard/MicroSD on the EXP_DK goes to a high speed (parallel)
 *      port to the processor.  This routine sets it up and gives it a
 *      drive number.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_SDCard_MCI_Require(TUInt32 aDriveNumber)
{
    //DEVICE_CREATE_ONCE();
    UEZPlatform_ExpansionSecondary_SDCard_MCI_Require(aDriveNumber);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZPlatform_I2S_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the I2S drivers for talking to the expansion board.
 *---------------------------------------------------------------------------*/
void UEZGUI_EXP_DK_I2S_Require(void)
{
#if (UEZ_PROCESSOR==NXP_LPC1788)
#include <Source/Processor/NXP/LPC17xx_40xx/LPC17xx_40xx_I2S.h>
    static const T_LPC17xx_40xx_I2S_Settings settings = {
            GPIO_NONE,  // I2S_RX_SDA   = not used, conflicts
            GPIO_NONE,  // I2S_RX_SCK   = not used, conflicts
            GPIO_NONE,  // I2S_RX_WS    = not used, conflicts
            GPIO_NONE,  // I2S_RX_MCLK  = not used
            GPIO_P0_9,  // I2S_TX_SDA   = P0.9_I2STX_SDA_MOSI1_MAT2.3
            GPIO_P0_7,  // I2S_TX_SCK   = P0.7_I2STX_CLK_SCK1_MAT2.1
            GPIO_P0_8,  // I2S_TX_WS    = P0.8_I2STX_WS_MISO1_MAT2.2
            GPIO_NONE,  // I2S_TX_MCLK  = not used
    };
    DEVICE_CREATE_ONCE();
    LPC17xx_40xx_I2S_Require(&settings);
#endif
    Generic_I2S_Create("I2S", "I2S");
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_GainSpan_PMOD_Require
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the networking for Wireless connections.
 *---------------------------------------------------------------------------*/
#include <Source/Library/Network/GainSpan/CmdLib/GainSpan_CmdLib.h>
#include <Source/Devices/Network/GainSpan/Network_GainSpan.h>
void UEZGUI_EXP_DK_GainSpan_PMOD_Require(void)
{
    // These SPI pins work for both the Application Header and the PMOD Header
    // for GainSpan Modules with SPI configured.
    const T_GainSpan_Network_SPISettings spi_settings = {
        // SPI Settings
        PRIMARY_EXPANSION_SPI,// SSP Name
        1000000, // 1 MHz
        // Pins.
        GPIO_EXPDK_I2SRXSDA_SSEL,       // SPI CS
        GPIO_EXPDK_IRQ_ISPENTRY,        //  WIFI-IRQ
        GPIO_NONE, // iProgramMode not connected
        GPIO_NONE, // iSPIMode not connected
        GPIO_NONE, // iSRSTn not connected (only to RESET_OUT)
        GPIO_NONE, // Factory Reset not connected 
        };

    DEVICE_CREATE_ONCE();

    UEZPlatform_ExpansionPrimary_SPI_A_Require();
    Network_GainSpan_Create("WirelessNetwork0", &spi_settings);
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_USB_Host_Port_B_Detect
 *---------------------------------------------------------------------------*
 * Description:
 *      Detect USB Host mode on mini-AB port using OTG adapter ID pin to GND
 *---------------------------------------------------------------------------*/
TBool UEZGUI_EXP_DK_USB_Host_Port_B_Detect()
{
    TBool IsDevice;

    UEZGPIOLock(GPIO_USB_HOST_ENABLE); // P0_12 on LPC1788 uEZGUIs
    UEZGPIOSet(GPIO_USB_HOST_ENABLE); // disable MIC2025 power for USB
    UEZGPIOOutput(GPIO_USB_HOST_ENABLE);
    UEZGPIOInput(GPIO_EXPDK_USBDID);
    UEZGPIOSetMux(GPIO_EXPDK_USBDID, 0);

    IsDevice = UEZGPIORead(GPIO_EXPDK_USBDID);

    if(!IsDevice){
        UEZGPIOClear(GPIO_USB_HOST_ENABLE);// enable MIC2025 USB Power
    }
    return IsDevice;
}

/*---------------------------------------------------------------------------*
 * Routine:  UEZGUI_EXP_DK_Detect
 *---------------------------------------------------------------------------*
 * Description:
 *      Determine if the EXP_DK is plugged in.  In this case, we'll use
 *      the I2C Mux and see if we can read it with a raw I2C read.
 * Outputs:
 *      TBool                     -- ETrue if found, else EFalse
 *---------------------------------------------------------------------------*/
TBool UEZGUI_EXP_DK_Detect(void)
{
    T_uezError error = UEZ_ERROR_NOT_FOUND;
    T_uezDevice i2c;
    TUInt8 data;

    // Determine if the EXP_DK is plugged in.
    UEZPlatform_ExpansionPrimary_I2C_A_Require();
    error = UEZI2COpen(PRIMARY_EXPANSION_I2C_A, &i2c);
    if (error)
        return EFalse;
    error = UEZI2CRead(i2c, UEZGUI_EXP_DK_I2CMUX_I2C_ADDRESS, 400, &data, 1, 200);
    UEZI2CClose(i2c);

    return (error == UEZ_ERROR_NONE)?ETrue:EFalse;
}
/** @} */
/*-------------------------------------------------------------------------*
 * End of File:  uEZGUI_EXP_DK.c
 *-------------------------------------------------------------------------*/
