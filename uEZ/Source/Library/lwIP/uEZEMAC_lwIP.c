/*-------------------------------------------------------------------------*
 * File:  uEZEMAC.c
 *-------------------------------------------------------------------------*
 * Description:
 *      UEZlex EMAC Support API.
 *
 * Implementation:
 *      Most all of this is callbacks between the lwIP TCP/IP stack
 *      and the HAL layer.  We use FreeRTOS types for compatibility.
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2015 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZ License.pdf or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(r) to your own hardware!   |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <HAL/HAL.h>
#include <HAL/EMAC.h>
#include <Types/EMAC.h>
#if (RTOS == FreeRTOS)
#include <FreeRTOS.h> // for types
#elif (RTOS == SafeRTOS)
#include <SafeRTOS.h>
#endif

#if 1 || UEZ_ENABLE_TCPIP_STACK
/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
static HAL_EMAC *G_emac;
static T_halWorkspace *G_emacWS;

portBASE_TYPE Init_EMAC(unsigned short PHYType)
{
    T_uezError error;
    extern T_uezError SetupEMAC(void);

    // Find the EMAC HAL and keep it around
    error = HALInterfaceFind("EMAC", (T_halWorkspace **)&G_emacWS);
    if (error != UEZ_ERROR_NONE) {
        G_emac = 0;
        G_emacWS = 0;
        return pdFAIL;
    }
    G_emac = (HAL_EMAC *)(G_emacWS->iInterface);

    // USe the NV settings to configure the EMAC
    if (SetupEMAC() != UEZ_ERROR_NONE)
        return pdFAIL;

    return pdPASS;
}

// TBD: Add header blocks
unsigned short StartReadFrame(void)
{
    return G_emac->StartReadFrame(G_emacWS);
}

portLONG lEMACSend( portCHAR *pcFrom, unsigned portLONG ulLength, portLONG lEndOfFrame )
{
    G_emac->RequestSend(G_emacWS);
    /* Copy the header into the Tx buffer. */
    G_emac->CopyToFrame(G_emacWS, pcFrom, ulLength);
    G_emac->DoSend(G_emacWS, ulLength);
    return 1;
}

unsigned int CheckFrameReceived(void)
{             // Packet received ?
    return G_emac->CheckFrameReceived(G_emacWS);
}

void CopyFromFrame_EMAC(void *Dest, unsigned short Size)
{
    G_emac->CopyFromFrame(G_emacWS, Dest, Size);
}

void EndReadFrame(void)
{
    G_emac->EndReadFrame(G_emacWS);
}

/*---------------------------------------------------------------------------*
 * Routine:  SetupEMAC
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the EMAC with the NV Settings.
 *---------------------------------------------------------------------------*/
T_EMACSettings UEZ_EMAC_Settings = {
    0, // type
    0, // address 0 = Micrel

    // Default Mac address
    {   0x00, 0xBD, 0x33, 0x06, 0x68, 0x22}
};

/*---------------------------------------------------------------------------*
 * Routine:  SetupEMAC
 *---------------------------------------------------------------------------*
 * Description:
 *      When the network goes active, SetupEMAC is called.  This routine
 *      initializes the EMAC.
 *---------------------------------------------------------------------------*/
T_uezError SetupEMAC(void)
{
    T_uezError error;
    HAL_EMAC **emac;
    extern void *UEZEMACGetMACAddr(TUInt32 aUnitNumber);

//    printf("Setting up EMAC ... ");

    // Copy over the emac settings
    memcpy(UEZ_EMAC_Settings.iMACAddress, UEZEMACGetMACAddr(0), 6);

    // Setup and configure the EMAC
    error = HALInterfaceFind("EMAC", (T_halWorkspace **)&emac);
    if (!error)
        error = (*emac)->Configure(emac, &UEZ_EMAC_Settings);
//    if (error)
//        printf("Error! (code %d)\n", error);
//    else
//        printf("OK\n");

    return error;
}


#else // !UEZ_ENABLE_TCPIP_STACK
portBASE_TYPE Init_EMAC(unsigned short PHYType)
{
    return pdFAIL;
}
unsigned short StartReadFrame(void)
{
    return pdFAIL;
}
portLONG lEMACSend( portCHAR *pcFrom, unsigned portLONG ulLength, portLONG lEndOfFrame )
{
    return 0;
}
unsigned int CheckFrameReceived(void)
{
    return 0;
}
void CopyFromFrame_EMAC(void *Dest, unsigned short Size)
{
}
void EndReadFrame(void)
{
}
#endif // UEZ_ENABLE_TCPIP_STACK


#include "netif/slipif.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"
#include "lwip/sio.h"

#ifndef sio_open
sio_fd_t sio_open(u8_t a)
{
    return 0;
}
#endif

#ifndef sio_send
void sio_send(u8_t a, sio_fd_t b)
{
}
#endif

#ifndef sio_recv
u8_t sio_recv(sio_fd_t c)
{
    return 0;
}
#endif

/*-------------------------------------------------------------------------*
 * End of File:  uEZTS.c
 *-------------------------------------------------------------------------*/
