/*
 * FreeModbus Libary: lwIP Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portevent.c,v 1.1 2006/08/30 23:18:07 wolti Exp $
 */

#include <string.h>

/* ----------------------- System includes ----------------------------------*/
#include "assert.h"

/* ----------------------- lwIP ---------------------------------------------*/
#include "lwip/api.h"
#include "lwip/sys.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_POLL_CYCLETIME       100     /* Poll cycle time is 100ms */

typedef struct {
  eMBEventType  event;
  void          *session;
  USHORT         len;
  UCHAR          data[1];
} xMBPortEvent;

/* ----------------------- Static variables ---------------------------------*/
static sys_mbox_t xMailBox = SYS_MBOX_NULL;
static eMBEventType eMailBoxEvent;

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortEventInit( void )
{
    eMailBoxEvent = EV_READY;
#if LWIP_2_0_x
    sys_mbox_new(&xMailBox, 0 );
#else
    xMailBox = sys_mbox_new( 0 );
#endif
    return xMailBox != SYS_MBOX_NULL ? TRUE : FALSE;
}

void
vMBPortEventClose( void )
{
    if( xMailBox != SYS_MBOX_NULL )
    {
#if LWIP_2_0_x
        sys_mbox_free( &xMailBox );
#else
        sys_mbox_free( xMailBox );
#endif
    }
}

#if NEW_PORT_EVENT == 0
BOOL
xMBPortEventPost( eMBEventType eEvent )
{
    eMailBoxEvent = eEvent;
    sys_mbox_post( xMailBox, &eMailBoxEvent );
    return TRUE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent )
{
    void           *peMailBoxEvent;
    BOOL            xEventHappend = FALSE;
    u32_t           uiTimeSpent;

    uiTimeSpent = sys_arch_mbox_fetch( xMailBox, &peMailBoxEvent, MB_POLL_CYCLETIME );
    if( uiTimeSpent != SYS_ARCH_TIMEOUT )
    {
        *eEvent = *( eMBEventType * ) peMailBoxEvent;
        eMailBoxEvent = EV_READY;
        xEventHappend = TRUE;
    }
    return xEventHappend;
}
#else
BOOL
xMBPortEventPost( eMBEventType eEvent, void *session, USHORT len, UCHAR *data )
{
    xMBPortEvent *postEvent;

    postEvent = malloc(sizeof(xMBPortEvent) + len);
    
    if (postEvent)
    {
      postEvent->event = eEvent;
      postEvent->session = session;
      postEvent->len = len;
      memcpy(&postEvent->data[0], data, len);
#if LWIP_2_0_x
      sys_mbox_post( &xMailBox, postEvent );
#else
      sys_mbox_post( xMailBox, postEvent );
#endif
      return TRUE;
    }
    
    return FALSE;
}

BOOL
xMBPortEventGet( eMBEventType * eEvent, void **session, USHORT *len, UCHAR *data )
{
    xMBPortEvent    *peGetEvent;
    BOOL            xEventHappend = FALSE;
    u32_t           uiTimeSpent;

#if LWIP_2_0_x
    uiTimeSpent = sys_arch_mbox_fetch( &xMailBox, (void *)&peGetEvent, MB_POLL_CYCLETIME );
#else
    uiTimeSpent = sys_arch_mbox_fetch( xMailBox, (void *)&peGetEvent, MB_POLL_CYCLETIME );
#endif
    if( uiTimeSpent != SYS_ARCH_TIMEOUT )
    {
        *eEvent = peGetEvent->event;
        *session = peGetEvent->session;
        *len = peGetEvent->len;
        memcpy(data, &peGetEvent->data[0], peGetEvent->len);
        xEventHappend = TRUE;
        free(peGetEvent);
    }
    return xEventHappend;
}
#endif // NEW_PORT_EVENT
