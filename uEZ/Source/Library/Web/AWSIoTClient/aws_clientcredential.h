/*
 * FreeRTOS V202203.00
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
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

#ifndef __AWS_CLIENTCREDENTIAL__H__
#define __AWS_CLIENTCREDENTIAL__H__

/* @TEST_ANCHOR */

/*
 * @brief MQTT Broker endpoint.
 *
 * @todo Set this to the fully-qualified DNS name of your MQTT broker.
 */
#ifndef clientcredentialMQTT_BROKER_ENDPOINT
#define clientcredentialMQTT_BROKER_ENDPOINT "a3w2mg5b2fkick-ats.iot.us-east-2.amazonaws.com"
#endif

/*
 * @brief Host name.
 *
 * @todo Set this to the unique name of your IoT Thing.
 * Please note that for convenience of demonstration only we
 * are using a #define here. In production scenarios the thing
 * name can be something unique to the device that can be read
 * by software, such as a production serial number, rather
 * than a hard coded constant.
 * For the AWS SDK demo this should match your --client_id parameter
 */

// This must match the   "Resource": [   :client/NAME line in the policy section of AWS portal.
// A wildcard can be used for example to allow all uEZGUI product names.
// For example copy this into policy version: arn:aws:iot:us-east-2:NUMBER:client/uEZGUI-*

#ifndef clientcredentialIOT_THING_NAME
//#define clientcredentialIOT_THING_NAME "sdk-nodejs-v2" // if shadow stack not setup in AWS portal
#define clientcredentialIOT_THING_NAME "uEZGUI-4357-50WVN" // only if shadow stack created in AWS portal.
//#define clientcredentialIOT_THING_NAME "uEZGUI-4357-70WVN" // only if shadow stack created in AWS portal.
#endif

/*
 * @brief Port number the MQTT broker is using.
 */
#ifndef clientcredentialMQTT_BROKER_PORT
#define clientcredentialMQTT_BROKER_PORT 8883
#endif

/*
 * @brief Port number the Green Grass Discovery use for JSON retrieval from cloud is using.
 */
#ifndef clientcredentialGREENGRASS_DISCOVERY_PORT
#define clientcredentialGREENGRASS_DISCOVERY_PORT 8443
#endif

/*
 * @brief Wi-Fi network to join.
 *
 * @todo If you are using Wi-Fi, set this to your network name.
 */
#ifndef clientcredentialWIFI_SSID
#define clientcredentialWIFI_SSID "iot_2.4"
#endif

/*
 * @brief Password needed to join Wi-Fi network.
 * @todo If you are using WPA, set this to your network password.
 */
#ifndef clientcredentialWIFI_PASSWORD
#define clientcredentialWIFI_PASSWORD "fdi_cloud"
#endif

/*
 * @brief Wi-Fi network security type.
 *
 * @see WIFISecurity_t.
 *
 * @note Possible values are eWiFiSecurityOpen, eWiFiSecurityWEP, eWiFiSecurityWPA,
 * eWiFiSecurityWPA2 (depending on the support of your device Wi-Fi radio).
 */
#ifndef clientcredentialWIFI_SECURITY
#define clientcredentialWIFI_SECURITY eWiFiSecurityWPA2
#endif

#endif /* ifndef __AWS_CLIENTCREDENTIAL__H__ */
