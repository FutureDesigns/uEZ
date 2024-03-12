/*
 * FreeRTOS V202212.01
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
 * @file tls_transport_mbedtls_pkcs11_FreeRTOS.h
 * @brief TLS transport interface header.
 * @note This file is derived from the tls_freertos.h header file found in the mqtt
 * section of IoT Libraries source code. The file has been modified to support using
 * PKCS #11 when using TLS.
 */

#ifndef MBEDTLS_BIO_TCP_SOCKETS_WRAPPER
#define MBEDTLS_BIO_TCP_SOCKETS_WRAPPER
// from transport_mbedtls_pkcs11.h AWS example


/* TLS transport header. */
//#include "transport_mbedtls_pkcs11.h" // version for FreeRTOS_TCP
#include "tls_transport_mbedtls_pkcs11_FreeRTOS.h" // version for lwip (our customized version with uEZ includes)

// TODO
BaseType_t mbedtls_bio_socket_connect(TlsTransportParams_t * pTlsTransportParams,
    const char *pHostName, uint16_t port, uint32_t receiveTimeoutMs, uint32_t sendTimeoutMs);

BaseType_t mbedtls_bio_socket_disconnect( TlsTransportParams_t * pTlsTransportParams );
BaseType_t mbedtls_bio_socket_delete( TlsTransportParams_t * pTlsTransportParams );

/**
 * @brief Sends data over wifi serial sockets.
 *
 * @param[in] ctx The network context containing the socket handle.
 * @param[in] buf Buffer containing the bytes to send.
 * @param[in] len Number of bytes to send from the buffer.
 *
 * @return Number of bytes sent on success; else a negative value.
 */
int mbedtls_bio_socket_send(void *ctx, const unsigned char *buf, size_t len);

/**
 * @brief Receives data from wifi serial socket.
 *
 * @param[in] ctx The network context containing the socket handle.
 * @param[out] buf Buffer to receive bytes into.
 * @param[in] len Number of bytes to receive from the network.
 *
 * @return Number of bytes received if successful; Negative value on error.
 */
int mbedtls_bio_socket_recv(void *ctx, unsigned char *buf, size_t len);

#endif /* ifndef MBEDTLS_BIO_TCP_SOCKETS_WRAPPER */
