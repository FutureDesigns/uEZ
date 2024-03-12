/*-------------------------------------------------------------------------*
 * File:  AWSIoTClient.c
 *-------------------------------------------------------------------------*
 * Description:
 *
 *-------------------------------------------------------------------------*/

 /*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://goo.gl/UDtTCR for details.
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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uEZ.h>
#include <uEZFlash.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Source/Library/Web/AWSIoTClient/AWSIoTClient.h"
#include "Source/Library/Web/AWSIoTClient/aws_dev_mode_key_provisioning.h"
#include "Source/Library/Encryption/mbedtls/port/uez_mbedtls.h"
#include "Source/RTOS/FreeRTOS-Plus/corePKCS11/source/include/core_pkcs11.h"
#include "Source/Library/Web/AWSIoTClient/aws_iot/iot-reference/examples/common/kvstore/kvstore.h"
#include "Source/Library/Web/AWSIoTClient/aws_iot/iot-reference/examples/common/mqtt_agent/mqtt_agent_task.h"
#include "Source/Library/Web/AWSIoTClient/mflash/mflash_file.h"

#include "AWSIoTClient.h"
#include "port/uez_mbedtls.h"
#include <sys/socket.h>
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
#include <uEZTimeDate.h>

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef struct 
{
    T_uezDevice iFlashDevice;
    T_uezDevice iNetworkDevice;
 } T_AWSIoTClient_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
extern NetworkContext_t xNetworkContext;

static T_AWSIoTClient_Workspace G_AWSIoTClient_Workspace;
static const mflash_file_t      dir_template[] = {{.path = KVSTORE_FILE_PATH, .max_size = 3000},
                                                  {.path = pkcs11palFILE_NAME_CLIENT_CERTIFICATE, .max_size = 2000},
                                                  {.path = pkcs11palFILE_NAME_KEY, .max_size = 2000},
                                                  {.path = pkcs11palFILE_CODE_SIGN_PUBLIC_KEY, .max_size = 2000},
                                                  {.path = pkcs11palFILE_NAME_JITR_CERTIFICATE, .max_size = 2000},
                                                  {0}};

TUInt8 gHostName[20] = "www.man7.org";
//TUInt8 gGetString[128] = "GET /index.html HTTP/1.1\r\nHost: www.man7.org\r\nConnection: close\r\n\r\n";
TUInt8 gGetString[128] = "GET /index.html HTTP/1.1\r\nHost: www.man7.org\r\nConnection: keep-alive\r\n\r\n";
TUInt8 gRespString[1024] = "";

/*---------------------------------------------------------------------------*
 * Prototypes:
 *---------------------------------------------------------------------------*/
extern void vShadowDeviceTask(void *pvParameters);
extern void vShadowUpdateTask(void *pvParameters);
extern T_uezTask G_lwipTask;

TaskHandle_t G_ShadowUpdateTask = NULL;
TaskHandle_t G_ShadowDeviceTask = NULL;
TaskHandle_t G_MqttTask = NULL;

/*---------------------------------------------------------------------------*
 * Routine:  AWSIoTClientStartUpTask (this requires large stack size)
 *---------------------------------------------------------------------------*/
//void AWSIoTClientStartUpTask( void * pvParameters )
void AWSIoTClientStartUpTask( T_uezTask aTask, void *pvParameters )
{
    //BaseType_t xStatus = pdFAIL;
    ( void ) aTask;
    T_uezError error = UEZ_ERROR_NONE;
    ( void ) pvParameters;
    TBool isRtcRecentlySet = EFalse;
    
    gNetwork = (T_uezDevice) pvParameters; //aNetwork;
    if(G_lwipTask != NULL) {
      lwip_socket_thread_init(); // if lwip init make sure to init per thread objects
    }

    // need accurate date to run TLS functions with date and cert checking
    T_uezTimeDate rtc_set_td, rtc_new_td;    
    if(UEZTimeDateGet(&rtc_set_td) == UEZ_ERROR_NONE) { // from RTC date will most likely not be > 999)
      if((rtc_set_td.iDate.iYear%100) < (G_aBuildTimeDate.iDate.iYear%100)) {
        DEBUG_RTT_Printf(0, "RTC unset, set year less than build year.\n");
      } else {
        TUInt32 daysFromBuild =  UEZDateCalcDuration(&rtc_set_td.iDate, &G_aBuildTimeDate.iDate);
        if(daysFromBuild > 5) {
           DEBUG_RTT_Printf(0, "RTC may be unset\n");
        } else {
          isRtcRecentlySet = ETrue;
        }
      }
        
      if(isRtcRecentlySet == EFalse) {
        DEBUG_RTT_Printf(0, "Manually registering an NTP server in case DHCP doesn't set one.\n");
        sntp_register_named_server(1, "pool.ntp.org"); // second server entry, in case DHCP populates entry 0.
        UEZTaskDelay(30000); // Need time to allow for RTC to be set from server. LPC4357 takes over 10 seconds to set the time.
        stop_sntp_server();

        if(UEZTimeDateGet(&rtc_new_td) == UEZ_ERROR_NONE) {
            TUInt8 difference =  UEZTimeDateCompare(&rtc_set_td, &rtc_new_td);
            if(difference == 0) {
                DEBUG_RTT_Printf(0, "Could not get RTC from NTP, setting to build date.\n");
                UEZTimeDateSet(&G_aBuildTimeDate);
                DEBUG_RTT_Printf(0, "RTC set to build date\n");
            } else {
                DEBUG_RTT_Printf(0, "RTC was set from NTP.\n");
            }
        }
      } else {
         DEBUG_RTT_Printf(0, "RTC was recently set or DHCP NTP is working.\n");
      }
    }

    memset(&G_AWSIoTClient_Workspace, 0, sizeof(G_AWSIoTClient_Workspace));

    /* TODO: Implement based on `init_task` of LPCXpresso54018 AWS Shadow demo */
    DEBUG_RTT_Printf(0, "AWSIoTClientStart starting.\n");

    error = (T_uezError) CRYPTO_InitHardware();
    if (UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "AWSIoTClientStart::CRYPTO_InitHardware failed, error=%d\n", error);

        __BKPT(0);
    }
    vTaskDelay(1);

    error = mflash_init(dir_template, true);
    if (UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "AWSIoTClientStart::mflash_init failed, error=%d\n", error);

        __BKPT(0);
    }
    vTaskDelay(1);

    /* A simple example to demonstrate key and certificate provisioning in
     * microcontroller flash using PKCS#11 interface. This should be replaced
     * by production ready key provisioning mechanism. */
    CK_RV ret_prov = vDevModeKeyProvisioning();
    if (ret_prov != CKR_OK)
    {
        DEBUG_RTT_Printf(0, "AWSIoTClientStart::vDevModeKeyProvisioning failed, ret_prov=%d\n", ret_prov);

        __BKPT(0);
    }
    vTaskDelay(1);

    /* AG: Network is already initialized at this point */
#if 0
    /* Initialize network. */
    if (init_network() != kStatus_Success)
    {
        PRINTF("\r\nInitialization of network failed.\r\n");
        while (1)
        {
        }
    }
#endif

    BaseType_t xResult = pdFAIL;

    xResult = KVStore_init();
    vTaskDelay(1);

    if (xResult == pdFAIL)
    {
        configPRINTF(("Failed to initialize key value configuration store.\r\n"));
    }

    if (xResult == pdPASS)
    {
        xResult = xMQTTAgentInit(appmainMQTT_AGENT_TASK_STACK_SIZE, appmainMQTT_AGENT_TASK_PRIORITY, &G_MqttTask);
    }

    if (xResult == pdPASS)
    {
        xResult = xTaskCreate(vShadowDeviceTask, "SHADOW_DEV", appmainSHADOW_DEVICE_TASK_STACK_SIZE, NULL,
                              appmainSHADOW_DEVICE_TASK_PRIORITY, &G_ShadowDeviceTask);
    }

    if (xResult == pdPASS)
    {
        xResult = xTaskCreate(vShadowUpdateTask, "SHADOW_APP", appmainSHADOW_UPDATE_TASK_STACK_SIZE, NULL,
                              appmainSHADOW_UPDATE_TASK_PRIORITY, &G_ShadowUpdateTask);
    }

    if (xResult != pdPASS)
    {
        DEBUG_RTT_Printf(0, "AWSIoTClientStart::xTaskCreate failed!\n");
        __BKPT(0);
    }

    // Exit task and free up memory from this "run-once" task.
}

/*---------------------------------------------------------------------------*
 * Routine:  AWSIoTClientStart
 *---------------------------------------------------------------------------*/
T_uezError AWSIoTClientStart(T_uezDevice aNetwork)
{
#if 0
   return xTaskCreate( AWSIoTClientStartUpTask,
                       "InitAWS",
                       2048,
                       (void *) aNetwork,
                       appmainMQTT_AGENT_TASK_PRIORITY,
                       NULL );
#else
   return UEZTaskCreate(
                (T_uezTaskFunction)AWSIoTClientStartUpTask,
                "InitAWS",                
                UEZ_TASK_STACK_BYTES(8192),
                (void *)aNetwork,
                UEZ_PRIORITY_NORMAL,
                NULL); //&G_awsStartupTask);
#endif
}

T_uezError AWSIoTClientStop(void)
{
  vTaskSuspend(G_ShadowUpdateTask);
  vTaskSuspend(G_ShadowDeviceTask);
  vTaskSuspend(G_MqttTask);
  return UEZ_ERROR_NONE;
}

T_uezError AWSIoTClientRestart(void)
{
  vTaskResume(G_MqttTask);
  vTaskResume(G_ShadowDeviceTask);
  vTaskResume(G_ShadowUpdateTask);
  return UEZ_ERROR_NONE;
}

/*-------------------------------------------------------------------------*
 * End of File:  AWSIoTClient.c
 *-------------------------------------------------------------------------*/
