/*
 * FreeRTOS V202012.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

/*
 * This demo task simulates an application or service which updates a device shadow using
 * device shadow library.
 *
 * This example assumes there is a buzzerHz state in the device shadow. It does the
 * following operations:
 * 1. Assemble strings for the MQTT topics of device shadow, by using macros defined by the Device Shadow library.
 * 2. Subscribe to those MQTT topics using the MQTT Agent.
 * 3. Register callbacks for incoming shadow topic publishes with the subsciption_manager.
 * 4. Wait until it is time to publish a requested change.
 * 5. Publish a desired state of buzzerHz. That will cause a delta message to be sent to device.
 * 6. Wait until either prvIncomingPublishUpdateAcceptedCallback or prvIncomingPublishUpdateRejectedCallback handle
 *    the response.
 * 7. Repeat from step 4.
 */

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Demo config. */
#include "Source/Library/Web/AWSIoTClient/config_files/demo_config.h"

/* MQTT library includes. */
#include "Source/RTOS/FreeRTOS-Plus/Application-Protocols/coreMQTT-Agent/source/include/core_mqtt_agent.h"

/* JSON library includes. */
#include "Source/RTOS/FreeRTOS-Plus/coreJSON/source/include/core_json.h"

/* Shadow API header. */
#include "Source/RTOS/FreeRTOS-Plus/AWS/device-shadow/source/include/shadow.h"


/* API to fetch thingname */
#include "Source/Library/Web/AWSIoTClient/aws_iot/iot-reference/examples/common/kvstore/kvstore.h"

/* MQTT Agent task APIs. */
#include "Source/Library/Web/AWSIoTClient/aws_iot/iot-reference/examples/common/mqtt_agent/mqtt_agent_task.h"

// includes for sensor readings to update
#include <uEZDeviceTable.h>
#include <Device/Temperature.h>
#include <Device/Accelerometer.h>
#include <Device/RTC.h>
//#include <uEZTimeDate.h>
#include <uEZTemperature.h>
#include <uEZAccelerometer.h>
#include <uEZPlatformAPI.h>

// for socket init
#include <sys/socket.h>

/**
 * @brief Format string representing a Shadow document with a "desired" state.
 *
 * The real json document will look like this:
 * {
 *   "state": {
 *     "desired": {
 *       "buzzerHz": 1
 *     }
 *   },
 *   "clientToken": "021909"
 * }
 *
 * Note the client token, which is optional. The token is used to identify the
 * response to an update. The client token must be unique at any given time,
 * but may be reused once the update is completed. For this demo, a timestamp
 * is used for a client token.
 */
#if 0
#define shadowexampleSHADOW_DESIRED_JSON \
    "{"                                  \
    "\"state\":{"                        \
    "\"desired\":{"                      \
    "\"powerOn\":%1u"                    \
    "}"                                  \
    "},"                                 \
    "\"clientToken\":\"%06lu\""          \
    "}"
#endif
// FDI updates to add sensor data.
// Note the following: The last number in each group cannot be followed by a comma.
// If there is a space between a negative sign and the number it will be rejected.
#define shadowexampleSHADOW_DESIRED_JSON \
    "{"                                  \
    "\"state\":{"                        \
    "\"reported\":{"                     \
    "\"buzzerHz\":%5u,"                  \
    "\"tempC\":%3d.%01d,"                \
    "\"accX\":%1d.%01d,"                 \
    "\"accY\":%1d.%01d,"                 \
    "\"accZ\":%1d.%01d,"                 \
    "\"vmon\":%5.1f,"                    \
    "\"touchX\":%4d,"                    \
    "\"touchY\":%4d"                     \
    "}"                                  \
    "},"                                 \
    "\"clientToken\":\"%06u\""           \
    "}"


/**
 * @brief The expected size of #SHADOW_DESIRED_JSON.
 *
 * Since all of the format specifiers in #SHADOW_DESIRED_JSON include a length,
 * its actual size can be precalculated at compile time from the difference between
 * the lengths of the format strings and their formatted output. We must subtract 2
 * from the length as according the following formula:
 * 1. The length of the format string "%1u" is 3.
 * 2. The length of the format string "%06lu" is 5.
 * 3. The formatted length in case 1. is 1 ( for the state of buzzerHz ).
 * 4. The formatted length in case 2. is 6 ( for the clientToken length ).
 * 5. Thus the additional size of our format is 2 = 3 + 5 - 1 - 6 + (4*(4-1)) + 1 (termination character).
 *
 * Custom applications may calculate the length of the JSON document with the same method.
 */
#define shadowexampleSHADOW_DESIRED_JSON_LENGTH    ( sizeof( shadowexampleSHADOW_DESIRED_JSON ) + 20 )


/**
 * @brief Time in ms to wait between requesting changes to the device shadow.
 */
#define shadowexampleMS_BETWEEN_REQUESTS               ( 25*1000U )

/**
 * @brief This demo uses task notifications to signal tasks from MQTT callback
 * functions. shadowexampleMS_TO_WAIT_FOR_NOTIFICATION defines the time, in ticks,
 * to wait for such a callback.
 */
#define shadowexampleMS_TO_WAIT_FOR_NOTIFICATION       ( 15000 )

/**
 * @brief The maximum amount of time in milliseconds to wait for the commands
 * to be posted to the MQTT agent should the MQTT agent's command queue be full.
 * Tasks wait in the Blocked state, so don't use any CPU time.
 */
#define shadowexampleMAX_COMMAND_SEND_BLOCK_TIME_MS    ( 1000 )

/**
 * @brief Maximum length of the topic buffer used to receive update accepted notifications from AWS IoT Shadow service.
 */
#define shadowexampleTOPIC_LENGTH_UPDATE_ACCEPTED                   \
    SHADOW_TOPIC_LEN_UPDATE_ACC( SHADOW_THINGNAME_MAX_LENGTH, \
                                 shadowexampleSHADOW_NAME_LENGTH )

/**
 * @brief Maximum length of the topic buffer used to receive update rejected notifications from AWS IoT Shadow service.
 */
#define shadowexampleTOPIC_LENGTH_UPDATE_REJECTED                   \
    SHADOW_TOPIC_LEN_UPDATE_REJ( SHADOW_THINGNAME_MAX_LENGTH, \
                                 shadowexampleSHADOW_NAME_LENGTH )

/**
 * @brief Maximum length of the topic buffer used to report the state of the device to AWS IoT Shadow service.
 */
#define shadowexampleTOPIC_LENGTH_UPDATE                        \
    SHADOW_TOPIC_LEN_UPDATE( SHADOW_THINGNAME_MAX_LENGTH, \
                             shadowexampleSHADOW_NAME_LENGTH )

/**
 * @brief Defines the structure to use as the command callback context in this
 * demo.
 */
struct MQTTAgentCommandContext
{
    BaseType_t xReturnStatus;
};

extern MQTTAgentContext_t xGlobalMqttAgentContext;
extern T_uezTask G_lwipTask;
extern uint16_t ucDeviceState;
/*-----------------------------------------------------------*/

/**
 * @brief Match the received clientToken with the one sent in a device shadow
 * update. Set to 0 when not waiting on a response.
 */
static uint32_t ulClientToken = 0U;

/**
 * @brief The handle of this task. It is used by callbacks to notify this task.
 */
static TaskHandle_t xShadowUpdateTaskHandle;

/**
 * @brief Topic is used to receive update accepted notifications from AWS IoT shadow
 * service.
 */
static char cUpdateAcceptedTopic[ shadowexampleTOPIC_LENGTH_UPDATE_ACCEPTED ];
static uint16_t usUpdateAcceptedTopicLength = 0U;

/**
 * @brief Topic is used to receive update rejected notifications from AWS IoT shadow
 * service.
 */
static char cUpdateRejectedTopic[ shadowexampleTOPIC_LENGTH_UPDATE_REJECTED ];
static uint16_t usUpdateRejectedTopicLength = 0U;

/**
 * @brief Topic is used to update the state of device in AWS IoT shadow service.
 */
static char cUpdateTopic[ shadowexampleTOPIC_LENGTH_UPDATE ];
static uint16_t usUpdateTopicLength = 0U;


static char cThingName[ SHADOW_THINGNAME_MAX_LENGTH + 1U ];
static size_t xThingNameLength = 0U;


/*-----------------------------------------------------------*/

/**
 * @brief Creates the required shadow topics for the example, given thing name as the parameter.
 *
 * @param[in] pcThingName Pointer to the thing name
 * @param[in] aThingNameLength Length of the thing name.
 * @return true if the shadow topics are created successfully, false otherwise.
 */
static bool prvCreateShadowTopics( const char * pcThingName,
                                   size_t aThingNameLength );


/**
 * @brief Subscribe to the used device shadow topics.
 *
 * @return true if the subscribe is successful;
 * false otherwise.
 */
static bool prvSubscribeToShadowUpdateTopics( void );

/**
 * @brief Passed into MQTTAgent_Subscribe() as the callback to execute when the
 * broker ACKs the SUBSCRIBE message. Its implementation sends a notification
 * to the task that called MQTTAgent_Subscribe() to let the task know the
 * SUBSCRIBE operation completed. It also sets the xReturnStatus of the
 * structure passed in as the command's context to the value of the
 * xReturnStatus parameter - which enables the task to check the status of the
 * operation.
 *
 * See https://freertos.org/mqtt/mqtt-agent-demo.html#example_mqtt_api_call
 *
 * @param[in] pxCommandContext Context of the initial command.
 * @param[in] pxReturnInfo The result of the command.
 */
static void prvSubscribeCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                         MQTTAgentReturnInfo_t * pxReturnInfo );

/**
 * @brief The callback to execute when there is an incoming publish on the
 * topic for accepted requests. It verifies the document is valid and is being waited on.
 * If so it notifies the task to inform completion of the update request.
 *
 * @param[in] pvIncomingPublishCallbackContext Context of the initial command.
 * @param[in] pxPublishInfo Deserialized publish.
 */
static void prvIncomingPublishUpdateAcceptedCallback( void * pxSubscriptionContext,
                                                      MQTTPublishInfo_t * pxPublishInfo );



/**
 * @brief The callback to execute when there is an incoming publish on the
 * topic for rejected requests. It verifies the document is valid and is being waited on.
 * If so it notifies the task to inform completion of the update request.
 *
 * @param[in] pvIncomingPublishCallbackContext Context of the initial command.
 * @param[in] pxPublishInfo Deserialized publish.
 */
static void prvIncomingPublishUpdateRejectedCallback( void * pxSubscriptionContext,
                                                      MQTTPublishInfo_t * pxPublishInfo );

/**
 * @brief Entry point of shadow demo.
 *
 * This main function demonstrates how to use the macros provided by the
 * Device Shadow library to assemble strings for the MQTT topics defined
 * by AWS IoT Device Shadow. It uses these macros for topics to subscribe
 * to:
 * - SHADOW_TOPIC_STRING_UPDATE_DELTA for "$aws/things/thingName/shadow/update/delta"
 * - SHADOW_TOPIC_STRING_UPDATE_ACCEPTED for "$aws/things/thingName/shadow/update/accepted"
 * - SHADOW_TOPIC_STRING_UPDATE_REJECTED for "$aws/things/thingName/shadow/update/rejected"
 *
 * It also uses these macros for topics to publish to:
 * - SHADOW_TOPIC_STIRNG_DELETE for "$aws/things/thingName/shadow/delete"
 * - SHADOW_TOPIC_STRING_UPDATE for "$aws/things/thingName/shadow/update"
 */
void vShadowUpdateTask( void * pvParameters );

/*-----------------------------------------------------------*/

static bool prvCreateShadowTopics( const char * pcThingName,
                                   size_t aThingNameLength )
{
    bool xStatus = true;
    ShadowStatus_t xShadowStatus = SHADOW_FAIL;

    if( xStatus == true )
    {
        xShadowStatus = Shadow_AssembleTopicString( ShadowTopicStringTypeUpdate,
                                                    pcThingName,
                                                    aThingNameLength,
                                                    shadowexampleSHADOW_NAME,
                                                    shadowexampleSHADOW_NAME_LENGTH,
                                                    cUpdateTopic,
                                                    shadowexampleTOPIC_LENGTH_UPDATE,
                                                    &usUpdateTopicLength );

        if( xShadowStatus != SHADOW_SUCCESS )
        {
            LogError( ( "Fail to construct update topic, error = %u.", xShadowStatus ) );
            xStatus = false;
        }
    }

    if( xStatus == true )
    {
        xShadowStatus = Shadow_AssembleTopicString( ShadowTopicStringTypeUpdateAccepted,
                                                    pcThingName,
                                                    aThingNameLength,
                                                    shadowexampleSHADOW_NAME,
                                                    shadowexampleSHADOW_NAME_LENGTH,
                                                    cUpdateAcceptedTopic,
                                                    shadowexampleTOPIC_LENGTH_UPDATE_ACCEPTED,
                                                    &usUpdateAcceptedTopicLength );

        if( xShadowStatus != SHADOW_SUCCESS )
        {
            LogError( ( "Fail to construct update accepted topic, error = %u.", xShadowStatus ) );
            xStatus = false;
        }
    }

    if( xStatus == true )
    {
        xShadowStatus = Shadow_AssembleTopicString( ShadowTopicStringTypeUpdateRejected,
                                                    pcThingName,
                                                    aThingNameLength,
                                                    shadowexampleSHADOW_NAME,
                                                    shadowexampleSHADOW_NAME_LENGTH,
                                                    cUpdateRejectedTopic,
                                                    shadowexampleTOPIC_LENGTH_UPDATE_REJECTED,
                                                    &usUpdateRejectedTopicLength );

        if( xShadowStatus != SHADOW_SUCCESS )
        {
            LogError( ( "Fail to construct update rejected topic, error = %u.", xShadowStatus ) );
            xStatus = false;
        }
    }

    return xStatus;
}

/*-----------------------------------------------------------*/

static bool prvSubscribeToShadowUpdateTopics( void )
{
    bool xReturnStatus = false;
    MQTTStatus_t xStatus;
    uint32_t ulNotificationValue;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    ( void ) ulNotificationValue;

    /* These must persist until the command is processed. */
    MQTTAgentSubscribeArgs_t xSubscribeArgs;
    MQTTSubscribeInfo_t xSubscribeInfo[ 2 ];
    MQTTAgentCommandContext_t xApplicationDefinedContext = { 0 };

    /* Subscribe to shadow topic for accepted responses for submitted updates. */
    xSubscribeInfo[ 0 ].pTopicFilter = cUpdateAcceptedTopic;
    xSubscribeInfo[ 0 ].topicFilterLength = usUpdateAcceptedTopicLength;
    xSubscribeInfo[ 0 ].qos = MQTTQoS1;
    /* Subscribe to shadow topic for rejected responses for submitted updates. */
    xSubscribeInfo[ 1 ].pTopicFilter = cUpdateRejectedTopic;
    xSubscribeInfo[ 1 ].topicFilterLength = usUpdateRejectedTopicLength;
    xSubscribeInfo[ 1 ].qos = MQTTQoS1;

    /* Complete the subscribe information.  The topic string must persist for
     * duration of subscription - although in this case is it a static const so
     * will persist for the lifetime of the application. */
    xSubscribeArgs.pSubscribeInfo = xSubscribeInfo;
    xSubscribeArgs.numSubscriptions = 2;

    /* Loop in case the queue used to communicate with the MQTT agent is full and
     * attempts to post to it time out.  The queue will not become full if the
     * priority of the MQTT agent task is higher than the priority of the task
     * calling this function. */
    xTaskNotifyStateClear( NULL );
    xCommandParams.blockTimeMs = shadowexampleMAX_COMMAND_SEND_BLOCK_TIME_MS;
    xCommandParams.cmdCompleteCallback = prvSubscribeCommandCallback;
    xCommandParams.pCmdCompleteCallbackContext = &xApplicationDefinedContext;
    LogInfo( ( "Sending subscribe request to agent for shadow topics." ) );

    do
    {
        /* If this fails, the agent's queue is full, so we retry until the agent
         * has more space in the queue. */
        xStatus = MQTTAgent_Subscribe( &xGlobalMqttAgentContext,
                                       &( xSubscribeArgs ),
                                       &xCommandParams );
    } while( xStatus != MQTTSuccess );

    /* Wait for acks from subscribe messages - this is optional.  If the
     * returned value is zero then the wait timed out. */
    ulNotificationValue = ulTaskNotifyTake( pdFALSE, pdMS_TO_TICKS( shadowexampleMS_TO_WAIT_FOR_NOTIFICATION ) );
    configASSERT( ulNotificationValue != 0UL );

    /* The callback sets the xReturnStatus member of the context. */
    if( xApplicationDefinedContext.xReturnStatus != true )
    {
        LogError( ( "Failed to subscribe to shadow update topics." ) );
    }
    else
    {
        LogInfo( ( "Received subscribe ack for shadow update topics." ) );
        xReturnStatus = true;
    }

    return xReturnStatus;
}

/*-----------------------------------------------------------*/

static void prvSubscribeCommandCallback( MQTTAgentCommandContext_t * pxCommandContext,
                                         MQTTAgentReturnInfo_t * pxReturnInfo )
{
    BaseType_t xSubscriptionAdded = pdFALSE;

    /* Check if the subscribe operation is a success. */
    if( pxReturnInfo->returnCode == MQTTSuccess )
    {
        /* Add subscriptions so that incoming publishes are routed to the application
         * callback. */
        xSubscriptionAdded = xAddMQTTTopicFilterCallback( cUpdateAcceptedTopic,
                                                          usUpdateAcceptedTopicLength,
                                                          prvIncomingPublishUpdateAcceptedCallback,
                                                          NULL,
                                                          pdTRUE );
        configASSERT( xSubscriptionAdded == pdTRUE );

        xSubscriptionAdded = xAddMQTTTopicFilterCallback( cUpdateRejectedTopic,
                                                          usUpdateRejectedTopicLength,
                                                          prvIncomingPublishUpdateRejectedCallback,
                                                          NULL,
                                                          pdTRUE );
        configASSERT( xSubscriptionAdded == pdTRUE );
    }

    /* Store the result in the application defined context so the calling task
     * can check it. */
    pxCommandContext->xReturnStatus = xSubscriptionAdded;

    xTaskNotifyGive( xShadowUpdateTaskHandle );
}


/*-----------------------------------------------------------*/

static void prvIncomingPublishUpdateAcceptedCallback( void * pxSubscriptionContext,
                                                      MQTTPublishInfo_t * pxPublishInfo )
{
    char * pcOutValue = NULL;
    uint32_t ulOutValueLength = 0U;
    uint32_t ulReceivedToken = 0U;
    JSONStatus_t result = JSONSuccess;

    /* Remove compiler warnings about unused parameters. */
    ( void ) pxSubscriptionContext;

    configASSERT( pxPublishInfo != NULL );
    configASSERT( pxPublishInfo->pPayload != NULL );

    LogDebug( ( "/update/accepted JSON payload: %.*s.",
                pxPublishInfo->payloadLength,
                ( const char * ) pxPublishInfo->pPayload ) );

    /* Handle the reported state with state change in /update/accepted topic.
     * Thus we will retrieve the client token from the JSON document to see if
     * it's the same one we sent with reported state on the /update topic.
     * The payload will look similar to this:
     *  {
     *      "state": {
     *          "desired": {
     *             "buzzerHz": 1
     *          }
     *      },
     *      "metadata": {
     *          "desired": {
     *              "buzzerHz": {
     *                  "timestamp": 1596573647
     *              }
     *          }
     *      },
     *      "version": 14698,
     *      "timestamp": 1596573647,
     *      "clientToken": "022485"
     *  }
     */

    /* Make sure the payload is a valid json document. */
    result = JSON_Validate( pxPublishInfo->pPayload,
                            pxPublishInfo->payloadLength );

    if( result != JSONSuccess )
    {
        LogError( ( "Invalid JSON document recieved!" ) );
    }
    else
    {
        /* Get clientToken from json documents. */
        result = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                              pxPublishInfo->payloadLength,
                              "clientToken",
                              sizeof( "clientToken" ) - 1,
                              &pcOutValue,
                              ( size_t * ) &ulOutValueLength );
    }

    if( result != JSONSuccess )
    {
        LogDebug( ( "Ignoring publish on /update/accepted with no clientToken field." ) );
    }
    else
    {
        /* Convert the code to an unsigned integer value. */
        ulReceivedToken = ( uint32_t ) strtoul( pcOutValue, NULL, 10 );

        /* If we are waiting for a response, ulClientToken will be the token for the response
         * we are waiting for, else it will be 0. ulRecievedToken may not match if the response is
         * not for us or if it is is a response that arrived after we timed out
         * waiting for it.
         */
        if( ulReceivedToken != ulClientToken )
        {
            LogDebug( ( "Ignoring publish on /update/accepted with clientToken %u.", ( uint32_t ) ulReceivedToken ) );
        }
        else
        {
            LogInfo( ( "Received accepted response for update with token %u. ", ( uint32_t ) ulClientToken ) );

            /* Wake up the shadow task which is waiting for this response. */
            xTaskNotifyGive( xShadowUpdateTaskHandle );
        }
    }
}

/*-----------------------------------------------------------*/

static void prvIncomingPublishUpdateRejectedCallback( void * pxSubscriptionContext,
                                                      MQTTPublishInfo_t * pxPublishInfo )
{
    JSONStatus_t result = JSONSuccess;
    char * pcOutValue = NULL;
    uint32_t ulOutValueLength = 0UL;
    uint32_t ulReceivedToken = 0U;

    /* Remove compiler warnings about unused parameters. */
    ( void ) pxSubscriptionContext;

    configASSERT( pxPublishInfo != NULL );
    configASSERT( pxPublishInfo->pPayload != NULL );

    LogDebug( ( "/update/rejected json payload: %.*s.",
                pxPublishInfo->payloadLength,
                ( const char * ) pxPublishInfo->pPayload ) );

    /* The payload will look similar to this:
     * {
     *    "code": error-code,
     *    "message": "error-message",
     *    "timestamp": timestamp,
     *    "clientToken": "token"
     * }
     */

    /* Make sure the payload is a valid json document. */
    result = JSON_Validate( pxPublishInfo->pPayload,
                            pxPublishInfo->payloadLength );

    if( result != JSONSuccess )
    {
        LogError( ( "Invalid JSON document recieved!" ) );
    }
    else
    {
        /* Get clientToken from json documents. */
        result = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                              pxPublishInfo->payloadLength,
                              "clientToken",
                              sizeof( "clientToken" ) - 1,
                              &pcOutValue,
                              ( size_t * ) &ulOutValueLength );
    }

    if( result != JSONSuccess )
    {
        LogDebug( ( "Ignoring publish on /update/rejected with clientToken %u.", ( uint32_t ) ulReceivedToken ) );
    }
    else
    {
        /* Convert the code to an unsigned integer value. */
        ulReceivedToken = ( uint32_t ) strtoul( pcOutValue, NULL, 10 );

        /* If we are waiting for a response, ulClientToken will be the token for the response
         * we are waiting for, else it will be 0. ulRecievedToken may not match if the response is
         * not for us or if it is is a response that arrived after we timed out
         * waiting for it.
         */
        if( ulReceivedToken != ulClientToken )
        {
            LogDebug( ( "Ignoring publish on /update/rejected with clientToken %u.", ( uint32_t ) ulReceivedToken ) );
        }
        else
        {
            /*  Obtain the error code. */
            result = JSON_Search( ( char * ) pxPublishInfo->pPayload,
                                  pxPublishInfo->payloadLength,
                                  "code",
                                  sizeof( "code" ) - 1,
                                  &pcOutValue,
                                  ( size_t * ) &ulOutValueLength );

            if( result != JSONSuccess )
            {
                LogWarn( ( "Received rejected response for update with token %u and no error code.", ( uint32_t ) ulClientToken ) );
            }
            else
            {
                LogWarn( ( "Received rejected response for update with token %u and error code %.*s.", ( uint32_t ) ulClientToken,
                           ulOutValueLength,
                           pcOutValue ) );
            }

            /* Wake up the shadow task which is waiting for this response. */
            xTaskNotifyGive( xShadowUpdateTaskHandle );
        }
    }
}

// read these text strings into the GUI to update labels
char G_CurrentShadowReportTemp[12] = {0};
char G_CurrentShadowReportAcc[40] = {0};
char G_CurrentShadowReport5V[10] = {0};
char G_CurrentShadowReportTouch[20] = {0};
char G_CurrentShadowReportClientT[12] = {0};

/*-----------------------------------------------------------*/

void vShadowUpdateTask( void * pvParameters )
{
    /* A buffer containing the desired document. It has static duration to prevent
     * it from being placed on the call stack. */
    static char pcDesiredDocument[ shadowexampleSHADOW_DESIRED_JSON_LENGTH + 1 ] = { 0 };
    static MQTTPublishInfo_t xPublishInfo = { MQTTQoS0 };

    bool xStatus = true;
    uint32_t ulNotificationValue;
    MQTTAgentCommandInfo_t xCommandParams = { 0 };
    MQTTStatus_t xCommandAdded;

    /* Variables for collecting device data */
    T_uezError error;
    /* Temperature */
    T_uezDevice temperature;
    TInt32 temp_raw, temp_int, temp_frac;
    /* Accelerometer */
    T_uezDevice acc_device;
    DEVICE_Accelerometer **G_accel0;
    AccelerometerReading reading;
    TInt16 xI, xF, yI, yF, zI, zF;
    /* Voltage monitor */
    TUInt32 vmon_raw = 0;
    float vmon = 0.0;
    /* Touchscreen */
    T_uezDevice touchscreen;
    T_uezQueue touch_queue;
    T_uezInputEvent touch_event = { 0 };
    static T_uezInputEvent prev_touch_event = { 0 };
    uint8_t tries;
    
    if(G_lwipTask != NULL) 
    {
        lwip_socket_thread_init(); // if lwip init make sure to init per thread objects
    }

    //char number[12];
    //float currentTemperatureC = 0.0;
    if (UEZTemperatureOpen("Temp0", &temperature) != UEZ_ERROR_NONE) 
    {
        LogError( ( "Failed to open temperature sensor! " ) );
    }

    // Three signed readings from accelerometer (x, y, and z).
    // Readings are in g's with signed 15.16 fixed notation
    error = UEZDeviceTableFind("Accel0", &acc_device);
    if (!error)
    {
        error = UEZDeviceTableGetWorkspace(acc_device,
                    (T_uezDeviceWorkspace **)&G_accel0);
    }
    
    error = (*G_accel0)->ReadXYZ(G_accel0, &reading, 4000);
    if (error != UEZ_ERROR_NONE) 
    {
        LogError( ( "Failed to open accelerometer! " ) );
    }
    
    /* Setup touchscreen queue to get touch events */
    error = UEZDeviceTableFind("Touchscreen", &touchscreen);
    if (error != UEZ_ERROR_NONE)
    {
        LogError( ( "Failed to find touchscreen!") );
    }
    else
    {
        error = UEZQueueCreate(50, sizeof(T_uezInputEvent), &touch_queue);
        if (error != UEZ_ERROR_NONE)
        {
            LogError( ( "Failed to create touch queue! " ) );
        }
        else
        {
            error = UEZTSAddQueue(touchscreen, touch_queue);
        }
    }

    /* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

    /* Record the handle of this task so that the callbacks so the callbacks can
     * send a notification to this task. */
    xShadowUpdateTaskHandle = xTaskGetCurrentTaskHandle();

    xThingNameLength = KVStore_getValueLength( KVS_CORE_THING_NAME );

    if( ( xThingNameLength > 0 ) && ( xThingNameLength <= SHADOW_THINGNAME_MAX_LENGTH ) )
    {
        memset( cThingName, 0x00, sizeof( cThingName ) );
        ( void ) KVStore_getString( KVS_CORE_THING_NAME, cThingName, sizeof( cThingName ) );
        xStatus = true;
    }
    else
    {
        LogError( ( "Failed to get thing name from KV store, thing name length received = %u, "
                    "max thing name length supported = %u", xThingNameLength, SHADOW_THINGNAME_MAX_LENGTH ) );
        xStatus = false;
    }

    if( xStatus == true )
    {
        xStatus = prvCreateShadowTopics( cThingName, xThingNameLength );
    }

    if( xStatus == true )
    {
        if( xGetMQTTAgentState() != MQTT_AGENT_STATE_CONNECTED )
        {
            ( void ) xWaitForMQTTAgentState( MQTT_AGENT_STATE_CONNECTED, portMAX_DELAY );
        }

        LogInfo( ( "MQTT Agent is connected. Initializing shadow update task." ) );
    }

    if( xStatus == true )
    {
        /* Subscribe to Shadow topics. */
        xStatus = prvSubscribeToShadowUpdateTopics();
    }

    if( xStatus == true )
    {
        /* Set up MQTTPublishInfo_t for the desired updates. */
        xPublishInfo.qos = MQTTQoS1;
        xPublishInfo.pTopicName = cUpdateTopic;
        xPublishInfo.topicNameLength = usUpdateTopicLength;
        xPublishInfo.pPayload = pcDesiredDocument;

        /* Set up the MQTTAgentCommandInfo_t for the demo loop.
         * We do not need a completion callback here since for publishes, we expect to get a
         * response on the appropriate topics for accepted or rejected reports. */
        xCommandParams.blockTimeMs = shadowexampleMAX_COMMAND_SEND_BLOCK_TIME_MS;
        xCommandParams.cmdCompleteCallback = NULL;

        for( ; ; )
        {
            bool new_touch_data = false;
            TickType_t cur_time = xTaskGetTickCount();
            static TickType_t prev_reported_time = 0;

            /* Look for new touch events, but clear queue and get the latest in the case where the reporting
               causes missed events */
            error = UEZQueueReceive(touch_queue, &touch_event, pdMS_TO_TICKS( shadowexampleMS_BETWEEN_REQUESTS ));
            if(UEZ_ERROR_NONE == error)
            {
                /* Clear out queue and get latest event in case it takes a bit to get through the thread */
                do
                {
                    error = UEZQueueReceive(touch_queue, &touch_event, UEZ_TIMEOUT_NONE);
                    /* Got new touch event! */
                    if((touch_event.iType == INPUT_EVENT_TYPE_XY) 
                        && ((touch_event.iEvent.iXY.iAction == XY_ACTION_PRESS_AND_HOLD) // If being pressed 
                            || (touch_event.iEvent.iXY.iAction != prev_touch_event.iEvent.iXY.iAction))) // If just released
                    {
                        /* Save away for next report */
                        memcpy(&prev_touch_event, &touch_event, sizeof(T_uezInputEvent));
                        memset(&touch_event, 0, sizeof(T_uezInputEvent));

                        new_touch_data = true;
                    }
                } while(UEZ_ERROR_NONE == error);
            }

            /* If we got new touch data or the report period has elapsed, send a new report */
            if((true == new_touch_data) 
              || ((cur_time - prev_reported_time) > pdMS_TO_TICKS( shadowexampleMS_BETWEEN_REQUESTS )))
            {
                /* Create a new client token and save it for use in the update accepted and rejected callbacks. */
                ulClientToken = (uint32_t )( xTaskGetTickCount() % 1000000 ); // this ensures it stays under 6 digits...
                       
                /* Read and calculate temperature */
                for(tries = 0; tries < 100; tries++) {
                    error = UEZTemperatureRead(temperature, &temp_raw);
                    if(error == UEZ_ERROR_NONE) {
                      break;
                    }
                }

                if (error == UEZ_ERROR_NONE) 
                {
                    temp_int = temp_raw >> 16;
                    temp_frac = ((((TUInt32)temp_raw) & 0xFFFF) * 10) >> 16;  // Convert to 1 digit decimal                
                    //sprintf(number, "%3d.%01d_C", temp_int, temp_frac); // currentTemperatureC
                } 
                else
                { // error
                    temp_int = -99;
                    temp_frac = 9; //currentTemperatureC
                    LogInfo( ( "Failed to get temperature!" ) );
                }

                /* Read and calculate accelerometer */
                for(tries = 0; tries < 100; tries++) {
                    error = (*G_accel0)->ReadXYZ(G_accel0, &reading, 500);
                    if(error == UEZ_ERROR_NONE) {
                      break;
                    }
                }

                if (error == UEZ_ERROR_NONE) 
                {
                    xI = reading.iX >> 16;                
                    xF = ((((TUInt32)reading.iX) & 0xFFFF) * 10) >> 16;  // Convert to 1 digit decimal
                    yI = reading.iY >> 16;                
                    yF = ((((TUInt32)reading.iY) & 0xFFFF) * 10) >> 16;  // Convert to 1 digit decimal
                    zI = reading.iZ >> 16;                
                    zF = ((((TUInt32)reading.iZ) & 0xFFFF) * 10) >> 16;  // Convert to 1 digit decimal
                } 
                else 
                { // error
                    xI = -99; xF = 9; yI = -99; yF = 9; zI = -99; zF = 9;
                    LogInfo( ( "Failed to get accel!" ) );
                }

                /* Read and calculate voltage monitor */
                vmon_raw = UEZPlatform_5V_Monitor_Get_Raw_Reading();
                vmon = (float)((float) ((float)vmon_raw / (float) 1023) * (float) 3.3 * (float)((float)5.0 / (float)2.5));

                vTaskDelay(1);
                /* Generate update report. */
                ( void ) memset( pcDesiredDocument,
                                 0x00,
                                 sizeof( pcDesiredDocument ) );

                xPublishInfo.payloadLength = snprintf( pcDesiredDocument,
                          shadowexampleSHADOW_DESIRED_JSON_LENGTH,
                          shadowexampleSHADOW_DESIRED_JSON,
                          ( uint32_t ) ucDeviceState,
                          temp_int, temp_frac, //( float ) currentTemperatureC,
                          xI, xF, yI, yF, zI, zF, // accelerometer
                          (double)vmon, // voltage monitor
                          ( int32_t ) prev_touch_event.iEvent.iXY.iX, ( int32_t ) prev_touch_event.iEvent.iXY.iY, // last touch coordinates
                          ( uint32_t ) ulClientToken );


                snprintf(G_CurrentShadowReportTemp, 12, "%3d.%01d C\n", temp_int, temp_frac);
                //snprintf(G_CurrentShadowReportAcc, 40, "X: %3d.%01d,Y: %3d.%01d,Z: %3d.%01d\n", xI, xF, yI, yF, zI, zF);
                snprintf(G_CurrentShadowReportAcc, 40, "%3d.%01d/%3d.%01d/%3d.%01d\n", xI, xF, yI, yF, zI, zF);
                snprintf(G_CurrentShadowReport5V, 10, "%5.1f V\n", (double) vmon);
                snprintf(G_CurrentShadowReportTouch, 20, "X: %4d, Y: %4d\n", (TUInt16) prev_touch_event.iEvent.iXY.iX, (TUInt16) prev_touch_event.iEvent.iXY.iY);
                snprintf(G_CurrentShadowReportClientT, 12, "%5u\n",( uint32_t ) ulClientToken);


                if( xPublishInfo.payloadLength > ( shadowexampleSHADOW_DESIRED_JSON_LENGTH ) )
                {
                    LogError( ("Buffer too small, shadowexampleSHADOW_DESIRED_JSON_LENGTH = %u but payloadLength = %u"), 
                            shadowexampleSHADOW_DESIRED_JSON_LENGTH, 
                            xPublishInfo.payloadLength );
                }

                configASSERT( xPublishInfo.payloadLength < ( shadowexampleSHADOW_DESIRED_JSON_LENGTH ) );

                /* Send desired state. */
                LogInfo( ( "Publishing to /update with following client token %u.", ( uint32_t ) ulClientToken ) );
                LogDebug( ( "Publish content: %.*s", strlen(pcDesiredDocument), pcDesiredDocument ) );

                vTaskDelay(1);
                xCommandAdded = MQTTAgent_Publish( &xGlobalMqttAgentContext,
                                                   &xPublishInfo,
                                                   &xCommandParams );
                       
                vTaskDelay(1);
                if( xCommandAdded != MQTTSuccess )
                {
                    LogInfo( ( "Failed to publish to shadow update." ) );
                }
                else
                {
                    /* Wait for the response to our report. When the Device shadow service receives the request it will
                     * publish a response to  the /update/accepted or update/rejected */
                    ulNotificationValue = ulTaskNotifyTake( pdFALSE, pdMS_TO_TICKS( shadowexampleMS_TO_WAIT_FOR_NOTIFICATION ) );

                    if( ulNotificationValue == 0 )
                    {
                        LogError( ( "Timed out waiting for response to report." ) );
                    }

                    /* Update report time so it doesn't try to send unnecessarily quick reports */
                    prev_reported_time = cur_time;
                }

                /* Clear the client token */
                ulClientToken = 0;

                LogDebug( ( "Sleeping until touch event or time for next publish." ) );
            }
        }
    }

    vTaskDelete( NULL );
}

/*-----------------------------------------------------------*/
