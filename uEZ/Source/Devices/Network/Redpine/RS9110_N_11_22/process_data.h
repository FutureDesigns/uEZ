/*HEADER**********************************************************************
Copyright (c)
All rights reserved
This software embodies materials and concepts that are confidential to Redpine
Signals and is made available solely pursuant to the terms of a written license
agreement with Redpine Signals

Project name : 
Module name  :
File Name    : process_data.h

File Description:
    

Author :

Rev History:
Ver   By               date        Description
---------------------------------------------------------
1.1   Redpine Signals
---------------------------------------------------------
*/

#ifndef _PROCESS_DATA_H_
#define _PROCESS_DATA_H_

#define MAX 10
#define LED_ZERO  0
#define LED_ONE   1
#define LED_TWO   2
#define LED_THREE 3
#define LED_FOUR  4

#define LED 0
#define SWITCH 1

#define SUCCESS 1

#define GET_REQ  0
#define SET_REQ  1
#define GET_RESP 2
#define SET_RESP 3

int32_t rsi_process_data(unsigned char *rx_buf);


#endif