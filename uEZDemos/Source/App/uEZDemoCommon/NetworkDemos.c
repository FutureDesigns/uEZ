/*-------------------------------------------------------------------------*
 * File:  NetworkStartup.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <uEZ.h>
#include <uEZPlatform.h>
#include "NetworkStartup.h"
#include "NetworkDemos.h"
#include <NVSettings.h>
#include <Config_Build.h>
#include <Source/Library/lwip_2.0.x/system/arch/sntp_uEZ.h>
#include <Source/Library/lwip_2.0.x/contrib/examples/snmp/snmp_example.h>
#include <Source/Library/lwip_2.0.x/contrib/examples/tftp/tftp_example.h>
#include <Source/Library/lwip_2.0.x/contrib/examples/lwiperf/lwiperf_example.h>
#include <sys/socket.h>

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Types:
 *-------------------------------------------------------------------------*/

#if (UEZ_ENABLE_TCPIP_STACK == 1)
extern T_uezTask G_lwipTask;

static TBool need_updated_server_time = EFalse; // create a user button to set to this to true, or set true only after on RTC battery drain.

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*
 * Common NetworkStartup:
 *-------------------------------------------------------------------------*/
T_uezError Start_lwIP_Network_Demos(T_uezDevice network)
{
    if(G_lwipTask != NULL) {
      lwip_socket_thread_init(); // if lwip init make sure to init per thread objects
    } // This would be needed to run below mqtt demo, iperf demo, etc unelss they start their own threads and init the objects.

    // Prefereably use DHCP time to avoid having uEZGUI spam the time server on every bootup. // Shown below is specifying a name server manually.
    // If the server is spammed it may send a KOD and stop responding.
    if(need_updated_server_time == ETrue) {
        sntp_register_named_server(1, "pool.ntp.org"); // second server entry, in case DHCP populates entry 0.
        UEZTaskDelay(15000);
        stop_sntp_server();
        need_updated_server_time = EFalse;
    }
    
    // run an lwIP example?

    // Can test using snmptwalk as below by substituting IP address. See snmpv3_dummy.c and related files.
    // snmpwalk -v3  -l authPriv -u lwip -a SHA -A "maplesyrup"  -x DES -X "maplesyrup" ipv4address
    //snmp_example_init();

    //tftp_example_init_client(); // not tested yet
    //tftp_example_init_server();  // test using atftp software to put files onto SD card.

    //lwiperf_example_init(); // can run iperf from PC to uEZGUI

    //mqtt_example_init(); // must provide some parameters to run this, see the AWS demo for TLS Shadow MQTT demo which is a more robust demo

    //NetworkThroughputServerTask(); // TODO this hasn't been tested in many years

    return UEZ_ERROR_NONE;

}

T_uezError Start_FreeRtosTCP_Network_Demos(T_uezDevice network)
{
  // TODO first we need to connect FreeRTOS+TCP to ethernet or wifi driver.
  return UEZ_ERROR_NONE;
}

T_uezError Start_MQTT_Demo(T_uezDevice network)
{
  // Do we want a separate general purpose MQTT demo? Local hosted with eclipse broker https://mosquitto.org/ ?
  return UEZ_ERROR_NONE;
}

#endif
/*-------------------------------------------------------------------------*
 * End of File:  NetworkStartup.c
 *-------------------------------------------------------------------------*/
