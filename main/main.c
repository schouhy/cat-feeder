#include "nvs_flash.h"

#include "feeder_wifi_connector.h"
#include "feeder_event_source.h"
#include "feeder_servo_driver.h"
#include "feeder_state.h"
#include "feeder_webserver.h"
#include "feeder_timer.h"

ESP_EVENT_DEFINE_BASE(LID_EVENTS);

ESP_EVENT_DEFINE_BASE(TIMER_EVENTS);

ESP_EVENT_DEFINE_BASE(WEBSERVER_EVENTS);

/// MAIN TASK /////////

void app_main(void)
{
    // Initialise ESP
    ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK( esp_netif_init() );
    ESP_ERROR_CHECK( esp_event_loop_create_default() );

    // Register the lid event handlers
    ESP_ERROR_CHECK( esp_event_handler_instance_register(LID_EVENTS, LID_EVENT_OPEN, lid_handler_open, NULL, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_instance_register(LID_EVENTS, LID_EVENT_CLOSE, lid_handler_close, NULL, NULL) );

    // Register timer events
    ESP_ERROR_CHECK( esp_event_handler_instance_register(WEBSERVER_EVENTS, WEBSERVER_EVENT_TIME_CONFIGURED, set_timer, NULL, NULL) );

    // Initialize Servo
    init_servo();

    // Initialise Wifi
    initialise_wifi_connector();
    wait_for_wifi_connection();

    // Sync datetime with SNTP
    sync_buenos_aires_sntp_datetime();

    // webserver
    static httpd_handle_t server = NULL;
    server = start_webserver();
    system_next_state();



    // xTaskCreate(&main_task, "main_task", 4096, NULL, 5, NULL);
}
