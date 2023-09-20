/*-------------------------------------------------------------------------*
 * File:  UbiquiOS_Test.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2016 Future Designs, Inc.
 *--------------------------------------------------------------------------
 * This file is part of the uEZ(R) distribution.  See the included
 * uEZLicense.txt or visit http://www.teamfdi.com/uez for details.
 *
 *    *===============================================================*
 *    |  Future Designs, Inc. can port uEZ(R) to your own hardware!  |
 *    |             We can get you up and running fast!               |
 *    |      See http://www.teamfdi.com/uez for more details.         |
 *    *===============================================================*
 *
 *-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <UEZProcessor.h>
#include <UEZGPIO.h>
#include "ubiquios.h"
#include "ubiquios_wlan_bcmfmac.h"
#include "ub_bt.h"
//#include "ubiquios_bt_generic_h4.h"
#include "ubiquios_bt_bcm434x.h"
//#include "sterlinglwb-fcc.def"
#include "ub_wlan_bcmfmac.h"

#include "licence.def"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define BLE_MODE_OBSERVER    (0)
#define BLE_MODE_BROADCASTER (1)
/* This #define selects between BLE observer mode (scanning) and BLE
 * broadcaster mode (advertising). */
//#define BLE_MODE BLE_MODE_OBSERVER
#define BLE_MODE BLE_MODE_BROADCASTER

#define TEST_WIFI           1
#define TEST_CELLULAR       0
#define TEST_BLE            1
#define SHARED_POWER_PIN    GPIO_P4_14

#define LOCALNAME   "ubiquiOS"
/**
 * The UUID for a service that we advertise support for.
 * This was generated using the UUID generator found here:
 * http://www.itu.int/en/ITU-T/asn1/Pages/UUID/generate_uuid.aspx
 */
#define SERVICE_UUID                                                    \
    {  0x09, 0x2a, 0x46, 0xe0, 0xd7, 0x3d, 0x11, 0xe4,                  \
       0x8e, 0x5e, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b, }

/*-------------------------------------------------------------------------*
 * Globals:
 *-------------------------------------------------------------------------*/
static TBool G_is_sleeping = EFalse;
static T_uezQueue G_sleep_queue;

TBool G_BLE_PoweredON = EFalse;
TBool G_WiFi_PoweredON = EFalse;

char G_BLE_MAC[20] = "\0";
char G_WIFI_MAC[20] = "\0";

/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/

static void
start_advertising_callback(ub_status_t status)
{
    if (status == UB_STATUS_SUCCESS)
    {
        ub_ustdio_printf("Advertising started successfully\n");
    }
    else
    {
        ub_ustdio_printf("Failed to start advertising (error code = %u)\n",
                         status);
    }
}

/**
 * This function starts BLE advertising.
 *
 * It constructs the advertising data that will go into our advertisements.
 * In this example our advertising data comprises two elements:
 *   * The device local name
 *   * A service UUID list that contains a single 128-bit UUID
 */
static void
start_advertising(void)
{
    ub_status_t status;
    ub_bt_le_advertising_data_t ad;
    ub_bt_uuid_t uuid = { .uuid128 = SERVICE_UUID };
    ub_bt_le_advertising_parameters_t parameters = {
            /* Non-connectable undirected advertising */
            .advertising_type = UB_BT_LE_ADV_NONCONN_IND,
            /* Advertising interval can range between 1-2 seconds */
            .advertising_interval_min =
                    ub_bt_le_time_to_advertising_interval(UB_SECONDS(1)),
            .advertising_interval_max =
                    ub_bt_le_time_to_advertising_interval(UB_SECONDS(2)),
            /* Use the device's public Bluetooth address when advertising */
            .own_address_type = UB_BT_LE_ADDRESS_TYPE_PUBLIC,
            /* UNUSED */
            .direct_address_type = UB_BT_LE_ADDRESS_TYPE_PUBLIC,
            /* UNUSED */
            .direct_address = { 0, 0, 0, 0, 0, 0 },
            /* Advertise on all advertising channels */
            .advertising_channel_map = UB_BT_LE_ALL_ADVERTISING_CHANNELS,
            /* No whitelisting */
            .advertising_filter_policy =
                        UB_BT_LE_ALLOW_SCAN_FROM_ANY_CONN_REQ_FROM_ANY,
            .readvertise_after_disconnect = false,
    };

    ub_bt_le_advertising_data_init(&ad);

    /* NB., the ub_lstrlen() macro provides a means of getting the length of
     * a string literal at compile time, thus not requiring a call to
     * strlen() which would have to iterate over the string.  However, it
     * is important to note that this macro only works on string literals.
     */

    ub_bt_le_ad_append_local_name(&ad, true, LOCALNAME, ub_lstrlen(LOCALNAME));

    ub_bt_le_ad_append_service_uuid_list(&ad, true, UB_BT_UUID_128_BIT,
                                         &uuid, 1);

    status = ub_bt_le_start_advertising(&parameters, &ad, NULL,
                                        start_advertising_callback);
    if (status != UB_STATUS_SUCCESS)
    {
        ub_ustdio_printf("Start advertising failed with status code %d\n",
                         status);
    }
}

static bool
bt_le_scan_callback(/*@null@*/ ub_bt_le_scan_result_t *result)
{
    int i = 0;
    ub_ustdio_printf("Scan Complete\n");
    while(result != NULL)
    {
        ub_ustdio_printf("Result#%d\n", ++i);

        int16_t tx_power_level = ub_bt_le_scan_get_tx_power(result);

        ub_ustdio_printf("BD_ADDR %m  RSSI %4d dBm TX %4d dBm  Timestamp %u\n",
                         result->address, result->rssi, tx_power_level,
                         result->timestamp);
        result = result->next;
    }
    ub_ustdio_printf("%d devices found\n", i);
    return false;
}

static void
start_scan(void)
{
    ub_status_t status;
    ub_bt_le_scan_args_t scan_args = {
        .max_results = 10,
        .max_time = UB_SECONDS(10),
        .passive_scanning = false,
        .scan_parameters = { 0 }, /* use defaults */
        .filter_duplicates = true,
    };
    status = ub_bt_le_scan(&scan_args, bt_le_scan_callback);
    if (status != UB_STATUS_SUCCESS)
    {
        ub_ustdio_printf("Failed to start BLE scan: %u\n", status);
    }
    else
    {
        ub_ustdio_printf("Started scan successfully\n");
    }
}

/**
 * Handle WLAN scan results passed back by ubiquiOS
 */
static void
scan_callback(ub_status_t status, ub_wlan_bss_list_t *bss_list)
{
    const ub_wlan_bss_list_item_t *item;

    if (status != UB_STATUS_SUCCESS)
    {
        ub_ustdio_printf("Scan failed with result code %d\n", status);
        return;
    }
    ub_ustdio_printf("found %d unique SSIDs\n", bss_list->unique_ssid_count);

    /* Walk through the result list and display the results */
    item = bss_list->head;
    while (item)
    {
        ub_ustdio_printf("  %t\t(bssid=%m, ch=%2d, rssi=%d, security=",
                         item->ssid, item->ssid_length,
                         item->bssid, item->channel, item->rssi);

        switch (item->security_type)
        {
        case UB_WLAN_SECURITY_OPEN:
            ub_ustdio_printf("open");
            break;
        case UB_WLAN_SECURITY_WEP:
            ub_ustdio_printf("wep");
            break;
        case UB_WLAN_SECURITY_WPA_PSK:
            ub_ustdio_printf("wpa psk");
            break;
        case UB_WLAN_SECURITY_WPA2_PSK:
            ub_ustdio_printf("wpa2 psk");
            break;
        case UB_WLAN_SECURITY_WPA_ENTERPRISE:
            ub_ustdio_printf("wpa enterprise");
            break;
        case UB_WLAN_SECURITY_WPA2_ENTERPRISE:
            ub_ustdio_printf("wpa2 enterprise");
            break;
        }

        ub_ustdio_printf(")\n");
        item = item->next;
    }

    ub_ustdio_printf("Scan complete.\n");

    ub_wlan_free_bss_list(bss_list);
}

#if TEST_WIFI
static void wlan_power_on_callback(ub_status_t status_code)
{
    if (status_code == UB_STATUS_SUCCESS)
    {
        const uint8_t* mac = ub_wlan_network_interface->get_mac_addr ();
        //strcpy(G_WIFI_MAC, (const char*)mac);
        ub_ustdio_sprintf(G_WIFI_MAC, "%m", ub_wlan_network_interface->get_mac_addr ());

        ub_ustdio_printf ("WLAN power on completed successfully. MAC address=%m\n",
                          ub_wlan_network_interface->get_mac_addr ());
        G_WiFi_PoweredON = ETrue;
            /* Start a scan for visible access points */
    //const ub_wlan_scan_args_t wlan_scan_args = {
    //    .ssid = { 0 },
    //    .ssid_length = 0,
    //    .max_scan_results = 0
    //};
    //ub_status_t status = ub_wlan_scan(&wlan_scan_args, scan_callback);
    
    }
    else
    {
        ub_ustdio_printf ("WLAN power on failed with code %u\n", status_code);
        G_WiFi_PoweredON = EFalse;
    }
}

static void bcm_wlan(void)
{
    ub_wlan_init_args_t wlan_init_args;
    ub_wlan_bcmfmac_init_args_t * wlan_driver_init_args = (ub_wlan_bcmfmac_init_args_t *) ub_wlan_bcmfmac_4343w_args;
    //const ub_wlan_scan_args_t wlan_scan_args = {
    //        .ssid = { 0 },
    //        .ssid_length = 0,
    //        .max_scan_results = 0 };

    ub_ustdio_printf("Using Broadcom WiFi\n");
    memset(&wlan_init_args, 0, sizeof(wlan_init_args));

    ub_wlan_init(&wlan_init_args, wlan_driver_init_args);

    ub_wlan_power_on (wlan_power_on_callback);
}
#endif

#if TEST_BLE
#define LOCALNAME   "ubiquiOS"
/**
 * The UUID for a service that we advertise support for.
 * This was generated using the UUID generator found here:
 * http://www.itu.int/en/ITU-T/asn1/Pages/UUID/generate_uuid.aspx
 */
#define SERVICE_UUID                                                    \
    {  0x09, 0x2a, 0x46, 0xe0, 0xd7, 0x3d, 0x11, 0xe4,                  \
       0x8e, 0x5e, 0x00, 0x02, 0xa5, 0xd5, 0xc5, 0x1b, }

static void bt_power_on_callback(ub_status_t status)
{
    if (status == UB_STATUS_SUCCESS) {
        const uint8_t *bd_addr = ub_bt_get_bd_addr();

        //strcpy(G_BLE_MAC, (const char*)bd_addr);
        ub_ustdio_sprintf(G_BLE_MAC, "%m", bd_addr);

        ub_ustdio_printf("Bluetooth powered on successfully. BD_ADDR=%m\n",
                bd_addr);
        //if (BLE_MODE == BLE_MODE_OBSERVER)
        //    start_scan();
        //else
            //start_advertising();
        G_BLE_PoweredON = ETrue;
    } else {
        ub_ustdio_printf("Bluetooth power on failed with code %u\n", status);
    }
}

static void power_on_bt(void)
{
    ub_bt_init_args_t bt_init_args;
    ub_bt_bcm434x_init_args_t bt_driver_init_args;
    ub_status_t status;
    
    memset(&bt_init_args, 0, sizeof(ub_bt_init_args_t));
    memset(&bt_driver_init_args, 0, sizeof(ub_bt_bcm434x_init_args_t));    
    
    bt_driver_init_args.hcd = &ub_bt_hcd_bcm4343w; // required?
    bt_init_args.device_name = LOCALNAME;
    bt_init_args.class_of_device = 0; // zero is default and already set

    ub_bt_init(&bt_init_args, &bt_driver_init_args);

    status = ub_bt_power_on(bt_power_on_callback);
    if (status != UB_STATUS_SUCCESS) {
        ub_ustdio_printf("Failed to power on BT: %u\n", status);
    } else {
        ub_ustdio_printf("BT powering on\n");
    }
}
#endif

static void sleep_init(void)
{

    if (UEZQueueCreate(4, 4, &G_sleep_queue) != UEZ_ERROR_NONE) {
        ub_ustdio_printf("Failed to initialize sleep queue\n");
    }
}

void wake_ubiquios_thread(void)
{
    /* Not a lot to do if we're already awake */
    if (!G_is_sleeping)
        return;

    TUInt32 wake_val = 1;
    /* Try prod ubiquiOS to wake it up */
    if (_isr_UEZQueueSend(G_sleep_queue, (void*)&wake_val) != UEZ_ERROR_NONE) {
        ub_ustdio_printf("Failed to wake ubiquiOS thread\n");
    }
}

/**
 *  Puts ubiquiOS thread asleep until the next event occurs (or we are woken)
 */
static void sleep_ubiquios_thread(TUInt32 wakeup_time)
{
    //enter_critical_section_threadx();

    /* ThreadX takes a number of ticks, not a time
     * Note: This assumes that 1 tick == 1ms. If this
     * is incorrect, a conversion will need to take place */
    TUInt32 sleep_time = (TUInt32)ub_time_sub(wakeup_time, ub_time_now());

    /* Throwaway message queue receive value */
    TUInt32 wake_val = 0;

    /* We don't want to go to sleep if we received a
     * wake request before entering the critical section */
    if (ub_main_hal_is_wake_pending() || ub_time_lt(wakeup_time, ub_time_now())
            || sleep_time == 0) {
        //exit_critical_section_threadx();
        return;
    }

    G_is_sleeping = ETrue;
    switch (UEZQueueReceive(G_sleep_queue, &wake_val, (TUInt32)sleep_time)) {
        case UEZ_ERROR_NONE:
            /* All is well */
            break;
        default:
            //ub_ustdio_printf("Failed to read from sleep queue\n");
          break;
    }

    G_is_sleeping = EFalse;
    //exit_critical_section_threadx();
}

static TUInt32 ubiquios_Test(T_uezTask aMyTask, void *aParams)
{
    TUInt32 next_event_time;
    printf("\nStarting UB version: %u.%u-%u\n", UB_VERSION_MAJOR, UB_VERSION_MINOR, UB_BUILD_ID);

    sleep_init();

    ub_init();
        
    /* We're using Bluetooth and WLAN - we must initialise coexistence */
   // ub_coex_init(); // adding this function call makes no difference for WLAN reconnect issue

#if TEST_WIFI
    bcm_wlan();
#endif
#if TEST_BLE
    power_on_bt();
#endif

    // Here is where you control which interfaces are turned on, and which is primary, second, third, etc.
    // Note that when using UB_TCPIP_ADD_IFGROUP DHCP is automatically set
UB_TCPIP_ADD_IFGROUP(UB_TCPIP_PRIMARY_IFGROUP_ID,
#if (TEST_WIFI == 1)
      ub_wlan_network_interface
#endif
#if (TEST_CELLUAR == 1)
      , ub_cellular_network_interface
#endif
    ); // enable multiple interface switch with 1, 2, or 3 interfaces
    //ub_tcpip_register_link_state_change_callback(UB_TCPIP_PRIMARY_IFGROUP_ID, network_state_change_callback);
    
    //ub_main();
    /* Execute timer main. */
    while (1) {
        ub_timer_main();

        /* We can go to sleep if we have no events planned immediately */
        if (ub_timer_next_event_time(&next_event_time))
            sleep_ubiquios_thread(next_event_time);
        else
            sleep_ubiquios_thread(ub_time_add_now(UB_TIMER_MAX_DURATION));
    }
}

static void Power_On_Off_BLE_WIFI(TBool aOn)
{
    static TBool runOnce = EFalse;
    TUInt32 config;

    if(!runOnce){
        runOnce = ETrue;
#if(UEZ_PROCESSOR == NXP_LPC4357)
    config = SCU_NORMAL_DRIVE_DEFAULT((SHARED_POWER_PIN >> 8) > 4 ? 4 : 0);
#else
#error "MCU not set!"
#endif
    UEZGPIOControl(SHARED_POWER_PIN, GPIO_CONTROL_SET_CONFIG_BITS, config);
#if (BCM_POWER_POL_AL == 1)
    UEZGPIOSet(SHARED_POWER_PIN); //Power off
#else
    UEZGPIOClear(SHARED_POWER_PIN); //Power off
#endif
    UEZGPIOOutput(SHARED_POWER_PIN);
    }
}

void Ubiquios_Start()
{
    static TBool haveRun = EFalse;

    if(!haveRun){
        haveRun = ETrue;

        Power_On_Off_BLE_WIFI(EFalse);
        UEZTaskDelay(500);
        Power_On_Off_BLE_WIFI(ETrue);

        UEZTaskCreate(ubiquios_Test, "Ubiquios", 10*1024, (void *)0,
                UEZ_PRIORITY_NORMAL, 0);
    }

}

/*-------------------------------------------------------------------------*
 * End of File:  UbiquiOS_Test.c
 *-------------------------------------------------------------------------*/
