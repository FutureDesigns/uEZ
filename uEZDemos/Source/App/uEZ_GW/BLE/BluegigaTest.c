/*-------------------------------------------------------------------------*
 * File:  BluegigaTest.c
 *-------------------------------------------------------------------------*
 * Description:
 *     
 *-------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * uEZ(R) - Copyright (C) 2007-2012 Future Designs, Inc.
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

/*-------------------------------------------------------------------------*
 * Includes:
 *-------------------------------------------------------------------------*/
#include <uEZ.h>
#include <uEZStream.h>
#include <types\Serial.h>
#include <UEZProcessor.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include "Leica_Callbacks.h"
#include "cmd_def.h"
#include "NVSettings.h"
#include "BluegigaTest.h"

/*-------------------------------------------------------------------------*
 * Constants:
 *-------------------------------------------------------------------------*/
#define FIRST_HANDLE 0x0001
#define LAST_HANDLE  0xffff

#define MAX_DEVICES 64

//#define DEBUG_BLE   1
#define RAW     0

#ifdef DEBUG_BLE
#define BLEprintf printf
#else
#define BLEprintf
#endif
/*-------------------------------------------------------------------------*
 * GLobals:
 *-------------------------------------------------------------------------*/
static T_uezDevice G_uart;

int found_devices_count = 0;
static bd_addr found_devices[MAX_DEVICES];

static bd_addr connect_addr;
TBool G_BLE_Device_Found = EFalse;
char G_BG_MAC[20] = "\0";

uint8 primary_service_uuid[] = {0x00, 0x28};
/*-------------------------------------------------------------------------*
 * Prototypes:
 *-------------------------------------------------------------------------*/
typedef enum {
    state_disconnected,
    state_connecting,
    state_connected,
    state_finding_services,
    state_finding_attributes,
    state_listening_measurements,
    state_finish,
    state_last
} states;
states state = state_disconnected;

const char *state_names[state_last] = {
    "disconnected",
    "connecting",
    "connected",
    "finding_services",
    "finding_attributes",
    "listening_measurements",
    "finish"
};

enum actions {
    action_none,
    action_scan,
    action_connect,
    action_info,
};
enum actions action = action_none;

static TUInt16 G_DistanceHandle = 0;
static TUInt16 G_DisplayUnitHandle = 0;
static TUInt16 G_InclinationHandle = 0;
static TUInt16 G_InclinationUnitHandle = 0;
static TUInt16 G_DistoModelHandle = 0;
static TUInt16 G_SerialHandle = 0;
//static TUInt16 G_TempHandle = 0;

static char G_D810_SerialNumber[20];

static void change_state(states new_state)
{
    //T_LeicaData settings;
#ifdef DEBUG_BLE
    BLEprintf("\nDEBUG: State changed: %s --> %s\n", state_names[state], state_names[new_state]);
#endif
    state = new_state;

    //Leica_Callbacks_GetSettings(&settings);
//    if(state == state_listening_measurements || state == state_connected){
//        settings.iConnected = ETrue;
//    } else {
//        settings.iConnected = EFalse;
//    }
//    if(G_D810_SerialNumber[0] != '\0'){
//        strcpy((char*)settings.iSerialNumber, G_D810_SerialNumber);
//    }
    //Leica_Callbacks_SetSettings(&settings);
}

static int cmp_bdaddr(bd_addr first, bd_addr second)
{
    int i;
    for (i = 0; i < sizeof(bd_addr); i++) {
        if (first.addr[i] != second.addr[i]) return 1;
    }
    return 0;
}

static void print_bdaddr(bd_addr bdaddr)
{
#if DEBUG_BLE
    BLEprintf("%02x:%02x:%02x:%02x:%02x:%02x",
            bdaddr.addr[5],
            bdaddr.addr[4],
            bdaddr.addr[3],
            bdaddr.addr[2],
            bdaddr.addr[1],
            bdaddr.addr[0]);
#endif
}

static void print_raw_packet(struct ble_header *hdr, unsigned char *data)
{
#if RAW
    TUInt32 start = UEZTickCounterGet();
    BLEprintf("%d Incoming packet: ", start);
    int i;
    for (i = 0; i < sizeof(*hdr); i++) {
        BLEprintf("%02x ", ((unsigned char *)hdr)[i]);
    }
    for (i = 0; i < hdr->lolen; i++) {
        BLEprintf("%02x ", data[i]);
    }
    BLEprintf("\n");
#endif
}

static void print_raw_output(uint8 len1, uint8* data1, uint16 len2, uint8* data2)
{
#if RAW
    TUInt32 start = UEZTickCounterGet();
    TUInt32 i;

    BLEprintf("%d Out going:\n", start);
    BLEprintf("    Data1: ");
    for(i = 0; i < len1; i++){
        BLEprintf("%02x ", data1[i]);
    }
    BLEprintf("\n");
    BLEprintf("    Data2: ");
    for(i = 0; i < len2; i++){
        BLEprintf("%02x ", data2[i]);
    }
    BLEprintf("\n");
#endif
}

static void output(uint8 len1, uint8* data1, uint16 len2, uint8* data2)
{
    T_uezError error;
    TUInt32 bytesWritten;
    TUInt32 len = len1 + len2;

    error = UEZStreamWrite(G_uart, (void*)&len, 1, &bytesWritten, 1000);
    error = UEZStreamWrite(G_uart, data1, (TUInt32)len1, &bytesWritten, 1000);
    error = UEZStreamWrite(G_uart, data2, (TUInt32)len2, &bytesWritten, 1000);
    print_raw_output(len1, data1, len2, data2);
    UEZStreamFlush(G_uart);

    if(error){
#if DEBUG_BLE
        BLEprintf("error\n");
#endif
    }
}

static int read_message(int timeout_ms)
{
    TUInt8 data[255];
    struct ble_header hdr;
    TUInt32 bytesRead;

    if(UEZStreamRead(G_uart, (void*)&hdr, sizeof(hdr), &bytesRead, timeout_ms) == UEZ_ERROR_NONE){
        G_BLE_Device_Found = ETrue;
        if(UEZStreamRead(G_uart, (void*)data, hdr.lolen, &bytesRead, timeout_ms) == UEZ_ERROR_NONE){
            const struct ble_msg *msg = ble_get_msg_hdr(hdr);

            if(msg && msg->handler){
                msg->handler(data);

            }
            print_raw_packet(&hdr, data);
        }
    } else {
        return 1;
    }

    return 0;
}

void ble_rsp_system_get_info(const struct ble_msg_system_get_info_rsp_t *msg)
{
#if DEBUG_BLE
    BLEprintf("Build: %u, protocol_version: %u, hardware: ", msg->build, msg->protocol_version);
    switch (msg->hw) {
    case 0x01: BLEprintf("BLE112"); break;
    case 0x02: BLEprintf("BLED112"); break;
    default: BLEprintf("Unknown");
    }
    BLEprintf("\n");
#endif
    G_BLE_Device_Found = ETrue;
}

void ble_evt_gap_scan_response(const struct ble_msg_gap_scan_response_evt_t *msg)
{
    if (found_devices_count >= MAX_DEVICES) change_state(state_finish);

    int i, j;
    char *name = NULL;

    // check if this device already found (only happens in "scan" action)
    for (i = 0; i < found_devices_count; i++) {
        if (!cmp_bdaddr(msg -> sender, found_devices[i])) return;
    }

    // continue by parsing advertisement fields (see BT4.0 spec for ad field type data)
    for (i = 0; i < msg -> data.len; ) {
        int8 len = msg -> data.data[i++];
        if (!len) continue;
        if (i + len > msg -> data.len) break; // not enough data, incomplete field
        uint8 type = msg -> data.data[i++];
        switch (type) {
            case 0x01:
                // flags field
                break;

            case 0x02:
                // partial list of 16-bit UUIDs
            case 0x03:

                break;

            case 0x04:
                // partial list of 32-bit UUIDs
            case 0x05:
                // complete list of 32-bit UUIDs
                for (j = 0; j < len - 1; j += 4)
                {
                    // loop through UUIDs 4 bytes at a time
                    // TODO: test for desired UUID here, if 32-bit UUID
                }
                break;

            case 0x06:
                // partial list of 128-bit UUIDs
            case 0x07:
                // complete list of 128-bit UUIDs
                for (j = 0; j < len - 1; j += 16)
                {
                    // loop through UUIDs 16 bytes at a time
                    // TODO: test for desired UUID here, if 128-bit UUID
                }
                break;
            case 0x09:
                name = malloc(len);
                memcpy(name, msg->data.data + i, len - 1);
                name[len - 1] = '\0';
        }

        i += len - 1;
    }

    memcpy(found_devices[found_devices_count].addr, msg -> sender.addr, sizeof(bd_addr));
    found_devices_count++;

    print_bdaddr(msg -> sender);
#if DEBUG_BLE
    BLEprintf(" RSSI=%d", (int8)(msg->rssi));
    BLEprintf(" Name=");
#endif
    if (name){
#if DEBUG_BLE
        BLEprintf("%s", name);
#endif
        //if(strcmp(name, "DISTO") == 0){
        char *p = strstr(name, "DISTO");
        if(p){
            memcpy(connect_addr.addr, msg -> sender.addr, sizeof(bd_addr));
            change_state(state_connecting);
            ble_cmd_gap_connect_direct(&connect_addr, msg -> address_type, 32, 48, 100, 0);
        }
        p += strlen("DISTO");
        if(*p == ' '){
            p++;
            strcpy(G_D810_SerialNumber, p);
        } else {
            memset((void*)G_D810_SerialNumber, 0, sizeof(G_D810_SerialNumber));
        }
    }else {
#if DEBUG_BLE
        BLEprintf("Unknown");
#endif
    }
#if DEBUG_BLE
    BLEprintf(" AddrType=");

    if (msg -> address_type) BLEprintf("RANDOM");
    else BLEprintf("PUBLIC");
    BLEprintf("\n");
#endif
    free(name);
}

void enable_indications(uint8 connection_handle, uint16 client_configuration_handle)
{
    uint8 configuration[] = {0x02, 0x00}; // enable indications
    ble_cmd_attclient_attribute_write(connection_handle, client_configuration_handle, 2, &configuration);
}

void ble_evt_connection_status(const struct ble_msg_connection_status_evt_t *msg)
{
    if(msg->flags&connection_connected)
    {
        action = action_connect;
        change_state(state_connected);
#if DEBUG_BLE
        BLEprintf("#connected\n");
#endif
        
        change_state(state_finding_services);
        ble_cmd_attclient_read_by_group_type(msg->connection, FIRST_HANDLE, LAST_HANDLE, 2, primary_service_uuid);

    } else{
        change_state(state_disconnected);
#if DEBUG_BLE
        BLEprintf("#Not connected -> Scan\n");
#endif
        action = action_scan;
        found_devices_count = 0;
        memset((void*)found_devices, 0, sizeof(found_devices));
            ble_cmd_gap_discover(gap_discover_generic);
        G_DistanceHandle = 0;
        G_DisplayUnitHandle = 0;
    }
}

void ble_evt_attclient_group_found(const struct ble_msg_attclient_group_found_evt_t *msg)
{
    if (msg->uuid.len == 0) return;
#if DEBUG_BLE
    uint16 uuid = (msg->uuid.data[1] << 8) | msg->uuid.data[0];


    BLEprintf("%04X\n", uuid);
#endif
}

void ble_rsp_attributes_read(const struct ble_msg_attributes_read_rsp_t *msg)
{
#if DEBUG_BLE
    TUInt8 i;
    BLEprintf("Handle %d ", msg->handle);

    for(i = 0; i < msg->value.len; i++){
        BLEprintf("%02x ", msg->value.data[i]);
    }
    BLEprintf("\n");

#endif
}

void ble_evt_attclient_attribute_value(const struct ble_msg_attclient_attribute_value_evt_t *msg)
{
    //T_LeicaData settings;

#if DEBUG_BLE
        TUInt32 i;
        BLEprintf("Handle %d ", msg->atthandle);

        for(i = 0; i < msg->value.len; i++){
            BLEprintf("%02x ", msg->value.data[i]);
        }
        BLEprintf("\n");
#endif

        //Leica_Callbacks_GetSettings(&settings);
//        if(msg->atthandle == G_SerialHandle){
//            strcpy((char*)settings.iSerialNumber, (char*)msg->value.data);
//        } else if(msg->atthandle == G_DistanceHandle){
//            settings.iDistanceInM = *(float*)&msg->value.data;
//        } else if(msg->atthandle == G_DisplayUnitHandle){
//            settings.iDisplayUnits = msg->value.data[0];
//        } else if(msg->atthandle == G_InclinationHandle){
//            settings.iInclinationInRad = *(float*)&msg->value.data;
//        } else if(msg->atthandle == G_InclinationUnitHandle){
//            settings.iInclinationDisplayUnit = msg->value.data[0];
//        } else if(msg->atthandle == G_TempHandle){
//            settings.iTemperatureInC = *(float*)&msg->value.data;
//        } else if(msg->atthandle == G_DistoModelHandle){
//            settings.iDistoModelNumber = msg->value.data[0];
//        }

    //Leica_Callbacks_SetSettings(&settings);
}

void ble_rsp_attclient_find_information(const struct ble_msg_attclient_find_information_rsp_t *msg)
{
#if DEBUG_BLE
    BLEprintf("Find: %d\n", msg->result);
#endif
}

void ble_evt_attclient_find_information_found(const struct ble_msg_attclient_find_information_found_evt_t *msg)
{
#if DEBUG_BLE
    TUInt8 i;
    BLEprintf("Found: %d UUID: ", msg->chrhandle);

    for(i = 0; i < msg->uuid.len; i ++){
        BLEprintf("%02x ", msg->uuid.data[i]);
    }
    BLEprintf("\n");
#endif
    if(msg->uuid.len == 16){
        if(msg->uuid.data[12] == 0x01){
            G_DistanceHandle = msg->chrhandle;
#if DEBUG_BLE
            BLEprintf("Distance Handle: %d\n", G_DistanceHandle);
#endif
        } else if(msg->uuid.data[12] == 0x02){
            G_DisplayUnitHandle = msg->chrhandle;
        } else if(msg->uuid.data[12] == 0x03){
            G_InclinationHandle = msg->chrhandle;
        } else if(msg->uuid.data[12] == 0x04){
            G_InclinationUnitHandle = msg->chrhandle;
        } else if(msg->uuid.data[12] == 0x05){
            G_DistoModelHandle = msg->chrhandle;
        }
    }
    if((msg->uuid.data[0] == 0x25) &&(msg->uuid.data[1] == 0x2A)){
        G_SerialHandle = msg->chrhandle;
    } else if((msg->uuid.data[0] == 0x1C) &&(msg->uuid.data[1] == 0x2A)){
        //G_TempHandle = msg->chrhandle;
    } else if((msg->uuid.data[0] == 0x24) &&(msg->uuid.data[1] == 0x2A)){
        //G_MaodelNumberHandle = msg->chrhandle;
    }
}

TUInt8 G_DistanceUUID[16] = { 0x94, 0xbf, 0xd5, 0x77, 0x09, 0x57, 0x9d, 0xb2, 0x95, 0x43, 0x31, 0xf8, 0x01, 0x01, 0xb1, 0x3a};
void ble_evt_attclient_procedure_completed(const struct ble_msg_attclient_procedure_completed_evt_t *msg)
{
#if DEBUG_BLE
    BLEprintf("Complete: %X\n", msg->result);
#endif
    if(state == state_finding_services){

            change_state(state_finding_attributes);
            //ble_cmd_attclient_find_information(msg->connection, DISTO_Handle_Start, DISTO_Handle_End);
            ble_cmd_attclient_find_information(msg->connection, FIRST_HANDLE, LAST_HANDLE);

    } else if(state == state_finding_attributes){
        if(G_DistanceHandle){
            change_state(state_listening_measurements);

            //ble_cmd_sm_encrypt_start(0, 1);
            //read_message(1000);
            //read_message(1000);

            //printf("Enable Indications: Connection %d, Handle %d\n", msg->connection, G_DistanceHandle);
            UEZTaskDelay(10);
            enable_indications(msg->connection, G_DistanceHandle+1);
            read_message(1000);
            read_message(1000);

            //printf("Enable Indications: Connection %d, Handle %d\n", msg->connection, G_DisplayUnitHandle);
            enable_indications(msg->connection, G_DisplayUnitHandle+1);
            read_message(1000);
            read_message(1000);

            //printf("Enable Indications: Connection %d, Handle %d\n", msg->connection, G_InclinationHandle);
            enable_indications(msg->connection, G_InclinationHandle+1);
            read_message(1000);
            read_message(1000);

            //printf("Enable Indications: Connection %d, Handle %d\n", msg->connection, G_InclinationUnitHandle);
            enable_indications(msg->connection, G_InclinationUnitHandle+1);
            read_message(1000);
            read_message(1000);

            //printf("Enable Indications: Connection %d, Handle %d\n", msg->connection, G_TempHandle);
            //enable_indications(msg->connection, G_TempHandle+1);
            //read_message(1000);
            //read_message(1000);
        }
    }
}

void ble_rsp_attclient_read_by_handle(const struct ble_msg_attclient_read_by_handle_rsp_t *msg)
{
#if DEBUG_BLE
    BLEprintf("Read: %X\n", msg->result);
#endif
}

TBool forever = ETrue;

void ble_rsp_system_address_get(const struct ble_msg_system_address_get_rsp_t *msg)
{
    //BLEprintf("ble_rsp_system_address_get\n");
    sprintf(G_BG_MAC, "%02X:%02X:%02X:%02X:%02X:%02X",
            msg->address.addr[5],
            msg->address.addr[4],
            msg->address.addr[3],
            msg->address.addr[2],
            msg->address.addr[1],
            msg->address.addr[0]);
    
    forever = EFalse;
}

static TUInt32 Bluegiga_Run(T_uezTask aMyTask, void *aParams)
{
    TBool oncePerConnection = EFalse;
    T_Serial_Settings settings;

    settings.iBaud = 9600;
    settings.iWordLength = 8;
    settings.iStopBit = SERIAL_STOP_BITS_1;
    settings.iParity = SERIAL_PARITY_NONE;
    settings.iFlowControl = SERIAL_FLOW_CONTROL_NONE;

    UEZGPIOSet(GPIO_P7_0);
    UEZGPIOSet(GPIO_P7_1);

    UEZGPIOSetMux(GPIO_P7_0, 4);
    UEZGPIOControl(GPIO_P7_0, GPIO_CONTROL_SET_CONFIG_BITS, SCU_NORMAL_DRIVE_DEFAULT(4));
    UEZGPIOSetMux(GPIO_P7_1, 4);
    UEZGPIOControl(GPIO_P7_1, GPIO_CONTROL_SET_CONFIG_BITS, SCU_NORMAL_DRIVE_DEFAULT(4));
    UEZGPIOOutput(GPIO_P7_0);
    UEZGPIOOutput(GPIO_P7_1);

#define CC_DEBUGGER 0
#if CC_DEBUGGER
    UEZGPIOSetMux(GPIO_P5_8, 4);
    UEZGPIOControl(GPIO_P5_8, GPIO_CONTROL_SET_CONFIG_BITS, SCU_NORMAL_DRIVE_DEFAULT(4));
    UEZGPIOSetMux(GPIO_P5_9, 4);
    UEZGPIOControl(GPIO_P5_9, GPIO_CONTROL_SET_CONFIG_BITS, SCU_NORMAL_DRIVE_DEFAULT(4));
    UEZGPIOInput(GPIO_P5_9);
    UEZGPIOInput(GPIO_P5_8);
    
    UEZGPIOSetMux(GPIO_P7_0, 4);
    UEZGPIOControl(GPIO_P7_0, GPIO_CONTROL_SET_CONFIG_BITS, SCU_NORMAL_DRIVE_DEFAULT(4));
    UEZGPIOSetMux(GPIO_P7_1, 4);
    UEZGPIOControl(GPIO_P7_1, GPIO_CONTROL_SET_CONFIG_BITS, SCU_NORMAL_DRIVE_DEFAULT(4));
    UEZGPIOInput(GPIO_P7_0);
    UEZGPIOInput(GPIO_P7_1);
    
    UEZGPIOSetMux(BLE_RESET_PIN, 0);
    UEZGPIOControl(BLE_RESET_PIN, GPIO_CONTROL_SET_CONFIG_BITS, SCU_NORMAL_DRIVE_DEFAULT(4));
    while(1){
      UEZTaskDelay(1000);
    }
#else

#endif

#ifdef DEBUG_BLE
    BLEprintf("BLE Task\n");
#endif
    if(UEZStreamOpen("UART0", &G_uart) == UEZ_ERROR_NONE){ 
        UEZStreamControl(G_uart, STREAM_CONTROL_SET_SERIAL_SETTINGS, &settings);
        bglib_output = output;

        //Reset the module
        UEZGPIOSetMux(BLE_RESET_PIN, 0);
        UEZGPIOControl(BLE_RESET_PIN, GPIO_CONTROL_SET_CONFIG_BITS, SCU_NORMAL_DRIVE_DEFAULT(0));
        UEZGPIOSet(BLE_RESET_PIN);
        UEZGPIOOutput(BLE_RESET_PIN);
        UEZTaskDelay(500);
        UEZGPIOClear(BLE_RESET_PIN);
        UEZTaskDelay(500);
        UEZGPIOSet(BLE_RESET_PIN);
        UEZTaskDelay(1000);

        read_message(1000);

        //ble_cmd_system_reset(0);
        //read_message(1000);

        ble_cmd_system_get_info();
        read_message(1000);

        ble_cmd_system_address_get();
        read_message(1000);

        //stop previous operation
        ble_cmd_gap_end_procedure();
        read_message(1000);

        action = action_scan;

        ble_cmd_gap_discover(gap_discover_observation);

        //works to turn on LED but causes COM issues.
        //ble_cmd_hardware_io_port_config_direction(0, 1);
        //ble_cmd_hardware_io_port_write(0, (1<<1), (1<<1));
        //ble_cmd_hardware_io_port_write(0, (1<<0), (0<<1));

        while(forever){

            if(state == state_listening_measurements){// && G_Complete){// && G_DistanceHandle){
                //UEZTaskDelay(1000);
                //if( i < 55){
                //    ble_cmd_attclient_read_by_handle(0, i);
                //    i++;
                //}
                //ble_cmd_attclient_read_by_handle(0, G_DistanceHandle);
                if(!oncePerConnection){
                    oncePerConnection = ETrue;

                    ble_cmd_attclient_read_by_handle(0, G_DistoModelHandle);
                    read_message(1000);
                    read_message(1000);
                    ble_cmd_attclient_read_by_handle(0, G_SerialHandle);
                }
            } else {
                oncePerConnection = EFalse;
            }
            read_message(1000);
            read_message(1000);
            read_message(1000);
            read_message(1000);
        }
    }
    UEZStreamClose(G_uart);
    InterruptUnregister(24);
    
    LPC_CGU->BASE_UART0_CLK = 1;

    //Change the Pins over to the Laird BLE
    UEZGPIOSetMux(GPIO_P7_24, 4); //RXD BLE121
    UEZGPIOSetMux(GPIO_P7_25, 4); //TXD BLE121
    
    UEZGPIOSetMux(GPIO_P4_11, 7); //RXD Laird
    UEZGPIOSetMux(GPIO_P5_18, 7); //TXD Laird    
    
    UEZTaskDelay(1);
    LPC_CGU->BASE_UART0_CLK = (9<<24) | (1<<11);

    Ubiquios_Start();
    return 0;
}

void Bluegiga_Start()
{
    static TBool haveRun = EFalse;

    if(!haveRun){
        haveRun = ETrue;
        UEZTaskCreate(Bluegiga_Run, "BLE", (100 * 1024), 0, UEZ_PRIORITY_NORMAL, 0);
    }
}

void Bluegiga_disconnect()
{
    if(state != state_disconnected){
        ble_cmd_connection_disconnect(0);
        //read_message(1000);
    }
}

void Bluegiga_discover()
{
    if(state == state_disconnected){
        ble_cmd_gap_end_procedure();
        ble_cmd_gap_discover(gap_discover_generic);
        //read_message(1000);
    }
}
/*-------------------------------------------------------------------------*
 * End of File:  BluegigaTest.c
 *-------------------------------------------------------------------------*/
