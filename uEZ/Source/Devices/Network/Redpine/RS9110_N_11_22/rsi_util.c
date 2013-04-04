/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name :
 Module name  :
 File Name    : rsi_util.c

 File Description:
 This file contains the Through put, RSSI, power consumption, SSID LIST specific code .

 List of functions:


 Author :

 Rev History:
 Ver   By               date        Description
 ---------------------------------------------------------
 1.1  Redpine Signals
 ---------------------------------------------------------
 */

#include "rsi_util.h"
#include "string.h"
#include "network_config.h"
#include "rsi_nic.h"

struct WiFi_CNFG_ScanRsp *ScanRsp;
extern struct WiFi_CNFG_Scan_t *scan_info;
extern struct WiFi_CNFG_Join_t *join_info;

/*FUNCTION*********************************************************************
 Function Name  : rs22_strlen
 Description    : This function finds the length of the string
 Returned Value : length of the string
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 str              | X   |     |     | String
 *END****************************************************************************/

uint16_t rs22_strlen(uint8_t *str)
{
    uint16_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_prepare_pkt_to_send
 Description    : This function encodes the packet with RSSI, THROUGHPUT, POWER_CONSUMPTION
 and SSID LIST
 Returned Value : Length of the packet encoded
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 type             | X   |     |     | type(R SSI, THROHPUT, POWER_CONSUPTION or SSID_LIST )
 val              | X   |     |     | Value to be encoded
 buff             |     |  X  |     | Buffer in to which the data is encoded
 *END****************************************************************************/

uint16_t rsi_prepare_pkt_to_send(uint16_t type, uint16_t val, uint8_t *buff)
{
    uint16_t index = 0;
    uint16_t count = 0;
    uint16_t len = 0;
    uint16_t rssi_val = 0;
    struct WiFi_CNFG_ScanInfo stScanInfo;
    buff[index++] = (uint8_t)type;

    switch (type) {
        case WIFI_CONFIG_ACK:
        case THROUGHPUT: /* Throughput */
        case POWER_CONSUMPTION: /* Power consumption */
        case RSSI: /* Recieved Signal Strength */
        {
            rsi_memcpy((buff + index), &val, 2);
            index += 2;
        }
            break;

        case SSID_LIST: /* Prepare the SSID list */
        { /* Get the APs List */
            ScanRsp = &Adaptor.ScanRsp;

            // move connected AP to first location
            for (count = 0; count < ScanRsp->uScanCount; count++) {
                if (!(strcmp((const char*)ScanRsp->stScanInfo[count].uSSID,
                        (const char*)join_info->uSSID))) {
                    if (count != 0) {
                        rsi_memcpy(&stScanInfo,&ScanRsp->stScanInfo[count], sizeof(struct WiFi_CNFG_ScanInfo));
                        rsi_memcpy(&ScanRsp->stScanInfo[count],&ScanRsp->stScanInfo[0], sizeof(struct WiFi_CNFG_ScanInfo));
                        rsi_memcpy(&ScanRsp->stScanInfo[0],&stScanInfo, sizeof(struct WiFi_CNFG_ScanInfo));
                    }
                }

            }
            count = 0;
            while (count < ScanRsp->uScanCount) {

                len = rs22_strlen(ScanRsp->stScanInfo[count].uSSID);
                if (len > 0) {
                    rsi_memcpy((buff + index), ScanRsp->stScanInfo[count].uSSID, len);
                    index += len;
                    buff[index++] = SEPARATOR;
                    buff[index++] = ScanRsp->stScanInfo[count].uSecMode + 0x30;
                    buff[index++] = SEPARATOR;
                    rssi_val = ScanRsp->stScanInfo[count].uRssiVal;
                    rsi_memcpy((buff + index), &rssi_val, 2);
                    index += 2;
                    buff[index++] = SEPARATOR;
                }
                count++;
            }
            index--;
        }
            break;

        default:
            break;
    }

    return index;
}

/*FUNCTION*********************************************************************
 Function Name  : rsi_memcmp
 Description    : This function compares the source and destination strings
 Returned Value : SUCCESS if both the strings are equal else FAILURE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 src_buf          |     |     |     | Source buffer
 dst_buf          |     |     |     | destination buffer
 len              |     |     |     | length
 *END****************************************************************************/

int16_t rsi_memcmp(uint8_t *src_buf, uint8_t *dst_buf, uint16_t len)
{
    uint16_t count = 0;
    while (count++ < len) {
        if (*src_buf != *dst_buf) {
            return -1;
        }
        src_buf++;
        dst_buf++;
    }
    return 0;

}

/*FUNCTION*********************************************************************
 Function Name  : str_to_ip
 Description    : This function converts the ip address from string form to integer form
 Returned Value : NONE
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 dest             |     |     |     | destination buffer
 src              |     |     |     | source buffer
 src_len          |     |     |     | length of the ip address string
 *END****************************************************************************/

void str_to_ip(uint8_t *dest, uint8_t *src, uint8_t src_len)
{
    uint16_t dest_index = 0;
    uint16_t src_index = 0;
    uint8_t temp = 0;
    for (; dest_index < 4; dest_index++) {
        temp = 0;
        for (; ((src[src_index] != '.') && (src_index < src_len)); src_index++) {
            temp = temp * 10;
            temp = temp + (src[src_index] - 0x30);
        }
        dest[dest_index] = temp;
        src_index++;
    }
    return;
}

/*FUNCTION*********************************************************************
 Function Name  : rs22_aToi
 Description    : This function converts the string to integer
 Returned Value : Converted integer value
 Parameters     :

 -----------------+-----+-----+-----+------------------------------
 Name             | I/P | O/P | I/O | Purpose
 -----------------+-----+-----+-----+------------------------------
 src              |  X  |     |     | string to be converted to the integer
 src_len          |  X  |     |     | len of the string

 *END****************************************************************************/

uint16_t rs22_aToi(uint8_t *src, uint8_t src_len)
{
    uint16_t src_index = 0;
    uint16_t temp = 0;

    for (src_index = 0; src_index < src_len; src_index++) {
        temp = temp * 10;
        temp = temp + (src[src_index] - 0x30);
    }

    return temp;
}

uint16_t rsi_convert_ip_to_string(uint8_t *num_buff, uint8_t *ip_buff)
{
    uint8_t temp_buf[8];

    uint8_t temp_buf2[8];

    uint16_t temp;

    uint16_t num_index = 0, ip_index = 0;

    uint16_t temp_index = 0, temp_index2 = 0;

    memset(ip_buff, 0, 17);

    while (num_index < 4)

    {

        temp = num_buff[num_index++];

        do {
            temp_buf[temp_index++] = (temp % 10) + '0';

            temp = temp / 10;

            ip_index++;

        } while (temp);
        rsi_strrev(temp_buf, temp_buf2, temp_index);

        rsi_strcat(temp_buf2, ip_buff + temp_index2, temp_index);

        temp_index = 0;

        if (num_index < 4)

            ip_buff[ip_index++] = '.';

        temp_index2 = ip_index;

    }

    return ip_index;

}

uint8_t rsi_strrev(void *src, void *dst, uint8_t len)
{
    uint8_t *dst1;
    uint8_t *src1;
    dst1 = dst;
    src1 = src;

    src1 = src1 + len - 1;

    while (len-- != 0)
        *dst1++ = *src1--;
    *dst1 = '\0';
    return 0;
}

void rsi_strcat(void *src, void *dst, uint8_t len)

{

    uint8_t *dst1;

    uint8_t *src1;

    dst1 = dst;

    src1 = src;

    while (len-- != 0)

        *dst1++ = *src1++;

}
