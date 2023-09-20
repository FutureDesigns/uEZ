/*-------------------------------------------------------------------------*
 * File:  ub_cell_hal.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include "ubiquios.h"
#include <uEZ.h>
#include <Include/HAL/HAL.h>
#include <Include/HAL/Serial.h>
#include <Types/Serial.h>
#include <UEZGPIO.h>
#include <UEZProcessor.h>
//#include "Ubiquios_Cell_Test.h"

#if (UEZ_PROCESSOR == NXP_LPC4357)
#include <Source/Processor/NXP/LPC43xx/LPC43xx_GPIO.h>
#include <Source/Processor/NXP/LPC43xx/LPC43xx_Serial.h>
#else
#error "MCU not set!"
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define CELL_UART           "UART2"
#define CELL_BAUD_RATE      115200

#define CELL_POWER_GPIO     GPIO_P3_12
#define CELL_RESET_GPIO     GPIO_P2_4
#define CELL_CTS_GPIO       GPIO_P7_3
#define CELL_DTR_GPIO       GPIO_P5_5

#define POWER_PIN           (GPIO_P3_12)
#define RESET_PIN           (GPIO_P2_4)
#define CTS_PIN             (GPIO_P7_3)
#define RTS_PIN             (GPIO_P7_22)
#define DTR_PIN             (GPIO_P5_5)

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static HAL_Serial **G_Cell_serial;
static TBool G_SerialSending = EFalse;
static T_uezQueue G_ReceiveQueue;

UB_RINGBUF_DECLARE(cell_rx_buf, uint8_t, BUFFER_SIZE, 1);
UB_TIMER_DECLARE( cell_uart_rx_task);

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
static void Cell_receiveByte(void* aCallback, TUInt8 aByte);
static void Cell_Transmite_Callback(void* aCallback, TUInt32 aNumBytesAvailable);
static void cell_uart_rx_task(void);

extern void wake_ubiquios_thread(void);

/********************************************************************
 * This function is called by the ubiquiOS cellular core module to
 * request initialization of the cellular module. It must be
 * implemented by platform-specific code.
 *
 * Typically this function will:
 *
 * claim and configure the GPIOs used to control the cellular possibly
 * including power control, reset, and interrupt request lines.
 * Initialize the communication channels.
 *
 ********************************************************************/
void ub_cell_hal_init(void)
{
    T_Serial_Settings settings;
    TUInt32 config;

    ub_ustdio_printf("Cell hal init\n");
#ifdef POWER_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((POWER_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(POWER_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOSet(POWER_PIN);
    UEZGPIOOutput(POWER_PIN);
#endif

#ifdef RESET_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((RESET_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(RESET_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOSet(RESET_PIN);
    UEZGPIOOutput(RESET_PIN);
#endif

#ifdef CTS_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((CTS_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(CTS_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOClear(CTS_PIN);
    //UEZGPIOOutput(CTS_PIN);
    UEZGPIOInput(CTS_PIN);
#endif

#ifdef DTR_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((DTR_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(DTR_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOClear(DTR_PIN);
    UEZGPIOOutput(DTR_PIN);
#endif

#ifdef RTS_PIN
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((RTS_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(RTS_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
    UEZGPIOClear(RTS_PIN);
    UEZGPIOOutput(RTS_PIN);
#endif

#if(UEZ_PROCESSOR == NXP_LPC4357)
    LPC43xx_GPIO4_Require();
    LPC43xx_UART2_Require(GPIO_P4_8, GPIO_P4_9, GPIO_NONE, GPIO_NONE);
#else
#error "MCU not set!"
#endif

    UEZGPIOSet(POWER_PIN);
    UEZTaskDelay(1);
    UEZGPIOClear(POWER_PIN);
    UEZTaskDelay(1000);
    UEZGPIOInput(POWER_PIN);

    settings.iBaud = CELL_BAUD_RATE;
    settings.iWordLength = 8;
    settings.iStopBit = SERIAL_STOP_BITS_1;
    settings.iParity = SERIAL_PARITY_NONE;
    settings.iFlowControl = SERIAL_FLOW_CONTROL_NONE;

    if (HALInterfaceFind(CELL_UART, (T_halWorkspace **)&G_Cell_serial)
            != UEZ_ERROR_NONE) {
        while (1)
            ;
    }
    (*G_Cell_serial)->SetSerialSettings(G_Cell_serial, &settings);
    (*G_Cell_serial)->Configure(G_Cell_serial, 0, Cell_receiveByte,
            Cell_Transmite_Callback);
    (*G_Cell_serial)->Activate(G_Cell_serial);

#if 0
    UEZQueueCreate(10, 1, &G_ReceiveQueue);

    UEZTaskCreate((T_uezTaskFunction)uart_rx_task, "Cell RX", 5 * 1024,
            (void *)0, UEZ_PRIORITY_NORMAL, 0);
#endif
}

/********************************************************************
 * This function is called by the ubiquiOS cellular core module to
 * request power on or power off of the cellular module.
 *
 * Typically, this function will control a voltage regulator enable
 * line to supply or remove power to the cellular module.
 *
 * Implementation of this function is optional.
 *
 * Parameters
 * on  true to power on, false to power off.
 * Returns
 *  UB_STATUS_SUCCESS if the device is ready to receive AT commands
 *  straight away.
 *  UB_STATUS_BUSY if the device will take some time to power on.
 *  In this case communication via AT commands with the modem will
 *  proceed if either ub_cell_hal_power_on_complete() is invoked at
 *  some later time or an unsolicited message is received from the
 *  modem. If neither of these occur before one minute elapses the
 *  power on will be deemed to have failed.
 *  Any other UB_STATUS_ code will be passed to the application to indicate power on failure.
 *
 ********************************************************************/
void ub_cell_hal_set_power_on(bool on)
{
    ub_ustdio_printf("Set cell power %s\n", on ? "on" : "off");
#ifdef CELL_RESET_GPIO
    if (on) {
        /* The CELL_RESET_PIN is set low on initialization. To reset
         * the module on we drive it high for at least a millisecond, which
         * pulls the pin on the module low due to the PMOD adaptor circuit. */

        UEZGPIOClear(CELL_RESET_GPIO);
        UEZTaskDelay(5);
        UEZGPIOSet(CELL_RESET_GPIO);

        /* The NL-SW-LTE-GELS3 takes around 20 seconds to initialize after
         * reset, at which time it will send an AT^SYSSTART message. To wait
         * for this message we return UB_STATUS_BUSY. If no message is received
         * within one minute power on will be deemed to have failed.  */
        //return UB_STATUS_BUSY;
    }
#endif
    //return UB_STATUS_SUCCESS;
}

#if 0
static void cell_uart_rx_task(void)
{
    TUInt32 i;
    TUInt8 data[1000];

    while (1) {
        if (UEZQueueReceive(G_ReceiveQueue, &data[i++], UEZ_TIMEOUT_INFINITE)
                == UEZ_ERROR_NONE) {
            while (UEZQueueReceive(G_ReceiveQueue, &data[i], 100)
                    == UEZ_ERROR_NONE) {
                i++;
            }
            wake_ubiquios_thread();
            ub_cell_hal_receive(data, i);
        }
    }
}

#else
static void cell_uart_rx_task(void)
{
    TUInt32 len;
    TUInt32 i, j;

    while (1) {
        const uint8_t *data;
        data = UB_RINGBUF_PEEK(cell_rx_buf, &len);

        /* Pass data up to higher layer */
        ub_cell_hal_receive(data, (uint16_t)len);
        UB_RINGBUF_GET(cell_rx_buf, NULL, len);

        ub_delay(2);
    }
}
#endif

static void Cell_receiveByte(void* aCallback, TUInt8 aByte)
{
    UB_RINGBUF_PUT(cell_rx_buf, &(aByte), 1);

    //wake_ubiquios_thread();
    UB_TIMER_SET_NOW(cell_uart_rx_task);

    //_isr_UEZQueueSend(G_ReceiveQueue, (void*)&aByte);
}

static void Cell_Transmite_Callback(void* aCallback, TUInt32 aNumBytesAvailable)
{
    G_SerialSending = EFalse;
}

/********************************************************************
 * This function is called by the ubiquiOS cellular core module to
 * transfer a block of data to the cellular module. It must be
 * implemented by platform-specific code.
 *
 * This function will typically write data to the relevant transport
 * (e.g. serial port or USB endpoint).
 *
 * Parameters
 *     data    The data to transmit.
 *     length  The length of the data to transmit.
 *
 ********************************************************************/
void ub_cell_hal_send(const TUInt8 * data, TUInt16 length)
{
    TUInt32 i;

    G_SerialSending = ETrue;
    for (i = 0; i < length; i++) {
        (*G_Cell_serial)->OutputByte(G_Cell_serial, data[i]);
    }
    while (G_SerialSending)
        ;
}

/*-------------------------------------------------------------------------*
 * End of File:  ub_cell_hal.c
 *-------------------------------------------------------------------------*/
