#ifndef BLUESTACK_H_
#define BLUESTACK_H_

#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "app_error.h"
#include "ble.h"
#include "ble_hci.h"
#include "ble_srv_common.h"
#include "ble_advdata.h"
#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#include "bluestack_config.h"
#include "bluestack_events.h"

typedef struct {
	char	*			dev_name;							///< The device name
	ble_uuid_t 			adv_uuid;							///< Adveritising UUID
	uint32_t 			adv_interval;						///< The advertising interval in orden on 0.625ms
	uint32_t 			adv_timeout;						///< The advertising timeout in seconds
	BS_BLE_EVT			usr_ble_evt;						///< Manejador de eventos de usuario
	CONN_PARAMS_ERR_H	usr_conn_params_err_h;				///< Manejador de errores de parametros de conexión
	CONN_PARAMS_EVT		usr_conn_params_evt;				///< Manejador de eventos de parametros de conexión
}bs_config_t;

void bs_init(bs_config_t * bs_cfg);

void bs_advertising_start(void);

void bs_power_manage(void);

#endif