/*
 * ubiquiOS porting layer for STM32 Nucleo: Bluetooth Generic H4
 *
 * Copyright 2015 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#include "ubiquios.h"
#include "ub_bt_bcm434x.h"
#include "ubiquios_bt_bcm434x.h"
#include <UEZ.h>
#include <Include/HAL/HAL.h>
#include <Include/HAL/Serial.h>
#include <Types/Serial.h>
#include <UEZGPIO.h>
#include <UEZProcessor.h>
//#include "licence.def"
#include <Source/Processor/NXP/LPC43xx/LPC43xx_GPIO.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Serial.h>

#define BLE_UART        "UART0"
//#define BLE_POWER       GPIO_P4_14 //Shared with WiFi, power control done in application
#define BLE_RTS         GPIO_P5_17
#define BLE_CTS         GPIO_P4_15
#define BUFFER_SIZE             (1024)

UB_RINGBUF_DECLARE(bt_tx_buf, uint8_t, BUFFER_SIZE, 1);
UB_RINGBUF_DECLARE(bt_rx_buf, uint8_t, BUFFER_SIZE, 1);
UB_TIMER_DECLARE( uart_rx_task);

static TUInt32 g_btuart_tx_length = 0;

/**
 * Tracks whether Bluetooth is enabled.
 */
static volatile TBool G_sending = EFalse;
static volatile TBool G_Receiving = EFalse;
//static T_uezDevice G_ble_uart;

static HAL_Serial **G_ble_serial;
static T_uezQueue G_ReceiveQueue;

static void
uart_rx_task(void);

static void BLE_receiveByte(void* aCallback, TUInt8 aByte);
static void BLE_Transmite_Callback(void* aCallback, TUInt32 aNumBytesAvailable);

/**
 * This function is called by the ubiquiOS Bluetooth core module to initialise
 * the Generic H4 device.
 */
void ub_bt_hal_bcm434x_init(void)
{
    TUInt32 config;
    T_Serial_Settings settings;
    ub_ustdio_printf("BT HAL init\n");

#ifdef BLE_POWER
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((BLE_POWER >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(BLE_POWER, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOClear(BLE_POWER); //Power off
    UEZGPIOOutput(BLE_POWER);
#endif

    settings.iBaud = 9600;
    settings.iWordLength = 8;
    settings.iStopBit = SERIAL_STOP_BITS_1;
    settings.iParity = SERIAL_PARITY_NONE;
    settings.iFlowControl = SERIAL_FLOW_CONTROL_NONE;

#ifdef BLE_CTS
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((BLE_CTS >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(BLE_CTS, GPIO_CONTROL_SET_CONFIG_BITS, config);
    //UEZGPIOOutput(BLE_CTS);
    //UEZGPIOSet(BLE_CTS);
    UEZGPIOInput(BLE_CTS);
#endif
    
#ifdef BLE_RTS
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((BLE_RTS >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(BLE_RTS, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOOutput(BLE_RTS);
    UEZGPIOClear(BLE_RTS);
#endif

#if 0
    UEZPlatform_FullDuplex_UART3_Require(1024, 1024);
    if(UEZStreamOpen(BLE_UART, &G_ble_uart) != UEZ_ERROR_NONE) {
        while(1);
    }

    UEZStreamControl(G_ble_uart, STREAM_CONTROL_GET_SERIAL_SETTINGS, &settings);
    UEZStreamControl(G_ble_uart, STREAM_CONTROL_SET_SERIAL_SETTINGS, &settings);
#else
    LPC43xx_GPIO7_Require();
    LPC43xx_UART0_Require(GPIO_P5_18, GPIO_P4_11, GPIO_NONE, GPIO_NONE);

    if (HALInterfaceFind(BLE_UART, (T_halWorkspace **)&G_ble_serial)
            != UEZ_ERROR_NONE) {
        while (1)
            ;
    }
    (*G_ble_serial)->SetSerialSettings(G_ble_serial, &settings);
    (*G_ble_serial)->Configure(G_ble_serial, 0, BLE_receiveByte,
            BLE_Transmite_Callback);
    (*G_ble_serial)->Activate(G_ble_serial);

    UEZQueueCreate(10, 1, &G_ReceiveQueue);
#endif
    //UEZTaskCreate((T_uezTaskFunction)uart_rx_task, "BLE RX", 5*1024, (void *)0,
    //        UEZ_PRIORITY_NORMAL, 0);
}

extern void wake_ubiquios_thread(void);
/**
 * Bluetooth UART receive task, implemented as a ubiquiOS timer.
 */

#if 0

static void
uart_rx_task(void)
{
    TUInt32 len;
    const uint8_t *data;

    data = UB_RINGBUF_PEEK(bt_rx_buf, &len);

    /* Pass data up to higher layer */
    ub_bt_hal_bcm434x_uart_rx(data, (uint16_t)len);
    UB_RINGBUF_GET(bt_rx_buf, NULL, len);

    (void)UB_RINGBUF_PEEK(bt_rx_buf, &len);
    if (len > 0)
    UB_TIMER_SET_NOW(uart_rx_task);
}

#else
static void uart_rx_task(void)
{
#if 1
    TUInt32 len;
#else
    TUInt8 data[100];
    TUInt32 i;
#endif

#if 1
    uint32_t i, j;
    while (1) {
        const uint8_t *data;
        data = UB_RINGBUF_PEEK(bt_rx_buf, &len);

        if (len == 0) {
            G_Receiving = EFalse;
            return;
        }

        /* Pass data up to higher layer */
        ub_bt_hal_bcm434x_uart_rx(data, (uint16_t)len);
        UB_RINGBUF_GET(bt_rx_buf, NULL, len);

        ub_delay(2);
        //printf("In: ");
        //for(i = 0; i < len; i++){
        //    printf("%02X ", *data++);
        //  data++;
        //  j = 0x1f;
        //  while(i) j--;
        //}
        //printf("\n");
    }
#else
    while(1) {
        i = 0;
        if(UEZQueueReceive(G_ReceiveQueue, &data[i++], UEZ_TIMEOUT_INFINITE) == UEZ_ERROR_NONE) {
            //Got a byte, keep getting bytes until a timeout
            while(UEZQueueReceive(G_ReceiveQueue, &data[i], 100) == UEZ_ERROR_NONE) {
                i++;
            }
            wake_ubiquios_thread();
            ub_bt_hal_bcm434x_uart_rx(data, (TUInt16)i);
        }
    }
#endif
}
#endif
UB_TIMER_DECLARE( uart_tx_task);
static void uart_tx_task(void)
{
    TUInt32 bytesToSend = 0;
    if (G_sending)
        return;

    /* Discard the data that was just sent from ringbuf */
    UB_RINGBUF_GET(bt_tx_buf, NULL, g_btuart_tx_length);
    g_btuart_tx_length = 0;

    /* If stuff left to send... */
    if (!UB_RINGBUF_IS_EMPTY(bt_tx_buf)) {
        const uint8_t *data = UB_RINGBUF_PEEK(bt_tx_buf, &g_btuart_tx_length);
        bytesToSend = g_btuart_tx_length;
        //while(bytesToSend){
        G_sending = ETrue;
        (*G_ble_serial)->OutputByte(G_ble_serial, *data);
        //    data++;
        //    bytesToSend--;
        //    while(G_sending);
        //}
    }
}

static void BLE_receiveByte(void* aCallback, TUInt8 aByte)
{
#if 1
    G_Receiving = ETrue;
    UB_RINGBUF_PUT(bt_rx_buf, &(aByte), 1);

    UB_TIMER_SET_NOW(uart_rx_task);
    //wake_ubiquios_thread();
    /* Schedule a timer to pick this data up in the
     * background.  */
    //UB_TIMER_SET_IF_SOONER(
    //    uart_rx_task,
    //    UB_MILLISECONDS(1));
#else
    _isr_UEZQueueSend(G_ReceiveQueue, (void*)&aByte);
#endif

}

static void BLE_Transmite_Callback(void* aCallback, TUInt32 aNumBytes)
{
    G_sending = EFalse;
    //UB_TIMER_SET_NOW(uart_tx_task);
}

/**
 * This function is called by the ubiquiOS Bluetooth core module to enable or
 * disable power of the Generic H4 device.
 */
void ub_bt_hal_bcm434x_set_power_on(bool on)
{
    ub_ustdio_printf("Set BT power %s\n", on ? "on" : "off");
#ifdef BLE_POWER
    if (on) {
        ub_delay(5);
        UEZGPIOSet(BLE_POWER);
    } else {
        UEZGPIOClear(BLE_POWER);
    }
#endif
}

/**
 * This function is called by the ubiquiOS Bluetooth core module to transmit
 * an octet.
 */
void ub_bt_hal_bcm434x_uart_tx(uint8_t octet)
{
    //printf("Out: %02X\n", octet);
#if 0
    while(G_Receiving) {
        uart_rx_task();
        //wake_ubiquios_thread();
        UEZTaskDelay(10);
    }
    /* Wait for space in the ringbuf */
    while (UB_RINGBUF_IS_FULL(bt_tx_buf));

    /* Lock the ring buf while we put data into it */
    UB_RINGBUF_PUT(bt_tx_buf, (uint8_t *)&octet, 1);

    /* If we're not already transmitting then kick off transmission */
    if (!G_sending)
    uart_tx_task();
#else
    while (UEZGPIORead(BLE_CTS)) {
        //printf(".");
    }
    //UEZGPIOSet(BLE_CTS);
    G_sending = ETrue;
    (*G_ble_serial)->OutputByte(G_ble_serial, octet);
    while (G_sending)
        ;
#endif
}
