#include "feeder_wifi_connector.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_wifi.h"
#include "esp_log.h"


//////////////// WIFI METHODS //////////////////////////

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

/* esp netif object representing the WIFI station */
static esp_netif_t *sta_netif = NULL;

/* The event group allows multiple bits for each event,
   but we only care about one event - are we connected
   to the AP with an IP? */
static const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "WIFI_CONNECTOR";

static void sta_event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Connected bit unset");
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "Connected bit set");
        esp_netif_ip_info_t ip;
        memset(&ip, 0, sizeof(esp_netif_ip_info_t));
        ESP_LOGI(TAG, "IP:"IPSTR, IP2STR(&ip.ip));
    }
}

static void start_sta(void)
{
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = CONFIG_WIFI_SSID,
            .password = CONFIG_WIFI_PASSWORD
        },
    };
    ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK( esp_wifi_start() );
}

void initialise_wifi_connector(void)
{
    wifi_event_group = xEventGroupCreate();

    sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );

    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &sta_event_handler, NULL) );
    ESP_ERROR_CHECK( esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &sta_event_handler, NULL) );

    start_sta();
}


static void terminate_sta(void)
{
    ESP_ERROR_CHECK( esp_wifi_disconnect() );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_NULL) );
}


// //////////////// ENTRYPOINT //////////////////////////

static void wait_for_flag(EventGroupHandle_t *event_group, const int flags, BaseType_t x_clear_on_exit)
{
    //const TickType_t x_ticks_to_wait = 10 * 1000 / portTICK_PERIOD_MS;
    xEventGroupWaitBits(*event_group, flags, x_clear_on_exit, pdTRUE, portMAX_DELAY);
}

static void wait_for_connection(EventGroupHandle_t *event_group)
{
    ESP_LOGI(TAG, "Waiting for connection...");
    wait_for_flag(event_group, WIFI_CONNECTED_BIT, pdFALSE);
    ESP_LOGI(TAG, "Connected!");
}

void wait_for_wifi_connection(void)
{
    wait_for_connection(&wifi_event_group);
}


// void main_task(void *pvParameters)
// {
//     esp_netif_ip_info_t ip;
//     memset(&ip, 0, sizeof(esp_netif_ip_info_t));
//     ESP_LOGI(TAG, "Holaaaaa");
//     while (1) {
//             vTaskDelay(10 * 1000 / portTICK_PERIOD_MS);
//             ESP_LOGI(TAG, "sungaratunga");
//     }
// }