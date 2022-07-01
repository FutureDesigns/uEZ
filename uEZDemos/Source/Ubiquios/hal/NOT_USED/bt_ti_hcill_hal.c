/*
 * ubiquiOS porting layer for STM32 Nucleo: TI HCILL Bluetooth
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
#include "flash_hal.h"



/* BT_EN        PA8 */
#define BT_EN_PORT                  (GPIOA)
#define BT_EN_PIN                   (GPIO_PIN_8)
#define BT_EN_ENABLE                (SET)
#define BT_EN_DISABLE               (RESET)

/* BT_CTS       PC0 */
#define BT_CTS_PORT                 (GPIOC)
#define BT_CTS_PIN                  (GPIO_PIN_0)

/* BT_RTS       PC1 */
#define BT_RTS_PORT                 (GPIOC)
#define BT_RTS_PIN                  (GPIO_PIN_1)

#define BT_UART_INSTANCE            (USART1)


#define BUFFER_SIZE                 (256)
#define BT_SERIAL_BAUD_RATE_BOOT    (115200)
#define BT_SERIAL_BAUD_RATE_FAST    (115200)

/* Ringbuffers for transmit and receive data */
UB_RINGBUF_DECLARE(tx_buf, uint8_t, BUFFER_SIZE, 1);
UB_RINGBUF_DECLARE(rx_buf, uint8_t, BUFFER_SIZE, 1);

UART_HandleTypeDef bt_huart;


static bool bt_enabled;


/**
 * This function updates the BaudRate value in the bt_huart.Init struct
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


static void
bt_hal_deinit_usart(void)
{
    bt_huart.Instance = BT_UART_INSTANCE;
    HAL_UART_DeInit(&bt_huart);
}


void
ub_bt_hal_ti_hcill_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    /* GPIOs */
    __GPIOA_CLK_ENABLE();
    __GPIOC_CLK_ENABLE();

    /* Configure BT_EN as output */
    GPIO_InitStruct.Pin = BT_EN_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(BT_EN_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BT_EN_PORT, BT_EN_PIN, RESET);

    /* Configure CTS as input */
    GPIO_InitStruct.Pin = BT_CTS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(BT_CTS_PORT, &GPIO_InitStruct);

    /* Configure RTS as output */
    GPIO_InitStruct.Pin = BT_RTS_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
    HAL_GPIO_Init(BT_RTS_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(BT_RTS_PORT, BT_RTS_PIN, RESET);


    /* Enable RX interrupt (TX will be enabled when we try to transmit) */
    __HAL_UART_ENABLE_IT(&bt_huart, UART_IT_RXNE);

    power_hal_init();
    flash_hal_init();
}


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
        ub_bt_hal_ti_hcill_uart_rx(data, (uint16_t)len);

        __HAL_UART_DISABLE_IT(&bt_huart, UART_IT_RXNE);
        UB_RINGBUF_GET(rx_buf, NULL, len);
        __HAL_UART_ENABLE_IT(&bt_huart, UART_IT_RXNE);

        /* If the receiver buffer is less than half full then we ensure
         * that RTS is cleared to allow reception of more data. */
        if (UB_RINGBUF_COUNT(rx_buf) < (BUFFER_SIZE/2))
        {
            __HAL_UART_DISABLE_IT(&bt_huart, UART_IT_RXNE);
            HAL_GPIO_WritePin(BT_RTS_PORT, BT_RTS_PIN, RESET);
            __HAL_UART_ENABLE_IT(&bt_huart, UART_IT_RXNE);
        }
    }
}


uint32_t
ub_bt_hal_ti_hcill_uart_get_rate_fast(void)
{
    return BT_SERIAL_BAUD_RATE_FAST;
}


void
ub_bt_hal_ti_hcill_uart_set_rate(ub_bt_hal_ti_hcill_uart_rate_t rate)
{
    /* First flush the transmit ringbuf */
    while (!UB_RINGBUF_IS_EMPTY(tx_buf));
    /* Wait while the serial port TX buffer is not empty.
     * NB., when it is empty this does not mean that transmission
     * has completed... there may still be data in the shift
     * register.
     */
    while (__HAL_UART_GET_FLAG(&bt_huart, UART_FLAG_TC) == RESET);

    /* Disable TX and RX interrupts */
    __HAL_UART_DISABLE_IT(&bt_huart, UART_IT_TXE);
    __HAL_UART_DISABLE_IT(&bt_huart, UART_IT_RXNE);

    UB_TIMER_CANCEL(uart_rx_task);

    switch (rate)
    {
    case UB_BT_HAL_TI_HCILL_UART_RATE_BOOT:
        bt_hal_init_usart(BT_SERIAL_BAUD_RATE_BOOT);
        break;

    case UB_BT_HAL_TI_HCILL_UART_RATE_FAST:
        bt_hal_init_usart(BT_SERIAL_BAUD_RATE_FAST);
        break;
    }

    /* Enable RX interrupt (TX will be enabled when we try to transmit) */
    __HAL_UART_ENABLE_IT(&bt_huart, UART_IT_RXNE);
}


void
ub_bt_hal_ti_hcill_set_power_on(bool on)
{
    power_hal_set_power_on(SUBSYSTEM_BT, on);
    bt_enabled = on;

    if (on)
    {
        HAL_GPIO_WritePin(BT_EN_PORT, BT_EN_PIN, BT_EN_ENABLE);
        bt_hal_init_usart(BT_SERIAL_BAUD_RATE_BOOT);
        while (HAL_GPIO_ReadPin(BT_CTS_PORT, BT_CTS_PIN) == GPIO_PIN_RESET)
            ub_delay(UB_MILLISECONDS(10));
    }
    else
    {
        HAL_GPIO_WritePin(BT_RTS_PORT, BT_RTS_PIN, RESET);
        HAL_GPIO_WritePin(BT_EN_PORT, BT_EN_PIN, BT_EN_DISABLE);
        bt_hal_deinit_usart();
    }

}


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


void
ub_bt_hal_ti_hcill_uart_tx(uint8_t octet)
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
                HAL_GPIO_ReadPin(BT_CTS_PORT, BT_CTS_PIN) == GPIO_PIN_RESET)
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

        /* When the buffer is nearly full assert RTS to prevent the other
         * end from transmitting any more data. */
        if (UB_RINGBUF_COUNT(rx_buf) >= (BUFFER_SIZE - 24))
        {
            HAL_GPIO_WritePin(BT_RTS_PORT, BT_RTS_PIN, SET);
        }
    }
}


ub_status_t
ub_bt_hal_ti_get_bts_block(uint32_t offset, uint32_t *length, uint8_t *buffer)
{
    return flash_hal_read(FLASH_HAL_IDX_BT, offset, length, buffer);
}
