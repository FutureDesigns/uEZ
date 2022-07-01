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
/* This is set in the Makefile */
#ifdef BT_HEADER
#include BT_HEADER
#endif
#include "stm32f4xx_hal.h"

#include "power_hal.h"



/* BT_EN        PA8 */
#define BT_EN_PORT                  (GPIOA)
#define BT_EN_PIN                   (GPIO_PIN_8)
#define BT_EN_ENABLE                (SET)
#define BT_EN_DISABLE               (RESET)

#define BT_UART_INSTANCE            (USART1)

/*
 * BT_UART_RTS on Arduino A4 PC1
 * BT_UART_CTS on Arduino A5 PC0
 */
#define BT_UART_RTS_CTS_PORT        (GPIOC)
#define BT_UART_RTS_PIN             (GPIO_PIN_1)
#define BT_UART_CTS_PIN             (GPIO_PIN_0)


#define BUFFER_SIZE                 (256)
#define BT_SERIAL_BAUD_RATE_BOOT    (115200)
#define BT_SERIAL_BAUD_RATE_FAST    (115200)

/* Ringbuffers for transmit and receive data */
UB_RINGBUF_DECLARE(tx_buf, uint8_t, BUFFER_SIZE, 1);
UB_RINGBUF_DECLARE(rx_buf, uint8_t, BUFFER_SIZE, 1);

UART_HandleTypeDef bt_huart;


/**
 * Tracks whether Bluetooth is enabled.
 */
static bool bt_enabled;


/**
 * This function updates the BaudRate value in the huartandle.Init struct
 * then invokes UART initialisation. It expects that the other fields
 * of the struct have already been populated.
 */
static void
bt_hal_init_usart(uint32_t baud_rate)
{
    bt_huart.Instance = BT_UART_INSTANCE;

    bt_huart.Init.WordLength = UART_WORDLENGTH_8B;
    bt_huart.Init.StopBits = UART_STOPBITS_1;
    bt_huart.Init.Parity = UART_PARITY_NONE;
    bt_huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    bt_huart.Init.Mode = UART_MODE_TX_RX;
    bt_huart.Init.OverSampling = UART_OVERSAMPLING_16;
    bt_huart.Init.BaudRate = baud_rate;
    HAL_UART_Init(&bt_huart);
}


/**
 * This function is called by the ubiquiOS Bluetooth core module to initialise
 * the Generic H4 device.
 */
void
ub_bt_hal_generic_h4_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIOs */
    __GPIOA_CLK_ENABLE(); /* BT_EN */
    __GPIOC_CLK_ENABLE(); /* BT_UART_CTS, BT_UART_RTS */

    /* Configure BT_EN as output */
    GPIO_InitStruct.Pin = BT_EN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(BT_EN_PORT, &GPIO_InitStruct);


    /* Configure BT_UART_RTS_PIN as input */
    GPIO_InitStruct.Pin = BT_UART_RTS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(BT_UART_RTS_CTS_PORT, &GPIO_InitStruct);

    /* Configure BT_UART_CTS_PIN as output, and drive it low */
    GPIO_InitStruct.Pin = BT_UART_CTS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(BT_UART_RTS_CTS_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BT_UART_RTS_CTS_PORT, BT_UART_CTS_PIN, GPIO_PIN_RESET);

    bt_hal_init_usart(BT_SERIAL_BAUD_RATE_BOOT);

    /* Enable RX interrupt (TX will be enabled when we try to transmit) */
    __HAL_UART_ENABLE_IT(&bt_huart, UART_IT_RXNE);

    power_hal_init();
}


/**
 * Bluetooth UART receive task, implemented as a ubiquiOS timer.
 */
UB_TIMER_DECLARE(uart_rx_task);
static void
uart_rx_task(void)
{
    while (1)
    {
        uint32_t len;
        const uint8_t *data;

        __HAL_UART_DISABLE_IT(&bt_huart, UART_IT_RXNE);
        data = UB_RINGBUF_PEEK(rx_buf, &len);
        __HAL_UART_ENABLE_IT(&bt_huart, UART_IT_RXNE);


        if (len == 0)
            return;

        /* Pass data up to higher layer */
        ub_bt_hal_generic_h4_uart_rx(data, (uint16_t)len);

        __HAL_UART_DISABLE_IT(&bt_huart, UART_IT_RXNE);
        UB_RINGBUF_GET(rx_buf, NULL, len);
        __HAL_UART_ENABLE_IT(&bt_huart, UART_IT_RXNE);
    }
}


/**
 * This function is called by the ubiquiOS Bluetooth core module to enable or
 * disable power of the Generic H4 device.
 */
void
ub_bt_hal_generic_h4_set_power_on(bool on)
{
    power_hal_set_power_on(SUBSYSTEM_BT, on);

    HAL_GPIO_WritePin(BT_EN_PORT, BT_EN_PIN, on ? BT_EN_ENABLE : BT_EN_DISABLE);
    if (on)
    {

        while (HAL_GPIO_ReadPin(BT_UART_RTS_CTS_PORT, BT_UART_RTS_PIN) ==
               GPIO_PIN_RESET)
        {
            ub_delay(UB_MILLISECONDS(10));
        }
    }

    bt_enabled = on;
}


/**
 * Transmits data over the Bluetooth UART.
 */
static void
bt_hal_tx_data(const uint8_t *buf, uint16_t len)
{
    while (len--)
    {
        /* Wait until TX queue has room for new data */
        while (UB_RINGBUF_IS_FULL(tx_buf));

        /* Disable transmit interrupt to avoid race conditions on
         * ringbuffer access */
        __HAL_UART_DISABLE_IT(&bt_huart, UART_IT_TXE);

        /* Copy character into transmit buffer */
        UB_RINGBUF_PUT(tx_buf, buf, 1);

        /* Enable transmit interrupt unconditionally */
        __HAL_UART_ENABLE_IT(&bt_huart, UART_IT_TXE);

        buf++;
    }
}


/**
 * This function is called by the ubiquiOS Bluetooth core module to transmit
 * an octet.
 */
void
ub_bt_hal_generic_h4_uart_tx(uint8_t octet)
{
    bt_hal_tx_data(&octet, 1);
}


/**
 * Handle RX not empty and TX empty interrupts
 */
void
USART1_IRQHandler(void)
{
    /* Look for more data to send on the Transmit Empty interrupt */
    if ((__HAL_UART_GET_FLAG(&bt_huart, UART_FLAG_TXE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&bt_huart, UART_IT_TXE) != RESET))
    {
        /* Note: TXE interrupt flag is always set while the DR register is
         *       empty, so we don't have to clear it explicitly. */

        /* If there is more data in our ringbuffer then grab a
         * character and send it */
        if (!UB_RINGBUF_IS_EMPTY(tx_buf) &&
            HAL_GPIO_ReadPin(BT_UART_RTS_CTS_PORT, BT_UART_RTS_PIN) ==
                GPIO_PIN_RESET)
        {
            uint8_t ch;
            UB_RINGBUF_GET(tx_buf, &ch, 1);
            bt_huart.Instance->DR = ch;
        }

        /* Disable TX interrupt if no more bytes in queue */
        if (UB_RINGBUF_IS_EMPTY(tx_buf))
        {
            __HAL_UART_DISABLE_IT(&bt_huart, UART_IT_TXE);
        }
    }

    /* Handle inbound data on the Receive Not Empty interrupt */
    if ((__HAL_UART_GET_FLAG(&bt_huart, UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&bt_huart, UART_IT_RXNE) != RESET))
    {
        /* Note: RXNE interrupt flag is cleared by reading the DR
         *       register, so we don't have to clear it explicitly. */

        uint8_t ch;

        ch = (uint8_t)bt_huart.Instance->DR;
        if (bt_enabled)
        {
            UB_RINGBUF_PUT_TAIL_DROP(rx_buf, &ch, 1);

            /* Schedule a timer to pick this data up in the
             * background. Set it so that at full line rate inbound it
             * will fire before the buffer gets three quarters full. */
            UB_TIMER_SET_IF_SOONER(
                uart_rx_task,
                UB_MILLISECONDS(3 * 1000 * BUFFER_SIZE /
                                (4 * BT_SERIAL_BAUD_RATE_FAST / 10)));
        }
    }
}
