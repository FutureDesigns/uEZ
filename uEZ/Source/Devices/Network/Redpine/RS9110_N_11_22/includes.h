/*HEADER**********************************************************************
Copyright (c)
All rights reserved
This software embodies materials and concepts that are confidential to Redpine
Signals and is made available solely pursuant to the terms of a written license
agreement with Redpine Signals

Project name : 
Module name  : 
File Name    : includes.h

File Description:


Author :

Rev History:
Ver  By         date          Description
---------------------------------------------------------
1.1  Redpine Signals
--------------------------------------------------------
*END**************************************************************************/
#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#include      <stdarg.h>
#include	  <stdlib.h>
#include	  <string.h>
#include      <stdint.h>
#include      <stdbool.h>
//#include 	  "rs22_nic.h"
//#include	  "spi_driver.h"
//#include      "iodefine.h"



#define SOCK_HANDLE1             1
#define SOCK_HANDLE0             0

/*
typedef unsigned char            uint8_t;
typedef char                     int8_t;
typedef unsigned int             uint32_t;
typedef          int             int32_t;
typedef short int                int16_t;
typedef unsigned short int       uint16_t;
*/
//typedef unsigned char            bool;
typedef float                    float_t;

#define   RSI_STATUS_FAILURE              -1
#define   RSI_STATUS_SUCCESS               0
#define   RSI_STATUS                       int32_t
#define   RSI_STATUS_SUCCESS                0
#define   RSI_STATUS_BUSY                   1
#define   RSI_STATUS_FAIL                  -1
#define   RS22_JOIN_FAILURE                -5
#define   DISCONNECT						0


#define  LOW              0
#define  MEDIUM           1
#define  HIGH             2

#define  IBSS             0   
#define  INFRASTRUCTURE   1

#define  CREATE           1
#define  JOIN             0

#define  ENABLE           1
#define  DISABLE          0

#define  POWER_MODE0      0         /* No power save enabled */
#define  POWER_MODE1      1         /* TA, BB, RF will go to sleep */
#define  POWER_MODE2      2         /* BB and RF will go to sleep */
#define LOAD_TA_INTSTRUCTION

#define LOAD_INST_FROM_HOST

#endif