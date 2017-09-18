
#include "app_timer.h"

#include "nrf52_cn.h"
#include "nrf_gpio.h"
#include "bluestack.h"
#include "bluestack_config.h"
#include "bluestack_events.h"
#include "nrf_delay.h"

#include "bluestack_service.h"

#define ADVERTISING_LED         LED1
#define LEDBUTTON_LED           LED2
#define LEDBUTTON_BUTTON        BUTTON1


//////////////////////////////////////////////////////////////////////////////////////
// UUID Base del servicio.
#define SRV_UUID_BASE          {0x23, 0xD4, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}

// UUID del servicio
#define SRV_UUID_SERVICE       0xAA01

// UUIDE de las características que incluye el servicio.
#define SRV_UUID_WRITE_CHAR    0xAA02
#define SRV_UUID_READ_CHAR     0xAA03


BlueStack_Characteristic_t chars[2];

uint32_t my_service_id;

APP_TIMER_DEF(advertising_led_timer_id);
#define ADVERTISING_LED_INTERVAL APP_TIMER_TICKS(1500)

/**@brief Function for the LEDs initialization.
 *
 * @details Initializes all LEDs used by the application.
 */
static void leds_init(void)
{
    nrf_gpio_cfg_output(LED1);
    nrf_gpio_cfg_output(LED2);
    nrf_gpio_pin_clear(LED1);
    nrf_gpio_pin_clear(LED2);
}


static void AdvertisingLedUpdate(){
    nrf_gpio_pin_set(ADVERTISING_LED);
    nrf_delay_ms(80);
    nrf_gpio_pin_clear(ADVERTISING_LED);
}


/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module.
 */
static void timers_init(void)
{
    // Initialize timer module, making it use the scheduler
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);

    err_code = app_timer_create(&advertising_led_timer_id, APP_TIMER_MODE_REPEATED, AdvertisingLedUpdate);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling write events to the LED characteristic.
 *
 * @param[in] p_lbs     Instance of LED Button Service to which the write applies.
 * @param[in] led_state Written/desired state of the LED.
 */
//static void led_write_handler(uint16_t conn_handle, ble_lbs_t * p_lbs, uint8_t led_state)
static void led_write_handler(BlueStack_Characteristic_t * p_char, uint8_t* data, uint8_t data_len)
{
    if(p_char->uuid == SRV_UUID_WRITE_CHAR && data_len == 1){
        if(data[0])
            nrf_gpio_pin_set(LEDBUTTON_LED);
        else
            nrf_gpio_pin_clear(LEDBUTTON_LED);

    }
    BlueStack_ServiceRead(my_service_id, &chars[0], data, 1);
}

void services_init(){

    BlueStack_Service_t my_service;

    ble_uuid128_t uuid_base = {SRV_UUID_BASE};

    memset(&chars[0], 0, sizeof(BlueStack_Characteristic_t));
    chars[0].uuid = SRV_UUID_READ_CHAR;
    chars[0].type = BLUESTACK_CHAR_TYPE_NTFY;
    chars[0].data_size = 1;

    memset(&chars[1], 0, sizeof(BlueStack_Characteristic_t));
    chars[1].uuid = SRV_UUID_WRITE_CHAR;
    chars[1].type = BLUESTACK_CHAR_TYPE_WR;
    chars[1].data_size = 1;



    memset(&my_service, 0, sizeof(my_service));
    my_service.service_uuid_base = uuid_base;
    my_service.service_uuid      = SRV_UUID_SERVICE;
    my_service.chars             = chars;
    my_service.chars_count       = 2;
    my_service.ble_write_handler = led_write_handler;

    my_service_id = BlueStack_ServiceAdd(&my_service);
}


void usr_on_ble_event(ble_evt_t const * p_ble_evt, void * p_context)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            app_timer_stop(advertising_led_timer_id);
            for(int i= 0; i < 10; i++)
            {
                nrf_gpio_pin_set(ADVERTISING_LED);
                nrf_delay_ms(40);
                nrf_gpio_pin_clear(ADVERTISING_LED);
                nrf_delay_ms(40);
            }
        break;

        case BLE_GAP_EVT_DISCONNECTED:
            bs_advertising_start();
            app_timer_start(advertising_led_timer_id, ADVERTISING_LED_INTERVAL, NULL);
        break;
    }
}


/**@brief Function for application main entry.
 */
int main(void)
{
    bs_config_t bs_cfg;
    
    // Initialize.
    leds_init();
    timers_init();
    
    bs_cfg.dev_name               = "nRF52_BS_Template";
    bs_cfg.adv_uuid.uuid          = 0;                                        // UUID a mostar en el Advertising
    bs_cfg.adv_uuid.type          = BLE_UUID_TYPE_BLE;                        // Tipo del dispositivo
    bs_cfg.adv_interval           = 100;                                      // Intervalo del Advertising en unidades de 0.625 ms
    bs_cfg.adv_timeout            = 180;                                      // Tiempo de espera del Advertising. (tiempo de visibilidad)
    bs_cfg.usr_ble_evt           = usr_on_ble_event;
    bs_cfg.usr_conn_params_err_h = NULL;
    bs_cfg.usr_conn_params_evt   = NULL;
    bs_init(&bs_cfg);
    services_init();
    bs_advertising_start();
    app_timer_start(advertising_led_timer_id, ADVERTISING_LED_INTERVAL, NULL);
    for (;;) { bs_power_manage(); }
}


/**
 * @}
 */
