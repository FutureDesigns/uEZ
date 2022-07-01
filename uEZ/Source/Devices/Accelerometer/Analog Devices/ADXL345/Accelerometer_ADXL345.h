/*-------------------------------------------------------------------------*
 * File:  Accelerometer_ADXL345.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef ACCELEROMETER_ADXL345_H_
#define ACCELEROMETER_ADXL345_H_

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
* licensing section of http://www.teamfdi.com/uez for full details of how
* and when the exception can be applied.
*
*    *===============================================================*
*    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
*    |             We can get you up and running fast!               |
*    |      See http://www.teamfdi.com/uez for more details.         |
*    *===============================================================*
*
*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <Device/Accelerometer.h>
#include <Device/I2CBus.h>

#ifdef __cplusplus
extern “C” {
#endif

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define ADXL345_ADDR                 (0x3A>>1)    // 7 bit addr

#ifndef ADXL345_SPEED
#define ADXL345_SPEED                400     // kHz
#endif

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
extern const DEVICE_Accelerometer Accelerometer_ADXL345_Interface;
T_uezError ST_Accelerometer_ADXL345_Configure(
    void *aWorkspace,
    DEVICE_I2C_BUS **aI2C);
T_uezError ST_Accelerometer_ADXL345_Create(
        const char *aName,
        const char *aI2CName);
		
#ifdef __cplusplus
}
#endif

#endif // ACCELEROMETER_ADXL345_H_
/*-------------------------------------------------------------------------*
 * End of File:  Accelerometer_ADXL345.h
 *-------------------------------------------------------------------------*/
