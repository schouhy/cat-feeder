#include <string.h>

#include "esp_sntp.h"
#include "esp_log.h"

#include "feeder_event_source.h"
#include "feeder_state.h"
#include "feeder_timer.h"

#define TIMER_PERIOD 1000000  // period of the timer event source in microseconds

static const char *TAG = "TIMER";

static esp_timer_handle_t TIMER;
static time_t *lid_open_time_ptr = NULL;


void set_lid_open_time_t(time_t open_time_t)
{
    lid_open_time_ptr = (time_t *)malloc(sizeof(time_t));
    *lid_open_time_ptr = open_time_t;
}

size_t get_lid_open_time_str(char * buffer)
{
    if(lid_open_time_ptr == NULL)
        return 0;

    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    localtime_r(lid_open_time_ptr, &tm);
    strftime(buffer, 17, "%Y-%m-%dT%H:%M", &tm);
    return 17;
}

esp_err_t get_lid_open_time_t(time_t *time)
{
    if(lid_open_time_ptr == NULL)
        return ESP_FAIL;

    *time = *lid_open_time_ptr;
    return ESP_OK;
}
static void timer_callback(void* arg)
{
    ESP_LOGI(TAG, "timer callback");
    ESP_ERROR_CHECK(esp_event_post(TIMER_EVENTS, TIMER_EVENT_EXPIRY, NULL, 0, portMAX_DELAY));
}

static void timer_expiry_handler(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{
    SystemState current_system_state = get_current_state();

    time_t open_lid_time_t;
    ESP_ERROR_CHECK( get_lid_open_time_t(&open_lid_time_t) );

    time_t now;
    time(&now);
    if( (current_system_state == SCHEDULED) && (open_lid_time_t <= now)) {
        system_next_state();
        ESP_ERROR_CHECK( esp_event_post(LID_EVENTS, LID_EVENT_OPEN, NULL, 0, portMAX_DELAY) );
    }
    if( (current_system_state == LID_OPEN) && (open_lid_time_t + (60 * LID_OPENING_TIME_IN_MINUTES) <= now)) {
        system_next_state();
        ESP_ERROR_CHECK( esp_event_post(LID_EVENTS, LID_EVENT_CLOSE, NULL, 0, portMAX_DELAY) );
        ESP_ERROR_CHECK( esp_timer_stop(TIMER) );

        free(lid_open_time_ptr);
        lid_open_time_ptr = NULL;
    }
}

void set_timer(void* handler_args, esp_event_base_t base, int32_t id, void* event_data)
{

    ESP_ERROR_CHECK(esp_event_handler_instance_register(TIMER_EVENTS, TIMER_EVENT_EXPIRY, timer_expiry_handler, NULL, NULL));

    // Create and start the event sources
    esp_timer_create_args_t timer_args = {
        .callback = &timer_callback,
    };

    ESP_ERROR_CHECK(esp_timer_create(&timer_args, &TIMER));
    ESP_LOGI(TAG, "starting timer");
    ESP_ERROR_CHECK(esp_timer_start_periodic(TIMER, TIMER_PERIOD));
    system_next_state();
}


void sync_buenos_aires_sntp_datetime(void){
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_init();

    // Set timezone to Buenos Aires time
    setenv("TZ", "UTC+3", 1);
    tzset();

    int retry = 0;
    const int retry_count = 10;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}