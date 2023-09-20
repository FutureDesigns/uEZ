/*
 * ubiquiOS porting layer for uEZ: WLAN BCM GSPI
 *
 * Copyright 2015 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorized copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#include <stdio.h>
#include "ubiquios.h"
#include "ub_wlan_hal_bcm_gspi.h"
#include <UEZ.h>
#include <UEZSPI.h>
#include <UEZProcessor.h>
#include <UEZGPIO.h>

//Unused GPIO should be commented out if they can be ignored.
//#define BCM_POWER_GPIO          GPIO_P4_14 //Shared with BLE, power control done in application
//#define BCM_RESET_GPIO          GPIO_P2_1
#define BCM_CS_GPIO             GPIO_P7_15
#define BCM_IRQ_GPIO            GPIO_P3_8
#define BCM_POWER_POL_AL        0 //Active high on Rev1 board  //0 = Active High, 1 = Active low
#define BCM_RESET_MS            (50)
#define BCM_SPI                 "SSP0"
#define BCM_CLOCK_RATE          1000 //Speed is set in KHz, end speed 1MHz
#define BCM_INT_EDGE            GPIO_INTERRUPT_RISING_EDGE

#define BCM_STACK_CONTROL_CS    (1) //0 = driver controlled, 1= UB Stack Controlled
#define BCM_TRANSFER_BITS       8

static T_uezDevice G_BCMSPI = 0;
/* This is set in the Makefile */
#ifdef WLAN_HEADER
#include WLAN_HEADER
#endif

void BCM_GPIO_IRQ_Handler(
        void *aInterruptHandlerWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType);

/********************************************************************
 * This function is called by the ubiquiOS WLAN core module
 * to request initialization of the WLAN HAL. It must be
 * implemented by platform-specific code.
 *
 * Typically this function will:
 * claim and configure the GPIOs used to control the WLAN transceiver,
 * possibly including power control, reset, and interrupt request lines.
 *
 * Initialize the SPI port that will be used to communicate with the WLAN
 * transceiver, along with its ports.
 *
 * install/configure an interrupt handler on the interrupt request line
 * from the WLAN transceiver.
 *
 ********************************************************************/
void ub_wlan_hal_bcm_gspi_init(void)
{
    TUInt32 config;

#ifdef BCM_POWER_GPIO
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((BCM_POWER_GPIO >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(BCM_POWER_GPIO, GPIO_CONTROL_SET_CONFIG_BITS, config);
#if (BCM_POWER_POL_AL == 1)
    UEZGPIOSet(BCM_POWER_GPIO); //Power off
#else
    UEZGPIOClear(BCM_POWER_GPIO); //Power off
#endif
    UEZGPIOOutput(BCM_POWER_GPIO);
#endif

    //Set the rest pin to GPIO
#ifdef BCM_RESET_GPIO
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((BCM_RESET_GPIO >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(BCM_RESET_GPIO, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOClear(BCM_RESET_GPIO);
    UEZGPIOOutput(BCM_RESET_GPIO);
    UEZTaskDelay(BCM_RESET_MS);
    UEZGPIOSet(BCM_RESET_GPIO);
#endif

#ifdef BCM_CS_GPIO
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((BCM_CS_GPIO >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(BCM_CS_GPIO, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOSet(BCM_CS_GPIO);
    UEZGPIOOutput(BCM_CS_GPIO);
#else
#error "CS pin not set!"
#endif

    if (G_BCMSPI == 0) {
        if (UEZSPIOpen(BCM_SPI, &G_BCMSPI) != UEZ_ERROR_NONE) {
            UEZFailureMsg("SPP not found!");
        }
    }

#ifdef BCM_IRQ_GPIO
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((BCM_IRQ_GPIO >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(BCM_IRQ_GPIO, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOInput(BCM_IRQ_GPIO);
    UEZGPIOConfigureInterruptCallback(BCM_IRQ_GPIO, BCM_GPIO_IRQ_Handler, NULL);
    //UEZGPIODisableIRQ( BCM_IRQ_GPIO, BCM_INT_EDGE);
    UEZGPIOEnableIRQ( BCM_IRQ_GPIO, BCM_INT_EDGE);
#else
#error "IRQ pin not set!"
#endif
}

/********************************************************************
 * This function is called by the ubiquiOS WLAN core module to request
 * hard reset of the WLAN transceiver.
 *
 * Typically, this function will briefly assert the reset signal to the
 * WLAN transceiver to effect a hard reset, but on platforms where this
 * line is not available it may resort to power cycling the transceiver.
 *
 * Implementation of this function is optional, and should not be
 * implemented if the platform hardware does not support hard reset.
 *
 * If this function is not implemented, then a 'hard' reset will be
 * performed at the HIP layer.
 *
 ********************************************************************/
void ub_wlan_hal_bcm_gspi_hard_reset(void)
{
#ifdef BCM_RESET_GPIO
    UEZGPIOClear(BCM_RESET_GPIO);
    UEZTaskDelay(BCM_RESET_MS);
    UEZGPIOSet(BCM_RESET_GPIO);
#endif
}

/********************************************************************
 * This function is called by the ubiquiOS WLAN core module to request
 * power on or power off of the WLAN transceiver.
 *
 * Typically, this function will control a voltage regulator enable line
 * to supply or remove power to the WLAN transceiver.
 *
 * Implementation of this function is recommended for platforms where it
 * is possible to control power to the WLAN transceiver. However, on
 * platforms where powering the WLAN transceiver on and off is not
 * supported this function should not be implemented. If this function is
 * not implemented, a hard reset will be performed via
 * ub_wlan_hal_bcm_gspi_hard_reset() whenever the device is to be powered
 * on (NB., this will result in a HIP-layer 'hard' reset if neither this
 * function nor ub_wlan_hal_bcm_gspi_hard_reset() are implemented).
 *
 ********************************************************************/
void ub_wlan_hal_bcm_gspi_set_power_on(bool on)
{
#ifdef BCM_POWER_GPIO
#if (BCM_POWER_POL_AL == 1)
    if(!on) {
#else
    if(on) {
#endif
        UEZGPIOSet(BCM_POWER_GPIO);
    } else {
        UEZGPIOClear(BCM_POWER_GPIO);
    }
#endif
}

/********************************************************************
 * This function is called by the ubiquiOS WLAN core module to request
 * assertion or desertion of the WLAN SPI chip select line. It must
 * be implemented by platform-specific code.
 *
 * This function will typically either control the relevant GPIO directly,
 * or effect the change via the SPI port, depending on the functionality
 * provided in the latter case.
 *
 * Parameters
 * enable true to select, false to de-select.
 *
 ********************************************************************/
void ub_wlan_hal_bcm_gspi_select(bool enable)
{
#ifdef BCM_CS_GPIO
    if (enable) {
        UEZGPIOClear(BCM_CS_GPIO);
    } else {
        UEZGPIOSet(BCM_CS_GPIO);
    }
#else
#error "CS not defined!"
#endif
}

/********************************************************************
 * This function is called by the ubiquiOS WLAN core module to transfer
 * an octet to or from the WLAN transceiver via SPI. It must be
 * implemented by platform-specific code.
 *
 * This function will typically write write_data to the relevant SPI port
 * and wait for the corresponding read octet to arrive before returning it.
 *
 * Parameters
 * write_data The octet to serialize.
 * Returns the octet read back.
 *
 ********************************************************************/
uint8_t ub_wlan_hal_bcm_gspi_rw(uint8_t write_data)
{
    SPI_Request SPIRequest;
    TUInt8 read_data[1];

    //Stack needs to control the data line for this
    //HAL uEZ Driver needs to be changed to support
    //this feature. Check HAL driver ISSPStart and ISSPEnd
    //have the following code.
    //if(aRequest->iCSGPIOPort == NULL){
    //    //GPIO not implemented, assume controlled by app.
    //    return;
    //}
    SPIRequest.iNumTransfers = 1;
    SPIRequest.iBitsPerTransfer = BCM_TRANSFER_BITS;
    SPIRequest.iRate = BCM_CLOCK_RATE;
    SPIRequest.iClockOutPolarity = EFalse;
    SPIRequest.iClockOutPhase = EFalse;
#if BCM_STACK_CONTROL_CS
    SPIRequest.iCSGPIOPort = NULL;
    SPIRequest.iCSGPIOBit = NULL;
#else
    SPIRequest.iCSGPIOPort = GPIO_TO_HAL_PORT(BCM_CS_GPIO);
    SPIRequest.iCSGPIOBit = GPIO_TO_PIN_BIT(BCM_CS_GPIO);
#endif
    SPIRequest.iCSPolarity = EFalse;
    SPIRequest.iDataMISO = read_data;
    SPIRequest.iDataMOSI = &write_data;
    SPIRequest.iFlags = 0;

    UEZSPITransferPolled(G_BCMSPI, &SPIRequest);

    //printf("rw: %02X %02X\n", write_data, read_data[0]);

    return read_data[0];
}

/********************************************************************
 * This function is called by the ubiquiOS WLAN core module to repeatedly
 * transfer a given octet of data to the WLAN transceiver via SPI.
 *
 * This function is not required to be implemented, but may optionally be
 * implemented in cases where a more efficient implementation is possible
 * (for example, using DMA). The default implementation of this function
 * is as follows:
 *
 * while (count--)
 *     ub_wlan_hal_bcm_gspi_rw(data);
 *
 * Note that no guarantees are given about alignment of data.
 *
 * Parameters
 * data The octet of data to transfer to the WLAN transceiver.
 * count Number of times to transfer the octet of data.
 *
 ********************************************************************/
#if 0 //Not implemented by FDI
void ub_wlan_hal_bcm_gspi_write_repeated(const uint8_t data, uint16_t count)
{

}
#endif

#if 0
/********************************************************************
 * This function is called by the ubiquiOS WLAN core module to transfer
 * a block of data to the WLAN transceiver via SPI.
 *
 * This function is not required to be implemented, but may optionally
 * be implemented in cases where a more efficient implementation is possible
 * (for example, using DMA). The default implementation of this function
 * is as follows:
 *
 * while (length--)
 *     ub_wlan_hal_bcm_gspi_rw(*data++);
 *
 * Note that no guarantees are given about alignment of data.
 *
 * Parameters
 * data Pointer to the first octet of data to send.
 * length Number of octets to send.
 *
 ********************************************************************/
void ub_wlan_hal_bcm_gspi_write_block(const uint8_t *data, uint16_t length)

{
    SPI_Request SPIRequest;

    //Allow the driver to control the CS
    SPIRequest.iNumTransfers = (TUInt32)length;
    SPIRequest.iBitsPerTransfer = BCM_TRANSFER_BITS;
    SPIRequest.iRate = BCM_CLOCK_RATE;
    SPIRequest.iClockOutPolarity = EFalse;
    SPIRequest.iClockOutPhase = EFalse;
#if BCM_STACK_CONTROL_CS
    SPIRequest.iCSGPIOPort = NULL;
    SPIRequest.iCSGPIOBit = NULL;
#else
    SPIRequest.iCSGPIOPort = GPIO_TO_HAL_PORT(BCM_CS_GPIO);
    SPIRequest.iCSGPIOBit = GPIO_TO_PIN_BIT(BCM_CS_GPIO);
#endif
    SPIRequest.iCSPolarity = EFalse;
    SPIRequest.iDataMISO = NULL;
    SPIRequest.iDataMOSI = data;
    SPIRequest.iFlags = 0;

    UEZSPITransferPolled(G_BCMSPI, &SPIRequest);

    printf("wb: ");
    for(int i = 0; i < length; i++){
        printf("%02X ", data[i]);
    }
    printf("\n");
}

/********************************************************************
 * This function is called by the ubiquiOS WLAN core module to transfer
 * a block of data from the WLAN transceiver to the host via SPI.
 *
 * This function is not required to be implemented, but may optionally
 * be implemented in cases where a more efficient implementation is
 * possible (for example, using DMA). The default implementation of
 * this function is as follows:
 *
 *  while (length--)
 *      data++ = ub_wlan_hal_bcm_gspi_rw(0xFF);
 *
 *  Note that no guarantees are given about alignment of data.
 *
 * Parameters
 * data Pointer to a buffer to place the received data in.
 * length Number of octets to receive.
 *
 ********************************************************************/
void ub_wlan_hal_bcm_gspi_read_block(uint8_t *data, uint16_t length)
{
    SPI_Request SPIRequest;

    //Allow the driver to control the CS
    SPIRequest.iNumTransfers = (TUInt32)length;
    SPIRequest.iBitsPerTransfer = BCM_TRANSFER_BITS;
    SPIRequest.iRate = BCM_CLOCK_RATE;
    SPIRequest.iClockOutPolarity = EFalse;
    SPIRequest.iClockOutPhase = EFalse;
#if BCM_STACK_CONTROL_CS
    SPIRequest.iCSGPIOPort = NULL;
    SPIRequest.iCSGPIOBit = NULL;
#else
    SPIRequest.iCSGPIOPort = GPIO_TO_HAL_PORT(BCM_CS_GPIO);
    SPIRequest.iCSGPIOBit = GPIO_TO_PIN_BIT(BCM_CS_GPIO);
#endif
    SPIRequest.iCSPolarity = EFalse;
    SPIRequest.iDataMISO = data;
    SPIRequest.iDataMOSI = NULL;
    SPIRequest.iFlags = 0;

    UEZSPITransferPolled(G_BCMSPI, &SPIRequest);

    printf("rb: ");
    for(int i = 0; i < length; i++){
        printf("%02X ", data[i]);
    }
    printf("\n");
}
#endif //testing
/********************************************************************
 * The function is called by the ubiquiOS WLAN core module to enable
 * or disable the interrupt handler for the interrupt request line
 * from the WLAN transceiver. It must be implemented by platform-specific
 * code.
 *
 * Typically, this function will enable or disable the GPIO edge-triggered
 * interrupt for the WLAN interrupt request signal depending on the value
 * of enable.
 *
 * Parameters
 * enable true to enable the interrupt, false to disable.
 *
 ********************************************************************/
void ub_wlan_hal_bcm_gspi_set_interrupt_enabled(bool enable)
{
    if (enable) {
        UEZGPIOEnableIRQ( BCM_IRQ_GPIO, BCM_INT_EDGE);
    } else {
        UEZGPIODisableIRQ( BCM_IRQ_GPIO, BCM_INT_EDGE);
    }
}

extern void wake_ubiquios_thread(void);
/********************************************************************
 * FDI Implement function to signal the stack the interrupt line has
 * pulled low
 *
 ********************************************************************/
void BCM_GPIO_IRQ_Handler(
        void *aInterruptHandlerWorkspace,
        TUInt32 aPortPins,
        T_gpioInterruptType aType)
{
    //This function is exposed by the ubiquiOS WLAN core module and
    //must be called by platform-specific code when WLAN interrupts
    //are enabled (see ub_wlan_hal_bcm_gspi_set_interrupt_enabled())
    //and the interrupt request line from the WLAN transceiver is asserted.
    wake_ubiquios_thread();
    ub_wlan_hal_bcm_gspi_interrupt_handler();
    UEZGPIOClearIRQ(BCM_IRQ_GPIO);
}

/********************************************************************
 * This function is called by the ubiquiOS WLAN core module to read the
 * current state of the interrupt request line from the WLAN transceiver.
 *
 * Typically, this function will read the value of the GPIO pin connected
 * to the WLAN interrupt request signal, and if it is low
 * (i.e., the interrupt is asserted) it will return true, else it will return
 * false.
 *
 * Returns
 * true if the interrupt request signal is currently asserted, otherwise false.
 *
 ********************************************************************/
bool ub_wlan_hal_bcm_gspi_is_interrupt_asserted(void)
{
    if (UEZGPIORead(BCM_IRQ_GPIO)) {
        return true;
    } else {
        return false;
    }
}
