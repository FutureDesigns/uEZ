/*
 * FreeRTOS V202111.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright 2022 NXP
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

/**
 * @file tls_transport_mbedtls_pkcs11_FreeRTOS.c
 * @brief TLS transport interface implementations. This implementation uses
 * mbedTLS.
 */
 
// from transport_mbedtls_pkcs11.h AWS example

/**************************************************/
/******* DO NOT CHANGE the following order ********/
/**************************************************/
#include <Source/Library/SEGGER/RTT/SEGGER_RTT.h>
/* Logging related header files are required to be included in the following order:
 * 1. Include the header file "logging_levels.h".
 * 2. Define LIBRARY_LOG_NAME and  LIBRARY_LOG_LEVEL.
 * 3. Include the header file "logging_stack.h".
 */

/* Include header that defines log levels. */

#include "Source/RTOS/FreeRTOS-Plus/Utilities/logging/logging_levels.h"

/* Logging configuration for the Sockets. */
#ifndef LIBRARY_LOG_NAME
#define LIBRARY_LOG_NAME "PkcsTlsTransport"
#endif
#ifndef LIBRARY_LOG_LEVEL
#define LIBRARY_LOG_LEVEL LOG_ERROR// LOG_INFO
#endif

#include "Source/Library/Web/AWSIoTClient/aws_iot/iot-reference/examples/common/logging/logging.h"

/*-----------------------------------------------------------*/
//#include <uEZ.h>
#include "uEZNetwork.h"

#define MBEDTLS_ALLOW_PRIVATE_ACCESS

#include "mbedtls/private_access.h"

/* Standard includes. */
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"

/* MbedTLS Bio TCP sockets wrapper include. */
#include "mbedtls_bio_tcp_sockets_wrapper.h" // TODO use this to switch between Wi-Fi/Cellualr/Ethernet, Lwip/FreeRTOSTCP usign the same function names.

/* TLS transport header. */
//#include "transport_mbedtls_pkcs11.h" // version for FreeRTOS_TCP
#include "tls_transport_mbedtls_pkcs11_FreeRTOS.h" // version for lwip (our customized version with uEZ includes)
#include "Source/RTOS/FreeRTOS-Plus/Application-Protocols/network_transport/mbedtls_pkcs11.h"


/* PKCS #11 includes. */
//#include "Source/Library/Web/AWSIoTClient/config_files/core_pkcs11_config.h" // This config is old from old FreeRTOS version.
#include "core_pkcs11_config.h" // Use this one from the newer default config, and to avoid using 2 different versions.

#include "../../../corePKCS11/source/include/core_pkcs11.h" //#include "Source/RTOS/FreeRTOS-Plus/corePKCS11/source/include/core_pkcs11.h"
#include "../../../corePKCS11/source/dependency/3rdparty/pkcs11/pkcs11.h" //#include "Source/RTOS/FreeRTOS-Plus/corePKCS11/source/dependency/3rdparty/pkcs11/pkcs11.h"
#include "../../../corePKCS11/source/include/core_pki_utils.h" //#include "Source/RTOS/FreeRTOS-Plus/corePKCS11/source/include/core_pki_utils.h"

/*-----------------------------------------------------------*/

/**
 * @brief Each compilation unit that consumes the NetworkContext must define it.
 * It should contain a single pointer as seen below whenever the header file
 * of this transport implementation is included to your project.
 *
 * @note When using multiple transports in the same compilation unit,
 *       define this pointer as void *.
 */
struct NetworkContext
{
    TlsTransportParams_t * pParams;
};

/*-----------------------------------------------------------*/
static uint32_t s_receiveTimeoutMs;


/*-----------------------------------------------------------*/
int mbedtls_bio_socket_send(void *ctx, const unsigned char *buf, size_t len)
{
//TCP_Sockets_Send

    T_uezError error = UEZ_ERROR_NONE;
    int ret = 0;
    NetworkContext_t * pNetworkContext = (NetworkContext_t *)ctx;
    TlsTransportParams_t * pTlsTransportParams = (TlsTransportParams_t *) &pNetworkContext->pParams;

    configASSERT(buf != NULL);

    error = UEZNetworkSocketWrite(pTlsTransportParams->aNetwork, pTlsTransportParams->tcpSocket, (unsigned char *) buf, len, EFalse, 10000);
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "\nFailed mbedtls_bio_socket_send::UEZNetworkSocketWrite, error=%d, length %u\n", error, len);
    }
    else
    {
        ret = len;
        //DEBUG_RTT_Printf(0, "\mbedtls_bio_socket_send ok length %u\n", len);
    }

    return ret;
}

/*-----------------------------------------------------------*/

int mbedtls_bio_socket_recv(void *ctx, unsigned char *buf, size_t len)
{
//TCP_Sockets_Recv
    T_uezError error = UEZ_ERROR_NONE;
    int ret = 0;
    TUInt32 bytes_read = 0;
    NetworkContext_t * pNetworkContext = (NetworkContext_t *)ctx;
    TlsTransportParams_t * pTlsTransportParams =  (TlsTransportParams_t *) &pNetworkContext->pParams;


    configASSERT(buf != NULL);

    error = UEZNetworkSocketRead(pTlsTransportParams->aNetwork, pTlsTransportParams->tcpSocket, buf, len, &bytes_read, s_receiveTimeoutMs);
    if(UEZ_ERROR_NONE != error)
    {
        DEBUG_RTT_Printf(0, "\nFailed mbedtls_bio_socket_recv::UEZNetworkSocketRead, error=%d\n", error);
        if(UEZ_ERROR_TIMEOUT == error)
        {
            ret = MBEDTLS_ERR_SSL_WANT_READ;
        }
    }
    else
    {
        ret = bytes_read;
        //DEBUG_RTT_Printf(0, "\mbedtls_bio_socket_recv ok\n");
    }    

    return ret;
}

/*-----------------------------------------------------------*/

BaseType_t mbedtls_bio_socket_connect(TlsTransportParams_t * pTlsTransportParams,
    const char *pHostName, uint16_t port, uint32_t receiveTimeoutMs, uint32_t sendTimeoutMs)
{

// TCP_Sockets_Connect

    T_uezError          error   = UEZ_ERROR_NONE;
    T_uezNetworkSocket  socket  = 0;
    T_uezNetworkAddr    address = { 0 };
   // TlsTransportParams_t * pTlsTransportParams = pNetworkContext->pParams;


    (void)sendTimeoutMs;
    s_receiveTimeoutMs = receiveTimeoutMs;

    error = UEZNetworkSocketCreate(pTlsTransportParams->aNetwork, UEZ_NETWORK_SOCKET_TYPE_TCP, &socket);
    if(UEZ_ERROR_NONE != error)
    {
        LogError(("Failed to create TCP socket with error %d.", error));
    }
    
    if(UEZ_ERROR_NONE == error)
    {
        error = UEZNetworkResolveAddress(pTlsTransportParams->aNetwork, pHostName, &address);
        if(UEZ_ERROR_NONE != error)
        {
            LogError(("Failed to resolve IP address of %s with error %d.", pHostName, error));
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZNetworkSocketBind(pTlsTransportParams->aNetwork, socket, 0, 0); // bind to current address/interface
        if(UEZ_ERROR_NONE != error)
        {
            LogError("Failed to bind TCP socket with error %d.", error);
        }
    }

    if(UEZ_ERROR_NONE == error)
    {
        error = UEZNetworkSocketConnect(pTlsTransportParams->aNetwork, socket, &address, port);
        if(UEZ_ERROR_NONE != error)
        {
            LogError(("Failed to establish TCP connection to %s (%d.%d.%d.%d), with error %d.", pHostName, 
                    address.v4[0], address.v4[1], address.v4[2], address.v4[3], error));
        }
    }

    if(UEZ_ERROR_NONE != error)
    {
        if (socket != 0)
        {
            UEZNetworkSocketDelete(pTlsTransportParams->aNetwork, socket);
        }
    }
    else
    {
        pTlsTransportParams->tcpSocket = socket;
    }

    if (error == UEZ_ERROR_NONE) {
        return 0;
    } else {
        return 1;
    }
}

//TCP_Sockets_Disconnect

BaseType_t mbedtls_bio_socket_disconnect( TlsTransportParams_t * pTlsTransportParams )
{
    BaseType_t tlsStatus = 0;
    //DEBUG_RTT_Printf(0, "TLS_FreeRTOS_Disconnect Socket close and delete called.\n");
    /* Call socket shutdown function to close connection. */
    UEZNetworkSocketClose(pTlsTransportParams->aNetwork, pTlsTransportParams->tcpSocket);
    UEZNetworkSocketDelete(pTlsTransportParams->aNetwork, pTlsTransportParams->tcpSocket);
    pTlsTransportParams->tcpSocket = 0;
    return tlsStatus;
}


BaseType_t mbedtls_bio_socket_delete( TlsTransportParams_t * pTlsTransportParams )
{
    //TCP_Sockets_Disconnect( pTlsTransportParams->tcpSocket );
    BaseType_t tlsStatus = 0;
    //DEBUG_RTT_Printf(0, "TLS_FreeRTOS_Disconnect Socket delete called.\n");
    UEZNetworkSocketClose(pTlsTransportParams->aNetwork, pTlsTransportParams->tcpSocket);
    UEZNetworkSocketDelete(pTlsTransportParams->aNetwork, pTlsTransportParams->tcpSocket);
    pTlsTransportParams->tcpSocket = 0;
    return tlsStatus;
}

#if 0
static int32_t privateKeySigningCallback(void *pvContext,
                                         mbedtls_md_type_t xMdAlg,
                                         const unsigned char *pucHash,
                                         size_t xHashLen,
                                         unsigned char *pucSig,
                                         size_t *pxSigLen,
                                         int32_t (*piRng)(void *, unsigned char *, size_t),
                                         void *pvRng)
{
    CK_RV xResult              = CKR_OK;
    int32_t lFinalResult       = 0;
    mbedtls_pk_context *pctx = (mbedtls_pk_context *)pvContext;
    SSLContext_t *pxTLSContext = (SSLContext_t *) pctx->pk_ctx;
    CK_MECHANISM xMech         = {0};
    CK_ULONG xToBeSignedLen = sizeof(xToBeSigned);

    /* Unreferenced parameters. */
    (void)(piRng);
    (void)(pvRng);
    (void)(xMdAlg);

    /* Sanity check buffer length. */
    if (xHashLen > sizeof(xToBeSigned))
    {
        xResult = CKR_ARGUMENTS_BAD;
    }

    /* Format the hash data to be signed. */
    if (CKK_RSA == pxTLSContext->xKeyType)
    {
        xMech.mechanism = CKM_RSA_PKCS;

        /* mbedTLS expects hashed data without padding, but PKCS #11 C_Sign function performs a hash
         * & sign if hash algorithm is specified.  This helper function applies padding
         * indicating data was hashed with SHA-256 while still allowing pre-hashed data to
         * be provided. */
        xResult        = vAppendSHA256AlgorithmIdentifierSequence((uint8_t *)pucHash, xToBeSigned);
        xToBeSignedLen = pkcs11RSA_SIGNATURE_INPUT_LENGTH;
    }
    else if (CKK_EC == pxTLSContext->xKeyType)
    {
        xMech.mechanism = CKM_ECDSA;
        memcpy(xToBeSigned, pucHash, xHashLen);
        xToBeSignedLen = xHashLen;
    }
    else
    {
        xResult = CKR_ARGUMENTS_BAD;
    }
    
    if(pxTLSContext->pxP11FunctionList == NULL)
    {    
        __BKPT();
    }
    if (CKR_OK == xResult)
    {
        /* Use the PKCS#11 module to sign. */
        xResult = pxTLSContext->pxP11FunctionList->C_SignInit(pxTLSContext->xP11Session, &xMech,
                                                              pxTLSContext->xP11PrivateKey);
    }

    CK_ULONG ulSigLen = sizeof( xToBeSigned );
    if (CKR_OK == xResult)
    {
        xResult   = pxTLSContext->pxP11FunctionList->C_Sign((CK_SESSION_HANDLE)pxTLSContext->xP11Session, xToBeSigned,
                                                          xToBeSignedLen, pucSig, &ulSigLen);
        //*pxSigLen = ( size_t ) ulSigLen; // we are getting passed a read only variable not a pointer!
    }

    if ((xResult == CKR_OK) && (CKK_EC == pxTLSContext->xKeyType))
    {
        /* PKCS #11 for P256 returns a 64-byte signature with 32 bytes for R and 32 bytes for S.
         * This must be converted to an ASN.1 encoded array. */
        if (ulSigLen != pkcs11ECDSA_P256_SIGNATURE_LENGTH)
        {
            xResult = CKR_FUNCTION_FAILED;
        }

        if (xResult == CKR_OK)
        {
            PKI_pkcs11SignatureTombedTLSSignature(pucSig, ulSigLen);
        }
    }

    if (xResult != CKR_OK)
    {
        LogError(("Failed to sign message using PKCS #11 with error code %02X.", xResult));
    }

    return lFinalResult;
}
#endif
