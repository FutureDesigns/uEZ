/*-------------------------------------------------------------------------*
 * File:  ST_LIS3DH_I2C.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef ST_LIS3DH_I2C_H_
#define ST_LIS3DH_I2C_H_

/*--------------------------------------------------------------------------
* uEZ(r) - Copyright (C) 2007-2015 Future Designs, Inc.
*--------------------------------------------------------------------------
* This file is part of the uEZ(r) distribution.
*
* uEZ(r) is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* uEZ(r) is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with uEZ(r); if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* A special exception to the GPL can be applied should you wish to
* distribute a combined work that includes uEZ(r), without being obliged
* to provide the source code for any proprietary components.  See the
* licensing section of http://goo.gl/UDtTCR for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://goo.gl/UDtTCR for more details.               |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Device/Accelerometer.h>
#include <Device/I2CBus.h>

#ifdef __cplusplus
extern "C" {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define LIS3DH_I2C_ADDR                 0x18    // 7 bit addr

#ifndef LIS3DH_I2C_SPEED
#define LIS3DH_I2C_SPEED                400     // kHz
#endif

#define ACCELEROMETER_G_EARTH       (9.80665F)      //Gravity on Earth m/s^2
#define ACCELEROMETER_G_MOON        (1.6F)          //Gravity on Earth's Moon m/s^2

#define LIS3DH_SENSITIVITY_2G       (0.001F)

// TODO add register definitions for read, configuration, etc
//Register Addresses
#define LIS3DH_REGISTER_STATUS_REG_AUX      0x07
#define LIS3DH_REGISTER_OUT_ADC1_L          0x08
#define LIS3DH_REGISTER_OUT_ADC1_H          0x09
#define LIS3DH_REGISTER_OUT_ADC2_L          0x0A
#define LIS3DH_REGISTER_OUT_ADC2_H          0x0B
#define LIS3DH_REGISTER_OUT_ADC3_L          0x0C
#define LIS3DH_REGISTER_OUT_ADC3_H          0x0D
#define LIS3DH_REGISTER_INT_COUNTER_REG     0x0E
#define LIS3DH_REGISTER_WHO_AM_I            0x0F
#define LIS3DH_REGISTER_CTRL_REG0           0x1E
#define LIS3DH_REGISTER_TEMP_CFG_REG        0x1F
#define LIS3DH_REGISTER_CTRL_REG1           0x20
#define LIS3DH_REGISTER_CTRL_REG2           0x21
#define LIS3DH_REGISTER_CTRL_REG3           0x22
#define LIS3DH_REGISTER_CTRL_REG4           0x23
#define LIS3DH_REGISTER_CTRL_REG5           0x24
#define LIS3DH_REGISTER_CTRL_REG6           0x25
#define LIS3DH_REGISTER_REFERENCE           0x26
#define LIS3DH_REGISTER_STATUS_REG          0x27
#define LIS3DH_REGISTER_OUT_X_L             0x28
#define LIS3DH_REGISTER_OUT_X_H             0x29
#define LIS3DH_REGISTER_OUT_Y_L             0x2A
#define LIS3DH_REGISTER_OUT_Y_H             0x2B
#define LIS3DH_REGISTER_OUT_Z_L             0x2C
#define LIS3DH_REGISTER_OUT_Z_H             0x2D
#define LIS3DH_REGISTER_FIFO_CTRL_REG       0x2E
#define LIS3DH_REGISTER_FIFO_SRC_REG        0x2F
#define LIS3DH_REGISTER_INT1_CFG            0x30
#define LIS3DH_REGISTER_INT1_SOURCE         0x31
#define LIS3DH_REGISTER_INT1_THS            0x32
#define LIS3DH_REGISTER_INT1_DURATION       0x33
#define LIS3DH_REGISTER_INT2_CFG            0x34
#define LIS3DH_REGISTER_INT2_SOURCE         0x35
#define LIS3DH_REGISTER_INT2_THS            0x36
#define LIS3DH_REGISTER_INT2_DURATION       0x37
#define LIS3DH_REGISTER_CLICK_CFG           0x38
#define LIS3DH_REGISTER_CLICK_SRC           0x39
#define LIS3DH_REGISTER_CLICK_THS           0x3A
#define LIS3DH_REGISTER_TIME_LIMIT          0x3B
#define LIS3DH_REGISTER_TIME_LATENCY        0x3C
#define LIS3DH_REGISTER_TIME_WINDOW         0x3D
#define LIS3DH_REGISTER_ACT_THS             0x3E
#define LIS3DH_REGISTER_ACT_DUR             0x3F
//Register Data
//CTRL_REG 1
#define LIS3DH_CTRL1_ODR_POWER_DOWN                     0x00
#define LIS3DH_CTRL1_ODR_1HZ                            0x10
#define LIS3DH_CTRL1_ODR_10HZ                           0x20
#define LIS3DH_CTRL1_ODR_25HZ                           0x30
#define LIS3DH_CTRL1_ODR_50HZ                           0x40
#define LIS3DH_CTRL1_ODR_100HZ                          0x50
#define LIS3DH_CTRL1_ODR_200HZ                          0x60
#define LIS3DH_CTRL1_ODR_400HZ                          0x70
#define LIS3DH_CTRL1_ODR_LPM_1_6KHZ                     0x80
#define LIS3DH_CTRL1_ODR_HR_NORM_1_344KHZ_LPM_5_375KHZ  0x90
#define LIS3DH_CTRL1_LPM_EN                             0x08
#define LIS3DH_CTRL1_Z_AXIS_EN                          0x04
#define LIS3DH_CTRL1_Y_AXIS_EN                          0x02
#define LIS3DH_CTRL1_X_AXIS_EN                          0x01
#define LIS3DH_CTRL1_XYZ_AXIS_EN                        0x07
//CRTL_REG 2
#define LIS3DH_CTRL2_HIGH_PASS_NORMAL_REF_RESET         0x00
#define LIS3DH_CTRL2_HIGH_PASS_REF_FILTER               0x40
#define LIS3DH_CTRL2_HIGH_PASS_NORMAL                   0x80
#define LIS3DH_CTRL2_HIGH_PASS_RESET_ON_INT             0xC0
#define LIS3DH_CTRL2_FDS_EN                             0x08
//CTRL_REG 4
#define LIS3DH_CTRL4_BDU_EN                             0x80    //enable Block data update, output registers not updated until MSB and LSB reading
#define LIS3DH_CTRL4_BIG_ENDIAN                         0x40    //Default is little endian
#define LIS3DH_CTRL4_SCALE_SELECT_2G                    0x00
#define LIS3DH_CTRL4_SCALE_SELECT_4G                    0x10
#define LIS3DH_CTRL4_SCALE_SELECT_8G                    0x20
#define LIS3DH_CTRL4_SCALE_SELECT_16G                   0x30
#define LIS3DH_CTRL4_HIGH_RES_EN                        0x08
//STATUS_REG
#define LIS3DH_STATUS_DATA_OVERWRITTEN                  0x80
#define LIS3DH_STATUS_DATA_AVAILABLE                    0x08

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
  
/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
extern const DEVICE_Accelerometer Accelerometer_ST_LIS3DH_via_I2C_Interface;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
T_uezError ST_Accelo_LIS3DH_Configure(void *aWorkspace,
	DEVICE_I2C_BUS **aI2C, TUInt8 reg, TUInt8 setting);
T_uezError ST_Accelo_LIS3DH_I2C_Create(
        const char *aName,
        const char *aI2CBusName);

#ifdef __cplusplus
}
#endif
		
#endif // ST_LIS3DH_I2C_H_
/*-------------------------------------------------------------------------*
 * End of File:  ST_LIS3DH_I2C.h
 *-------------------------------------------------------------------------*/
