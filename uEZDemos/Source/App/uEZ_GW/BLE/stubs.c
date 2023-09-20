//
// Bluegiga’s Bluetooth Smart Demo Application
// Contact: support@bluegiga.com.
//
// This is free software distributed under the terms of the MIT license reproduced below.
//
// Copyright (c) 2012, Bluegiga Technologies
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this
// software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "cmd_def.h"

#define debug       0

#if debug
#define dprintf printf
#else
#define dprintf(...)
#endif

void ble_default(const void*v)
{
    dprintf("ble_default\n");
}

void ble_rsp_hardware_set_rxgain(const void*nul)
{
    dprintf("ble_rsp_hardware_set_rxgain\n");
}

void ble_rsp_system_aes_setkey(const void*nul)
{
    dprintf("ble_rsp_system_aes_setkey\n");
}

void ble_rsp_system_aes_encrypt(const struct ble_msg_system_aes_encrypt_rsp_t*msg)
{
    dprintf("ble_rsp_system_aes_encrypt\n");
}

void ble_rsp_system_aes_decrypt(const struct ble_msg_system_aes_decrypt_rsp_t*msg)
{
    dprintf("ble_rsp_system_aes_decrypt\n");
}

void ble_rsp_flash_read_data(const struct ble_msg_flash_read_data_rsp_t*msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_test_channel_mode(const void*nul)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_evt_system_protocol_error(const struct ble_msg_system_protocol_error_evt_t*msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_evt_system_no_license_key(const void*nul)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_evt_attributes_status(const struct ble_msg_attributes_status_evt_t *msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_attclient_indicate_confirm(const struct ble_msg_attclient_indicate_confirm_rsp_t*msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_test_debug(const struct ble_msg_test_debug_rsp_t*msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_hardware_set_txpower(const void *nul)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_attclient_read_multiple(const struct ble_msg_attclient_read_multiple_rsp_t *msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_evt_attclient_read_multiple_response(const struct ble_msg_attclient_read_multiple_response_evt_t *msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_attclient_prepare_write(const struct ble_msg_attclient_prepare_write_rsp_t *msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_attclient_execute_write(const struct ble_msg_attclient_execute_write_rsp_t *msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_evt_attributes_user_read_request(const struct ble_msg_attributes_user_read_request_evt_t *msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_attributes_user_read_response(const void * nul)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_attributes_user_write_response(const void * nul)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_system_whitelist_clear(const void *nul)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_attclient_write_command(const struct ble_msg_attclient_write_command_rsp_t *msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_attclient_reserved(const void *nul)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_attclient_read_long(const struct ble_msg_attclient_read_long_rsp_t *msg)
{
    dprintf("ble_evt_system_protocol_error\n");
}

void ble_rsp_system_whitelist_append(const struct ble_msg_system_whitelist_append_rsp_t *msg)
{
    dprintf("ble_rsp_system_whitelist_append\n");
}

void ble_rsp_sm_set_parameters(const void *nul)
{
    dprintf("ble_rsp_sm_set_parameters\n");
}

void ble_rsp_sm_passkey_entry(const struct ble_msg_sm_passkey_entry_rsp_t *msg)
{
    dprintf("ble_rsp_sm_passkey_entry\n");
}

void ble_rsp_sm_get_bonds(const struct ble_msg_sm_get_bonds_rsp_t *msg)
{
    dprintf("ble_rsp_sm_get_bonds\n");
}

void ble_rsp_sm_set_oob_data(const void *nul)
{
    dprintf("ble_rsp_sm_set_oob_data\n");
}

void ble_rsp_sm_whitelist_bonds(const struct ble_msg_sm_whitelist_bonds_rsp_t *msg)
{
    dprintf("ble_rsp_sm_whitelist_bonds\n");
}

void ble_rsp_sm_set_security_mode(const void *nul)
{
    dprintf("ble_rsp_sm_set_security_mode\n");
}

void ble_rsp_gap_set_filtering(const struct ble_msg_gap_set_filtering_rsp_t *msg)
{
    dprintf("ble_rsp_gap_set_filtering\n");
}

void ble_rsp_gap_set_adv_parameters(const struct ble_msg_gap_set_adv_parameters_rsp_t *msg)
{
    dprintf("ble_rsp_gap_set_adv_parameters\n");
}

void ble_rsp_hardware_io_port_write(const struct ble_msg_hardware_io_port_write_rsp_t *msg)
{
    dprintf("ble_rsp_hardware_io_port_write\n");
}

void ble_rsp_hardware_io_port_read(const struct ble_msg_hardware_io_port_read_rsp_t *msg)
{
    dprintf("ble_rsp_hardware_io_port_read\n");
}

void ble_rsp_hardware_timer_comparator(const struct ble_msg_hardware_timer_comparator_rsp_t *msg)
{dprintf("ble_rsp_hardware_timer_comparator");

}

void ble_rsp_hardware_spi_config(const struct ble_msg_hardware_spi_config_rsp_t *msg)
{dprintf("ble_rsp_hardware_spi_config\n");

}

void ble_rsp_hardware_spi_transfer(const struct ble_msg_hardware_spi_transfer_rsp_t *msg)
{dprintf("ble_rsp_hardware_spi_transfer\n");

}

void ble_rsp_hardware_i2c_read(const struct ble_msg_hardware_i2c_read_rsp_t *msg)
{dprintf("ble_rsp_hardware_i2c_read\n");

}

void ble_rsp_hardware_i2c_write(const struct ble_msg_hardware_i2c_write_rsp_t *msg)
{dprintf("ble_rsp_hardware_i2c_write\n");

}

void ble_rsp_hardware_io_port_irq_enable(const struct ble_msg_hardware_io_port_irq_enable_rsp_t *msg)
{dprintf("ble_rsp_hardware_io_port_irq_enable\n");

}

void ble_rsp_hardware_io_port_irq_direction(const struct ble_msg_hardware_io_port_irq_direction_rsp_t *msg)
{dprintf("ble_rsp_hardware_io_port_irq_direction\n");

}

void ble_rsp_hardware_analog_comparator_enable(const void *nul)
{dprintf("ble_rsp_hardware_analog_comparator_enable\n");

}

void ble_rsp_hardware_analog_comparator_read(const struct ble_msg_hardware_analog_comparator_read_rsp_t *msg)
{dprintf("ble_rsp_hardware_analog_comparator_read\n");

}

void ble_rsp_hardware_analog_comparator_config_irq(const struct ble_msg_hardware_analog_comparator_config_irq_rsp_t *msg)
{dprintf("ble_rsp_hardware_analog_comparator_config_irq\n");

}

void ble_rsp_test_get_channel_map(const struct ble_msg_test_get_channel_map_rsp_t *msg)
{dprintf("ble_rsp_test_get_channel_map\n");

}

void ble_evt_attributes_value(const struct ble_msg_attributes_value_evt_t *msg)
{dprintf("ble_evt_attributes_value\n");

}

void ble_evt_sm_bonding_fail(const struct ble_msg_sm_bonding_fail_evt_t *msg)
{dprintf("ble_evt_sm_bonding_fail\n");

}

void ble_evt_sm_passkey_display(const struct ble_msg_sm_passkey_display_evt_t *msg)
{dprintf("ble_evt_sm_passkey_display\n");

}

void ble_evt_sm_passkey_request(const struct ble_msg_sm_passkey_request_evt_t *msg)
{dprintf("ble_evt_sm_passkey_request\n");

}

void ble_evt_sm_bond_status(const struct ble_msg_sm_bond_status_evt_t *msg)
{dprintf("ble_evt_sm_bond_status\n");

}

void ble_rsp_gap_set_adv_data(const struct ble_msg_gap_set_adv_data_rsp_t *msg)
{dprintf("ble_rsp_gap_set_adv_data\n");

}

void ble_rsp_gap_set_scan_parameters(const struct ble_msg_gap_set_scan_parameters_rsp_t *msg)
{dprintf("ble_rsp_gap_set_scan_parameters\n");

}

void ble_rsp_gap_set_directed_connectable_mode(const struct ble_msg_gap_set_directed_connectable_mode_rsp_t *msg)
{dprintf("ble_rsp_gap_set_directed_connectable_mode\n");

}

void ble_rsp_hardware_io_port_config_direction(const struct ble_msg_hardware_io_port_config_direction_rsp_t *msg)
{dprintf("ble_rsp_hardware_io_port_config_direction\n");

}

void ble_rsp_hardware_io_port_config_pull(const struct ble_msg_hardware_io_port_config_pull_rsp_t *msg)
{dprintf("ble_rsp_hardware_io_port_config_pull\n");

}

void ble_rsp_hardware_io_port_config_function(const struct ble_msg_hardware_io_port_config_function_rsp_t *msg)
{dprintf("ble_rsp_hardware_io_port_config_function\n");

}

void ble_rsp_gap_set_privacy_flags(const void *nul)
{dprintf("ble_rsp_gap_set_privacy_flags\n");

}

void ble_rsp_gap_connect_selective(const struct ble_msg_gap_connect_selective_rsp_t *msg)
{dprintf("ble_rsp_gap_connect_selective\n");

}

void ble_rsp_system_whitelist_remove(const struct ble_msg_system_whitelist_remove_rsp_t *msg)
{dprintf("ble_rsp_system_whitelist_remove\n");

}

void ble_rsp_system_reset(const void* nul)
{dprintf("ble_rsp_system_reset\n");
}

void ble_rsp_system_hello(const void* nul)
{dprintf("ble_rsp_system_hello\n");
}

//void ble_rsp_system_address_get(const struct ble_msg_system_address_get_rsp_t *msg)
//{dprintf("ble_rsp_system_address_get\n");
//}

void ble_rsp_system_reg_write(const struct ble_msg_system_reg_write_rsp_t *msg)
{dprintf("ble_rsp_system_reg_write\n");
}

void ble_rsp_system_reg_read(const struct ble_msg_system_reg_read_rsp_t *msg)
{dprintf("ble_rsp_system_reg_read\n");
}

void ble_rsp_system_get_counters(const struct ble_msg_system_get_counters_rsp_t *msg)
{dprintf("ble_rsp_system_get_counters\n");
}

void ble_rsp_system_get_connections(const struct ble_msg_system_get_connections_rsp_t *msg)
{dprintf("ble_rsp_system_get_connections\n");
}

void ble_rsp_system_read_memory(const struct ble_msg_system_read_memory_rsp_t *msg)
{dprintf("ble_rsp_system_read_memory\n");
}

//void ble_rsp_system_get_info(const struct ble_msg_system_get_info_rsp_t *msg)
//{
//}

void ble_rsp_system_endpoint_tx(const struct ble_msg_system_endpoint_tx_rsp_t *msg)
{dprintf("ble_rsp_system_endpoint_tx\n");
}

void ble_rsp_flash_ps_defrag(const void* nul)
{dprintf("ble_rsp_flash_ps_defrag\n");
}

void ble_rsp_flash_ps_dump(const void* nul)
{dprintf("ble_rsp_flash_ps_dump\n");
}

void ble_rsp_flash_ps_erase_all(const void* nul)
{dprintf("ble_rsp_flash_ps_erase_all\n");
}

void ble_rsp_flash_ps_save(const struct ble_msg_flash_ps_save_rsp_t *msg)
{dprintf("ble_rsp_flash_ps_save\n");
}

void ble_rsp_flash_ps_load(const struct ble_msg_flash_ps_load_rsp_t *msg)
{dprintf("ble_rsp_flash_ps_load\n");
}

void ble_rsp_flash_ps_erase(const void* nul)
{dprintf("ble_rsp_flash_ps_erase\n");
}

void ble_rsp_flash_erase_page(const struct ble_msg_flash_erase_page_rsp_t *msg)
{dprintf("ble_rsp_flash_erase_page\n");
}

void ble_rsp_flash_write_data(const struct ble_msg_flash_write_data_rsp_t * msg)
{dprintf("ble_rsp_flash_write_data\n");
}

void ble_rsp_attributes_write(const struct ble_msg_attributes_write_rsp_t *msg)
{dprintf("ble_rsp_attributes_write\n");
}

void ble_rsp_attributes_send(const struct ble_msg_attributes_send_rsp_t *msg)
{dprintf("ble_rsp_attributes_send\n");
}

//void ble_rsp_attributes_read(const struct ble_msg_attributes_read_rsp_t *msg)
//{dprintf("NC\n");
//}

void ble_rsp_attributes_read_type(const struct ble_msg_attributes_read_type_rsp_t *msg)
{dprintf("ble_rsp_attributes_read_type\n");
}

void ble_rsp_connection_disconnect(const struct ble_msg_connection_disconnect_rsp_t *msg)
{dprintf("ble_rsp_connection_disconnect\n");
}

void ble_rsp_connection_get_rssi(const struct ble_msg_connection_get_rssi_rsp_t *msg)
{dprintf("ble_rsp_connection_get_rssi\n");
}

void ble_rsp_connection_update(const struct ble_msg_connection_update_rsp_t *msg)
{dprintf("ble_rsp_connection_update\n");
}

void ble_rsp_connection_version_update(const struct ble_msg_connection_version_update_rsp_t *msg)
{dprintf("ble_rsp_connection_version_update\n");
}

void ble_rsp_connection_channel_map_get(const struct ble_msg_connection_channel_map_get_rsp_t *msg)
{dprintf("ble_rsp_connection_channel_map_get\n");
}

void ble_rsp_connection_channel_map_set(const struct ble_msg_connection_channel_map_set_rsp_t *msg)
{dprintf("ble_rsp_connection_channel_map_set\n");
}

void ble_rsp_connection_features_get(const struct ble_msg_connection_features_get_rsp_t *msg)
{dprintf("ble_rsp_connection_features_get\n");
}

void ble_rsp_connection_get_status(const struct ble_msg_connection_get_status_rsp_t *msg)
{dprintf("ble_rsp_connection_get_status\n");
}

void ble_rsp_connection_raw_tx(const struct ble_msg_connection_raw_tx_rsp_t *msg)
{dprintf("ble_rsp_connection_raw_tx\n");
}

void ble_rsp_attclient_find_by_type_value(const struct ble_msg_attclient_find_by_type_value_rsp_t *msg)
{dprintf("ble_rsp_attclient_find_by_type_value\n");
}

void ble_rsp_attclient_read_by_group_type(const struct ble_msg_attclient_read_by_group_type_rsp_t *msg)
{dprintf("ble_rsp_attclient_read_by_group_type\n");
}

void ble_rsp_attclient_read_by_type(const struct ble_msg_attclient_read_by_type_rsp_t *msg)
{dprintf("ble_rsp_attclient_read_by_type\n");
}

//void ble_rsp_attclient_find_information(const struct ble_msg_attclient_find_information_rsp_t *msg)
//{
//}

//void ble_rsp_attclient_read_by_handle(const struct ble_msg_attclient_read_by_handle_rsp_t *msg)
//{
//}

void ble_rsp_attclient_attribute_write(const struct ble_msg_attclient_attribute_write_rsp_t *msg)
{dprintf("ble_rsp_attclient_attribute_write\n");
}

void ble_rsp_sm_encrypt_start(const struct ble_msg_sm_encrypt_start_rsp_t *msg)
{dprintf("ble_rsp_sm_encrypt_start\n");
}

void ble_rsp_sm_set_bondable_mode(const void* nul)
{dprintf("ble_rsp_sm_set_bondable_mode\n");
}

void ble_rsp_sm_delete_bonding(const struct ble_msg_sm_delete_bonding_rsp_t *msg)
{dprintf("ble_rsp_sm_delete_bonding\n");
}

void ble_rsp_gap_set_address_mode(const void* nul)
{dprintf("ble_rsp_gap_set_address_mode\n");
}

void ble_rsp_gap_set_mode(const struct ble_msg_gap_set_mode_rsp_t *msg)
{dprintf("ble_rsp_gap_set_mode\n");
}

void ble_rsp_gap_discover(const struct ble_msg_gap_discover_rsp_t *msg)
{dprintf("ble_rsp_gap_discover\n");
}

void ble_rsp_gap_connect_direct(const struct ble_msg_gap_connect_direct_rsp_t *msg)
{dprintf("ble_rsp_gap_connect_direct\n");
}

void ble_rsp_gap_end_procedure(const struct ble_msg_gap_end_procedure_rsp_t *msg)
{dprintf("ble_rsp_gap_end_procedure\n");
}

void ble_rsp_hardware_io_port_config_irq(const struct ble_msg_hardware_io_port_config_irq_rsp_t *msg)
{dprintf("ble_rsp_hardware_io_port_config_irq\n");
}

void ble_rsp_hardware_set_soft_timer(const struct ble_msg_hardware_set_soft_timer_rsp_t *msg)
{dprintf("ble_rsp_hardware_set_soft_timer\n");
}

void ble_rsp_hardware_adc_read(const struct ble_msg_hardware_adc_read_rsp_t *msg)
{dprintf("ble_rsp_hardware_adc_read\n");
}

void ble_rsp_test_phy_tx(const void* nul)
{dprintf("ble_rsp_test_phy_tx\n");
}

void ble_rsp_test_phy_rx(const void* nul)
{dprintf("ble_rsp_test_phy_rx\n");
}

void ble_rsp_test_phy_end(const struct ble_msg_test_phy_end_rsp_t *msg)
{dprintf("ble_rsp_test_phy_end\n");
}

void ble_rsp_test_phy_reset(const void* nul)
{dprintf("ble_rsp_test_phy_reset\n");
}

void ble_evt_system_boot(const struct ble_msg_system_boot_evt_t *msg)
{dprintf("ble_evt_system_boot\n");
}

void ble_evt_system_debug(const struct ble_msg_system_debug_evt_t *msg)
{dprintf("ble_evt_system_debug\n");
}

void ble_evt_system_endpoint_watermark_rx(const struct ble_msg_system_endpoint_watermark_rx_evt_t *msg)
{dprintf("ble_evt_system_endpoint_watermark_rx\n");
}

void ble_evt_system_endpoint_watermark_tx(const struct ble_msg_system_endpoint_watermark_tx_evt_t *msg)
{dprintf("ble_evt_system_endpoint_watermark_tx\n");
}

void ble_rsp_system_endpoint_set_watermarks(const struct ble_msg_system_endpoint_set_watermarks_rsp_t*msg)
{dprintf("ble_rsp_system_endpoint_set_watermarks\n");

}

void ble_rsp_hardware_usb_enable(const struct ble_msg_hardware_usb_enable_rsp_t *msg)
{dprintf("ble_rsp_hardware_usb_enable\n");
}

void ble_rsp_system_endpoint_rx(const struct ble_msg_system_endpoint_rx_rsp_t*msg)
{dprintf("ble_rsp_system_endpoint_rx\n");

}

void ble_evt_system_script_failure(const struct ble_msg_system_script_failure_evt_t *msg)
{dprintf("ble_evt_system_script_failure\n");
}

void ble_evt_flash_ps_key(const struct ble_msg_flash_ps_key_evt_t *msg)
{dprintf("ble_evt_flash_ps_key\n");
}

//void ble_evt_connection_status(const struct ble_msg_connection_status_evt_t *msg)
//{dprintf("NC\n");
//    if(msg->flags&connection_connected)
//    {dprintf("NC\n");
//        dprintf("#connected -> disconnect\n");
//        ble_cmd_connection_disconnect(msg->connection);
//    }else
//    {dprintf("NC\n");
//        dprintf("#Not connected -> Scan\n");
//        ble_cmd_gap_discover(1);
//    }
//}

void ble_evt_connection_version_ind(const struct ble_msg_connection_version_ind_evt_t *msg)
{dprintf("ble_evt_connection_version_ind\n");
}

void ble_evt_connection_feature_ind(const struct ble_msg_connection_feature_ind_evt_t *msg)
{dprintf("ble_evt_connection_feature_ind\n");
}

void ble_evt_connection_raw_rx(const struct ble_msg_connection_raw_rx_evt_t *msg)
{dprintf("ble_evt_connection_raw_rx\n");
}

void ble_evt_connection_disconnected(const struct ble_msg_connection_disconnected_evt_t *msg)
{dprintf("ble_evt_connection_disconnected\n");
    ble_cmd_connection_get_status(0);
}

void ble_evt_attclient_indicated(const struct ble_msg_attclient_indicated_evt_t *msg)
{dprintf("ble_evt_attclient_indicated\n");
}

//void ble_evt_attclient_procedure_completed(const struct ble_msg_attclient_procedure_completed_evt_t *msg)
//{dprintf("ble_evt_attclient_procedure_completed\n");
//}

//void ble_evt_attclient_group_found(const struct ble_msg_attclient_group_found_evt_t *msg)
//{dprintf("ble_evt_attclient_group_found\n");
//}

void ble_evt_attclient_attribute_found(const struct ble_msg_attclient_attribute_found_evt_t *msg)
{dprintf("ble_evt_attclient_attribute_found\n");
}

//void ble_evt_attclient_find_information_found(const struct ble_msg_attclient_find_information_found_evt_t *msg)
//{dprintf("ble_evt_attclient_find_information_found\n");
//}

//void ble_evt_attclient_attribute_value(const struct ble_msg_attclient_attribute_value_evt_t *msg)
//{dprintf("ble_evt_attclient_attribute_value\n");
//}

void ble_evt_sm_smp_data(const struct ble_msg_sm_smp_data_evt_t *msg)
{dprintf("ble_evt_sm_smp_data\n");
}

//void ble_evt_gap_scan_response(const struct ble_msg_gap_scan_response_evt_t *msg)
//{dprintf("ble_evt_gap_scan_response\n");
//    int i;
//    for(i=0;i<6;i++)
//        dprintf("%02x%s",msg->sender.addr[5-i],i<5?":":"");
//    dprintf("\t%d\n",msg->rssi);
//}

void ble_evt_gap_mode_changed(const struct ble_msg_gap_mode_changed_evt_t *msg)
{dprintf("ble_evt_gap_mode_changed\n");
}

void ble_evt_hardware_io_port_status(const struct ble_msg_hardware_io_port_status_evt_t *msg)
{dprintf("ble_evt_hardware_io_port_status\n");
}

void ble_evt_hardware_soft_timer(const struct ble_msg_hardware_soft_timer_evt_t *msg)
{dprintf("ble_evt_hardware_soft_timer\n");
}

void ble_evt_hardware_adc_result(const struct ble_msg_hardware_adc_result_evt_t *msg)
{dprintf("ble_evt_hardware_adc_result\n");
}

void ble_evt_hardware_analog_comparator_status(const struct ble_msg_hardware_analog_comparator_status_evt_t *msg)
{dprintf("ble_evt_hardware_analog_comparator_status\n");
}

void ble_rsp_connection_slave_latency_disable(const struct ble_msg_connection_slave_latency_disable_rsp_t *msg)
{dprintf("ble_rsp_connection_slave_latency_disable\n");
}

void ble_rsp_gap_set_initiating_con_parameters(const struct ble_msg_gap_set_initiating_con_parameters_rsp_t *msg)
{dprintf("ble_rsp_gap_set_initiating_con_parameters\n");
}

/**Reset system**/
void ble_rsp_dfu_reset(const void *nul)
{dprintf("ble_rsp_dfu_reset\n");
}

/**set address for flashing**/
void ble_rsp_dfu_flash_set_address(const struct ble_msg_dfu_flash_set_address_rsp_t *msg)
{dprintf("ble_rsp_dfu_flash_set_address\n");}

/**Upload binary for flashing. Address will be updated automatically.**/
void ble_rsp_dfu_flash_upload(const struct ble_msg_dfu_flash_upload_rsp_t *msg)
{dprintf("ble_rsp_dfu_flash_upload\n");}
/**Uploading is finished.**/
void ble_rsp_dfu_flash_upload_finish(const struct ble_msg_dfu_flash_upload_finish_rsp_t *msg)
{dprintf("ble_rsp_dfu_flash_upload_finish\n");}
/**Device booted up in dfu, and is ready to receive commands**/
void ble_evt_dfu_boot(const struct ble_msg_dfu_boot_evt_t *msg)
{dprintf("ble_evt_dfu_boot\n");}
