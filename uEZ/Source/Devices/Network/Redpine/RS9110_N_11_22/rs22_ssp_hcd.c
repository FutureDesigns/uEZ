/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name : RS22
 Module name  : SPI driver
 File Name    : rs22_ssp_hcd.c

 File Description:

 List of functions:
 rs22_spi_init
 rs22_ssp_receive_start_token
 rs22_ssp_send_and_receive
 rs22_ssp_send_junk_and_receive
 rs22_ssp_send_and_receive_junk

 Author :

 Rev History:
 Sl  By           date        change     details
 ---------------------------------------------------------
 1   Redpine Signals
 ---------------------------------------------------------
 ************************************************************************/

#include "includes.h"
#include "rsi_nic.h"
#include "rsi_spi.h"
#include "Redpine_RS9110_N_11_22.h"
//#include "dynamic_config.h"

/*! \brief The number of test data bytes. */
//#define NUM_BYTES     2

/* Global variables */
/*! \brief Result of the test. */
//bool success = true;

//uint32_t interrupt_received = 0;
/* This is the ISR when the external interrupt comes this gets executed */

/*FUNCTION*********************************************************************
 Function Name  : rs22_ssp_receive_start_token
 Description    : This function reads the data from the RX FIFO till it
 receives the valid start token
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 |     |     |     |
 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

int32_t rs22_ssp_receive_start_token(void) // checked
{
    uint32_t count = 0;
    uint8_t src_buf[] = { 0x00, 0x00, 0x00 };
    uint8_t dst_buf[3];

    while (1) {
        if ((count++) > 60000) {
            return RSI_STATUS_FAIL;
        }
        RedpineRS9110_Internal_TransferBytes(&src_buf[0], &dst_buf[0], 1);

        if ((dst_buf[0] == SSP_START_TOKEN_8)) {
            break;
        }
    }

    return RSI_STATUS_SUCCESS;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_ssp_send_and_receive
 Description    : This is the wrapper function that calls RedpineRS9110_Internal_TransferBytes
 which will transimit and recieve data on the SPI.
 Returned Value : NONE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 src_ptr			 |	X  |	 |     | Pointer to the data to be transmitted on the SPI
 dst_ptr			 |	   | X   |     | recv buffer to collect recieved bytes from the SPI
 count			 |	X  |     |     | Length of the data to be transmitted
 *END****************************************************************************/
void rs22_ssp_send_and_receive( // checked
        uint8_t *src_ptr,
        uint8_t *dst_ptr,
        uint32_t count)
{
    RedpineRS9110_Internal_TransferBytes(src_ptr, dst_ptr, count);
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_ssp_send_junk_and_receive
 Description    : This is the wrapper function. This function transmits junk data and recieve valid data.
 Returned Value : NONE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 dst_ptr          |     |   X |     | recv buffer to collect recieved bytes from the SPI
 count			 | X   |     |     | Length of the data to recieve

 *END****************************************************************************/
void rs22_ssp_send_junk_and_receive(uint8_t *dst_ptr, uint32_t count) // checked
{
    RedpineRS9110_Internal_TransferBytes(NULL, dst_ptr, count);
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_ssp_send_and_receive_junk
 Description    : This is the wrapper function. This function transmits valid data and recieve junk.
 Returned Value : NONE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 src_ptr			 |	X  |	 |     | Pointer to the data to be transmitted on the SPI
 count			 |	X  |     |     | Length of the data to be transmitted

 *END****************************************************************************/
void rs22_ssp_send_and_receive_junk(uint8_t *src_ptr, uint32_t count) // checked
{
    RedpineRS9110_Internal_TransferBytes(src_ptr, NULL, count);

}
