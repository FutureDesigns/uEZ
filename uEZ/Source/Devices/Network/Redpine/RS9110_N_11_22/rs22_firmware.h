/*HEADER**********************************************************************
 Copyright (c)
 All rights reserved
 This software embodies materials and concepts that are confidential to Redpine
 Signals and is made available solely pursuant to the terms of a written license
 agreement with Redpine Signals

 Project name : RS22
 Module name  : SPI driver
 File Name    : rs22_firmware.h

 File Description:


 Author :

 Rev History:
 Ver  By         date          Description
 ---------------------------------------------------------
 1.1  Redpine Signals
 --------------------------------------------------------
 *END**************************************************************************/

#ifndef __RS22_FIRMWARE_H__
#define __RS22_FIRMWARE_H__

const uint8_t sbtaim1[] = {
#include "Firmware/sbinst1.map"
        };
const uint8_t sbtaim2[] = {
#include "Firmware/sbinst2.map"
        };
const uint8_t sbtadm[] = {
#include "Firmware/sbdata1.map"
        };

#ifndef EVAL_LICENCE

const uint8_t iutaim1[] = {
#include "Firmware/iuinst1.map"
        };

const uint8_t iutaim2[] = {
#include "Firmware/iuinst2.map"
        };

const uint8_t iutadm[] = {
#include "Firmware/iudata.map"
        };

const uint8_t fftaim1[] = {
#include "Firmware/ffinst1.map"
        };
const uint8_t fftaim2[] = {
#include "Firmware/ffinst2.map"
        };
const uint8_t fftadm[] = {
#include "Firmware/ffdata.map"
        };
#endif   // EVAL_LICENCE
const uint8_t sbtadm2[] = {
#include "Firmware/sbdata2.map"
        };
#endif
