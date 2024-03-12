/*
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Dirk Ziegelmeier <dziegel@gmx.de>
 *
 */

#include <stdio.h>

#include "lwip/apps/tftp_client.h"
#include "lwip/apps/tftp_server.h"
#include "tftp_example.h"
#include <uEZFile.h>

#include <string.h>

#if LWIP_UDP

#if 1 // TODO test

/* Define a base directory for TFTP access
 * ATTENTION: This code does NOT check for sandboxing,
 * i.e. '..' in paths is not checked! */
#ifndef LWIP_TFTP_EXAMPLE_BASE_DIR
//#define LWIP_TFTP_EXAMPLE_BASE_DIR "0:" // USB Drive
#define LWIP_TFTP_EXAMPLE_BASE_DIR "1:" // SD card
#endif

/* Define this to a file to get via tftp client */
#ifndef LWIP_TFTP_EXAMPLE_CLIENT_FILENAME
#define LWIP_TFTP_EXAMPLE_CLIENT_FILENAME "test.bin"
#endif

/* Define this to a server IP string */
#ifndef LWIP_TFTP_EXAMPLE_CLIENT_REMOTEIP
#define LWIP_TFTP_EXAMPLE_CLIENT_REMOTEIP "192.168.0.1"
#endif

static char full_filename[128];
static T_uezFile G_ftpFileHandle;

static void *
tftp_open_file(const char* fname, u8_t is_write)
{
  snprintf(full_filename, sizeof(full_filename), "%s%s", LWIP_TFTP_EXAMPLE_BASE_DIR, fname);
  full_filename[sizeof(full_filename)-1] = 0;

  if (is_write) {
    UEZFileOpen(full_filename, FILE_FLAG_WRITE, &G_ftpFileHandle);
    return (void *) G_ftpFileHandle; //return (void*)fopen(full_filename, "wb");
  } else {
    UEZFileOpen(full_filename, FILE_FLAG_READ_ONLY, &G_ftpFileHandle);
    return (void *) G_ftpFileHandle; //return (void*)fopen(full_filename, "rb");
  }
}

static void*
tftp_open(const char* fname, const char* mode, u8_t is_write)
{
  LWIP_UNUSED_ARG(mode);
  return tftp_open_file(fname, is_write);
}

static void
tftp_close(void* handle)
{
  UEZFileClose((T_uezFile) handle);//fclose((FILE*)handle);
}

static int
tftp_read(void* handle, void* buf, int bytes)
{
  TUInt32 aNumBytesRead;
  UEZFileRead((T_uezFile)handle, buf, bytes, &aNumBytesRead);
  int ret = aNumBytesRead; //fread(buf, 1, bytes, (FILE*)handle);  
  if (ret <= 0) {
    return -1;
  }
  return ret;
}

static int
tftp_write(void* handle, struct pbuf* p)
{
  TUInt32 aNumBytesWritten;
  while (p != NULL) {
    UEZFileWrite((T_uezFile)handle, p->payload, p->len, &aNumBytesWritten);
    if ( //fwrite(p->payload, 1, p->len, (FILE*)handle)
        aNumBytesWritten != (size_t)p->len
       ) {
      return -1;
    }
    p = p->next;
  }

  return 0;
}

/* For TFTP client only */
static void
tftp_error(void* handle, int err, const char* msg, int size)
{
  char message[100];

  LWIP_UNUSED_ARG(handle);

  memset(message, 0, sizeof(message));
  MEMCPY(message, msg, LWIP_MIN(sizeof(message)-1, (size_t)size));

  printf("TFTP error: %d (%s)", err, message);
}

static const struct tftp_context tftp = {
  tftp_open,
  tftp_close,
  tftp_read,
  tftp_write,
  tftp_error
};

void
tftp_example_init_server(void)
{
  LOCK_TCPIP_CORE();
  tftp_init_server(&tftp);
  UNLOCK_TCPIP_CORE();
}

void
tftp_example_init_client(void)
{
  void *f;
  err_t err;
  ip_addr_t srv;
  LWIP_UNUSED_ARG(err);
  int ret = ipaddr_aton(LWIP_TFTP_EXAMPLE_CLIENT_REMOTEIP, &srv);
  LWIP_ASSERT("ipaddr_aton failed", ret == 1);

  LOCK_TCPIP_CORE();
  err = tftp_init_client(&tftp);
  UNLOCK_TCPIP_CORE();
  LWIP_ASSERT("tftp_init_client failed", err == ERR_OK);

  f = tftp_open_file(LWIP_TFTP_EXAMPLE_CLIENT_FILENAME, 1);
  LWIP_ASSERT("failed to create file", f != NULL);

  err = tftp_get(f, &srv, TFTP_PORT, LWIP_TFTP_EXAMPLE_CLIENT_FILENAME, TFTP_MODE_OCTET);
  LWIP_ASSERT("tftp_get failed", err == ERR_OK);
  LWIP_UNUSED_ARG(ret);
}
#endif

#endif /* LWIP_UDP */
