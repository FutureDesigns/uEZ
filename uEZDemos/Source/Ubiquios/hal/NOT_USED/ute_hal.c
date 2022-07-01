/*
 * ubiquiOS porting layer for STM32 Nucleo: UTE with debug logging
 *
 * Copyright 2015 UbiquiOS Technology Limited - All Rights Reserved
 *
 * Unauthorised copying of this software in binary or source code form
 * via any medium is strictly prohibited. This software is proprietary
 * and confidential.
 */
#include "ubiquios.h"
#include "stm32f4xx_hal.h"


#define BUFFER_SIZE              (256)
#define UTE_SERIAL_BAUD_RATE     (115200)

/* Ringbuffers for transmit and receive data */
UB_RINGBUF_DECLARE(tx_buf, uint8_t, BUFFER_SIZE, 1);
UB_RINGBUF_DECLARE(rx_buf, uint8_t, BUFFER_SIZE, 1);

UART_HandleTypeDef ute_huart;


/**
 * Initialise the UTE transport
 */
void
ute_hal_init(void)
{
    /* USART2 is on Port A (TX on PA2 and RX on PA3) so we need to
     * ensure the clock for this port is enabled */
    __GPIOA_CLK_ENABLE();

    ute_huart.Instance = USART2;

    ute_huart.Init.WordLength = UART_WORDLENGTH_8B;
    ute_huart.Init.StopBits = UART_STOPBITS_1;
    ute_huart.Init.Parity = UART_PARITY_NONE;
    ute_huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    ute_huart.Init.Mode = UART_MODE_TX_RX;
    ute_huart.Init.OverSampling = UART_OVERSAMPLING_16;
    ute_huart.Init.BaudRate = UTE_SERIAL_BAUD_RATE;
    HAL_UART_Init(&ute_huart);

    /* Enable RX interrupt (TX will be enabled when we try to transmit) */
    __HAL_UART_ENABLE_IT(&ute_huart, UART_IT_RXNE);
}


UB_TIMER_DECLARE(uart_rx_task);
static void
uart_rx_task(void)
{
    while (1)
    {
        uint32_t len;
        const uint8_t *data;

        __HAL_UART_DISABLE_IT(&ute_huart, UART_IT_RXNE);
        data = UB_RINGBUF_PEEK(rx_buf, &len);
        __HAL_UART_ENABLE_IT(&ute_huart, UART_IT_RXNE);


        if (len == 0)
            return;

        /* Pass received data to UTE */
        ub_ute_hal_receive_data(data, len);

        __HAL_UART_DISABLE_IT(&ute_huart, UART_IT_RXNE);
        UB_RINGBUF_GET(rx_buf, NULL, len);
        __HAL_UART_ENABLE_IT(&ute_huart, UART_IT_RXNE);
    }
}


void
ub_ute_hal_send_data(const uint8_t *buf, uint16_t len)
{
    while (len--)
    {
        /* Wait until TX queue has room for new data */
        while (UB_RINGBUF_IS_FULL(tx_buf));

        /* Disable transmit interrupt to avoid race conditions on
         * ringbuffer access */
        __HAL_UART_DISABLE_IT(&ute_huart, UART_IT_TXE);

        /* Copy character into transmit buffer */
        UB_RINGBUF_PUT(tx_buf, buf, 1);

        /* Enable transmit interrupt unconditionally */
        __HAL_UART_ENABLE_IT(&ute_huart, UART_IT_TXE);

        buf++;
    }
}


/**
 * Handle RX not empty and TX empty interrupts
 */
void
USART2_IRQHandler(void)
{
    /* Look for more data to send on the Transmit Empty interrupt */
    if ((__HAL_UART_GET_FLAG(&ute_huart, UART_FLAG_TXE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&ute_huart, UART_IT_TXE) != RESET))
    {
        /* Note: TXE interrupt flag is always set while the DR register is
         *       empty, so we don't have to clear it explicitly. */

        /* If there is more data in our ringbuffer then grab a
         * character and send it */
        if (!UB_RINGBUF_IS_EMPTY(tx_buf))
        {
            uint8_t ch;
            UB_RINGBUF_GET(tx_buf, &ch, 1);
            ute_huart.Instance->DR = ch;
        }

        /* Disable TX interrupt if no more bytes in queue */
        if (UB_RINGBUF_IS_EMPTY(tx_buf))
        {
            __HAL_UART_DISABLE_IT(&ute_huart, UART_IT_TXE);
        }
    }

    /* Handle inbound data on the Receive Not Empty interrupt */
    if ((__HAL_UART_GET_FLAG(&ute_huart, UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&ute_huart, UART_IT_RXNE) != RESET))
    {
        /* Note: RXNE interrupt flag is cleared by reading the DR
         *       register, so we don't have to clear it explicitly. */

        uint8_t ch;

        ch = (uint8_t)ute_huart.Instance->DR;
        UB_RINGBUF_PUT_TAIL_DROP(rx_buf, &ch, 1);

        /* Schedule a timer to pick this data up in the
         * background. Set it so that at full line rate inbound it
         * will fire before the buffer gets three quarters full. */
        UB_TIMER_SET_IF_SOONER(
            uart_rx_task,
            UB_MILLISECONDS(3 * 1000 * BUFFER_SIZE /
                            (4 * UTE_SERIAL_BAUD_RATE / 10)));
    }
}




/*
 * Debug logging over UTE transport
 *
 * The UTE transport supports tunnelling of debug log (so long as the
 * UTE start-of-frame delimiter - 0xa5 - is not sent) so that both UTE
 * and debug logging can be enabled using a single UART. We take
 * advantage of this functionality below by mapping the ustdout and
 * ustderr file descriptors onto the UTE transport.
 */


/**
 * Output a character in the debug log
 */
static void
debug_putc(ub_opaque_arg_t opaque_arg, char ch)
{
    if (ch == '\n')
        debug_putc(opaque_arg, '\r');

    /* When tunneling debug over the UTE transport we are not allowed
     * to transmit the character 0xA5, since that indicates the start
     * of a UTE control frame. We substitute a space. */
    if (ch == 0xA5)
        ch = ' ';

    ub_ute_hal_send_data((const uint8_t *)&ch, 1);
}


/**
 * Flush method for our debug serial port. Busy loop until all
 * characters have been sent from our ring buffer.
 */
static void
debug_fflush(ub_opaque_arg_t opaque_arg)
{
    UNUSED(opaque_arg);
    while (!UB_RINGBUF_IS_EMPTY(tx_buf));
}


/**
 * Declare a ustdio file descriptor for our serial port so that we can
 * override \c ustdout and \c ustderr below.
 */
static ub_ustdio_file_t debug_serial_file =
{
    debug_putc, debug_fflush,
    { .p = NULL }
};


/**
 * ubiquiOS has weak definitions of \c ustdout and \c ustderr which
 * redirect output to a black hole. We override these here to cause
 * that same output to come out a serial port using the debug HAL
 * we've just implemented.
 */
ub_ustdio_file_t *ustdout = &debug_serial_file;
ub_ustdio_file_t *ustderr = &debug_serial_file;
