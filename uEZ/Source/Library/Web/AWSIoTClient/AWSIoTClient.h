/*-------------------------------------------------------------------------*
 * File:  AWSIoTClient.h
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/
#ifndef AWS_IOT_CLIENT_H_
#define AWS_IOT_CLIENT_H_

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
 * Constants:
 *-------------------------------------------------------------------------*/
/**
 * @brief Stack size and priority for shadow device sync task.
 */
#define appmainSHADOW_DEVICE_TASK_STACK_SIZE (4096)
#define appmainSHADOW_DEVICE_TASK_PRIORITY   (tskIDLE_PRIORITY+UEZ_PRIORITY_LOW)

/**
 * @brief Stack size and priority for shadow update application task.
 */
#define appmainSHADOW_UPDATE_TASK_STACK_SIZE (4096)
#define appmainSHADOW_UPDATE_TASK_PRIORITY   (tskIDLE_PRIORITY+UEZ_PRIORITY_NORMAL)

/**
 * @brief Stack size and priority for MQTT agent task.
 * Stack size is capped to an adequate value based on requirements from MbedTLS stack
 * for establishing a TLS connection. Task priority of MQTT agent is set to a priority
 * higher than other MQTT application tasks, so that the agent can drain the queue
 * as work is being produced.
 */
#define appmainMQTT_AGENT_TASK_STACK_SIZE (2048*9)// match what Renesas RA demo uses until proven we can use less.
#define appmainMQTT_AGENT_TASK_PRIORITY   (tskIDLE_PRIORITY+UEZ_PRIORITY_NORMAL)

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
/* The function that implements the AWS IoT Client Task. */
T_uezError AWSIoTClientStart(T_uezDevice aNetwork);
T_uezError AWSIoTClientStop(void);
T_uezError AWSIoTClientRestart(void);

#endif // AWS_IOT_CLIENT_H_
/*-------------------------------------------------------------------------*
 * End of File:  AWSIoTClient.h
 *-------------------------------------------------------------------------*/
