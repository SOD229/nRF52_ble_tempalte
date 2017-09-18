#ifndef BLUESTACK_EVENTS_H_
#define BLUESTACK_EVENTS_H_

#include "ble.h"
#include "ble_conn_params.h"
#include "bluestack_config.h"
#include "ble_hci.h"

typedef void (* BS_BLE_EVT) (ble_evt_t const * p_ble_evt, void * p_context);

typedef void (* CONN_PARAMS_ERR_H) (uint32_t nrf_error);

typedef void (* CONN_PARAMS_EVT)(ble_conn_params_evt_t * p_evt);

void on_ble_event(ble_evt_t const * p_ble_evt, void * p_context);

void conn_params_error_handler(uint32_t nrf_error);

void on_conn_params_evt(ble_conn_params_evt_t * p_evt);

#endif