/*-------------------------------------------------------------------------*
 * File:  Redpine_RS9110_N_11_22_driver.c
 *-------------------------------------------------------------------------*
 * Description:
 *      Device implementation of the Network for the PCA9551
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2010 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(tm) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/
#include "includes.h"
#include <string.h>
#include <stdio.h>
#include <uEZ.h>
#include <uEZDeviceTable.h>
#include <uEZGPIO.h>
#include "Redpine_RS9110_N_11_22.h"
#include "rsi_nic.h"
#include "rsi_api.h"
#include "rsi_spi.h"

/*---------------------------------------------------------------------------*
 * Macros:
 *---------------------------------------------------------------------------*/
#if 1
#define dprintf printf
#else
#define dprintf(...)
#endif

/*---------------------------------------------------------------------------*
 * Constants:
 *---------------------------------------------------------------------------*/
#define REDPINE_NUM_SOCKETS             REDPINE_RS9110_N_11_22_MAX_NUM_SOCKETS
#define REDPINE_LOCAL_PORT_START        0x8000
#define REDPINE_LOCAL_PORT_END          0x9000

/*---------------------------------------------------------------------------*
 * Types:
 *---------------------------------------------------------------------------*/
typedef enum {
    SOCKET_STATE_FREE, SOCKET_STATE_BUSY
} T_redpineSocketState;

typedef TUInt16 T_redpineSocketFlags;
#define SOCKET_FLAG_CREATED         (1<<0)
#define SOCKET_FLAG_CLOSED          (1<<1)
#define SOCKET_FLAG_REMOTE_CLOSED   (1<<2)

typedef struct {
    T_redpineSocketState iState;
    T_uezNetworkSocketType iType;
    TUInt16 iPort;
    Callbacks_T iCallbacks;
    networkConfig_t iConfig;
    TUInt32 iAppId;
    volatile T_redpineSocketFlags iFlags;
    TUInt16 iSourcePort;
    TUInt32 iSocketId;
    T_uezSemaphore iAcceptSem;
    T_uezSemaphore iCloseSem;
    T_uezQueue iRecvQueue;
} T_redpineSocket;

typedef struct {
    DEVICE_Network **iDevice;
    TUInt32 iNumOpen;
    T_uezSemaphore iSem;

    // SPI port and pin, and request configuration
    DEVICE_SPI_BUS **iSPI;
    SPI_Request iSPIRequest;

    // Option power pin (use 0 if none connected)
    HAL_GPIOPort **iGPIOPowerPort;
    TUInt32 iGPIOPowerPin;

    // Optional reset pin (use 0 if none connected)
    HAL_GPIOPort **iGPIOResetPort;
    TUInt32 iGPIOResetPin;

    // Interrupt port and pin
    HAL_GPIOPort **iGPIOInterruptPort;
    TUInt32 iGPIOInterruptPin;

    // Task running in the background processing
    // the state of the RS9110.
    T_uezTask iTask;
    T_uezSemaphore iTaskSem;
    volatile TBool iTaskDie;
    volatile TBool iTaskDead;

    // track the last status
    volatile RS22_STATUS iLastStatus;

    volatile T_uezNetworkScanStatus iScanStatus;
    T_uezSemaphore iScanSem;

    volatile T_uezNetworkJoinStatus iJoinStatus;
    T_uezSemaphore iJoinSem;

    // Track sockets in the network
    T_redpineSocket iSockets[REDPINE_NUM_SOCKETS + 1]; // first one is not used
    // use range of REDPINE_LOCAL_PORT_START - REDPINE_LOCAL_PORT_END
    TUInt16 iNextLocalPortNum;

    // Infrastructure settings to use when bring up
    T_uezNetworkSettings iInfrastructureSettings;
    TBool iInfrastructureStarted;
} T_Network_Redpine_RS9110_Workspace;

/*---------------------------------------------------------------------------*
 * Globals:
 *---------------------------------------------------------------------------*/
// This implementation only works with one RS9110 unit
// and so this is the global define for the device for callbacks.
// The Redpine RS library doesn't work with one more device.
static T_Network_Redpine_RS9110_Workspace *G_rs9110;

// Redpine flag
extern uint8_t auto_connect_open_ap;
ipconfig_t *ipConfig = &Adaptor.ipConfig;
networkConfig_t networkConfig;

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a redpine networking subsystem workspace.
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
static TUInt16 INextLocalPort(T_Network_Redpine_RS9110_Workspace *p)
{
    TUInt16 port;

    port = p->iNextLocalPortNum++;
    if (p->iNextLocalPortNum == REDPINE_LOCAL_PORT_END)
        p->iNextLocalPortNum = REDPINE_LOCAL_PORT_START;

    return port;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_InitializeWorkspace
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup a redpine networking subsystem workspace.
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_Redpine_RS9110_InitializeWorkspace(void *aWorkspace)
{
    T_uezError error;
    TUInt32 i;

    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    p->iNumOpen = 0;
    p->iScanStatus = UEZ_NETWORK_SCAN_STATUS_IDLE;
    p->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_IDLE;
    memset(p->iSockets, 0, sizeof(p->iSockets));
    p->iNextLocalPortNum = REDPINE_LOCAL_PORT_START;

    // Then create a semaphore to limit the number of accessors
    error = UEZSemaphoreCreateBinary(&p->iSem);

    // Initialize all socket's semaphores and queues
    for (i = 1; (i <= REDPINE_NUM_SOCKETS) && (error == UEZ_ERROR_NONE); i++) {
        error = UEZSemaphoreCreateCounting(&p->iSockets[i].iAcceptSem, 1, 0);
        if (error)
            break;
        error = UEZQueueCreate(REDPINE_RS9110_N_11_22_RECV_QUEUE_SIZE, 1,
                &p->iSockets[i].iRecvQueue);
        if (error)
            break;
        error = UEZSemaphoreCreateCounting(&p->iSockets[i].iCloseSem, 1, 0);
        if (error)
            break;
    }

    // Make a singleton out of this
    G_rs9110 = p;

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_Task
 *---------------------------------------------------------------------------*
 * Description:
 *      Process all background tasks
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
TUInt32 Network_Redpine_RS9110_Task(T_uezTask aMyTask, void *aParameters)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aParameters;
    HAL_GPIOPort **p_gpio = p->iGPIOInterruptPort;
    TUInt32 pins;
    TUInt32 lastPins=0;
    TUInt32 iStatusUpdateTime = UEZTickCounterGet();
    TUInt32 time;
    TUInt32 size;

    // Keep running until we are told to die
    while (!p->iTaskDie) {
        // Sleep a little bit
        UEZTaskDelay(1);

        // Grab the background task semaphore
        UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);

        // Check to see if we need to process anything from
        // the interrupt
        if (p_gpio) {
            (*p_gpio)->Read(p_gpio, p->iGPIOInterruptPin, &pins);
            if ((pins) && (pins != lastPins))
                rsi_external_inerrupt_handler();
            lastPins = pins;
        }

        /* Process any pending interrupts */
        if (Adaptor.pkt_received)
            p->iLastStatus = rsi_interrupt_handler(Adaptor.RcvPkt, (uint32_t*)&size);

        do {
            pins = 0;
            if (p_gpio) {
                // Interrupt line still low?
                (*p_gpio)->Read(p_gpio, p->iGPIOInterruptPin, &pins);

                // If low, more to process, do it now
                if (pins) {
                    p->iLastStatus = rsi_interrupt_handler(Adaptor.RcvPkt,
                            (uint32_t*)&size);
                }
            }
        } while (pins);

        // See if it is time to do a wireless LAN status check
        time = UEZTickCounterGet();
        if ((time - iStatusUpdateTime)
                >= REDPINE_RS9110_N_11_22_CHECK_STATUS_INTERVAL) {
            // Send out a query request for an update
            rsi_wlan_conn_query_req();
            iStatusUpdateTime = time;
        }

        // Done with this round, release the semaphore
        // so the outside routines can run tasks
        UEZSemaphoreRelease(p->iTaskSem);
    }

    // Coming out, dying, dead
    p->iTaskDead = ETrue;

    return 0;
}

/*---------------------------------------------------------------------------*
 * Routine:  IStartup
 *---------------------------------------------------------------------------*
 * Description:
 *      Start up the Redpine module
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IStartup(
        T_Network_Redpine_RS9110_Workspace *p,
        T_uezNetworkSettings *aSettings)
{
    T_uezError error = UEZ_ERROR_NONE;
    struct WiFi_CNFG_Scan_t *scan_info = &Adaptor.scan_params;
    struct WiFi_CNFG_Join_t *join_info = &Adaptor.join_params;
    ipconfig_t *ipConfig = &Adaptor.ipConfig;
    TUInt32 i;
//    TBool doingScan = EFalse;
//    TBool doingJoin = EFalse;
    int status;

    /* Initialize Socket data base */
    rsi_init_socket_data_base();

    // Force a firmware update
    if (aSettings->iNetworkType == UEZ_NETWORK_TYPE_FORCE_UPGRADE_FIRMWARE)
        Adaptor.image_upgrade = 1;
    else
        Adaptor.image_upgrade = 0;

    // Start up the power pins
    if (rsi_poweron_device() != RSI_STATUS_SUCCESS)
        return UEZ_ERROR_COULD_NOT_START;

    // Setup a semaphore for handling routines that may conflict on
    // the outside and when the task is running.
    UEZSemaphoreCreateBinary(&p->iTaskSem);
    if (error)
        return error;

    // Setup semaphores that allow waiting for the join status
    UEZSemaphoreCreateCounting(&p->iJoinSem, 1, 0);
    if (error)
        return error;

    // Setup semaphores that allow waiting for the scan status
    UEZSemaphoreCreateCounting(&p->iScanSem, 1, 0);
    if (error)
        return error;

    // About to create the background task.  Its about to be alive.
    p->iTaskDie = EFalse;
    p->iTaskDead = EFalse;

    // Now setup the default configuration settings
    scan_info->uChannelNo = aSettings->iScanChannel;
    if (aSettings->iNetworkType == UEZ_NETWORK_TYPE_INFRASTRUCTURE) {
        join_info->uNwType = INFRASTRUCTURE;
    } else if (aSettings->iNetworkType == UEZ_NETWORK_TYPE_IBSS) {
        join_info->uNwType = IBSS;
//        if (!aSettings->iAutoConnectToOpen) {
            // Not doing an auto-connect on a IBSS (peer to peer ad-hoc) network,
            // so determine if this is a create or join
            join_info->uChannel = aSettings->iIBSSChannel;
            if (aSettings->iIBSSRole == UEZ_NETWORK_IBSS_ROLE_CREATE) {
                join_info->uIbss_mode = CREATE;
            } else if (aSettings->iIBSSRole == UEZ_NETWORK_IBSS_ROLE_JOIN) {
                join_info->uIbss_mode = JOIN;
            } else {
                return UEZ_ERROR_INVALID_PARAMETER;
            }
//        }
    } else if (aSettings->iNetworkType
            == UEZ_NETWORK_TYPE_FORCE_UPGRADE_FIRMWARE) {
        join_info->uNwType = INFRASTRUCTURE;
    } else {
        return UEZ_ERROR_INVALID_PARAMETER;
    }

    // Set the default data rate (0=auto)
    join_info->uDataRate = aSettings->iTXRate;

    // Set the power of the transmittor
    if (aSettings->iTXPower == UEZ_NETWORK_TRANSMITTER_POWER_HIGH)
        join_info->uPowerlevel = HIGH;
    else if (aSettings->iTXPower == UEZ_NETWORK_TRANSMITTER_POWER_MEDIUM)
        join_info->uPowerlevel = MEDIUM;
    else if (aSettings->iTXPower == UEZ_NETWORK_TRANSMITTER_POWER_LOW)
        join_info->uPowerlevel = LOW;
    else
        return UEZ_ERROR_INVALID_PARAMETER;

    // clear the Scan SSID
    memset(scan_info->uSSID, 0, UEZ_NETWORK_INFO_NAME_LENGTH);
    i = 0;
    if ((aSettings->iSSID) /* && (!aSettings->iAutoConnectToOpen) */) {
        // Copy over the SSID (up to the limit of UEZ_NETWORK_INFO_NAME_LENGTH characters)
        for (i = 0; (i < UEZ_NETWORK_INFO_NAME_LENGTH)
                && (aSettings->iSSID[i]); i++)
            scan_info->uSSID[i] = aSettings->iSSID[i];
    }

//    doingScan = ETrue;
    // Fill rest with nulls
    while (i < UEZ_NETWORK_INFO_NAME_LENGTH)
        scan_info->uSSID[i++] = '\0';

#if 0
    // Copy over the SSID and PSK for automatic joining
    memcpy(join_info->uSSID, aSettings->iJoinSSID, sizeof(join_info->uSSID));
    memcpy(join_info->uPSK, aSettings->iJoinPSK, sizeof(join_info->uPSK));

    // If auto connect feature is enabled, need to scan in all
    // the channels in broadcast mode
    if ((aSettings->iAutoConnectToOpen == ETrue) && (aSettings->iNetworkType
            != UEZ_NETWORK_TYPE_IBSS)) {
        // Can any channel since unsecure
        scan_info->uChannelNo = 0;

        // Don't join a specific SSID
        memset(join_info->uSSID, 0, UEZ_NETWORK_INFO_NAME_LENGTH);
        auto_connect_open_ap = ENABLE;
        doingJoin = ETrue;
    }

    if (join_info->uSSID[0])
        doingJoin = ETrue;
#endif
    // Initialize IP Configuration (was rsi_initialize_ipconfig_parmas)
    if ((aSettings->iNetworkType != UEZ_NETWORK_TYPE_IBSS)
            && (aSettings->iEnableDHCP)) {
        ipConfig->DHCP = ENABLE;
    } else {
        ipConfig->DHCP = DISABLE;
    }
    if (ipConfig->DHCP == DISABLE) {
        // Use the hard coded addresses
        memcpy(ipConfig->uIPaddr, aSettings->iIPAddress.v4, 4);
        memcpy(ipConfig->uSubnet, aSettings->iSubnetMask.v4, 4);
        memcpy(ipConfig->uDefaultGateway, aSettings->iGatewayAddress.v4, 4);
    }

    /* Initialize socket creation info */
    // TODO: Do we need this?
#if 0
    /* socket type */
    networkConfig->uSocketType = SOCKET_LTCP;
    /* Local port number */
    networkConfig->stLocalPort = 800;
    /* Remote port number */
    networkConfig->remote_address.sinPort = 2054;
    /* Remote ip address */
    memcpy(networkConfig->remote_address.ip, remote_ip, 4);
#endif

    if (aSettings->iNetworkType == UEZ_NETWORK_TYPE_FORCE_UPGRADE_FIRMWARE) {
        // Before starting the background task, if we are doing a firmware
        // upgrade, sit here and process commands until we are done
        do {
            status = Read_tcp_ip_event();
        } while ((status != RS22_FW_UPGRADE_SUCCESS) && (status
                != RS22_STATUS_SUCCESS));
        return UEZ_ERROR_NONE;
    }

    // Startup a task to handle the network driver
    error = UEZTaskCreate(Network_Redpine_RS9110_Task, "RS9110",
            UEZ_TASK_STACK_BYTES(1024), p, UEZ_PRIORITY_HIGH, &p->iTask);
    if (error)
        return error;

    // Wait for the scan to complete
#if 0
    if (doingScan) {
        p->iScanStatus = UEZ_NETWORK_SCAN_STATUS_BUSY;
        UEZSemaphoreGrab(p->iScanSem, UEZ_TIMEOUT_INFINITE);
    }

    // Wait for the join to complete
    if (doingJoin) {
        p->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_BUSY;
        UEZSemaphoreGrab(p->iJoinSem, UEZ_TIMEOUT_INFINITE);
    }

    // Do the ipconfig step (if we have a connection
    if (p->iJoinStatus == UEZ_NETWORK_JOIN_STATUS_SUCCESS) {
        UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);
        rsi_send_ipconfig_request(ipConfig);
        UEZSemaphoreRelease(p->iTaskSem);
    }
#endif

    // Configure power setting (default is no power saving)
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);
    rsi_send_power_save_request(POWER_MODE0);
    UEZSemaphoreRelease(p->iTaskSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  IShutdown
 *---------------------------------------------------------------------------*
 * Description:
 *      Shut down the Redpine module since no one is using it or even
 *      looking for incoming connections.
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
static T_uezError IShutdown(T_Network_Redpine_RS9110_Workspace *p)
{
    PARAM_NOT_USED(p);

    // Kill the background task
    p->iTaskDie = ETrue;

    // Wait for the background task to die
    while (!p->iTaskDead)
        UEZTaskDelay(1);

    // Delete the semaphores
    UEZSemaphoreDelete(p->iScanSem);
    UEZSemaphoreDelete(p->iJoinSem);
    UEZSemaphoreDelete(p->iTaskSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  IResetPower
 *---------------------------------------------------------------------------*
 * Description:
 *      Try to reset the power pins on the module
 * Inputs:
 *      T_Network_Redpine_RS9110_Workspace *p -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
static void IResetPower(T_Network_Redpine_RS9110_Workspace *p)
{
    // Start up with power down
    if (p->iGPIOPowerPort) {
        GPIO_CLEAR(p->iGPIOPowerPort, p->iGPIOPowerPin);
        (*p->iGPIOPowerPort)->SetOutputMode(p->iGPIOPowerPort, p->iGPIOPowerPin);
    }

    // Drive reset low
    if (p->iGPIOResetPort) {
        GPIO_CLEAR(p->iGPIOResetPort, p->iGPIOResetPin);
        (*p->iGPIOResetPort)->SetOutputMode(p->iGPIOResetPort, p->iGPIOResetPin);
    }

    // Wait at least 10 ms
    UEZTaskDelay(10);

    // Turn back on the power
    if (p->iGPIOPowerPort)
        GPIO_SET(p->iGPIOPowerPort, p->iGPIOPowerPin);

    // Must be active for 10 ms before we release from reset
    UEZTaskDelay(20);

    // Raise reset
    if (p->iGPIOResetPort)
        GPIO_SET(p->iGPIOResetPort, p->iGPIOResetPin);

    // Wait a bit
    UEZTaskDelay(20);

    // Drop/Toggle the reset line for 1 ms
    if (p->iGPIOResetPort)
        GPIO_CLEAR(p->iGPIOResetPort, p->iGPIOResetPin);
    UEZTaskDelay(1);
    if (p->iGPIOResetPort)
        GPIO_SET(p->iGPIOResetPort, p->iGPIOResetPin);
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_Open
 *---------------------------------------------------------------------------*
 * Description:
 *      Open up a new network user
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_Redpine_RS9110_Open(void *aWorkspace)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p->iNumOpen++;
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_Close
 *---------------------------------------------------------------------------*
 * Description:
 *      Close out a network that was previously opened.
 * Inputs:
 *      void *aWorkspace -- Workspace
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_Redpine_RS9110_Close(void *aWorkspace)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    if (p->iNumOpen == 0) {
        error = UEZ_ERROR_NOT_OPEN;
    } else {
        p->iNumOpen--;
        if (p->iNumOpen == 0) {
            // Last user done!  Let's close out and turn off the library
            IShutdown(p);
        }
    }
    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_Scan
 *---------------------------------------------------------------------------*
 * Description:
 *      Start a scan and setup a callback routine for receiving events.
 * Inputs:
 *      void *aWorkspace -- Workspace
 *      T_uezNetworkScanCallback *aCallback -- Routine to call when a scan is
 *          found or the scan is complete
 *      void *aCallbackWorkspace -- Workspace to use with the callback
 *      char *aScanSSID -- ID to scan under (null string if none)
 *      TUInt32 aTimeout -- Total time in ms to attempt scan before timing out.
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_Redpine_RS9110_Scan(
        void *aWorkspace,
        TUInt32 aChannelNumber,
        const char *aScanSSID,
        T_uezNetworkScanCallback aCallback,
        void *aCallbackWorkspace,
        TUInt32 aTimeout)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;
    char newSSID[UEZ_NETWORK_INFO_NAME_LENGTH];
    T_uezNetworkInfo info;
    struct WiFi_CNFG_ScanInfo *p_scan;

    PARAM_NOT_USED(aCallback);PARAM_NOT_USED(aCallbackWorkspace);

    // Doing a scan is an exclusive command.  Nothing else can be
    // processing.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Request the scan (without messing up the background task)
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);

    // Create a proper SSID from a string
    i = 0;
    memset(newSSID, 0, UEZ_NETWORK_INFO_NAME_LENGTH);
    if (aScanSSID) {
        // Copy over the SSID (up to the limit of UEZ_NETWORK_INFO_NAME_LENGTH characters)
        for (i = 0; (i < UEZ_NETWORK_INFO_NAME_LENGTH) && (aScanSSID[i]); i++)
            newSSID[i] = aScanSSID[i];
    }

    // TODO: For now, only do the scan if it has never been done OR
    // the SSID has changed.
    //    if ((p->iScanStatus != SCAN_STATUS_COMPLETE)
    //            || (Adaptor.scan_params.uChannelNo != aChannelNumber) || (memcmp(
    //            newSSID, Adaptor.scan_params.uSSID, UEZ_NETWORK_INFO_NAME_LENGTH)
    //            != 0)) {
    if (p->iScanStatus != UEZ_NETWORK_SCAN_STATUS_COMPLETE) {
        // Something has changed, change the adapter and try again
        memcpy(Adaptor.scan_params.uSSID, newSSID, UEZ_NETWORK_INFO_NAME_LENGTH);
        Adaptor.scan_params.uChannelNo = aChannelNumber;
        rsi_send_scan_request(Adaptor.scan_params.uChannelNo,
                Adaptor.scan_params.uSSID);

        UEZSemaphoreRelease(p->iTaskSem);

        // Wait for the scan to complete up to the timeout
        p->iScanStatus = UEZ_NETWORK_SCAN_STATUS_BUSY;
        error = UEZSemaphoreGrab(p->iScanSem, aTimeout);

        UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);
    }

    if (!error) {
        // Parse the scanned entries (if there is a callback)
        if (aCallback) {
            for (i = 0; i < Adaptor.ScanRsp.uScanCount; i++) {
                // Fill ou the structure and call the callback routine
                memset(&info, 0, sizeof(info));
                p_scan = &Adaptor.ScanRsp.stScanInfo[i];
                memcpy(info.iName, p_scan->uSSID, UEZ_NETWORK_INFO_NAME_LENGTH);
                info.iChannel = p_scan->uChannelNumber;
                info.iRSSILevel = -((TInt32)p_scan->uRssiVal);
                switch (p_scan->uSecMode) {
                    case 0:
                        info.iSecurityMode = UEZ_NETWORK_SECURITY_MODE_OPEN;
                        break;
                    case 1:
                        info.iSecurityMode = UEZ_NETWORK_SECURITY_MODE_WPA;
                        break;
                    case 2:
                        info.iSecurityMode = UEZ_NETWORK_SECURITY_MODE_WPA2;
                        break;
                    case 3:
                        info.iSecurityMode = UEZ_NETWORK_SECURITY_MODE_WEP;
                        break;
                    default:
                        info.iSecurityMode = UEZ_NETWORK_SECURITY_MODE_UNKNOWN;
                }
                info.iSecurityMode = (T_uezNetworkSecurityMode)p_scan->uSecMode;

                // Call the callback per info item
                // but stop if we are told to stop
                if (aCallback(aCallbackWorkspace, &info))
                    break;
            }

            // When done, call one last time with a 0.  This indicates
            // the end of the list.
            aCallback(aCallbackWorkspace, 0);
        }
    }

    UEZSemaphoreRelease(p->iTaskSem);

    UEZSemaphoreRelease(p->iSem);

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_Join
 *---------------------------------------------------------------------------*
 * Description:
 *      Join a network access point
 * Inputs:
 *      void *aWorkspace -- Workspace
 *      char *aJoinName -- Name of network to join
 *      char *aJoinPassword -- Password for access (or empty string for none)
 *      TUInt32 aTimeout -- Time out until abort join
 * Outputs:
 *      T_uezError -- Error code
 *---------------------------------------------------------------------------*/
T_uezError Network_Redpine_RS9110_Join(
        void *aWorkspace,
        const char *aJoinName,
        const char *aJoinPassword,
        TUInt32 aTimeout)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;
    TUInt32 i;
    struct WiFi_CNFG_Join_t *join_info = &Adaptor.join_params;

    // Doing a scan is an exclusive command.  Nothing else can be
    // processing.
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Request the join (without messing up the background task)
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);

    // Copy over the SSID and PSK for automatic joining
    memset(join_info->uSSID, 0, sizeof(join_info->uSSID));
    if (aJoinName) {
        for (i = 0; aJoinName[i] && i < sizeof(join_info->uSSID); i++)
            join_info->uSSID[i] = aJoinName[i];
    }
    memset(join_info->uPSK, 0, sizeof(join_info->uPSK));
    if (aJoinName) {
        for (i = 0; aJoinName[i] && i < sizeof(join_info->uPSK); i++)
            join_info->uPSK[i] = aJoinName[i];
    }

    // start the join request
    error = UEZSemaphoreGrab(p->iJoinSem, 0);
    p->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_BUSY;
    rsi_send_join_request(&Adaptor.join_params);
    UEZSemaphoreRelease(p->iTaskSem);

    // At this point, everything is automatic.  We wait for
    // the join to complete
    error = UEZSemaphoreGrab(p->iJoinSem, aTimeout);

    if ((!error) && (p->iJoinStatus != UEZ_NETWORK_JOIN_STATUS_SUCCESS))
        error = UEZ_ERROR_COULD_NOT_START;

    UEZSemaphoreRelease(p->iSem);

    return error;
}
/*---------------------------------------------------------------------------*
 * Internal Routine:  RedpineRS9110_Internal_ResetPower
 *---------------------------------------------------------------------------*
 * Description:
 *      Internal routine for Redpine RS9110_N_11_22 driver ONLY.
 *      Reset the power to the redpine module
 *---------------------------------------------------------------------------*/
void RedpineRS9110_Internal_ResetPower(void)
{
    // Reset the power
    IResetPower(G_rs9110);
}

/*---------------------------------------------------------------------------*
 * Internal Routine:  RedpineRS9110_Internal_ResetPower
 *---------------------------------------------------------------------------*
 * Description:
 *      The Redpine library has requested a SPI transfer.  Pass on the
 *      the command to the current Redpine module's SPI bus transferring
 *      data in and out.  No errors are reported
 * Inputs:
 *      TUInt8 *aDataMOSI -- Data going from master to slave
 *      TUInt8 *aDataMISO -- Data going from slave to master
 *      TUInt32 aNumBytes -- Number of bytes to transfer
 *---------------------------------------------------------------------------*/
void RedpineRS9110_Internal_TransferBytes(
        TUInt8 *aDataMOSI,
        TUInt8 *aDataMISO,
        TUInt32 aNumBytes)
{
    DEVICE_SPI_BUS **p_spi = G_rs9110->iSPI;
    SPI_Request *r = &G_rs9110->iSPIRequest;
//TUInt32 i;
    // Only run if open
    if (!G_rs9110->iNumOpen)
        return;
#if 0
if (aNumBytes < 100) {
  if (aDataMOSI) {
    printf("[");
    for (i=0; i<aNumBytes; i++)
      printf("%02X", aDataMOSI[i]);
    printf("]");
  } else {
    printf("0");
  }
} else {
  printf("[...]");
}
printf("->");
#endif
    // Configure the request
    r->iDataMOSI = aDataMOSI;
    r->iDataMISO = aDataMISO;
    r->iNumTransfers = aNumBytes;

    // Do the in/out transfer
    (*p_spi)->Start(p_spi, r);
    (*p_spi)->TransferInOutBytes(p_spi, r);
    (*p_spi)->Stop(p_spi, r);
#if 0
if (aNumBytes < 100) {
  if (aDataMISO) {
    printf("[");
    for (i=0; i<aNumBytes; i++)
      printf("%02X", aDataMISO[i]);
    printf("]");
  } else {
    printf("0");
  }
} else {
  printf("[...]");
}
//printf("\n");
#endif
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_Scan_Complete_Callback
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback for scan completing.
 *---------------------------------------------------------------------------*/
void Network_Redpine_RS9110_Scan_Complete_Callback(void)
{
    G_rs9110->iScanStatus = UEZ_NETWORK_SCAN_STATUS_COMPLETE;
    UEZSemaphoreRelease(G_rs9110->iScanSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_Join_Complete_Callback
 *---------------------------------------------------------------------------*
 * Description:
 *      Callback for join completing.
 * Inputs:
 *      TBool aSuccess -- ETrue if join was successful, else EFalse
 *---------------------------------------------------------------------------*/
void Network_Redpine_RS9110_Join_Complete_Callback(TBool aSuccess)
{
    if (aSuccess) {
        G_rs9110->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_SUCCESS;
    } else {
        G_rs9110->iJoinStatus = UEZ_NETWORK_JOIN_STATUS_FAIL;
    }
    UEZSemaphoreRelease(G_rs9110->iJoinSem);
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_Configure
 *---------------------------------------------------------------------------*
 * Description:
 *      Configure the Redpine RS9110 to pick out a SPI configuration and
 *      GPIO for chip select.
 * Inputs:
 *      void *aW                    -- Workspace
 *      DEVICE_SPI_BUS **aSPI       -- SPI interface
 *      TUInt32 aSPIRate            -- SPI rate to run on this system
 *      HAL_GPIOPort **aSPICSPort   -- Chip select GPIO port
 *      TUInt32 aSPICSPin           -- Chip select GPIO pin
 *      HAL_GPIOPort **aGPIOResetPort -- Reset GPIO port (0 for none)
 *      TUInt32 aGPIOResetPin       -- Reset GPIO pin (0 for none)
 *      HAL_GPIOPort **aGPIOPowerPort -- Power GPIO port (0 for none)
 *      TUInt32 aGPIOPowerPin -- Power GPIO pin (0 for none)
 *      HAL_GPIOPort **aGPIOInterruptPort -- Interrupt GPIO port
 *      TUInt32 aGPIOInterruptIn -- Interrupt GPIO pin
 *---------------------------------------------------------------------------*/
void Network_Redpine_RS9110_Configure(
        void *aWorkspace,
        DEVICE_SPI_BUS **aSPI,
        TUInt32 aSPIRate,
        HAL_GPIOPort **aSPICSPort,
        TUInt32 aSPICSPin,
        HAL_GPIOPort **aGPIOResetPort,
        TUInt32 aGPIOResetPin,
        HAL_GPIOPort **aGPIOPowerPort,
        TUInt32 aGPIOPowerPin,
        HAL_GPIOPort **aGPIOInterruptPort,
        TUInt32 aGPIOInterruptPin)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;

    // Setup the SPI bus (and associated chip select)
    p->iSPI = aSPI;
    p->iSPIRequest.iRate = aSPIRate;
    p->iSPIRequest.iCSGPIOPort = aSPICSPort;
    p->iSPIRequest.iCSGPIOBit = aSPICSPin;
    p->iSPIRequest.iCSPolarity = EFalse;
    p->iSPIRequest.iClockOutPhase = EFalse;
    p->iSPIRequest.iClockOutPolarity = EFalse;
    p->iSPIRequest.iRate = aSPIRate;
    p->iSPIRequest.iBitsPerTransfer = 8;

    // Setup other pins
    p->iGPIOPowerPort = aGPIOPowerPort;
    p->iGPIOPowerPin = aGPIOPowerPin;
    p->iGPIOResetPort = aGPIOResetPort;
    p->iGPIOResetPin = aGPIOResetPin;
    p->iGPIOInterruptPort = aGPIOInterruptPort;
    p->iGPIOInterruptPin = aGPIOInterruptPin;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_GetStatus
 *---------------------------------------------------------------------------*
 * Description:
 *      Get the current status of the RS9110.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_uezNetworkStatus *aStatus -- Status structure returned
 *---------------------------------------------------------------------------*/
static T_uezError Network_Redpine_RS9110_GetStatus(
        void *aWorkspace,
        T_uezNetworkStatus *aStatus)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    struct WiFi_CNFG_Join_t *join_info = &Adaptor.join_params;
    ipconfig_t *ipConfig = &Adaptor.ipConfig;

    // Grab and pause the background task (we don't want
    // the background task changing things while we copy over the info)
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);

    // Copy over the current info
    memcpy(aStatus->iIPAddr.v4, ipConfig->uIPaddr, 4);
    memcpy(aStatus->iGatewayAddress.v4, ipConfig->uDefaultGateway, 4);
    memcpy(aStatus->iSubnetMask.v4, ipConfig->uSubnet, 4);
    aStatus->iInfo.iChannel = join_info->uChannel;
    aStatus->iInfo.iRSSILevel = 0; // current not supported
    aStatus->iInfo.iSecurityMode = (T_uezNetworkSecurityMode)join_info->uSecType;
    memcpy(aStatus->iInfo.iName, join_info->uSSID, sizeof(aStatus->iInfo.iName));
    aStatus->iJoinStatus = p->iJoinStatus;
    aStatus->iScanStatus = p->iScanStatus;

    // Done, release the background task and continue
    UEZSemaphoreRelease(p->iTaskSem);
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

T_uezError Network_Redpine_RS9110_SocketCreate(
        void *aWorkspace,
        T_uezNetworkSocketType aType,
        T_uezNetworkSocket *aSocket)
{
    // Find a free socket slot
    T_uezNetworkSocket i;
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;

    // Only support TCP sockets current
    if (aType != UEZ_NETWORK_SOCKET_TYPE_TCP)
        return UEZ_ERROR_INVALID_PARAMETER;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    *aSocket = 0; // none found yet
    for (i = 1; i <= REDPINE_NUM_SOCKETS; i++) {
        if (p->iSockets[i].iState == SOCKET_STATE_FREE) {
            // Allocate this slot and report back this slot
            p->iSockets[i].iState = SOCKET_STATE_BUSY;
            p->iSockets[i].iType = aType;
            p->iSockets[i].iFlags = 0;
            p->iSockets[i].iAppId = 0;
            *aSocket = i;
            break;
        }
    }
    UEZSemaphoreRelease(p->iSem);

    if (!*aSocket)
        return UEZ_ERROR_OUT_OF_HANDLES;
    return UEZ_ERROR_NONE;
}

static T_redpineSocket *ISocketFindByAppId(int32_t aAppId)
{
    int i;
    T_Network_Redpine_RS9110_Workspace *p = G_rs9110;
    T_redpineSocket *p_sock;

    p_sock = p->iSockets + 1;
    for (i = 1; i <= REDPINE_NUM_SOCKETS; i++, p_sock++) {
        if (p_sock->iState != SOCKET_STATE_FREE) {
            if (p_sock->iAppId == aAppId) {
                return p_sock;
            }
        }
    }
    return 0;
}

static T_redpineSocket *ISocketFindBySocketId(int32_t aSocketId)
{
    int i;
    T_Network_Redpine_RS9110_Workspace *p = G_rs9110;
    T_redpineSocket *p_sock;

    p_sock = p->iSockets + 1;
    for (i = 1; i <= REDPINE_NUM_SOCKETS; i++, p_sock++) {
        if (p_sock->iState != SOCKET_STATE_FREE) {
            if (p_sock->iSocketId == aSocketId) {
                return p_sock;
            }
        }
    }
    return 0;
}

static uint32_t ICallbackSocketCreated(int32_t aSocketId, uint16_t aSourcePort)
{
    TUInt32 appId = identify_table_entry(aSourcePort, 0);
    T_redpineSocket *p_sock = ISocketFindByAppId(appId);
    if (p_sock) {
        // Store the descriptor
        p_sock->iSourcePort = aSourcePort;
        p_sock->iFlags |= SOCKET_FLAG_CREATED;
        p_sock->iSocketId = aSocketId;
    }
    return 0;
}

static uint32_t ICallbackSocketAccept(int32_t aSocketId, int32_t aNewSocketId, // not used
        uint16_t aDestinationPort,
        uint32_t aDestinationIPAddress)
{
    // Accept the id
    T_redpineSocket *p_sock = ISocketFindBySocketId(aSocketId);
    PARAM_NOT_USED(aNewSocketId);
    if (p_sock) {
        // Release the associated semaphore for this socket
        // and allow the calling task to go on
        UEZSemaphoreRelease(p_sock->iAcceptSem);
    }
    return 0;
}

static uint32_t ICallbackSocketReceive(
        uint8_t *pBuf,
        uint32_t BytesReceived,
        uint16_t aSocketId)
{
    // TODO: This is a terrible way to do the code!  We should be
    // more efficient.  Plus, we should never lose data like this!
    // Accept the id
    T_redpineSocket *p_sock = ISocketFindBySocketId(aSocketId);
    T_uezQueue q;
    if (p_sock) {
        // First version of this code just stuffs in (yes) one character
        // at a time and drops anything that falls out.
        q = p_sock->iRecvQueue;
        while (BytesReceived--) {
            if (UEZQueueSend(q, pBuf++, 0) == UEZ_ERROR_TIMEOUT)
                break;
        }
    }
    return 0;
}

static uint32_t ICallbackSocketClose(int32_t aSocketId)
{
    // Mark the socket as closed
    T_redpineSocket *p_sock = ISocketFindBySocketId(aSocketId);
    if (p_sock) {
        p_sock->iFlags |= SOCKET_FLAG_CLOSED;
        UEZSemaphoreRelease(p_sock->iCloseSem);
    } else {
        printf("ICallbackSocketClose: Trying to close socket id %d early!\n", aSocketId);
    }
    return 0;
}

static uint32_t ICallbackSocketRemoteClose(int32_t aSocketId)
{
    // Mark the socket as closed
    T_redpineSocket *p_sock = ISocketFindBySocketId(aSocketId);
    if (p_sock) {
        p_sock->iFlags |= SOCKET_FLAG_CLOSED | SOCKET_FLAG_REMOTE_CLOSED;
        UEZSemaphoreRelease(p_sock->iCloseSem);
    } else {
        printf("ICallbackSocketRemoteClose: Trying to close socket id %d early!\n", aSocketId);
    }
    return 0;
}

T_uezError Network_Redpine_RS9110_SocketBind(
        void *aWorkspace,
        T_uezNetworkSocket aSocket,
        T_uezNetworkAddr *aAddr,
        TUInt16 aPort)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    networkConfig_t config;
    T_redpineSocket *p_sock;
    int32_t status;

    if (aSocket == 0)
        return UEZ_ERROR_HANDLE_INVALID;
    if (aSocket > REDPINE_NUM_SOCKETS)
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Check to see the state of the socket is valid
    p_sock = &p->iSockets[aSocket];
    if (p_sock->iState == SOCKET_STATE_FREE) {
        UEZSemaphoreRelease(p->iSem);
        return UEZ_ERROR_NOT_FOUND;
    }

    // Setup to receive on the given port and output on a local (listening) port
    p_sock->iPort = aPort;
    config.stLocalPort = aPort;
    config.uSocketType
            = (p_sock->iType == UEZ_NETWORK_SOCKET_TYPE_TCP) ? SOCKET_LTCP
                    : SOCKET_UDP;
    config.uSocketDescriptor = 0; // unknown currently
    config.remote_address.sinPort = INextLocalPort(p);
    if (aAddr) {
        config.remote_address.ip[0] = aAddr->v4[0];
        config.remote_address.ip[1] = aAddr->v4[1];
        config.remote_address.ip[2] = aAddr->v4[2];
        config.remote_address.ip[3] = aAddr->v4[3];
    } else {
        config.remote_address.ip[0] = 0;
        config.remote_address.ip[1] = 0;
        config.remote_address.ip[2] = 0;
        config.remote_address.ip[3] = 0;
    }

    // We'll create a user socket, but don't let the background task
    // process this command.
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);
    //    status = rsi_create_user_socket(&config);
    memcpy(&Adaptor.networkConfig[Adaptor.socket_created], &config,
            sizeof(networkConfig_t));
    p_sock->iCallbacks.cb_sock_create_event = ICallbackSocketCreated;
    p_sock->iCallbacks.cb_accept_event = ICallbackSocketAccept;
    p_sock->iCallbacks.cb_recv_event = ICallbackSocketReceive;
    p_sock->iCallbacks.cb_close_event = ICallbackSocketClose;
    p_sock->iCallbacks.cb_remoteclose_event = ICallbackSocketRemoteClose;
    rsi_socket_register((uint32_t*)&p_sock->iAppId, config.stLocalPort,
            *((uint32_t *)&Adaptor.ipConfig.uIPaddr[0]), &p_sock->iCallbacks);
    /* Send the socket create request */
    status = rsi_socket_create((SOCKET_TYPE)config.uSocketType,
            config.stLocalPort, &config.remote_address);
    if (status == RS22_STATUS_SUCCESS) {
        Adaptor.socket_created++;
        do {
            /* Read the socket create event */
            status = Read_tcp_ip_event();
        } while (status == RS22_STATUS_FAILURE);
    }
    UEZSemaphoreRelease(p->iTaskSem);
    UEZSemaphoreRelease(p->iSem);

    if (status != RS22_STATUS_SUCCESS)
        return UEZ_ERROR_NOT_AVAILABLE;

    // Copy over the configuration information
    p_sock->iConfig = config;

    return UEZ_ERROR_NONE;
}

T_uezError Network_Redpine_RS9110_SocketConnect(
        void *aWorkspace,
        T_uezNetworkSocket aSocket,
        T_uezNetworkAddr *aAddr,
        TUInt16 aPort)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    networkConfig_t config;
    T_redpineSocket *p_sock;
    int32_t status;

    if (aSocket == 0)
        return UEZ_ERROR_HANDLE_INVALID;
    if (aSocket > REDPINE_NUM_SOCKETS)
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Check to see the state of the socket is valid
    p_sock = &p->iSockets[aSocket];
    if (p_sock->iState == SOCKET_STATE_FREE) {
        UEZSemaphoreRelease(p->iSem);
        return UEZ_ERROR_NOT_FOUND;
    }

    // Setup TCP or UDP port
    config.remote_address.sinPort = aPort;
    config.uSocketType
            = (p_sock->iType == UEZ_NETWORK_SOCKET_TYPE_TCP) ? SOCKET_TCP
                    : SOCKET_UDP;
    config.uSocketDescriptor = 0; // unknown currently
    p_sock->iPort = config.stLocalPort = INextLocalPort(p);
    if (aAddr) {
        config.remote_address.ip[0] = aAddr->v4[0];
        config.remote_address.ip[1] = aAddr->v4[1];
        config.remote_address.ip[2] = aAddr->v4[2];
        config.remote_address.ip[3] = aAddr->v4[3];
    } else {
        config.remote_address.ip[0] = 0;
        config.remote_address.ip[1] = 0;
        config.remote_address.ip[2] = 0;
        config.remote_address.ip[3] = 0;
    }

    // We'll create a user socket, but don't let the background task
    // process this command.
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);
    //    status = rsi_create_user_socket(&config);
    memcpy(&Adaptor.networkConfig[Adaptor.socket_created], &config,
            sizeof(networkConfig_t));
    p_sock->iCallbacks.cb_sock_create_event = ICallbackSocketCreated;
    p_sock->iCallbacks.cb_accept_event = ICallbackSocketAccept;
    p_sock->iCallbacks.cb_recv_event = ICallbackSocketReceive;
    p_sock->iCallbacks.cb_close_event = ICallbackSocketClose;
    p_sock->iCallbacks.cb_remoteclose_event = ICallbackSocketRemoteClose;
    rsi_socket_register((uint32_t*)&p_sock->iAppId, config.stLocalPort,
            *((uint32_t *)&Adaptor.ipConfig.uIPaddr[0]), &p_sock->iCallbacks);
    /* Send the socket create request */
    status = rsi_socket_create((SOCKET_TYPE)config.uSocketType,
            config.stLocalPort, &config.remote_address);
    if (status == RS22_STATUS_SUCCESS) {
        Adaptor.socket_created++;
        do {
            /* Read the socket create event */
            status = Read_tcp_ip_event();
        } while (status == RS22_STATUS_FAILURE);
    }
    UEZSemaphoreRelease(p->iTaskSem);
    UEZSemaphoreRelease(p->iSem);

    if (status != RS22_STATUS_SUCCESS)
        return UEZ_ERROR_NOT_AVAILABLE;

    // Copy over the configuration information
    p_sock->iConfig = config;

    return UEZ_ERROR_NONE;
}

T_uezError Network_Redpine_RS9110_SocketListen(
        void *aWorkspace,
        T_uezNetworkSocket aSocket)
{
    // Currently, Listen doesn't do anything (done all in bind above)
    return UEZ_ERROR_NONE;
}

T_uezError Network_Redpine_RS9110_SocketAccept(
        void *aWorkspace,
        T_uezNetworkSocket aListenSocket,
        T_uezNetworkSocket *aFoundSocket,
        TUInt32 aTimeout)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_redpineSocket *p_sock;
    T_uezSemaphore sem;
    T_uezError error = UEZ_ERROR_NONE;
    //TUInt16 port;

// TESTING
*aFoundSocket = aListenSocket;

    if (aListenSocket == 0)
        return UEZ_ERROR_HANDLE_INVALID;
    if (aListenSocket > REDPINE_NUM_SOCKETS)
        return UEZ_ERROR_HANDLE_INVALID;

    // Grab everything to setup this socket accept
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);

    // Check to see the state of the socket is valid
    p_sock = &p->iSockets[aListenSocket];
    if (p_sock->iState == SOCKET_STATE_FREE) {
        // No good
        error = UEZ_ERROR_NOT_FOUND;
    } else if ((p_sock->iFlags & SOCKET_FLAG_CREATED) == 0) {
        error = UEZ_ERROR_NOT_AVAILABLE;
    }
    if (error) {
        UEZSemaphoreRelease(p->iTaskSem);
        UEZSemaphoreRelease(p->iSem);
        return error;
    }

    // Got the socket, now get the semaphore handle
    // and put it in an accept mode
    sem = p_sock->iAcceptSem;
    //port = p_sock->iConfig.stLocalPort;

    // Hold on, is this socket closed?
#if 1
    if (p_sock->iFlags & SOCKET_FLAG_CLOSED) {
        p_sock->iFlags = SOCKET_FLAG_CREATED;
        UEZSemaphoreRelease(p->iTaskSem);
        UEZSemaphoreRelease(p->iSem);
        // Rebind this
        Network_Redpine_RS9110_SocketBind(aWorkspace, aListenSocket, 0,
                p_sock->iPort);
        UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
        UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);
    }
#endif
    p_sock->iFlags = SOCKET_FLAG_CREATED;

    UEZSemaphoreRelease(p->iTaskSem);
    UEZSemaphoreRelease(p->iSem);

    // Wait for the accept to come in
    return UEZSemaphoreGrab(sem, aTimeout);
}

T_uezError Network_Redpine_RS9110_SocketClose(
        void *aWorkspace,
        T_uezNetworkSocket aSocket)
{
    // Close out the existing socket
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_redpineSocket *p_sock;
    //networkConfig_t networkConfig;
    TUInt32 appId;
    int32_t status;

    if (aSocket == 0)
        return UEZ_ERROR_HANDLE_INVALID;
    if (aSocket > REDPINE_NUM_SOCKETS)
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Check to see the state of the socket is valid
    p_sock = &p->iSockets[aSocket];
    if (p_sock->iState == SOCKET_STATE_FREE) {
        UEZSemaphoreRelease(p->iSem);
        return UEZ_ERROR_NOT_FOUND;
    }

    // Process the closing of the socket without
    // interference of the background task
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);
    appId = p_sock->iAppId;

    // Are we already closed?
    if ((p_sock->iFlags & SOCKET_FLAG_CLOSED) == 0) {
        //rsi_close_user_socket(&networkConfig);
        status = rs22_socket_close(p_sock->iSocketId);
        if (status == RS22_STATUS_SUCCESS) {
            Adaptor.socket_created--;
            /* Deregester the socket from the socket data base */
            do {
                /* Read the socket close event */
                status = Read_tcp_ip_event();
            } while (status == RS22_STATUS_FAILURE);
        }
        UEZSemaphoreRelease(p->iTaskSem);
        // Wait for the socket to close
        UEZSemaphoreGrab(p_sock->iCloseSem, 60000);
    } else {
        UEZSemaphoreRelease(p->iTaskSem);
    }

    rsi_socket_deregister(appId);

    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

T_uezError Network_Redpine_RS9110_SocketDelete(
        void *aWorkspace,
        T_uezNetworkSocket aSocket)
{
    // Close out the existing socket
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_redpineSocket *p_sock;

    if (aSocket == 0)
        return UEZ_ERROR_HANDLE_INVALID;
    if (aSocket > REDPINE_NUM_SOCKETS)
        return UEZ_ERROR_HANDLE_INVALID;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);

    // Check to see the state of the socket is valid
    p_sock = &p->iSockets[aSocket];
    if (p_sock->iState == SOCKET_STATE_FREE) {
        UEZSemaphoreRelease(p->iSem);
        return UEZ_ERROR_NOT_FOUND;
    }

    //    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);
    //    UEZSemaphoreRelease(p->iTaskSem);
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

T_uezError Network_Redpine_RS9110_SocketRead(
        void *aWorkspace,
        T_uezNetworkSocket aSocket,
        void *aData,
        TUInt32 aNumBytes,
        TUInt32 *aNumBytesRead,
        TUInt32 aTimeout)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_redpineSocket *p_sock;
    TUInt32 i;
    TUInt8 c;
    TUInt8 *p_data = (TUInt8 *)aData;
    T_uezQueue q;
    T_uezError error = UEZ_ERROR_NONE;

    // TODO: I'm almost embarrassed to write the code this way, but it
    // is a starting point.  We just read byte per byte through the queue.
    // This technique works good enough for serial ports, but may be too
    // slow for ethernet.  At least for wireless it'll work.

    *aNumBytesRead = 0;
    if (aSocket == 0)
        return UEZ_ERROR_HANDLE_INVALID;
    if (aSocket > REDPINE_NUM_SOCKETS)
        return UEZ_ERROR_HANDLE_INVALID;

    // Grab everything to setup this socket accept
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);

    // Check to see the state of the socket is valid
    p_sock = &p->iSockets[aSocket];
    if (p_sock->iState == SOCKET_STATE_FREE)
        error = UEZ_ERROR_NOT_FOUND;
    if (p_sock->iFlags & SOCKET_FLAG_CLOSED)
        error = UEZ_ERROR_NOT_READY;
    if (error) {
        UEZSemaphoreRelease(p->iTaskSem);
        UEZSemaphoreRelease(p->iSem);
        return error;
    }

    // Got the socket, now get the queue handle
    q = p_sock->iRecvQueue;

    UEZSemaphoreRelease(p->iTaskSem);
    UEZSemaphoreRelease(p->iSem);

    // Read out all the bytes as we need them (or are available until it
    // times out)
    for (i = 0; i < aNumBytes; i++) {
        error = UEZQueueReceive(q, &c, aTimeout);
        if (error)
            break;
        *(p_data++) = c;
    }
    *aNumBytesRead = i;

    // Wait for the accept to come in
    return error;
}

T_uezError Network_Redpine_RS9110_SocketWrite(
        void *aWorkspace,
        T_uezNetworkSocket aSocket,
        void *aData,
        TUInt32 aNumBytes,
        TBool aFlush,
        TUInt32 aTimeout)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_redpineSocket *p_sock;
    T_uezError error = UEZ_ERROR_NONE;
    PARAM_NOT_USED(aFlush);

    if (aSocket == 0)
        return UEZ_ERROR_HANDLE_INVALID;
    if (aSocket > REDPINE_NUM_SOCKETS)
        return UEZ_ERROR_HANDLE_INVALID;

    // Grab everything to setup this socket accept
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);

    // Check to see the state of the socket is valid
    p_sock = &p->iSockets[aSocket];
    if (p_sock->iState == SOCKET_STATE_FREE)
        error = UEZ_ERROR_NOT_FOUND;
    if (p_sock->iFlags & SOCKET_FLAG_CLOSED)
        error = UEZ_ERROR_NOT_READY;
    if (error) {
        UEZSemaphoreRelease(p->iTaskSem);
        UEZSemaphoreRelease(p->iSem);
        return error;
    }

    // Write out the data, blocking until complete
    rsi_send_data(p_sock->iSocketId, aNumBytes, aData);

    // Let the background task run again
    UEZSemaphoreRelease(p->iTaskSem);
    UEZSemaphoreRelease(p->iSem);

    // Wait for the accept to come in
    return error;
}

T_uezError IAuxCmdFirmwareGetVersion(
        T_Network_Redpine_RS9110_Workspace *p,
        char *aVersion)
{
    T_uezError error = UEZ_ERROR_NONE;
    int status;
    extern uint8_t fwversion[];

    // Grab everything to setup this socket accept
    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    UEZSemaphoreGrab(p->iTaskSem, UEZ_TIMEOUT_INFINITE);

    // Request the firmware version
    rsi_send_fwversion_Req();
    status = Read_tcp_ip_event();
    if (status == RS22_STATUS_SUCCESS)
        strcpy(aVersion, (const char *)fwversion);
    else
        strcpy(aVersion, "not found");

    // Copy over the current firmware version
    // Let the background task run again
    UEZSemaphoreRelease(p->iTaskSem);
    UEZSemaphoreRelease(p->iSem);

    return error;
}

T_uezError Network_Redpine_RS9110_AuxControl(
        void *aWorkspace,
        TUInt32 aAuxCommand,
        void *aAuxData)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    switch (aAuxCommand) {
        case REDPINE_RS9110_N_11_22_AUX_COMMAND_GET_FIRMWARE_VERSION:
            error = IAuxCmdFirmwareGetVersion(p, (char *)aAuxData);
            break;
        default:
            error = UEZ_ERROR_NOT_SUPPORTED;
            break;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_Create
 *---------------------------------------------------------------------------*
 * Description:
 *      Create the Redpine RS9110 Network device driver and configure with
 *      the given settings (SPI bus, SPI rate, and GPIO pins).
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_Redpine_RS9110_Settings *aSettings -- Settings for the RS9110
 *---------------------------------------------------------------------------*/
T_uezError Network_Redpine_RS9110_Create(
    const char *aName,
    T_Redpine_RS9110_Settings *aSettings)
{
    void *p;
    T_uezDevice spi;
    DEVICE_SPI_BUS **p_spi;
    T_uezError error;

    UEZDeviceTableRegister(aName,
            (T_uezDeviceInterface *)&Redpine_RS9110_N_11_22_Over_SPI_Network_Interface, 0,
            (T_uezDeviceWorkspace **)&p);

    error = UEZDeviceTableFind(aSettings->iSPIBusName, &spi);
    if (error)
        return error;
    UEZDeviceTableGetWorkspace(spi, (T_uezDeviceWorkspace **)&p_spi);

    UEZGPIOLock(aSettings->iReset);
    UEZGPIOLock(aSettings->iPower);
    UEZGPIOLock(aSettings->iSPIChipSelect);
    UEZGPIOLock(aSettings->iInterrupt);

    Network_Redpine_RS9110_Configure(
        p,
        p_spi,
        aSettings->iSPIRate,
        GPIO_TO_HAL_PORT(aSettings->iSPIChipSelect),
        GPIO_TO_PIN_BIT(aSettings->iSPIChipSelect),
        GPIO_TO_HAL_PORT(aSettings->iReset),
        GPIO_TO_PIN_BIT(aSettings->iReset),
        GPIO_TO_HAL_PORT(aSettings->iPower),
        GPIO_TO_PIN_BIT(aSettings->iPower),
        GPIO_TO_HAL_PORT_OR_FAIL(aSettings->iInterrupt),
        GPIO_TO_PIN_BIT(aSettings->iInterrupt));

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_InfrastructureConfigure
 *---------------------------------------------------------------------------*
 * Description:
 *      Setup the infrastructure settings to be used when bringing up
 *      or taking down the infrastructure.
 * Inputs:
 *      void *aW                    -- Workspace
 *      T_Redpine_RS9110_Settings *aSettings -- Settings for the RS9110
 *---------------------------------------------------------------------------*/
T_uezError Network_Redpine_RS9110_InfrastructureConfigure(
    void *aWorkspace,
    T_uezNetworkSettings *aSettings)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;

    UEZSemaphoreGrab(p->iSem, UEZ_TIMEOUT_INFINITE);
    p->iInfrastructureSettings = *aSettings;
    UEZSemaphoreRelease(p->iSem);

    return UEZ_ERROR_NONE;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_InfrastructureBringUp
 *---------------------------------------------------------------------------*
 * Description:
 *      Bring up the RS9110 network (using the previously set settings)
 * Inputs:
 *      void *aW                    -- Workspace
 *---------------------------------------------------------------------------*/
T_uezError Network_Redpine_RS9110_InfrastructureBringUp(void *aWorkspace)
{
    T_Network_Redpine_RS9110_Workspace *p =
            (T_Network_Redpine_RS9110_Workspace *)aWorkspace;
    T_uezError error = UEZ_ERROR_NONE;

    if (!p->iInfrastructureStarted) {
        error = IStartup(p, &p->iInfrastructureSettings);

        // Did we start up?
        if (!error)
            p->iInfrastructureStarted = ETrue;
    }

    return error;
}

/*---------------------------------------------------------------------------*
 * Routine:  Network_Redpine_RS9110_InfrastructureTakeDown
 *---------------------------------------------------------------------------*
 * Description:
 *      Take down the RS9110 infrastructure.
 *      CURRENTLY NOT SUPPORTED.
 * Inputs:
 *      void *aW                    -- Workspace
 *---------------------------------------------------------------------------*/
T_uezError Network_Redpine_RS9110_InfrastructureTakeDown(void *aWorkspace)
{
    PARAM_NOT_USED(aWorkspace);

    // Currently, taking down the RS9110 is not supported
    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError Network_Redpine_RS9110_Leave(void *aWorkspace, TUInt32 aTimeout)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aTimeout);

    return UEZ_ERROR_NOT_SUPPORTED;
}

T_uezError Network_Redpine_RS9110_ResolveAddress(void *aWorkspace,
    const char *aName,
    T_uezNetworkAddr *aAddr)
{
    PARAM_NOT_USED(aWorkspace);
    PARAM_NOT_USED(aName);
    PARAM_NOT_USED(aAddr);

    return UEZ_ERROR_NOT_SUPPORTED;
}

/*---------------------------------------------------------------------------*
 * Device Interface table:
 *---------------------------------------------------------------------------*/
const DEVICE_Network Redpine_RS9110_N_11_22_Over_SPI_Network_Interface = { {
// Common device interface
        "Network:Redpine:RS9110_N_11_22:SPI",
        0x0107,
        Network_Redpine_RS9110_InitializeWorkspace,
        sizeof(T_Network_Redpine_RS9110_Workspace), },

// Functions
        // added in uEZ v1.07
        Network_Redpine_RS9110_Open,
        Network_Redpine_RS9110_Close,
        Network_Redpine_RS9110_Scan,
        Network_Redpine_RS9110_Join,
        Network_Redpine_RS9110_GetStatus,
        Network_Redpine_RS9110_SocketCreate,
        Network_Redpine_RS9110_SocketConnect,
        Network_Redpine_RS9110_SocketBind,
        Network_Redpine_RS9110_SocketListen,
        Network_Redpine_RS9110_SocketAccept,
        Network_Redpine_RS9110_SocketClose,
        Network_Redpine_RS9110_SocketDelete,
        Network_Redpine_RS9110_SocketRead,
        Network_Redpine_RS9110_SocketWrite,
        Network_Redpine_RS9110_AuxControl,

        // uEZ v1.09
        Network_Redpine_RS9110_Leave,
        // uEZ v1.13
        Network_Redpine_RS9110_ResolveAddress,
        // uEZ v2.04
        Network_Redpine_RS9110_InfrastructureConfigure,
        Network_Redpine_RS9110_InfrastructureBringUp,
        Network_Redpine_RS9110_InfrastructureTakeDown,
};

/*-------------------------------------------------------------------------*
 * End of File:  Redpine_RS9110_N_11_22_driver.c
 *-------------------------------------------------------------------------*/
