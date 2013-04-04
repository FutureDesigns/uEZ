/*-------------------------------------------------------------------------*
 * File:  RS9110_N_11_22.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef RS9110_N_11_22_H_
#define RS9110_N_11_22_H_

/*--------------------------------------------------------------------------
 * uEZ(tm) - Copyright (C) 2007-2011 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(tm) distribution.
 *
 * uEZ(tm) is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * uEZ(tm) is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with uEZ(tm); if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * A special exception to the GPL can be applied should you wish to
 * distribute a combined work that includes uEZ(tm), without being obliged
 * to provide the source code for any proprietary components.  See the
 * licensing section of http://www.teamfdi.com/uez for full details of how
 * and when the exception can be applied.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <Device/Network.h>
#include <Device/SPIBus.h>
#include <HAL/GPIO.h>
#include <Types/GPIO.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
// How fast is the SPI data transferred?
#ifndef REDPINE_RS9110_N_11_22_SPI_RATE
#define REDPINE_RS9110_N_11_22_SPI_RATE    1000 // kHz
#endif

// How often is the link status checked?
#ifndef REDPINE_RS9110_N_11_22_CHECK_STATUS_INTERVAL
#define REDPINE_RS9110_N_11_22_CHECK_STATUS_INTERVAL    250 // ms
#endif

#ifndef REDPINE_RS9110_N_11_22_RECV_QUEUE_SIZE
#define REDPINE_RS9110_N_11_22_RECV_QUEUE_SIZE          1024 // bytes
#endif

#ifndef REDPINE_RS9110_N_11_22_MAX_NUM_SOCKETS
#define REDPINE_RS9110_N_11_22_MAX_NUM_SOCKETS          2 // 4
#endif

// Aux Commands:
#define REDPINE_RS9110_N_11_22_AUX_COMMAND_GET_FIRMWARE_VERSION     1

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/
typedef struct {
        const char *iSPIBusName;
        TUInt32 iSPIRate;
        T_uezGPIOPortPin iSPIChipSelect;
        T_uezGPIOPortPin iReset;
        T_uezGPIOPortPin iPower;
        T_uezGPIOPortPin iInterrupt;
} T_Redpine_RS9110_Settings;

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
void Network_Redpine_RS9110_Configure(
        void *aWorkspace,
        DEVICE_SPI_BUS **aSPI,
        TUInt32 aSPIRate,
        HAL_GPIOPort **aSPICSPort,
        TUInt32 aSPICSPin,
        HAL_GPIOPort **aGPIOResetPort,
        TUInt32 aGPIOResetPin,
        HAL_GPIOPort **aGPIOPowerPort,
        TUInt32 aGPIOPowerPin,
        HAL_GPIOPort **aGPIOInterruptPort,
        TUInt32 aGPIOInterruptPin);
T_uezError Network_Redpine_RS9110_Create(
    const char *aName,
    T_Redpine_RS9110_Settings *aSettings);

extern const DEVICE_Network Redpine_RS9110_N_11_22_Over_SPI_Network_Interface;

void Network_Redpine_RS9110_Scan_Complete_Callback(void);
void Network_Redpine_RS9110_Join_Complete_Callback(TBool aSuccess);
void RedpineRS9110_Internal_TransferBytes(
        TUInt8 *aDataMOSI,
        TUInt8 *aDataMISO,
        TUInt32 aNumBytes);

#endif // RS9110_N_11_22_H_
/*-------------------------------------------------------------------------*
 * End of File:  RS9110_N_11_22.h
 *-------------------------------------------------------------------------*/
