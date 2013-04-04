/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name : RS22
 Module name  : SPI driver
 File Name    : rs22_spi_pine.c

 File Description:

 List of functions:
 rsi_send_ssp_init_cmd
 rsi_send_ssp_read_intrStatus_cmd
 rsi_send_ssp_intrAck_cmd
 rs22_send_ta_load_cmd
 rs22_send_ta_sft_rst_cmd
 rs22_read_qstatus
 rs22_send_ssp_wakeup_cmd
 rs22_send_ssp_wakeup_cmd_read
 rs22_send_ta_config_params_cmd
 rs22_send_dummy_write
 rs22_init_interface


 Author :

 Rev History:
 Sl  By          date change   details
 ---------------------------------------------------------
 1   Srinivas    11 Sep 09
 ---------------------------------------------------------
 *END*************************************************************************/

#include "includes.h"

#include <uEZ.h>
#include "rsi_nic.h"
//#include "iodefine.h"
#include "rsi_api.h"
//#include "leds.h"
#include "rsi_util.h"
#include "rsi_spi.h"

#if BOOT_LOAD
#include "rs22_firmware.h"

#define TA_SB_IM1_BASE_ADDR 0x00000000
#define TA_SB_IM2_BASE_ADDR 0x2014010
#define TA_SB_DM_BASE_ADDR  0x2000000 //0x20003100
#define TA_IU_IM1_BASE_ADDR 0x00000000
#define TA_IU_IM2_BASE_ADDR 0x2000000
#define TA_IU_DM_BASE_ADDR  0x20003100

#define SBTADM2_BASE_ADDR     0x2010000

#define IU_BASE_ADDR     	0x2008000
#define CONFIG_TA_ADDR   	0x5F00

/*FUNCTION*********************************************************************
 Function Name  : rsi_load_software
 Description    : This function loads the softeware boot loader
 and image upgradation.
 Returned Value : SUCCESS or FAILURE
 Parameters     : type

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 type             | X   |    |     | To switch to appropriate function
 *END****************************************************************************/
int32_t rsi_load_firmware(uint16_t type) // checked
{
    int32_t status = 0;
    switch (type) {
        case MGMT_LOAD_SOFT_BOOTLOADER:
            status = rs22_load_instructions(1);
            break;

#ifdef LOAD_TA_INTSTRUCTION 
        case MGMT_LOAD_IMAGE_UPGRADER:
            status = rs22_load_instructions(0);
            break;
#ifndef EVAL_LICENCE		   
        case MGMT_UPGRADE_FF_TAIM1:
            status = rs22_load_TA_instructions(0);
            break;
        case MGMT_UPGRADE_FF_TAIM2:
            status = rs22_load_TA_instructions(1);
            break;
        case MGMT_UPGRADE_FF_TADM:
            status = rs22_load_TA_instructions(2);
            break;
#endif  //EVAL_LICENCE
#endif		   
#ifdef LOAD_INST_FROM_HOST
        case LOAD_SB_TADM2:
            status = rs22_load_TA_instructions(3);
            break;
#endif
        default:
            status = -1;
            break;
    }
    return (status);
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_load_instructions
 Description    : This functions loads TA instructions and data memory
 Returned Value : On success 0 will be returned else a negative number
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 pAdapter         |     |  X  |     | pointer to our adapter
 *END**************************************************************************/
int32_t rs22_load_instructions(uint16_t sb) // checked
{
    uint32_t data;
    uint32_t block_size = 256;
    uint32_t instructions_sz = 0;
    uint32_t num_blocks;
    uint32_t cur_indx = 0, ii, i;
    uint8_t *ta_firmware = NULL;
    uint32_t src_address = 0;

    do {

        data = CPU_TO_LE32(TA_SOFT_RST_SET);

        if (rs22_send_ta_sft_rst_cmd(data) != RS22_STATUS_SUCCESS) {
            return -1;
        }

        for (i = 0; i < 3; i++) //changed
        {
            /*loading in non-multi block mode*/
            switch (i) {
                case 0: {
                    if (sb) {
                        instructions_sz = sizeof(sbtaim1); //changed
                        ta_firmware = (uint8_t *)&sbtaim1[0]; //changed
                        src_address = TA_SB_IM1_BASE_ADDR;
                    }
#ifndef EVAL_LICENCE				
                    else {

                        instructions_sz = sizeof(iutaim1);
                        ta_firmware = (uint8_t *)&iutaim1[0];
                        src_address = TA_IU_IM1_BASE_ADDR;
                        i++;
                    }
#endif //EVAL_LICENCE				
                }
                    break;
                case 1: {
                    if (sb) {
                        instructions_sz = sizeof(sbtaim2);
                        ta_firmware = (uint8_t *)&sbtaim2[0];
                        src_address = TA_SB_IM2_BASE_ADDR;
                    }
#ifndef EVAL_LICENCE						
                    else {

                        instructions_sz = sizeof(iutaim2);
                        ta_firmware = (uint8_t *)&iutaim2[0];
                        src_address = TA_IU_IM2_BASE_ADDR;

                    }
#endif  //EVAL_LICENCE						
                }
                    break;
                case 2: {
                    if (sb) {
                        instructions_sz = sizeof(sbtadm);
                        ta_firmware = (uint8_t *)&sbtadm[0];
                        src_address = TA_SB_DM_BASE_ADDR;
                    }
#ifndef EVAL_LICENCE						
                    else {

                        instructions_sz = sizeof(iutadm);
                        ta_firmware = (uint8_t *)&iutadm[0];
                        src_address = TA_IU_DM_BASE_ADDR;

                    }
#endif //EVAL_LICENCE						
                }
                    break;
            }

            if (ta_firmware == NULL) {
                return -1;
            }
            if (instructions_sz % 4) {
                instructions_sz += 4 - (instructions_sz % 4);
            }

            num_blocks = instructions_sz / block_size;

            for (cur_indx = 0, ii = 0; ii < num_blocks; ii++, cur_indx
                    += block_size) {
                if (rs22_send_ta_load_cmd(&ta_firmware[cur_indx], block_size,
                        (src_address + cur_indx)) != RS22_STATUS_SUCCESS) {
                    return -1;
                }
            }

            /* This should be 4 byte aligned */
            if (instructions_sz % block_size) {
                if (rs22_send_ta_load_cmd(&ta_firmware[cur_indx],
                        (instructions_sz % block_size),
                        (src_address + cur_indx)) != RS22_STATUS_SUCCESS) {
                    return -1;
                }
            }
        }
        data = CPU_TO_LE32(TA_SOFT_RST_CLR);

        /*bringing TA out of reset*/
        if (rs22_send_ta_sft_rst_cmd(data) != RS22_STATUS_SUCCESS) {
            return -1;
        }
    } while (FALSE);
    //rs22_load_dummy_data();
    return 0;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_load_TA_instructions
 Description    : This functions loads TA instructions and data memory
 Returned Value : On success 0 will be returned else a negative number
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 pAdapter         |     |  X  |     | pointer to our adapter
 *END**************************************************************************/
#ifdef LOAD_TA_INTSTRUCTION 
int32_t rs22_load_TA_instructions(uint8_t i) // checked
{
    uint32_t block_size = 256;
    uint32_t instructions_sz = 0;
    uint32_t num_blocks;
    uint32_t cur_indx = 0, ii;
    uint8_t *ta_firmware = NULL;
    uint32_t src_address = 0;

#ifndef EVAL_LICENCE		  
    uint8_t buffer[16];
#endif //EVAL_LICENCE		  
    do {
        /*loading in non-multi block mode*/
        switch (i) {
#ifndef EVAL_LICENCE		
            case 0: {
                /* Load fftaim1 */
                instructions_sz = sizeof(fftaim1);
                ta_firmware = (uint8_t *)&fftaim1[0];
                src_address = IU_BASE_ADDR;
            }
                break;
            case 1: {
                /* Load fftaim2 */
                instructions_sz = sizeof(fftaim2);
                ta_firmware = (uint8_t *)&fftaim2[0];
                src_address = IU_BASE_ADDR;
            }
                break;
            case 2: {
                /* Load fftadm */
                instructions_sz = sizeof(fftadm);
                ta_firmware = (uint8_t *)&fftadm[0];
                src_address = IU_BASE_ADDR;
            }
                break;
#endif  //EVAL_LICENCE
#ifdef LOAD_INST_FROM_HOST

            case 3: {
                instructions_sz = sizeof(sbtadm2);
                ta_firmware = (uint8_t *)&sbtadm2[0];
                src_address = SBTADM2_BASE_ADDR;
            }
                break;
#endif
        }
        if (ta_firmware == NULL) {
            return -1;
        }
        /* This should be 4 byte alligned */
        if (instructions_sz % 4) {
            instructions_sz += 4 - (instructions_sz % 4);
        }

        num_blocks = instructions_sz / block_size;

        for (cur_indx = 0, ii = 0; ii < num_blocks; ii++, cur_indx
                += block_size) {
            if (rs22_send_ta_load_cmd(&ta_firmware[cur_indx], block_size,
                    (src_address + cur_indx)) != RS22_STATUS_SUCCESS) {
                return -1;
            }
        }

        /* This should be 4 byte aligned */
        if (instructions_sz % block_size) {
            if (rs22_send_ta_load_cmd(&ta_firmware[cur_indx], (instructions_sz
                    % block_size), (src_address + cur_indx))
                    != RS22_STATUS_SUCCESS) {
                return -1;
            }
        }

#ifndef EVAL_LICENCE
        switch (i) {
            case 0: {
                /* upgrade TAIM1 */
                rs22_memset(buffer, 0, 16);
                buffer[1] = ((MGMT_UPGRADE_FF_TAIM1 & 0xff00) >> 8);
                buffer[0] = (MGMT_UPGRADE_FF_TAIM1 & 0x00ff);
                buffer[3] = ((instructions_sz & 0xff00) >> 8);
                buffer[2] = (instructions_sz & 0x00ff);
                rs22_send_TA_mgmt_frame((uint8_t *)&buffer, RSI_DESC_LEN );
            }
                break;

            case 1: {
                /* upgrade TAIM2 */
                rs22_memset(buffer, 0, 16);
                buffer[1] = ((MGMT_UPGRADE_FF_TAIM2 & 0xff00) >> 8);
                buffer[0] = (MGMT_UPGRADE_FF_TAIM2 & 0x00ff);
                buffer[3] = ((instructions_sz & 0xff00) >> 8);
                buffer[2] = (instructions_sz & 0x00ff);
                rs22_send_TA_mgmt_frame((uint8_t *)&buffer, RSI_DESC_LEN );
            }
                break;

            case 2: {
                /* upgrade TADM */
                rs22_memset(buffer, 0, 16);
                buffer[1] = ((MGMT_UPGRADE_FF_TADM & 0xff00) >> 8);
                buffer[0] = (MGMT_UPGRADE_FF_TADM & 0x00ff);
                buffer[3] = ((instructions_sz & 0xff00) >> 8);
                buffer[2] = (instructions_sz & 0x00ff);
                rs22_send_TA_mgmt_frame((uint8_t *)&buffer, RSI_DESC_LEN );
                break;
            }
            default:
                break;
        }
#endif	//EVAL_LICENCE
    } while (FALSE);

    return 0;
}
#endif
#endif

/*FUNCTION*********************************************************************
 Function Name  : rs22_card_write
 Description    : This function writes frames to the WLAN card
 Returned Value : On success RS22_STATUS_SUCCESS will be returned
 else RS22_STATUS_FAILURE is returned
 Parameters     :

 -----------------------+-----+-----+-----+------------------------------
 Name                   | I/P | O/P | I/O | Purpose
 -----------------------+-----+-----+-----+------------------------------
 pkt                    |     |  X  |     | Pointer to the buffer
 Len                    | X   |     |     | len of the data
 queueno                | X   |     |     | queue number
 *END**************************************************************************/

RS22_STATUS rs22_card_write(uint8_t *pkt, uint32_t Len, uint32_t queueno) // checked
{
    struct bus_cmd_s cmd;
    int32_t Status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;
    uint32_t frag = Len % 4;

    if (frag) {
        /* If we are running at 32 bit granularity length should be 4 byte aligned */
        Len += (4 - frag);
    }

    /* Prepare the AHB_SLAVE_WRITE cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = AHB_SLAVE_WRITE;
    cmd.data = (void *)pkt;
    cmd.length = Len;
    cmd.address = queueno;

    /* Send the cmd to the BUS */
    while (ii < 50000) {
        Status = rs22_ssp_bus_request_synch(&cmd);
        if ((Status == RSI_STATUS_BUSY) || (Status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (Status != RS22_STATUS_SUCCESS) {
        return Status;
    }

    return Status;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_send_mgmt_frame
 Description    : This is a wrapper function which will invoke
 rs22_card_write to send the given mangement packet.
 Returned Value : On success RS22_STATUS_SUCCESS will be returned
 else RS22_STATUS_FAILURE is returned
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 Adaptor          |  X  |     |     | pointer to our adapter
 frame            |  X  |     |     | management frame which is going to send
 len              |  X  |     |     | length of the managemnet frame
 *END**************************************************************************/
RS22_STATUS rs22_send_mgmt_frame(uint8_t frame[], uint32_t len)
{
    RS22_STATUS status = RS22_STATUS_SUCCESS;
    ((uint8_t *)frame)[14] = SND_MGMT_Q;

    /* Send the 16 byte frame descriptor */
    status = rs22_card_write(frame, 16, SND_MGMT_Q);

    if (status != RS22_STATUS_SUCCESS) {
        return (status);
    }

    /* Send the Remaining data */
    status = rs22_card_write(&frame[16], (len - 16), SND_MGMT_Q);

    if (status != RS22_STATUS_SUCCESS) {
        return status;
    }

    return status;
}

/*************************************************************************
 Function Name  : rs22_send_TA_mgmt_frame
 Description    : This is a wrapper function which will invoke
 rs22_card_write to send the given mangement packet.
 Returned Value : On success RS22_STATUS_SUCCESS will be returned
 else RS22_STATUS_FAILURE is returned
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 Adaptor          |  X  |     |     | pointer to our adapter
 frame            |  X  |     |     | management frame which is going to send
 len              |  X  |     |     | length of the managemnet frame
 *END**************************************************************************/
RS22_STATUS rs22_send_TA_mgmt_frame(uint8_t frame[], uint32_t len) // checked
{
    RS22_STATUS status = RS22_STATUS_SUCCESS;
    /* Add the queue number in the frame descriptor */
    ((uint8_t *)frame)[14] = SND_TA_MGMT_Q;

    /* Send the 16 byte frame descriptor */
    status = rs22_card_write(frame, 16, SND_TA_MGMT_Q);

    if (status != RS22_STATUS_SUCCESS) {
        return (status);
    }

    if (len > 16) {
        /* Send the Remaining data */
        status = rs22_card_write(&frame[16], (len - 16), SND_TA_MGMT_Q);

        if (status != RS22_STATUS_SUCCESS) {
            return status;
        }
    }
    return status;
}

/*************************************************************************
 Function Name  : rs22_send_data_frame
 Description    : This is a wrapper function which will invoke
 rs22_card_write to send the given data packet.
 Returned Value : On success RS22_STATUS_SUCCESS will be returned
 else RS22_STATUS_FAILURE is returned
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 frame            |  X  |     |     | data frame which is going to send
 len              |  X  |     |     | length of the data frame
 *END**************************************************************************/
RS22_STATUS rs22_send_data_frame(uint8_t frame[], uint32_t len)
{
    RS22_STATUS status = RS22_STATUS_SUCCESS;
    /* Add the queue number in the frame descriptor */
    ((uint8_t *)frame)[14] = SND_DATA_Q;
    /* Send the 16 byte frame descriptor */
    status = rs22_card_write(frame, 16, SND_DATA_Q);

    if (status != RS22_STATUS_SUCCESS) {
        return (status);
    }
    if (len > 16) {
        /* Send the data */
        status = rs22_card_write(&frame[16], (len - 16), SND_DATA_Q);

        if (status != RS22_STATUS_SUCCESS) {
            return status;
        }
    }
    return status;
}
/*FUNCTION*********************************************************************
 Function Name  : rsi_card_read
 Description    : This function read frames from the SD card
 Returned Value : On success RS22_STATUS_SUCCESS will be returned
 else RS22_STATUS_FAILURE is returned
 Parameters     :

 -----------------------+-----+-----+-----+------------------------------
 Name                   | I/P | O/P | I/O | Purpose
 -----------------------+-----+-----+-----+------------------------------
 frame_desc             |     |     |     |
 ptr                    |     |     |     |
 pktlen                 |     |     |     |
 queueno                |     |     |     |
 *END**************************************************************************/
volatile uint32_t length1 = 0;
volatile uint8_t te_len = 0;

RS22_STATUS rsi_card_read(
        uint8_t *frame_desc,
        uint8_t ***ptr,
        uint32_t *pktLen,
        uint32_t *queueno)
{
    uint16_t ii = 0;
    struct bus_cmd_s cmd;
    int32_t Status = RS22_STATUS_SUCCESS;
    uint32_t frag = 0;
    int32_t flag = 0;

    /* Read the descriptor first */
    /* Prepare the AHB_SLAVE_READ cmd */
    rs22_memset(&cmd,0,sizeof(struct bus_cmd_s));
    cmd.type = AHB_SLAVE_READ;
    cmd.data = frame_desc;
    cmd.length = FRAME_DESC_SZ;

    /* Send the cmd to the BUS */
    while (ii < 50000) /* This will try for 50000 times if the SPI SLAVE is busy */
    {
        Status = rs22_ssp_bus_request_synch(&cmd);
        if ((Status == RSI_STATUS_BUSY) || (Status == RSI_STATUS_FAIL)) {
            ii++;
            continue; /* Send the command again */
        }
        break;
    }

    if (Status != RS22_STATUS_SUCCESS) {
        return Status;
    }

    /* Extarct the queueu number from the recieved data */
    *queueno = frame_desc[14] & RSI_DESC_QUEUE_NUM_MASK;
    if (*queueno == RCV_LMAC_MGMT_Q) {/* LMAC Management queue */
        if (frame_desc[1] & 0x01) {/* Length is present */

            /* Extract  the length */
            length1 = LE16_TO_CPU(*(uint16_t *)&frame_desc[4]);
        } else {/* Length is 0 */

            length1 = frame_desc[0];
        } /* End if <condition> */
    } else if (*queueno == RCV_TA_MGMT_Q) {
        te_len = frame_desc[0];
        te_len = te_len & 0xFF;
        if (te_len != 0) {/* Length is present and data is present  */

            length1 = CPU_TO_LE16(*(uint16_t *)&frame_desc[8]);
        } else {/* Length is 0 and data is not present */

            te_len = frame_desc[0];
            length1 = te_len; //frame_desc[0];
            return Status;
        } /* End if <condition> */
    } else {
        /* Data pkt */
        if (frame_desc[15] & RSI_DESC_AGGR_ENAB_MASK) {
            uint16_t ii, no_pkts, cur_length;
            no_pkts = (frame_desc[15]) & 0x7;

            for (ii = 0; ii < no_pkts; ii++) {
                cur_length
                        = CPU_TO_LE16((*(uint16_t *)&frame_desc[ii * 2]) & 0xFFF);

                length1 += cur_length;
                frag = cur_length % 4;
                if (frag) {
                    /* Allign the data length to 4 bytes */
                    length1 += (4 - frag);
                }
            }
        } else {
            length1 = ((frame_desc[1] << 8) + (frame_desc[0] & 0x0FFF));
            if (length1 > RSI_RCV_BUFFER_LEN) {
                length1 = RSI_RCV_BUFFER_LEN;
            }
            if (length1 < RSI_HEADER_SIZE) {
                while (0)
                    ;
            }
            flag = 1;
            *pktLen = length1;
            if (length1 % 4) {
                /* Allign the recv data length to 4 bytes */
                length1 += (4 - length1 % 4);
            }
        }
    }

    if (length1 != 0) {
        if (flag == 0) {
            *pktLen = length1;
            frag = length1 % 4;
            if (frag) {
                length1 += (4 - frag);
            }
        }
        /* Read the data frame */
        /* Prepare the AHB_SLAVE_READ cmd */
        rs22_memset(&cmd, 0,sizeof(struct bus_cmd_s));
        cmd.type = AHB_SLAVE_READ;
        cmd.data = **ptr;
        cmd.length = length1;

        /* Send the cmd to the BUS */
        while (ii < 50000) {
            Status = rs22_ssp_bus_request_synch(&cmd);
            if ((Status == RSI_STATUS_BUSY) || (Status == RSI_STATUS_FAIL)) {
                ii++;
                continue;
            }
            break;
        }
    }
    if (Status != RS22_STATUS_SUCCESS) {
        return Status;
    }
    return Status;
}
/*FUNCTION*********************************************************************
 Function Name  : rsi_send_init_request
 Description    : This function  sends the wlan init request
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------

 *END****************************************************************************/
int32_t rsi_send_init_request(void)
{
    RS22_STATUS status = RS22_STATUS_SUCCESS;
    struct RSI_Mac_frame_t mgmt;

    rs22_memset(mgmt.descriptor, 0, 8);
#if NEW_FIRMWARE
    mgmt.descriptor[0] = CPU_TO_LE16(MGMT_DESC_RS22_INIT | 68);
    mgmt.descriptor[3] = 0x5555;
    status = rs22_send_TA_mgmt_frame((uint8_t *)&mgmt, RSI_DESC_LEN + 68);
#else
    mgmt.descriptor[0] = CPU_TO_LE16(MGMT_DESC_RS22_INIT | sizeof(struct RSI_Mac_frame_t));
    status = rs22_send_TA_mgmt_frame((uint8_t *)&mgmt, RSI_DESC_LEN + sizeof(struct RSI_Mac_frame_t));
#endif

    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        while (0)
            ;
    }
    return (status);
}
RS22_STATUS rsi_firmware_upgrade_receive(
        uint8_t **DataRcvPacket,
        uint32_t *size)
{
    RS22_STATUS Status = RS22_STATUS_FAILURE;
    uint32_t pktLen;
    uint32_t q_no;
    uint16_t type;
    uint8_t desc[16];

    do {
        Status = rsi_card_read(desc, &DataRcvPacket, &pktLen, &q_no);

        if (Status != RS22_STATUS_SUCCESS) {
            break;
        }

        switch (q_no) {

            case RCV_TA_MGMT_Q: {/* TA management queue */
                if (desc[0] & 0xFF) {
                    pktLen = LE16_TO_CPU(*(uint16_t *)&(desc[8]));
                } else {
                    pktLen = LE32_TO_CPU((uint32_t)(desc[0]));
                } /* End if <condition> */

                if (!pktLen) {
                    pktLen = 4;
                }
                pktLen += 16;

                /* Type is upper 5 bits of descriptor's second byte */
                type = ((desc[1] & 0xFF) << 8);

                switch (type) {

                    /* software boot loader TAIM1 loaded */
                    case MGMT_DESC_TAIM1_DONE_RESPONSE: {
                        rsi_load_firmware((uint16_t)MGMT_UPGRADE_FF_TAIM2);
                    }
                        break;
                        /* software boot loader TAIM2 loaded */
                    case MGMT_DESC_TAIM2_DONE_RESPONSE: {
                        rsi_load_firmware((uint16_t)MGMT_UPGRADE_FF_TADM);
                    }
                        break;
                        /* software boot loader TADM loaded */
                    case MGMT_DESC_TADM_DONE_RESPONSE: {
                        Status = RS22_FW_UPGRADE_SUCCESS;
                    }
                        break;

                    default: {
                        while (0)
                            ;
                    }
                }
            }
                break;
            default: {
                while (0)
                    ;
                break;
            }
        }
    } while (FALSE);

    return Status;
}

#ifndef EVAL_LICENCE
int16_t rsi_upgrade_firmware(void)
{
    volatile RS22_STATUS Status = RS22_STATUS_FAILURE;
    INTERRUPT_TYPE InterruptType;
    uint32_t InterruptStatus = 0;
    uint8_t rcv_pkt[50];
    uint8_t *RcvPacket = &rcv_pkt[0];// Adaptor.RcvPkt;
    uint32_t size;

    rsi_load_firmware((uint16_t)MGMT_UPGRADE_FF_TAIM1);
    //TBD:      ICU.IER[9].BIT.IEN5 = 0;
    while (1) {
        UEZTaskDelay(50); // wait 50 ms before checking

        /* Check any interrupt is pending */
        InterruptStatus = rsi_send_ssp_read_intrStatus_cmd();
        if (InterruptStatus == 0xffff) {
            while (1)
                ;
        }
        InterruptStatus = (InterruptStatus & ~(0xD3));
        /* Interrupt is pending */
        InterruptType = rs22_get_interrupt_type(InterruptStatus );
        if (InterruptType == DATA_PENDING) {
            InterruptStatus = InterruptStatus & (~(SD_MGMT_PENDING));
            Status = rsi_firmware_upgrade_receive(&RcvPacket, &size);

            if (Status == RS22_FW_UPGRADE_SUCCESS) {
                break;
            }
        }
    }
    //TBD:      ICU.IER[9].BIT.IEN5 = 1;
    return Status;
}
#endif  //EVAL_LICENCE
/*FUNCTION*********************************************************************
 Function Name  : rsi_send_scan_request
 Description    : This function  sends WLAN scan request
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 channel          |  X  |     |     | Channel number on which to scan
 SSID             |  X  |     |     | SSID of the Access Point
 *END****************************************************************************/

uint16_t rsi_send_scan_request(uint32_t channel, uint8_t *SSID)
{
    struct RSI_Mac_frame_t mgmt;
    RS22_STATUS status = RS22_STATUS_SUCCESS;

    mgmt.frameType.Scan.uChannelNo = channel;
    rs22_memset(mgmt.frameType.Scan.uSSID, 0, 32);

    rs22_strcpy((char *)mgmt.frameType.Scan.uSSID, (const char *)SSID);

    rs22_memset(mgmt.descriptor, 0, 8);
#if NEW_FIRMWARE
    mgmt.descriptor[0]
            = CPU_TO_LE16(MGMT_DESC_RS22_SCAN | sizeof(struct RSI_Mac_frame_t));
    mgmt.descriptor[3] = 0x5555;
    status = rs22_send_TA_mgmt_frame((uint8_t *)&mgmt, RSI_DESC_LEN
            + sizeof(struct RSI_Mac_frame_t));
#else
    mgmt.descriptor[0] = CPU_TO_LE16(MGMT_DESC_RS22_SCAN | sizeof(struct RSI_Mac_frame_t));
    status = rs22_send_TA_mgmt_frame((uint8_t *)&mgmt,
            RSI_DESC_LEN + sizeof(struct RSI_Mac_frame_t));
#endif
    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        while (0)
            ;
    }
    return (status);
}
/*FUNCTION*********************************************************************
 Function Name  : rsi_send_join_request
 Description    : This function  sends the WLAN Association request to the specific
 Access point
 Returned Value : SUCCESS or FAILURE
 Parameters     :
 -----------------+-----+-----+------+------------------------------
 Name             | I/P | O/P | I/O  | Purpose
 -----------------+-----+-----+------+------------------------------
 uNwType          | X   |     | 		| Netwrok type
 uSecType		 | X   |     |		| Security type
 uPSK			 | X   |	 |		| Preshared key
 ssid			 | X   |	 |		| SSID of the Access Point
 uDataRate		 | X   |	 |		| Data rate
 uPowerlevel      | X   |	 |		| Power level

 *END****************************************************************************/
struct RSI_Mac_frame_t mgmt;
uint16_t rsi_send_join_request(struct WiFi_CNFG_Join_t *join_params)
{

    uint16_t status = 0;

#if NEW_FIRMWARE
    mgmt.frameType.Join.uNwType = join_params->uNwType;
    mgmt.frameType.Join.uSecType = join_params->uSecType;
    mgmt.frameType.Join.uDataRate = join_params->uDataRate;

#else
    mgmt.frameType.Join.uNwType = join_params->uNwType;
    mgmt.frameType.Join.uSecType = join_params->uSecType;
#endif
    rs22_memset(&mgmt.frameType.Join.uPSK[0], 0, 32);
    if (join_params->uPSK) {
        rs22_memcpy(&mgmt.frameType.Join.uPSK[0], join_params->uPSK, 32);
    }

    if (join_params->uSSID) {
        rs22_memset(&mgmt.frameType.Join.uSSID[0], 0, 32);
        rs22_memcpy(&mgmt.frameType.Join.uSSID[0], join_params->uSSID, 32);
    }
    mgmt.frameType.Join.uPowerlevel = join_params->uPowerlevel;
    mgmt.frameType.Join.uIbss_mode = join_params->uIbss_mode;
    mgmt.frameType.Join.uChannel = join_params->uChannel;
    rs22_memset(mgmt.descriptor, 0, 8);
#if NEW_FIRMWARE
    mgmt.descriptor[0]
            = CPU_TO_LE16(MGMT_DESC_RS22_JOIN | sizeof(struct RSI_Mac_frame_t));
    mgmt.descriptor[3] = 0x5555;
    rs22_send_TA_mgmt_frame((uint8_t *)&mgmt, sizeof(struct RSI_Mac_frame_t)
            + RSI_DESC_LEN);

#else
    mgmt.descriptor[0] = CPU_TO_LE16(MGMT_DESC_RS22_JOIN | (sizeof(struct RSI_Mac_frame_t)));
    rs22_send_TA_mgmt_frame((uint8_t *)&mgmt,
            RSI_DESC_LEN + sizeof(struct RSI_Mac_frame_t));
#endif

    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_send_scan_request
 Description    : This function  sends WLAN scan request
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 channel          |  X  |     |     | Channel number on which to scan
 SSID             |  X  |     |     | SSID of the Access Point
 *END****************************************************************************/

uint16_t rsi_send_power_save_request(uint8_t power_mode)
{
    struct RSI_Mac_frame_t mgmt;
    RS22_STATUS status = RS22_STATUS_SUCCESS;

    mgmt.frameType.Power_mode.uPower_mode = power_mode;

    rs22_memset(mgmt.descriptor, 0, 8);
#if NEW_FIRMWARE
    mgmt.descriptor[0]
            = CPU_TO_LE16(MGMT_DESC_RS22_PWSAVE | sizeof(struct RSI_Mac_frame_t));
    mgmt.descriptor[3] = 0x5555;
    status = rs22_send_TA_mgmt_frame((uint8_t *)&mgmt, RSI_DESC_LEN
            + sizeof(struct RSI_Mac_frame_t));
#else
    mgmt.descriptor[0] = CPU_TO_LE16(MGMT_DESC_RS22_PWSAVE | sizeof(struct RSI_Mac_frame_t));
    status = rs22_send_TA_mgmt_frame((uint8_t *)&mgmt,
            RSI_DESC_LEN + sizeof(struct RSI_Mac_frame_t));
#endif
    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        while (0)
            ;
    }
    return (status);
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_send_TCP_config_request
 Description    : This function sends the ipconfig request to the module
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+------+-----+-----+------------------------------
 Name             | I/P  | O/P | I/O | Purpose
 -----------------+------+-----+-----+------------------------------
 uUseDHCP 		 |	X	|	  |		| DHCP enable or disable
 uIPaddr			 |	X	|	  |		| Ipaddress to be configured
 uNetMask		 |	X	|	  |		| subnet mask to be configured
 uDefaultGateway  |	X	|	  |		| Gateway to be configured
 *END****************************************************************************/
int32_t rsi_send_TCP_config_request(
        uint8_t uUseDHCP,
        uint8_t uIPaddr[],
        uint8_t uNetMask[],
        uint8_t uDefaultGateway[])
{
    volatile int32_t iStatus = RS22_STATUS_SUCCESS;
    struct TCP_CNFG_Configure cmd;
    rs22_memset(cmd.descriptor, 0, 9);

    cmd.descriptor[0]
            = CPU_TO_LE16(sizeof(struct TCP_CNFG_Configure) - RSI_DESC_LEN);
#if NEW_FIRMWARE
    cmd.descriptor[3] = 0x5555;
#endif
    cmd.descriptor[8] = CPU_TO_LE16(E_TCP_CMD_Configure);
    cmd.uUseDHCP = uUseDHCP;
    rs22_memcpy(cmd.uIPaddr, uIPaddr, 4);
    rs22_memcpy(cmd.uNetMask, uNetMask, 4);
    rs22_memcpy(cmd.uDefaultGateway, uDefaultGateway, 4);

#if NEW_FIRMWARE
    iStatus = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct TCP_CNFG_Configure) /*+ 53*/);
#else
    iStatus = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct TCP_CNFG_Configure));
#endif

    if (iStatus == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        while (0)
            ;
    }
    return (iStatus);
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_socket_create
 Description    : This function  sends the socket create request
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 uSocketType		 |  X  |     |     | Type of the socket to be created(TCP, UDP, LTCP, LUDP )
 stLocalPort		 |  X  |     |     | Local port number
 stRemoteAddress  |  X  |     |     | Remote ip address and port number
 *END****************************************************************************/
RS22_STATUS rsi_socket_create(
        SOCKET_TYPE uSocketType,
        uint16_t stLocalPort,
        stRemoteAdress_t *stRemoteAddress)
{
    struct TCP_CNFG_Socket cmd;
    RS22_STATUS status = RS22_STATUS_SUCCESS;

    rs22_memset(cmd.descriptor, 0, 9);

#if NEW_FIRMWARE
    cmd.descriptor[0]
            = CPU_TO_LE16(sizeof(struct TCP_CNFG_Socket) /*+ 56*/- RSI_DESC_LEN);
    cmd.descriptor[3] = 0x5555;
#else
    cmd.descriptor[0] = CPU_TO_LE16(sizeof(struct TCP_CNFG_Socket) - RSI_DESC_LEN);
#endif

    cmd.descriptor[8] = CPU_TO_LE16(E_TCP_CMD_Socket);
    cmd.uSocketType = CPU_TO_LE16(uSocketType);
    cmd.stLocalPort = CPU_TO_LE16(stLocalPort);
    cmd.stRemoteAddress.sin_port = CPU_TO_LE16(stRemoteAddress->sinPort);
    rs22_memcpy(&cmd.stRemoteAddress.sin_addr.s_addr[0], stRemoteAddress->ip, 4);

#if NEW_FIRMWARE
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct TCP_CNFG_Socket) /*+ 56*/);
#else
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct TCP_CNFG_Socket));
#endif
    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        status = RS22_STATUS_FAILURE;
    }
    return (status);
}

/*FUNCTION*********************************************************************
 Function Name  : get_proto_type
 Description    : This function finds the Socket type (TCP, UDP, LTCP, LUDP )
 Returned Value : Socket type
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 sd               |  X  |     |     | Socket descriptor
 *END****************************************************************************/

int32_t rsi_get_proto_type(uint16_t sd)
{

    return (gsock_table[get_table_entry(sd)].prototype);

}
/*FUNCTION*********************************************************************
 Function Name  : rsi_socket_send
 Description    : This function sends the data to the remote server or client
 Returned Value : Socket type
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 uSocketDescriptor|  X  |     |     | Socket descriptor
 uBufferLength    |  X  |     |     | length of the data
 pBuffer          |  X  |     |     | pointer to the data to be sent
 *END****************************************************************************/
struct TCP_CNFG_Send cmd;
uint16_t rsi_socket_send(
        uint16_t uSocketDescriptor,
        uint32_t uBufferLength,
        uint8_t *pBuffer)
{
    RS22_STATUS status = RS22_STATUS_SUCCESS;
    uint16_t uDataOffset;
    uint16_t prototype;

    rs22_memset(cmd.descriptor, 0, 9);
    *((uint16_t *)&cmd.descriptor[8]) = E_TCP_CMD_Send;
    cmd.uSocketDescriptor = uSocketDescriptor;
    cmd.uBufferLength = uBufferLength;

    /* Get the type of the socket to send on */
    prototype = rsi_get_proto_type(uSocketDescriptor);
    switch (prototype) {
        case RSI_PROTO_UDP:
            uDataOffset = RSI_PROTO_UDP_OFFSET;
            break;
        case RSI_PROTO_TCP:
        case RSI_PROTO_LTCP:
            uDataOffset = RSI_PROTO_TCP_OFFSET;
            break;
        default:
            uDataOffset = 0;

    }

#if 1
    *((uint16_t *)&cmd.descriptor[0]) = (uBufferLength + uDataOffset);
#else
    val = ((uint16_t)uBufferLength + uDataOffset);
    cmd.descriptor[0] = (((val & 0x00FF) << 8) + ((val & 0xFF00) >> 8));
#endif
    cmd.uDataOffset = uDataOffset;
    rs22_memcpy((void *)&cmd.uBuffer[uDataOffset - 10], (void *)pBuffer, uBufferLength);

    status = rs22_send_data_frame((uint8_t *)&cmd, uBufferLength + uDataOffset
            + RSI_DESC_LEN);

    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        while (0)
            ;
    }
    return (status);
}

/*FUNCTION*********************************************************************
 Function Name  :
 Description    :
 Returned Value :
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 uSocketType		 |  X  |     |     | Type of the socket to be created(TCP, UDP, LTCP, LUDP )
 stLocalPort		 |  X  |     |     | Local port number
 stRemoteAddress  |  X  |     |     | Remote ip address and port number
 *END****************************************************************************/
uint16_t rsi_Send_Rssi_Req(void)
{

    struct RSSI_QUERY_Req cmd;
    RS22_STATUS status = RS22_STATUS_SUCCESS;

    rs22_memset(cmd.descriptor, 0, 9);

#if NEW_FIRMWARE
    cmd.descriptor[0]
            = CPU_TO_LE16(sizeof(struct RSSI_QUERY_Req) /*+ 56*/- RSI_DESC_LEN);
    cmd.descriptor[3] = 0x5555;
#else
    cmd.descriptor[0] = CPU_TO_LE16(sizeof(struct RSSI_QUERY_Req) - RSI_DESC_LEN);
#endif

    cmd.descriptor[8] = CPU_TO_LE16(E_TCP_RSSI_Query);

#if NEW_FIRMWARE
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct RSSI_QUERY_Req));
#else
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct RSSI_QUERY_Req));
#endif
    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        status = RS22_STATUS_FAILURE;
    }
    return (status);
}

/*FUNCTION*********************************************************************
 Function Name  :
 Description    :
 Returned Value :
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 uSocketType		 |  X  |     |     | Type of the socket to be created(TCP, UDP, LTCP, LUDP )
 stLocalPort		 |  X  |     |     | Local port number
 stRemoteAddress  |  X  |     |     | Remote ip address and port number
 *END****************************************************************************/
uint16_t rsi_Send_Wlan_Conn_Status_Req(void)
{

    struct WLAN_CONN_Query_Req cmd;
    RS22_STATUS status = RS22_STATUS_SUCCESS;

    rs22_memset(cmd.descriptor, 0, 9);

#if NEW_FIRMWARE
    cmd.descriptor[0]
            = CPU_TO_LE16(sizeof(struct WLAN_CONN_Query_Req) /*+ 56*/- RSI_DESC_LEN);
    cmd.descriptor[3] = 0x5555;
#else
    cmd.descriptor[0] = CPU_TO_LE16(sizeof(struct WLAN_CONN_Query_Req) - RSI_DESC_LEN);
#endif

    cmd.descriptor[8] = CPU_TO_LE16(E_TCP_CONSTATUS_Query);

#if NEW_FIRMWARE
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct WLAN_CONN_Query_Req));
#else
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct WLAN_CONN_Query_Req));
#endif
    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        status = RS22_STATUS_FAILURE;
    }
    return (status);
}

/*FUNCTION*********************************************************************
 Function Name  :
 Description    :
 Returned Value :
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 uSocketType		 |  X  |     |     | Type of the socket to be created(TCP, UDP, LTCP, LUDP )
 stLocalPort		 |  X  |     |     | Local port number
 stRemoteAddress  |  X  |     |     | Remote ip address and port number
 *END****************************************************************************/
uint16_t rsi_send_fwversion_Req(void)
{

    struct WLAN_CONN_Query_Req cmd;
    RS22_STATUS status = RS22_STATUS_SUCCESS;

    rs22_memset(cmd.descriptor, 0, 9);

#if NEW_FIRMWARE
    cmd.descriptor[0]
            = CPU_TO_LE16(sizeof(struct FWVERSION_Query_Req) /*+ 56*/- RSI_DESC_LEN);
    cmd.descriptor[3] = 0x5555;
#else
    cmd.descriptor[0] = CPU_TO_LE16(sizeof(struct FWVERSION_Query_Req) - RSI_DESC_LEN);
#endif

    cmd.descriptor[8] = CPU_TO_LE16(E_TCP_FW_VERSION_Get);

#if NEW_FIRMWARE
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct FWVERSION_Query_Req));
#else
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct FWVERSION_Query_Req));
#endif
    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        status = RS22_STATUS_FAILURE;
    }
    return (status);
}

/*FUNCTION*********************************************************************
 Function Name  :
 Description    :
 Returned Value :
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 uSocketType		 |  X  |     |     | Type of the socket to be created(TCP, UDP, LTCP, LUDP )
 stLocalPort		 |  X  |     |     | Local port number
 stRemoteAddress  |  X  |     |     | Remote ip address and port number
 *END****************************************************************************/
uint16_t rsi_disconnect_wlan_conn(void)
{
    struct WLAN_CONN_Query_Req cmd;
    RS22_STATUS status = RS22_STATUS_SUCCESS;

    rs22_memset(cmd.descriptor, 0, 9);

#if NEW_FIRMWARE
    cmd.descriptor[0]
            = CPU_TO_LE16(sizeof(struct WLAN_Disconn_Req) /*+ 56*/- RSI_DESC_LEN);
    cmd.descriptor[3] = 0x5555;
#else
    cmd.descriptor[0] = CPU_TO_LE16(sizeof(struct WLAN_Disconn_Req) - RSI_DESC_LEN);
#endif
    cmd.descriptor[8] = CPU_TO_LE16(E_WLAN_DISCONNECT_Req);

#if NEW_FIRMWARE
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct WLAN_Disconn_Req));
#else
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct WLAN_Disconn_Req));
#endif
    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        status = RS22_STATUS_FAILURE;
    }
    return (status);
}

#ifdef POWER_SAVE
/*FUNCTION*********************************************************************
 Function Name  : rs22_send_pwrsv_frame
 Description    :
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------

 *END****************************************************************************/
int32_t rs22_send_pwrsv_frame (uint16_t byte)
{

    RS22_STATUS status = RS22_STATUS_SUCCESS;
    struct RSI_Mac_frame_t mgmt;

    rs22_memset(mgmt.descriptor, 0, 8);
#if NEW_FIRMWARE
    mgmt.descriptor[0] = CPU_TO_LE16(MGMT_DESC_RS22_PWR | 64);
    mgmt.descriptor[1] = byte;
    mgmt.descriptor[3] = 0x5555;
    status = rs22_send_TA_mgmt_frame((uint8_t *)&mgmt, RSI_DESC_LEN + 64);
#else
    mgmt.descriptor[0] = CPU_TO_LE16(MGMT_DESC_RS22_PWR | sizeof(struct RSI_Mac_frame_t));
    status = rs22_send_TA_mgmt_frame((uint8_t *)&mgmt, RSI_DESC_LEN + sizeof(struct RSI_Mac_frame_t));
#endif

    if(status == RS22_STATUS_SUCCESS)
    {
        //asm("nop");
        while(0);

    }
    else
    {
        //asm("nop");
        while(0);
    }
    return(status);

}
/*FUNCTION*********************************************************************
 Function Name  : RSI_enable_pwrsv_mode
 Description    :
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------

 *END****************************************************************************/
int32_t RSI_enable_pwrsv_mode( uint16_t byte )
{
    int16_t status = RSI_STATUS_SUCCESS;
    uint32_t size;

    status = rs22_send_pwrsv_frame(byte);
    if(status != RS22_STATUS_SUCCESS)
    {
        return status;
    }
    while(!(Adaptor.pkt_received));
    if(Adaptor.pkt_received > 0)
    {
        /* Poll for the interrupt */
        status = rs22_interrupt_handler(Adaptor.RcvPkt, &size);

    }

    return status;

}
#endif

/*FUNCTION*********************************************************************
 Function Name  : rs22_socket_close
 Description    : This function closes the socket specified by the socket descriptor
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 uSocketDescriptor | X  |     |     | socket descriptor
 *END****************************************************************************/
uint16_t rs22_socket_close(uint16_t uSocketDescriptor)
{
    struct TCP_CNFG_Close cmd;
    RS22_STATUS status = RS22_STATUS_SUCCESS;

    rs22_memset(cmd.descriptor, 0, 9);
    cmd.descriptor[0]
            = CPU_TO_LE16(sizeof(struct TCP_CNFG_Close) - RSI_DESC_LEN);
    cmd.descriptor[3] = 0x5555;
    cmd.descriptor[8] = CPU_TO_LE16(E_TCP_CMD_Close);
    cmd.uSocketDescriptor = CPU_TO_LE16(uSocketDescriptor);
    /* Send te socket close */
    status = rs22_send_data_frame((uint8_t *)&cmd,
            sizeof(struct TCP_CNFG_Close));

    if (status == RS22_STATUS_SUCCESS) {
        while (0)
            ;
    } else {
        while (0)
            ;
    }
    return (status);
}

/**************************************************************************************/

/*FUNCTION*********************************************************************
 Function Name  : rsi_send_ssp_init_cmd
 Description    : This function sends the SPI initialization command
 Returned Value : Returns 0 on success and -1 on failure
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------

 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

int8_t rsi_send_ssp_init_cmd(void) // checked
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Prepare the INITIALIZATION cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = SPI_INITIALIZATION;

    /* Send the cmd to the BUS */
    while (ii < 1) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return (status);
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_send_ssp_read_intrStatus_cmd
 Description    : This function sends the SPI INTERNAL READ interrupt status
 register command
 Returned Value : Returns the register value
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------

 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/
int32_t inttStatus = 0;
uint32_t rsi_send_ssp_read_intrStatus_cmd(void)
{
    struct bus_cmd_s cmd;
    int32_t intrStatus = 0;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Read the HOST INTERRUPT (SPI slave) register */
    /* Prepare the SPI_INTERNAL_READ cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = SPI_INTERNAL_READ;
    cmd.data = (void *)&intrStatus;
    cmd.length = 2;
    cmd.address = 0x0000; /* HOST INTERRUPT REG */
    //Since the address is 0, no need to call CPU_TO_LE32 also.

    /* Send the cmd to the BUS */
    while (ii < 1500) {
        status = rs22_ssp_bus_request_synch(&cmd);
        //inttStatus = intrStatus;
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
        intrStatus = 0xffff;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return (LE32_TO_CPU(intrStatus));
}

/*FUNCTION*********************************************************************
 Function Name  : send_ssp_intrAck_cmd
 Description    : This function sends the AHB_MASTER_WRITE interrupt ack
 command
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 val              |  X  |     |     |
 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

uint32_t rsi_send_ssp_intrAck_cmd(uint32_t val)
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Prepare the AHB_MASTER_WRITE cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = AHB_MASTER_WRITE;
    cmd.data = &val;
    cmd.length = 4;
    cmd.address = CPU_TO_LE32(0x22000010); /* HOST INTERRUPT REG */

    /* Send the cmd to the BUS */
    while (ii < 150) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }

    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return 0;
}
/*FUNCTION*********************************************************************
 Function Name  : send_ssp_intrAck_cmd
 Description    : This function sends the AHB_MASTER_WRITE interrupt ack
 command
 Returned Value :
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 Adapter          |  X  |     |     | pointer to our adapter
 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

uint32_t rsi_ssp_write_mask_register(uint32_t val)
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Prepare the SPI_INTERNAL_WRITE cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = AHB_MASTER_WRITE;
    cmd.data = &val;
    cmd.length = 4;
    cmd.address = 0x22000008; /* HOST INTERRUPT REG */
    /* Send the cmd to the BUS */
    while (ii < 4) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return 0;
}
/*FUNCTION*********************************************************************
 Function Name  : send_ta_load_cmd
 Description    : This function sends the AHB_MASTER_WRITE interrupt ack
 command
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 data             |     |  X  |     | Pointer to the data buffer
 size             |  X  |     |     | size
 address          |  X  |     |     | Adress
 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

int32_t rs22_send_ta_load_cmd(uint8_t *data, uint32_t size, uint32_t address) // checked
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Prepare the AHB_MASTER_WRITE cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = AHB_MASTER_WRITE;
    cmd.data = data;
    cmd.length = size;
    cmd.address = CPU_TO_LE32(address);

    /* Send the cmd to the BUS */
    while (ii < 150) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_send_ta_read_cmd
 Description    : This function sends the AHB_MASTER_READ interrupt ack
 command
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 data			 |     |  X  | Pointer to the data buffer
 size			 | X   |     | size
 address			 | X   |     | Address
 *END****************************************************************************/
int32_t rs22_send_ta_read_cmd(uint8_t *data, uint32_t size, uint32_t address)
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Prepare the AHB_MASTER_WRITE cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = AHB_MASTER_READ;
    cmd.data = data;
    cmd.length = size;
    cmd.address = CPU_TO_LE32(address);

    /* Send the cmd to the BUS */
    while (ii < 150) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : send_ta_sft_rst_cmd
 Description    : This function sends the AHB_MASTER_WRITE interrupt ack
 command
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 val              |  X  |     |     | val
 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

int32_t rs22_send_ta_sft_rst_cmd(uint32_t val) // checked
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Prepare the AHB_MASTER_WRITE cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = AHB_MASTER_WRITE;
    cmd.data = &val;
    cmd.length = 4;
    cmd.address = CPU_TO_LE32(0x22000004);

    /* Send the cmd to the BUS */
    while (ii < 150) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_send_ssp_wakeup_cmd
 Description    : This function sends the SPI INTERNAL WRITE interrupt status
 register command
 Returned Value : Returns the register value
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 byte             |  X  |     |     |
 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

uint32_t rs22_send_ssp_wakeup_cmd( IN uint8_t byte)
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Read the HOST INTERRUPT (SPI slave) register */
    /* Prepare the SPI_INTERNAL_WRITE cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = SPI_INTERNAL_WRITE;
    cmd.data = (void *)&byte;
    cmd.length = 2;
    cmd.address = CPU_TO_LE32(0x0000007E);

    /* Send the cmd to the BUS */
    while (ii < 50) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_send_ssp_wakeup_cmd_read
 Description    : This function sends the SPI INTERNAL READ interrupt status
 register command
 Returned Value : Returns the register value
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------

 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

uint32_t rs22_send_ssp_wakeup_cmd_read()
{
    struct bus_cmd_s cmd;
    uint32_t intrStatus = 0;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Read the HOST INTERRUPT (SPI slave) register */
    /* Prepare the SPI_INTERNAL_READ cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = SPI_INTERNAL_READ;
    cmd.data = (void *)&intrStatus;
    cmd.length = 2;
    cmd.address = CPU_TO_LE32(0x0000007E);
    ;

    /* Send the cmd to the BUS */
    while (ii < 1000) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : send_ta_config_params_cmd
 Description    : This function sends the AHB_MASTER_WRITE interrupt ack
 command
 Returned Value : SUCCESS or FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 data             |     |  X  |     |
 size             |  X  |     |     |
 address          |  X  |     |     |
 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

uint32_t rs22_send_ta_config_params_cmd(
        uint8_t *data,
        uint32_t size,
        uint32_t address)
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Prepare the AHB_MASTER_WRITE cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = AHB_MASTER_WRITE;
    cmd.data = data;
    cmd.length = size;
    cmd.address = CPU_TO_LE32(address);

    /* Send the cmd to the BUS */
    while (ii < 150) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_send_dummy_write
 Description    : This function does dummy writes
 Returned Value : Returns the register value
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 byte             |  X  |     |     |
 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

uint32_t rs22_send_dummy_write( IN uint8_t byte)
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = SPI_DUMMY_WRITE;
    cmd.data = (void *)&byte;
    cmd.length = 2;
    cmd.address = CPU_TO_LE32(0x003F);

    /* Send the cmd to the BUS */
    while (ii < 50) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_wakeup_reg_write
 Description    : This function writes a register for wakeup
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------

 -----------------+-----+-----+-----+------------------------------
 *END**************************************************************************/

uint32_t rs22_wakeup_reg_write(void)
{
    int32_t status = RSI_STATUS_SUCCESS;
    uint8_t byte = 0x01;

    status = rs22_send_ssp_wakeup_cmd(byte);
    if (status != RSI_STATUS_SUCCESS) {
        return status;
    }

    status = rs22_send_ssp_wakeup_cmd_read();
    if (status != RSI_STATUS_SUCCESS) {
        return status;
    }

    return RSI_STATUS_SUCCESS;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_send_ssp_mode_set
 Description    :
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/
uint32_t rs22_send_ssp_mode_set( IN uint8_t byte //1 - For High Speed Mode
)
{
    struct bus_cmd_s cmd;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Read the HOST INTERRUPT (SPI slave) register */
    /* Prepare the SPI_INTERNAL_WRITE cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = SPI_INTERNAL_WRITE;
    cmd.data = (void *)&byte;
    cmd.length = 2;
    //When we are writing in SPI Internal Write, Direct address should be given
    //Here SPI mode register direct address is 0x08000008, where 0x08000000 is
    //the base address and 0x00000008 is the offset
    //Here only give the offset.
    cmd.address = CPU_TO_LE32(0x00000008);

    /* Send the cmd to the BUS */
    while (ii < 50) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_send_ssp_mode_read
 Description    : This function reads SPI mode ( high speed or low speed )
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/
uint32_t rs22_send_ssp_mode_read()
{
    struct bus_cmd_s cmd;
    uint32_t intrStatus = 0;
    int32_t status = RSI_STATUS_SUCCESS;
    uint32_t ii = 0;

    /* Read the HOST INTERRUPT (SPI slave) register */
    /* Prepare the SPI_INTERNAL_READ cmd */
    rs22_memset(&cmd, 0, sizeof(struct bus_cmd_s));
    cmd.type = SPI_INTERNAL_READ;
    cmd.data = (void *)&intrStatus;
    cmd.length = 2;
    cmd.address = CPU_TO_LE32(0x00000008);

    /* Send the cmd to the BUS */
    while (ii < 1000) {
        status = rs22_ssp_bus_request_synch(&cmd);
        if ((status == RSI_STATUS_BUSY) || (status == RSI_STATUS_FAIL)) {
            ii++;
            continue;
        }
        break;
    }
    if (status != RSI_STATUS_SUCCESS) {
        //asm("nop");
        while (0)
            ;
    }
    return status;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_set_high_speed_mode
 Description    : This function sets the spi in high speed mode
 Returned Value : SUCCESS or FAILURE
 Parameters     : NONE

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 *END****************************************************************************/
uint32_t rs22_set_high_speed_mode()
{
    int32_t status = RSI_STATUS_SUCCESS;
    uint8_t byte = 0x01;

    status = rs22_send_ssp_mode_set(byte);
    if (status != RSI_STATUS_SUCCESS) {
        return status;
    }

    status = rs22_send_ssp_mode_read();
    if (status != RSI_STATUS_SUCCESS) {
        return status;
    }

    return RSI_STATUS_SUCCESS;
}

/* $EOF */
/* Log */

