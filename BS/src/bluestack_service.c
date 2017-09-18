#include "bluestack_service.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"

BlueStack_Service_t _services[MAX_SERVICES];
uint8_t _services_count = 0;


void BlueStack_OnServiceBleEvent(BlueStack_Service_t* service, ble_evt_t const * p_ble_evt) {
    
    ble_gatts_evt_write_t const * p_evt_write = &(p_ble_evt->evt.gatts_evt.params.write);
    uint8_t i;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            //on_connect(service, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            //on_disconnect(service, p_ble_evt);
            service->conn_handle = BLE_CONN_HANDLE_INVALID;
            break;
            
        case BLE_GATTS_EVT_WRITE:
            //on_write(service, p_ble_evt);

            for (i = 0; i < service->chars_count; i++)
            {
                if(service->chars[i].handle.value_handle == p_evt_write->handle)
                {
                    if(service->ble_write_handler != NULL)
                        service->ble_write_handler(&service->chars[i], (uint8_t *)p_evt_write->data, p_evt_write->len);

                    break;
                }
            }
            
            break;
            
        default:
            // No implementation needed.
            break;
    }
}

uint32_t BlueStack_ServiceAdd(BlueStack_Service_t* service) {
    
    uint32_t err_code;
    uint8_t i;
    ble_uuid_t ble_uuid;
    service->conn_handle = BLE_CONN_HANDLE_INVALID;

    if(_services_count >= MAX_SERVICES)
        return -1;

    _services[_services_count].service_handle    = service->service_handle;
    _services[_services_count].conn_handle       = service->conn_handle;
    _services[_services_count].service_uuid_type = service->service_uuid_type;
    _services[_services_count].service_uuid_base = service->service_uuid_base;
    _services[_services_count].service_uuid      = service->service_uuid;
    _services[_services_count].ble_write_handler = service->ble_write_handler;
    _services[_services_count].chars             = service->chars;
    _services[_services_count].chars_count       = service->chars_count;

    err_code = sd_ble_uuid_vs_add(&_services[_services_count].service_uuid_base, &_services[_services_count].service_uuid_type);
    if(err_code != NRF_SUCCESS) return -1;

    ble_uuid.type = _services[_services_count].service_uuid_type;
    ble_uuid.uuid = _services[_services_count].service_uuid;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &_services[_services_count].service_handle);
    if(err_code != NRF_SUCCESS) return -1;


    for (i = 0; i <  _services[_services_count].chars_count; i++)
    {
            BlueStack_CharAdd(&_services[_services_count].chars[i], _services_count);
    }
    _services_count++;
    return _services_count -1;

}


uint32_t BlueStack_CharAdd(BlueStack_Characteristic_t* c, uint8_t service_id) {
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;
    ble_gatts_attr_md_t cccd_md;

    memset(&char_md, 0, sizeof(char_md));

    if(c->type & BLUESTACK_CHAR_TYPE_NTFY)
    {
        memset(&cccd_md, 0, sizeof(cccd_md));
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
        BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
        cccd_md.vloc = BLE_GATTS_VLOC_STACK;
        char_md.p_cccd_md         = &cccd_md;
    }
    else
        char_md.p_cccd_md         = NULL;

    
    char_md.char_props.read   = (c->type & BLUESTACK_CHAR_TYPE_RD) ? 1:0;
    char_md.char_props.write  = (c->type & BLUESTACK_CHAR_TYPE_WR) ? 1:0;
    char_md.char_props.notify  = (c->type & BLUESTACK_CHAR_TYPE_NTFY) ? 1:0;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = _services[service_id].service_uuid_type;
    ble_uuid.uuid = c->uuid;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = c->data_size;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = c->data_size;
    attr_char_value.p_value      = NULL;
    
    return sd_ble_gatts_characteristic_add(_services[service_id].service_handle, &char_md, &attr_char_value, &c->handle);


}

uint32_t BlueStack_ServiceRead(uint8_t service_id, BlueStack_Characteristic_t* p_char, uint8_t* data, uint8_t data_len) {
    ble_gatts_hvx_params_t params;
    uint16_t len = data_len;
    
    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_char->handle.value_handle;
    params.p_data = data;
    params.p_len  = &len;
    
    if(service_id < _services_count)
        return sd_ble_gatts_hvx(_services[service_id].conn_handle, &params);

    return -1;

}


// Necesita cambios
uint32_t BlueStack_ServiceNotify(uint8_t service_id, BlueStack_Characteristic_t* p_char, uint8_t* data, uint8_t data_len) {
    ble_gatts_hvx_params_t params;
    uint16_t len = data_len;
    
    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_char->handle.value_handle;
    params.p_data = data;
    params.p_len  = &len;
    
    if(service_id < _services_count)
        return sd_ble_gatts_hvx(_services[service_id].conn_handle, &params);

    return -1;

}



