#ifndef _BLUESTACK_SERVICE_H_
#define _BLUESTACK_SERVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "bluestack_events.h"


#define CHECK_ERROR(ERR_CODE)  if (ERR_CODE != NRF_SUCCESS) return ERR_CODE

#define MAX_SERVICES      16
#define MAX_SERVICE_CHARS 16

#define BLUESTACK_CHAR_TYPE_RD      0x01
#define BLUESTACK_CHAR_TYPE_WR      0x02
#define BLUESTACK_CHAR_TYPE_NTFY    0x04


/**
 * Macro para llamar al manejador de eventos BLE con
 * todos los servicios definidos a travez de la función
 * BlueStack_onServiceBleEvent
 */
#define BLUESTACK_SERVICE_DISPATCHER_LOOP for(uint8_t i = 0; i < _services_count; ++i) BlueStack_OnServiceBleEvent(&_services[i], p_ble_evt)
    

typedef struct bluestack_srv_s BlueStack_Service_t;
typedef struct bluestack_chr_s BlueStack_Characteristic_t;

/**
 * Definición de función para manejar eventos de escritura sobre características.
 * Se asigna una función de este tipo a cada servicio para manejar los enventos de
 * escritura de stack BLE.
 */
typedef void (*BlueStack_Service_Write_Handler_t) (BlueStack_Characteristic_t * p_char, uint8_t* data, uint8_t data_len);


/**
 * @struct BlueStack_Characteristic_t
 * @brief Estructura para guardar la información de una
          característica BLE de un servicio
 */
typedef struct bluestack_chr_s{
    ble_gatts_char_handles_t    handle;     //!< Manejador de la característica
    uint8_t                     type;       ///< Tipo de la característica
    uint16_t                    uuid;       ///< UUID de la característica
    uint8_t                     data_size;  ///< Tamaño del paquete de datos de la característica
}BlueStack_Characteristic_t;


/**
 * @struct BlueStack_Characteristic_t
 * @brief Estructura para guardar un servicio BLE.
 */
typedef struct bluestack_srv_s{
    uint16_t                            service_handle;     ///< Manejador del servicio.
    uint16_t                            conn_handle;        ///< Manejador de la conexión BLE.
    uint8_t                             service_uuid_type;  ///< Tipo del UUID del servicio
    ble_uuid128_t                       service_uuid_base;  ///< UUID base del servicio.
    uint16_t                            service_uuid;       ///< UUID del servicio.
    BlueStack_Service_Write_Handler_t   ble_write_handler;  ///< Función manejadora para eventos de escritura del servicio.
    BlueStack_Characteristic_t          *chars;             ///< Apuntador a un arreglo de caracerísticas.
    uint8_t                             chars_count;        ///< Número de características del arreglo de características.
}BlueStack_Service_t;


/**
 * @brief      Función para manejar los eventos del stack bluetooth en los
 *             servicios.
 *             
 * Esta función se debe llamar en el despachador de eventos del BLE con cada
 * uno de los servicios inicializados como parámetros. 
 *
 * @param      service    Servicio que recibe el evento
 * @param      p_ble_evt  Evento BLE que manejará el servicio.
 */
void BlueStack_OnServiceBleEvent(BlueStack_Service_t* service, ble_evt_t const * p_ble_evt);


/**
 * @brief      Registra un servicio al administrador de servicios BlueStack
 *
 * @param      service  El servcio a registrar
 *
 * @return     Devuelve el identificador del servicio utilizado por BlueStack, si hubo error
 *             devuelve -1.
 */
uint32_t BlueStack_ServiceAdd(BlueStack_Service_t* service);


/**
 * @brief      Agrega un característica a un servicio registrado previamente
 *             por la función BlueStack_ServiceAdd
 *
 * @param      c           Característica a agregar.
 * @param[in]  service_id  Identificador del servico al que se agrega la característica.
 *
 * @return     Devuelve NRF_SUCCESS si no hubo errores, de lo contrario devuelve el error.
 */
uint32_t BlueStack_CharAdd(BlueStack_Characteristic_t* c, uint8_t service_id);
//uint32_t BlueStack_WriteCharAdd(BlueStack_Characteristic_t* c, uint8_t service_id);
//uint32_t BlueStack_ReadCharAdd(BlueStack_Characteristic_t* c, uint8_t service_id);

/**
 * @brief      Función para enviar datos a las características BLE de tipo Lectura.
 * 
 * Envía datos al dispotivo Central por medio de las características de lectura, 
 * es decir, cuando un dispostivo central hace una petición de lectura.
 *
 * @param[in]  service_id  ID del servicio al que se van a enviar los datos.
 * @param      p_char      Apuntador a la característica a la que se envían los datos para leer.
 * @param      data        Arreglo de datos a enviar.
 * @param[in]  data_len    Tamaño del arreglo de datos a enviar.
 *
 * @return     Si hay error devuelve -1.
 */
uint32_t BlueStack_ServiceRead(uint8_t service_id, BlueStack_Characteristic_t* p_char, uint8_t* data, uint8_t data_len);

/**
 * @brief      Función para enviar datos a las características BLE de tipo Notificación.
 *
 * Envía datos al dispotivo Central por medio de las características de Notificación, 
 * es decir, cuando un dispostivo central hace una petición de lectura.
 * 
 * @param[in]  service_id  ID del servicio al que se van a enviar los datos.
 * @param      p_char      Apuntador a la característica a la que se envían los datos de Notificación.
 * @param      data        Arreglo de datos a enviar.
 * @param[in]  data_len    Tamaño del arreglo de datos a enviar.
 *
 * @return     Si hay error devuelve -1.
 */
uint32_t BlueStack_ServiceNotify(uint8_t service_id, BlueStack_Characteristic_t* p_char, uint8_t* data, uint8_t data_len);

#endif // _BLUESTACK_SERVICE_H_


