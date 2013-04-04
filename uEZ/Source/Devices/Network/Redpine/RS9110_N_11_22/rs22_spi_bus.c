/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name : RS22
 Module name  : SPI driver
 File Name    : rs22_spi_bus.c

 File Description:

 List of functions:
 rs22_ssp_bus_request_synch

 Author :

 Rev History:
 Sl  By           date        change     details
 ---------------------------------------------------------
 1   Redpine Signals
 ---------------------------------------------------------
 *END*************************************************************************/
#include "includes.h"
#include "rsi_nic.h"
#include "rsi_spi.h"

/*FUNCTION*********************************************************************
 Function Name  : rs22_ssp_bus_request_synch
 Description    : This function prepares the cmd pkt according to the request
 and either writes to or reads from the SSP FIFOs
 Returned Value : Returns the status of the cmd
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 handle           |  X  |     |     | SSP_BUS_HANDLE
 cmd              |     |     |  X  | pointer to the bus cmd structure
 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

uint8_t junk[12] = {
        0x55,
        0x55,
        0x55,
        0x55,
        0x55,
        0x55,
        0x55,
        0x55,
        0x55,
        0x55,
        0x55 };

volatile unsigned glbl_glbl_count = 0;

int32_t rs22_ssp_bus_request_synch(struct bus_cmd_s *cmd) // checked
{
    struct cmd_fmt_s tx_cmd;
    int32_t status = RSI_STATUS_FAIL;
    int32_t tknStatus = RSI_STATUS_SUCCESS;

    uint8_t rx_buf[8];
    uint16_t busy_retry = 0;

    /* Enter the Critical section */
    rs22_memset((uint8_t *)&tx_cmd, 0, sizeof(struct cmd_fmt_s));
    rs22_memset((void *)rx_buf, 0, 8);

    switch (cmd->type) {
        case SPI_INITIALIZATION: {
            /* Form C1 here */
            tx_cmd.buf[0] = CMD_INITIALIZATION;
            /* Form C2 here */
            tx_cmd.buf[1] = 0x00;//Dummy data to send

#if USE_NO_DMA
            rs22_ssp_send_and_receive((uint8_t *)(&tx_cmd.buf[0]), rx_buf, 2);
#else
            rs22_ssp_send_and_receive_with_dma((uint8_t *)(&tx_cmd.buf[0]), rx_buf, 2);
#endif

            /* Check the status */
            if (rx_buf[1] == SPI_STATUS_SUCCESS) {
                status = RSI_STATUS_SUCCESS;
            } else if (rx_buf[1] == SPI_STATUS_FAILURE) {
                status = RSI_STATUS_FAIL;
            } else {
                status = RSI_STATUS_FAIL;
            } /* End if <condition> */
        }
            break;

        case SPI_INTERNAL_READ: {
            /* Prepare the cmd */
            /* Form C1 here */
            tx_cmd.buf[0] = CMD_READ_WRITE;
            tx_cmd.buf[0] |= (uint8_t)(cmd->length & 0x0003);

            /* Only LSB 5 bits of the address are valid */
            /* Form C2 here */
            tx_cmd.buf[1] |= (uint8_t)(cmd->address & 0x3F);
#if USE_NO_DMA
            /* Send the cmd and receive the status */
            rs22_ssp_send_and_receive((uint8_t *)&tx_cmd.buf[0], rx_buf, 2);
#else
            rs22_ssp_send_and_receive_with_dma((uint8_t *)&tx_cmd.buf[0], rx_buf, 2);
#endif

            /* Check the status */
            if ((rx_buf[1] == SPI_STATUS_SUCCESS)) {
                tknStatus = rs22_ssp_receive_start_token();
                if (tknStatus == RSI_STATUS_BUSY) {
                    status = RSI_STATUS_BUSY;
                    break;
                } else if (tknStatus == RSI_STATUS_FAIL) {

                    status = RSI_STATUS_FAIL;
                    break;
                } else {
                    while (0)
                        ;
                } /* End if <condition> */
                /* Send C3 and C4 (length in 2bytes) here */
#if USE_NO_DMA

                glbl_glbl_count = 1;
                /* Send the junk data and receive the required data */
                rs22_ssp_send_and_receive(junk, cmd->data, cmd->length);

                glbl_glbl_count = 2;
#else
                rs22_ssp_send_and_receive_with_dma(junk, cmd->data, cmd->length);
#endif

                status = RSI_STATUS_SUCCESS;
            } else if (rx_buf[1] == SPI_STATUS_BUSY) {
                status = RSI_STATUS_BUSY;
            } else if (rx_buf[1] == SPI_STATUS_FAILURE) {
                status = RSI_STATUS_FAIL;
            } else {

                if (rx_buf[1] == 0xa4)
                    status = RSI_STATUS_FAIL;
            }
        }
            break;

        case SPI_INTERNAL_WRITE: {
            /* Prepare the cmd */
            /* Form C1 here */
            tx_cmd.buf[0] = CMD_READ_WRITE;
            tx_cmd.buf[0] |= CMD_WRITE;
            tx_cmd.buf[0] |= (uint8_t)(cmd->length & 0x0003);

            /* Only LSB 5 bits of the address are valid */
            //tx_cmd.buf[1] |= (uint8_t)(cmd->address & 0x3F);
            /* Form C2 here */
            tx_cmd.buf[1] |= (uint8_t)LE32_TO_CPU((cmd->address & 0xFF000000));
            /* Send C1 & C2 here */
#if USE_NO_DMA
            /* Send the cmd and receive the status */
            rs22_ssp_send_and_receive((uint8_t *)&tx_cmd.buf[0], rx_buf, 2);
#else
            rs22_ssp_send_and_receive_with_dma((uint8_t *)&tx_cmd.buf[0],rx_buf, 2);
#endif
            /* Check the status */
            if (rx_buf[1] == SPI_STATUS_SUCCESS) {
                /* Send C3 & C4 here */
#if USE_NO_DMA
                /* Send the data the junk data */
                /* Here there are no issues while receiving junk data as cmd->length equals 2 only */
                rs22_ssp_send_and_receive(cmd->data, rx_buf, cmd->length);
#else
                rs22_ssp_send_and_receive_with_dma(cmd->data, rx_buf, cmd->length);
#endif
                status = RSI_STATUS_SUCCESS;
            } else if (rx_buf[1] == SPI_STATUS_BUSY) {
                status = RSI_STATUS_BUSY;
            } else if (rx_buf[1] == SPI_STATUS_FAILURE) {
                status = RSI_STATUS_FAIL;
                break;
            } else {
                status = RSI_STATUS_FAIL;
            } /* End if <condition> */
        }
            break;

        case AHB_MASTER_READ: {
            /* Prepare the cmd */
            /* Form C1, C2, C3 and C4 here */
            tx_cmd.buf[0] = CMD_READ_WRITE;
            tx_cmd.buf[0] |= AHB_BUS_ACCESS;
            tx_cmd.buf[0] |= TRANSFER_LEN_16_BITS;

            *(uint16_t *)&tx_cmd.buf[2] = cmd->length;
#if USE_NO_DMA
            /* Send the cmd and receive the response */
            /* Send C1, C2, C3 and C4 here */
            rs22_ssp_send_and_receive((uint8_t *)&tx_cmd.buf[0], &rx_buf[0], 4);
            /* Send the address and receive the response */
            /* Send A1, A2, A3 and A4 here */
            rs22_ssp_send_and_receive((uint8_t *)(&(cmd->address)), &rx_buf[4],
                    4);
#else
            rs22_ssp_send_and_receive_with_dma((uint8_t *)&tx_cmd.buf[0], &rx_buf[0], 4);
            rs22_ssp_send_and_receive_with_dma((uint8_t *)(&(cmd->address)), &rx_buf[4], 4);
#endif
            /* Check the status */
            if (rx_buf[1] == SPI_STATUS_SUCCESS) {
                tknStatus = rs22_ssp_receive_start_token();
                if (tknStatus == RSI_STATUS_BUSY) {
                    status = RSI_STATUS_BUSY;
                } else if (tknStatus == RSI_STATUS_FAIL) {
                    status = RSI_STATUS_FAIL;
                } else {
                    /* Here we can read the data using DMA */
                    //rs22_ssp_send_and_receive_with_dma(junk, cmd->data, cmd->length);

                    rs22_ssp_send_and_receive(junk, cmd->data, cmd->length);
                    status = RSI_STATUS_SUCCESS;
                } /* End if <condition> */
            } else if (rx_buf[1] == SPI_STATUS_BUSY) {
                status = RSI_STATUS_BUSY;
            } else if (rx_buf[1] == SPI_STATUS_FAILURE) {
                status = RSI_STATUS_FAIL;
            } else {
                status = RSI_STATUS_FAIL;
            } /* End if <condition> */
        }
            break;

        case AHB_MASTER_WRITE: {
            /* Prepare the cmd */
            /* Form C1 and C2 here */
            tx_cmd.buf[0] = CMD_READ_WRITE;
            tx_cmd.buf[0] |= CMD_WRITE;
            tx_cmd.buf[0] |= AHB_BUS_ACCESS;
            tx_cmd.buf[0] |= TRANSFER_LEN_16_BITS;

            tx_cmd.buf[1] = 0x00;

            //Here cmd->length need a big endian to little endian converstion
            /* Form C3 and C4 here */

            tx_cmd.buf[2] = LOBYTE(cmd->length);
            tx_cmd.buf[3] = HIBYTE(cmd->length);

            // Lets try the following code for the same
            do {
                /* Send C1 and C2 here */
#if USE_NO_DMA
                rs22_ssp_send_and_receive((uint8_t *)&tx_cmd.buf[0], rx_buf, 2);
#else
                rs22_ssp_send_and_receive_with_dma((uint8_t *)&tx_cmd.buf[0], rx_buf, 2);
#endif
                if (rx_buf[1] == SPI_STATUS_SUCCESS) {
                    /* Send C3 and C4 here first and A1, A2, A3 and A4 here */
#if USE_NO_DMA
                    rs22_ssp_send_and_receive((uint8_t *)&(tx_cmd.buf[2]),
                            &rx_buf[2], 2);
                    rs22_ssp_send_and_receive((uint8_t *)&(cmd->address),
                            &rx_buf[4], 4);
#else
                    rs22_ssp_send_and_receive_with_dma((uint8_t *)&(tx_cmd.buf[2]), &rx_buf[2], 2);
                    rs22_ssp_send_and_receive_with_dma((uint8_t *)&(cmd->address), &rx_buf[4], 4);
#endif
                    break;
                } else if ((rx_buf[1] == SPI_STATUS_BUSY) && busy_retry < 5) {
                    busy_retry++;//Added by srinivas
                    continue;
                } else {

                    break;
                }
            } while (1);
            /* Check the status */
            if (rx_buf[1] == SPI_STATUS_SUCCESS) {
                /* Send the data */
                //Here if it doesn't work then try to use rs22_ssp_send_and_receive,
                //by increasing the rx_buf size. Take care of the increased length,
                //which is not the feasible solution. Otherwise uncomment above for loop code
                /* Send the data here, we can also use DMA */
                //rs22_ssp_send_and_receive_junk_with_dma((uint8_t*)(cmd->data), cmd->length);
                rs22_ssp_send_and_receive_junk((uint8_t*)(cmd->data),
                        cmd->length);
                status = RSI_STATUS_SUCCESS;
            } else if (rx_buf[1] == SPI_STATUS_BUSY) {
                status = RSI_STATUS_BUSY;
            } else if (rx_buf[1] == SPI_STATUS_FAILURE) {
                status = RSI_STATUS_FAIL;
            } else {
                status = RSI_STATUS_FAIL;
            } /* End if <condition> */
        }
            break;

        case AHB_SLAVE_READ: {
            /* Prepare the cmd */
            /* Form C1 here */
            tx_cmd.buf[0] = CMD_READ_WRITE;
            tx_cmd.buf[0] |= AHB_BUS_ACCESS;
            tx_cmd.buf[0] |= AHB_SLAVE;
            tx_cmd.buf[0] |= TRANSFER_LEN_16_BITS;

            //Since we are using only 8-bit granularity, commented the above statement
            //Following supports only for 8-bit granularity
            /* Form C2 here */
            tx_cmd.buf[1] = 0x00;//(uint8_t)cmd->address;

            /* Form C3 and C4 here */
            tx_cmd.buf[2] = LOBYTE(cmd->length);
            tx_cmd.buf[3] = HIBYTE(cmd->length);

            /* Send C1, C2, C3 and C4 here */
#if USE_NO_DMA
            /* Send the cmd and receive the response */
            rs22_ssp_send_and_receive((uint8_t *)&tx_cmd.buf[0], rx_buf, 2);
#else
            rs22_ssp_send_and_receive_with_dma((uint8_t *)&tx_cmd.buf[0], rx_buf, 4);
#endif

            /* Check the status */
            //if (rx_buf[1] == SPI_STATUS_SUCCESS)
            if ((rx_buf[1] != SPI_STATUS_BUSY) && (rx_buf[1]
                    != SPI_STATUS_FAILURE)) {
                rs22_ssp_send_and_receive((uint8_t *)(&(tx_cmd.buf[2])),
                        rx_buf, 2);
                /* Get the Start token first */
                tknStatus = rs22_ssp_receive_start_token();
                if (tknStatus == RSI_STATUS_BUSY) {
                    status = RSI_STATUS_BUSY;
                } else if (tknStatus == RSI_STATUS_FAIL) {
                    status = RSI_STATUS_FAIL;
                } else {

                    /* Reading data using DMA */
                    //rs22_ssp_send_junk_and_receive_with_dma(&((uint8_t *)cmd->data)[0],cmd->length);
                    rs22_ssp_send_junk_and_receive(&((uint8_t *)cmd->data)[0],
                            cmd->length);
                    status = RSI_STATUS_SUCCESS;

                    status = RSI_STATUS_SUCCESS;

                }

            } else if (rx_buf[1] == SPI_STATUS_BUSY) {
                status = RSI_STATUS_BUSY;
            } else if (rx_buf[1] == SPI_STATUS_FAILURE) {
                status = RSI_STATUS_FAIL;
            } else {
                status = RSI_STATUS_FAIL;
            } /* End if <condition> */

        }
            break;

        case AHB_SLAVE_WRITE: {
            /* Prepare the cmd */
            /* Form C1 here */
            tx_cmd.buf[0] = CMD_READ_WRITE;
            tx_cmd.buf[0] |= CMD_WRITE;
            tx_cmd.buf[0] |= AHB_BUS_ACCESS;
            tx_cmd.buf[0] |= AHB_SLAVE;
            tx_cmd.buf[0] |= TRANSFER_LEN_16_BITS;

            /* Form C2 here */
            tx_cmd.buf[1] = (cmd->address);

            tx_cmd.buf[2] = LOBYTE(cmd->length);
            tx_cmd.buf[3] = HIBYTE(cmd->length);

            /* Send C1 and C2 here */
#if USE_NO_DMA
            /* Send the cmd and receive the status */
            rs22_ssp_send_and_receive((uint8_t *)&tx_cmd.buf[0], rx_buf, 2);
#else
            rs22_ssp_send_and_receive_with_dma((uint8_t *)&tx_cmd.buf[0], rx_buf, 2);

#endif
            /* Check the status */
            //	if ((rx_buf[1] == SPI_STATUS_SUCCESS) || (rx_buf[1] == 0xa4))
            if ((rx_buf[1] != SPI_STATUS_BUSY) && (rx_buf[1]
                    != SPI_STATUS_FAILURE)) {
                /* Send C3 and C4 here */
#if USE_NO_DMA
                /* Send the remaining cmd and receive the status */
                rs22_ssp_send_and_receive((uint8_t *)(&(tx_cmd.buf[2])),
                        rx_buf, 2);
#else
                rs22_ssp_send_and_receive_with_dma((uint8_t *)(&(tx_cmd.buf[2])), &rx_buf[2], 2);
#endif
                /* Send data using DMA here */
                /* Above two can be combined -- FIXME: Srinivas */
                //rs22_ssp_send_and_receive_junk_with_dma(cmd->data, cmd->length);
                rs22_ssp_send_and_receive_junk(cmd->data, cmd->length);

                status = RSI_STATUS_SUCCESS;
            } else if (rx_buf[1] == SPI_STATUS_BUSY) {
                status = RSI_STATUS_BUSY;
            } else if (rx_buf[1] == SPI_STATUS_FAILURE) {
                status = RSI_STATUS_FAIL;
            } else {
                status = RSI_STATUS_FAIL;
            } /* End if <condition> */
        }
            break;

        default: {
            status = RSI_STATUS_FAIL;
        }
            break;
    }

    return (status);
}

/* $EOF */
/* Log */
